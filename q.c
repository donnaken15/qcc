
/**
 *  \file q.c
 *  \brief Code for parsing Q scripts
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "q.h"

// PC format only
// or atleast (and tested on) GH3

QSECTION FASTCALL_A void eputs(char*t) //chart lol
{
	fputs(t,stderr);
} // saved like 100 bytes not making this a macro :/
// ofc because stderr is an extra value to push
// and because IOB
QSECTION eprintf(char*fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = vfprintf(stderr,fmt,args);
	va_end(args);
	return ret;
}

#if (Qlogging == 1)
#define qlog(t) eputs(t)
#define qlogx(fmt,...) eprintf(fmt,__VA_ARGS__)
#else
#define qlog(t)
#define qlogx(...)
#endif

QSECTION void die()
{
#if 0
	puts("Press a key to exit");
	getc(stdin);
#endif
	exit(1);
}

#if (BE == 1)
/**
 *  \brief Swap endianness of input value
 *  
 *  \param [in] value thx FASTcall --e-b-p- -1-3-3-7- -a-c-c-e-s-s- -t-o- -v-a-l-u-e--
 *  \return Swapped endianness of value
 *  
 *  duh
 *  
 *  \details Screw GAS
 */
// should this be inline or something
// i tried to do that with pointers
// in inline asm with macros but
// failed immediately
QSECTION FASTCALL_A Eswap(int _)
{
	__asm__ __volatile__ (
		"  bswap    %%eax\n"
		"  leave\n"
		"  ret\n"
		:
		:
		:"memory"
	);
	return;
}
#else
#define Eswap(i) i
#endif

QSECTION char qhead[] = {
	0x1C, 0x08, 0x02, 0x04, 0x10, 0x04, 0x08, 0x0C,
	0x0C, 0x08, 0x02, 0x04, 0x14, 0x02, 0x04, 0x0C,
	0x10, 0x10, 0x0C, 0x00
};
const uint _sizeofQToken = QSIZEOF(QToken) - 4; // for use with binary writing
const uint _sizeofQNode = QSIZEOF(QNode) - 4;
const uint _sizeofQArray = QSIZEOF(QArray);

QSECTION char*tmpname;

QSECTION char*print4digit = "%4u\n";

#if (PREGEN_CRCTAB == 1)
// pulled straight from tony hawk
QSECTION uint crctable[256] = // CRC polynomial 0xedb88320
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
QSECTION void initCRC32()
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
QSECTION FASTCALL_A QKey crc32(char*text)
{
	register uint crc = 0xFFFFFFFF, len = strlen(text);
	for (register uint i = 0; i < len; i++)
	{
		register char sym = text[i]; // ugh
		if (sym <= 'Z' && sym >= 'A') sym += 0x20;
		// *thinking that doing less than first will be faster
		// because its checking values less than 0x80 first
		else if (sym == '/') sym = '\\';
		crc = crc >> 8 & 0x00FFFFFF ^ crctable[(crc ^ sym) & 0xFF];
	}
	return crc;
}
char syntaxChars[] = {
	'=',
	';',
	':',
	',',
	'(',
	')',
	'{',
	'}',
	'[',
	']',
	'+',
	'-',
	'*',
	'/',
	'<',
	'>',
	'!'
};
/*char syntaxIds[] = { // already in order
	QOpSet,
	QOpSEnd,
	QOpColn,
	QOpDlim,
	QOpPBeg,
	QOpPEnd,
	QOpBBeg,
	QOpBEnd,
	QOpABeg,
	QOpAEnd,
	QOpPlus,
	QOpAstx,
	QOpDvid,
	QOpLess,
	QOpGrtr,
	QOpNot
};*/
QSECTION FASTCALL_A charfilter(char c)
{
	switch (c)
	{
		case ' ':
		case '\t':
			return CF_Whitespace;
		case 'A'...'Z': // just found out about this in TCC doc
		case 'a'...'z':
		case '_':
			return CF_Alphabet;
		case '0'...'9':
		case '.':
		case '-': // probably shouldnt put here
			return CF_Number;
		case '"':
		case '\'':
			return CF_String;
		case '\n':
		case '\r':
		case 0:
			return CF_None;
	}
	for (int i = 0; i < sizeof(syntaxChars); i++)
	{
		if (c == syntaxChars[i])
			return CF_Syntax;
	}
	return CF_Unknown;
}
QSECTION char*tokenErrorHead = "ERROR @ Tokenizer: \"%s\"\n";
/**
 *  \brief Read text and create a token from it
 *  
 *  \param [in] text Text to parse
 *  \param [in] out Pointer to create token at
 *           ^  ^
 *            :/
 *  
 *  \return Return position in text after parsing
 *  
 *  \details Read text that contains syntax, names, strings, numbers, and skip whitespace and comments that precede
 */
