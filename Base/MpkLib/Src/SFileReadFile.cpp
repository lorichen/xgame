#define __MPKLIB_SELF__
#include "MpkLib.h"
#include "SCommon.h"

//-----------------------------------------------------------------------------
// Defines

#define ID_WAVE     0x46464952          // Signature of WAVes for name breaking
#define ID_EXE      0x00005A4D          // Signature of executable files

//-----------------------------------------------------------------------------
// Local structures

struct TID2Ext
{
    DWORD dwID;
    char * szExt;
};

//-----------------------------------------------------------------------------
// ReadMPKBlock
//
//  hf          - MPK File handle.
//  dwBlockPos  - Position of block in the file (relative to file begin)
//  buffer      - Pointer to target buffer to store blocks.
//  dwBlockSize - Number of bytes to read. Must be multiplier of block size.
//
//  Returns number of bytes read.

static DWORD WINAPI ReadMPKBlocks(TMPKFile * hf, DWORD dwBlockPos, BYTE * buffer, DWORD blockBytes)
{
    LARGE_INTEGER FilePos;
    TMPKArchive * ha = hf->ha;          // Archive handle
    BYTE  * tempBuffer = NULL;          // Buffer for reading compressed data from the file
    DWORD   dwFilePos = dwBlockPos;     // Reading position from the file
    DWORD   toRead;                     // Number of bytes to read
    DWORD   blockNum;                   // Block number (needed for decrypt)
    DWORD   bytesRead = 0;              // Total number of bytes read
    DWORD   bytesRemain = 0;            // Number of data bytes remaining up to the end of the file
    DWORD   nBlocks;                    // Number of blocks to load
    DWORD   i;

    // Test parameters. Block position and block size must be block-aligned, block size nonzero
    if((dwBlockPos & (ha->dwBlockSize - 1)) || blockBytes == 0)
        return 0;

    // Check the end of file
    if((dwBlockPos + blockBytes) > hf->pBlock->dwFSize)
        blockBytes = hf->pBlock->dwFSize - dwBlockPos;

    bytesRemain = hf->pBlock->dwFSize - dwBlockPos;
    blockNum    = dwBlockPos / ha->dwBlockSize;
    nBlocks     = blockBytes / ha->dwBlockSize;
    if(blockBytes % ha->dwBlockSize)
        nBlocks++;

    // If file has variable block positions, we have to load them
    if((hf->pBlock->dwFlags & MPK_FILE_COMPRESSED) && hf->bBlockPosLoaded == FALSE)
    {
        if(hf->pBlock->dwFilePos != ha->dwFilePos)
        {
            FilePos.QuadPart = hf->pBlock->dwFilePos;
            SetFilePointer(ha->hFile, FilePos.LowPart, &FilePos.HighPart, FILE_BEGIN);
        }

        // Read block positions from begin of file.
        toRead = (hf->nBlocks+1) * sizeof(DWORD);
        ReadFile(ha->hFile, hf->pdwBlockPos, toRead, &bytesRead, NULL);
        CONVERTBUFFERTOLITTLEENDIAN32BITS(hf->pdwBlockPos, (hf->nBlocks+1));

        // If the archive if protected some way, perform additional check
        // Sometimes, the file appears not to be encrypted, but it is.
        // Edit: In WoW 1.10+, there's a new flag. With this flag present,
        // there's one additional entry in the block table. If this flag
        // is present, we skip this test to keep the file readable.
        
        if((hf->pBlock->dwFlags & MPK_FILE_04000000) == 0)
        {
            if(hf->pdwBlockPos[0] != bytesRead)
                hf->pBlock->dwFlags |= MPK_FILE_ENCRYPTED;
        }

        // Decrypt loaded block positions if necessary
        if(hf->pBlock->dwFlags & MPK_FILE_ENCRYPTED)
        {
            // If we don't know the file seed, try to find it.
            if(hf->dwSeed1 == 0)
                hf->dwSeed1 = DetectFileSeed(hf->pdwBlockPos, bytesRead);

            // If we don't know the file seed, sorry but we cannot extract the file.
            if(hf->dwSeed1 == 0)
                return 0;

            // Decrypt block positions
            DecryptMPKBlock(hf->pdwBlockPos, bytesRead, hf->dwSeed1 - 1);

            // Check if the block positions are correctly decrypted
            // I don't know why, but sometimes it will result invalid block positions on some files
            if(hf->pdwBlockPos[0] != bytesRead)
            {
                // Try once again to detect file seed and decrypt the blocks
                FilePos.QuadPart = hf->pBlock->dwFilePos;
                SetFilePointer(ha->hFile, FilePos.LowPart, &FilePos.HighPart, FILE_BEGIN);
                ReadFile(ha->hFile, hf->pdwBlockPos, toRead, &bytesRead, NULL);

                CONVERTBUFFERTOLITTLEENDIAN32BITS(hf->pdwBlockPos, (hf->nBlocks+1));
                hf->dwSeed1 = DetectFileSeed(hf->pdwBlockPos, bytesRead);
                DecryptMPKBlock(hf->pdwBlockPos, bytesRead, hf->dwSeed1 - 1);

                // Check if the block positions are correctly decrypted
                if(hf->pdwBlockPos[0] != bytesRead)
                    return 0;
            }
        }

        // Update hf's variables
        hf->bBlockPosLoaded = TRUE;
        ha->dwFilePos       = hf->pBlock->dwFilePos + bytesRead;
    }

    // Get file position and number of bytes to read
    dwFilePos = dwBlockPos;
    toRead    = blockBytes;
    if(hf->pBlock->dwFlags & MPK_FILE_COMPRESSED)
    {
        dwFilePos = hf->pdwBlockPos[blockNum];
        toRead    = hf->pdwBlockPos[blockNum + nBlocks] - dwFilePos;
    }
    dwFilePos += hf->pBlock->dwFilePos;

    // Get work buffer for store read data
    tempBuffer = buffer;
    if(hf->pBlock->dwFlags & MPK_FILE_COMPRESSED)
    {
        if((tempBuffer = ALLOCMEM(BYTE, toRead)) == NULL)
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return 0;
        }
    }

    // Set file pointer, if necessary 
    if(ha->dwFilePos != dwFilePos) 
    {
        FilePos.QuadPart = dwFilePos;
        ha->dwFilePos = SetFilePointer(ha->hFile, FilePos.LowPart, &FilePos.HighPart, FILE_BEGIN);
    }

    // 15018F87 : Read all requested blocks
    ReadFile(ha->hFile, tempBuffer, toRead, &bytesRead, NULL);
    ha->dwFilePos = dwFilePos + bytesRead;

    // Block processing part.
    DWORD blockStart = 0;               // Index of block start in work buffer
    DWORD blockSize  = min(blockBytes, ha->dwBlockSize);
    DWORD index      = blockNum;        // Current block index

    bytesRead = 0;                      // Clear read byte counter

    // Walk through all blocks
    for(i = 0; i < nBlocks; i++, index++)
    {
        BYTE * inputBuffer = tempBuffer + blockStart;
        int    outLength = ha->dwBlockSize;

        if(bytesRemain < (DWORD)outLength)
            outLength = bytesRemain;

        // Get current block length
        if(hf->pBlock->dwFlags & MPK_FILE_COMPRESSED)
            blockSize = hf->pdwBlockPos[index+1] - hf->pdwBlockPos[index];

        // If block is encrypted, we have to decrypt it.
        if(hf->pBlock->dwFlags & MPK_FILE_ENCRYPTED)
        {
            CONVERTBUFFERTOLITTLEENDIAN32BITS((DWORD *)inputBuffer, blockSize / sizeof(DWORD));

            // If we don't know the seed, try to decode it as WAVE file
            if(hf->dwSeed1 == 0)
                hf->dwSeed1 = DetectFileSeed2((DWORD *)inputBuffer, 3, ID_WAVE, hf->pBlock->dwFSize - 8, 0x45564157);

            // Let's try MSVC's standard EXE or header
            if(hf->dwSeed1 == 0)
                hf->dwSeed1 = DetectFileSeed2((DWORD *)inputBuffer, 2, 0x00905A4D, 0x00000003);

            if(hf->dwSeed1 == 0)
                return 0;

            DecryptMPKBlock((DWORD *)inputBuffer, blockSize, hf->dwSeed1 + index);
            CONVERTBUFFERTOLITTLEENDIAN32BITS((DWORD *)inputBuffer, blockSize / sizeof(DWORD));
        }

        // If the block is really compressed, decompress it.
        // WARNING : Some block may not be compressed, it can be determined only
        // by comparing uncompressed and compressed size !!!
        if(blockSize < (DWORD)outLength)
        {
            // Is the file compressed with PKWARE Data Compression Library ?
            if(hf->pBlock->dwFlags & MPK_FILE_COMPRESS_PKWARE)
                Decompress_pklib((char *)buffer, &outLength, (char *)inputBuffer, (int)blockSize);

            // Is it a file compressed by Blizzard's multiple compression ?
            // Note that Storm.dll v 1.0.9 distributed with Warcraft III
            // passes the full path name of the opened archive as the new last parameter
            if(hf->pBlock->dwFlags & MPK_FILE_COMPRESS_MULTI)
                SCompDecompress((char *)buffer, &outLength, (char *)inputBuffer, (int)blockSize);
            bytesRead += outLength;
            buffer    += outLength;
        }
        else
        {
            if(buffer != inputBuffer)
                memcpy(buffer, inputBuffer, blockSize);

            bytesRead += blockSize;
            buffer    += blockSize;
        }
        blockStart  += blockSize;
        bytesRemain -= outLength;
    }

    // Delete input buffer, if necessary
    if(hf->pBlock->dwFlags & MPK_FILE_COMPRESSED)
        FREEMEM(tempBuffer);

    return bytesRead;
}

