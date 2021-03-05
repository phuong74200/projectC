# Libraries:
    - winsock
    - gtk
# Cách thiết lập môi trường:
    - Tải và cài đặt msys2: https://repo.msys2.org/distrib/x86_64/msys2-x86_64-20210228.exe
    - Mở msys2.exe và nhập các dòng lệnh sau:
        $ pacman -Syu
        $ pacman -Su
        $ pacman -S mingw-w64-x86_64-gtk3
        $ pacman -S mingw-w64-x86_64-toolchain base-devel
# Lưu ý:
    - Vì sử dụng socket nên trước khi compile mọi người vui lòng sửa lại file .\client\script\client.c dỏng 13
    #define ADDR "192.168.1.1" thành địa chỉ IPv4 của máy chủ (máy sẽ chạy file server.exe)
    - Phần mềm chỉ hoạt động trong mạng LAN
    - Có một số phần mềm diệt virus sẽ khiến phần mềm không hoạt động
# Cách compile
    - Cách 1:
        + Chạy file .\server\compiler.bat
        + Chạy file .\client\compiler.bat
        Sau khi chạy xong 2 file này sẽ xuất hiện lần lượt 2 file:
        + .\server\server.exe
        + .\server\client.exe
        Chỉ cần chạy 2 file này là được
    - Cách 2:
        + Mở msys2
        + chạy các lệnh sau:
            $ cd .\client
            $ PATH=C:/mingw64/bin:$PATH
            $ gcc `pkg-config --cflags gtk+-3.0` -o "client" script\\client.c `pkg-config --libs gtk+-3.0` -lwsock32
            $ cd ~
            $ cd .\server
            $ gcc script\server.c -o server.exe -lwsock32
        + Sau khi chạy xong 2 file này sẽ xuất hiện lần lượt 2 file:
            + .\server\server.exe
            + .\server\client.exe
        Chỉ cần chạy 2 file này là được