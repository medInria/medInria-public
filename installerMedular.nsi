;NSIS Modern User Interface
;Script to generate a win32 installer

;-----------------------------------
; Basic includes
    !include "MUI.nsh"
;-----------------------------------

;General

  ;Name and file
  Name "${PROJECT_NAME}"
  !ifdef WIN64
    OutFile "${medularDIR}\${PROJECT_NAME}-${VERSION}-win32-x64.exe"
  !else
    OutFile "${medularDIR}\${PROJECT_NAME}-${VERSION}-win32-x86.exe"
  !endif
  
  XPStyle on

  ;Function .onInit
  ;# the plugins dir is automatically deleted when the installer exits
  ;InitPluginsDir
  ;File /oname=$PLUGINSDIR\splash.bmp "pixmaps\logomedular.bmp"

  ;advsplash::show 1000 600 400 -1 $PLUGINSDIR\splash
  ;Pop $0 ; $0 has '1' if the user closed the splash screen early,
  ;; '0' if everything closed normally, and '-1' if some error occurred.
  ;FunctionEnd

  ;Default installation folder
  InstallDir "$PROGRAMFILES\${PROJECT_NAME}"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\${PROJECT_NAME}" ""
;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING
  !define MUI_HEADERIMAGE
  ;!define MUI_HEADERIMAGE_BITMAP "pixmaps\logomedularHeader.bmp" ; optional
;--------------------------------
;Pages

  Var STARTMENU_FOLDER
  Var MUI_TEMP
  
  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE "${SRCDIR}\license.txt"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "${PROJECT_NAME}" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  !insertmacro MUI_PAGE_STARTMENU Application $STARTMENU_FOLDER


  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH
  
  !insertmacro MUI_UNPAGE_WELCOME
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH
  

;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "${PROJECT_NAME} (required)" SecMedular  

  SectionIn RO  

  SetOutPath "$INSTDIR"

  ; Specific for medinria
  ;File "${medularDIR}\release\medular.exe"
  ;WARNING: copies everything in install directory: could be much better.
  File /r "${INST_PREFIX}\*.*" 
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
  ;Create shortcuts
  CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
  CreateShortCut  "$SMPROGRAMS\$STARTMENU_FOLDER\${PROJECT_NAME}.lnk"  "$INSTDIR\bin\${PROJECT_NAME}.exe" 
  CreateShortCut  "$DESKTOP\${PROJECT_NAME}.lnk"  "$INSTDIR\bin\${PROJECT_NAME}.exe"
  CreateShortCut  "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk"  "$INSTDIR\Uninstall.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_END
 
  ;Store installation folder
  WriteRegStr HKCU "Software\${PROJECT_NAME}" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"


;  ;File association
;  !define Index "Line${__LINE__}"
;  ReadRegStr $1 HKCR ".dts" ""
;  StrCmp $1 "" "${Index}-NoBackup"
;    StrCmp $1 "medular.dts" "${Index}-NoBackup"
;    WriteRegStr HKCR ".opt" "backup_val" $1
;  "${Index}-NoBackup:"
;  WriteRegStr HKCR ".dts" "" "medular.dts"
;  ReadRegStr $0 HKCR "medular.dts" ""
;  StrCmp $0 "" 0 "${Index}-Skip"
;	WriteRegStr HKCR "medular.dts" "" "medular DTI Study"
;	WriteRegStr HKCR "medular.dts\shell" "" "open"
;	WriteRegStr HKCR "medular.dts\DefaultIcon" "" "$INSTDIR\medular.exe,0"
;  "${Index}-Skip:"
;  WriteRegStr HKCR "medular.dts\shell\open\command" "" \
;    '$INSTDIR\medular.exe -mod dtitrack -dts "%1"'
;  WriteRegStr HKCR "medular.dts\shell\edit" "" "Edit Options File"
;  WriteRegStr HKCR "medular.dts\shell\edit\command" "" \
;    '$INSTDIR\medular.exe "%1"'
; 
;  System::Call 'Shell32::SHChangeNotify(i 0x8000000, i 0, i 0, i 0)'
;  !undef Index;


SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_Secmedular ${LANG_ENGLISH} "Select this option to install \
  the main program ${PROJECT_NAME}."
;  LangString DESC_SecTensorViewer ${LANG_ENGLISH} "Select this option to install \
;  the Tensor Viewer program."

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${Secmedular} $(DESC_Secmedular)
;    !insertmacro MUI_DESCRIPTION_TEXT ${SecTensorViewer} $(DESC_SecTensorViewer)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...
  Delete    "$INSTDIR\*.*"
  RMDir  /r "$INSTDIR"

  !insertmacro MUI_STARTMENU_GETFOLDER Application $MUI_TEMP
  Delete "$SMPROGRAMS\$MUI_TEMP\${PROJECT_NAME}.lnk"
  Delete "$SMPROGRAMS\$MUI_TEMP\Uninstall.lnk"
  RMDir "$SMPROGRAMS\$MUI_TEMP"
  RMDir /r "$LOCALAPPDATA\inria\${PROJECT_NAME}"
  RMDir "$LOCALAPPDATA\inria"
  Delete    "$DESKTOP\${PROJECT_NAME}.lnk"
;  Delete    "$DESKTOP\DTI Track.lnk"
;  Delete    "$DESKTOP\ImageViewer.lnk"
;  Delete    "$DESKTOP\LSE.lnk"
;  Delete    "$DESKTOP\LSC.lnk"
;  Delete    "$DESKTOP\ImageFusion.lnk"
;  Delete    "$DESKTOP\TensorViewer.lnk"
;  Delete    "$DESKTOP\SLViewer.lnk"
  DeleteRegKey HKCU "Software\${PROJECT_NAME}"
  DeleteRegKey HKU "Software\${PROJECT_NAME}"
  ;medular is hard coded
  DeleteRegKey HKCU "Software\inria\medular"
  ;WARNING!!!!!!!!!!!!!! do not delete if other dtk software
  DeleteRegKey HKCU "Software\inria\dtk"
  DeleteRegKey HKU "Software\inria\dtk" 
  DeleteRegKey HKU "Software\inria\medular"
  DeleteRegKey /ifempty HKU "Software\inria"
  DeleteRegKey /ifempty HKCU "Software\inria"
;  ;start of restore script
;  !define Index "Line${__LINE__}"
;  ReadRegStr $1 HKCR ".dts" ""
;  StrCmp $1 "medular.dts" 0 "${Index}-NoOwn" ; only do this if we own it
;    ReadRegStr $1 HKCR ".dts" "backup_val"
;    StrCmp $1 "" 0 "${Index}-Restore" ; if backup="" then delete the whole key
;      DeleteRegKey HKCR ".dts"
;    Goto "${Index}-NoOwn"
;    "${Index}-Restore:"
;      WriteRegStr HKCR ".dts" "" $1
;      DeleteRegValue HKCR ".dts" "backup_val"
;   
;    DeleteRegKey HKCR "medular.dts" ;Delete key with association settings
; 
;    System::Call 'Shell32::SHChangeNotify(i 0x8000000, i 0, i 0, i 0)'
;    "${Index}-NoOwn:"
;  !undef Index

SectionEnd  
