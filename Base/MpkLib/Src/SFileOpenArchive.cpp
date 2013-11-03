#define __MPKLIB_SELF__
#include "MpkLib.h"
#include "SCommon.h"

/*****************************************************************************/
/* Local functions                                                           */
/*****************************************************************************/

static BOOL IsAviFile(TMPKHeader * pHeader)
{
    DWORD * AviHdr = (DWORD *)pHeader;

    // Test for 'RIFF', 'AVI ' or 'LIST'
    return (AviHdr[0] == 'FFIR' && AviHdr[2] == ' IVA' && AviHdr[3] == 'TSIL');
}

// This function gets the right positions of the hash table and the block table.
static int RelocateMpkTablePositions(TMPKArchive * ha)
{
    TMPKHeader * pHeader = ha->pHeader;
    DWORD dwFileSize = GetFileSize(ha->hFile, NULL);

    // MPKs must have table positions in the range (0; dwFileSize)
    if(pHeader->dwHashTablePos + ha->dwMpkPos < dwFileSize &&
       pHeader->dwBlockTablePos + ha->dwMpkPos < dwFileSize)
    {
        pHeader->dwHashTablePos  += ha->dwMpkPos;
        pHeader->dwBlockTablePos += ha->dwMpkPos;
        return ERROR_SUCCESS;
    }

    // Otherwise the archive has bad format    
    return ERROR_BAD_FORMAT;
}


/*****************************************************************************/
/* Public functions                                                          */
/*****************************************************************************/

//-----------------------------------------------------------------------------
// SFileGetLocale and SFileSetLocale
// Set the locale for all neewly opened archives and files

LCID WINAPI SFileGetLocale()
{
    return lcLocale;
}

LCID WINAPI SFileSetLocale(LCID lcNewLocale)
{
    lcLocale = lcNewLocale;
    return lcLocale;
}

//-----------------------------------------------------------------------------
// SFileOpenArchiveEx (not a public function !!!)
//
//   szFileName - MPK archive file name to open
//   dwPriority - When SFileOpenFileEx called, this contains the search priority for searched archives
//   dwFlags    - 
//   phMPK      - Pointer to store open archive handle

