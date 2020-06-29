#pragma once
#include "../dependencies/common_includes.hpp"
// http methods
// using libcurl

namespace http {
	extern int last_status_code;
	std::string get(const char* url);
	void download_file(const char* url, const char* path);
	std::string post(const char* url, const char* data);
}