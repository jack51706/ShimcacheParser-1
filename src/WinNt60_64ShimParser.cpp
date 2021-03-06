#include "WinNt60_64ShimParser.h"
#include "CacheEntryNt60_64.h"
#include "ShimCacheInfo.h"

WinNt60_64ShimParser::WinNt60_64ShimParser() {

}

bool WinNt60_64ShimParser::parse(string &binData, vector<pair<wstring, wstring>> &result) {
	if (!this->initial(binData, result))
		return false;
	u_int num = 0x01;
	u_char *begin = (u_char*)this->mbinData.c_str();
	u_char *index = begin;
	index += CACHE_HEADER_SIZE_NT6_0;
	while ((size_t)(index - begin)<binData.size() && num <= this->numEntries) {
		if (!this->extractEntry(index))
			break;
		index += NT6_0_ENTRY_SIZE64;
		num++;
	}

	return true;
}

bool WinNt60_64ShimParser::initial(string &binData, vector<pair<wstring, wstring>> &result) {
	this->mbinData = binData;
	this->numEntries = *(u_int*)(binData.c_str() + 4);
	this->result = &result;
    this->result->resize(0);
	return true;
}

bool WinNt60_64ShimParser::extractEntry(u_char *index) {
	CacheEntryNt60_64 entry;
	string path;
	wstring wpath;
	string lastModDate;
	size_t end;
	end = (char*)index - this->mbinData.c_str() + NT6_0_ENTRY_SIZE64;
	if (end > this->mbinData.size())
		return false; //overflow
	entry = *(CacheEntryNt60_64*)index;
	//make file path
	wpath = wstring((wchar_t*)(this->mbinData.c_str() + entry.dwPathOffset)
					, entry.wLength / sizeof(wchar_t));
	this->filterPath(wpath);
	//make time
	lastModDate = this->makeTime(entry.ftLastModTime);
	//put to result
    this->putToResult(lastModDate, wpath);
	return true;
}
