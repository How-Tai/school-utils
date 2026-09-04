#pragma once

#include <string>

std::string randomHex(size_t bytes);
std::string sha256Hex(const std::string& value);
std::string passwordHash(const std::string& password, const std::string& salt);
bool constantTimeEqual(const std::string& a, const std::string& b);
std::string getCookie(const std::string& cookieHeader, const std::string& name);
