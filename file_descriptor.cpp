#include "file_descriptor.hpp"

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

