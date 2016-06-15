#ifndef _FILE_DESCRIPTOR_HPP_
#define _FILE_DESCRIPTOR_HPP_

#include "cppfuse.hpp"
#ifdef _FDM_USE_UNORDERED_MAP_
#include <unordered_map>
#else
#include <map>
#endif

typedef uint64_t fuse_fh_t;
struct FileDescriptor {
	fuse_fh_t fh; // fuse_file_info *fi;
	FuseFDCallback *node;
	FileDescriptor() : fh(0), node(nullptr) {}
	FileDescriptor(fuse_fh_t fh, FuseFDCallback *node) : fh(fh), node(node) {}
	FileDescriptor(fuse_file_info *fi, FuseFDCallback *node) : fh(fi->fh), node(node) {}
};

//class FileDescriptorContainer {
class FDManager {
	fuse_fh_t last;
#ifdef _FDM_USE_UNORDERED_MAP_
	std::unordered_map<fuse_fh_t, FileDescriptor> fdmap;
#else
	std::map<fuse_fh_t, FileDescriptor> fdmap;
#endif
public:
	FDManager();

	FileDescriptor* find(fuse_fh_t fh);
	FileDescriptor* find(fuse_file_info *fi);

	fuse_fh_t registerFD(const FileDescriptor&);
	fuse_fh_t registerFD(fuse_fh_t fh, FuseFDCallback *node);
	fuse_fh_t registerFD(fuse_file_info *fi, FuseFDCallback *node);

	void releaseFD(fuse_fh_t fh);
	void releaseFD(fuse_file_info *fi);
};


FDManager::FDManager() : last(0) {
#ifdef _FDM_USE_UNORDERED_MAP_
	fdmap.reserve(8);
#endif
}

FileDescriptor* FDManager::find(fuse_fh_t fh) {
	auto it = this->fdmap.find(fh);
	if (it == fdmap.end())
		return nullptr;
	return &it->second;
}

FileDescriptor* FDManager::find(fuse_file_info *fi) {
	if (!fi)
		return nullptr;
	return find(fi->fh);
}

fuse_fh_t FDManager::registerFD(const FileDescriptor &fd) {
	last++;
	fdmap[last] = fd;
	return last;
}

fuse_fh_t FDManager::registerFD(fuse_fh_t fh, FuseFDCallback *node) {
	return registerFD(FileDescriptor(fh, node));
}

fuse_fh_t FDManager::registerFD(fuse_file_info *fi, FuseFDCallback *node) {
	if (!fi)
		return 0;
	fuse_fh_t fh = registerFD(FileDescriptor(fi, node));
	fi->fh = fh;
	return fh;
}

void FDManager::releaseFD(fuse_fh_t fh) {
	fdmap.erase(fh);
}

void FDManager::releaseFD(fuse_file_info *fi) {
	if (!fi)
		return;
	releaseFD(fi->fh);
}


#endif // _FILE_DESCRIPTOR_HPP_
