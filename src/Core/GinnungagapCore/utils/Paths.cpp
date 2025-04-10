#include "Paths.hpp"

#include <algorithm>
#include <iostream>
#ifdef _WIN32
#include <windows.h>    //GetModuleFileNameW
#else
#include <limits.h>
#include <unistd.h>     //readlink
#endif

std::filesystem::path getexepath()
{
#ifdef _WIN32
    wchar_t path[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, path, MAX_PATH);
    return path;
#elif __linux__
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    return std::string(result, (count > 0) ? count : 0);
#endif
}

std::vector<std::filesystem::path> getFiles(const std::filesystem::path& rootpath)
{
    std::vector<std::filesystem::path> out;
    for(auto& p: std::filesystem::directory_iterator(rootpath))
        out.push_back(p);
    std::sort(out.begin(), out.end());
    return out;
}

bool doesDirectoryExist(const std::filesystem::path& path)
{
    return std::filesystem::is_directory(path);
}


bool doesFileExist(const std::filesystem::path& path)
{
    return std::filesystem::is_regular_file(path);
}

void safe_create_dir(const std::string& path)
{
    if (!std::filesystem::exists(path))
    {
        if (!std::filesystem::create_directories(path))
        {
            std::string err_msg = "Failed to create path: " + path;
            std::cout << err_msg << std::endl;
            throw std::runtime_error(err_msg);
        }
    }
}

void safe_create_dir(const std::filesystem::path& path)
{
    safe_create_dir(path.string());
}

std::ofstream safe_create_textfile(const std::string& path)
{
    std::ofstream stream(path, std::ios::out);
    if (!stream.is_open())
    {
        std::string err_msg = "Failed to create file: " + path;
        std::cout << err_msg << std::endl;
        throw std::runtime_error(err_msg);
    }
    return stream;
}


std::ofstream safe_create_textfile(const std::filesystem::path& path)
{
    return safe_create_textfile(path.string());
}
