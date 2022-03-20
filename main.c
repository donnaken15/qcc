
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

int main(int argc, char*argv[])
{
	puts("Tests");
	puts("CRC generation:");
	printf("%16s: %08X\n", "none", crc32("none"));
	printf("%16s: %08X\n", "wesle", crc32("wesle"));
	printf("%16s: %08X\n", "wesley", crc32("wesley"));
	printf("%16s: %08X\n", "fastgh3", crc32("fastgh3"));
	printf("%16s: %08X\n", "player", crc32("player"));
	// wish i could use gets in this case
	WriteQB(eval_scr(load("testfile.q")), "testfile.qb");
	//getc(stdin); // system pause
}
