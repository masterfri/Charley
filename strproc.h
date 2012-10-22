#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _STR_PROC_
#define _STR_PROC_

#define DT_STRUCT 0x00
#define DT_LONG   0x01
#define DT_DOUBLE 0x02
#define DT_STRING 0x03
#define DT_ARRAY  0x10
#define MAX_STRING_LEN 1024

typedef void				SpVoid;
typedef SpVoid* 			SpLpVoid;
typedef SpLpVoid 			SpNodePtr;
typedef char 				SpChar;
typedef SpChar*			SpLpcStr;
typedef SpChar*			SpLpStr;
typedef unsigned short	SpWord;
typedef short				SpShort;
typedef unsigned char		SpByte;
typedef SpByte*			SpLpByte;
typedef unsigned int		SpDword;
typedef long int			SpLong;
typedef FILE*				SpFile;
typedef double			SpDouble;

SpLpcStr GetLex(SpLpcStr lpStr, SpLpcStr lpcSep, SpWord * nsep);	
SpLpcStr GetLastWord(SpLpcStr lpStr, SpLpcStr lpcSep, SpWord * nsep);
SpLpcStr GetLastWordS(SpLpcStr lpStr, SpLpcStr * lpcSep, SpWord * nsep);
SpLpcStr GetLexS(SpLpStr lpStr, SpLpcStr * lpcSep, SpWord * nsep);
SpVoid CopyFromTo(SpLpcStr lpcBeg, SpLpcStr lpcEnd, SpLpStr lpBuffer);
SpByte CmpWhileStr2(SpLpcStr lpcStr1, SpLpcStr lpcStr2);
SpVoid Filter(SpLpStr lpStr, SpLpcStr skipped, SpByte just_one);
SpVoid Replace(SpLpStr lpStr, SpLpcStr symbols, SpChar repl);
SpDword GetStr(SpFile f, SpChar * buf, SpDword max, SpByte copy_rn);
SpDword PutStr(SpFile f, SpChar * buf, SpByte ins_rn);
SpByte Skip(SpFile f, SpLpcStr skipped);
SpByte ReadWhile(SpFile f, SpLpcStr sym, SpLpStr buffer);
SpVoid StrCpy(SpLpStr szDet, SpLpcStr szSou);
SpLpcStr StrCmp(SpLpcStr szStr1, SpLpcStr szStr2);
SpVoid StrCat(SpLpStr szStr1, SpLpcStr szStr2);
SpDword StrLen(SpLpcStr szStr);
SpDword GetStrIndex(SpLpcStr * StrList, SpLpcStr szKey);

class DataScript {
	public:
		struct DSNode {
			SpLpStr szName;
			SpByte bType;
			SpLpVoid lpvData;
			DSNode * next;
		};
	private:		
		DSNode * Head;
		SpChar Look;
		SpByte LastErrorCode;
		SpChar LastErrorStr[64];
		SpFile ds_file;
		SpDword nLine;
		DSNode * enum_ptr;
		SpVoid GetChar();	
		SpVoid Match(SpChar x);
		SpVoid Expected(SpLpcStr msg);
		SpByte IsAlpha(SpChar x);
		SpByte IsDigit(SpChar x);
		SpByte IsSpace(SpChar x);
		SpByte IsAlNum(SpChar x);
		SpVoid GetName(SpLpStr Token);
		SpLpVoid GetNum(SpLpByte lpbType);
		SpLpVoid GetData(SpLpByte lpbType);
		SpLpVoid ReadArray(SpLpByte lpbType);
		SpLpVoid GetString();
		SpDouble GetDblPart();
		SpVoid SkipSpace();		
		SpVoid Assignment(SpLpStr lpszName, SpLpByte lpbType, SpLpVoid& lpvData);
		DSNode * DataBlock();
		SpVoid ReadScript();
		DSNode * AddNode(DSNode * last, SpLpcStr lpszName, SpByte bType, SpLpVoid lpvData);
		SpVoid DeleteTree(DSNode * dtree);
		size_t FindData(SpLpcStr szRoot, SpLpVoid lpBuffer, SpLpByte lpbType, DSNode * dtree);
		DSNode * FindNode(SpLpcStr szName, DSNode * dtree);
		size_t GetDataSize(SpLpVoid lpData, SpByte bType);				
	public:
		DataScript();
		SpNodePtr GoToNode(SpLpcStr szRoot, SpNodePtr Start = NULL);
		size_t ExtractData(SpLpcStr szRoot, SpLpVoid lpBuffer, SpLpByte lpbType, SpNodePtr Start = NULL);
		SpByte Read(SpLpcStr szFile);
		SpByte GetLastError(SpLpStr szBuf);
		SpByte BeginEnumeration(SpLpcStr szRoot, SpNodePtr Start = NULL);
		SpLpVoid Enumerate(SpLpStr szBuffer, SpLpByte lpbType);
		inline SpVoid Free() {DeleteTree(Head); Head = NULL; enum_ptr = NULL;}
		inline SpNodePtr StoreUnumPosition() {return (SpNodePtr)enum_ptr;}
		inline SpVoid RestoreUnumPosition(SpNodePtr pos) {enum_ptr = (DSNode *)pos;}
		~DataScript() {DeleteTree(Head);}
};

#endif // _STR_PROC_
