#include "../dependencies/common_includes.hpp"
#include "../http/http.hpp"
#include "github.hpp"

json github::get_release(const char* owner, const char* repo, const char* version) {
	char url[512];
	sprintf_s(url, 512, "https://api.github.com/repos/%s/%s/releases/%s", owner, repo, version);

	auto res = http::get(url);
	auto j = json::parse(res);
	return j;
}
