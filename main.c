
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
	char*outf = 0;
	char*scriptname = 0;
	// convenient for if the program
	// runs at the root path of a scripts folder
	// so just "scripts\guitar\guitar.qb" can be input
	// without specifying it via --name switch
	char writedbg = 0;
	
	char switchcount = 3;
	char*switchnames[] = {
		"--out",
		"--name",
			// originally named --scriptname but
			// stuck out *literally* amongst the smaller named params
		"--dbg"
	};
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
		if (i == argc) //* |:|
			break;
		if (!doesArgExist)
		{
			printf("Unknown switch: %s\n", argv[i]);
		}
	}
	if (!outf)
	{
		register int fnlen = strlen(argv[1])+1;
		outf = (char*)memcpy(malloc(fnlen+1),argv[1],fnlen);
		*(short*)(&outf[fnlen-1]) = 0x0062; // 1337 lol // 'b\0'
	}
	if (!scriptname)
		scriptname = outf;
	
	#if (PREGEN_CRCTAB == 0)
		initCRC32();
	#endif
	
	printf("Compiling %s...\n",argv[1]);
	QDbg dbgmain;
	void*_out_dbg = 0; // more weirding
	if (writedbg)
	{
		_out_dbg = &dbgmain;
	}
	QNode output = eval_scr(load(argv[1]),_out_dbg);
	
	puts("Writing output...");
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
		register int nameNoExt_len = (int)(strrchr(argv[1], '.') - argv[1]) * -1;
		register char* nameWithDbg = (char*)memcpy(malloc(nameNoExt_len+5),argv[1],nameNoExt_len);
		*(int*)(&nameWithDbg[nameNoExt_len]) = 0x6762642E; // '.dbg'
		// main.c:162: warning: multi-character character constant NO ONE CARES
		nameWithDbg[nameNoExt_len+4] = 0;
		
		puts("Writing additional debug file...\n");
		WriteDBG(&_out_dbg_, nameWithDbg);
	}
	
	puts("Done");
	return 0;
}
