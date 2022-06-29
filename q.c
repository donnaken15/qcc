
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "q.h"

// PC format only
// or atleast GH3

int die()
{
	puts("Press a key to exit");
	getc(stdin);
	exit(1);
}

/*
though now i cant use inline :(
UNLESS SOMEONE WANTS TO ENABLE SUPPORT FOR THAT!
*/
__inline Eswap(int value)
{
	return ((value & 0xFF) << 24) |
			((value & 0xFF00) << 8) |
			((value & 0xFF0000) >> 8) |
			((value & 0xFF000000) >> 24);
}

// APPARENTLY I CAN'T DEFINE, LIKE, IMMEDIATE ASSIGNED VARS IN THE .h FILE FOR SOME REASON
char qhead[] = {
	0x1C, 0x08, 0x02, 0x04, 0x10, 0x04, 0x08, 0x0C,
	0x0C, 0x08, 0x02, 0x04, 0x14, 0x02, 0x04, 0x0C,
	0x10, 0x10, 0x0C, 0x00
};
uint _sizeofQToken = sizeof(__static_QToken) - 4; // for use with binary writing
uint _sizeofQNode = sizeof(__static_QNode) - 4;

//#define DEBUG_KEY_COUNT 0x8000
//int  debugkeys [DEBUG_KEY_COUNT];
//char debugnames[DEBUG_KEY_COUNT][0x100];

char*tmpname;

#define Qlogging 0

#if (Qlogging == 1)
#define qlog(t) puts(t)
#define qlogx printf
#else
#define qlog(t)
#define qlogx(...)
#endif

#define PREGEN_CRCTAB 0

#if (PREGEN_CRCTAB == 1)
// pulled straight from tony hawk
uint crctable[256] = // CRC polynomial 0xedb88320
{
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
	0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
	0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
	0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
	0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
	0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
	0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
	0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
	0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
	0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
	0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
	0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
	0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
	0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
	0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
	0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
	0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
	0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
	0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
	0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
	0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
	0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
	0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
	0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
	0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
	0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
	0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
	0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
	0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
	0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
	0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
	0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
	0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
	0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
	0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
	0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
	0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
	0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
	0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};
