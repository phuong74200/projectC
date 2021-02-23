setlocal

set mytime=""
For /f "tokens=1-2 delims=/:" %%a in ("%TIME%") do (set mytime=%%a.%%b)

if exist client.exe (
    copy client.exe cache\~client_%mytime%.exe
    del /f /q client.exe
    copy script\client.c cache\~client_%mytime%.c
    copy UI\builder.xml cache\~cbuilder_%mytime%.xml
) else (
    rem 1412
)

cd /d C:\msys64\usr\bin\
set filePath=%~dP0
set root=%filePath%
set filePath=%filePath:\=/%
bash -l -c "PATH=C:/mingw64/bin:$PATH && cd "%filePath%script" && gcc `pkg-config --cflags gtk+-3.0` -o "%filePath%client" client.c `pkg-config --libs gtk+-3.0` -lwsock32"
pause