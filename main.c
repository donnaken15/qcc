
#include <stdio.h>
#include <stdlib.h>
#include "q.h"

char*_read = "r";


char*load(char*fname)
{
	FILE*lfile = fopen(fname,_read);
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
	
	// argv[0] = this exe
	if (argc == 1)
	{
		puts("qcc - (WIP) q compiler\n"
				"by donnaken15\n\n"
				"NOTE: C-STYLE SYNTAX\n\n"
				"usage:\n"
				"qcc [script file]\n\n"
				"optional arguments:\n"
				"    --out  [file]  - specify output\n"
				"    --dbg          - generate debug file\n"
				"    --name [name]  - specify debug name");
				// optimize this too?
				// since the switch names
				// practically exist twice
		return;
	}
	else
		puts("qcc");
	char*input = argv[1];
	FILE*fscr = fopen(input,_read);
	if (!fscr)
	{
		puts("NONEXISTENT FILE!1!!");
		fclose(fscr);
		return 3;
	}
	fclose(fscr);
	char*outf = 0;
	char*scriptname = 0;
	// convenient for if the program
	// runs at the root path of a scripts folder
	// so just "scripts\guitar\guitar.qb" can be input
	// without specifying it via --name switch
	//
	// still had to use the switch in qbuild anyway |:|
	char writedbg = 0;
	
	char switchcount = 3;
	char*switchnames[] = {
		"--out",
		"--name",
			// originally named --scriptname but
			// stuck out *literally* amongst the smaller named params
		"--dbg"
	};
	// CAN'T USE VOID IN GCC
	void*switchoutvals[] = {
		&outf,
		&scriptname,
		&writedbg
	};
	char switchextraarg[] = {
		// 1 = arg+1 string -> switchoutvals
		// 0 = arg exists bool -> switchoutvals
		1,
		1,
		0
	};
	for (int i = 2; i < argc; i++)
	{
		char doesArgExist = 0;
		for (int j = 0; j < switchcount; j++)
		{
			if (!strcmp(argv[i],switchnames[j]))
			{
				if (switchextraarg[j])
				{
					if (i+1<argc)
					{
						if (!*switchoutvals[j])
						{
							*switchoutvals[j] = argv[i+1];
							i++;
							doesArgExist = 1;
							break;
						}
						else
						{
							i += 2; //*
							printf("%s argument entered more than once\n",switchnames[j]);
							break;
						}
					}
					else
					{
						printf("Incomplete parameters @ %s argument\n",switchnames[j]);
						return 3;
					}
				}
				else
				{
					*switchoutvals[j] = (char)1;
					doesArgExist = 1;
					break;
				}
			}
		}
		if (i >= argc) //* |:|
			break;
		if (!doesArgExist)
		{
			printf("Unknown switch: %s\n", argv[i]);
		}
	}
	if (!outf)
	{
		register int fnlen = strlen(input);
		outf = (char*)memcpy(malloc(++fnlen),input,fnlen);
		*(short*)(&outf[fnlen-1]) = 0x0062; // 1337 lol // 'b\0'
	}
	if (!scriptname)
		scriptname = outf;
	
	#if (PREGEN_CRCTAB == 0)
		initCRC32();
	#endif
	
	printf("Compiling %s...\n",input);
	QDbg dbgmain;
	void*_out_dbg = 0; // more weirding
	if (writedbg)
	{
		_out_dbg = &dbgmain;
	}
	QNode output = eval_scr(load(input),_out_dbg);
	
	//puts("Writing output...");
	WriteQB(output, outf);
	if (writedbg)
	{
		QLOCAL(QDbg) _out_dbg_; // insert scriptname param
		char*namedbg = outf;
		if (scriptname)
			namedbg = scriptname;
		_out_dbg_.key = crc32(namedbg);
		_out_dbg_.name = namedbg;
		_out_dbg_.next = dbgmain;
		
		// stupid for no reason complicated thing
		register int nameNoExt_len = (int)(strrchr(input, '.') - input) * -1;
		register char* nameWithDbg = (char*)memcpy(malloc(nameNoExt_len+5),input,nameNoExt_len);
		*(int*)(&nameWithDbg[nameNoExt_len]) = 0x6762642E; // '.dbg'
		// main.c:162: warning: multi-character character constant NO ONE CARES
		nameWithDbg[nameNoExt_len+4] = 0;
		
		//puts("Writing additional debug file...\n");
		WriteDBG(&_out_dbg_, nameWithDbg);
	}
	
	puts("OK");
	return 0;
}
