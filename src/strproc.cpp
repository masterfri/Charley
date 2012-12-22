#include "strproc.h"

SpLpcStr GetLex(SpLpcStr lpStr, SpLpcStr lpcSep, SpWord * nsep)
{
	SpLpcStr p1;
	while (*lpStr) {
		p1 = lpcSep;
		if (nsep) *nsep = 0;
		while (*p1) {
			if (*(p1++) == *lpStr) {
				return lpStr;
			}
			if (nsep) {
				(*nsep)++;
			}
		}
		lpStr++;
	}
	return 0;
}

SpLpcStr GetLastWord(SpLpcStr lpStr, SpLpcStr lpcSep, SpWord * nsep)
{
	SpLpcStr p = lpStr + StrLen(lpStr) - 1, p1;
	while (p >= lpStr){
		p1 = lpcSep;
		if (nsep) *nsep = 0;
		while (*p1){
			if (*(p1++) == *p) {
				return p + 1;	
			}
			if (nsep) {
				(*nsep)++;
			}
		}
		p--;
	}
	return 0;
}

SpLpcStr GetLexS(SpLpStr lpStr, SpLpcStr * lpcSep, SpWord * nsep)
{	
	SpLpcStr * s;
	while (*lpStr) {
		s = lpcSep;
		if (nsep) * nsep = 0;
		while (*s) {
			if (CmpWhileStr2(lpStr, *s)) {
				return lpStr;
			}
			s++; 
			if (nsep) {
				(*nsep)++;
			}
		}
		lpStr++;
	}
	return 0;
}

SpLpcStr GetLastWordS(SpLpcStr lpStr, SpLpcStr * lpcSep, SpWord * nsep) 
{
	SpLpcStr p = lpStr + StrLen(lpStr) - 1;
	SpLpcStr *s;
	while (p >= lpStr) {
		s = lpcSep;
		if (nsep) {
			*nsep = 0;
		}
		while (*s) {
			if (CmpWhileStr2(p, *s)) {
				return p + StrLen(*s);
			}
			s++; 
			if (nsep) {
				(*nsep)++;
			}
		}
		p--;
	}
	return 0;
}

SpVoid CopyFromTo(SpLpcStr lpcBeg, SpLpcStr lpcEnd, SpLpStr lpBuffer)
{
	while (lpcBeg < lpcEnd) {
		*(lpBuffer++) = *(lpcBeg++);
	}
	*lpBuffer = 0;
}

SpByte CmpWhileStr2(SpLpcStr lpcStr1, SpLpcStr lpcStr2)
{	
	while (*lpcStr2) {
		if (*(lpcStr1++) != *(lpcStr2++)) {
			return 0;			
		}
	}
	return 1;
}

SpVoid Filter(SpLpStr lpStr, SpLpcStr skipped, SpByte just_one)
{
	SpLpStr p2 = lpStr, sk;
	SpChar fsk;
	while (*lpStr) {
		sk = (SpLpStr)skipped;
		fsk = 0;
		while (*sk) {
			if (*lpStr == *sk) {
				fsk = *sk; 
				break;
			}
			sk++;
		}
		if (fsk) {
			if (just_one) {
				*(p2++) = *(lpStr++);
			}
			while (*lpStr == fsk) {
				lpStr++;
			}
			continue;
		}
		*(p2++) = *(lpStr++);		
	}
	*p2 = 0;
}

SpVoid Replace(SpLpStr lpStr, SpLpcStr symbols, SpChar repl)
{
	SpLpcStr p1;
	while (*lpStr) {
		p1 = symbols;		
		while (*p1) {
			if (*p1 == *lpStr) {
				*lpStr = repl; 
				break;
			}
			p1++; 			
		}
		lpStr++;
	}	
}

SpDword GetStrIndex(SpLpcStr * StrList, SpLpcStr szKey)
{
	SpDword ind;
	for (ind = 0; *StrList; StrList++, ind++) {
		if (!StrCmp(* StrList, szKey)) {
			return ind;
		}
	}
	return 0xffffffff;
}

SpDword GetStr(SpFile f, SpChar * buf, SpDword max, SpByte copy_rn)
{
	SpDword iter = 0;
	SpChar lbuf;
	while (fread(&lbuf, sizeof(SpChar), 1, f) == sizeof(SpChar)) {
		if (iter < max - 1) {
			if (copy_rn || !(lbuf == 13 || lbuf == 10)) {
				buf[iter] = lbuf;
				iter++;
			}
		}
		if (lbuf == 10) {
			break;
		}
	}
	buf[iter] = 0;
	return iter;
}

