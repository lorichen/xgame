#define __MPKLIB_SELF__
#include "MpkLib.h"
#include "SCommon.h"

//-----------------------------------------------------------------------------
// Defines

#define DEFAULT_BLOCK_SIZE  3       // Default size of the block

//-----------------------------------------------------------------------------
// Local tables

static DWORD PowersOfTwo[] = 
{
               0x0000002, 0x0000004, 0x0000008,
    0x0000010, 0x0000020, 0x0000040, 0x0000080,
    0x0000100, 0x0000200, 0x0000400, 0x0000800,
    0x0001000, 0x0002000, 0x0004000, 0x0008000,
    0x0010000, 0x0020000, 0x0040000, 0x0080000,
    0x0000000
};

/*****************************************************************************/
/* Public functions                                                          */
/*****************************************************************************/

//-----------------------------------------------------------------------------
// Opens or creates a (new) MPK archive.
//
//  szMpkName - Name of the archive to be created.
//
//  dwCreationDisposition:
//   Value          Archive exists         Archive doesn't exist
//   ----------     ---------------------  ---------------------
//   CREATE_NEW     Fails                  Creates new archive
//   CREATE_ALWAYS  Overwrites existing    Creates new archive
//   OPEN_EXISTING  Opens the archive      Fails
//   OPEN_ALWAYS    Opens the archive      Creates new archive
//   
// dwHashTableSize - Size of the hash table (only if creating a new archive).
//        Must be between 2^4 (= 16) and 2^18 (= 262 144)
// phMpk - Receives handle to the archive

