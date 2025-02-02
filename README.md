# CKeylogger (Projet Éducatif)
  
> Ce projet est uniquement à des fins éducatives et de sensibilisation à la sécurité informatique. L’utilisation de ce keylogger à des fins malveillantes est strictement interdite et peut entraîner des conséquences légales.

## Fonctionnalités

- Capture des frappes clavier en temps réel

- Exécutables pour Linux & Windows

- Récupère les fenêtres & Liens visités 

- Stockage des données dans un fichier local

## Installation

***Prérequis***

> Aucun prérequis nécessaire pour utiliser les exécutables fournis.

Pour des informations sur la compilation à partir des sources, veuillez consulter les **README.md** spécifiques dans les dossiers **Linux/Source Code/** et **Windows/Source Code/**.

***Téléchargement***

Clonez le repository :
```bash
 git clone https://github.com/ton-utilisateur/CKeylogger.git
 cd CKeylogger
```
Les exécutables sont prêts à l'emploi :

**Keylogger** pour Linux  

**Meditor.exe** version portable & **MeditorInstaller** version avec setup pour Windows  

> La version Installer est designée spécifiquement pour comprendre une partie de comment les hackers/entreprises évadent les anti-virus avec certificats autosignés, fichers resources, manifest, logo, config, etc...   
  
## Utilisation  

***Pour Linux :***

```bash
 ./Keylogger
```

***Pour Windows :***

Comme n'importe quel programme double-cliquez sur l'exécutable ça vous génèrera un fichier log lorsque vous fermerez l'éditeur de texte dans **C:\Users\votrenom\Documents\debug_log.txt**.

Pour le **MeditorInstaller.exe** faites le setup comme sur n'importe quel autre programme. Cela vous créera un répertoire **Professional Suite Software** dans **C:\Programs\\**.  

## Structure du projet  

```bash
CKeylogger/
├── Linux/
│   ├── Keylogger (exécutable Linux)
│   ├── Source Code/
│   │   └── Keylogger.c
│   └── README.md
├── Windows/
│   ├── Meditor.exe (exécutable Windows)
│   ├── MeditorInstaller.exe (installateur Windows)
│   ├── Source Code/
│   │   └── * (fichiers sources pour Windows)
│   └── README.md
├── README.md
```

## Avertissement Légal

Ce keylogger est développé à des fins éducatives uniquement. L’utilisation de ce logiciel pour enregistrer des frappes clavier à l’insu des utilisateurs sans leur consentement est illégale et peut entraîner des poursuites judiciaires.

L’auteur décline toute responsabilité en cas d’utilisation abusive de ce projet.

## Crédit 

Ce projet a été réalisé par margoul1

N’hésitez pas à ouvrir des issues ou proposer des pull requests pour améliorer ce projet !

# Contributions  

Si jamais ce logiciel vous a plus ^^ vous pouvez [me soutenir ici](https://buymeacoffee.com/margoul1n)