SpDword PutStr(SpFile f, SpChar * buf, SpByte ins_rn)
{
	SpDword size = StrLen(buf);	
	SpChar rn[] = {13, 10};
	SpDword r = fwrite(buf, size * sizeof(SpChar), 1, f);
	if (ins_rn) {
		fwrite(rn, sizeof(SpChar), 2, f);
	}
	return (SpDword)r;
}

SpByte ReadWhile(SpFile f, SpLpcStr sym, SpLpStr buffer)
{
	SpChar buf;
	SpLpStr p;
	SpByte res;
	while (fread(&buf, sizeof(SpChar), 1, f) == sizeof(SpChar)) {
		p = (SpLpStr)sym;
		res = 0;
		while (*p) {
			if (*p == buf) {
				if (buffer) {
					*buffer = 0;									
				}
				return res;
			}
			res++;	
			p++;
		}
		if (buffer) { 
			*(buffer++) = buf;
		}
	}
	return 0xff;
}

SpByte Skip(SpFile f, SpLpcStr skipped)
{
	SpChar buf;
	SpLpStr p;
	SpByte fnd;	
	while (fread(&buf, sizeof(SpChar), 1, f) == sizeof(SpChar)) {
		p = (SpLpStr)skipped;	
		fnd = 0;
		while (*p) {
			if (*p == buf) { 
				fnd = 1; 
				break;
			}
			p++;
		}
		if (!fnd) {
			fseek(f, -(SpLong)sizeof(SpChar), SEEK_CUR);
			return 1;
		}
	}
	return 0;
}

SpVoid StrCpy(SpLpStr szDet, SpLpcStr szSou)
{
	do {
		*(szDet++) = *szSou; 
	} while (*(szSou++));
}

SpLpcStr StrCmp(SpLpcStr szStr1, SpLpcStr szStr2)
{
	do {
		if (*(szStr1++) != *(szStr2++)) {
			return szStr1 - 1;
		}
	} while (*szStr1);
	return NULL;
}

SpDword StrLen(SpLpcStr szStr)
{
	SpDword res = 0;
	while (*(szStr++)) {
		res++;
	}
	return res;
}

SpVoid StrCat(SpLpStr szStr1, SpLpcStr szStr2)
{
	StrCpy(szStr1 + StrLen(szStr1), szStr2);	
}

SpVoid DataScript::GetChar()
{
	if (!ds_file) {
		LastErrorCode = 1;
		StrCpy(LastErrorStr, (SpLpStr)"File error.");
	} else if (fread(&Look, sizeof(SpChar), 1, ds_file) != sizeof(SpChar)) {
		LastErrorCode = 2;
		StrCpy(LastErrorStr, (SpLpStr)"Can read symbol.");
	}
	if (Look == 10) nLine++;
}

SpVoid DataScript::Expected(SpLpcStr msg)
{
	sprintf(LastErrorStr, "%s expected.", msg);
	LastErrorCode = 3;
}

SpVoid DataScript::Match(SpChar x)
{
	if (Look == x) {
		GetChar();
		SkipSpace();
	} else {
		SpChar buf[5];
		sprintf(buf, "'%c'", x);
		Expected(buf);
	}
}

SpByte DataScript::IsAlpha(SpChar x)
{
	return ((x >= 'a' && x <= 'z') ||
			(x >= 'A' && x <= 'Z') ||
			(x >= 'à' && x <= 'ÿ') ||
			(x >= 'À' && x <= 'ÿ') ||
			(x == '_'));
}

SpByte DataScript::IsSpace(SpChar x)
{
	return (x == ' ' || x == '\t' || x == '\r' || x == '\n');
}

SpByte DataScript::IsDigit(SpChar x)
{
	return (x >= '0' && x <= '9');
}

SpByte DataScript::IsAlNum(SpChar x)
{
	return (IsAlpha(x) || IsDigit(x));
}

SpVoid DataScript::GetName(SpLpStr Token)
{
	SpByte i = 0;
	if (!IsAlpha(Look)) {
		Expected((SpLpStr)"Name"); 
		return; 
	}
	while (IsAlNum(Look)) {
		Token[i++] = Look;
		GetChar();
	}
	Token[i] = 0;
	SkipSpace();	
}