// When this function is called, it is already ensured that the parameters are valid
// (e.g. the "dwToRead + dwFilePos" is not greater than the file size)
static DWORD WINAPI ReadMPKFileSingleUnit(TMPKFile * hf, DWORD dwFilePos, BYTE * pbBuffer, DWORD dwToRead)
{
    LARGE_INTEGER FilePos;
    TMPKArchive * ha = hf->ha; 
    DWORD dwBytesRead;
    int bytesRead = 0;

    if(hf->pBlock->dwFilePos != ha->dwFilePos)
    {
        FilePos.QuadPart = hf->pBlock->dwFilePos;
        SetFilePointer(ha->hFile, FilePos.LowPart, &FilePos.HighPart, FILE_BEGIN);
        ha->dwFilePos = hf->pBlock->dwFilePos;
    }

    // If the file is really compressed, decompress it.
    // Otherwise, read the data as-is to the caller.
    if(hf->pBlock->dwCSize < hf->pBlock->dwFSize)
    {
        if(hf->pbFileBuffer == NULL)
        {
            BYTE * inputBuffer = NULL;
            int outputBufferSize = (int)hf->pBlock->dwFSize;
            int inputBufferSize = (int)hf->pBlock->dwCSize;

            hf->pbFileBuffer = ALLOCMEM(BYTE, outputBufferSize);
            inputBuffer = ALLOCMEM(BYTE, inputBufferSize);
            if(inputBuffer != NULL && hf->pbFileBuffer != NULL)
            {
                // Read the compressed file data
                ReadFile(ha->hFile, inputBuffer, inputBufferSize, &dwBytesRead, NULL);

                // Is the file compressed with PKWARE Data Compression Library ?
                if(hf->pBlock->dwFlags & MPK_FILE_COMPRESS_PKWARE)
                    Decompress_pklib((char *)hf->pbFileBuffer, &outputBufferSize, (char *)inputBuffer, (int)inputBufferSize);

                // Is it a file compressed by Blizzard's multiple compression ?
                // Note that Storm.dll v 1.0.9 distributed with Warcraft III
                // passes the full path name of the opened archive as the new last parameter
                if(hf->pBlock->dwFlags & MPK_FILE_COMPRESS_MULTI)
                    SCompDecompress((char *)hf->pbFileBuffer, &outputBufferSize, (char *)inputBuffer, (int)inputBufferSize);
            }

            // Free the temporary buffer
            if(inputBuffer != NULL)
                FREEMEM(inputBuffer);
        }

        // Copy the file data, if any there
        if(hf->pbFileBuffer != NULL)
        {
            memcpy(pbBuffer, hf->pbFileBuffer + dwFilePos, dwToRead);
            bytesRead += dwToRead;
        }
    }
    else
    {
        // Read the uncompressed file data
        ReadFile(ha->hFile, pbBuffer, dwToRead, &dwBytesRead, NULL);
        bytesRead = (int)dwBytesRead;
    }

    return (DWORD)bytesRead;
}