BOOL WINAPI SFileCreateArchiveEx(const char * szMpkName, DWORD dwCreationDisposition, DWORD dwHashTableSize, HANDLE * phMPK)
{
    LARGE_INTEGER FilePos;
    TMPKArchive * ha = NULL;                // MPK archive handle
    HANDLE hFile    = INVALID_HANDLE_VALUE; // File handle
    DWORD dwMpkPos = 0;                     // Position of MPK header within the file
    DWORD dwTransferred = 0;                    // Number of bytes written into the archive
    BOOL bFileExists = FALSE;
    int nIndex = 0;
    int nError = ERROR_SUCCESS;

    // Pre-initialize the result value
    if(phMPK != NULL)
        *phMPK = NULL;

    // Check the parameters, if they are valid
    if(szMpkName == NULL || *szMpkName == 0 || phMPK == NULL)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    // Check the value of dwCreationDisposition against file existence
    bFileExists = (GetFileAttributes(szMpkName) != 0xFFFFFFFF);

    // If the file exists and open required, do it.
    if(bFileExists && (dwCreationDisposition == OPEN_EXISTING || dwCreationDisposition == OPEN_ALWAYS))
    {
        // Try to open the archive normal way. If it fails, it means that
        // the file exist, but it is not a MPK archive.
        if(SFileOpenArchiveEx(szMpkName, 0, 0, phMPK, GENERIC_READ | GENERIC_WRITE))
            return TRUE;
    }

    // Two error cases
    if(dwCreationDisposition == CREATE_NEW && bFileExists)
    {
        SetLastError(ERROR_ALREADY_EXISTS);
        return FALSE;
    }
    if(dwCreationDisposition == OPEN_EXISTING && bFileExists == FALSE)
    {
        SetLastError(ERROR_FILE_NOT_FOUND);
        return FALSE;
    }

    // At this point, we have to create the archive. If the file exists,
    // we will convert it to MPK archive.
    // Check the value of hash table size. It has to be a power of two
    // and must be between HASH_TABLE_SIZE_MIN and HASH_TABLE_SIZE_MAX
    if(dwHashTableSize < HASH_TABLE_SIZE_MIN)
        dwHashTableSize = HASH_TABLE_SIZE_MIN;
    if(dwHashTableSize > HASH_TABLE_SIZE_MAX)
        dwHashTableSize = HASH_TABLE_SIZE_MAX;
    
    // Round the hash table size up to the nearest power of two
    for(nIndex = 0; PowersOfTwo[nIndex] != 0; nIndex++)
    {
        if(dwHashTableSize <= PowersOfTwo[nIndex])
        {
            dwHashTableSize = PowersOfTwo[nIndex];
            break;
        }
    }

    // Prepare the buffer for decryption engine
    if(nError == ERROR_SUCCESS)
        nError = PrepareStormBuffer();

    // Get the position where the MPK header will begin.
    if(nError == ERROR_SUCCESS)
    {
        hFile = CreateFile(szMpkName, GENERIC_READ | GENERIC_WRITE, 0, NULL, dwCreationDisposition, 0, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
            nError = GetLastError();
    }

    // Retrieve the file size and round it up to 0x200 bytes
    if(nError == ERROR_SUCCESS)
    {
        FilePos.QuadPart = 
        dwMpkPos = ((GetFileSize(hFile, NULL) + 0x1FF) & 0xFFFFFE00);

        if(SetFilePointer(hFile, FilePos.LowPart, &FilePos.HighPart, FILE_BEGIN) == 0xFFFFFFFF)
            nError = GetLastError();
    }

    // Set the new end of the file to the MPK header position
    if(nError == ERROR_SUCCESS)
    {
        if(!SetEndOfFile(hFile))
            nError = GetLastError();
    }

    // Create the archive handle
    if(nError == ERROR_SUCCESS)
    {
        if((ha = ALLOCMEM(TMPKArchive, 1)) == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // Fill the MPK archive handle structure and create the header,
    // block buffer, hash table and block table
    if(nError == ERROR_SUCCESS)
    {
        memset(ha, 0, sizeof(TMPKArchive));
        strcpy(ha->szFileName, szMpkName);
        ha->hFile         = hFile;
        ha->dwBlockSize   = 0x200 << DEFAULT_BLOCK_SIZE;
        ha->dwMpkPos      = dwMpkPos;
        ha->dwFilePos     = dwMpkPos;
        ha->pHeader       = &ha->Header;
        ha->pHashTable    = ALLOCMEM(TMPKHash, dwHashTableSize);
        ha->pBlockTable   = ALLOCMEM(TMPKBlock, dwHashTableSize);
        ha->pbBlockBuffer = ALLOCMEM(BYTE, ha->dwBlockSize);
        ha->pListFile     = NULL;
        ha->dwFlags      |= MPK_FLAG_CHANGED;

        if(!ha->pHashTable || !ha->pBlockTable || !ha->pbBlockBuffer)
            nError = GetLastError();
        hFile = INVALID_HANDLE_VALUE;
    }

    // Fill the MPK header and all buffers
    if(nError == ERROR_SUCCESS)
    {
        memset(ha->pHeader, 0, sizeof(TMPKHeader));
        ha->pHeader->dwID            = ID_MPK;
        ha->pHeader->dwDataOffs      = sizeof(TMPKHeader);
        ha->pHeader->wBlockSize      = 3;        // 0x1000 bytes per block
        ha->pHeader->dwArchiveSize   = sizeof(TMPKHeader) + dwHashTableSize * sizeof(TMPKHash);
        ha->pHeader->dwHashTableSize = dwHashTableSize;
        ha->pHeader->dwHashTablePos  = ha->dwMpkPos + sizeof(TMPKHeader);
        ha->pHeader->dwBlockTablePos = ha->dwMpkPos + sizeof(TMPKHeader) + dwHashTableSize * sizeof(TMPKHash);

        memset(ha->pBlockTable, 0, sizeof(TMPKBlock) * dwHashTableSize);
        memset(ha->pHashTable, 0xFF, sizeof(TMPKHash) * dwHashTableSize);
    }

    // Write the MPK header to the file
    if(nError == ERROR_SUCCESS)
    {
        ha->dwFilePos = ha->dwMpkPos + sizeof(TMPKHeader);
        CONVERTTMPKHEADERTOLITTLEENDIAN(ha->pHeader);
        if(!WriteFile(ha->hFile, ha->pHeader, sizeof(TMPKHeader), &dwTransferred, NULL))
            nError = GetLastError();
        CONVERTTMPKHEADERTOLITTLEENDIAN(ha->pHeader);
    }

    // Create the internal listfile
    if(nError == ERROR_SUCCESS)
        nError = SListFileCreateListFile(ha);

    // Try to add the internal listfile
    if(nError == ERROR_SUCCESS)
        SFileAddListFile((HANDLE)ha, NULL);

    // Cleanup : If an error, delete all buffers and return
    if(nError != ERROR_SUCCESS)
    {
        FreeMPKArchive(ha);
        if(hFile != INVALID_HANDLE_VALUE)
            CloseHandle(hFile);
        SetLastError(nError);
    }
    
    // Return the values
    *phMPK = (HANDLE)ha;
    return (nError == ERROR_SUCCESS);
}

//-----------------------------------------------------------------------------
// Changes locale ID of a file

BOOL WINAPI SFileSetFileLocale(HANDLE hFile, LCID lcNewLocale)
{
    TMPKFile * hf = (TMPKFile *)hFile;

    // Invalid handle => do nothing
    if(IsValidFileHandle(hf) == FALSE || IsValidMpkHandle(hf->ha) == FALSE)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    // If the file has not been open for writing, do nothing.
    if(hf->ha->pListFile == NULL)
        return ERROR_ACCESS_DENIED;

    hf->pHash->lcLocale = lcNewLocale;
    hf->ha->dwFlags |= MPK_FLAG_CHANGED;
    return TRUE;
}

//-----------------------------------------------------------------------------
// Adds a file into the archive

BOOL WINAPI SFileAddFileEx(HANDLE hMPK, const char * szFileName, const char * szArchivedName, DWORD dwFlags, DWORD dwQuality, int nFileType)
{
    TMPKArchive * ha = (TMPKArchive *)hMPK;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    BOOL   bReplaced = FALSE;          // TRUE if replacing file in the archive
    int    nError = ERROR_SUCCESS;

    if(nError == ERROR_SUCCESS)
    {
        // Check valid parameters
        if(IsValidMpkHandle(ha) == FALSE || szFileName == NULL || *szFileName == 0 || szArchivedName == NULL || *szArchivedName == 0)
            nError = ERROR_INVALID_PARAMETER;

        // Check the values of dwFlags
        if((dwFlags & MPK_FILE_COMPRESS_PKWARE) && (dwFlags & MPK_FILE_COMPRESS_MULTI))
            nError = ERROR_INVALID_PARAMETER;
    }

    // If anyone is trying to add listfile, and the archive already has a listfile,
    // deny the operation, but return success.
    if(nError == ERROR_SUCCESS)
    {
        if(ha->pListFile != NULL && !_stricmp(szFileName, LISTFILE_NAME))
            return ERROR_SUCCESS;
    }

	FILETIME ftCreateFileTime;
	FILETIME ftLastLastWriteFileTime;

    // Open added file
    if(nError == ERROR_SUCCESS)
    {
        hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, NULL);
        if(hFile == INVALID_HANDLE_VALUE)
		{
			nError = GetLastError();
		}

		GetFileTime(hFile, &ftCreateFileTime, NULL, & ftLastLastWriteFileTime);

    }

    if(nError == ERROR_SUCCESS)
        nError = AddFileToArchive(ha, hFile, szArchivedName, dwFlags, dwQuality, nFileType, &bReplaced);

    // Add the file into listfile also
    if(nError == ERROR_SUCCESS && bReplaced == FALSE)
	{
		//crr mod
		//nError = SListFileAddNode(ha, szArchivedName);
		SListFileAddNode(ha, szArchivedName, ftCreateFileTime, ftLastLastWriteFileTime);
	}

    // Cleanup and exit
    if(hFile != INVALID_HANDLE_VALUE)
        CloseHandle(hFile);
    if(nError != ERROR_SUCCESS)
        SetLastError(nError);
    return (nError == ERROR_SUCCESS);
}
                                                                                                                                 
// Adds a data file into the archive
BOOL WINAPI SFileAddFile(HANDLE hMPK, const char * szFileName, const char * szArchivedName, DWORD dwFlags)
{
    return SFileAddFileEx(hMPK, szFileName, szArchivedName, dwFlags, 0, SFILE_TYPE_DATA);
}

// Adds a WAVE file into the archive
BOOL WINAPI SFileAddWave(HANDLE hMPK, const char * szFileName, const char * szArchivedName, DWORD dwFlags, DWORD dwQuality)
{
    return SFileAddFileEx(hMPK, szFileName, szArchivedName, dwFlags, dwQuality, SFILE_TYPE_WAVE);
}

//-----------------------------------------------------------------------------
// BOOL SFileRemoveFile(HANDLE hMPK, char * szFileName)
//
// This function removes a file from the archive. The file content
// remains there, only the entries in the hash table and in the block
// table are updated. 

BOOL WINAPI SFileRemoveFile(HANDLE hMPK, const char * szFileName)
{
    TMPKArchive * ha = (TMPKArchive *)hMPK;
    TMPKHash    * pFileHash = NULL;     // Hash entry of deleted file
    TMPKBlock   * pFileBlock = NULL;    // Block entry of deleted file
    DWORD dwBlockIndex = 0;
    int nError = ERROR_SUCCESS;

    // Check the parameters
    if(nError == ERROR_SUCCESS)
    {
        if(IsValidMpkHandle(ha) == FALSE)
            nError = ERROR_INVALID_PARAMETER;
        if((DWORD)szFileName > 0x00010000 && *szFileName == 0)
            nError = ERROR_INVALID_PARAMETER;
    }

    // Do not allow to remove listfile
    if(nError == ERROR_SUCCESS)
    {
        if((DWORD)szFileName > 0x00010000 && !_stricmp(szFileName, LISTFILE_NAME))
            nError = ERROR_ACCESS_DENIED;
    }

    // Get hash entry belonging to this file
    if(nError == ERROR_SUCCESS)
    {
        if((pFileHash = GetHashEntryEx(ha, (char *)szFileName, lcLocale)) == NULL)
            nError = ERROR_FILE_NOT_FOUND;
    }

    // If index was not found, or is greater than number of files, exit.
    if(nError == ERROR_SUCCESS)
    {
        if((dwBlockIndex = pFileHash->dwBlockIndex) > ha->pHeader->dwBlockTableSize)
            nError = ERROR_FILE_NOT_FOUND;
    }

    // Get block and test if the file is not already deleted
    if(nError == ERROR_SUCCESS)
    {
        pFileBlock = ha->pBlockTable + dwBlockIndex;
        if((pFileBlock->dwFlags & MPK_FILE_EXISTS) == 0)
            nError = ERROR_FILE_NOT_FOUND;
    }

    // Now invalidate the block entry and the hash entry. Do not make any
    // relocations and file copying, use SFileCompactArchive for it.
    if(nError == ERROR_SUCCESS)
    {
//      pFileBlock->dwFSize     = 0;
        pFileBlock->dwFlags     = 0;
        pFileHash->dwName1      = 0xFFFFFFFF;
        pFileHash->dwName2      = 0xFFFFFFFF;
        pFileHash->lcLocale     = 0xFFFFFFFF;
        pFileHash->dwBlockIndex = HASH_ENTRY_DELETED;

        // Update MPK archive
        ha->dwFlags |= MPK_FLAG_CHANGED;
    }

    // Remove the file from the list file
    if(nError == ERROR_SUCCESS && lcLocale == LANG_NEUTRAL)
        nError = SListFileRemoveNode(ha, szFileName);

    // Resolve error and exit
    if(nError != ERROR_SUCCESS)
        SetLastError(nError);
    return (nError == ERROR_SUCCESS);
}

// Renames the file within the archive.
BOOL WINAPI SFileRenameFile(HANDLE hMPK, const char * szFileName, const char * szNewFileName)
{
    TMPKArchive * ha = (TMPKArchive *)hMPK;
    TMPKHash * pOldHash = NULL;         // Hash entry for the original file
    TMPKHash * pNewHash = NULL;         // Hash entry for the renamed file
    DWORD dwBlockIndex = 0;
    int nError = ERROR_SUCCESS;

    // Test the valid parameters
    if(nError == ERROR_SUCCESS)
    {
        if(hMPK == NULL || szNewFileName == NULL || *szNewFileName == 0)
            nError = ERROR_INVALID_PARAMETER;
        if((DWORD)szFileName > 0x00010000 && *szFileName == 0)
            nError = ERROR_INVALID_PARAMETER;
    }

    // Do not allow to rename listfile
    if(nError == ERROR_SUCCESS)
    {
        if((DWORD)szFileName > 0x00010000 && !_stricmp(szFileName, LISTFILE_NAME))
            nError = ERROR_ACCESS_DENIED;
    }

    // Test if the file already exists in the archive
    if(nError == ERROR_SUCCESS)
    {
        if((pNewHash = GetHashEntryEx(ha, szNewFileName, lcLocale)) != NULL)
            nError = ERROR_ALREADY_EXISTS;
    }

    // Get the hash table entry for the original file
    if(nError == ERROR_SUCCESS)
    {
        if((pOldHash = GetHashEntryEx(ha, szFileName, lcLocale)) == NULL)
            nError = ERROR_FILE_NOT_FOUND;
    }

    // Get the hash table entry for the renamed file
    if(nError == ERROR_SUCCESS)
    {
        // Save block table index and remove the hash table entry
        dwBlockIndex = pOldHash->dwBlockIndex;
        pOldHash->dwName1      = 0xFFFFFFFF;
        pOldHash->dwName2      = 0xFFFFFFFF;
        pOldHash->lcLocale     = 0xFFFFFFFF;
        pOldHash->dwBlockIndex = HASH_ENTRY_DELETED;

        if((pNewHash = FindFreeHashEntry(ha, szNewFileName)) == NULL)
            nError = ERROR_CAN_NOT_COMPLETE;
    }

    // Save the block index and clear the hash entry
    if(nError == ERROR_SUCCESS)
    {
        // Copy the block table index
        pNewHash->dwBlockIndex = dwBlockIndex;
        ha->dwFlags |= MPK_FLAG_CHANGED;
    }

    // Rename the file in the list file
    if(nError == ERROR_SUCCESS)
        nError = SListFileRenameNode(ha, szFileName, szNewFileName);

    // Resolve error and return
    if(nError != ERROR_SUCCESS)
        SetLastError(nError);
    return (nError == ERROR_SUCCESS);
}

