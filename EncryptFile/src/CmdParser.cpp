
#include "CmdParser.h"
#include "Util.h"
#include "Convert.h"
#include <stdexcept>
#include <iostream>

using namespace std;

CmdParser::CmdParser(const int argc, char* argv[]) {
	for (int i = 1; i < argc; ++i) {
		std::wstring param(Convert::charToWChar(argv[i]));
		if (param.length() < 1) {
			continue;
		}
		if (param.at(0) != L'-') {
			Util::BuffType buff;
			buff << L"Param \"" << param.c_str() << L"\" not starting with \"-\"";
			throw Util::generateErrorMessage<std::runtime_error>(buff);
		}
		param = param.substr(1);

		std::shared_ptr<std::wstring> value;
		size_t equalSign = param.find(L'=');
		if (std::wstring::npos != equalSign) {
			if (param.length() > equalSign + 1) {
				value.reset(new std::wstring(param.substr(equalSign + 1)));
			}
			param.resize(equalSign, L'\0');
		}
		parameters.push_back(std::tuple<std::wstring, std::shared_ptr<std::wstring>>(param, value));
	}
}

bool CmdParser::hasParameter(const wchar_t* const param) {
	for (ListType::iterator it = parameters.begin(); it != parameters.end(); ++it) {
		if (std::get<0>(*it).compare(param) == 0) {
			return true;
		}
	}
	return false;
}

std::shared_ptr<const std::wstring> CmdParser::getParameter(const wchar_t* const param) {
	for (ListType::iterator it = parameters.begin(); it != parameters.end(); ++it) {
		if (std::get<0>(*it).compare(param) == 0) {
			return std::shared_ptr<const std::wstring>(std::get<1>(*it));
		}
	}
	return std::shared_ptr<const std::wstring>();
}