//-----------------------------------------------------------------------------
// ReadMPKFile

static DWORD WINAPI ReadMPKFile(TMPKFile * hf, DWORD dwFilePos, BYTE * pbBuffer, DWORD dwToRead)
{
    TMPKArchive * ha = hf->ha; 
    TMPKBlock * pBlock = hf->pBlock; // Pointer to file block
    DWORD dwBytesRead = 0;           // Number of bytes read from the file
    DWORD dwBlockPos;                // Position in the file aligned to the whole blocks
    DWORD dwLoaded;

    // File position is greater or equal to file size ?
    if(dwFilePos >= pBlock->dwFSize)
        return dwBytesRead;

    // If too few bytes in the file remaining, cut them
    if((pBlock->dwFSize - dwFilePos) < dwToRead)
        dwToRead = (pBlock->dwFSize - dwFilePos);

    // If the file is stored as single unit, handle it separately
    if(pBlock->dwFlags & MPK_FILE_SINGLE_UNIT)
        return ReadMPKFileSingleUnit(hf, dwFilePos, pbBuffer, dwToRead);

    // Block position in the file
    dwBlockPos = dwFilePos & ~(ha->dwBlockSize - 1);  // Position in the block

    // Load the first block, if noncomplete. It may be loaded in the cache buffer.
    // We have to check if this block is loaded. If not, load it.
    if((dwFilePos % ha->dwBlockSize) != 0)
    {
        // Number of bytes remaining in the buffer
        DWORD dwToCopy;
        DWORD dwLoaded = ha->dwBlockSize;

        // Check if data are loaded in the cache
        if(hf != ha->pLastFile || dwBlockPos != ha->dwBlockPos)
        {
            // Load one MPK block into archive buffer
            dwLoaded = ReadMPKBlocks(hf, dwBlockPos, ha->pbBlockBuffer, ha->dwBlockSize);
            if(dwLoaded == 0)
                return (DWORD)-1;

            // Save lastly accessed file and block position for later use
            ha->pLastFile  = hf;
            ha->dwBlockPos = dwBlockPos;
            ha->dwBuffPos  = dwFilePos % ha->dwBlockSize;
        }
        dwToCopy = dwLoaded - ha->dwBuffPos;
        if(dwToCopy > dwToRead)
            dwToCopy = dwToRead;

        // Copy data from block buffer into target buffer
        memcpy(pbBuffer, ha->pbBlockBuffer + ha->dwBuffPos, dwToCopy);
    
        // Update pointers
        dwToRead      -= dwToCopy;
        dwBytesRead   += dwToCopy;
        pbBuffer      += dwToCopy;
        dwBlockPos    += ha->dwBlockSize;
        ha->dwBuffPos += dwToCopy;

        // If all, return.
        if(dwToRead == 0)
            return dwBytesRead;
    }

    // Load the whole ("middle") blocks only if there are more or equal one block
    if(dwToRead > ha->dwBlockSize)
    {                                           
        DWORD dwBlockBytes = dwToRead & ~(ha->dwBlockSize - 1);

        dwLoaded = ReadMPKBlocks(hf, dwBlockPos, pbBuffer, dwBlockBytes);
        if(dwLoaded == 0)
            return (DWORD)-1;

        // Update pointers
        dwToRead    -= dwLoaded;
        dwBytesRead += dwLoaded;
        pbBuffer    += dwLoaded;
        dwBlockPos  += dwLoaded;

        // If all, return.
        if(dwToRead == 0)
            return dwBytesRead;
    }

    // Load the terminating block
    if(dwToRead > 0)
    {
        DWORD dwToCopy = ha->dwBlockSize;

        // Check if data are loaded in the cache
        if(hf != ha->pLastFile || dwBlockPos != ha->dwBlockPos)
        {
            // Load one MPK block into archive buffer
            dwToCopy = ReadMPKBlocks(hf, dwBlockPos, ha->pbBlockBuffer, ha->dwBlockSize);
            if(dwToCopy == 0)
                return (DWORD)-1;

            // Save lastly accessed file and block position for later use
            ha->pLastFile  = hf;
            ha->dwBlockPos = dwBlockPos;
        }
        ha->dwBuffPos  = 0;

        // Check number of bytes read
        if(dwToCopy > dwToRead)
            dwToCopy = dwToRead;

        memcpy(pbBuffer, ha->pbBlockBuffer, dwToCopy);
        dwBytesRead  += dwToCopy;
        ha->dwBuffPos = dwToCopy;
    }
    
    // Return what we've read
    return dwBytesRead;
}

