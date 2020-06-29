#include <cstdio>
#include <Windows.h>
#include <algorithm>

// adapted from https://stackoverflow.com/a/940743
void get_version(const char* path, char* out) {
	DWORD ver_handle = 0;
	UINT size = 0;
	LPBYTE lp_buffer = nullptr;
	const auto ver_size = GetFileVersionInfoSizeA(path, &ver_handle);

	if (ver_size != NULL) {
	    auto *const ver_data = new char[ver_size];
	    if (GetFileVersionInfoA(path, ver_handle, ver_size, ver_data)) {
	        if (VerQueryValue(ver_data, L"\\", reinterpret_cast<VOID FAR* FAR*>(&lp_buffer),&size)) {	        	
	            if (size) {
	                auto* ver_info = reinterpret_cast<VS_FIXEDFILEINFO*>(lp_buffer);
	            	char buf[512];
	                if (ver_info->dwSignature == VS_FFI_SIGNATURE) {
	                	sprintf_s(
							buf, 
							512, 
							"%d.%d.%d.%d", 
							(ver_info->dwFileVersionMS >> 16) & 0xffff,
	                		(ver_info->dwFileVersionMS >>  0) & 0xffff,
	                		(ver_info->dwFileVersionLS >> 16) & 0xffff,
	                		(ver_info->dwFileVersionLS >>  0) & 0xffff
						);
						std::memcpy(out, buf, 512);
	                }
	            }
	        }
	    }
	    delete[] ver_data;
	}
}