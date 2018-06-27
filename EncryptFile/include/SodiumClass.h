#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include <sodium.h>


class Sodium {
private:
	static const unsigned int KEY_LENGTH;
	static const unsigned int PW_OPSLIMIT;
	static const unsigned int PW_MEMLIMIT;
	static const unsigned int PW_ALG;

	bool forEncryption;
	bool inProgress;
	std::vector<unsigned char> key;
	std::vector<unsigned char> salt;
	std::vector<unsigned char> header;
	crypto_secretstream_xchacha20poly1305_state state;
	void init(const std::wstring& password, const std::vector<unsigned char>& salt);
	std::vector<unsigned char> encrypt(const std::vector<unsigned char>& src, bool isLast);
	std::vector<unsigned char> decrypt(const std::vector<unsigned char>& src, bool isLast);
public:
	static const unsigned int PW_SALT_LENGTH;
	static const unsigned int HEADER_LENGTH;
	static const unsigned int ENCRYPTED_CHUNK_ADDITIONAL_SIZE;

	Sodium(const std::wstring& password);
	Sodium(const std::wstring& password, const std::vector<unsigned char>& salt, const std::vector<unsigned char>& header);
	~Sodium() noexcept(false);

	const std::vector<unsigned char>& getSalt() {
		return salt;
	}

	const std::vector<unsigned char>& getHeader() {
		return header;
	}

	bool isInProgress() {
		return inProgress;
	}

	bool isEncrypting() {
		return forEncryption;
	}

	inline std::vector<unsigned char> doWork(const std::vector<unsigned char>& src, bool isLast) {
		if (forEncryption) {
			return encrypt(src, isLast);
		}
		else {
			return decrypt(src, isLast);
		}
	}
};
