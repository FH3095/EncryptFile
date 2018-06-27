#pragma once

#include <list>
#include <string>
#include <memory>
#include <tuple>

class CmdParser {
private:
	typedef std::list<std::tuple<std::wstring, std::shared_ptr<std::wstring>>> ListType;
	ListType parameters;
public:
	CmdParser(const int argc, char* argv[]);

	bool hasParameter(const wchar_t* const param);
	std::shared_ptr<const std::wstring> getParameter(const wchar_t* const param);
};
