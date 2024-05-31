#pragma once

#include <vector>
#include "Scope.h"
#include "ThreadPool.h"
#include <functional>
#include <unordered_set>

class Core {

public:
	static void traverseDir(const std::string &path, std::vector<Scope> &scopesOut);

	static void queueChecksumJobs(ThreadPool<> &threadPool, std::vector<Scope> &scopes);

	static void buildHashmap(std::vector<Scope> &scopes);

	static std::unordered_set<FileEntry *> decideFilesToRemove(const std::vector<FileEntry *> &files);
};
