#define __MPKLIB_SELF__
#include "MpkLib.h"
#include "SCommon.h"
#include <ctype.h>
#include <stdarg.h>

//-----------------------------------------------------------------------------
// The buffer for decryption engine.

TMPKArchive * pFirstOpen = NULL;        // The first member of MPK archives chain
LCID          lcLocale   = LANG_NEUTRAL;// File locale

//-----------------------------------------------------------------------------
// Storm buffer functions

// Buffer for the decryption engine
#define MPK_BUFFER_SIZE   0x500
static DWORD StormBuffer[MPK_BUFFER_SIZE];
static BOOL  bStormBufferCreated = FALSE;

int PrepareStormBuffer()
{
    DWORD dwSeed = 0x00100001;
    DWORD index1 = 0;
    DWORD index2 = 0;
    int   i;

    // Initialize the decryption buffer.
    // Do nothing if already done.
    if(bStormBufferCreated == FALSE)
    {
        for(index1 = 0; index1 < 0x100; index1++)
        {
            for(index2 = index1, i = 0; i < 5; i++, index2 += 0x100)
            {
                DWORD temp1, temp2;

                dwSeed = (dwSeed * 125 + 3) % 0x2AAAAB;
                temp1  = (dwSeed & 0xFFFF) << 0x10;

                dwSeed = (dwSeed * 125 + 3) % 0x2AAAAB;
                temp2  = (dwSeed & 0xFFFF);

                StormBuffer[index2] = (temp1 | temp2);
            }
        }
        bStormBufferCreated = TRUE;
    }
    return ERROR_SUCCESS;
}

//-----------------------------------------------------------------------------
// Encrypting and decrypting hash table

void EncryptHashTable(DWORD * pdwTable, BYTE * pbKey, DWORD dwLength)
{
    DWORD dwSeed1 = 0x7FED7FED;
    DWORD dwSeed2 = 0xEEEEEEEE;
    DWORD ch;                           // One key character

    // Prepare seeds
    while(*pbKey != 0)
    {
        ch = toupper(*pbKey++);

        dwSeed1 = StormBuffer[0x300 + ch] ^ (dwSeed1 + dwSeed2);
        dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }

    // Encrypt it
    dwSeed2 = 0xEEEEEEEE;
    while(dwLength-- > 0)
    {
        dwSeed2   += StormBuffer[0x400 + (dwSeed1 & 0xFF)];
        ch       = *pdwTable;
        *pdwTable++ = ch ^ (dwSeed1 + dwSeed2);

        dwSeed1  = ((~dwSeed1 << 0x15) + 0x11111111) | (dwSeed1 >> 0x0B);
        dwSeed2  = ch + dwSeed2 + (dwSeed2 << 5) + 3;
    }
}

void DecryptHashTable(DWORD * pdwTable, BYTE * pbKey, DWORD dwLength)
{
    DWORD dwSeed1 = 0x7FED7FED;
    DWORD dwSeed2 = 0xEEEEEEEE;
    DWORD ch;                           // One key character

    // Prepare seeds
    while(*pbKey != 0)
    {
        ch = toupper(*pbKey++);

        dwSeed1 = StormBuffer[0x300 + ch] ^ (dwSeed1 + dwSeed2);
        dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }

    // Decrypt it
    dwSeed2 = 0xEEEEEEEE;
    while(dwLength-- > 0)
    {
        dwSeed2 += StormBuffer[0x400 + (dwSeed1 & 0xFF)];
        ch       = *pdwTable ^ (dwSeed1 + dwSeed2);

        dwSeed1  = ((~dwSeed1 << 0x15) + 0x11111111) | (dwSeed1 >> 0x0B);
        dwSeed2  = ch + dwSeed2 + (dwSeed2 << 5) + 3;
        *pdwTable++ = ch;
    }
}

//-----------------------------------------------------------------------------
// Encrypting and decrypting block table

void EncryptBlockTable(DWORD * pdwTable, BYTE * pbKey, DWORD dwLength)
{
    DWORD dwSeed1 = 0x7FED7FED;
    DWORD dwSeed2 = 0xEEEEEEEE;
    DWORD ch;                           // One key character

    // Prepare seeds
    while(*pbKey != 0)
    {
        ch = toupper(*pbKey++);

        dwSeed1 = StormBuffer[0x300 + ch] ^ (dwSeed1 + dwSeed2);
        dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }

    // Decrypt it
    dwSeed2 = 0xEEEEEEEE;
    while(dwLength-- > 0)
    {
        dwSeed2   += StormBuffer[0x400 + (dwSeed1 & 0xFF)];
        ch       = *pdwTable;
        *pdwTable++ = ch ^ (dwSeed1 + dwSeed2);

        dwSeed1  = ((~dwSeed1 << 0x15) + 0x11111111) | (dwSeed1 >> 0x0B);
        dwSeed2  = ch + dwSeed2 + (dwSeed2 << 5) + 3;
    }
}

