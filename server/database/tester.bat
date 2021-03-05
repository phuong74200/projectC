@echo off

rem user generator for testing :))

echo ^<username^>fuong74200^</username^>^<password^>123456^</password^>^<token^>adm_fuong74200^</token^>^<role^>admin^</role^>^<index^>0^</index^>>accounts\0.xml
echo ^<username^>fuong74200^</username^>^<password^>123456^</password^>^<token^>adm_fuong74200^</token^>^<role^>admin^</role^>^<index^>0^</index^>>token_index\adm_fuong74200.xml
echo ^<username^>fuong74200^</username^>^<password^>123456^</password^>^<token^>adm_fuong74200^</token^>^<role^>admin^</role^>^<index^>0^</index^>>user_index\fuong74200.xml

set /a count=1
:generateUserLoop
    set /a count=count+1
    set password=123456
    set token=%random%%random%%random%%random%%random%%random%
    Setlocal EnableDelayedExpansion
    Set _RNDLength=8
    Set _Alphanumeric=ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789
    Set _Str=%_Alphanumeric%987654321
    :_LenLoop
    IF NOT "%_Str:~18%"=="" SET _Str=%_Str:~9%& SET /A _Len+=9& GOTO :_LenLoop
    SET _tmp=%_Str:~9,1%
    SET /A _Len=_Len+_tmp
    Set _count=0
    SET _RndAlphaNum=
    :_loop
    Set /a _count+=1
    SET _RND=%Random%
    Set /A _RND=_RND%%%_Len%
    SET _RndAlphaNum=!_RndAlphaNum!!_Alphanumeric:~%_RND%,1!
    If !_count! lss %_RNDLength% goto _loop
    echo ^<username^>!_RndAlphaNum!^</username^>^<password^>%password%^</password^>^<token^>!_RndAlphaNum!!_RndAlphaNum!%password%^</token^>^<role^>user^</role^>^<index^>%count%^</index^>>accounts\%count%.xml
    echo ^<username^>!_RndAlphaNum!^</username^>^<password^>%password%^</password^>^<token^>!_RndAlphaNum!!_RndAlphaNum!%password%^</token^>^<role^>user^</role^>^<index^>%count%^</index^>>token_index\%token%.xml
    echo ^<username^>!_RndAlphaNum!^</username^>^<password^>%password%^</password^>^<token^>!_RndAlphaNum!!_RndAlphaNum!%password%^</token^>^<role^>user^</role^>^<index^>%count%^</index^>>user_index\!_RndAlphaNum!.xml
    echo !_RndAlphaNum! ^| %password% ^| !_RndAlphaNum!!_RndAlphaNum!%password% ^| user ^| %count%
    endlocal
goto :generateUserLoop