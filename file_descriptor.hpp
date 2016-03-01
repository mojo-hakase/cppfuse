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

#endif // _FILE_DESCRIPTOR_HPP_
