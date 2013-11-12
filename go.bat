gcc -o myb64.exe -std=c90 -fno-builtin -Wall myb64.c
choice /m Continue
if %ERRORLEVEL%==2 exit /b

myb64