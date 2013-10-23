#define __MPKLIB_SELF__
#include "MpkLib.h"
#include "SCommon.h"

//-----------------------------------------------------------------------------
// Defines

#define LISTFILE_CACHE_SIZE 0x1000

//-----------------------------------------------------------------------------
// Local functions

static BOOL IsValidSearchHandle(TMPKSearch * hs)
{
    if(hs == NULL || IsBadReadPtr(hs, sizeof(TMPKSearch)))
        return FALSE;

    if(!IsValidMpkHandle(hs->ha))
        return FALSE;

    return TRUE;
}

// This function compares a string with a wildcard search string.
// returns TRUE, when the string matches with the wildcard.
BOOL CheckWildCard(const char * szString, const char * szWildCard)
{
    char * szTemp;                      // Temporary helper pointer
    int nResult = 0;                    // For memcmp return values
    int nMustNotMatch = 0;              // Number of following chars int szString,
                                        // which must not match with szWildCard
    int nMustMatch = 0;                 // Number of the following characters,
                                        // which must match

    // When the string is empty, it does not match with every wildcard
    if(*szString == 0)
        return FALSE;

    // When the mask is empty, it matches to every wildcard
    if(szWildCard == NULL || *szWildCard == 0)
        return FALSE;

    // Do normal test
    for(;;)
    {
        switch(*szWildCard)
        {
            case '*': // Means "every number of characters"
                // Skip all asterisks
                while(*szWildCard == '*')
                    szWildCard++;

                // When no more characters in wildcard, it means that the strings match
                if(*szWildCard == 0)
                    return TRUE;

                // The next N characters must not agree
                nMustNotMatch |= 0x70000000;
                break;
            
            case '?':  // Means "One or no character"
                while(*szWildCard == '?')
                {
                    nMustNotMatch++;
                    szWildCard++;
                }
                break;

            default:
                // If the two characters match
                if(toupper(*szString) == toupper(*szWildCard))
                {
                    // When end of string, they agree
                    if(*szString == 0)
                        return TRUE;

                    nMustNotMatch = 0;
                    szWildCard++;
                    szString++;
                    break;
                }

                // If the next character must match, the string does not match
                if(nMustNotMatch == 0)
                    return FALSE;

                // Count the characters which must match after characters
                // that must not match
                szTemp = (char *)szWildCard;
                nMustMatch = 0;
                while(*szTemp != 0 && *szTemp != '*' && *szTemp != '?')
                {
                    nMustMatch++;
                    szTemp++;
                }

                // Now skip characters from szString up to number of chars
                // that must not match
                nResult = -1;
                while(nMustNotMatch > 0 && *szString != 0)
                {
                    if((nResult = _strnicmp(szString, szWildCard, nMustMatch)) == 0)
                        break;
                    
                    szString++;
                    nMustNotMatch--;
                }

                // Make one more comparison
                if(nMustNotMatch == 0)
                    nResult = _strnicmp(szString, szWildCard, nMustMatch);

                // If a match has been found, continue the search
                if(nResult == 0)
                {
                    nMustNotMatch = 0;
                    szWildCard += nMustMatch;
                    szString   += nMustMatch;
                    break;
                }
                return FALSE;
        }
    }
}

// Performs one MPK search
static int DoMPKSearch(TMPKSearch * hs, SFILE_FIND_DATA * lpFindFileData)
{
    TMPKArchive * ha = hs->ha;
    TFileNode * pNode;
    TMPKHash * pHashEnd = ha->pHashTable + ha->pHeader->dwHashTableSize;
    TMPKHash * pHash = ha->pHashTable + hs->dwNextIndex;

    // Do until some file found or no more files
    while(pHash < pHashEnd)
    {
        pNode = ha->pListFile[hs->dwNextIndex++];

        // If this entry is free, do nothing
        if(pHash->dwBlockIndex < HASH_ENTRY_FREE && (DWORD)pNode < HASH_ENTRY_FREE)
        {
            // Check the file name.
            if(CheckWildCard(pNode->szFileName, hs->szSearchMask))
            {
                TMPKBlock * pBlock = ha->pBlockTable + pHash->dwBlockIndex;

                lpFindFileData->lcLocale     = pHash->lcLocale;
                lpFindFileData->dwFileSize   = pBlock->dwFSize;
                lpFindFileData->dwFileFlags  = pBlock->dwFlags;
                lpFindFileData->dwBlockIndex = pHash->dwBlockIndex;
                lpFindFileData->dwCompSize   = pBlock->dwCSize;

                // Fill the file name and plain file name
                strcpy(lpFindFileData->cFileName, pNode->szFileName);
                lpFindFileData->szPlainName = strrchr(lpFindFileData->cFileName, '\\');
                if(lpFindFileData->szPlainName == NULL)
                    lpFindFileData->szPlainName = lpFindFileData->cFileName;
                else
                    lpFindFileData->szPlainName++;

                // Fill the next entry
                return ERROR_SUCCESS;
            }
        }

        pHash++;
    }

    // No more files found, return error
    return ERROR_NO_MORE_FILES;
}

static void FreeMPKSearch(TMPKSearch *& hs)
{
    if(hs != NULL)
    {
        FREEMEM(hs);
        hs = NULL;
    }
}

//-----------------------------------------------------------------------------
// Public functions

