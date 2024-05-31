#include <array>
#include <fstream>
#include <iostream>
#include "Core.h"
#include "xxh3.h"
#include <regex>
#include <unordered_set>

void Core::traverseDir(const std::string &path, std::vector<Scope> &scopesOut) {
	Scope scope{};
	scope.dirName = path;

	std::filesystem::directory_iterator iterator;
	try {
		iterator = std::filesystem::directory_iterator(path);
	} catch (std::exception &e) {
		return;
	}

	for (const auto &entry: iterator) {
		try {
			if (entry.is_directory()) {
				traverseDir(entry.path().string(), scopesOut);
				continue;
			}

			if (entry.is_regular_file()) {
				scope.entries.emplace_back(entry.path());
			}
		} catch (std::exception &e) {
			continue;
		}
	}

	if (!scope.entries.empty()) {
		scopesOut.emplace_back(scope);
	}
}

void Core::queueChecksumJobs(ThreadPool<> &threadPool, std::vector<Scope> &scopes) {
	for (Scope &scope: scopes) {
		for (auto &entry: scope.entries) {
			threadPool.queueTask([&entry]() {
				constexpr int BUFF_SIZE = 16'384;
				std::array<char, BUFF_SIZE> buffer{};
				XXH3_state_t *hashState = XXH3_createState();
				XXH3_64bits_reset(hashState);

				std::ifstream fStream(entry.getPath(), std::ios::binary);
				if (!fStream) {
					return;
				}

				std::streamsize numRead;
				while (!fStream.eof()) {
					fStream.read(buffer.data(), BUFF_SIZE);
					numRead = fStream.gcount();
					XXH3_64bits_update(hashState, buffer.data(), numRead);
				}

				entry.setHash(XXH3_64bits_digest(hashState));
			});
		}
	}
}

void Core::buildHashmap(std::vector<Scope> &scopes) {
	for (Scope &scope: scopes) {
		for (auto &file: scope.entries) {
			if (!file.getHash()) {
				continue;
			}

			if (scope.map.contains(*file.getHash())) {
				auto list = &scope.map[*file.getHash()];
				list->emplace_back(&file);
			} else {
				scope.map[*file.getHash()] = std::vector{&file};
			}
		}
	}
}

std::unordered_set<FileEntry *> Core::decideFilesToRemove(const std::vector<FileEntry *> &files) {
	std::unordered_set<FileEntry *> filesToRemove{};
	filesToRemove.reserve(files.size() - 1);
	const std::regex incrementingNumberRegex(".* \\([0-9]+\\)(.*)");
	bool containsIncrementingNumbers = false;
	FileEntry *fileToKeep = nullptr;
	for (const auto file: files) {
		if (std::regex_search(file->getPath().filename().string(), incrementingNumberRegex)) {
			containsIncrementingNumbers = true;
		} else {
			fileToKeep = file;
		}
	}

	if (!(containsIncrementingNumbers && fileToKeep != nullptr)) {
		// just pick one
		fileToKeep = files[0];
	}

	for (const auto file: files) {
		if (file != fileToKeep) {
			filesToRemove.insert(file);
		}
	}
	return filesToRemove;
}