void DecryptBlockTable(DWORD * pdwTable, BYTE * pbKey, DWORD dwLength)
{
    DWORD dwSeed1 = 0x7FED7FED;
    DWORD dwSeed2 = 0xEEEEEEEE;
    DWORD ch;                           // One key character

    // Prepare seeds
    while(*pbKey != 0)
    {
        ch = toupper(*pbKey++);

        dwSeed1 = StormBuffer[0x300 + ch] ^ (dwSeed1 + dwSeed2);
        dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }

    // Encrypt it
    dwSeed2 = 0xEEEEEEEE;
    while(dwLength-- > 0)
    {
        dwSeed2 += StormBuffer[0x400 + (dwSeed1 & 0xFF)];
        ch     = *pdwTable ^ (dwSeed1 + dwSeed2);

        dwSeed1  = ((~dwSeed1 << 0x15) + 0x11111111) | (dwSeed1 >> 0x0B);
        dwSeed2  = ch + dwSeed2 + (dwSeed2 << 5) + 3;
        *pdwTable++ = ch;
    }
}

//-----------------------------------------------------------------------------
// Functions tries to get file decryption key. The trick comes from block
// positions which are stored at the begin of each compressed file. We know the
// file size, that means we know number of blocks that means we know the first
// DWORD value in block position. And if we know encrypted and decrypted value,
// we can find the decryption key !!!
//
// hf    - MPK file handle
// block - DWORD array of block positions
// ch    - Decrypted value of the first block pos

DWORD DetectFileSeed(DWORD * block, DWORD decrypted)
{
    DWORD saveSeed1;
    DWORD temp = *block ^ decrypted;    // temp = seed1 + seed2
    temp -= 0xEEEEEEEE;                 // temp = seed1 + StormBuffer[0x400 + (seed1 & 0xFF)]

    for(int i = 0; i < 0x100; i++)      // Try all 255 possibilities
    {
        DWORD seed1;
        DWORD seed2 = 0xEEEEEEEE;
        DWORD ch;

        // Try the first DWORD (We exactly know the value)
        seed1  = temp - StormBuffer[0x400 + i];
        seed2 += StormBuffer[0x400 + (seed1 & 0xFF)];
        ch     = block[0] ^ (seed1 + seed2);

        if(ch != decrypted)
            continue;

        // Add 1 because we are decrypting block positions
        saveSeed1 = seed1 + 1;

        // If OK, continue and test the second value. We don't know exactly the value,
        // but we know that the second one has lower 16 bits set to zero
        // (no compressed block is larger than 0xFFFF bytes)
        seed1  = ((~seed1 << 0x15) + 0x11111111) | (seed1 >> 0x0B);
        seed2  = ch + seed2 + (seed2 << 5) + 3;

        seed2 += StormBuffer[0x400 + (seed1 & 0xFF)];
        ch     = block[1] ^ (seed1 + seed2);

        if((ch & 0xFFFF0000) == 0)
            return saveSeed1;
    }
    return 0;
}

// Function tries to detect file seed. It expectes at least two uncompressed bytes
DWORD DetectFileSeed2(DWORD * pdwBlock, UINT nDwords, ...)
{
    va_list argList;
    DWORD dwDecrypted[0x10];
    DWORD saveSeed1;
    DWORD dwTemp;
    DWORD i, j;
    
    // We need at least two DWORDS to detect the seed
    if(nDwords < 0x02 || nDwords > 0x10)
        return 0;
    
    va_start(argList, nDwords);
    for(i = 0; i < nDwords; i++)
        dwDecrypted[i] = va_arg(argList, DWORD);
    va_end(argList);
    
    dwTemp = (*pdwBlock ^ dwDecrypted[0]) - 0xEEEEEEEE;
    for(i = 0; i < 0x100; i++)      // Try all 255 possibilities
    {
        DWORD seed1;
        DWORD seed2 = 0xEEEEEEEE;
        DWORD ch;

        // Try the first DWORD
        seed1  = dwTemp - StormBuffer[0x400 + i];
        seed2 += StormBuffer[0x400 + (seed1 & 0xFF)];
        ch     = pdwBlock[0] ^ (seed1 + seed2);

        if(ch != dwDecrypted[0])
            continue;

        saveSeed1 = seed1;

        // If OK, continue and test all bytes.
        for(j = 1; j < nDwords; j++)
        {
            seed1  = ((~seed1 << 0x15) + 0x11111111) | (seed1 >> 0x0B);
            seed2  = ch + seed2 + (seed2 << 5) + 3;

            seed2 += StormBuffer[0x400 + (seed1 & 0xFF)];
            ch     = pdwBlock[j] ^ (seed1 + seed2);

            if(ch == dwDecrypted[j] && j == nDwords - 1)
                return saveSeed1;
        }
    }
    return 0;
}


