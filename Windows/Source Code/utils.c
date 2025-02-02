#include "common.h"
#include <wincrypt.h>

// Clé de chiffrement (à changer)
static const unsigned char KEY[] = {
    0x89, 0x45, 0x91, 0xAB, 0xCD, 0xEF, 0x12, 0x34,
    0x56, 0x78, 0x90, 0xAB, 0xCD, 0xEF, 0x12, 0x34
};

void encrypt_data(unsigned char* data, size_t len) {
    for(size_t i = 0; i < len; i++) {
        data[i] ^= KEY[i % sizeof(KEY)];
    }
}

void decrypt_data(unsigned char* data, size_t len) {
    encrypt_data(data, len);  // XOR est réversible
}

BOOL verify_integrity(void) {
    WCHAR path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    
    HANDLE hFile = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, 
                             NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if(hFile == INVALID_HANDLE_VALUE) return FALSE;
    
    CloseHandle(hFile);
    return TRUE;
} 