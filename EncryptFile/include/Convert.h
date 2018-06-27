#pragma once

#include <string>
#include <vector>
#include <locale>

class Convert {
private:
	Convert() {
	};

	template <typename From, typename To>
	static std::basic_string<To> convertChars(const From* const str, const To fillCharacter, To(*func)(const std::locale&, From)) {
		std::locale loc;
		std::basic_string<From> src = std::basic_string<From>(str);
		size_t srcLen = src.length();
		std::vector<To> dst;

		dst.resize(srcLen + 1, fillCharacter);

		for (size_t i = 0; i < srcLen; ++i) {
			dst[i] = (*func)(loc, src.at(i));
		}
		return std::basic_string<To>(dst.data());
	}

	static char wcharNarrow(const std::locale& loc, wchar_t c) {
		return std::use_facet<std::ctype<wchar_t>>(loc).narrow(c, '?');
	}

	static wchar_t wcharWiden(const std::locale& loc, char c) {
		return std::use_facet<std::ctype<wchar_t>>(loc).widen(c);
	}

public:
	static std::wstring charToWChar(const char* const str);
	static std::string wcharToChar(const wchar_t* const str);
};
