
#include "CmdParser.h"
#include "Util.h"
#include <stdexcept>
#include <iostream>

using namespace std;

CmdParser::CmdParser(const int argc, char* argv[]) {
	for (int i = 1; i < argc; ++i) {
		std::string param(argv[i]);
		if (param.length() < 1) {
			continue;
		}
		if (param.at(0) != L'-') {
			Util::BuffType buff;
			buff << "Param \"" << param.c_str() << "\" not starting with \"-\"";
			throw Util::generateErrorMessage<std::runtime_error>(buff);
		}
		param = param.substr(1);

		std::shared_ptr<std::string> value;
		size_t equalSign = param.find(L'=');
		if (std::string::npos != equalSign) {
			if (param.length() > equalSign + 1) {
				value.reset(new std::string(param.substr(equalSign + 1)));
			}
			param.resize(equalSign, L'\0');
		}
		parameters.push_back(std::tuple<std::string, std::shared_ptr<std::string>>(param, value));
	}
}

bool CmdParser::hasParameter(const char* const param) {
	for (ListType::iterator it = parameters.begin(); it != parameters.end(); ++it) {
		if (std::get<0>(*it).compare(param) == 0) {
			return true;
		}
	}
	return false;
}

std::shared_ptr<const std::string> CmdParser::getParameter(const char* const param) {
	for (ListType::iterator it = parameters.begin(); it != parameters.end(); ++it) {
		if (std::get<0>(*it).compare(param) == 0) {
			return std::shared_ptr<const std::string>(std::get<1>(*it));
		}
	}
	return std::shared_ptr<const std::string>();
}