//-----------------------------------------------------------------------------
// SFileReadFile

BOOL WINAPI SFileReadFile(HANDLE hFile, VOID * lpBuffer, DWORD dwToRead, DWORD * pdwRead, LPOVERLAPPED lpOverlapped)
{
    TMPKFile * hf = (TMPKFile *)hFile;
    DWORD dwBytes = 0;                  // Number of bytes (for everything)
    int nError = ERROR_SUCCESS;

    // Zero the number of bytes read
    if(pdwRead != NULL)
        *pdwRead = 0;

    // Check valid parameters
    if(nError == ERROR_SUCCESS)
    {
        if(hf == NULL || lpBuffer == NULL)
            nError = ERROR_INVALID_PARAMETER;
    }

    // If direct access to the file, use Win32 for reading
    if(nError == ERROR_SUCCESS && hf->hFile != INVALID_HANDLE_VALUE)
    {
        ReadFile(hf->hFile, lpBuffer, dwToRead, pdwRead, lpOverlapped);
        if(*pdwRead < dwToRead)
        {
            SetLastError(ERROR_HANDLE_EOF);
            return FALSE;
        }
        return TRUE;
    }

    // Read all the bytes available in the buffer (If any)
    if(nError == ERROR_SUCCESS)
    {
        if(dwToRead > 0)
        {
            dwBytes = ReadMPKFile(hf, hf->dwFilePos, (BYTE *)lpBuffer, dwToRead);
            if(dwBytes == (DWORD)-1)
            {
                SetLastError(ERROR_CAN_NOT_COMPLETE);
                return FALSE;
            }
            hf->ha->pLastFile = hf;
            hf->dwFilePos += dwBytes;
        }
        if(pdwRead != NULL)
            *pdwRead = dwBytes;
    }

    // Check number of bytes read. If not OK, return FALSE.
    if(dwBytes < dwToRead)
    {
        SetLastError(ERROR_HANDLE_EOF);
        return FALSE;
    }
    return TRUE;
}

