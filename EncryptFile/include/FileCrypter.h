#pragma once

#include "SodiumClass.h"
#include "Util.h"
#include <tuple>
#include <vector>
#include <fstream>
#include <cstdint>

class FileCrypter {
private:
	static const int64_t CHUNK_SIZE;
	static void checkStream(const std::ios& stream, const char* const fileOp) {
		if (stream.fail()) {
			Util::BuffType msg;
			msg << "Failed to " << fileOp << ". Bad? " << stream.bad();
			throw Util::generateErrorMessage<std::runtime_error>(msg);
		}
	}
	std::vector<unsigned char> chunk;
	void openFiles(const std::string& source, std::ifstream& input, const std::string& dest, std::ofstream& output);
	void doWork(const int64_t chunkSize, Sodium& sodium, std::ifstream& input, std::ofstream& output);
public:
	FileCrypter() {
		chunk.resize(CHUNK_SIZE, 0);
	}
	void doEncrypt(const std::string& password, const std::string& sourceFile, const std::string& destFile);
	void doDecrypt(const std::string& password, const std::string& sourceFile, const std::string& destFile);
};
