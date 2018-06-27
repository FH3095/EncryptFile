
#include "EncryptFile.h"
#include "CmdParser.h"
#include "Convert.h"
#include <stdexcept>
#include <cstdlib>
#include <memory>
#include <iostream>
#include <sstream>
#include "SodiumClass.h"
#include "FileCrypter.h"

using namespace std;

void printHelp();

const std::wstring ENCRYPTED_FILENAME_ENDING(L".enc");

int wmain(int argc, wchar_t* argv[])
{
	try {
		CmdParser parser(argc, argv);
		//wchar_t* args[] = { L"aaa", L"-encrypt", L"-file=ToEnc.txt" };
		//CmdParser parser(3, args);
		if (parser.hasParameter(L"help")) {
			printHelp();
			return 0;
		}

		bool encrypt;
		if (parser.hasParameter(L"encrypt")) {
			encrypt = true;
		}
		else if (parser.hasParameter(L"decrypt")) {
			encrypt = false;
		}
		else {
			wcerr << L"Missing -encrypt or -decrypt parameter." << endl;
			return 0;
		}

		std::shared_ptr<const std::wstring> srcFilename = parser.getParameter(L"file");
		std::wstring dstFilename;
		if (!srcFilename || srcFilename->empty()) {
			wcerr << L"Missing -file parameter or value of -file." << endl;
			return 0;
		}
		// Check filename ends with .enc
		if (!encrypt) {
			size_t pos = srcFilename->rfind(ENCRYPTED_FILENAME_ENDING);
			if ((pos + ENCRYPTED_FILENAME_ENDING.size()) != srcFilename->size()) {
				wcerr << L"File to decrypt doesnt end with \"" << ENCRYPTED_FILENAME_ENDING << L"\"." << endl;
				return 0;
			}
			dstFilename = srcFilename->substr(0, srcFilename->size() - ENCRYPTED_FILENAME_ENDING.size());
		}
		else {
			dstFilename = *srcFilename;
			dstFilename.append(ENCRYPTED_FILENAME_ENDING);
		}

		std::shared_ptr<const std::wstring> password = parser.getParameter(L"password");
		if (!password || password->empty()) {
			std::wstringbuf buff;
			wcout << L"Enter Password: ";
			wcin.get(buff);
			wcout << endl;
			if (buff.str().length() < 1) {
				wcerr << L"Empty password is not allowed." << endl;
				return 0;
			}
			password.reset(new std::wstring(buff.str()));
		}

		FileCrypter crypter;
		if (encrypt) {
			crypter.doEncrypt(*password, *srcFilename, dstFilename);
		}
		else {
			crypter.doDecrypt(*password, *srcFilename, dstFilename);
		}

		wcout << L"Finished." << endl;
		return 0;
	}
	catch (const std::exception& e) {
		wcerr << Convert::charToWChar(e.what()).c_str() << endl;
		std::_Exit(EXIT_FAILURE);
	}
}

void printHelp() {
	wcout << L"HELP: " << endl
		<< L"\t-help: Print this help and exit" << endl
		<< L"\t[-encrypt|-decrypt]: Encrypt or decrypt file" << endl
		<< L"\t-file=Filename: File to decrypt or encrypt" << endl
		<< L"\t-password=Password: Password to use (you should pipe the password, otherwise password is visible on command line)" << endl;
}
