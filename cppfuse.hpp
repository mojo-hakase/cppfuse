#ifndef _CPPFUSE_HPP_
#define _CPPFUSE_HPP_

#define FUSE_USE_VERSION 26
#include <fuse.h>
#include <string>

class FuseFDCallback {
public:
	virtual int 	read    	(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {return 0;}
	virtual int 	write    	(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {return 0;}
	virtual int 	flush    	(const char *path, struct fuse_file_info *fi) {return -ENOENT;}
	virtual int 	release  	(const char *path, struct fuse_file_info *fi) {return -ENOENT;}
	virtual int 	fsync    	(const char *path, int datasync, struct fuse_file_info *fi) {return -ENOENT;}
	virtual int 	readdir  	(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {return -ENOENT;}
	virtual int 	releasedir  	(const char *path, struct fuse_file_info *fi) {return -ENOENT;}
	virtual int 	fsyncdir    	(const char *path, int datasync, struct fuse_file_info *fi) {return -ENOENT;}
	virtual int 	ftruncate    	(const char *path, off_t offset, struct fuse_file_info *fi) {return -ENOENT;}
	virtual int 	fgetattr  	(const char *path, struct stat *statbuf, struct fuse_file_info *fo) {return -ENOENT;}
};

class FusePathCallback {
public:
	virtual int	getattr   	(const char *path, struct stat *statbuf) {return -ENOENT;}
	virtual int 	readlink  	(const char *path, char *link, size_t size) {return -ENOENT;}
	virtual int 	mknod     	(const char *path, mode_t mode, dev_t dev) {return -ENOENT;}
	virtual int 	mkdir    	(const char *path, mode_t mode) {return -ENOENT;}
	virtual int 	unlink    	(const char *path) {return -ENOENT;}
	virtual int 	rmdir    	(const char *path) {return -ENOENT;}
	virtual int 	symlink    	(const char *path, const char *link) {return -ENOENT;}
	virtual int 	rename    	(const char *path, const char *newpath) {return -ENOENT;}
	virtual int 	link    	(const char *path, const char *link) {return -ENOENT;}
	virtual int 	chmod    	(const char *path, mode_t mode) {return -ENOENT;}
	virtual int 	chown    	(const char *path, uid_t uid, gid_t gid) {return -ENOENT;}
	virtual int 	truncate    	(const char *path, off_t offset) {return -ENOENT;}
	virtual int 	utimens    	(const char *path, const struct timespec tv[2]) {return -ENOENT;}
	virtual int 	open    	(const char *path, struct fuse_file_info *fi) {return -ENOENT;}
	virtual int 	statfs  	(const char *path, struct statvfs *statv) {return -ENOENT;}
#ifdef HAVE_SYS_XATTR_H
	virtual int 	setxattr    	(const char *path, const char *name, const char *value, size_t size, int flags) {return -ENOENT;}
	virtual int 	getxattr  	(const char *path, const char *name, char *value, size_t size) {return -ENOENT;}
	virtual int 	listxattr  	(const char *path, char *name, size_t size) {return -ENOENT;}
	virtual int 	removexattr    	(const char *path, const char *name) {return -ENOENT;}
#endif
	virtual int 	opendir  	(const char *path, struct fuse_file_info *fi) {return -ENOENT;}
	virtual int 	access  	(const char *path, int mask) {return -ENOENT;}
	virtual int 	create    	(const char *path, mode_t mode, struct fuse_file_info *fi) {return -ENOENT;}
};

class FuseCallback : public FuseFDCallback, public FusePathCallback {};

class Fuse : public FuseCallback {
public:

	// callback functions for fuse
	// they call the virtual functions
	static void *	s_init  	(struct fuse_conn_info *);
	static void 	s_destroy  	(void *);
	static int	s_getattr   	(const char *, struct stat *);
	static int 	s_readlink  	(const char *, char *, size_t);
	static int 	s_mknod     	(const char *, mode_t, dev_t);
	static int 	s_mkdir    	(const char *, mode_t);
	static int 	s_unlink    	(const char *);
	static int 	s_rmdir    	(const char *);
	static int 	s_symlink    	(const char *, const char *);
	static int 	s_rename    	(const char *, const char *);
	static int 	s_link    	(const char *, const char *);
	static int 	s_chmod    	(const char *, mode_t);
	static int 	s_chown    	(const char *, uid_t, gid_t);
	static int 	s_truncate    	(const char *, off_t);
	static int 	s_utimens    	(const char *, const struct timespec tv[2]);
	static int 	s_open  	(const char *, struct fuse_file_info *);
	static int 	s_read  	(const char *, char *, size_t, off_t, struct fuse_file_info *);
	static int 	s_write    	(const char *, const char *, size_t, off_t, struct fuse_file_info *);
	static int 	s_statfs  	(const char *, struct statvfs *);
	static int 	s_flush    	(const char *, struct fuse_file_info *);
	static int 	s_release  	(const char *, struct fuse_file_info *);
	static int 	s_fsync    	(const char *, int, struct fuse_file_info *);
#ifdef HAVE_SYS_XATTR_H
	static int 	s_setxattr    	(const char *, const char *, const char *, size_t, int);
	static int 	s_getxattr  	(const char *, const char *, char *, size_t);
	static int 	s_listxattr  	(const char *, char *, size_t);
	static int 	s_removexattr  	(const char *, const char *);
#endif
	static int 	s_opendir  	(const char *, struct fuse_file_info *);
	static int 	s_readdir  	(const char *, void *, fuse_fill_dir_t, off_t, struct fuse_file_info *);
	static int 	s_releasedir  	(const char *, struct fuse_file_info *);
	static int 	s_fsyncdir    	(const char *, int, struct fuse_file_info *);
	static int 	s_access  	(const char *, int);
	static int 	s_create    	(const char *, mode_t, struct fuse_file_info *);
	static int 	s_ftruncate    	(const char *, off_t, struct fuse_file_info *);
	static int 	s_fgetattr  	(const char *, struct stat *, struct fuse_file_info *);

	typedef unsigned int fs_use_flag_t;
	static const fs_use_flag_t FS_USE_GETATTR    	= 1 <<  0;
	static const fs_use_flag_t FS_USE_READLINK   	= 1 <<  1;
	static const fs_use_flag_t FS_USE_MKNOD      	= 1 <<  2;
	static const fs_use_flag_t FS_USE_MKDIR      	= 1 <<  3;
	static const fs_use_flag_t FS_USE_UNLINK     	= 1 <<  4;
	static const fs_use_flag_t FS_USE_RMDIR      	= 1 <<  5;
	static const fs_use_flag_t FS_USE_SYMLINK    	= 1 <<  6;
	static const fs_use_flag_t FS_USE_RENAME     	= 1 <<  7;
	static const fs_use_flag_t FS_USE_LINK       	= 1 <<  8;
	static const fs_use_flag_t FS_USE_CHMOD      	= 1 <<  9;
	static const fs_use_flag_t FS_USE_CHOWN      	= 1 << 10;
	static const fs_use_flag_t FS_USE_TRUNCATE   	= 1 << 11;
	static const fs_use_flag_t FS_USE_UTIMENS      	= 1 << 12;
	static const fs_use_flag_t FS_USE_OPEN       	= 1 << 13;
	static const fs_use_flag_t FS_USE_READ       	= 1 << 14;
	static const fs_use_flag_t FS_USE_WRITE      	= 1 << 15;
	static const fs_use_flag_t FS_USE_STATFS     	= 1 << 16;
	static const fs_use_flag_t FS_USE_FLUSH      	= 1 << 17;
	static const fs_use_flag_t FS_USE_RELEASE    	= 1 << 18;
	static const fs_use_flag_t FS_USE_FSYNC      	= 1 << 19;
#ifdef HAVE_SYS_XATTR_H
	static const fs_use_flag_t FS_USE_SETXATTR   	= 1 << 20;
	static const fs_use_flag_t FS_USE_GETXATTR   	= 1 << 21;
	static const fs_use_flag_t FS_USE_LISTXATTR  	= 1 << 22;
	static const fs_use_flag_t FS_USE_REMOVEXATTR	= 1 << 23;
#endif
	static const fs_use_flag_t FS_USE_OPENDIR    	= 1 << 24;
	static const fs_use_flag_t FS_USE_READDIR    	= 1 << 25;
	static const fs_use_flag_t FS_USE_RELEASEDIR 	= 1 << 26;
	static const fs_use_flag_t FS_USE_FSYNCDIR   	= 1 << 27;
	static const fs_use_flag_t FS_USE_ACCESS     	= 1 << 28;
	static const fs_use_flag_t FS_USE_CREATE     	= 1 << 29;
	static const fs_use_flag_t FS_USE_FTRUNCATE  	= 1 << 30;
	static const fs_use_flag_t FS_USE_FGETATTR   	= 1 << 31;

	static const fs_use_flag_t FS_USE_ALL = 0xFFFFFFFF;
	static const fs_use_flag_t FS_USE_READONLY = 0x97656003;
	static const fs_use_flag_t FS_USE_NONE = 0x0;

	static struct fuse_operations flags2struct(fs_use_flag_t f);

	virtual int mount(int argc, char **argv);

	virtual ~Fuse() {}

	Fuse(fs_use_flag_t = FS_USE_ALL);

	virtual void 	init    	(struct fuse_conn_info *conn) {}
	virtual void 	destroy  	(void *userdata) {}

private:
	fs_use_flag_t opflags;
};

#endif // _CPPFUSE_HPP_