SpLpVoid DataScript::GetNum(SpLpByte lpbType)
{
	SpLpVoid lpvNum;
	* lpbType = DT_LONG;
	SpLong intpart = 0;
	SpShort sign = 1;
	SpDouble dblpart = 0;
	if (Look == '-') {
		sign = -1;
		GetChar();
	}
	if (!IsDigit(Look)) {
		Expected((SpLpStr) "Digit"); 
		return 0;
	}
	while (IsDigit(Look)) {
		intpart = intpart * 10 + (Look - 0x30);
		GetChar();
	}
	if (Look == '.') {
		GetChar();
		* lpbType = DT_DOUBLE;
		dblpart = GetDblPart();
		if (LastErrorCode) return 0;
		lpvNum = malloc(sizeof(SpDouble));
		dblpart += (SpDouble)intpart;
		dblpart *= sign;
		memcpy(lpvNum, &dblpart, sizeof(SpDouble));
	} else {
		intpart *= sign;
		lpvNum = malloc(sizeof(SpLong));
		memcpy(lpvNum, &intpart, sizeof(SpLong));
	}
	SkipSpace();
	return lpvNum;
}

SpDouble DataScript::GetDblPart()
{
	if (!IsDigit(Look)) {
		Expected((SpLpStr)"Digit"); 
		return 0;
	}
	SpDouble div = 0.1, res = 0;
	while (IsDigit(Look)) {
		res += (Look - 0x30) * div;
		div *= 0.1;
		GetChar();
	}
	return res;
}

SpVoid DataScript::SkipSpace()
{
	do {
		while (IsSpace(Look)) {
			GetChar();
		}
		if (Look == '#') {
			do {
				GetChar();
			} while (Look != 13);
			if (Look == 10) {
				GetChar();
			}
		} else {
			break;
		}
	} while (1);
}

SpLpVoid DataScript::GetData(SpLpByte lpbType)
{
	SpLpVoid lpvData;
	if (Look == '"') {
		GetChar();
		lpvData = GetString();
		Match('"');		
		*lpbType = DT_STRING;
	} else {
		lpvData = GetNum(lpbType);		
	}
	return lpvData;
}

SpLpVoid DataScript::ReadArray(SpLpByte lpbType)
{
	SpByte type, sourcetype;
	SpLong * lpsize = (SpLong*)GetNum(&type), size;
	if (LastErrorCode) {
		return 0;
	}
	size = *lpsize;
	free(lpsize);
	if (size <= 0 || type != DT_LONG) {		
		StrCpy(LastErrorStr, (SpLpStr)"Invalid array size.");
		LastErrorCode = 4;
		return 0;
	}
	Match(']'); 
	if (LastErrorCode) {
		return 0;
	}
	Match('='); 
	if (LastErrorCode) {
		return 0;
	}
	Match('{'); 
	if (LastErrorCode) {
		return 0;
	}
	SpLpVoid * array = (SpLpVoid *)malloc((size + 1) * sizeof(SpLpVoid));
	memset(array, 0, (size + 1) * sizeof(SpLpVoid));
	array[size] = 0;
	array[0] = GetData(&sourcetype);
	if (LastErrorCode) {
		free(array);
		return 0;
	}
	for (SpLong i = 1; i < size; i++) {
		if (LastErrorCode) {
			for (SpLong j = 0; j < i; j++) {
				if (array[j]) {
					free(array[j]);
				}
			}
			free(array);
			return 0;
		}
		Match(',');
		if (!LastErrorCode) {
			array[i] = GetData(&type);
			if (type != sourcetype){
				StrCpy(LastErrorStr, (SpLpStr) "Elements of array have various type.");
				LastErrorCode = 5;
			}
		}
	}
	Match('}');
	* lpbType = sourcetype | DT_ARRAY;
	return array;
}

SpLpVoid DataScript::GetString()
{
	SpLpStr lpStr;
	SpChar str[MAX_STRING_LEN];
	SpDword iterator = 0;
	while (Look != '"'){
		str[iterator++] = Look;
		GetChar();
	}
	str[iterator] = 0;
	lpStr = (SpLpStr)malloc(sizeof(SpChar) * (iterator + 1));
	StrCpy(lpStr, str);
	return (SpLpVoid)lpStr;
}

