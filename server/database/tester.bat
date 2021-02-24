set /a count=3
:loop
    set /a count=count+1
    set username=%random%
    set password=%random%
    set token=%random%%random%%random%%random%%random%%random%

    echo ^<username^>%username%^</username^>^<password^>%password%^</password^>^<token^>%token%^</token^>^<role^>user^</role^>^<index^>%count%^</index^>>>accounts\%count%.xml
    echo ^<username^>%username%^</username^>^<password^>%password%^</password^>^<token^>%token%^</token^>^<role^>user^</role^>^<index^>%count%^</index^>>>token_index\%token%.xml
    echo ^<username^>%username%^</username^>^<password^>%password%^</password^>^<token^>%token%^</token^>^<role^>user^</role^>^<index^>%count%^</index^>>user_index\%username%.xml
goto :loop