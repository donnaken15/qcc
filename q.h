#pragma once

typedef void* unk;
typedef unsigned int QKey;
typedef unsigned int uint;
typedef unsigned short ushort;

//typedef int QKey;
//typedef int   QTokenType;
enum QTokenType {
	QTokEOF = -1,
	//QTokNone,
	QTokOp,
	QTokKey,
	QTokInt,
	QTokFloat,
	QTokStr,
	QTokEnd,
};
enum QOps {
	QOpSet,  // =
	QOpCmp,  // ==
	QOpSEnd, // ;
	QOpColn, // :
	QOpDlim, // ,
	QOpPBeg, // (
	QOpPEnd, // )
	QOpBBeg, // {
	QOpBEnd, // }
	QOpABeg, // [
	QOpAEnd, // ]
	QOpPlus, // +
	QOpMnus, // -
	QOpAstx, // *
	QOpDvid, // /
	QOpLess, // <
	QOpGrtr, // >
	QOpNot,  // !
};
enum QNodeTypes
{
	QTypeUndefined = 0x0,
	QTypeInt = 0x1,
	QTypeFloat = 0x2,
	QTypeCString = 0x3,
	QTypeWString = 0x4,
	QTypePair = 0x5,
	QTypeVector = 0x6,
	QTypeScript = 0x7,
	QTypeCFunc = 0x8,
	QTypeUnk9 = 0x9,
	QTypeQbStruct = 0xA,
	QTypeQbArray = 0xC,
	QTypeQbKey = 0xD,
	QTypeUnk20 = 0x14,
	QTypeUnk21 = 0x15,
	QTypeBinaryTree1 = 0x16,
	QTypeBinaryTree2 = 0x17,
	QTypeStringPointer = 0x1A,
	QTypeQbMap = 0x1B,
	QTypeQbKeyStringQs = 0x1C,
};

#define QLOCAL(t) __static_ ## t
#define QSIZEOF(t) sizeof(QLOCAL(t))
typedef struct {
	union {
		unk   value;
		QKey  nkey; // i dont know/forget why i named this specifically as it is
		int   number;
		float single;
		char* string;
		int   op;
	};
	uint type;
	void*next;
	//^ WHY
} __static_QToken, *QToken;
//uint _sizeofQToken; // for use with binary writing
//uint _sizeofQNode;
typedef struct {
	ushort always20;
	ushort type;
	QKey name;
	QKey parent;
	union {
		unk   value;
		QKey  key;
		int   number;
		float single;
		char* string; //
		void* data;   // for variable length data (strings,structs)
		              // this makes this value simply a pointer to the data
	};
	uint pad10;
	void*next; // should i make it this way
	           // where every struct that operates
			   // in a list has next as the last value
			   // so its easy for a function to interpret
			   // a vague use of (void*) next in a
			   // varying sized struct
			   // using sizeof(struct)-4
			   // (which would also require NextItem
			   // changing to function that way)
			   // and i just realized it works
			   // the way it does with different
			   // structs because its preprocessed
			   // to write out i->next instead
			   // of using a real function
			   // actually just realized itd be
			   // harder since sizeof isnt calculated
			   // at runtime, then next would need
			   // to be at a fixed offset
			   // and then actually mess up
			   // the writing funcs
			   // am i talking to myself??
			   // I NEED A LIFE (AMIRITE)
} __static_QNode, *QNode;
typedef struct {
	ushort always01;
	ushort type;
	uint count;
	union {
		unk  * values;
		QKey * keys;
		int  * numbers;
		float* floats;
		char **strings;
		void **data;
		QNode**structs;
	};
} __static_QArray, *QArray;
#define NextItem(i) /*if (i->next)*/ i = i->next

typedef struct {
	QKey key;
	char*name;
	void*next;
	//uint**lnum; // implement for scripts
} __static_QDbg, *QDbg;
//extern char*tmpname;

enum CharFilters {
	CF_Whitespace = 0,
	CF_Number,   // (Sk8)
	CF_Alphabet, // (Sk)8
	CF_Alphanum, // thought id be clever here to use a bitmask or something
				 // but im also checking one character at a time with this
				 // though then later enums & 0b11 are not allowed then
	CF_Syntax  = 0b000100,
	CF_String  = 0b001000,
	CF_Unknown = 0x7FFFFFFC,
	CF_None    = 0x80000000
};

#define copy(addr,size) memcpy(malloc(size),addr,size)

#define Qlogging 1

#define PREGEN_CRCTAB 0

#define BE 1
// why even

#define CRCD(c,s) (c)
// :p

#define FASTCALL_A   __attribute__ ((regparm(1)))
#define FASTCALL_AD  __attribute__ ((regparm(2)))
#define FASTCALL_ADC __attribute__ ((regparm(3)))
#define QSECTION     __attribute__ ((section(".q")))
// because why not

#if (BE == 1)
#define BSWAP(i)	__asm__ __volatile__ (\
		"  mov      %0, %%eax\n"\
		"  bswap    %%eax\n"\
		"  mov      %%eax, %0\n"\
		:"=m"(i)\
	)
QSECTION FASTCALL_A Eswap(int);
#define ESWAP(i) (((i & 0xFF) << 24) | ((i & 0xFF00) << 8) | ((i & 0xFF0000) >> 8) | ((i & 0xFF000000) >> 24))
#else
#define ESWAP(i) i
#define BSWAP(i) i
#endif
QSECTION FASTCALL_A QKey crc32(char*);
QSECTION void initCRC32();
QSECTION FASTCALL_AD  char*tokenize(char*, QToken);
QSECTION FASTCALL_A   QNode parse(QToken);
QSECTION FASTCALL_AD  QNode eval_scr(char*, QDbg*dbg);
QSECTION FASTCALL_ADC void WriteQB(QNode, char*, QKey name);
QSECTION FASTCALL_AD  void WriteDBG(QDbg dbg, char*fname);
QSECTION void die();
