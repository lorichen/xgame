#define __MPKLIB_SELF__
#include "MpkLib.h"
#include "SCommon.h"
#include <assert.h>

//-----------------------------------------------------------------------------
// Listfile entry structure

#define LISTFILE_CACHE_SIZE 0x1000      // Size of one cache element
#define NO_MORE_CHARACTERS 256
#define HASH_TABLE_SIZE    31           // Initial hash table size (should be a prime number)

struct TListFileCache
{
    HANDLE  hFile;                      // Stormlib file handle
    char  * szMask;                     // File mask
    DWORD   dwFileSize;                 // Total size of the cached file
    DWORD   dwBuffSize;                 // File of the cache
    DWORD   dwFilePos;                  // Position of the cache in the file
    BYTE  * pBegin;                     // The begin of the listfile cache
    BYTE  * pPos;
    BYTE  * pEnd;                       // The last character in the file cache

    BYTE Buffer[1];                     // Listfile cache itself
};

//-----------------------------------------------------------------------------
// Local functions (cache)

// Reloads the cache. Returns number of characters
// that has been loaded into the cache.
static int ReloadCache(TListFileCache * pCache)
{
    // Check if there is enough characters in the cache
    // If not, we have to reload the next block
    if(pCache->pPos >= pCache->pEnd)
    {
        // If the cache is already at the end, do nothing more
        if((pCache->dwFilePos + pCache->dwBuffSize) >= pCache->dwFileSize)
            return 0;

        pCache->dwFilePos += pCache->dwBuffSize;
        SFileReadFile(pCache->hFile, pCache->Buffer, pCache->dwBuffSize, &pCache->dwBuffSize, NULL);
        if(pCache->dwBuffSize == 0)
            return 0;

        // Set the buffer pointers
        pCache->pBegin =
        pCache->pPos = &pCache->Buffer[0];
        pCache->pEnd = pCache->pBegin + pCache->dwBuffSize;
    }

    return pCache->dwBuffSize;
}

static int ReadLine(TListFileCache * pCache, char * szLine, int nMaxChars)
{
    int nChars = 0;                     // Number of chars loaded
    
    // Make space for the terminating zero character
    nMaxChars--;

__BeginLoading:

    // Skip newlines, spaces, tabs and another non-printable stuff
    while(pCache->pPos < pCache->pEnd && *pCache->pPos <= 0x20)
        pCache->pPos++;

    // Copy the remaining characters
    while(pCache->pPos < pCache->pEnd && nChars < nMaxChars)
    {
        // If we have found a newline, stop loading
        if(*pCache->pPos == 0x0D || *pCache->pPos == 0x0A)
        {
            *szLine = 0;
            return nChars;
        }

        *szLine++ = *pCache->pPos++;
        nChars++;
    }

    // If we now need to reload the cache, do it
    if(pCache->pPos == pCache->pEnd)
    {
        if(ReloadCache(pCache) > 0)
            goto __BeginLoading;
    }

    *szLine = 0;
    return nChars;
}

static int ReadBytes(TListFileCache * pCache, void * pBuf, int nByteCounts)
{
	int nChars = 0;                     // Number of chars loaded
    char *pBufPos = (char*)pBuf;

__BeginLoading:

	// Copy the remaining characters
	while(pCache->pPos < pCache->pEnd && nChars < nByteCounts)
	{
		*pBufPos++ = *pCache->pPos++;
		nChars++;
	}

	// If we now need to reload the cache, do it
	if(pCache->pPos == pCache->pEnd)
	{
		if(ReloadCache(pCache) > 0)
			goto __BeginLoading;
	}
	return nChars;
}

//-----------------------------------------------------------------------------
// Local functions (listfile nodes)

// This function creates the name for the listfile.
// the file will be created under unique name in the temporary directory
static void GetListFileName(TMPKArchive * /* ha */, char * szListFile)
{
    char szTemp[MAX_PATH];

    // Create temporary file name int TEMP directory
    GetTempPath(sizeof(szTemp)-1, szTemp);
    GetTempFileName(szTemp, LISTFILE_NAME, 0, szListFile);
}

// Creates the whole listfile. The listfile is an array of TListFileNode
// structures. The size of the array is the same like the hash table size,
// the ordering is the same too (listfile item index is the same like
// the index in the MPK hash table)

