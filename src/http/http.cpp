#include "http.hpp"

auto constexpr USER_AGENT = "fastsnipe-updater/1.0";
namespace http {
	int last_status_code = 0;
}
/**
 * https://stackoverflow.com/a/36401787
 * Adapted to fit codestyle
 */
size_t curlwrite_callbackfunc_stdstring(void* contents, const size_t size, const size_t nmemb, std::string* s) {
    const auto new_length = size * nmemb;
    try {
        s->append(static_cast<char*>(contents), new_length);
    } catch(std::bad_alloc &e) {
	    // handle memory problem
	    return 0;
    }
    return new_length;
}

static size_t curlwrite_callbackfunc_file(void *ptr, size_t size, size_t nmemb, void *stream)
{
  const auto written = fwrite(ptr, size, nmemb, static_cast<FILE*>(stream));
  return written;
}

std::string http::get(const char* url) {
	CURL* req = curl_easy_init();
	std::string s;

	curl_easy_setopt(req, CURLOPT_URL, url);
	curl_easy_setopt(req, CURLOPT_USERAGENT, USER_AGENT);
	curl_easy_setopt(req, CURLOPT_WRITEFUNCTION, curlwrite_callbackfunc_stdstring);
	curl_easy_setopt(req, CURLOPT_WRITEDATA, &s);
	/*auto res = */ curl_easy_perform(req);
	curl_easy_getinfo(req, CURLINFO_RESPONSE_CODE, &last_status_code);
	curl_easy_cleanup(req);

#ifdef _DEBUG
	//printf("(http) GET %s - length %llu - str %s\n", url, s.length(), s.c_str());
#endif
	
	return s;
}

void http::download_file(const char* url, const char* path) {
	CURL* req = curl_easy_init();
	FILE* file;
	fopen_s(&file, path, "wb");
	curl_easy_setopt(req, CURLOPT_URL, url);
	curl_easy_setopt(req, CURLOPT_USERAGENT, USER_AGENT);
	curl_easy_setopt(req, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(req, CURLOPT_WRITEFUNCTION, curlwrite_callbackfunc_file);
	curl_easy_setopt(req, CURLOPT_FOLLOWLOCATION, 1L);

	if (file) {
		curl_easy_setopt(req, CURLOPT_WRITEDATA, file);
		curl_easy_perform(req);
		fclose(file);
	}
	curl_easy_cleanup(req);
}


std::string http::post(const char* url, const char* data) {
	CURL* req = curl_easy_init();
	std::string s;

	curl_easy_setopt(req, CURLOPT_URL, url);
	curl_easy_setopt(req, CURLOPT_USERAGENT, USER_AGENT);
	curl_easy_setopt(req, CURLOPT_WRITEFUNCTION, curlwrite_callbackfunc_stdstring);
	curl_easy_setopt(req, CURLOPT_WRITEDATA, &s);
	curl_easy_setopt(req, CURLOPT_POSTFIELDS, data);
	struct curl_slist *chunk = nullptr;
	chunk = curl_slist_append(chunk, "Content-Type: application/json");
	chunk = curl_slist_append(chunk, "Charset: utf-8");
	curl_easy_setopt(req, CURLOPT_HTTPHEADER, chunk);
	
	/*auto res = */ curl_easy_perform(req);
	curl_easy_cleanup(req);

#ifdef _DEBUG
	//printf("(http) POST %s - length %llu - str %s\n", url, s.length(), s.c_str());
#endif
	
	return s;
}
