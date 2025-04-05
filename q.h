#pragma once

#define Qlogging 1
//#define PREGEN_CRCTAB 0

typedef void*unk;
typedef unsigned int QKey;
typedef unsigned int uint;
typedef unsigned short ushort;

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
	QOpAxsr, // .
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
/**
 *  @struct QToken
 *  @brief Struct used to read text into more computer readable values
 */
typedef struct {
	/**
	 *  @var Value
	 *  A union to define a value of various types
	 *  
	 *  @details A value that can be read as an integer, float, string, operation enum, pointer, or unknown value
	 */
	union {
		unk   value;
		QKey  nkey; // i dont know/forget why i named this specifically as it is
		int   number;
		float single;
		char* string;
		int   op;
	};
	/**
	 *  @var Token type
	 *  @details Type of token to know what type of value the token has
	 */
	uint type;
	/**
	 *  @var Pointer of next token item
	 *  @details If 0, it has no (more) leading items
	 */
	struct QLOCAL(QToken)*next;
	// ok that's kind of dumb but at least it works
} *QToken, QLOCAL(QToken);
//uint _sizeofQToken; // for use with binary writing
//uint _sizeofQNode;
/**
 *  @struct QNode
 *  @brief Struct that contains data compiled from tokens
 */
typedef struct {
	ushort always20;
	/**
	 *  @var Node type
	 *  @details Type of node to know what type of value it has
	 */
	ushort type;
	/**
	 *  @var Node name
	 *  @details CRC checksum of item name
	 */
	QKey name;
	/**
	 *  @var QB File
	 *  @details CRC checksum of the name of the file this item is stored in
	 */
	QKey parent;
	union {
		unk   value;
		QKey  key;
		int   number;
		float single;
		char* string; //
		struct QLOCAL(QArray)*data;   // for variable length data (strings,structs)
		              // this makes this value simply a pointer to the data
	};
	uint pad10;
	/**
	 *  @var Pointer of next token item
	 *  @details If 0, it has no (more) leading items
	 */
	struct QLOCAL(QNode)*next;
	// should i make it this way
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
} *QNode, QLOCAL(QNode);
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
} *QArray, QLOCAL(QArray);
typedef struct {
	unsigned char count;
	char*sequence; // QOps
} *QOpSeq, QLOCAL(QOpSeq);
#define NextItem(i) /*if (i->next)*/ i = i->next

/**
 *  @struct QDbg
 *  @brief Struct that provides human readable names of CRC checksums
 */
typedef struct {
	QKey key;
	char*name;
	struct QLOCAL(QDbg)*next;
	//uint**lnum; // implement for scripts
} *QDbg, QLOCAL(QDbg);
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

/**
 *  @def Enable script compilation logging
 */
#ifndef Qlogging
// just in case the user manually
// sets this switch from the compiler
// for whatever reason
#define Qlogging 1
#endif

/**
 *  @def Enable pregenerated CRC table
 *  @details Decide whether or not to use a pregenerated CRC table
 */
#ifndef PREGEN_CRCTAB
#define PREGEN_CRCTAB 0
#endif

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
