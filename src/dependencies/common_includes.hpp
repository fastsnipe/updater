#pragma once
#include <Windows.h>
#include <filesystem>
#include "curl/curl.h"
#include "json/json.hpp"
#include "zip/src/zip.h"

namespace fs = std::filesystem;
using json = nlohmann::json;