#include <complex>
#include <cstdio>
#include <cstdlib>
#include "version.hpp"
#include "dependencies/common_includes.hpp"
#include "github/github.hpp"
#include "http/http.hpp"

json fs_j = {};
json gui_j = {};

void redownload_release(const char* owner, const char* repo, const char* release, const char* out) {
	auto j = fs_j;
	if (_stricmp(repo, "gui") == 0) {
		j = gui_j;
	}

	printf("[*] Found update for %s!\n", repo);
	printf("[+] %s (v%s, published %s)\n\n\n", j["name"].get<std::string>().c_str(), j["tag_name"].get<std::string>().c_str(), j["published_at"].get<std::string>().c_str());
	const auto url = j["assets"][0]["browser_download_url"].get<std::string>();
	http::download_file(url.c_str(), out);
	printf("\n\n[+] Download complete for %s!\n", repo);
}

void redownload_fastsnipe() {
	redownload_release("fastsnipe", "fastsnipe", "latest", "./fastsnipe.exe");
}

int on_extract_entry(const char *filename, void *arg) {
    static auto i = 0;
    const auto total_entries = *static_cast<int*>(arg);
    printf("[*] Extracted %s (%d of %d)\n", filename, ++i, total_entries);
    return 0;
}
void redownload_fastsnipe_gui() {
	redownload_release("fastsnipe", "gui", "latest", "./gui.zip");
	auto* zip = zip_open("gui.zip", 0, 'r');
	auto total_entries = zip_total_entries(zip);
	zip_close(zip);
	zip_extract("gui.zip", "./", on_extract_entry, &total_entries);
	fs::remove("gui.zip"); // remove temporary zip file
	printf("[*] Update complete for gui\n");
}

bool running_in_blacklisted_folder() {
	// prevent running in downloads, desktop, or documents
	char path_[MAX_PATH];
	GetModuleFileNameA(nullptr, path_, MAX_PATH);
	auto path = fs::path(path_);
	path._Remove_filename_and_separator();
	const auto path_str = path.string();
	if (path_str.ends_with("Downloads") || path_str.ends_with("Desktop") || path_str.ends_with("Documents")) {
		return true;
	}
	return false;
}

void check_status(json j) {
	if (http::last_status_code != 200) {
		fprintf(stderr, "[!] Error contacting GitHub API to get release info\n\nError: %s\n\n\nTry waiting 5 minutes before running the updater again.\nExiting.\n\n\n", j["message"].get<std::string>().c_str());
		system("pause");
		exit(1);
	}
}

int main() {
	if (running_in_blacklisted_folder()) {
		fprintf(stderr, "[!] Please move the updater to its own folder. Running the updater will create 50+ files in the current folder.\n");
		system("pause");
		return 1;
	}
	printf("[*] Checking for updates...\n");
	fs_j = github::get_release("fastsnipe", "fastsnipe", "latest");
	check_status(fs_j);
	gui_j = github::get_release("fastsnipe", "gui", "latest");
	check_status(gui_j);
	if (!fs::exists("./fastsnipe.exe")) {
		printf("[!] Couldn\'t find fastsnipe.exe. Will download.\n");
		redownload_fastsnipe();
	}
	if (!fs::exists("./fastsnipe-gui.exe")) {
		printf("[!] Couldn\'t find fastsnipe-gui.exe. Will download.\n");
		redownload_fastsnipe_gui();
	}
	char fastsnipe_version[512];
	char gui_version[512];
	get_version("./fastsnipe.exe", fastsnipe_version);
	get_version("./fastsnipe-gui.exe", gui_version);

	if (_stricmp(fs_j["tag_name"].get<std::string>().c_str(), fastsnipe_version) != 0) {
		// out of date
		printf("[!] fastsnipe.exe out of date. Your version: %s, latest version: %s\n    Updating...\n", fastsnipe_version, fs_j["tag_name"].get<std::string>().c_str());
		redownload_fastsnipe();
	}
	if (_stricmp(gui_j["tag_name"].get<std::string>().c_str(), gui_version) != 0) {
		// out of date
		printf("[!] fastsnipe-gui.exe out of date. Your version: %s, latest version: %s\n    Updating...\n", gui_version, gui_j["tag_name"].get<std::string>().c_str());
		redownload_fastsnipe_gui();
	}

	printf("[*] All files up to date. You may now launch fastsnipe-gui.\n");	
	system("pause"); // wait for keypress before exiting to allow time to read the log
	return 0;
}