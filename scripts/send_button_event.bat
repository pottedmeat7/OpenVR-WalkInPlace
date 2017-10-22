tasklist /FI "IMAGENAME eq client_commandline.exe" 2>NUL | find /I /N "client_commandline.exe">NUL
if "%ERRORLEVEL%"=="0" (
   echo Program is running
) ELSE (
   set buttonevent=%1
   set deviceid=%2
   set buttonid=%3
   shift
   shift
   shift
   C:\PROGRA~1\OpenVR-InputEmulator\client_commandline.exe buttonevent %*
   echo OK
)