int SListFileCreateListFile(TMPKArchive * ha)
{
    DWORD dwItems = ha->pHeader->dwHashTableSize;

    // The listfile should be NULL now
    assert(ha->pListFile == NULL);

    ha->pListFile = ALLOCMEM(TFileNode *, dwItems);
    if(ha->pListFile == NULL)
        return ERROR_NOT_ENOUGH_MEMORY;

    memset(ha->pListFile, 0xFF, dwItems * sizeof(TFileNode *));
    return ERROR_SUCCESS;
}

// Adds a filename into the listfile. If the file name is already there,
// does nothing.
int SListFileAddNode(TMPKArchive * ha, const char * szFileName)
{
    TFileNode * pNode   = NULL;
    TMPKHash * pHashEnd = ha->pHashTable + ha->pHeader->dwHashTableSize;
    TMPKHash * pHash0   = GetHashEntry(ha, szFileName);
    TMPKHash * pHash    = pHash0;
    DWORD dwHashIndex = 0;
    DWORD dwLength;                     // File name length
    DWORD dwName1;
    DWORD dwName2;

    // If the file does not exist within the MPK, do nothing
    if(pHash == NULL)
        return ERROR_SUCCESS;

    // If the listfile entry already exists, do nothing
    dwHashIndex = pHash - ha->pHashTable;
    dwName1     = pHash->dwName1;
    dwName2     = pHash->dwName2;
    if((DWORD)ha->pListFile[dwHashIndex] <= HASH_ENTRY_DELETED)
        return ERROR_SUCCESS;
    
    // Create the listfile node and insert it into the listfile table
    dwLength = strlen(szFileName);
    pNode = (TFileNode *)ALLOCMEM(char, sizeof(TFileNode) + dwLength);
    pNode->dwRefCount = 0;
    pNode->dwLength   = dwLength;
    strcpy(pNode->szFileName, szFileName);

    // Fill the nodes for all language versions
    while(pHash->dwBlockIndex < HASH_ENTRY_DELETED)
    {
        if(pHash->dwName1 == dwName1 && pHash->dwName2 == dwName2)
        {
            pNode->dwRefCount++;
            ha->pListFile[pHash - ha->pHashTable] = pNode;
        }

        if(++pHash >= pHashEnd)
            pHash = ha->pHashTable;
        if(pHash == pHash0)
            break;
    }
    return ERROR_SUCCESS;
}

int  SListFileAddNode(TMPKArchive * ha, const char * szFileName, const FILETIME &ftCreateTime, const FILETIME &ftLastWriteTime)
{
	TFileNode * pNode   = NULL;
	TMPKHash * pHashEnd = ha->pHashTable + ha->pHeader->dwHashTableSize;
	TMPKHash * pHash0   = GetHashEntry(ha, szFileName);
	TMPKHash * pHash    = pHash0;
	DWORD dwHashIndex = 0;
	DWORD dwLength;                     // File name length
	DWORD dwName1;
	DWORD dwName2;

	// If the file does not exist within the MPK, do nothing
	if(pHash == NULL)
		return ERROR_SUCCESS;

	// If the listfile entry already exists, do nothing
	dwHashIndex = pHash - ha->pHashTable;
	dwName1     = pHash->dwName1;
	dwName2     = pHash->dwName2;
	if((DWORD)ha->pListFile[dwHashIndex] <= HASH_ENTRY_DELETED)
		return ERROR_SUCCESS;

	// Create the listfile node and insert it into the listfile table
	dwLength = strlen(szFileName);
	pNode = (TFileNode *)ALLOCMEM(char, sizeof(TFileNode) + dwLength);
	pNode->dwRefCount = 0;
	pNode->dwLength   = dwLength;
	strcpy(pNode->szFileName, szFileName);

	pNode->ftCreateFileTime = ftCreateTime;
	pNode->ftLastWriteFileTime = ftLastWriteTime;

	// Fill the nodes for all language versions
	while(pHash->dwBlockIndex < HASH_ENTRY_DELETED)
	{
		if(pHash->dwName1 == dwName1 && pHash->dwName2 == dwName2)
		{
			pNode->dwRefCount++;
			ha->pListFile[pHash - ha->pHashTable] = pNode;
		}

		if(++pHash >= pHashEnd)
			pHash = ha->pHashTable;
		if(pHash == pHash0)
			break;
	}
	return ERROR_SUCCESS;
}



