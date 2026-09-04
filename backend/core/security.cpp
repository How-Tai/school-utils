#include "security.h"

#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <iomanip>
#include <sstream>
#include <vector>

std::string randomHex(size_t bytes) {
	std::vector<unsigned char> data(bytes);
	if(RAND_bytes(data.data(), static_cast<int>(data.size())) != 1) return "";

	std::ostringstream out;
	out << std::hex << std::setfill('0');
	for(unsigned char byte : data) out << std::setw(2) << static_cast<int>(byte);
	return out.str();
}

std::string sha256Hex(const std::string& value) {
	unsigned char digest[EVP_MAX_MD_SIZE];
	unsigned int length = 0;
	EVP_MD_CTX* context = EVP_MD_CTX_new();
	if(!context) return "";

	bool ok = EVP_DigestInit_ex(context, EVP_sha256(), nullptr) == 1 && EVP_DigestUpdate(context, value.data(), value.size()) == 1 && EVP_DigestFinal_ex(context, digest, &length) == 1;
	EVP_MD_CTX_free(context);
	if(!ok) return "";

	std::ostringstream out;
	out << std::hex << std::setfill('0');
	for(unsigned int i = 0; i < length; ++i) out << std::setw(2) << static_cast<int>(digest[i]);
	return out.str();
}

std::string passwordHash(const std::string& password, const std::string& salt) {
	std::vector<unsigned char> output(32);
	if(PKCS5_PBKDF2_HMAC(password.c_str(), static_cast<int>(password.size()), reinterpret_cast<const unsigned char*>(salt.data()), static_cast<int>(salt.size()), 210000, EVP_sha256(), static_cast<int>(output.size()), output.data()) != 1) return "";

	std::ostringstream out;
	out << std::hex << std::setfill('0');
	for(unsigned char byte : output) out << std::setw(2) << static_cast<int>(byte);
	return out.str();
}

bool constantTimeEqual(const std::string& a, const std::string& b) {
	if(a.size() != b.size()) return false;
	return CRYPTO_memcmp(a.data(), b.data(), a.size()) == 0;
}

std::string getCookie(const std::string& cookieHeader, const std::string& name) {
	std::string target = name + "=";
	size_t start = 0;
	while(start < cookieHeader.size()) {
		while(start < cookieHeader.size() && (cookieHeader[start] == ' ' || cookieHeader[start] == ';')) ++start;
		if(cookieHeader.compare(start, target.size(), target) == 0) {
			size_t valueStart = start + target.size();
			size_t end = cookieHeader.find(';', valueStart);
			return cookieHeader.substr(valueStart, end == std::string::npos ? std::string::npos : end - valueStart);
		}
		size_t end = cookieHeader.find(';', start);
		if(end == std::string::npos) break;
		start = end + 1;
	}
	return "";
}