#else
// implementation by nanook (wouldn't need if table is constant)
// but also the table is a kilobyte
uint crctable[256];
void initCRC32()
{
	unsigned int crc, poly = 0xEDB88320;
	for (int i = 0; i < 256; i++)
	{
		crc = i;
		for (int j = 0; j < 8; j++)
		{
			if (crc & 1)
				crc = crc >> 1 ^ poly;
			else
				crc >>= 1;
		}
		crctable[i] = crc;
	}
}
#endif
QKey crc32(char*text)
{
	register uint crc = 0xFFFFFFFF, len = strlen(text);
	for (register uint i = 0; i < len; i++)
	{
		crc = crc >> 8 & 0x00FFFFFF ^ crctable[(crc ^ text[i]) & 0xFF];
	}
	return crc;
}
charfilter(char c)
{
	if (c == ' ' || c == '\t')
		return CF_Whitespace;
	if (   (c >= 'A' && c <= 'Z')
		|| (c >= 'a' && c <= 'z')
		|| c == '_')
		return CF_Alphabet;
	if ((c >= '0' && c <= '9') || c == '.' || c == '-')
		return CF_Number;
	if (c == '=' || c == ';' ||
		c >= '(' || c >= ')' ||
		c >= '{' || c >= '}' ||
		c >= '[' || c >= ']')
		return CF_Syntax;
	if (c == '\n' || !c)
		return CF_None;
}
char*tokenErrorHead = "ERROR @ Tokenizer: \"%s\"\n";
char*tokenize(char*text, QToken out)
{
	int*type = &out->type;
	//puts(text);
	checkForBothWSandCmnts:
	while (*text == ' ' || *text == '\t' || *text == '\n' || *text == '\r')
	{
		text++;
	}
	if (*text == '/')
	{
		if (*(text+1) == '/')
		{
			qlog("got comment");
			text++;
			do {
				text++;
			} while (*text != '\n' && *text);
		}
		else if (*(text+1) == '*')
		{
			qlog("got multiline comment");
			text++;
			text++;
			do {
				if (!*text)
					break;
				else
				{
					if (!*(text+1))
						break;
					if (*text == '*')
						if (*(text+1) == '/')
							break;
				}
				text++;
			} while (1);
			text++;
			text++;
		}
		else
		{
			puts("wtf are you doing");
			die();
		}
		goto checkForBothWSandCmnts; // hi rato :^)
	}
	if (!*text)
	{
		out->type = QTokEOF;
		qlog("end of file");
		return text;
	}
	//this is dumb
	//CF_Alphabet scope
	char*id = text;
	int  il = 0;
	//CF_Number scope
	char*num  = text;
	int  dgt  = 0;
	int  dec  = 0;
	switch (charfilter(*text))
	{
		//  identifier
	case CF_Alphabet:
		do {
			il++;
			text++;
			//printf("%u\n", charfilter(*text));
		} while (charfilter(*text) & CF_Alphanum);
		*type = QTokOp;
		// do i even need to do this here instead of just looking for CRC'd keywords
		static char*invkey = "Invalid keyword",
			*kwint = "int",
			*kwfloat = "float",
			*kwif = "if",
			*kwqbkey = "qbkey";
		if (!strncmp(id, kwint, 3))
		{
			if (charfilter(id[3]) != CF_Whitespace)
			{
				printf(tokenErrorHead,kwint,id);
				puts(invkey);
				die();
			}
			out->op = QOpInt;
			goto FoundId;
		}
		if (!strncmp(id, kwfloat, 5))
		{
			if (charfilter(id[5]) != CF_Whitespace)
			{
				printf(tokenErrorHead,kwfloat,id);
				puts(invkey);
				die();
			}
			out->op = QOpFloat;
			goto FoundId;
		}
		if (!strncmp(id, kwif, 2))
		{
			if (charfilter(id[2]) != CF_Whitespace)
			{
				printf(tokenErrorHead,kwif,id);
				puts(invkey);
				die();
			}
			out->op = QOpIf;
			goto FoundId;
		}
		if (!strncmp(id, kwqbkey, 5))
		{
			if (charfilter(id[5]) != CF_Whitespace)
			{
				printf(tokenErrorHead,kwqbkey,id);
				puts(invkey);
				die();
			}
			out->op = QOpKey;
			goto FoundId;
		}
		*type = QTokKey; // last resort

		char*keySZ = malloc(il + 1);//can i not do this
		strncpy(keySZ, id, il);
		keySZ[il] = 0;
		out->nkey = crc32(keySZ);
		tmpname = keySZ;

	FoundId:
		qlogx("new token: %2u, value: %08X, string: %2u: %.*s\n",
			*type, out->value, il, il, id);
		break;
	case CF_Syntax:
		*type = QTokOp;
		//if (il == 1)oops
		{
			switch (*text)
			{
			case '=':
				out->op = QOpSet;
				break;
			case ';':
				out->op = QOpSEnd;
				break;
			case '(':
				out->op = QOpPBeg;
				break;
			case ')':
				out->op = QOpPEnd;
				break;
			case '{':
				out->op = QOpBBeg;
				break;
			case '}':
				out->op = QOpBEnd;
				break;
			case '[':
				out->op = QOpABeg;
				break;
			case ']':
				out->op = QOpAEnd;
				break;
			}
		}
		qlogx("new token: %2u, value:      %3u, string:  1: %c\n",
			*type, out->value, *text);
		text++;
		break;
	case CF_Number:
		do {
			text++;
			dgt++;
			if (*text == '.')
				dec++;
			if (dec > 1)
			{
				printf(tokenErrorHead,num);
				printf(
					"Invalid number, found more than one decimal.\n"
					"Are you creating version numbers, bro?\n");
				die();
				// abort created stuck processes
				// i can't kill, which makes me
				// want to kill *my* process...
			}
			// check for extra signs
			if (*text == '-')
			{
				printf(tokenErrorHead,num);
				printf("Invalid number, found more than one negative sign.\n");
				die();
			}
		} while (charfilter(*text) == CF_Number);
		if (!dec)
		{
			*type = QTokInt;
			out->number = atoi(num);
		}
		else
		{
			*type = QTokFloat;
			out->single = atof(num);
		}
		qlogx("new token: %2u, value: %08X, string: %2u: %.*s\n",
			*type, out->value, dgt, dgt, num);
		break;
	case CF_None:
		break;
	}
	return text;
}
void printErrorHead(uint depth)
{
	printf("ERROR @ Parser: token %3u\n", depth);
}
void exitOnPrematEnd(uint depth)
{
	static char*PrematEnd = "Unexpected end of input";
	puts(depth);
	printf(PrematEnd);
	die();
}
QNode parse(QToken tok)
{
	QNode items = malloc(sizeof(__static_QNode));
	QNode node = items;
	uint tokdepth = 0;
	while (tok)
	{
		node->always20 = 0x2000;
		node->x278081F3 = ESWAP(0x278081F3); //BE
		node->pad10 = 0;
		node->next = 0;
		//node->type = tok->type;
		qlogx("%4u: token: type: %2u, %p\n", tokdepth, tok->type, tok->value);
		switch (tok->type)
		{
		case QTokOp:
			qlogx("      op   , type :   %2u\n", tok->op);
			switch (tok->op)
			{
				// int player = 0;
				// ---
			case QOpFloat:
				node->type = QTypeFloat;
				goto VarDefine;
			case QOpKey:
				node->type = QTypeQbKey;
				goto VarDefine;
			case QOpInt:
				// variable declaration
				node->type = QTypeInt;
			VarDefine:; // <-- lol V
				static char*VarDef_err1_end = " when declaring variable.\n";
				if (!tok->next)
				{
					exitOnPrematEnd(tokdepth);
				}
				NextItem(tok);
				tokdepth++;
				qlogx("%4u\n", tokdepth);
				// int player = 0;
				//     ------
				if (tok->type != QTokKey)
				{
					printErrorHead(tokdepth);
					printf("Encountered non QbKey name%s", VarDef_err1_end);
					die();
				}
				node->name = tok->nkey;
				if (!tok->next)
				{
					exitOnPrematEnd(tokdepth);
				}
				NextItem(tok);
				tokdepth++;
				qlogx("%4u\n", tokdepth);
				// int player = 0;
				//            -
				if (tok->type != QTokOp || tok->op != QOpSet)
				{
					printErrorHead(tokdepth);
					printf("Assignment operation not found%s", VarDef_err1_end);
					die();
				}
				if (!tok->next)
				{
					exitOnPrematEnd(tokdepth);
				}
				NextItem(tok);
				tokdepth++;
				qlogx("%4u\n", tokdepth);
				// int player = 0;
				//              -
				// dumb
				switch (node->type)
				{
				case QTypeInt:
					if (tok->type != QTokInt)
						goto QAssignNotMatching;
					break;
				case QTypeQbKey:
					if (tok->type != QTokKey)
						goto QAssignNotMatching;
					break;
				case QTypeFloat:
					if (tok->type != QTokFloat)
						goto QAssignNotMatching;
					break;
				QAssignNotMatching:
					printErrorHead(tokdepth);
					printf("Value type not matching type of variable");
					die();
					break;
				}
				node->value = tok->value;
				if (!tok->next)
				{
					exitOnPrematEnd(tokdepth);
				}
				NextItem(tok);
				tokdepth++;
				qlogx("%4u\n", tokdepth);
				if (tok->type != QTokOp || tok->value != QOpSEnd)
				{
					printErrorHead(tokdepth);
					printf("Unclosed statement");
					die();
				}
				qlogx("      New node: %3u, name: %p, value: %p\n",
					node->type, node->name, node->number);
				break;
			}
			break;
		case QTokEOF:
			node->next = 0xFFFFFFFF;
			break;
		}
		if (tok->next)
		{
			node->next = malloc(sizeof(__static_QNode));
			NextItem(node);
		}
		NextItem(tok);
		tokdepth++;
	}
	return items;
}
checkdbg(QDbg dbg,uint key)
{
	while (dbg)
	{
		if (dbg->key == key)
			return 1; // return true if a key already exists
		NextItem(dbg);
	}
	return 0;
}
QNode eval_scr(char*scr, QDbg*dbg)
{
	if (dbg)
		*dbg = calloc(1,sizeof(__static_QDbg));
	qlog(scr);
	char*lp = scr;
	QToken tokens;
	QToken tmptok;
	char firstitem = 1;
	QDbg tmpdbg = *dbg;
	while (*lp)
	{
		if (!firstitem)
		{
			tmptok->next = malloc(sizeof(__static_QToken));
			NextItem(tmptok);
			tmptok->next = 0;
		}
		else
		{
			tmptok = malloc(sizeof(__static_QToken));
			tokens = tmptok;
			firstitem = 0;
		}
		qlogx("%4u: ", lp - scr);
		lp = tokenize(lp, tmptok);
		if (tmptok->type == QTokKey)
		{
			if (!checkdbg(*dbg,tmptok->nkey)) // disallow dupes
			{
				tmpdbg->name = tmpname;
				tmpdbg->key  = tmptok->nkey;
				tmpdbg->next = calloc(1,sizeof(__static_QDbg));
				NextItem(tmpdbg);
			}
		}
	}
#if 0
	qlog("test:");
	for (QToken test = tokens; test; NextItem(test))
	{
		qlogx("token: type: %2u, ", test->type);
		switch (test->type)
		{
		case QTokOp:
			qlogx("op   , type :   %2u\n", test->op);
			break;
		case QTokKey:
			qlogx("key  , key  : %8X\n", test->nkey);
			break;
		case QTokInt:
			qlogx("int  , value: %4u\n", test->number);
			break;
		case QTokFloat:
			qlogx("float, value: %4f\n", test->single);
			break;
		}
	}
#endif
	QNode items = parse(tokens);
	return items;
}
void WriteQB(QNode items, char*fname)
{
	QFile qbin = malloc(sizeof(QHead)+4);
	qbin->head.gap = 0;
	qbin->head.size = 0;
	qbin->items = items;
	memcpy(&qbin->head.unknown, qhead, sizeof(qhead));
	FILE*qf = fopen(fname, "wb");
	fwrite(qbin, sizeof(QHead), 1, qf);
	for (; items && items->next != 0xFFFFFFFF; NextItem(items))
	{
		items->value = Eswap(items->value);
		items->name = Eswap(items->name);
		fwrite(items, _sizeofQNode, 1, qf);
		items->value = Eswap(items->value);
		items->name = Eswap(items->name);
	}
	qbin->head.size = ftell(qf);
	fseek(qf, 4, SEEK_SET);
	qbin->head.size = Eswap(qbin->head.size);
	fwrite(&qbin->head.size, sizeof(int), 1, qf);
	qbin->head.size = Eswap(qbin->head.size);
	fclose(qf);
}
const char* dbg_hd0 = "[LineNumbers]\n";
const char* dbg_hd1 = "[Checksums]\n";
char*nl = "\n\0";
void WriteDBG(QDbg dbg, char*fname)
{
	FILE*dbgf = fopen(fname, "w");
	fwrite(dbg_hd0, 14, 1, dbgf);
	fputs(nl, dbgf);
	fwrite(dbg_hd1, 12, 1, dbgf);
	for (QDbg test2 = dbg; test2 && test2->next; NextItem(test2))
	{
		fprintf(dbgf, "0x%08x %s\n", test2->key, test2->name);
	}
	fputs(nl, dbgf);
	fclose(dbgf);
}

