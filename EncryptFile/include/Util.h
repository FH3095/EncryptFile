#pragma once

#include <sstream>
#include <string>
#include "Convert.h"

class Util {
private:
	Util() {
	};
public:
	typedef std::wostringstream BuffType;
	template <typename Exception>
	static Exception generateErrorMessage(const BuffType& stream) {
		return Exception(Convert::wcharToChar(stream.str().c_str()));
	}
};
