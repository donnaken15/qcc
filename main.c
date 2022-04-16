
#include <stdio.h>
#include <stdlib.h>
#include "q.h"

char*load(char*fname)
{
	FILE*lfile = fopen(fname,"rb");
	fseek(lfile,0,SEEK_END);
	size_t size = ftell(lfile);
	rewind(lfile);
	char*out = calloc(++size,1);
	out[--size] = 0;
	fread(out,1,size,lfile);
	fclose(lfile);
	return  out;
}

int   argc;
char**argv, __env;
_start()
{
	__getmainargs(&argc,&argv,&__env,0);
	#if (PREGEN_CRCTAB == 0)
		initCRC32();
	#endif
	puts("Tests");
	puts("CRC generation:");
	printf("%16s: %08X\n", "none", crc32("none"));
	printf("%16s: %08X\n", "wesle", crc32("wesle"));
	printf("%16s: %08X\n", "wesley", crc32("wesley"));
	printf("%16s: %08X\n", "fastgh3", crc32("fastgh3"));
	printf("%16s: %08X\n", "player", crc32("player"));
	// wish i could use gets in this case
	// but also multiline ops would break (literally)
	QDbg test1;
	//printf("%p\n",test1);
	QNode test0 = eval_scr(load("testfile.q"),&test1);
	//printf("%p\n",test1);
	puts("\nDebug keys:\n");
	for (QDbg test2 = test1; test2 && test2->next; NextItem(test2))
	{
		printf("0x%08x %s\n", test2->key, test2->name);
	}
	WriteQB(test0, "testfile.qb");
	WriteDBG(test1, "testfile.dbg");
	getc(stdin); // system pause
}
