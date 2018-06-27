#pragma once

#include <string>
#include <vector>

class Convert {
private:
	Convert() {
	};

	template <typename From, typename To, typename CTypeChar>
	static std::basic_string<To> convertChars(const From* const str, const To fillCharacter, To(*func)(const std::ctype<CTypeChar>&, From)) {
		std::locale loc;
		std::basic_string<From> src = std::basic_string<From>(str);
		size_t srcLen = src.length();
		std::vector<To> dst;

		dst.resize(srcLen + 1, fillCharacter);

		const std::ctype<CTypeChar>& facet = std::use_facet<std::ctype<CTypeChar>>(loc);
		for (size_t i = 0; i < srcLen; ++i) {
			dst[i] = (*func)(facet, src.at(i));
		}
		return std::basic_string<To>(dst.data());
	}

	static char wcharNarrow(const std::ctype<wchar_t>& ctype, wchar_t c) {
		return ctype.narrow(c);
	}

	static wchar_t wcharWiden(const std::ctype<wchar_t>& ctype, char c) {
		return ctype.widen(c);
	}

public:
	static std::wstring charToWChar(const char* const str);
	static std::string wcharToChar(const wchar_t* const str);
};
