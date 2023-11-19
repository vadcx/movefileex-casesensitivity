#include <windows.h>
#include <string>
#include "main.h"

int main(int argc, char **argv) {
    bool status;
    printf("Hello, world\n");

    int tempBufLen = MAX_PATH+1;
    char tempPath[tempBufLen];

    if (argc <= 1) {
        int tempPathLen = GetTempPath(tempBufLen, (TCHAR*)tempPath);
        if (tempPathLen > tempBufLen) {
            printf("Could not receive temp path, allocated: %d, needed: %d\n", tempBufLen, tempPathLen);
            return 1;
        }
    } else {
        printf("Temp path specified by command line: '%s'\n", argv[argc - 1]);
        strcpy(tempPath, argv[argc - 1]); // override
    }
    printf("Temp path: '%s'\n", tempPath);
    

    char const *sourceName = "sourcefile.txt";
    char const *lowerName = "case.txt";
    char const *upperName = "CASE.txt";
    char *sourcePath = (char*) malloc(strlen(sourceName) + strlen(tempPath) + 1);
    strcpy(sourcePath, tempPath); 
    strcat(sourcePath, sourceName);

    char *lowerPath = (char*) malloc(strlen(lowerName) + strlen(tempPath) + 1);
    strcpy(lowerPath, tempPath); 
    strcat(lowerPath, lowerName);

    char *upperPath = (char*) malloc(strlen(upperName) + strlen(tempPath) + 1);
    strcpy(upperPath, tempPath); 
    strcat(upperPath, upperName);

    // delete after previous run if exists
    DeleteFile(upperPath);

    printf("Creating temporary source file: '%s'\n", sourcePath);
    HANDLE sourceFile = CreateFile(
        sourcePath,
        GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
        );
    
    if (sourceFile == INVALID_HANDLE_VALUE) {
        ShowLastErrorMsg();
        return 1;
    }

    printf("Moving file from '%s' to '%s'\n", sourcePath, lowerPath);
    status = MoveFileEx(sourcePath, lowerPath, 0);
    if (!status) {
        ShowLastErrorMsg();
        return 1;
    }
/**
    printf("Changing capitalization from '%s' to '%s'\n", lowerPath, upperPath);
    status = MoveFileEx(lowerPath, upperPath, 0);
    if (!status) {
        ShowLastErrorMsg();
        return 1;
    }
*/
    HANDLE caseFile = CreateFile(
        upperPath,
        GENERIC_READ | DELETE,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_POSIX_SEMANTICS,
        NULL
        );
    if (caseFile == INVALID_HANDLE_VALUE) {
        ShowLastErrorMsg();
        return 1;
    }

    puts("Opening the resulting case file...");
    LPBY_HANDLE_FILE_INFORMATION caseInfo;
    status = GetFileInformationByHandle(caseFile, caseInfo);
    if (!status) {
        ShowLastErrorMsg();
        return 1;
    }

    int finalBufLen = MAX_PATH+1;
    TCHAR caseFileFinalPath[finalBufLen];
    DWORD statusD;

    statusD = GetFinalPathNameByHandle(caseFile, caseFileFinalPath, finalBufLen, FILE_NAME_NORMALIZED);
    if (statusD == 0) {
        ShowLastErrorMsg();
        return 1;
    } else if (statusD > finalBufLen) {
        printf("Buffer too small to hold final path length for upper case: %d\n", statusD);
        return 1;
    }
    printf("Resulting case file path: '%s'\n", caseFileFinalPath);

    char const *anotherPath = upperPath; //"C:\\Users\\Alex\\AppData\\Local\\Temp\\Another.txt";
    int wchars_num = MultiByteToWideChar( CP_UTF8 , 0 , anotherPath, -1, NULL , 0 );
    wchar_t* wstr = new wchar_t[wchars_num];
    int wcharsSize = sizeof(wchar_t) * wchars_num;
    MultiByteToWideChar( CP_UTF8 , 0 , anotherPath, -1, wstr, wchars_num );

    wprintf(L"Wide path is (%d chars, %d bytes): '%ls'\n", wchars_num, wcharsSize, wstr);

    BY_HANDLE_FILE_INFORMATION fi = {0};
    GetFileInformationByHandle(caseFile, &fi);

    int renameInfoSize = sizeof(FILE_RENAME_INFO) + wcharsSize + sizeof(wchar_t);
    FILE_RENAME_INFO *renameInfo = (FILE_RENAME_INFO*)malloc(renameInfoSize);
    renameInfo->ReplaceIfExists = true;
    renameInfo->RootDirectory = NULL;
    renameInfo->FileNameLength = wcharsSize;
    wchar_t *infoNewName = renameInfo->FileName;
    memcpy(renameInfo->FileName, wstr, wcharsSize);

    puts("Renaming file with SetFileInformationByHandle\n");
    status = SetFileInformationByHandle(caseFile, FileRenameInfo, (LPVOID)renameInfo, renameInfoSize);
    if (!status) {
        ShowLastErrorMsg();
        return 1;
    }

    statusD = GetFinalPathNameByHandle(caseFile, caseFileFinalPath, finalBufLen, FILE_NAME_NORMALIZED);
    if (statusD == 0) {
        ShowLastErrorMsg();
        return 1;
    } else if (statusD > finalBufLen) {
        printf("Buffer too small to hold final path length for upper case: %d\n", statusD);
        return 1;
    }
    printf("Resulting case file path: '%s'\n", caseFileFinalPath);

    CloseHandle(caseFile);

    printf("Done!\n");
    
    return 0;
}

LPCTSTR ErrorMessage(DWORD error) 
{ 
    LPVOID lpMsgBuf;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER 
        | FORMAT_MESSAGE_FROM_SYSTEM 
        | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0,
        NULL);

    return((LPCTSTR)lpMsgBuf);
}

void ShowLastErrorMsg() {
    LPCTSTR errMsg = ErrorMessage(GetLastError());
    printf("GetLastError: %s", errMsg);
	//::MessageBox( 0,errMsg, "GetLastError", MB_OK );
    LocalFree((HLOCAL)errMsg);
}


