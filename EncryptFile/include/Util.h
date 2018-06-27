#pragma once

#include <sstream>
#include <string>

class Util {
private:
	Util() {
	};
public:
	typedef std::ostringstream BuffType;
	template <typename Exception>
	static Exception generateErrorMessage(const BuffType& stream) {
		return Exception(stream.str().c_str());
	}
};
