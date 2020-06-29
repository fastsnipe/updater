#pragma once
#include "../dependencies/json/json.hpp"
namespace github {
	nlohmann::json get_release(const char* owner, const char* repo, const char* version);
}