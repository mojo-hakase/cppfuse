#ifndef _TRANSPARENT_FUSE_HPP_
#define _TRANSPARENT_FUSE_HPP_

#include "cppfuse.hpp"

class TransparentFuse : public Fuse {
	std::string baseDir;
public:
	TransparentFuse(const std::string &baseDir, fs_use_flag_t flags = FS_USE_ALL) : Fuse(flags), baseDir(baseDir) {}
	int mount(int argc, char **argv) {
    		if ((argc < 3) || (argv[argc-2][0] == '-') || (argv[argc-1][0] == '-'))
			return -1;
		setBaseDir(argv[argc-2]);
		argv[argc-2] = argv[argc-1];
		argv[argc-1] = NULL;
		argc--;
		return Fuse::mount(argc, argv);
	}

	void setBaseDir(const std::string &baseDir);
	std::string getBaseDir() {
		return baseDir;
	}
	std::string fullpath(const char *path) {
		return (baseDir + path);
	}

//	virtual void *	init    	(struct fuse_conn_info *conn);
//	virtual void 	destroy  	(void *userdata);
	virtual int	getattr   	(const char *path, struct stat *statbuf);
	virtual int 	readlink  	(const char *path, char *link, size_t size);
	virtual int 	mknod     	(const char *path, mode_t mode, dev_t dev);
	virtual int 	mkdir    	(const char *path, mode_t mode);
	virtual int 	unlink    	(const char *path);
	virtual int 	rmdir    	(const char *path);
	virtual int 	symlink    	(const char *path, const char *link);
	virtual int 	rename    	(const char *path, const char *newpath);
	virtual int 	link    	(const char *path, const char *link);
	virtual int 	chmod    	(const char *path, mode_t mode);
	virtual int 	chown    	(const char *path, uid_t uid, gid_t gid);
	virtual int 	truncate    	(const char *path, off_t offset);
	virtual int 	utimens    	(const char *path, const struct timespec tv[2]);
	virtual int 	open    	(const char *path, struct fuse_file_info *fi);
	virtual int 	read    	(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
	virtual int 	write    	(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi);
	virtual int 	statfs  	(const char *path, struct statvfs *statv);
	virtual int 	flush    	(const char *path, struct fuse_file_info *fi);
	virtual int 	release  	(const char *path, struct fuse_file_info *fi);
	virtual int 	fsync    	(const char *path, int datasync, struct fuse_file_info *fi);
#ifdef HAVE_SYS_XATTR_H
	virtual int 	setxattr    	(const char *path, const char *name, const char *value, size_t size, int flags);
	virtual int 	getxattr  	(const char *path, const char *name, char *value, size_t size);
	virtual int 	listxattr  	(const char *path, char *name, size_t size);
	virtual int 	removexattr    	(const char *path, const char *name);
#endif
	virtual int 	opendir  	(const char *path, struct fuse_file_info *fi);
	virtual int 	readdir  	(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
	virtual int 	releasedir  	(const char *path, struct fuse_file_info *fi);
	virtual int 	fsyncdir    	(const char *path, int datasync, struct fuse_file_info *fi);
	virtual int 	access  	(const char *path, int mask);
	virtual int 	create    	(const char *path, mode_t mode, struct fuse_file_info *fi);
	virtual int 	ftruncate    	(const char *path, off_t offset, struct fuse_file_info *fi);
	virtual int 	fgetattr  	(const char *path, struct stat *statbuf, struct fuse_file_info *fo);
};

#endif // _TRANSPARENT_FUSE_HPP_
