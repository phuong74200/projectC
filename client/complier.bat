setlocal
cd /d C:\msys64\usr\bin\
set filePath=%~dP0
set root=%filePath%
set filePath=%filePath:\=/%
bash -l -c "PATH=C:/mingw64/bin:$PATH && cd "%filePath%script" && gcc `pkg-config --cflags gtk+-3.0` -o "%filePath%client" client.c `pkg-config --libs gtk+-3.0`"
pause