for /F "tokens=2" %%i in ('date /t') do set mydate=%%i
set mytime=%time%
"C:\Program Files (x86)\NSIS\makensis.exe" "installer.nsi"
echo complete built installer %mydate%:%mytime% > built_installer.txt
OpenVR-WalkInPlace-Installer.exe
echo complete installation %mydate%:%mytime% > installed_exe.txt
