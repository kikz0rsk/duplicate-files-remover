#pragma once

#include <vector>
#include <map>
#include "FileEntry.h"
#include "xxh3.h"

struct Scope {
	std::string dirName{};
	std::vector<FileEntry> entries{};
	std::map<XXH64_hash_t, std::vector<FileEntry *>> map{};
};