SpVoid DataScript::Assignment(SpLpStr lpszName, SpLpByte lpbType, SpLpVoid& lpvData)
{	
	GetName(lpszName);
	if (LastErrorCode) {
		return;
	}
	if (Look == '=') {
		Match('=');
		lpvData = GetData(lpbType);
		if (!LastErrorCode) {
			Match(';');	
		}
	} else if (Look == '{') {
		* lpbType = DT_STRUCT;
		Match('{');
		lpvData = (SpLpVoid)DataBlock();
		if (!LastErrorCode) {
			Match('}');	
		}
	} else if (Look == '[') {
		Match('[');
		lpvData = ReadArray(lpbType);
		if (!LastErrorCode) {
			Match(';');
		}
	} else {
		Expected((SpLpStr) "'{' or '='");	
	}
}

DataScript::DSNode * DataScript::DataBlock()
{
	DSNode * last = 0, * head = 0;
	SpLpVoid lpvData;
	SpChar szName[64];
	SpByte bType;
	do {
		Assignment(szName, &bType, lpvData);
		if (LastErrorCode) {
			DeleteTree(head);
			return NULL;
		}
		last = AddNode(last, szName, bType, lpvData);
		if (!head) {
			head = last;
		}
	} while (Look != '}');
	return head;
}

SpVoid DataScript::ReadScript()
{
	SpChar szKeyWord[100];
	GetChar();
	nLine = 1;
	SkipSpace();
	GetName(szKeyWord);
	if (StrCmp(szKeyWord, (SpLpStr) "datascript")) {
		Expected((SpLpStr) "Keyword 'datascript'");
		LastErrorCode = 5;
		return;
	}
	if (!LastErrorCode) {
		Match('{');
		if (!LastErrorCode) {
			Head = DataBlock();
		}
	}
}

SpByte DataScript::Read(SpLpcStr szFile)
{
	ds_file = fopen(szFile, "r");
	StrCpy(LastErrorStr, (SpLpStr) "Data have been read successfully.");
	if (!ds_file) {
		sprintf(LastErrorStr, "Can not open file %s.", szFile);
		LastErrorCode = 6;
		return LastErrorCode;
	}
	ReadScript();	
	fclose(ds_file);
	return LastErrorCode;
}

DataScript::DSNode * DataScript::AddNode(DSNode * last, SpLpcStr lpszName, SpByte bType, SpLpVoid lpvData)
{
	DSNode * nnode = new DSNode;
	nnode->bType = bType;
	nnode->lpvData = lpvData;
	nnode->szName = new SpChar [StrLen(lpszName) + 1];
	StrCpy(nnode->szName, lpszName);
	nnode->next = NULL;
	if (last){ last->next = nnode; }
	return nnode;
}

SpVoid DataScript::DeleteTree(DSNode * dtree)
{
	DSNode * iter = dtree, * del;
	while (iter) {
		del = iter;
		iter = iter->next;
		delete[] del->szName;
		if (del->bType == DT_STRUCT) {
			DeleteTree((DSNode * )del->lpvData);
		} else {
			if (del->bType & DT_ARRAY) {
				SpLpVoid * array = (SpLpVoid * )del->lpvData;
				SpDword i = 0;
				while (array[i]) { 
					free(array[i]); 
					i++;
				}
			}
			free(del->lpvData);
		}
		delete del;
	}
}

size_t DataScript::ExtractData(SpLpcStr szRoot, SpLpVoid lpBuffer, SpLpByte lpbType, SpNodePtr Start)
{
	return FindData(szRoot, lpBuffer, lpbType, (DSNode *)((Start)?Start:Head));
}

