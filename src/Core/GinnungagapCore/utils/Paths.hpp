#pragma once

#include <filesystem>
#include <fstream>
#include <vector>

std::filesystem::path getexepath();
std::vector<std::filesystem::path> getFiles(const std::filesystem::path& rootpath);
bool doesDirectoryExist(const std::filesystem::path& path);
bool doesFileExist(const std::filesystem::path& path);
void safe_create_dir(const std::string& path);
void safe_create_dir(const std::filesystem::path& path);
std::ofstream safe_create_textfile(const std::string& path);
std::ofstream safe_create_textfile(const std::filesystem::path& path);