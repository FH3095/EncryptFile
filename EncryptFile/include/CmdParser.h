#pragma once

#include <list>
#include <string>
#include <memory>
#include <tuple>

class CmdParser {
private:
	typedef std::list<std::tuple<std::string, std::shared_ptr<std::string>>> ListType;
	ListType parameters;
public:
	CmdParser(const int argc, char* argv[]);

	bool hasParameter(const char* const param);
	std::shared_ptr<const std::string> getParameter(const char* const param);
};
