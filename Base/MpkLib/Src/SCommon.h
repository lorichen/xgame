#ifndef __SCOMMON_H__
#define __SCOMMON_H__

//-----------------------------------------------------------------------------
// MpkLib private defines

#define SFILE_TYPE_DATA  0              // Process the file as data file
#define SFILE_TYPE_WAVE  1              // Process the file as WAVe file

//-----------------------------------------------------------------------------
// External variables

extern TMPKArchive * pFirstOpen;
extern LCID          lcLocale;

//-----------------------------------------------------------------------------
// Encryption and decryption functions

int   PrepareStormBuffer();

void  EncryptHashTable(DWORD * pdwTable, BYTE * pbKey, DWORD dwLength);
void  DecryptHashTable(DWORD * pdwTable, BYTE * pbKey, DWORD dwLength);
TMPKHash * FindFreeHashEntry(TMPKArchive * ha, const char * szFileName);

void  EncryptBlockTable(DWORD * pdwTable, BYTE * pbKey, DWORD dwLength);
void  DecryptBlockTable(DWORD * pdwTable, BYTE * pbKey, DWORD dwLength);

DWORD DetectFileSeed(DWORD * block, DWORD decrypted);
DWORD DetectFileSeed2(DWORD * block, UINT nDwords, ...);
void  EncryptMPKBlock(DWORD * pdwBlock, DWORD dwLength, DWORD dwSeed1);
void  DecryptMPKBlock(DWORD * pdwBlock, DWORD dwLength, DWORD dwSeed1);

DWORD DecryptHashIndex(TMPKArchive * ha, const char * szFileName);
DWORD DecryptName1    (const char * szFileName);
DWORD DecryptName2    (const char * szFileName);
DWORD DecryptFileSeed (const char * szFileName);

TMPKHash * GetHashEntry  (TMPKArchive * ha, const char * szFileName);
TMPKHash * GetHashEntryEx(TMPKArchive * ha, const char * szFileName, LCID lcLocale);

//-----------------------------------------------------------------------------
// Compression and decompression functions

int Compress_pklib  (char * pbOutBuffer, int * pdwOutLength, char * pbInBuffer, int dwInLength, int * pCmpType, int nCmpLevel);
int Decompress_pklib(char * pbOutBuffer, int * pdwOutLength, char * pbInBuffer, int dwInLength);

//-----------------------------------------------------------------------------
// Checking functions

BOOL IsValidMpkHandle(TMPKArchive * ha);
BOOL IsValidFileHandle(TMPKFile * hf);

//-----------------------------------------------------------------------------
// Other functions

BOOL SFileOpenArchiveEx(const char * szMpkName, DWORD dwPriority, DWORD dwFlags, HANDLE * phMPK, DWORD dwAccessMode = GENERIC_READ);
int  AddFileToArchive(TMPKArchive * ha, HANDLE hFile, const char * szArchivedName, DWORD dwFlags, DWORD dwQuality, int nFileType, BOOL * pbReplaced);
int  SaveMPKTables(TMPKArchive * ha);
void FreeMPKArchive(TMPKArchive *& ha);

BOOL CheckWildCard(const char * szString, const char * szWildCard);

//-----------------------------------------------------------------------------
// Listfile functions

int  SListFileCreateListFile(TMPKArchive * ha);
int  SListFileAddNode(TMPKArchive * ha, const char * szAddedFile);
int  SListFileAddNode(TMPKArchive * ha, const char * szAddedFile, const FILETIME &ftCreateTime, const FILETIME &ftLastWriteTime);
int  SListFileRemoveNode(TMPKArchive * ha, const char * szFileName);
int  SListFileRenameNode(TMPKArchive * ha, const char * szOldFileName, const char * szNewFileName);
int  SListFileFreeListFile(TMPKArchive * ha);

int  SListFileSaveToMpk(TMPKArchive * ha);

#endif // __SCOMMON_H__