//-----------------------------------------------------------------------------
// Encrypting and decrypting MPK blocks

void EncryptMPKBlock(DWORD * block, DWORD dwLength, DWORD dwSeed1)
{
    DWORD dwSeed2 = 0xEEEEEEEE;
    DWORD ch;

    // Round to DWORDs
    dwLength >>= 2;

    while(dwLength-- > 0)
    {
        dwSeed2 += StormBuffer[0x400 + (dwSeed1 & 0xFF)];
        ch     = *block;
        *block++ = ch ^ (dwSeed1 + dwSeed2);

        dwSeed1  = ((~dwSeed1 << 0x15) + 0x11111111) | (dwSeed1 >> 0x0B);
        dwSeed2  = ch + dwSeed2 + (dwSeed2 << 5) + 3;
    }
}

void DecryptMPKBlock(DWORD * block, DWORD dwLength, DWORD dwSeed1)
{
    DWORD dwSeed2 = 0xEEEEEEEE;
    DWORD ch;

    // Round to DWORDs
    dwLength >>= 2;

    while(dwLength-- > 0)
    {
        dwSeed2 += StormBuffer[0x400 + (dwSeed1 & 0xFF)];
        ch     = *block ^ (dwSeed1 + dwSeed2);

        dwSeed1  = ((~dwSeed1 << 0x15) + 0x11111111) | (dwSeed1 >> 0x0B);
        dwSeed2  = ch + dwSeed2 + (dwSeed2 << 5) + 3;
        *block++ = ch;
    }
}


DWORD DecryptHashIndex(TMPKArchive * ha, const char * szFileName)
{
    BYTE * pbKey   = (BYTE *)szFileName;
    DWORD  dwSeed1 = 0x7FED7FED;
    DWORD  dwSeed2 = 0xEEEEEEEE;
    DWORD  ch;

    while(*pbKey != 0)
    {
        ch = toupper(*pbKey++);

        dwSeed1 = StormBuffer[0x000 + ch] ^ (dwSeed1 + dwSeed2);
        dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }
    return (dwSeed1 & (ha->pHeader->dwHashTableSize - 1));
}

DWORD DecryptName1(const char * szFileName)
{
    BYTE * pbKey   = (BYTE *)szFileName;
    DWORD  dwSeed1 = 0x7FED7FED;
    DWORD  dwSeed2 = 0xEEEEEEEE;
    DWORD  ch;

    while(*pbKey != 0)
    {
        ch = toupper(*pbKey++);

        dwSeed1 = StormBuffer[0x100 + ch] ^ (dwSeed1 + dwSeed2);
        dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }
    return dwSeed1;
}

DWORD DecryptName2(const char * szFileName)
{
    BYTE * pbKey   = (BYTE *)szFileName;
    DWORD  dwSeed1 = 0x7FED7FED;
    DWORD  dwSeed2 = 0xEEEEEEEE;
    int    ch;

    while(*pbKey != 0)
    {
        ch = toupper(*pbKey++);

        dwSeed1 = StormBuffer[0x200 + ch] ^ (dwSeed1 + dwSeed2);
        dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }
    return dwSeed1;
}

DWORD DecryptFileSeed(const char * szFileName)
{
    BYTE * pbKey   = (BYTE *)szFileName;
    DWORD  dwSeed1 = 0x7FED7FED;          // EBX
    DWORD  dwSeed2 = 0xEEEEEEEE;          // ESI
    DWORD  ch;

    while(*pbKey != 0)
    {
        ch = toupper(*pbKey++);           // ECX

        dwSeed1 = StormBuffer[0x300 + ch] ^ (dwSeed1 + dwSeed2);
        dwSeed2 = ch + dwSeed1 + dwSeed2 + (dwSeed2 << 5) + 3;
    }
    return dwSeed1;
}

