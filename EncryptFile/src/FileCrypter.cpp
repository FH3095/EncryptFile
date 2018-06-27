
#include "FileCrypter.h"
#include "SodiumClass.h"
#include "Util.h"
#include <fstream>
#include <algorithm>

const int64_t FileCrypter::CHUNK_SIZE = 1 * 1024 * 1024;

void FileCrypter::openFiles(const std::string& source, std::ifstream& input, const std::string& dest, std::ofstream& output) {
	input.open(source, std::ifstream::in | std::ifstream::binary);
	if (!input.is_open()) {
		Util::BuffType msg;
		msg << "Cant open input-file " << source;
		throw Util::generateErrorMessage<std::runtime_error>(msg);
	}

	output.open(dest, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
	if (!output.is_open()) {
		Util::BuffType msg;
		msg << "Cant open output-file " << dest;
		throw Util::generateErrorMessage<std::runtime_error>(msg);
	}
}

void FileCrypter::doWork(const int64_t readSize, Sodium& sodium, std::ifstream& input, std::ofstream& output) {
	const int64_t startPos = input.tellg();
	checkStream(input, "tell start from input file");

	input.seekg(0, std::ifstream::end);
	checkStream(input, "seek input file to end");

	const int64_t fileSize = input.tellg();
	checkStream(input, "tell end from input file");

	input.seekg(startPos, std::ifstream::beg);
	checkStream(input, "seek input file to start");

	int64_t remaining;
	std::vector<unsigned char> encrypted;
	while ((remaining = fileSize - input.tellg()) > 0) {
		const int64_t toReadSize = std::min(readSize, remaining);
		chunk.resize(toReadSize);

		input.read(reinterpret_cast<char*>(chunk.data()), chunk.size());
		checkStream(input, "read from file");

		encrypted = sodium.doWork(chunk, remaining == toReadSize);

		output.write(reinterpret_cast<const char*>(encrypted.data()), encrypted.size());
		checkStream(output, "write to file");
	}

	output.flush();
	checkStream(output, "flush output file");
}

void FileCrypter::doEncrypt(const std::string & password, const std::string & sourceFile, const std::string & destFile) {
	std::ifstream input;
	std::ofstream output;
	openFiles(sourceFile, input, destFile, output);

	Sodium sodium(password);

	output.write(reinterpret_cast<const char*>(sodium.getSalt().data()), Sodium::PW_SALT_LENGTH);
	checkStream(output, "write salt to file");

	output.write(reinterpret_cast<const char*>(sodium.getHeader().data()), Sodium::HEADER_LENGTH);
	checkStream(output, "write header to file");

	doWork(CHUNK_SIZE, sodium, input, output);

	input.close();
	output.close();
}

void FileCrypter::doDecrypt(const std::string & password, const std::string & sourceFile, const std::string & destFile) {
	std::ifstream input;
	std::ofstream output;
	openFiles(sourceFile, input, destFile, output);

	std::vector<unsigned char> salt;
	std::vector<unsigned char> header;
	salt.resize(Sodium::PW_SALT_LENGTH, 0);
	header.resize(Sodium::HEADER_LENGTH, 0);

	input.read(reinterpret_cast<char*>(salt.data()), salt.size());
	checkStream(input, "read salt from file");

	input.read(reinterpret_cast<char*>(header.data()), header.size());
	checkStream(input, "read header from file");

	Sodium sodium(password, salt, header);

	doWork(CHUNK_SIZE + Sodium::ENCRYPTED_CHUNK_ADDITIONAL_SIZE, sodium, input, output);

	input.close();
	output.close();
}
