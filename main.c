
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

#define copy(addr,size) memcpy(malloc(size),addr,size)

int   argc;
char**argv, __env;
_start()
{
	__getmainargs(&argc,&argv,&__env,0);
	
	puts("qcc - (WIP) q compiler\n"
			"by donnaken15\n");
	// argv[0] = this exe
	if (argc == 1)
	{
		puts("NOTE: C-STYLE SYNTAX\n\n"
				"usage:\n"
				"qcc [script file]\n\n"
				"optional arguments:\n"
				"    --out [file]   - specify output\n"
				"    --dbg          - generate debug file\n"
				"    --name [name]  - specify debug name");
		return;
	}
	FILE*fscr = fopen(argv[1],"r");
	if (!fscr)
	{
		puts("NONEXISTENT FILE!1!!");
		fclose(fscr);
		return 3;
	}
	fclose(fscr);
	// <strikeout>should the script file really be a non-1st argument</strikeout>
	char*outf = (char*)0;
	char*scriptname = argv[1];
	// convenient for if the program
	// runs at the root path of a scripts folder
	// so just "scripts\guitar\guitar.qb" can be input
	// without specifying it via --name switch
	char writedbg = 0;
	for (int i = 2; i < argc; i++)
	{
		// SHOULD MAKE THESE INTO A FUNCTION OR ARRAY
		if (!strcmp(argv[i],"--out"))
		{
			if (i+1<argc)
			{
				outf = argv[i+1];
			}
			else
			{
				puts("Incomplete parameters @ --out argument");
				return 3;
			}
		}
		if (!strcmp(argv[i],"--name"))
			// originally named --scriptname but
			// stuck out *literally* amongst the smaller named params
		{
			if (i+1<argc)
			{
				scriptname = argv[i+1];
			}
			else
			{
				puts("Incomplete parameters @ --name argument");
				return 3;
			}
		}
		if (!strcmp(argv[i],"--dbg"))
			writedbg = 1;
	}
	if (!outf)
	{
		register int fnlen = strlen(argv[1])+1;
		outf = memcpy(malloc(fnlen+1),argv[1],fnlen);
		*(short*)(&outf[fnlen-1]) = 0x0062; // 1337 lol // 'b\0'
	}
	
	#if (PREGEN_CRCTAB == 0)
		initCRC32();
	#endif
	
	printf("Compiling %s...\n",argv[1]);
	QDbg _out_dbg;
	QNode output = eval_scr(load(argv[1]),&_out_dbg);
	
	__static_QDbg _out_dbg_; // insert scriptname param
	_out_dbg_.key = crc32(outf);
	_out_dbg_.name = outf;
	_out_dbg_.next = _out_dbg;
	
	puts("Writing output...");
	WriteQB (output, outf);
	if (writedbg)
	{
		// stupid for no reason complicated thing
		register int nameNoExt_len = (int)(strrchr(argv[1], '.') - argv[1]) * -1;
		register char* nameWithDbg = memcpy(malloc(nameNoExt_len+5),argv[1],nameNoExt_len);
		*(int*)(&nameWithDbg[nameNoExt_len]) = 0x6762642E; // '.dbg'
		nameWithDbg[nameNoExt_len+4] = 0;
		
		puts("Writing additional debug file...\n");
		WriteDBG(&_out_dbg_, nameWithDbg);
	}
	
	puts("Done");
	return 0;
	
	
	// SPECIFIC LEFTOVER TESTING
	
	/*puts("Tests");
	puts("CRC generation:");
	static char*printtest = "%16s: %08X\n";
	// TCC NEEDS STRING POOLING
	static char*testkeys[] = {
		"none",
		"wesle",
		"wesley",
		"fastgh3",
		"player"
	};
	for (int i = 0; i < 5; i++)
		printf(printtest, testkeys[i], crc32(testkeys[i]));*/
	// wish i could use gets in this case
	// but also multiline ops would break (literally)
	
	#if 0
	QDbg test1;
	QNode test0 = eval_scr(load("testfile.q"),&test1);
	const char*fname = "testfile.qb";
	puts("\nDebug keys:\n");
	__static_QDbg test1_; // insert my filename
	test1_.key = crc32(fname);
	test1_.name = fname;
	test1_.next = test1;
	for (QDbg test2 = &test1_; test2 && test2->next; NextItem(test2))
	{
		printf("0x%08x %s\n", test2->key, test2->name);
	}
	WriteQB(test0, "testfile.qb");
	WriteDBG(&test1_, "testfile.dbg");
	getc(stdin); // system pause
	#endif
}
