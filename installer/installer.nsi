;--------------------------------
;Include Modern UI

	!include "MUI2.nsh"

	!define MUI_ICON "..\client_overlay\bin\x64\res\OVRWIP.ico"
	!define MUI_HEADERIMAGE
	!define MUI_HEADERIMAGE_BITMAP "..\client_overlay\bin\x64\res\OVRWIP.ico"
	!define MUI_HEADERIMAGE_RIGHT

;--------------------------------
;General

	!define OVERLAY_BASEDIR "..\client_overlay\bin\x64"
	!define DRIVER_BASEDIR "..\driver_vrwalkinplace"

	;Name and file
	Name "OpenVR Walk In Place"
	OutFile "OpenVR-WalkInPlace.exe"
	
	;Default installation folder
	InstallDir "$PROGRAMFILES64\OpenVR-WalkInPlace"
	
	;Get installation folder from registry if available
	InstallDirRegKey HKLM "Software\OpenVR-WalkInPlace\Overlay" ""
	
	;Request application privileges for Windows Vista
	RequestExecutionLevel admin
	
;--------------------------------
;Variables

VAR upgradeInstallation

;--------------------------------
;Interface Settings

	!define MUI_ABORTWARNING

;--------------------------------
;Pages

	!insertmacro MUI_PAGE_LICENSE "..\LICENSE"
	!define MUI_PAGE_CUSTOMFUNCTION_PRE dirPre
	!insertmacro MUI_PAGE_DIRECTORY
	!insertmacro MUI_PAGE_INSTFILES
  
	!insertmacro MUI_UNPAGE_CONFIRM
	!insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
	!insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Macros

;--------------------------------
;Functions

Function dirPre
	StrCmp $upgradeInstallation "true" 0 +2 
		Abort
FunctionEnd

Function .onInit
	StrCpy $upgradeInstallation "false"

	ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenVRWalkInPlace" "UninstallString"
	StrCmp $R0 "" done	
	
	; If SteamVR is already running, display a warning message and exit
	FindWindow $0 "Qt5QWindowIcon" "SteamVR Status"
	StrCmp $0 0 +3
		MessageBox MB_OK|MB_ICONEXCLAMATION \
			"SteamVR is still running. Cannot install this software.$\nPlease close SteamVR and try again."
		Abort
 
	IfFileExists $INSTDIR\OpenVR-WalkInPlaceOverlay.exe 0 +5
		MessageBox MB_OKCANCEL|MB_ICONEXCLAMATION \
			"OpenVR Walk In Place is already installed. $\n$\nClick `OK` to upgrade the \
			existing installation or `Cancel` to cancel this upgrade." \
			IDOK upgrade
		Abort
 
	upgrade:
		StrCpy $upgradeInstallation "true"

	done:
FunctionEnd

;--------------------------------
;Installer Sections

Section "Install" SecInstall
	
	StrCmp $upgradeInstallation "true" 0 noupgrade 
		DetailPrint "Uninstall previous version..."
		ExecWait '"$INSTDIR\Uninstall.exe" /S _?=$INSTDIR'
		Delete $INSTDIR\Uninstall.exe
		Goto afterupgrade
		
	noupgrade:

	afterupgrade:

	SetOutPath "$INSTDIR"


	;ADD YOUR OWN FILES HERE...
	File "${OVERLAY_BASEDIR}\LICENSE"
	File "${OVERLAY_BASEDIR}\*.exe"
	File "${OVERLAY_BASEDIR}\*.dll"
	File "${OVERLAY_BASEDIR}\*.bat"
	File "${OVERLAY_BASEDIR}\*.vrmanifest"
	File "/oname=qt.conf" "${OVERLAY_BASEDIR}\qt_win.conf"
	File "${OVERLAY_BASEDIR}\..\logging.conf"
	File /r "${OVERLAY_BASEDIR}\res"
	File /r "${OVERLAY_BASEDIR}\qtdata"


	; Install redistributable
	ExecWait '"$INSTDIR\vcredist_x64.exe" /install /quiet'
	
	Var /GLOBAL vrRuntimePath
	nsExec::ExecToStack '"$INSTDIR\OpenVR-WalkInPlaceOverlay.exe" -openvrpath'
	Pop $0
	Pop $vrRuntimePath
	DetailPrint "VR runtime path: $vrRuntimePath"

	SetOutPath "$vrRuntimePath\drivers\00vrwalkinplace"
	File "${DRIVER_BASEDIR}\driver.vrdrivermanifest"
	SetOutPath "$vrRuntimePath\drivers\00vrwalkinplace\resources"
	File "${DRIVER_BASEDIR}\resources\driver.vrresources"
	File /r "${DRIVER_BASEDIR}\resources\icons"
	File /r "${DRIVER_BASEDIR}\resources\input"
	File /r "${DRIVER_BASEDIR}\resources\settings"
	SetOutPath "$vrRuntimePath\drivers\00vrwalkinplace\bin\win64"
	File "${DRIVER_BASEDIR}\bin\x64\driver_00vrwalkinplace.dll"

	; Install the vrmanifest
	nsExec::ExecToLog '"$INSTDIR\OpenVR-WalkInPlaceOverlay.exe" -installmanifest'
	
	; Post-installation step
	nsExec::ExecToLog '"$INSTDIR\OpenVR-WalkInPlaceOverlay.exe" -postinstallationstep'
  
	;Store installation folder
	WriteRegStr HKLM "Software\OpenVR-WalkInPlace\Overlay" "" $INSTDIR
	WriteRegStr HKLM "Software\OpenVR-WalkInPlace\Driver" "" $vrRuntimePath
  
	;Create uninstaller
	WriteUninstaller "$INSTDIR\Uninstall.exe"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenVRWalkInPlace" "DisplayName" "OpenVR Walk In Place"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenVRWalkInPlace" "UninstallString" "$\"$INSTDIR\Uninstall.exe$\""