// Removes a filename from the listfile.
// If the name is not there, does nothing
int SListFileRemoveNode(TMPKArchive * ha, const char * szFileName)
{
    TFileNode * pNode = NULL;
    TMPKHash * pHash = GetHashEntry(ha, szFileName);
    DWORD dwHashIndex = 0;

    if(pHash != NULL)
    {
        dwHashIndex = pHash - ha->pHashTable;
        pNode = ha->pListFile[dwHashIndex];
        ha->pListFile[dwHashIndex] = (TFileNode *)HASH_ENTRY_DELETED;

        // If the reference count has reached zero, do nothing
        if(--pNode->dwRefCount == 0)
            FREEMEM(pNode);
    }
    return ERROR_SUCCESS;
}


// Renames a node. We will not deal with the renaming, we'll simply
// remove the old node and insert the new one.
int SListFileRenameNode(TMPKArchive * ha, const char * szOldFileName, const char * szNewFileName)
{
    SListFileRemoveNode(ha, szOldFileName);
    return SListFileAddNode(ha, szNewFileName);
}


int SListFileFreeListFile(TMPKArchive * ha)
{
    if(ha->pListFile != NULL)
    {
        for(DWORD i = 0; i < ha->pHeader->dwHashTableSize; i++)
        {
            TFileNode * pNode = ha->pListFile[i];

            if((DWORD)pNode < HASH_ENTRY_FREE)
            {
                if(--pNode->dwRefCount == 0)
                {
                    FREEMEM(pNode);
                    ha->pListFile[i] = (TFileNode *)HASH_ENTRY_FREE;
                }
            }
        }

        FREEMEM(ha->pListFile);
        ha->pListFile = NULL;
    }

    return ERROR_SUCCESS;
}

