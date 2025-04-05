
/**
 *  @file main.c
 *  @brief Interface for the code in q.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "q.h"

#define TEXTSECT     __attribute__ ((section(".text")))

char*_read = "r";

/**
 *  @brief Easy file loading to string/byte array.
 *  
 *  @param [in] fname File name
 *  @return File contents
 *  
 *  @details ez
 */
FASTCALL_A char*load(char*fname)
{
	FILE*lfile = fopen(fname,_read);
	if(!lfile)
	{
		fclose(lfile);
		return 0;
	}
	fseek(lfile,0,SEEK_END);
	size_t size = ftell(lfile);
	rewind(lfile);
	char*out = calloc(++size,1);
	out[--size] = 0;
	fread(out,1,size,lfile);
	fclose(lfile);
	return out;
}

// wtf
int __getmainargs(int * _Argc, char *** _Argv, char *** _Env, int _DoWildCard);

int   argc;
char**argv, __env;
_start() // main(int argc,char**argv)
{
	//                  wtf v
	__getmainargs(&argc,(char***)&argv,(char***)&__env,0);
	
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
		return 1;
	}
	else
		puts("qcc");
	
	char*input = argv[1];
	char*script = load(input);
	if (!script)
	{
		puts("NONEXISTENT FILE!1!!");
		return 3;
	}
	char*outf = 0;
	char*scriptname = 0;
	// convenient for if the program
	// runs at the root path of a scripts folder
	// so just "scripts\guitar\guitar.qb" can be input
	// without specifying it via --name switch
	//
	// still had to use the switch in qbuild anyway |:|
	char*writedbg = 0;
	
	char switchcount = 3;
	char*switchnames[] = {
		"--out",
		"--name",
			// originally named --scriptname but
			// stuck out *literally* amongst the smaller named params
		"--dbg"
	};
	// CAN'T USE VOID IN GCC
	char**switchoutvals[] = {
		// entered switches and params get written here
		&outf,
		&scriptname,
		&writedbg
	};
	char switchextraarg = 0b00000011;
	// 1 = arg+1 string -> switchoutvals
	// 0 = arg exists bool -> switchoutvals
	for (int i = 2; i < argc; i++)
	{
		char doesArgExist = 0;
		for (int j = 0; j < switchcount; j++)
		{
			if (!strcmp(argv[i],switchnames[j]))
			{
				if (switchextraarg & (1<<j))
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
					*switchoutvals[j] = (char*)1;
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
	// name file after the input but with .qb
	// if not specifying output file
	if (!outf)
	{
		register int fnlen = strlen(input);
		outf = (char*)memcpy(malloc(++fnlen),input,fnlen);
		*(short*)(&outf[fnlen-1]) = 0x0062; // 1337 lol // 'b\0'
	}
	// debug name for the script is the output's
	// file name if not specified manually
	if (!scriptname)
		scriptname = outf;
	
	#if (PREGEN_CRCTAB == 0)
		initCRC32();
	#endif
	
	printf("Compiling %s...\n",input);
	QDbg dbgmain;
	char*namedbg = outf;
	void*_out_dbg = 0; // more weirding
	if (writedbg)
	{
		_out_dbg = &dbgmain;
		namedbg = scriptname;
	}
	QNode output = eval_scr(script,_out_dbg);
	
	//puts("Writing output...");
	WriteQB(output, outf, crc32(namedbg));
	if (writedbg)
	{
		QLOCAL(QDbg) _out_dbg_; // insert scriptname param
		_out_dbg_.key = crc32(namedbg); // should i do this in WriteDBG
		_out_dbg_.name = namedbg;
		_out_dbg_.next = dbgmain;
		
		// stupid for no reason complicated thing
		register int nameNoExt_len = (int)(strrchr(input, '.') - input);
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
