#include <iostream>
#include <filesystem>
#include "Core.h"

int run() {
	std::setlocale(LC_ALL, "");

	std::cout << "Generating file list..." << std::endl;
	std::vector<Scope> scopes{};
	Core::traverseDir(".", scopes);

	std::cout << "Calculating checksums..." << std::endl;
	ThreadPool<> threadPool{};
	Core::queueChecksumJobs(threadPool, scopes);
	threadPool.wait();

	std::cout << "Building file hashmap..." << std::endl;
	Core::buildHashmap(scopes);

	std::cout << "Identical files:" << std::endl;
	std::vector<FileEntry *> allFilesToDelete{};
	for (const Scope &scope: scopes) {
		for (const auto &duplicates: scope.map) {
			auto &list = duplicates.second;
			if (list.size() < 2) {
				continue;
			}

			std::unordered_set<FileEntry *> filesToDelete = Core::decideFilesToRemove(list);
			std::cout << scope.dirName << ":" << std::endl;
			for (const auto entry: list) {
				std::cout << '\t' << entry->getPath().string();
				if (filesToDelete.find(entry) != filesToDelete.end()) {
					std::cout << " (delete)";
					allFilesToDelete.emplace_back(entry);
				} else {
					std::cout << " (keep)";
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
	}

	if (allFilesToDelete.empty()) {
		std::cout << "No files to delete" << std::endl;
		return 0;
	}

	std::cout << "Do you really want to delete duplicates? (Y/N)" << std::endl;
	std::string res{};
	std::getline(std::cin, res);
	if (!(res == "y" || res == "Y")) {
		std::cout << "Aborting" << std::endl;
		return 0;
	}

	for (const auto &file: allFilesToDelete) {
		try {
			std::filesystem::remove(file->getPath());
		} catch (std::exception &e) {
			std::cout << "An error occurred while deleting file " << file->getPath().string() << ": " << e.what()
					  << std::endl;
		}
	}

	return 0;
}

int main() {
	try {
		return run();
	} catch (std::exception &e) {
		std::cout << "Application failed with unhandled error: " + std::string{e.what()} << std::endl;
	}
}