HANDLE WINAPI SFileFindFirstFile(HANDLE hMPK, const char * szMask, SFILE_FIND_DATA * lpFindFileData, const char * szListFile)
{
    TMPKArchive * ha = (TMPKArchive *)hMPK;
    TMPKSearch * hs = NULL;             // Search object handle
    int nSize  = 0;
    int nError = ERROR_SUCCESS;

    // Check for the valid parameters
    if(nError == ERROR_SUCCESS)
    {
        if(!IsValidMpkHandle(ha))
            nError = ERROR_INVALID_PARAMETER;
       
        if(szMask == NULL || lpFindFileData == NULL)
            nError = ERROR_INVALID_PARAMETER;

        if(szListFile == NULL && !IsValidMpkHandle(ha))
            nError = ERROR_INVALID_PARAMETER;
    }

    // Include the listfile into the MPK's internal listfile
    // Note that if the listfile name is NULL, do nothing because the
    // internal listfile is always included.
    if(nError == ERROR_SUCCESS && szListFile != NULL)
        nError = SFileAddListFile((HANDLE)ha, szListFile);

    // Allocate the structure for MPK search
    if(nError == ERROR_SUCCESS)
    {
        nSize = sizeof(TMPKSearch) + strlen(szMask) + 1;
        if((hs = (TMPKSearch *)ALLOCMEM(char, nSize)) == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // Perform the first search
    if(nError == ERROR_SUCCESS)
    {
        memset(hs, 0, sizeof(TMPKSearch));
        hs->ha          = ha;
        hs->dwNextIndex = 0;
        strcpy(hs->szSearchMask, szMask);
        nError = DoMPKSearch(hs, lpFindFileData);
    }

    // Cleanup
    if(nError != ERROR_SUCCESS)
    {
        FreeMPKSearch(hs);
        SetLastError(nError);
    }
    
    // Return the result value
    return (HANDLE)hs;
}

BOOL WINAPI SFileFindNextFile(HANDLE hFind, SFILE_FIND_DATA * lpFindFileData)
{
    TMPKSearch * hs = (TMPKSearch *)hFind;
    int nError = ERROR_SUCCESS;

    // Check the parameters
    if(nError == ERROR_SUCCESS)
    {
        if(!IsValidSearchHandle(hs) || lpFindFileData == NULL)
            nError = ERROR_INVALID_PARAMETER;
    }

    if(nError == ERROR_SUCCESS)
        nError = DoMPKSearch(hs, lpFindFileData);

    if(nError != ERROR_SUCCESS)
    {
        SetLastError(nError);
        return FALSE;
    }
    return TRUE;
}

BOOL WINAPI SFileFindClose(HANDLE hFind)
{
    TMPKSearch * hs = (TMPKSearch *)hFind;

    // Check the parameters
    if(!IsValidSearchHandle(hs))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    FreeMPKSearch(hs);
    return TRUE;
}



BOOL   WINAPI SFileFindGetFileTime(HANDLE hFind,
							   /*__out_opt*/ FILETIME *lpCreationTime,
							   /*__out_opt*/ FILETIME *lpLastAccessTime,
							   /*__out_opt*/ FILETIME *lpLastWriteTime
							   )
{
	TMPKSearch * hs = (TMPKSearch *)hFind;
	TMPKArchive * ha = hs->ha;
	TFileNode * pNode;
	TMPKHash * pHashEnd = ha->pHashTable + ha->pHeader->dwHashTableSize;
	TMPKHash * pHash = ha->pHashTable + hs->dwNextIndex - 1;//-1 才是真正的找到的节点

	if(pHash >= pHashEnd)
	{
		return FALSE;
	}

	pNode = ha->pListFile[hs->dwNextIndex - 1];//-1 才是真正的找到的节点
	if((DWORD)pNode >= HASH_ENTRY_FREE)
	{
		return FALSE;
	}
	if (NULL != lpCreationTime)
	{
		*lpCreationTime = pNode->ftCreateFileTime;
	}

	if (NULL != lpLastAccessTime)
	{
		//do nothing now~~
		lpLastAccessTime = lpLastAccessTime;
	}

	if (NULL != lpLastWriteTime)
	{
		*lpLastWriteTime = pNode->ftLastWriteFileTime;
	}

	
	return TRUE;

}

BOOL   WINAPI SFileFindSetFileTime(HANDLE hFind,
							   /*__in_opt CONST*/ FILETIME *lpCreationTime,
							   /*__in_opt CONST*/ FILETIME *lpLastAccessTime,
							   /*__in_opt CONST*/ FILETIME *lpLastWriteTime
							   )
{
	TMPKSearch * hs = (TMPKSearch *)hFind;
	TMPKArchive * ha = hs->ha;
	TFileNode * pNode;
	TMPKHash * pHashEnd = ha->pHashTable + ha->pHeader->dwHashTableSize;
	TMPKHash * pHash = ha->pHashTable + hs->dwNextIndex - 1;//-1 才是真正的找到的节点

	if(pHash >= pHashEnd)
	{
		return FALSE;
	}

	pNode = ha->pListFile[hs->dwNextIndex - 1];//-1 才是真正的找到的节点
	if((DWORD)pNode >= HASH_ENTRY_FREE)
	{
		return FALSE;
	}

	if (NULL != lpCreationTime)
	{
		pNode->ftCreateFileTime = *lpCreationTime;
	}

	if (NULL != lpLastAccessTime)
	{
		//do nothing now~~
		lpLastAccessTime = lpLastAccessTime;
	}

	if (NULL != lpLastWriteTime)
	{
		pNode->ftLastWriteFileTime = *lpLastWriteTime;
	}

	return TRUE;
}