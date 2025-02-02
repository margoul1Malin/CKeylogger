!include "MUI2.nsh"
!include "FileFunc.nsh"

# Définitions
!define APPNAME "Modern Editor Pro"
!define COMPANYNAME "Professional Software Solutions"
!define DESCRIPTION "Professional Text Editor"
!define VERSION "1.5.2"

# Informations générales
Name "${APPNAME}"
OutFile "meditorinstaller.exe"
InstallDir "$PROGRAMFILES64\${COMPANYNAME}\${APPNAME}"
RequestExecutionLevel admin

# Interface simplifiée (sans images)
!define MUI_ICON "app.ico"
!define MUI_UNICON "app.ico"

# Pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "license.txt"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

# Langue
!insertmacro MUI_LANGUAGE "English"

Section "MainSection" SEC01
    SetOutPath "$INSTDIR"
    SetOverwrite try
    
    # Fichiers principaux
    File "Meditor.exe"
    File "config.ini"
    File "license.txt"
    File "app.ico"
    
    # Création des raccourcis
    CreateDirectory "$SMPROGRAMS\${COMPANYNAME}"
    CreateShortcut "$SMPROGRAMS\${COMPANYNAME}\${APPNAME}.lnk" "$INSTDIR\meditor_signed.exe"
    CreateShortcut "$DESKTOP\${APPNAME}.lnk" "$INSTDIR\meditor_signed.exe"
    
    # Registre Windows
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "DisplayName" "${APPNAME}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "UninstallString" "$INSTDIR\uninstall.exe"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "DisplayIcon" "$INSTDIR\app.ico"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "Publisher" "${COMPANYNAME}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "DisplayVersion" "${VERSION}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "EstimatedSize" "8192"
    
    # Association de fichiers
    WriteRegStr HKCR ".txt" "" "ModernEditor.Document"
    WriteRegStr HKCR "ModernEditor.Document" "" "Modern Editor Document"
    WriteRegStr HKCR "ModernEditor.Document\DefaultIcon" "" "$INSTDIR\app.ico"
    WriteRegStr HKCR "ModernEditor.Document\shell\open\command" "" '"$INSTDIR\meditor_signed.exe" "%1"'
    
    WriteUninstaller "$INSTDIR\uninstall.exe"
SectionEnd

Section "Uninstall"
    Delete "$INSTDIR\meditor_signed.exe"
    Delete "$INSTDIR\config.ini"
    Delete "$INSTDIR\license.txt"
    Delete "$INSTDIR\app.ico"
    Delete "$INSTDIR\uninstall.exe"
    
    Delete "$SMPROGRAMS\${COMPANYNAME}\${APPNAME}.lnk"
    Delete "$DESKTOP\${APPNAME}.lnk"
    RMDir "$SMPROGRAMS\${COMPANYNAME}"
    RMDir "$INSTDIR"
    
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}"
    DeleteRegKey HKCR ".txt"
    DeleteRegKey HKCR "ModernEditor.Document"
SectionEnd 