//-----------------------------------------------------------------------------
// SFileGetFilePos
//
// Returns position of archive file in the archive (relative to begin of file)

DWORD WINAPI SFileGetFilePos(HANDLE hFile, DWORD * pdwFilePosHigh)
{
    TMPKFile * hf = (TMPKFile *)hFile;
    
    if(pdwFilePosHigh != NULL)
        *pdwFilePosHigh = 0;

    if(hf == NULL)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return (DWORD)-1;
    }

    // If opened as plain file, ...
    if(hf->hFile != INVALID_HANDLE_VALUE)
        return 0;

    // If opened from archive, return file size
    return hf->ha->dwMpkPos + hf->pBlock->dwFilePos;
}


DWORD WINAPI SFileGetFilePointer(HANDLE hFile, DWORD * pdwFilePosHigh)
{
	TMPKFile * hf = (TMPKFile *)hFile;

	if(pdwFilePosHigh != NULL)
		*pdwFilePosHigh = 0;

	if(hf == NULL)
	{
		SetLastError(ERROR_INVALID_PARAMETER);
		return (DWORD)-1;
	}

	// If opened as plain file, ...
	if(hf->hFile != INVALID_HANDLE_VALUE)
		return 0;

	// If opened from archive, return file size
	return hf->dwFilePos;
}

