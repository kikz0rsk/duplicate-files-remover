#pragma once

#include <string>
#include <xxh3.h>
#include <optional>
#include <utility>
#include <filesystem>

class FileEntry {
protected:
	std::filesystem::path path;
	std::optional<XXH64_hash_t> hash;

public:
	explicit FileEntry(std::string_view path);

	explicit FileEntry(std::filesystem::path path);

	[[nodiscard]] std::optional<XXH64_hash_t> getHash() const;

	[[nodiscard]] std::filesystem::path getPath() const;

	void setHash(XXH64_hash_t hash);
};