TMPKHash * GetHashEntry(TMPKArchive * ha, const char * szFileName)
{
    TMPKHash * pHashEnd = ha->pHashTable + ha->pHeader->dwHashTableSize;
    TMPKHash * pHash0;                      // File hash entry (start)
    TMPKHash * pHash;                       // File hash entry (current)
    DWORD dwIndex = (DWORD)szFileName;      // Hash index
    DWORD dwName1;
    DWORD dwName2;

    // If filename is given by index, we have to search all hash entries for the right index.
    if(dwIndex <= ha->pHeader->dwBlockTableSize)
    {
        // Pass all the hash entries and find the 
        for(pHash = ha->pHashTable; pHash < pHashEnd; pHash++)
        {
            if(pHash->dwBlockIndex == dwIndex)
                return pHash;
        }
        return NULL;
    }

    // Decrypt name and block index
    dwIndex = DecryptHashIndex(ha, szFileName);
    dwName1 = DecryptName1(szFileName);
    dwName2 = DecryptName2(szFileName);
    pHash   = pHash0 = ha->pHashTable + dwIndex;
    
    // Look for hash index
    while(pHash->dwBlockIndex != HASH_ENTRY_FREE)
    {
        if(pHash->dwName1 == dwName1 && pHash->dwName2 == dwName2 && pHash->dwBlockIndex != HASH_ENTRY_DELETED)
            return pHash;

        // Move to the next hash entry
        if(++pHash >= pHashEnd)
            pHash = ha->pHashTable;
        if(pHash == pHash0)
            break;
    }

    // File was not found
    return NULL;
}

// Retrieves the locale-specific hash entry
TMPKHash * GetHashEntryEx(TMPKArchive * ha, const char * szFileName, LCID lcLocale)
{
    TMPKHash * pHashEnd = ha->pHashTable + ha->pHeader->dwHashTableSize;
    TMPKHash * pHash0 = NULL;           // Language-neutral hash entry
    TMPKHash * pHashX = NULL;           // Language-speficic
    TMPKHash * pHash = GetHashEntry(ha, szFileName);

    if(pHash != NULL)
    {
        TMPKHash * pHashStart = pHash;
        DWORD dwName1 = pHash->dwName1;
        DWORD dwName2 = pHash->dwName2;

        while(pHash->dwBlockIndex != HASH_ENTRY_FREE)
        {
            if(pHash->dwName1 == dwName1 && pHash->dwName2 == dwName2)
            {
                if(pHash->lcLocale == LANG_NEUTRAL)
                    pHash0 = pHash;
                if(pHash->lcLocale == lcLocale)
                    pHashX = pHash;

                // If both found, break the loop
                if(pHash0 != NULL && pHashX != NULL)
                    break;
            }

            if(++pHash >= pHashEnd)
                pHash = ha->pHashTable;
            if(pHash == pHashStart)
                return NULL;
        }

        if(lcLocale != LANG_NEUTRAL && pHashX != NULL)
            return pHashX;
        if(pHash0 != NULL)
            return pHash0;
        return NULL;
    }

    return pHash;
}

// Encrypts file name and gets the hash entry
// Returns the hash pointer, which is always within the allocated array
TMPKHash * FindFreeHashEntry(TMPKArchive * ha, const char * szFileName)
{
    TMPKHash * pHashEnd = ha->pHashTable + ha->pHeader->dwHashTableSize;
    TMPKHash * pHash0;                    // File hash entry (search start)
    TMPKHash * pHash;                     // File hash entry
    DWORD dwIndex = DecryptHashIndex(ha, szFileName);
    DWORD dwName1 = DecryptName1(szFileName);
    DWORD dwName2 = DecryptName2(szFileName);
    DWORD dwBlockIndex = 0xFFFFFFFF;

    // Save the starting hash position
    pHash = pHash0 = ha->pHashTable + dwIndex;

    // Look for the first free hash entry. Can be also a deleted entry
    while(pHash->dwBlockIndex < HASH_ENTRY_DELETED)
    {
        if(++pHash >= pHashEnd)
            pHash = ha->pHashTable;
        if(pHash == pHash0)
            return NULL;
    }

    // Fill the hash entry with the informations about the file name
    pHash->dwName1  = dwName1;
    pHash->dwName2  = dwName2;
    pHash->lcLocale = lcLocale;

    // Now we have to find a free block entry
    for(dwIndex = 0; dwIndex < ha->pHeader->dwBlockTableSize; dwIndex++)
    {
        TMPKBlock * pBlock = ha->pBlockTable + dwIndex;

        if((pBlock->dwFlags & MPK_FILE_EXISTS) == 0)
        {
            dwBlockIndex = dwIndex;
            break;
        }
    }

    // If no free block entry found, we have to use the index
    // at the end of the current block table
    if(dwBlockIndex == 0xFFFFFFFF)
        dwBlockIndex = ha->pHeader->dwBlockTableSize;
    pHash->dwBlockIndex = dwBlockIndex;
    return pHash;
}

//-----------------------------------------------------------------------------
// Checking for valid archive handle and valid file handle

BOOL IsValidMpkHandle(TMPKArchive * ha)
{
    if(ha == NULL || IsBadReadPtr(ha, sizeof(TMPKArchive)))
        return FALSE;
    if(ha->pHeader == NULL || IsBadReadPtr(ha->pHeader, sizeof(TMPKHeader)))
        return FALSE;
    
    return (ha->pHeader->dwID == ID_MPK || ha->pHeader->dwID == ID_MPQ);
}