SectionEnd

;--------------------------------
;Uninstaller Section

Section "Uninstall"
	; If SteamVR is already running, display a warning message and exit
	FindWindow $0 "Qt5QWindowIcon" "SteamVR Status"
	StrCmp $0 0 +3
		MessageBox MB_OK|MB_ICONEXCLAMATION \
			"SteamVR is still running. Cannot uninstall this software.$\nPlease close SteamVR and try again."
		Abort

	; Remove the vrmanifest
	nsExec::ExecToLog '"$INSTDIR\OpenVR-WalkInPlaceOverlay.exe" -removemanifest'

	; Delete installed files
	Var /GLOBAL vrRuntimePath2
	ReadRegStr $vrRuntimePath2 HKLM "Software\OpenVR-WalkInPlace\Driver" ""
	DetailPrint "VR runtime path: $vrRuntimePath2"
	Delete "$vrRuntimePath2\drivers\00vrwalkinplace\driver.vrdrivermanifest"
	Delete "$vrRuntimePath2\drivers\00vrwalkinplace\resources\driver.vrresources"
	Delete "$vrRuntimePath2\drivers\00vrwalkinplace\resources\settings\default.vrsettings"
	Delete "$vrRuntimePath2\drivers\00vrwalkinplace\bin\win64\driver_00vrwalkinplace.dll"
	Delete "$vrRuntimePath2\drivers\00vrwalkinplace\bin\win64\driver_vrwalkinplace.log"
	Delete "$vrRuntimePath2\drivers\00vrwalkinplace\bin\win64\error.log"
	Delete "$vrRuntimePath2\drivers\00vrwalkinplace\bin\win64\OpenVR-WalkInPlaceOverlay.log"
	Delete "$vrRuntimePath2\drivers\00vrwalkinplace\resources\icons\controller_status_ready.png"
	Delete "$vrRuntimePath2\drivers\00vrwalkinplace\resources\icons\vive_wand.svg"
	Delete "$vrRuntimePath2\drivers\00vrwalkinplace\resources\input\ovrwip_controller_profile.json"
	Delete "$vrRuntimePath2\drivers\00vrwalkinplace\resources\input\legacy_bindings_ovrwip_controller.json"
	RMdir "$vrRuntimePath2\drivers\00vrwalkinplace\resources\input"
	RMdir "$vrRuntimePath2\drivers\00vrwalkinplace\resources\icons"
	RMdir "$vrRuntimePath2\drivers\00vrwalkinplace\resources\settings"
	RMdir "$vrRuntimePath2\drivers\00vrwalkinplace\resources\"
	RMdir "$vrRuntimePath2\drivers\00vrwalkinplace\bin\win64\"
	RMdir "$vrRuntimePath2\drivers\00vrwalkinplace\bin\"
	RMdir "$vrRuntimePath2\drivers\00vrwalkinplace\"
	
	!include uninstallFiles.list

	DeleteRegKey HKLM "Software\OpenVR-WalkInPlace\Overlay"
	DeleteRegKey HKLM "Software\OpenVR-WalkInPlace\Driver"
	DeleteRegKey HKLM "Software\OpenVR-WalkInPlace"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OpenVRWalkInPlace"
SectionEnd