// Saves the whole listfile into the MPK.
int SListFileSaveToMpk(TMPKArchive * ha)
{
    TFileNode * pNode = NULL;
    TMPKHash * pHashEnd = NULL;
    TMPKHash * pHash0 = NULL;
    TMPKHash * pHash = NULL;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    char   szListFile[MAX_PATH];
    char   szBuffer[sizeof(int) + MAX_PATH +sizeof(FILETIME) + sizeof (FILETIME)];
    DWORD dwTransferred;
    DWORD dwLength = 0;
    DWORD dwName1 = 0;
    DWORD dwName2 = 0;
    LCID lcSave = lcLocale;
    int  nError = ERROR_SUCCESS;

    // If no listfile, do nothing
    if(ha->pListFile == NULL)
        return ERROR_SUCCESS;

    // Create the local listfile
    if(nError == ERROR_SUCCESS)
    {
        GetListFileName(ha, szListFile);
        hFile = CreateFile(szListFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_DELETE_ON_CLOSE, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            nError = GetLastError();
    }

    // Find the hash entry corresponding to listfile
    pHashEnd = ha->pHashTable + ha->pHeader->dwHashTableSize;
    pHash0 = pHash = GetHashEntry(ha, 0);
    if(pHash == NULL)
        pHash0 = pHash = ha->pHashTable;

    // Save the file
    if(nError == ERROR_SUCCESS)
    {
        for(;;)
        {
            if(pHash->dwName1 != dwName1 && pHash->dwName2 != dwName2 && pHash->dwBlockIndex < HASH_ENTRY_DELETED)
            {
                dwName1 = pHash->dwName1;
                dwName2 = pHash->dwName2;
                pNode = ha->pListFile[pHash - ha->pHashTable];

                if((DWORD)pNode < HASH_ENTRY_DELETED)
                {
//                    memcpy(szBuffer, pNode->szFileName, pNode->dwLength);
//                    szBuffer[pNode->dwLength + 0] = 0x0D;
//                    szBuffer[pNode->dwLength + 1] = 0x0A;
//                    WriteFile(hFile, szBuffer, pNode->dwLength + 2, &dwTransferred, NULL);
					//crr mod
					//结构改为文件名长度+文件名 + 创建时间 + 修改时间
					char * pPos = szBuffer;
					memcpy(pPos, &pNode->dwLength, sizeof(pNode->dwLength));

					pPos += sizeof(pNode->dwLength);
					memcpy(pPos, pNode->szFileName, pNode->dwLength);

					pPos += pNode->dwLength;
					memcpy(pPos, &pNode->ftCreateFileTime, sizeof(pNode->ftCreateFileTime));
					pPos += sizeof(pNode->ftCreateFileTime);

					memcpy(pPos, &pNode->ftLastWriteFileTime, sizeof(pNode->ftLastWriteFileTime));
					pPos += sizeof(pNode->ftLastWriteFileTime);

					WriteFile(hFile, szBuffer, pPos - szBuffer, &dwTransferred, NULL);
                }
            }

            if(++pHash >= pHashEnd)
                pHash = ha->pHashTable;
            if(pHash == pHash0)
                break;
        }

        // Write the listfile name (if not already there)
        if(GetHashEntry(ha, LISTFILE_NAME) == NULL)
        {
//             dwLength = strlen(LISTFILE_NAME);
//             memcpy(szBuffer, LISTFILE_NAME, dwLength);
//             szBuffer[dwLength + 0] = 0x0D;
//             szBuffer[dwLength + 1] = 0x0A;
//             WriteFile(hFile, szBuffer, dwLength + 2, &dwTransferred, NULL);

			//crr mod
			//结构改为文件名长度+文件名 + 创建时间 + 修改时间
			
			dwLength = strlen(LISTFILE_NAME);
			char * pPos = szBuffer;
			memcpy(pPos, &dwLength, sizeof(dwLength));

			pPos += sizeof(dwLength);
			memcpy(pPos, LISTFILE_NAME, dwLength);

			pPos += dwLength;
			//Listfile的创建时间及修改时间无意义，故随便乱写一个.
			FILETIME  ftCreateFileTime = {0,0};
			FILETIME  ftLastWriteFileTime = {0,0};  
			memcpy(pPos, &ftCreateFileTime, sizeof(ftCreateFileTime));
			pPos += sizeof(ftCreateFileTime);

			memcpy(pPos, &ftLastWriteFileTime, sizeof(ftLastWriteFileTime));
			pPos += sizeof(ftLastWriteFileTime);

			WriteFile(hFile, szBuffer, pPos - szBuffer, &dwTransferred, NULL);
        }                     
        
        // Add the listfile into the archive.
        SFileSetLocale(LANG_NEUTRAL);
        nError = AddFileToArchive(ha, hFile, LISTFILE_NAME, MPK_FILE_COMPRESS_PKWARE | MPK_FILE_ENCRYPTED | MPK_FILE_REPLACEEXISTING, 0, SFILE_TYPE_DATA, NULL);
    }

    // Close the temporary file. This will delete it too.
    if(hFile != INVALID_HANDLE_VALUE)
        CloseHandle(hFile);

    lcLocale = lcSave;
    return nError;
}

//-----------------------------------------------------------------------------
// File functions

// Adds a listfile into the MPK archive.
// Note that the function does not remove the 
int WINAPI SFileAddListFile(HANDLE hMpk, const char * szListFile)
{
    TListFileCache * pCache = NULL;
    TMPKArchive * ha = (TMPKArchive *)hMpk;
    HANDLE hListFile = NULL;
    char  szFileName[MAX_PATH + 1];
    DWORD dwSearchScope = SFILE_OPEN_LOCAL_FILE;
    DWORD dwCacheSize = 0;
    DWORD dwFileSize = 0;
    int nLength = 0;
    int nError = ERROR_SUCCESS;

    // If the szListFile is NULL, it means we have to open internal listfile
    if(szListFile == NULL)
    {
        szListFile = LISTFILE_NAME;
        dwSearchScope = SFILE_OPEN_FROM_MPK;
    }

    // Open the local/internal listfile
    if(nError == ERROR_SUCCESS)
    {
        if(!SFileOpenFileEx((HANDLE)ha, szListFile, dwSearchScope, &hListFile))
            nError = GetLastError();
    }

    if(nError == ERROR_SUCCESS)
    {
        dwCacheSize = 
        dwFileSize = SFileGetFileSize(hListFile, NULL);

        // Try to allocate memory for the complete file. If it fails,
        // load the part of the file
        pCache = (TListFileCache *)ALLOCMEM(char, (sizeof(TListFileCache) + dwCacheSize));
        if(pCache == NULL)
        {
            dwCacheSize = LISTFILE_CACHE_SIZE;
            pCache = (TListFileCache *)ALLOCMEM(char, sizeof(TListFileCache) + dwCacheSize);
        }

        if(pCache == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    if(nError == ERROR_SUCCESS)
    {
        // Initialize the file cache
        memset(pCache, 0, sizeof(TListFileCache));
        pCache->hFile      = hListFile;
        pCache->dwFileSize = dwFileSize;
        pCache->dwBuffSize = dwCacheSize;
        pCache->dwFilePos  = 0;

        // Fill the cache
        SFileReadFile(hListFile, pCache->Buffer, pCache->dwBuffSize, &pCache->dwBuffSize, NULL);

        // Initialize the pointers
        pCache->pBegin =
        pCache->pPos = &pCache->Buffer[0];
        pCache->pEnd = pCache->pBegin + pCache->dwBuffSize;

//        // Load the node tree
//         while((nLength = ReadLine(pCache, szFileName, sizeof(szFileName) - 1)) > 0)
// 		{
//             SListFileAddNode(ha, szFileName);
// 		}

		int iFileNameLen = 0;
		FILETIME  ftCreateFileTime;  
		FILETIME  ftLastLastWriteFileTime;
		while (nLength = ReadBytes(pCache, &iFileNameLen, sizeof(iFileNameLen)) > 0)
		{
			nLength = ReadBytes(pCache, szFileName, iFileNameLen);
			szFileName[nLength] = '\0';
			ReadBytes(pCache, &ftCreateFileTime, sizeof(ftCreateFileTime));
			ReadBytes(pCache, &ftLastLastWriteFileTime, sizeof(ftLastLastWriteFileTime));
			SListFileAddNode(ha, szFileName, ftCreateFileTime, ftLastLastWriteFileTime);
		}
    }

    // Cleanup & exit
    if(pCache != NULL)
        SListFileFindClose((HANDLE)pCache);
    return nError;
}

//-----------------------------------------------------------------------------
// Passing through the listfile

HANDLE SListFileFindFirstFile(HANDLE hMpk, const char * szListFile, const char * szMask, SFILE_FIND_DATA * lpFindFileData)
{
    TListFileCache * pCache = NULL;
    TMPKArchive * ha = (TMPKArchive *)hMpk;
    HANDLE hListFile = NULL;
    DWORD dwSearchScope = SFILE_OPEN_LOCAL_FILE;
    DWORD dwCacheSize = 0;
    DWORD dwFileSize = 0;
    int nLength = 0;
    int nError = ERROR_SUCCESS;

    // Initialize the structure with zeros
    memset(lpFindFileData, 0, sizeof(SFILE_FIND_DATA));

    // If the szListFile is NULL, it means we have to open internal listfile
    if(szListFile == NULL)
    {
        szListFile = LISTFILE_NAME;
        dwSearchScope = SFILE_OPEN_FROM_MPK;
    }

    // Open the local/internal listfile
    if(nError == ERROR_SUCCESS)
    {
        if(!SFileOpenFileEx((HANDLE)ha, szListFile, dwSearchScope, &hListFile))
            nError = GetLastError();
    }

    if(nError == ERROR_SUCCESS)
    {
        dwCacheSize = 
        dwFileSize = SFileGetFileSize(hListFile, NULL);

        // Try to allocate memory for the complete file. If it fails,
        // load the part of the file
        pCache = (TListFileCache *)ALLOCMEM(char, sizeof(TListFileCache) + dwCacheSize);
        if(pCache == NULL)
        {
            dwCacheSize = LISTFILE_CACHE_SIZE;
            pCache = (TListFileCache *)ALLOCMEM(char, sizeof(TListFileCache) + dwCacheSize);
        }

        if(pCache == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    if(nError == ERROR_SUCCESS)
    {
        // Initialize the file cache
        memset(pCache, 0, sizeof(TListFileCache));
        pCache->hFile      = hListFile;
        pCache->dwFileSize = dwFileSize;
        pCache->dwBuffSize = dwCacheSize;
        pCache->dwFilePos  = 0;
        if(szMask != NULL)
        {
            pCache->szMask = ALLOCMEM(char, strlen(szMask) + 1);
            strcpy(pCache->szMask, szMask);
        }

        // Fill the cache
        SFileReadFile(hListFile, pCache->Buffer, pCache->dwBuffSize, &pCache->dwBuffSize, NULL);

        // Initialize the pointers
        pCache->pBegin =
        pCache->pPos = &pCache->Buffer[0];
        pCache->pEnd = pCache->pBegin + pCache->dwBuffSize;

        for(;;)
        {
//             // Read the (next) line
//             nLength = ReadLine(pCache, lpFindFileData->cFileName, sizeof(lpFindFileData->cFileName));
//             if(nLength == 0)
//             {
//                 nError = ERROR_NO_MORE_FILES;
//                 break;
//             }

				int iFileNameLen = 0;
				int nLength = ReadBytes(pCache, &iFileNameLen, sizeof(iFileNameLen));
				if (nLength <= 0)
				{
					nError = ERROR_NO_MORE_FILES;
					break;
				}

			     nLength = ReadBytes(pCache, lpFindFileData->cFileName, iFileNameLen);
				 lpFindFileData->cFileName[nLength] = '\0';

				FILETIME  ftCreateFileTime;  
				FILETIME  ftLastLastWriteFileTime;
				ReadBytes(pCache, &ftCreateFileTime, sizeof(ftCreateFileTime));
				ReadBytes(pCache, &ftLastLastWriteFileTime, sizeof(ftLastLastWriteFileTime));

            // If some mask entered, check it
            if(CheckWildCard(lpFindFileData->cFileName, pCache->szMask))
                break;                
        }
    }

    // Cleanup & exit
    if(nError != ERROR_SUCCESS)
    {
        memset(lpFindFileData, 0, sizeof(SFILE_FIND_DATA));
        SListFileFindClose((HANDLE)pCache);
        pCache = NULL;

        SetLastError(nError);
    }
    return (HANDLE)pCache;
}

BOOL SListFileFindNextFile(HANDLE hFind, SFILE_FIND_DATA * lpFindFileData)
{
    TListFileCache * pCache = (TListFileCache *)hFind;
    BOOL bResult = FALSE;
    int nError = ERROR_SUCCESS;
    int nLength;

    for(;;)
    {
        // Read the (next) line
//         nLength = ReadLine(pCache, lpFindFileData->cFileName, sizeof(lpFindFileData->cFileName));
//         if(nLength == 0)
//         {
//             nError = ERROR_NO_MORE_FILES;
//             break;
//         }

		int iFileNameLen = 0;
		nLength = ReadBytes(pCache, &iFileNameLen, sizeof(iFileNameLen));
		if (nLength <= 0)
		{
			nError = ERROR_NO_MORE_FILES;
			break;
		}

		nLength = ReadBytes(pCache, lpFindFileData->cFileName, iFileNameLen);
		lpFindFileData->cFileName[nLength] = '\0';

		FILETIME  ftCreateFileTime;  
		FILETIME  ftLastLastWriteFileTime;
		ReadBytes(pCache, &ftCreateFileTime, sizeof(ftCreateFileTime));
		ReadBytes(pCache, &ftLastLastWriteFileTime, sizeof(ftLastLastWriteFileTime));

        // If some mask entered, check it
        if(CheckWildCard(lpFindFileData->cFileName, pCache->szMask))
        {
            bResult = TRUE;
            break;
        }
    }

    if(nError != ERROR_SUCCESS)
        SetLastError(nError);
    return bResult;
}

BOOL SListFileFindClose(HANDLE hFind)
{
    TListFileCache * pCache = (TListFileCache *)hFind;

    if(pCache != NULL)
    {
        if(pCache->hFile != NULL)
            SFileCloseFile(pCache->hFile);
        if(pCache->szMask != NULL)
            FREEMEM(pCache->szMask);

        FREEMEM(pCache);
        return TRUE;
    }

    return FALSE;
}