BOOL SFileOpenArchiveEx(const char * szMpkName, DWORD dwPriority, DWORD /* dwFlags */, HANDLE * phMPK, DWORD dwAccessMode)
{
    LARGE_INTEGER FilePos;
    TMPKArchive * ha = NULL;            // Archive handle
    HANDLE hFile = INVALID_HANDLE_VALUE;// Opened archive file handle
    DWORD dwMaxBlockIndex = 0;          // Maximum value of block entry
    DWORD dwBlockTableSize;             // Block table size.
    DWORD dwTransferred;                // Number of bytes read
    DWORD dwBytes = 0;                  // Number of bytes to read
    int nError = ERROR_SUCCESS;   

    // Check the right parameters
    if(nError == ERROR_SUCCESS)
    {
        if(szMpkName == NULL || *szMpkName == 0 || phMPK == NULL)
            nError = ERROR_INVALID_PARAMETER;
    }

    // Ensure that StormBuffer is allocated
    if(nError == ERROR_SUCCESS)
        nError = PrepareStormBuffer();

    // Open the MPK archive file
    if(nError == ERROR_SUCCESS)
    {
        hFile = CreateFile(szMpkName, dwAccessMode, /*FILE_SHARE_READ*/FILE_SHARE_READ | /*FILE_SHARE_WRITE*/GENERIC_WRITE, NULL, OPEN_EXISTING, 0, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            nError = GetLastError();
    }
    
    // Allocate the MPKhandle
    if(nError == ERROR_SUCCESS)
    {
        if((ha = ALLOCMEM(TMPKArchive, 1)) == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // Initialize handle structure and allocate structure for MPK header
    if(nError == ERROR_SUCCESS)
    {
        memset(ha, 0, sizeof(TMPKArchive));
        strncpy(ha->szFileName, szMpkName, strlen(szMpkName));
        ha->hFile      = hFile;
        ha->bFromCD    = FALSE;
        ha->dwPriority = dwPriority;
        ha->pHeader    = &ha->Header;
        ha->pListFile  = NULL;
        hFile = INVALID_HANDLE_VALUE;
    }

    // Find the offset of MPK header within the file
    if(nError == ERROR_SUCCESS)
    {
        for(;;)
        {
            ha->pHeader->dwID = 0;
            
            FilePos.QuadPart = ha->dwMpkPos;
            SetFilePointer(ha->hFile, FilePos.LowPart, &FilePos.HighPart, FILE_BEGIN);
            ReadFile(ha->hFile, ha->pHeader, sizeof(TMPKHeader), &dwTransferred, NULL);

            // Convert Header to LittleEndian
            CONVERTTMPKHEADERTOLITTLEENDIAN(ha->pHeader);

            // Special check : Some MPKs are actually AVI files, only with
            // changed extension.
            if(ha->dwMpkPos == 0 && IsAviFile(ha->pHeader))
            {
                nError = ERROR_AVI_FILE;
                break;
            }

            // If different number of bytes read, break the loop
            if(dwTransferred != sizeof(TMPKHeader))
            {
                nError = ERROR_BAD_FORMAT;
                break;
            }

            // W3M Map Protectors set some garbage value into the "dwDataOffs"
            // field of MPK header. This value is apparently ignored by Storm.dll
            if(ha->pHeader->dwDataOffs != sizeof(TMPKHeader))
            {
                ha->dwFlags |= MPK_FLAG_PROTECTED;
                ha->pHeader->dwDataOffs = sizeof(TMPKHeader);
            }

            // If valid signature has been found, break the loop
			if((ha->pHeader->dwID == ID_MPK || ha->pHeader->dwID == ID_MPQ) && 
               ha->pHeader->dwHashTablePos < ha->pHeader->dwArchiveSize &&
               ha->pHeader->dwBlockTablePos < ha->pHeader->dwArchiveSize)
                break;

            // Move to the next possible offset
            ha->dwMpkPos += 0x200;
        }
    }

    // Relocate tables position and check again
    if(nError == ERROR_SUCCESS)
    {
        ha->dwBlockSize = (0x200 << ha->pHeader->wBlockSize);
        nError = RelocateMpkTablePositions(ha);
    }

    // Allocate buffers
    if(nError == ERROR_SUCCESS)
    {
        //
        // Note that the block table should be as large as the hash table
        // (For later file additions).
        //
        // I have found a MPK which has the block table larger than
        // the hash table. We should avoid buffer overruns caused by that.
        //
        dwBlockTableSize = max(ha->pHeader->dwHashTableSize, ha->pHeader->dwBlockTableSize);

        ha->pHashTable    = ALLOCMEM(TMPKHash, ha->pHeader->dwHashTableSize);
        ha->pBlockTable   = ALLOCMEM(TMPKBlock, dwBlockTableSize);
        ha->pbBlockBuffer = ALLOCMEM(BYTE, ha->dwBlockSize);

        if(!ha->pHashTable || !ha->pBlockTable || !ha->pbBlockBuffer)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // Read the hash table into the buffer
    if(nError == ERROR_SUCCESS)
    {
        dwBytes = ha->pHeader->dwHashTableSize * sizeof(TMPKHash);

        FilePos.QuadPart = ha->pHeader->dwHashTablePos;
        SetFilePointer(ha->hFile, FilePos.LowPart, &FilePos.HighPart, FILE_BEGIN);
        ReadFile(ha->hFile, ha->pHashTable, dwBytes, &dwTransferred, NULL);

        // We have to convert every DWORD in ha->pHashTable from LittleEndian
        CONVERTBUFFERTOLITTLEENDIAN32BITS((DWORD *)ha->pHashTable, dwBytes / sizeof(DWORD));

        if(dwTransferred != dwBytes)
            nError = ERROR_FILE_CORRUPT;
    }

    // Decrypt hash table and check if it is correctly decrypted
    if(nError == ERROR_SUCCESS)
    {
        TMPKHash * pHashEnd = ha->pHashTable + ha->pHeader->dwHashTableSize;
        TMPKHash * pHash;

		//更换加密的key
        //DecryptHashTable((DWORD *)ha->pHashTable, (BYTE *)"(hash table)", (ha->pHeader->dwHashTableSize * 4));
		DecryptHashTable((DWORD *)ha->pHashTable, (BYTE *)"Ik46lh&^#$$j", (ha->pHeader->dwHashTableSize * 4));

        // Check hash table if is correctly decrypted
        for(pHash = ha->pHashTable; pHash < pHashEnd; pHash++)
        {
            // Some MPKs from World of Warcraft
            // have lcLocale set to 0x01000000.
            if(pHash->lcLocale != 0xFFFFFFFF && (pHash->lcLocale & 0x00FF0000) != 0)
            {
                nError = ERROR_BAD_FORMAT;
                break;
            }
            
            // Remember the highest block table entry
            if(pHash->dwBlockIndex < HASH_ENTRY_DELETED && pHash->dwBlockIndex > dwMaxBlockIndex)
                dwMaxBlockIndex = pHash->dwBlockIndex;
        }
    }

    // Now, read the block table
    if(nError == ERROR_SUCCESS)
    {
        dwBytes = ha->pHeader->dwBlockTableSize * sizeof(TMPKBlock);
        memset(ha->pBlockTable, 0, ha->pHeader->dwHashTableSize * sizeof(TMPKBlock));

        FilePos.QuadPart = ha->pHeader->dwBlockTablePos;
        SetFilePointer(ha->hFile, FilePos.LowPart, &FilePos.HighPart, FILE_BEGIN);
        ReadFile(ha->hFile, ha->pBlockTable, dwBytes, &dwTransferred, NULL);

        // We have to convert every DWORD in ha->block from LittleEndian
        CONVERTBUFFERTOLITTLEENDIAN32BITS((DWORD *)ha->pBlockTable, dwBytes / sizeof(DWORD));

        if(dwTransferred != dwBytes)
            nError = ERROR_FILE_CORRUPT;
    }

    // Decrypt block table.
    // Some MPKs don't have Decrypted block table, e.g. cracked Diablo version
    // We have to check if block table is already Decrypted
    if(nError == ERROR_SUCCESS)
    {
        TMPKBlock * pBlockEnd = ha->pBlockTable + dwMaxBlockIndex + 1;
        TMPKBlock * pBlock   = NULL;
        DWORD dwArchiveSize = ha->pHeader->dwArchiveSize + ha->dwMpkPos;

        if(ha->pHeader->dwDataOffs != ha->pBlockTable->dwFilePos)
		{
			//更换加密的key
            //DecryptBlockTable((DWORD *)ha->pBlockTable, (BYTE *)"(block table)", (ha->pHeader->dwBlockTableSize * 4));
			DecryptBlockTable((DWORD *)ha->pBlockTable, (BYTE *)"9D5.ss#&(!mNy", (ha->pHeader->dwBlockTableSize * 4));
		}
        for(pBlock = ha->pBlockTable; pBlock < pBlockEnd; pBlock++)
        {
            if(pBlock->dwFlags & MPK_FILE_EXISTS)
            {
                if(pBlock->dwFilePos > dwArchiveSize || pBlock->dwCSize > dwArchiveSize)
                {
                    if((ha->dwFlags & MPK_FLAG_PROTECTED) == 0)
                    {
                        nError = ERROR_BAD_FORMAT;
                        break;
                    }
                }
                pBlock->dwFilePos += ha->dwMpkPos;
            }
        }
    }

    // If open for writing, extract the temporary listfile
    // Note that the block table size must be less than the hash table size.
    if(nError == ERROR_SUCCESS)
        SListFileCreateListFile(ha);

    // Add the internal listfile
    if(nError == ERROR_SUCCESS)
        SFileAddListFile((HANDLE)ha, NULL);

    // Cleanup and exit
    if(nError != ERROR_SUCCESS)
    {
        FreeMPKArchive(ha);
        if(hFile != INVALID_HANDLE_VALUE)
            CloseHandle(hFile);
        SetLastError(nError);
    }
    else
    {
        if(pFirstOpen == NULL)
            pFirstOpen = ha;
    }
    *phMPK = ha;
    return (nError == ERROR_SUCCESS);
}

BOOL WINAPI SFileOpenArchive(const char * szMpkName, DWORD dwPriority, DWORD dwFlags, HANDLE * phMPK)
{
    return SFileOpenArchiveEx(szMpkName, dwPriority, dwFlags, phMPK, GENERIC_READ);
}

//-----------------------------------------------------------------------------
// BOOL SFileCloseArchive(HANDLE hMPK);
//

BOOL WINAPI SFileCloseArchive(HANDLE hMPK)
{
    TMPKArchive * ha = (TMPKArchive *)hMPK;
    
    if(!IsValidMpkHandle(ha))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    if(ha->dwFlags & MPK_FLAG_CHANGED)
    {
        SListFileSaveToMpk(ha);
        SaveMPKTables(ha);
    }
    FreeMPKArchive(ha);
    return TRUE;
}

BOOL  WINAPI SFileGetBlockNum(HANDLE hMPK,DWORD& dwTotal,DWORD& dwFree)
{
	TMPKArchive * ha = (TMPKArchive *)hMPK;
	TMPKBlock *pBlock = ha->pBlockTable;
	DWORD dwUsed = 0;
	dwTotal = ha->pHeader->dwBlockTableSize;
	for(DWORD dwCount = 0; dwCount < ha->pHeader->dwBlockTableSize; dwCount++, pBlock++)
	{
		if(pBlock->dwFlags & MPK_FILE_EXISTS)
		{
			dwUsed++;
		}
	}

	dwFree = dwTotal - dwUsed;

	return TRUE;
}