//-----------------------------------------------------------------------------
// SFileGetFileSize

DWORD WINAPI SFileGetFileSize(HANDLE hFile, DWORD * pdwFileSizeHigh)
{
    TMPKFile * hf = (TMPKFile *)hFile;
    
    if(pdwFileSizeHigh != NULL)
        *pdwFileSizeHigh = 0;

    if(hf == NULL)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return (DWORD)-1;
    }

    // If opened as plain file, ...
    if(hf->hFile != INVALID_HANDLE_VALUE)
        return GetFileSize(hf->hFile, pdwFileSizeHigh);

    // If opened from archive, return file size
    return hf->pBlock->dwFSize;
}

DWORD WINAPI SFileSetFilePointer(HANDLE hFile, LONG lFilePos, LONG * pdwFilePosHigh, DWORD dwMethod)
{
    TMPKArchive * ha;
    TMPKFile * hf = (TMPKFile *)hFile;

    if(hf == NULL || (pdwFilePosHigh != NULL && *pdwFilePosHigh != 0))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return (DWORD)-1;
    }

    // If opened as plain file, call Win32 API
    if(hf->hFile != INVALID_HANDLE_VALUE)
        return SetFilePointer(hf->hFile, lFilePos, pdwFilePosHigh, dwMethod);
    ha = hf->ha;

    switch(dwMethod)
    {
        case FILE_BEGIN:
            // Cannot set pointer before begin of file
            if(-lFilePos > (LONG)hf->dwFilePos)
                hf->dwFilePos = 0;
            else
                hf->dwFilePos = lFilePos;
            break;

        case FILE_CURRENT:
            // Cannot set pointer before begin of file
            if(-lFilePos > (LONG)hf->dwFilePos)
                hf->dwFilePos = 0;
            else
                hf->dwFilePos += lFilePos;
            break;

        case FILE_END:
            // Cannot set file position before begin of file
            if(-lFilePos >= (LONG)hf->pBlock->dwFSize)
                hf->dwFilePos = 0;
            else
                hf->dwFilePos = hf->pBlock->dwFSize + lFilePos;
            break;

        default:
            return ERROR_INVALID_PARAMETER;
    }

    if(hf == ha->pLastFile && (hf->dwFilePos & ~(ha->dwBlockSize - 1)) == ha->dwBlockPos)
        ha->dwBuffPos = hf->dwFilePos & (ha->dwBlockSize - 1);
    else
    {
        ha->pLastFile = NULL;
        ha->dwBuffPos = 0;
    }

    return hf->dwFilePos;
}

