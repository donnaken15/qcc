@echo off

:_TEST
cls
mode CON COLS=62 LINES=68

tcc -c q.c && tcc main.c q.o -o qcc.exe
if ERRORLEVEL 1 ( pause && del *.o /q && SET ERRORLEVEL=1 && goto :_TEST )
del *.o /q

qcc testfile.q --dbg
copy testfile.qb  validator /y > nul
copy testfile.dbg validator /y > nul
E:\qtest\bin\pakdir validator  > nul

pause
goto :_TEST
