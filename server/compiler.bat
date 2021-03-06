if exist server.exe (
    copy server.exe cache\~server_%mytime%.exe
    del /f /q client.exe
    copy script\server.c cache\~server_%mytime%.c
) else (
    rem 1412
)

gcc script\server.c -o server.exe -lwsock32
pause