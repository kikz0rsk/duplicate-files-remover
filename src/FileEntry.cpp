#include "FileEntry.h"

FileEntry::FileEntry(std::string_view path) : path(path) {}

FileEntry::FileEntry(std::filesystem::path path) : path(std::move(path)) {}

std::optional<XXH64_hash_t> FileEntry::getHash() const {
	return hash;
}

std::filesystem::path FileEntry::getPath() const {
	return path;
}

void FileEntry::setHash(XXH64_hash_t _hash) {
	this->hash = _hash;
}
