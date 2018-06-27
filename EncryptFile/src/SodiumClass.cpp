
#include "SodiumClass.h"
#include "Convert.h"
#include "Util.h"
#include <stdexcept>

const unsigned int Sodium::KEY_LENGTH = crypto_secretstream_xchacha20poly1305_KEYBYTES;
const unsigned int Sodium::HEADER_LENGTH = crypto_secretstream_xchacha20poly1305_HEADERBYTES;
const unsigned int Sodium::ENCRYPTED_CHUNK_ADDITIONAL_SIZE = crypto_secretstream_xchacha20poly1305_ABYTES;
const unsigned int Sodium::PW_OPSLIMIT = crypto_pwhash_OPSLIMIT_SENSITIVE;
const unsigned int Sodium::PW_MEMLIMIT = crypto_pwhash_MEMLIMIT_MODERATE;
const unsigned int Sodium::PW_ALG = crypto_pwhash_ALG_ARGON2ID13;
const unsigned int Sodium::PW_SALT_LENGTH = crypto_pwhash_SALTBYTES;

void Sodium::init(const std::wstring& password, const std::vector<unsigned char>& newSalt) {
	if (sodium_init() < 0) {
		throw std::runtime_error("Cant init libsodium");
	}
	if (newSalt.size() != PW_SALT_LENGTH) {
		std::wostringstream msg;
		msg << L"Invalid salt size: " << newSalt.size() << L" expected " << PW_SALT_LENGTH;
		throw Util::generateErrorMessage<std::runtime_error>(msg);
	}
	this->salt = newSalt;
	inProgress = false;

	key.resize(KEY_LENGTH, 0);
	header.resize(HEADER_LENGTH, 0);

	int result = crypto_pwhash(key.data(), key.size(), reinterpret_cast<const char*>(password.c_str()), password.size() * sizeof(wchar_t), salt.data(), PW_OPSLIMIT, PW_MEMLIMIT, PW_ALG);
	if (result != 0) {
		throw std::runtime_error("Cant derive key from password");
	}
}

Sodium::Sodium(const std::wstring& password) {
	if (sodium_init() < 0) {
		throw std::runtime_error("Cant init libsodium");
	}
	forEncryption = true;

	std::vector<unsigned char> newSalt;
	newSalt.resize(PW_SALT_LENGTH, 0);
	randombytes_buf(newSalt.data(), newSalt.size());

	init(password, newSalt);
	crypto_secretstream_xchacha20poly1305_init_push(&state, header.data(), key.data());
}

Sodium::Sodium(const std::wstring& password, const std::vector<unsigned char>& salt, const std::vector<unsigned char>& header) {
	forEncryption = false;
	init(password, salt);
	if (header.size() != HEADER_LENGTH) {
		Util::BuffType msg;
		msg << L"Invalid header size: " << header.size() << L" expected " << HEADER_LENGTH;
		throw Util::generateErrorMessage<std::runtime_error>(msg);
	}
	if (crypto_secretstream_xchacha20poly1305_init_pull(&state, header.data(), key.data()) != 0) {
		Util::BuffType msg;
		msg << L"Cant init pull, invalid header or key?";
		throw Util::generateErrorMessage<std::runtime_error>(msg);
	}
}

Sodium::~Sodium() noexcept(false) {
	if (inProgress) {
		Util::BuffType msg;
		msg << L"File encryption or decryption never flagged finished";
		throw Util::generateErrorMessage<std::runtime_error>(msg);
	}
}

std::vector<unsigned char> Sodium::encrypt(const std::vector<unsigned char>& src, bool isLast) {
	if (!forEncryption) {
		Util::BuffType msg;
		msg << L"Called encrypt but instance is for decryption";
		throw Util::generateErrorMessage<std::logic_error>(msg);
	}
	inProgress = !isLast;
	unsigned long long retLen;
	std::vector<unsigned char> ret(src.size() + ENCRYPTED_CHUNK_ADDITIONAL_SIZE);
	crypto_secretstream_xchacha20poly1305_push(&state, ret.data(), &retLen, src.data(), src.size(), NULL, 0,
		isLast ? crypto_secretstream_xchacha20poly1305_TAG_FINAL : crypto_secretstream_xchacha20poly1305_TAG_MESSAGE);
	ret.resize(retLen);
	return ret;
}

std::vector<unsigned char> Sodium::decrypt(const std::vector<unsigned char>& src, bool isLast) {
	if (forEncryption) {
		Util::BuffType msg;
		msg << L"Called decrypt but instance is for encryption";
		throw Util::generateErrorMessage<std::logic_error>(msg);
	}
	if (src.size() < (ENCRYPTED_CHUNK_ADDITIONAL_SIZE + 1)) {
		Util::BuffType msg;
		msg << L"Cant decrypt chunk which is less than " << (ENCRYPTED_CHUNK_ADDITIONAL_SIZE + 1) << L" bytes long but chunk is "
			<< src.size() << L" bytes long";
		throw Util::generateErrorMessage<std::underflow_error>(msg);
	}
	inProgress = true;
	unsigned char tag;
	unsigned long long retLen;
	std::vector<unsigned char> ret(src.size() - ENCRYPTED_CHUNK_ADDITIONAL_SIZE);

	if (crypto_secretstream_xchacha20poly1305_pull(&state, ret.data(), &retLen, &tag, src.data(), src.size(), NULL, 0) != 0) {
		Util::BuffType msg;
		msg << L"Cant decrypt data, invalid chunk?";
		throw Util::generateErrorMessage<std::runtime_error>(msg);
	}
	if (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL) {
		if (!isLast) {
			Util::BuffType msg;
			msg << L"Data finished but not last chunk";
			throw Util::generateErrorMessage<std::runtime_error>(msg);
		}
		inProgress = false;
	}
	ret.resize(retLen);
	return ret;
}