//-----------------------------------------------------------------------------
// Tries to retrieve the file name

static TID2Ext id2ext[] = 
{
    {0x1A51504D, "mpq"},                // MPK archive header ID ('MPK\x1A')
    {0x46464952, "wav"},                // WAVE header 'RIFF'
    {0x324B4D53, "smk"},                // Old "Smacker Video" files 'SMK2'
    {0x694B4942, "bik"},                // Bink video files (new)
    {0x0801050A, "pcx"},                // PCX images used in Diablo I
    {0x544E4F46, "fnt"},                // Font files used in Diablo II
    {0x6D74683C, "html"},               // HTML '<htm'
    {0x4D54483C, "html"},               // HTML '<HTM
    {0x216F6F57, "tbl"},                // Table files
    {0x31504C42, "blp"},                // BLP textures
    {0x584C444D, "mdx"},                // MDX files
    {0x45505954, "pud"},                // Warcraft II maps
    {0x38464947, "gif"},                // GIF images 'GIF8'
    {0, NULL}                           // Terminator 
};

BOOL WINAPI SFileGetFileName(HANDLE hFile, char * szFileName)
{
    TMPKFile * hf = (TMPKFile *)hFile;  // MPK File handle
    char * szExt = "xxx";               // Default extension
    DWORD dwFirstBytes[2];              // The first 4 bytes of the file
    DWORD dwFilePos;                    // Saved file position
    int nError = ERROR_SUCCESS;
    int i;

    // Pre-zero the output buffer
    if(szFileName != NULL)
        *szFileName = 0;

    // Check valid parameters
    if(nError == ERROR_SUCCESS)
    {
        if(hf == NULL || szFileName == NULL)
            nError = ERROR_INVALID_PARAMETER;
    }
    
    // If the file name is already filled, return it.
    if(nError == ERROR_SUCCESS && *hf->szFileName != 0)
    {
        if(szFileName != hf->szFileName)
            strcpy(szFileName, hf->szFileName);
        return TRUE;
    }

    if(nError == ERROR_SUCCESS)
    {
        if(hf->dwFileIndex == (DWORD)-1)
            nError = ERROR_CAN_NOT_COMPLETE;
    }

    // Read the first 8 bytes from the file
    if(nError == ERROR_SUCCESS)
    {
        dwFirstBytes[0] = dwFirstBytes[1] = 0;
        dwFilePos = SFileSetFilePointer(hf, 0, NULL, FILE_CURRENT);   
        if(!SFileReadFile(hFile, &dwFirstBytes, sizeof(dwFirstBytes), NULL))
            nError = GetLastError();
        CONVERTBUFFERTOLITTLEENDIAN32BITS(dwFirstBytes, sizeof(dwFirstBytes) / sizeof(DWORD));
        SFileSetFilePointer(hf, dwFilePos, NULL, FILE_BEGIN);
    }

    if(nError == ERROR_SUCCESS)
    {
        if((dwFirstBytes[0] & 0x0000FFFF) == ID_EXE)
            szExt = "exe";
        else if(dwFirstBytes[0] == 0x00000006 && dwFirstBytes[1] == 0x00000001)
            szExt = "dc6";
        else
        {
            for(i = 0; id2ext[i].szExt != NULL; i++)
            {
                if(id2ext[i].dwID == dwFirstBytes[0])
                {
                    szExt = id2ext[i].szExt;
                    break;
                }
            }
        }

        // Create the file name
        sprintf(hf->szFileName, "File%08lu.%s", hf->dwFileIndex, szExt);
        if(szFileName != hf->szFileName)
            strcpy(szFileName, hf->szFileName);
    }
    return (nError == ERROR_SUCCESS);
}

//-----------------------------------------------------------------------------
// Retrieves an information about an archive or about a file within the archive
//
//  hMpkOrFile - Handle to an MPK archive or to a file
//  dwInfoType - Information to obtain

