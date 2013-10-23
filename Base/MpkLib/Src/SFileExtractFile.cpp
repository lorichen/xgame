#define __MPKLIB_SELF__
#include "MpkLib.h"
#include "SCommon.h"

BOOL WINAPI SFileExtractFile(HANDLE hMpk, const char * szToExtract, const char * szExtracted)
{
    HANDLE hLocalFile = INVALID_HANDLE_VALUE;
    HANDLE hMpkFile = NULL;
    int nError = ERROR_SUCCESS;

    // Create the local file
    if(nError == ERROR_SUCCESS)
    {
        hLocalFile = CreateFile(szExtracted, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
        if(hLocalFile == INVALID_HANDLE_VALUE)
            nError = GetLastError();
    }

    // Open the MPK file
    if(nError == ERROR_SUCCESS)
    {
        if(!SFileOpenFileEx(hMpk, szToExtract, 0, &hMpkFile))
            nError = GetLastError();
    }

    // Copy the file's content
    if(nError == ERROR_SUCCESS)
    {
        char  szBuffer[0x1000];
        DWORD dwTransferred = 1;

        while(dwTransferred > 0)
        {
            SFileReadFile(hMpkFile, szBuffer, sizeof(szBuffer), &dwTransferred, NULL);
            if(dwTransferred == 0)
                break;

            WriteFile(hLocalFile, szBuffer, dwTransferred, &dwTransferred, NULL);
            if(dwTransferred == 0)
                break;
        }
    }

    // Close the files
    if(hMpkFile != NULL)
        SFileCloseFile(hMpkFile);
    if(hLocalFile != INVALID_HANDLE_VALUE)
        CloseHandle(hLocalFile);
    if(nError != ERROR_SUCCESS)
        SetLastError(nError);
    return (BOOL)(nError == ERROR_SUCCESS);
}
