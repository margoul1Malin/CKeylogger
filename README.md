CKeylogger (Projet Éducatif)

Ce projet est uniquement à des fins éducatives et de sensibilisation à la sécurité informatique. L’utilisation de ce keylogger à des fins malveillantes est strictement interdite et peut entraîner des conséquences légales.

Fonctionnalités

Capture des frappes clavier en temps réel

Compatible Linux et Windows

Stockage des données dans un fichier local

Code simple et modulaire pour l'apprentissage

Installation

Prérequis

Aucun prérequis nécessaire pour utiliser les exécutables fournis.

Pour des informations sur la compilation à partir des sources, veuillez consulter les README.md spécifiques dans les dossiers keylogger_linux et keylogger_windows.

Téléchargement

Clonez le repository :

 git clone https://github.com/ton-utilisateur/CKeylogger.git
 cd CKeylogger

Les exécutables sont prêts à l'emploi :

ckeylogger pour Linux

ckeylogger.exe pour Windows

Utilisation

Pour Linux :

 ./ckeylogger

Pour Windows :

 ckeylogger.exe

Les frappes seront enregistrées dans un fichier log.txt situé dans le même répertoire que l’exécutable.

Structure du Projet

CKeylogger/
├── keylogger_linux/
│   └── keylogger_linux.c
│   └── README.md
├── keylogger_windows/
│   └── keylogger_windows.c
│   └── README.md
├── ckeylogger (exécutable Linux)
├── ckeylogger.exe (exécutable Windows)
├── README.md
└── log.txt  # Fichier généré avec les frappes

Avertissement Légal

Ce keylogger est développé à des fins éducatives uniquement. L’utilisation de ce logiciel pour enregistrer des frappes clavier à l’insu des utilisateurs sans leur consentement est illégale et peut entraîner des poursuites judiciaires.

L’auteur décline toute responsabilité en cas d’utilisation abusive de ce projet.

Crédit

Ce projet a été réalisé par Ton Nom ou Pseudo.

N’hésitez pas à ouvrir des issues ou proposer des pull requests pour améliorer ce projet !

Licence

Distribué sous la licence MIT. Voir le fichier LICENSE pour plus d’informations.

