tcc -c q.c && tcc main.c q.o -o qcc.exe
if ERRORLEVEL 1 pause
del *.o