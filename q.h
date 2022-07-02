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
	// should i separate keywords and one symbol operations here
	QOpIf,   // if
	QOpInt,  // int
	QOpFloat,// float
	QOpKey,  // qbkey
	QOpStr,  // string
	QOpStrc, // struct
	//QOpPtr,  // qbkeyref
	QOpSet,  // =
	QOpCmp,  // ==
	QOpSEnd, // ;
	QOpPBeg, // (
	QOpPEnd, // )
	QOpBBeg, // {
	QOpBEnd, // }
	QOpABeg, // [
	QOpAEnd, // ]
};

#define QLOCAL(t) __static_ ## t
#define QSIZEOF(t) sizeof(QLOCAL(t))
typedef struct {
	union {
		unk   value;
		QKey  nkey;
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
	uint name;
	uint x278081F3;
	union {
		unk   value;
		uint  key;
		int   number;
		float single;
		char* string;
	};
	uint pad10;
	void*next;
} __static_QNode, *QNode;
typedef struct {
	uint gap;
	uint size;
	uint unknown[5];
} QHead;
typedef struct {
	QHead head;
	QNode items;
} *QFile;
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
	CF_Syntax = 0b000100,
	CF_None = 0x80000000
};

__inline Eswap(int);
#define ESWAP(i) (((i & 0xFF) << 24) | ((i & 0xFF00) << 8) | ((i & 0xFF0000) >> 8) | ((i & 0xFF000000) >> 24))
QKey crc32(char*);
void initCRC32();
char*tokenize(char*, QToken);
QNode parse(QToken);
QNode eval_scr(char*, QDbg*dbg);
void WriteQB(QNode, char*);
void WriteDBG(QDbg dbg, char*fname);