QSECTION FASTCALL_AD char*tokenize(char*text, QToken out)
{
	int*type = &out->type;
	// thinking this will be faster than -> every time
	// because thats compiled like [ebp+16] or something
	checkForBothWSandCmnts:
	while (*text == ' ' || *text == '\t' || *text == '\n' || *text == '\r')
	{
		text++;
	}
	if (*text == '/')
	{
		if (text[1] == '/')
		{
			qlog("got comment,\n      ");
			text++;
			do {
				text++;
			} while (*text != '\n' && *text);
		}
		else if (text[1] == '*')
		{
			qlog("got multiline comment,\n      ");
			text++;
			text++;
			do {
				if (!*text)
					break;
				else
				{
					if (!text[1])
						break;
					if (*text == '*')
						if (text[1] == '/')
							break;
				}
				text++;
			} while (1);
			text++;
			text++;
		}
		else
		{
			eputs("wtf are you doing\n");
			die();
		}
		goto checkForBothWSandCmnts; // hi rato :^)
	}
	if (!*text)
	{
		*type = QTokEOF;
		return text;
	}
	static char*newTokenPrintHead = "new token: %2u, ";
	static char*newTokenPrint = "value: %08X, string: %2u: %.*s\n";
	switch (charfilter(*text))
	{
		// identifier/key
	case CF_Alphabet: // "[a]123abc"
	{
		// this works now now that its scoped
		// in a code block where its actually
		// discarded after execution
		char*id = text;
		int  il = 0;
		do {
			il++;
			text++;
		} while (charfilter(*text) & CF_Alphanum); //"a[123abc]"
		
		*type = QTokKey;

		char*keySZ = malloc(++il);// can i not do this
		strncpy(keySZ, id, --il);
		keySZ[il] = 0;
		out->nkey = crc32(keySZ);
		tmpname = keySZ;

	FoundId:
		qlogx(newTokenPrintHead, *type);
		qlogx(newTokenPrint, out->value, il, il, id);
		break;
	}
	case CF_Number: //-0.0
	{
		// try to assume alternate use first
		// and that all numbers dont have
		// spaces in between syntax
		// as if any sane person does that
		if (*text == '-')
		{
			if (charfilter(text[1]) != CF_Number)
				goto case_CF_Syntax;
		}
		char*num  = text;
		int  dgt  = 0;
		int  dec  = 0;
		do {
			text++;
			dgt++;
			if (*text == '.')
				dec++;
			if (dec > 1)
			{
				eprintf(tokenErrorHead,num);
				eputs(
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
				eprintf(tokenErrorHead,num);
				eputs("Invalid number, found more than one negative sign.\n");
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
		qlogx(newTokenPrint, out->value, dgt, dgt, num);
		break;
	}
	case CF_String:
		// i dont care about allowing widechar/unicode
		// right now, so " and ' are treated the same
		*type = QTokStr;
		{
			char*ss = text+1;
			int  sl = 0;
			char wc = '"'; // wrapping character, default: "
			char*trim;
			switch (*text)
			{
			case '\'':
				wc = '\'';
			case '"':
				do { text++; }
				while (*text != wc && *text);
				sl = text-ss;
				trim = malloc(++sl);
				trim[--sl] = 0;
				strncpy(trim,ss,sl);
				out->string = trim;
				break;
			}
			qlogx(newTokenPrintHead, *type);
			qlogx("str  : %3u: \"%s\"\n", sl, trim);
		}
		text++;
		break;
	case CF_Syntax:
	case_CF_Syntax:
		*type = QTokOp;
		{
			switch (*text)
			{
			case '=':
				if (text[1] == '=')
				{
					text++;
					out->op = QOpCmp;
					break;
				}
				out->op = QOpSet;
				goto gotSyntaxId;
			}
			// 1 to skip =/== check ^
			for (int i = 1; i < sizeof(syntaxChars); i++)
			{
				if (*text == syntaxChars[i])
				{
					out->op = ++i;
					break;
				}
			}
		}
		gotSyntaxId:;
		qlogx(newTokenPrintHead, *type);
		qlogx("value:      %3u, string:  1: %c\n", out->value, *text);
		text++;
		break;
	case CF_Unknown:
		//if (*text == '`') // check for key enclosed in ``
	default:
		eputs("Unknown characters");
	case CF_None:
		break;
	}
	return text;
}
QSECTION FASTCALL_A void printErrorHead(uint depth)
{
	eprintf("ERROR @ Parser: token %3u\n", depth);
}
QSECTION FASTCALL_AD void toknextInexist(QToken tok, uint depth)
{
	if (!tok->next)
	{
		static char*PrematEnd = "Unexpected end of input\n";
		printErrorHead(depth);
		eputs(PrematEnd);
		die();
	}
}
QSECTION FASTCALL_ADC void VarDef_check_err(char* str0, int depth, int assert)
{
	if (!assert)
		return;
	qlogx(print4digit, depth);
	printErrorHead(depth);
	eputs(str0);
	eputs(" when declaring variable.\n");
	die();
}
QSECTION FASTCALL_AD void parseSyntaxNext(QToken*tok, uint*depth)
{
	toknextInexist(*tok,*depth);
	NextItem((*tok));
	(*depth)++;
}
QSECTION char*VarDef_err1_str0 = "Encountered non QbKey name";
QSECTION char*VarDef_err1_str1 = "Assignment operation not found";
QSECTION char*VarDef_err1_str2 = "Type mismatch or invalid syntax";
QSECTION char*VarDef_err1_str3 = "Mistyped array syntax";
QSECTION char*Unclosedstatement = "Unclosed statement\n";
QSECTION FASTCALL_AD void checkStatementCapoff(QToken tok, uint depth)
{
	if (tok->type != QTokOp || tok->op != QOpSEnd)
	{
		qlogx(print4digit, depth);
		printErrorHead(depth);
		eputs(Unclosedstatement);
		die();
	}
}
QSECTION char* newNodePrint = "New node: %3u, name: %p, value: %p\n";
// somehow doesn't work
QSECTION void finishVarStatement(QToken*tok, uint*depth, QNode node, char isArray)
{
	parseSyntaxNext(tok,depth);
	checkStatementCapoff(*tok,*depth);
	if (!isArray)
		qlogx(newNodePrint,
			node->type, node->name, node->number);
}
/**
 *  \brief Read tokens and compile data from them
 *  
 *  \param [in] tok List of tokens
 *  \return List of items
 *  
 *  \details Read tokens from tokenizer and check  \
 *           for statements to compile the script  \
 *           into data that's then written into a file
 */
QSECTION FASTCALL_A QNode parse(QToken tok)
{
	QNode items = malloc(QSIZEOF(QNode));
	QNode node = items;
	uint tokdepth = 0;
	while (tok)
	{
		node->always20 = 0x2000;
		node->parent = 0; //BE, write parent name in WriteQB
						// where a name will be specified
						// AND BASICALLY GO UNUSED
		node->pad10 = 0;
		node->next = 0;
		//qlogx("%4u: token: type: %2u, %p\n", tokdepth, tok->type, tok->value);
		qlogx("%4u: ", tokdepth);
		char isArray = 0;
		static char*gotArray = "got array\n";
		switch (tok->type)
		{
		case QTokKey:
			//qlogx("      op   , type :   %2u\n", tok->op);
			switch (tok->nkey)
			{
				// Parse 32-bit vars
				{
					//
					// int player = 0;
					// ---
				case CRCD(0x365AA16A,"float"):
					node->type = QTypeFloat;
					goto Var32Define;
				case CRCD(0x19918CAE,"qbkey"):
					node->type = QTypeQbKey;
					goto Var32Define;
				case CRCD(0xEBAE254E,"int"):
					// variable declaration
					node->type = QTypeInt;
				Var32Define:; // <-- lol V
					parseSyntaxNext(&tok,&tokdepth);
					// int player = 0;
					//     ------
					// or
					// int[] numbers = [ 0,0,0 ];
					//    -
					if (tok->type == QTokOp &&
						tok->op == QOpABeg)
					{
						parseSyntaxNext(&tok,&tokdepth);
						// int[] numbers = [ 0,0,0 ];
						//     -
						VarDef_check_err(VarDef_err1_str3,
							tokdepth,
								tok->type == QTokOp &&
								tok->op != QOpAEnd);
						isArray = 1;
						parseSyntaxNext(&tok,&tokdepth);
						// int[] numbers = [ 0,0,0 ];
						//       -------
					}
					VarDef_check_err(VarDef_err1_str0,
						tokdepth,tok->type != QTokKey);
					node->name = tok->nkey;
					parseSyntaxNext(&tok,&tokdepth);
					// int player = 0;
					//            -
					VarDef_check_err(VarDef_err1_str1,
						tokdepth,tok->type != QTokOp || tok->op != QOpSet);
					parseSyntaxNext(&tok,&tokdepth);
					// int player = 0;
					//              -
					if (isArray)
					{
						// int player = [ 0,0,0 ];
						//              -
						VarDef_check_err(VarDef_err1_str3,
							tokdepth,
								tok->type == QTokOp &&
								tok->op != QOpABeg);
						parseSyntaxNext(&tok,&tokdepth);
						// int player = [ 0,0,... ];
						//                -
						
						QArray arr = malloc(QSIZEOF(QArray));
						node->data = arr;
						arr->always01 = 0x0100;
						arr->type = node->type;
						node->type = QTypeQbArray;
						arr->data = malloc(1<<2);
						arr->count = 0;
						
						qlog(gotArray);
						
						if (tok->type == QTokOp &&
							tok->op == QOpAEnd)
						{
							goto ZeroValues;
						}
						while (1)
						{
							// int player = [ 0,0,... ];
							//                -
							switch (node->type)
							{
							case QTypeInt:
								if (tok->type != QTokInt)
									goto QArrAssignNotMatching;
								break;
							case QTypeQbKey:
								if (tok->type != QTokKey)
									goto QArrAssignNotMatching;
								break;
							case QTypeFloat:
								if (tok->type != QTokFloat)
									goto QArrAssignNotMatching;
								break;
							QArrAssignNotMatching:
								VarDef_check_err(VarDef_err1_str2,
									tokdepth,1);
							}
							arr->data = realloc(arr->data,(arr->count+1)<<2);
							// ^ could this be wasteful?
							arr->numbers[arr->count] = tok->number;
							//qlogx("      element %3u: %08X\n",arr->count,tok->number);
							(arr->count)++;
							parseSyntaxNext(&tok,&tokdepth);
							// int player = [ 0,0,... ];
							//                 -  or  -
							if (tok->type == QTokOp &&
								tok->op == QOpDlim)
							{
								parseSyntaxNext(&tok,&tokdepth);
							}
							else
								if (tok->type == QTokOp &&
									tok->op == QOpAEnd)
							{
								break;
							}
							else
							{
								VarDef_check_err(VarDef_err1_str3,
									tokdepth, 1);
							}
							
						}
					}
					else
					{
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
							VarDef_check_err(VarDef_err1_str2,
								tokdepth,1);
						}
						node->value = tok->value;
					}
					ZeroValues:
					finishVarStatement(&tok, &tokdepth, node, isArray);
					break;
				}
				// dynamic values
				{
				// string text = "Hello, World!";
				// ------
				case CRCD(0x61414D56,"string"):
					node->type = QTypeCString;
					
					parseSyntaxNext(&tok,&tokdepth);
					
					// or
					// string[] texts = ['A','B','C'];
					if (tok->type == QTokOp &&
						tok->op == QOpABeg)
					{
						parseSyntaxNext(&tok,&tokdepth);
						// string[] texts = ['A','B','C'];
						//       --
						VarDef_check_err(VarDef_err1_str3,
							tokdepth,
								tok->type == QTokOp &&
								tok->op != QOpAEnd);
						isArray = 1;
						parseSyntaxNext(&tok,&tokdepth);
						// string[] texts = ['A','B','C'];
						//          -----
						// or
					}
					// string text = "Hello, World!";
					//        ----
					VarDef_check_err(VarDef_err1_str0,
						tokdepth,tok->type != QTokKey);
					node->name = tok->nkey;
					parseSyntaxNext(&tok,&tokdepth);
					// string text = "Hello, World!";
					//             -
					VarDef_check_err(VarDef_err1_str1,
						tokdepth,tok->type != QTokOp || tok->op != QOpSet);
					parseSyntaxNext(&tok,&tokdepth);
					// string text = "Hello, World!";
					//               ---------------
					
					if (isArray)
					{
						// string[] texts = ['A','B','C'];
						//                  -
						VarDef_check_err(VarDef_err1_str3,
							tokdepth,
								tok->type == QTokOp &&
								tok->op != QOpABeg);
						parseSyntaxNext(&tok,&tokdepth);
						
						QArray arr = malloc(QSIZEOF(QArray));
						node->data = arr;
						arr->always01 = 0x0100;
						arr->type = node->type;
						node->type = QTypeQbArray;
						arr->data = malloc(1<<2);
						arr->count = 0;
						
						qlog(gotArray);
						
						if (tok->type == QTokOp &&
							tok->op == QOpAEnd)
						{
							goto ZeroValuesAStr;
						}
						while (1)
						{
							// string[] texts = ['A','B','C'];
							//                   ---
							VarDef_check_err(VarDef_err1_str2,
								tokdepth,node->type == QTypeCString);
							
							arr->data = realloc(arr->data,(arr->count+1)<<2);
							
							arr->strings[arr->count] = tok->string;
							//qlogx("      element %3u: %s\n",arr->count,tok->string);
							(arr->count)++;
							parseSyntaxNext(&tok,&tokdepth);
							// string[] texts = ['A','B','C'];
							//                      -  or   -
							if (tok->type == QTokOp &&
								tok->op == QOpDlim)
							{
								parseSyntaxNext(&tok,&tokdepth);
							}
							else
								if (tok->type == QTokOp &&
									tok->op == QOpAEnd)
							{
								break;
							}
							else
							{
								VarDef_check_err(VarDef_err1_str3,
									tokdepth, 1);
							}
							
						}
					}
					else
					{
						VarDef_check_err(VarDef_err1_str2,
							tokdepth,tok->type != QTokStr);
						node->value = tok->value;
					}
					
					ZeroValuesAStr:
					finishVarStatement(&tok, &tokdepth, node, isArray);
					break;
				/* struct items = {
				** ------	int i = 4;
				** };
				*/
				//case CRCD(0x456D28D1,"struct"):
					//node->type = QTypeQbStruct;
					//break;
					//this will have to result in
					//all this declaration code
					//being its own function(s)
					//just like logger and qdb
					//
					//wanted to get pair and vector
					//done first, so
					//
					//for qbkeyref, look for * or &
					//after the type or value is specified
				}
				default:
					printErrorHead(tokdepth);
					eputs("Invalid syntax\n");
					die();
			}
			break;
		case QTokEOF:
			node->next = (void*)0xFFFFFFFF;
			goto EndItAll;
		default:
			printErrorHead(tokdepth);
			eputs("Invalid syntax\n");
			die();
		}
		if (tok->next)
		{
			node->next = malloc(QSIZEOF(QNode));
			NextItem(node);
		}
		NextItem(tok);
		tokdepth++;
	}
	//qlog("EOF\n");
	EndItAll:
	return items;
}
QSECTION FASTCALL_AD checkdbg(QDbg dbg,uint key)
{
	while (dbg)
	{
		if (dbg->key == key)
			return 1; // return true if a key already exists
		NextItem(dbg);
	}
	return 0;
}
QSECTION QKey kwkeys[] = {
	// I NEED TO FIND OUT HOW TO
	// MAKE A MACRO THAT MAKES A
	// CONSTANT STRING TO A CONSTANT KEY
	// BLACK MAGIC CONSTEXPR ON C99
	CRCD(0xEBAE254E,"int"),
	CRCD(0x365AA16A,"float"),
	CRCD(0x19918CAE,"qbkey"),
	CRCD(0x61414D56,"string"),
	CRCD(0xF6A5E196,"pair"),
	CRCD(0xE491B7A4,"vector"),
	CRCD(0xAEC8F983,"if"),
};
QSECTION FASTCALL_A isKeyword(QKey key)
{
	for (int i = 0; i < kwkeys[i]; i++)
	{
		if(key == kwkeys[i])
			return 1;
	}
	return 0;
}
/**
 *  \brief Perform full compilation of a script string.
 *  
 *  \param [in] scr Text to parse
 *  \param [in] dbg List to write debug names to. If 0, debug names will not be written.
 *  \return List of compiled values/items
 *  
 *  \details ok i regret wanting to document my code i give up on saying anything else here help me please im in a mental institution blinks twice help please kill
 */
QSECTION FASTCALL_AD QNode eval_scr(char*scr, QDbg*dbg)
{
	if (dbg)
		*dbg = calloc(1,QSIZEOF(QDbg));
	//qlog(scr);
	char*lp = scr;
	QToken tokens;
	QToken tmptok;
	char firstitem = 1;
	QDbg tmpdbg;
	if (dbg)
		tmpdbg = *dbg;
	qlog("eval:\ntokenizing:\n");
	while (*lp)
	{
		if (!firstitem)
		{
			tmptok->next = malloc(QSIZEOF(QToken));
			NextItem(tmptok);
			tmptok->next = 0;
		}
		else
		{
			tmptok = malloc(QSIZEOF(QToken));
			tokens = tmptok;
			firstitem = 0;
		}
		qlogx("%4u: ", lp - scr);
		lp = tokenize(lp, tmptok);
		if (dbg)
			if (tmptok->type == QTokKey/* && !isKeyword(tmptok->nkey)*/)
			{
				// figure out how to not add keys for the
				// first part of variable declaration statements
				// OR JUST ACTUALLY GO WITH THE ORIGINAL SYNTAX STUPID POS
				// "YOU ARE DEVIATING FROM THE SYNTAX :CRINGE:" - zedek
				if (!checkdbg(*dbg,tmptok->nkey)) // disallow dupes
				{
					tmpdbg->name = tmpname;
					tmpdbg->key  = tmptok->nkey;
					tmpdbg->next = calloc(1,QSIZEOF(QDbg));
					NextItem(tmpdbg);
				}
			}
		// have to add EOF token whenever there's
		// extra space at the end of a file
		// because next token is initialized
		// and the tokenizer reads meaningless text
	}
	qlog("end of file\n");
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
			qlogx("key  , key  : %08X\n", test->nkey);
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
	qlog("parsing:\n");
	QNode items = parse(tokens);
	// should i free memory here
	return items;
}
QSECTION FASTCALL_A void AlignFile(FILE*f,char bits)
{
	int fnull[] = {0,0,0,0,0,0,0,0};
	// bits == 2, align to 4 bytes
	bits = 1 << bits;
	char mask = bits-1;
	char align = ftell(f);
	if (align&mask)
		fwrite(fnull, 1, bits-(align&mask), f);
}
/**
 *  \brief Write list of values/items of a compiled script to a new file.
 *  
 *  \param [in] items List of items to write
 *  \param [in] fname File name
 *  \param [in] name  CRC of script name
 *  \return None
 */
QSECTION FASTCALL_ADC void WriteQB(QNode items, char*fname, QKey name)
{
	FILE*qf = fopen(fname, "wb");
	if (!qf)
	{
		eputs("ERROR: Failed to create file.\n");
		die();
	}
	{
		int gaps[] = {0,0};
		//  heh
		fwrite(&gaps,sizeof(int),2,qf);
		fwrite(qhead,sizeof(char),sizeof(qhead),qf);
	}
	if (name)
		BSWAP(name);
	for (; items && items->next != (void*)-1; NextItem(items))
	{
		items->name = Eswap(items->name);
		items->parent = name;
		switch (items->type)
		{
			// 32 bit / fixed values
			default:
				items->number = Eswap(items->number);
				fwrite(items, _sizeofQNode, 1, qf);
				items->number = Eswap(items->number);
				break;
			// dynamic / pointed values
			case QTypeCString:
			{
				char*test;
				test = items->string;
				items->number = Eswap(ftell(qf)+0x14);
				fwrite(items, _sizeofQNode, 1, qf);
				items->string = test;
				fwrite(test, 1, strlen(test)+1, qf);
				AlignFile(qf,2);
			}
				break;
			case QTypeQbArray:
			{
				QArray arr;
				arr = items->data;
				items->number = Eswap(ftell(qf)+0x14);
				fwrite(items, _sizeofQNode, 1, qf);
				items->data = arr;
				arr->count = Eswap(arr->count);
				int* ptr = arr->numbers;
				// weird
				// and unnecessary just to satisfy stdio
				arr->numbers = (int*)Eswap(ftell(qf)+0xC);
				int thxNS = _sizeofQArray;
				if (Eswap(arr->count) < 2)
					thxNS -= 4;
				fwrite(arr, thxNS, 1, qf);
				arr->count = Eswap(arr->count);
				if (arr->type == QTypeCString)
				{
					char**strs = copy(ptr,arr->count<<2);
					int*strlens = malloc(arr->count<<2); // STACK ARRAY MAKES .TEXT LARGER
					int stroff = 0;
					for (int i = 0; i < arr->count; i++)
					{
						ptr[i] = Eswap(ftell(qf)+(arr->count<<2)+stroff);
						strlens[i] = strlen(strs[i])+1;
						stroff += strlens[i];
					}
					fwrite(ptr, sizeof(int), arr->count, qf);
					arr->strings = strs;
					for (int i = 0; i < arr->count; i++)
					{
						fwrite(strs[i], sizeof(char), strlens[i], qf);
					}
					AlignFile(qf,2);
				}
				else
				{
					for (int i = 0; i < arr->count; i++)
						ptr[i] = Eswap(ptr[i]);
					fwrite(ptr, sizeof(int), arr->count, qf);
				}
			}
				break;
		}
		items->name = Eswap(items->name);
		items->parent = Eswap(name);
	}
	//if (name)
		//BSWAP(name); // ...
	int qsize = ftell(qf);
	fseek(qf, 4, SEEK_SET);
	qsize = Eswap(qsize);
	fwrite(&qsize, sizeof(int), 1, qf);
	//qsize = Eswap(qsize);
	fclose(qf);
}
QSECTION const char* dbg_hd0 = "[LineNumbers]\n";
QSECTION const char* dbg_hd1 = "[Checksums]\n";
QSECTION char*nl = "\n\0";
/**
 *  \brief Write debug names from a compiled script to a file.
 *  
 *  \param [in] dbg   List of debug names
 *  \param [in] fname File name
 *  \return None
 */
QSECTION FASTCALL_AD void WriteDBG(QDbg dbg, char*fname)
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
