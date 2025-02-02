#ifndef COMMON_H
#define COMMON_H

#include <windows.h>
#include <stdio.h>

// Fonction de chiffrement pour les données sensibles
void encrypt_data(unsigned char* data, size_t len);

// Fonction de déchiffrement
void decrypt_data(unsigned char* data, size_t len);

// Fonction de vérification d'intégrité
BOOL verify_integrity(void);

// Fonction de surveillance
void process_activity(void);

#endif 