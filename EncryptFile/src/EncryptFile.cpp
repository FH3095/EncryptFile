
#include "EncryptFile.h"
#include "CmdParser.h"
#include <stdexcept>
#include <cstdlib>
#include <memory>
#include <iostream>
#include <sstream>
#include "SodiumClass.h"
#include "FileCrypter.h"

using namespace std;

void printHelp();

const std::string ENCRYPTED_FILENAME_ENDING(".enc");

int main(int argc, char* argv[])
{
	try {
		CmdParser parser(argc, argv);
		//char* args[] = { "aaa", "-encrypt", "-file=ToEnc.txt" };
		//CmdParser parser(3, args);
		if (parser.hasParameter("help")) {
			printHelp();
			return 0;
		}

		bool encrypt;
		if (parser.hasParameter("encrypt")) {
			encrypt = true;
		}
		else if (parser.hasParameter("decrypt")) {
			encrypt = false;
		}
		else {
			cerr << "Missing -encrypt or -decrypt parameter." << endl;
			return 0;
		}

		std::shared_ptr<const std::string> srcFilename = parser.getParameter("file");
		std::string dstFilename;
		if (!srcFilename || srcFilename->empty()) {
			cerr << "Missing -file parameter or value of -file." << endl;
			return 0;
		}
		// Check filename ends with .enc
		if (!encrypt) {
			size_t pos = srcFilename->rfind(ENCRYPTED_FILENAME_ENDING);
			if ((pos + ENCRYPTED_FILENAME_ENDING.size()) != srcFilename->size()) {
				cerr << "File to decrypt doesnt end with \"" << ENCRYPTED_FILENAME_ENDING << "\"." << endl;
				return 0;
			}
			dstFilename = srcFilename->substr(0, srcFilename->size() - ENCRYPTED_FILENAME_ENDING.size());
		}
		else {
			dstFilename = *srcFilename;
			dstFilename.append(ENCRYPTED_FILENAME_ENDING);
		}

		std::shared_ptr<const std::string> password = parser.getParameter("password");
		if (!password || password->empty()) {
			std::stringbuf buff;
			cout << "Enter Password: ";
			cin.get(buff);
			cout << endl;
			if (buff.str().length() < 1) {
				cerr << "Empty password is not allowed." << endl;
				return 0;
			}
			password.reset(new std::string(buff.str()));
		}

		FileCrypter crypter;
		if (encrypt) {
			crypter.doEncrypt(*password, *srcFilename, dstFilename);
		}
		else {
			crypter.doDecrypt(*password, *srcFilename, dstFilename);
		}

		cout << "Finished." << endl;
		return 0;
	}
	catch (const std::exception& e) {
		cerr << e.what() << endl;
		std::_Exit(EXIT_FAILURE);
	}
}

void printHelp() {
	cout << "HELP: " << endl
		<< "\t-help: Print this help and exit" << endl
		<< "\t[-encrypt|-decrypt]: Encrypt or decrypt file" << endl
		<< "\t-file=Filename: File to decrypt or encrypt" << endl
		<< "\t-password=Password: Password to use (you should pipe the password, otherwise password is visible on command line)" << endl;
}
