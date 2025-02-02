# Linux Keylogger

Un keylogger avancé pour Linux avec support AZERTY/QWERTY et détection des fenêtres actives.

![Version](https://img.shields.io/badge/version-1.0.0-blue)
![License](https://img.shields.io/badge/license-MIT-green)

## 📋 Fonctionnalités

- Détection automatique de la disposition du clavier (AZERTY/QWERTY)
- Support complet des caractères spéciaux et touches de fonction
- Détection des fenêtres actives et URLs des navigateurs
- Support des principaux navigateurs :
  - Firefox
  - Chrome
  - Opera
  - Edge
  - Safari
  - Tor Browser
- Logging détaillé avec horodatage
- Gestion des modificateurs (Shift, AltGr, etc.)

## 🚀 Installation

### Prérequis
bash
sudo apt-get install libudev-dev libx11-dev

### Compilation
bash
gcc -o keylogger example.c -ludev -lX11

## 💡 Utilisation

bash
sudo ./keylogger

Le programme créera un fichier `log.txt` qui contiendra :
- Les touches pressées
- Les fenêtres actives
- Les URLs visitées

## ⚙️ Configuration

Le programme détecte automatiquement :
- La disposition du clavier
- Les périphériques d'entrée
- Les fenêtres actives

## 🔒 Sécurité

**Attention** : Ce programme est destiné à des fins éducatives uniquement. Son utilisation pour espionner des utilisateurs sans leur consentement est illégale.

## 🤝 Contribution

Les contributions sont les bienvenues ! N'hésitez pas à :
- Signaler des bugs
- Proposer des améliorations
- Soumettre des pull requests

## 📄 Licence

Ce projet est sous licence MIT. Voir le fichier [LICENSE](LICENSE) pour plus de détails.

## 🛠️ Dépendances

- libudev
- libX11
- Linux (kernel > 2.6)