BOOL IsValidFileHandle(TMPKFile * hf)
{
    if(hf == NULL || IsBadReadPtr(hf, sizeof(TMPKFile)))
        return FALSE;

    if(hf->hFile != INVALID_HANDLE_VALUE)
        return TRUE;

    return IsValidMpkHandle(hf->ha);
}

#define DEFAULT_CMP    0x02
//#define DEFAULT_CMP    0x08       // Hellfire only

static int uWaveCmpLevel[] = {-1, 4, 2};
static int uWaveCmpType[] = {DEFAULT_CMP, 0x81, 0x81};

// This function writes a local file into the MPK archive.
// Returns 0 if OK, otherwise error code.
int AddFileToArchive(TMPKArchive * ha, HANDLE hFile, const char * szArchivedName, DWORD dwFlags, DWORD dwQuality, int nFileType, BOOL * pbReplaced)
{
    LARGE_INTEGER FilePos = {0};
    TMPKHash  * pHash   = NULL;         // Entry in the hash table
    TMPKBlock * pBlock  = NULL;         // Entry in the block table
    DWORD * pdwBlockPos = NULL;         // Block position table (compressed files only)
    BYTE  * pbFileData = NULL;          // Uncompressed (source) data
    BYTE  * pbCompressed = NULL;        // Compressed (target) data
    BYTE  * pbToWrite = NULL;           // Data to write to the file
    DWORD   dwSeed1   = 0;              // Encryption seed
    DWORD   nBlocks   = 0;              // Number of file blocks
    DWORD   dwBytes   = 0;              // Number of bytes
    DWORD   dwTransferred = 0;          // Number of bytes written into archive file
    DWORD   nBlock = 0;                 // Index of the currently written block
    BOOL    bReplaced = FALSE;          // TRUE if replaced, FALSE if added
    int     nCmpFirst = DEFAULT_CMP;    // Compression for the first data block
    int     nCmpNext  = DEFAULT_CMP;    // Compression for the next data blocks
    int     nCmp      = DEFAULT_CMP;    // Current compression
    int     nCmpLevel = -1;             // Compression level
    int     nError = ERROR_SUCCESS;

    // Set the right compression types
    if(dwFlags & MPK_FILE_COMPRESS_PKWARE)
        nCmpFirst = nCmpNext = 0x08;

    if(dwFlags & MPK_FILE_COMPRESS_MULTI)
    {
        if(nFileType == SFILE_TYPE_DATA)
            nCmpFirst = nCmpNext = DEFAULT_CMP;
    
        if(nFileType == SFILE_TYPE_WAVE)
        {
            nCmpNext  = uWaveCmpType[dwQuality];
            nCmpLevel = uWaveCmpLevel[dwQuality];
        }
    }

    // Check if the file already exists in the archive
    if(nError == ERROR_SUCCESS)
    {
        if((pHash = GetHashEntryEx(ha, szArchivedName, lcLocale)) != NULL)
        {
            if(pHash->lcLocale == lcLocale)
            {
                if((dwFlags & MPK_FILE_REPLACEEXISTING) == 0)
                {
                    nError = ERROR_ALREADY_EXISTS;
                    pHash = NULL;
                }
                else
                    bReplaced = TRUE;
            }
            else
                pHash = NULL;
        }

        if(nError == ERROR_SUCCESS && pHash == NULL)
        {
            pHash = FindFreeHashEntry(ha, szArchivedName);
            if(pHash == NULL)
                nError = ERROR_HANDLE_DISK_FULL;
        }
    }

    // Get the block table entry for the file
    if(nError == ERROR_SUCCESS)
    {
        DWORD dwFileSize = GetFileSize(hFile, NULL);

        // Fix the flags, if the file is too small
        if(dwFileSize < 0x04)
            dwFlags &= ~(MPK_FILE_ENCRYPTED | MPK_FILE_FIXSEED);
        if(dwFileSize < 0x20)
            dwFlags &= ~MPK_FILE_COMPRESSED;

        if(pHash->dwBlockIndex == (DWORD)-1)
            pHash->dwBlockIndex = ha->pHeader->dwBlockTableSize;

        // The block table index cannot be larger than hash table size
        if(pHash->dwBlockIndex >= ha->pHeader->dwHashTableSize)
            nError = ERROR_HANDLE_DISK_FULL;
    }

    // The file will be stored after the end of the last archived file
    // (i.e. at old position of archived file
    if(nError == ERROR_SUCCESS)
    {
        TMPKBlock * pBlockEnd = ha->pBlockTable + ha->pHeader->dwBlockTableSize;
        DWORD dwFilePos = sizeof(TMPKHeader) + ha->dwMpkPos;
        const char * szTemp = strrchr(szArchivedName, '\\');

        // Find the position of the last file. It has to be after the last archived file
        // (Do not use the dwArchiveSize here, because it may or may not
        // include the hash table at the end of the file
        for(pBlock = ha->pBlockTable; pBlock < pBlockEnd; pBlock++)
        {
            if(pBlock->dwFlags & MPK_FILE_EXISTS)
            {
                if((pBlock->dwFilePos + pBlock->dwCSize) > dwFilePos)
                    dwFilePos = pBlock->dwFilePos + pBlock->dwCSize;
            }
        }

        pBlock            = ha->pBlockTable + pHash->dwBlockIndex;
        pBlock->dwFilePos = dwFilePos;
        pBlock->dwFSize   = GetFileSize(hFile, NULL);
        pBlock->dwFlags   = dwFlags | MPK_FILE_EXISTS;

        // Create seed1 for file encryption
        if(szTemp != NULL)
            szArchivedName = szTemp + 1;

        if(pBlock->dwFlags & MPK_FILE_ENCRYPTED)
        {
            dwSeed1 = DecryptFileSeed(szArchivedName);
            if(dwFlags & MPK_FILE_FIXSEED)
                dwSeed1 = (dwSeed1 + (pBlock->dwFilePos - ha->dwMpkPos)) ^ pBlock->dwFSize;
        }
    }

    // Allocate buffer for the input data
    if(nError == ERROR_SUCCESS)
    {
        nBlocks = (pBlock->dwFSize / ha->dwBlockSize) + 1;
        if(pBlock->dwFSize % ha->dwBlockSize)
            nBlocks++;

        pBlock->dwCSize = 0;
        if((pbFileData = ALLOCMEM(BYTE, ha->dwBlockSize)) == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
        pbToWrite = pbFileData;
    }

    // Allocate buffers for the compressed data
    if(nError == ERROR_SUCCESS && (pBlock->dwFlags & MPK_FILE_COMPRESSED))
    {
        pdwBlockPos  = ALLOCMEM(DWORD, nBlocks);
        pbCompressed = ALLOCMEM(BYTE, ha->dwBlockSize * 2);
        if(pdwBlockPos == NULL || pbCompressed == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
        pbToWrite = pbCompressed;
    }

    // Set the file position to the point where the file will be stored
    if(nError == ERROR_SUCCESS)
    {
        // Set the file pointer to hash table position
        if(ha->dwFilePos != pBlock->dwFilePos)
        {
            FilePos.QuadPart = pBlock->dwFilePos;
            ha->dwFilePos = SetFilePointer(ha->hFile, FilePos.LowPart, &FilePos.HighPart, FILE_BEGIN);
        }
    }

    // Write block positions (if the file will be compressed)
    if(nError == ERROR_SUCCESS && (pBlock->dwFlags & MPK_FILE_COMPRESSED))
    {
        dwBytes = nBlocks * sizeof(DWORD);

        memset(pdwBlockPos, 0, dwBytes);
        pdwBlockPos[0] = dwBytes;
        
        // Write the block positions
        CONVERTBUFFERTOLITTLEENDIAN32BITS((unsigned long *)pdwBlockPos, nBlocks);
        WriteFile(ha->hFile, pdwBlockPos, dwBytes, &dwTransferred, NULL);
        CONVERTBUFFERTOLITTLEENDIAN32BITS((unsigned long *)pdwBlockPos, nBlocks);
        ha->dwFilePos += dwTransferred;
        if(dwTransferred == dwBytes)
            pBlock->dwCSize += dwBytes;
        else
            nError = GetLastError();
    }

    // Update the positions of hash/block table and write all blocks
    if(nError == ERROR_SUCCESS)
    {
        nCmp = nCmpFirst;

        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
        for(nBlock = 0; nBlock < nBlocks-1; nBlock++)
        {
            DWORD dwInLength  = ha->dwBlockSize;
            DWORD dwOutLength = ha->dwBlockSize;

            // Load the block from the file
            ReadFile(hFile, pbFileData, ha->dwBlockSize, &dwInLength, NULL);
            if(dwInLength == 0)
                break;

            // Compress the block, if necessary
            dwOutLength = dwInLength;
            if(pBlock->dwFlags & MPK_FILE_COMPRESSED)
            {
                // Should be enough for compression
                int nOutLength = ha->dwBlockSize * 2;
                int nCmpType = 0;

                if(pBlock->dwFlags & MPK_FILE_COMPRESS_PKWARE)
                    Compress_pklib((char *)pbCompressed, &nOutLength, (char *)pbFileData, dwInLength, &nCmpType, 0);

                if(pBlock->dwFlags & MPK_FILE_COMPRESS_MULTI)
                    SCompCompress((char *)pbCompressed, &nOutLength, (char *)pbFileData, dwInLength, nCmp, 0, nCmpLevel);

                // The compressed block size must NOT be the same or greater like
                // the original block size. If yes, do not compress the block
                // and store the data as-is.
                if(nOutLength >= (int)dwInLength)
                {
                    memcpy(pbCompressed, pbFileData, dwInLength);
                    nOutLength = dwInLength;
                }

                // Update block positions
                dwOutLength = nOutLength;
                pdwBlockPos[nBlock+1] = pdwBlockPos[nBlock] + dwOutLength;
                nCmp = nCmpNext;
            }

            // Encrypt the block, if necessary
            if(pBlock->dwFlags & MPK_FILE_ENCRYPTED)
            {
                CONVERTBUFFERTOLITTLEENDIAN32BITS((unsigned long *)pbToWrite, dwOutLength / sizeof(DWORD));
                EncryptMPKBlock((DWORD *)pbToWrite, dwOutLength, dwSeed1 + nBlock);
                CONVERTBUFFERTOLITTLEENDIAN32BITS((unsigned long *)pbToWrite, dwOutLength / sizeof(DWORD));
            }
            
            // Write the block
            WriteFile(ha->hFile, pbToWrite, dwOutLength, &dwTransferred, NULL);
            if(dwTransferred != dwOutLength)
            {
                nError = ERROR_DISK_FULL;
                break;
            }
            pBlock->dwCSize += dwOutLength;
        }
    }

    // Now save the block positions
    if(nError == ERROR_SUCCESS && (pBlock->dwFlags & MPK_FILE_COMPRESSED))
    {
        dwBytes = nBlocks * sizeof(DWORD);

        // If file is encrypted, block positions are also encrypted
        if(pBlock->dwFlags & MPK_FILE_ENCRYPTED)
            EncryptMPKBlock(pdwBlockPos, dwBytes, dwSeed1 - 1);
        
        FilePos.QuadPart = pBlock->dwFilePos;
        SetFilePointer(ha->hFile, FilePos.LowPart, &FilePos.HighPart, FILE_BEGIN);
        
        CONVERTBUFFERTOLITTLEENDIAN32BITS((unsigned long *)pdwBlockPos, nBlocks);
        WriteFile(ha->hFile, pdwBlockPos, dwBytes, &dwTransferred, NULL);
        if(dwTransferred != dwBytes)
            nError = ERROR_CAN_NOT_COMPLETE;
    }

    // If success, we have to change the settings
    // in MPK header. If failed, we have to clean hash entry
    // Update all MPK informations
    if(nError == ERROR_SUCCESS)
    {
        ha->pLastFile  = NULL;
        ha->dwBlockPos = 0;
        ha->dwBuffPos  = 0;
        ha->dwFilePos  = SetFilePointer(ha->hFile, 0, NULL, FILE_CURRENT);
        ha->dwFlags   |= MPK_FLAG_CHANGED;

        ha->pHeader->dwHashTablePos  = pBlock->dwFilePos + pBlock->dwCSize;
        ha->pHeader->dwBlockTablePos = ha->pHeader->dwHashTablePos + (ha->pHeader->dwHashTableSize * sizeof(TMPKHash));
        ha->pHeader->dwArchiveSize   = ha->pHeader->dwBlockTablePos + (ha->pHeader->dwBlockTableSize * sizeof(TMPKBlock)) - ha->dwMpkPos;
        if(pHash->dwBlockIndex >= ha->pHeader->dwBlockTableSize)
        {
            ha->pHeader->dwArchiveSize += sizeof(TMPKBlock);
            ha->pHeader->dwBlockTableSize++;
        }
    }
    else
    {
        // Clear the hash table entry
        if(pHash != NULL)
            memset(pHash, 0xFF, sizeof(TMPKHash));
    }

    // Cleanup
    if(pbCompressed != NULL)
        FREEMEM(pbCompressed);
    if(pdwBlockPos != NULL)
        FREEMEM(pdwBlockPos);
    if(pbFileData != NULL)
        FREEMEM(pbFileData);
    if(pbReplaced != NULL)
        *pbReplaced = bReplaced;
    return nError;
}

// This method saves MPK header, hash table and block table.
int SaveMPKTables(TMPKArchive * ha)
{
    LARGE_INTEGER FilePos;
    BYTE * pbBuffer = NULL;
    DWORD dwBytes;
    DWORD dwWritten;
    DWORD dwBuffSize = max(ha->pHeader->dwHashTableSize, ha->pHeader->dwBlockTableSize);
    int   nError = ERROR_SUCCESS;

    // Allocate buffer for encrypted tables
    if(nError == ERROR_SUCCESS)
    {
        // Allocate temporary buffer for tables encryption
        pbBuffer = ALLOCMEM(BYTE, sizeof(TMPKHash) * dwBuffSize);
        if(pbBuffer == NULL)
            nError = ERROR_NOT_ENOUGH_MEMORY;
    }

    // Write the MPK Header
    if(nError == ERROR_SUCCESS)
    {
        TMPKHeader hdr;

        // Copy the MPK header
        hdr = *ha->pHeader;

        // Relocate the variables from the header
        hdr.dwHashTablePos  -= ha->dwMpkPos;
        hdr.dwBlockTablePos -= ha->dwMpkPos;

        // Write the MPK header
        FilePos.QuadPart = ha->dwMpkPos;
        SetFilePointer(ha->hFile, FilePos.LowPart, &FilePos.HighPart, FILE_BEGIN);

        // Convert to littleendian for file save
        CONVERTTMPKHEADERTOLITTLEENDIAN(&hdr);
        WriteFile(ha->hFile, &hdr, sizeof(TMPKHeader), &dwWritten, NULL);
        if(dwWritten != sizeof(TMPKHeader))
            nError = ERROR_DISK_FULL;
    }

    // Write the hash table
    if(nError == ERROR_SUCCESS)
    {
        dwBytes = ha->pHeader->dwHashTableSize * sizeof(TMPKHash);
        memcpy(pbBuffer, ha->pHashTable, dwBytes);
        //EncryptHashTable((DWORD *)pbBuffer, (BYTE *)"(hash table)", dwBytes >> 2);
		//更换加密的key
		EncryptHashTable((DWORD *)pbBuffer, (BYTE *)"Ik46lh&^#$$j", dwBytes >> 2);
        FilePos.QuadPart = ha->pHeader->dwHashTablePos;
        SetFilePointer(ha->hFile, FilePos.LowPart, &FilePos.HighPart, FILE_BEGIN);

        // Convert to littleendian for file save
        CONVERTBUFFERTOLITTLEENDIAN32BITS((unsigned long *)pbBuffer, dwBytes / sizeof(unsigned long));
        WriteFile(ha->hFile, pbBuffer, dwBytes, &dwWritten, NULL);
        if(dwWritten != dwBytes)
            nError = ERROR_DISK_FULL;
    }

    // Write the block table
    if(nError == ERROR_SUCCESS)
    {
        TMPKBlock * pBlock = (TMPKBlock *)pbBuffer;
        DWORD dwCount;

        dwBytes = ha->pHeader->dwBlockTableSize * sizeof(TMPKBlock);
        memcpy(pBlock, ha->pBlockTable, dwBytes);

        // Relocate block table to be relative from the begin of MPK, not from the begin of the file
        for(dwCount = 0; dwCount < ha->pHeader->dwBlockTableSize; dwCount++, pBlock++)
        {
            if(pBlock->dwFlags & MPK_FILE_EXISTS)
                pBlock->dwFilePos -= ha->dwMpkPos;
        }

        // Encrypt the block table and write it to the file
		//更换加密的key
        //EncryptBlockTable((DWORD *)pbBuffer, (BYTE *)"(block table)", dwBytes >> 2);
		EncryptBlockTable((DWORD *)pbBuffer, (BYTE *)"9D5.ss#&(!mNy", dwBytes >> 2);
        FilePos.QuadPart = ha->pHeader->dwBlockTablePos;
        SetFilePointer(ha->hFile, FilePos.LowPart, &FilePos.HighPart, FILE_BEGIN);

        // Convert to littleendian for file save
        CONVERTBUFFERTOLITTLEENDIAN32BITS((DWORD *)pbBuffer, dwBytes / sizeof(DWORD));
        WriteFile(ha->hFile, pbBuffer, dwBytes, &dwWritten, NULL);
        if(dwWritten != dwBytes)
            nError = ERROR_DISK_FULL;
    }

    // Set end of file here
    if(nError == ERROR_SUCCESS)
    {
        FilePos.QuadPart = ha->pHeader->dwArchiveSize + ha->dwMpkPos;
        SetFilePointer(ha->hFile, FilePos.LowPart, &FilePos.HighPart, FILE_BEGIN);
        SetEndOfFile(ha->hFile);
    }

    // Cleanup and exit
    if(pbBuffer != NULL)
        FREEMEM(pbBuffer);
    return nError;
}

// Frees the MPK archive
void FreeMPKArchive(TMPKArchive *& ha)
{
    if(ha != NULL)
    {
        FREEMEM(ha->pbBlockBuffer);
        FREEMEM(ha->pBlockTable);
        FREEMEM(ha->pHashTable);
        if(ha->pListFile != NULL)
            SListFileFreeListFile(ha);

        if(ha->hFile != INVALID_HANDLE_VALUE)
            CloseHandle(ha->hFile);
        FREEMEM(ha);
        ha = NULL;
    }
}