size_t DataScript::FindData(SpLpcStr szRoot, SpLpVoid lpBuffer, SpLpByte lpbType, DSNode * dtree)
{
	size_t size;
	SpLpStr p = (SpLpStr)szRoot, s;
	SpChar buf[64];
	if (!IsAlpha(*p)) {
		return 0;
	}
	s = buf;
	while (IsAlNum(*p)) {
		*(s++) = *(p++);
	}
	*s = 0;
	DSNode * node = FindNode(buf, dtree);
	if (!node) {
		return 0;
	}
	if (node->bType == DT_STRUCT && *p == '.') {
		return FindData(p + 1, lpBuffer, lpbType, (DSNode *)node->lpvData);
	} else if (node->bType != DT_STRUCT && *p == 0) {
		size = GetDataSize(node->lpvData, node->bType);
		if (lpBuffer) {
			memcpy(lpBuffer, node->lpvData, size);
		}
		if (lpbType) {
			*lpbType = node->bType;
		}
		return size;
	} else if (node->bType != DT_STRUCT && *p == '[') {
		p++; 
		s = buf;
		while (IsDigit(*p))	{
			*(s++) = *(p++);
		}
		*s = 0;
		if (*p == ']' && *(p+1) == 0) {
			SpLong index = atoi(buf);
			SpLpVoid * array = (SpLpVoid *) node->lpvData;
			SpByte type = node->bType & 0x0f;
			for (SpLong i = 0; i <= index; i++) {
				if (!array[i]) {
					return 0;
				}
			}
			size = GetDataSize(array[index], type);
			if (lpBuffer) {
				memcpy(lpBuffer, array[index], size);
			}
			if (lpbType) {
				*lpbType = type;
			}
			return size;
		}
		return 0;
	}
	return 0;
}

size_t DataScript::GetDataSize(SpLpVoid lpData, SpByte bType)
{
	if (bType == DT_LONG) {
		return sizeof(SpLong);
	}
	if (bType == DT_DOUBLE) {
		return sizeof(SpDouble);
	}
	if (bType == DT_STRING) {
		return sizeof(SpChar) * (StrLen((SpLpcStr)lpData) + 1);
	}
	if (bType & DT_ARRAY) {
		SpLpVoid * array = (SpLpVoid *)lpData;
		size_t size = sizeof(SpLpVoid);
		while (*(array++)) {
			size += sizeof(SpLpVoid);
		}
		return size;
	}
	return 0;
}

DataScript::DSNode * DataScript::FindNode(SpLpcStr szName, DSNode * dtree)
{
	DSNode * iter = dtree;
	while (iter) {
		if (!StrCmp(iter->szName, szName)) {
			return iter;
		}
		iter = iter->next;
	}
	return NULL;
}

DataScript::DataScript()
{
	Head = NULL;
	LastErrorCode = 0;
	enum_ptr = NULL;
	ds_file = NULL;
}

SpByte DataScript::GetLastError(SpLpStr szBuf)
{
	sprintf(szBuf, "Line %d : %s ", nLine, LastErrorStr);
	return LastErrorCode;
}

SpByte DataScript::BeginEnumeration(SpLpcStr szRoot, SpNodePtr Start) 
{
	SpLpStr p = (SpLpStr)szRoot, s;
	SpChar buf[64];
	enum_ptr = (DSNode *)((Start)?Start:Head);
	if (p) {
		while (*p) {
			if (!IsAlpha(*p)) {
				return 0;
			}
			s = buf;
			while (IsAlNum(*p)) {
				*(s++) = *(p++);
			}
			*s = 0; 
			enum_ptr = FindNode(buf, enum_ptr);
			if (!enum_ptr || enum_ptr->bType != DT_STRUCT) {
				return 0;
			}
			enum_ptr = (DSNode *)enum_ptr->lpvData;
			if (*p) {
				p++;
			}
		}
	}
	return 1;
}

SpLpVoid DataScript::Enumerate(SpLpStr szBuffer, SpLpByte lpbType)
{
	if (!enum_ptr) {
		return 0;
	}
	if (szBuffer) {
		StrCpy(szBuffer, enum_ptr->szName);
	}
	if (lpbType) {
		*lpbType = enum_ptr->bType;
	}
	SpLpVoid data = enum_ptr->lpvData;
	enum_ptr = enum_ptr->next;
	return data;
}

SpNodePtr DataScript::GoToNode(SpLpcStr szRoot, SpNodePtr Start)
{
	if (!Start) {
		Start = (SpNodePtr)Head;
	}
	SpLpStr p = (SpLpStr)szRoot, s;
	SpChar buf[64];
	if (!IsAlpha(*p)) {
		return 0;
	}
	s = buf;
	while (IsAlNum(*p))	 {
		*(s++) = *(p++);
	}
	*s = 0;
	DSNode * node = FindNode(buf, (DSNode *)Start);
	if (!node) {
		return 0;
	}
	if (node->bType == DT_STRUCT && *p == '.') {
		return GoToNode(p + 1, (DSNode *)node->lpvData);
	} 
	if (node->bType == DT_STRUCT && *p == 0) {
		return (SpNodePtr) node->lpvData;
	}
	return 0;
}