DWORD WINAPI SFileGetFileInfo(HANDLE hMpkOrFile, DWORD dwInfoType)
{
    TMPKArchive * ha = (TMPKArchive *)hMpkOrFile;
    TMPKFile    * hf = (TMPKFile *)hMpkOrFile;
    TMPKBlock   * pBlockEnd;
    TMPKBlock   * pBlock;
    DWORD dwFileCount = 0;
    DWORD dwSeed;

    switch(dwInfoType)
    {
        case SFILE_INFO_ARCHIVE_SIZE:
            if(IsValidMpkHandle(ha))
                return ha->pHeader->dwArchiveSize;
            break;

        case SFILE_INFO_HASH_TABLE_SIZE: // Size of the hash table
            if(IsValidMpkHandle(ha))
                return ha->pHeader->dwHashTableSize;
            break;

        case SFILE_INFO_BLOCK_TABLE_SIZE: // Size of the hash table
            if(IsValidMpkHandle(ha))
                return ha->pHeader->dwBlockTableSize;
            break;

        case SFILE_INFO_BLOCK_SIZE:
            if(IsValidMpkHandle(ha))
                return ha->dwBlockSize;
            break;

        case SFILE_INFO_HASH_TABLE:
            if(IsValidMpkHandle(ha))
                return (DWORD)ha->pHashTable;
            break;

        case SFILE_INFO_BLOCK_TABLE:
            if(IsValidMpkHandle(ha))
                return (DWORD)ha->pBlockTable;
            break;

        case SFILE_INFO_NUM_FILES:
            if(IsValidMpkHandle(ha))
            {
                pBlockEnd = ha->pBlockTable + ha->pHeader->dwBlockTableSize;
                for(pBlock = ha->pBlockTable; pBlock < pBlockEnd; pBlock++)
                {
                    if(pBlock->dwFlags & MPK_FILE_EXISTS)
                        dwFileCount++;
                }
                return dwFileCount;
            }
            break;

        case SFILE_INFO_HASH_INDEX:
            if(IsValidFileHandle(hf))
                return hf->dwHashIndex;
            break;

        case SFILE_INFO_CODENAME1:
            if(IsValidFileHandle(hf))
                return hf->pHash->dwName1;
            break;

        case SFILE_INFO_CODENAME2:
            if(IsValidFileHandle(hf))
                return hf->pHash->dwName2;
            break;

        case SFILE_INFO_LOCALEID:
            if(IsValidFileHandle(hf))
                return hf->pHash->lcLocale;
            break;

        case SFILE_INFO_BLOCKINDEX:
            if(IsValidFileHandle(hf))
                return hf->dwFileIndex;
            break;

        case SFILE_INFO_FILE_SIZE:
            if(IsValidFileHandle(hf))
                return hf->pBlock->dwFSize;
            break;

        case SFILE_INFO_COMPRESSED_SIZE:
            if(IsValidFileHandle(hf))
                return hf->pBlock->dwCSize;
            break;

        case SFILE_INFO_FLAGS:
            if(IsValidFileHandle(hf))
                return hf->pBlock->dwFlags;
            break;

        case SFILE_INFO_POSITION:
            if(IsValidFileHandle(hf))
                return hf->pBlock->dwFilePos;
            break;

        case SFILE_INFO_SEED:
            if(IsValidFileHandle(hf))
                return hf->dwSeed1;
            break;

        case SFILE_INFO_SEED_UNFIXED:
            if(IsValidFileHandle(hf))
            {
                dwSeed = hf->dwSeed1;
                if(hf->pBlock->dwFlags & MPK_FILE_FIXSEED)
                    dwSeed = (dwSeed ^ hf->pBlock->dwFSize) - (hf->pBlock->dwFilePos - hf->ha->dwMpkPos);
                return dwSeed;
            }
            break;
    }

    // Unknown parameter or invalid handle
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0xFFFFFFFF;
}
