
#include "Convert.h"
#include <array>

using namespace std;

std::wstring Convert::charToWChar(const char* const str) {
	return Convert::convertChars(str, L'\0', &Convert::wcharWiden);
}

std::string Convert::wcharToChar(const wchar_t* const str) {
	return Convert::convertChars(str, '\0', &Convert::wcharNarrow);
}
