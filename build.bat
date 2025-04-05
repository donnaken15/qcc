@echo off
:#c:\fasm\fasm q2.asm
tcc -c q.c && tcc main.c q.o -o qcc.exe
if ERRORLEVEL 1 ( pause && del *.o /q && SET ERRORLEVEL=1 )
del *.o /q