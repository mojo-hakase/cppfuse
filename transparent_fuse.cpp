#include "transparent_fuse.hpp"

#include <cstdlib>
#include <unistd.h>	// readlink, close, mknod
#include <fcntl.h>	// open, mknod
#include <sys/types.h>	// open, mkfifo, mknod, *dir
#include <sys/stat.h>	// open, mkfifo, mknod
#include <dirent.h>	// *dir
#include <stdio.h>	// rename
#include <errno.h>

std::string realpath(const std::string &path) {
	char *p = ::realpath(path.c_str(), NULL);
	std::string rp(p);
	std::free(p);
	return rp;
}

void TransparentFuse::setBaseDir(const std::string &baseDir) {
	this->baseDir = realpath(baseDir);
}

int TransparentFuse::getattr(const char *path, struct stat *statbuf) {
	int res = ::lstat(fullpath(path).c_str(), statbuf);
	if (res != 0)
		res = -errno;
	return res;
}
int TransparentFuse::readlink(const char *path, char *link, size_t size) {
	int res;
	res = ::readlink(fullpath(path).c_str(), link, size - 1);
	if (res >= 0) {
		link[res] = '\0';
		res = 0;
	}
	return -res;
}
int TransparentFuse::mknod (const char *path, mode_t mode, dev_t dev) {
	int retstat;
	const char *fpath = fullpath(path).c_str();

	// source: bbfs.c by Joseph J. Pfeiffer
	// Copyright (C) 2012 Joseph J. Pfeiffer, Jr., Ph.D. <pfeiffer@cs.nmsu.edu>
	// On Linux this could just be 'mknod(path, mode, rdev)' but this
	//  is more portable
	if (S_ISREG(mode)) {
		retstat = ::open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (retstat < 0)
			retstat = -errno;
		else {
			retstat = ::close(retstat);
		if (retstat < 0)
			retstat = -errno;
		}
	} else
		if (S_ISFIFO(mode)) {
			retstat = ::mkfifo(fpath, mode);
			if (retstat < 0)
				retstat = -errno;
		} else {
			retstat = ::mknod(fpath, mode, dev);
		if (retstat < 0)
			retstat = -errno;
	}
	
	return retstat;
	//
}
int TransparentFuse::mkdir(const char *path, mode_t mode) {
	int res = ::mkdir(fullpath(path).c_str(), mode);
	if (res < 0)
		res = -errno;
	return res;
}
int TransparentFuse::unlink(const char *path) {
	int res = ::unlink(fullpath(path).c_str());
	if (res < 0)
		res = -errno;
	return res;
}
int TransparentFuse::rmdir(const char *path) {
	int res = ::rmdir(fullpath(path).c_str());
	if (res < 0)
		res = -errno;
	return res;
}
int TransparentFuse::symlink(const char *path, const char *link) {
	int res = ::symlink(path, fullpath(link).c_str());
	if (res < 0)
		res = -errno;
	return res;
}
int TransparentFuse::rename(const char *path, const char *newpath) {
	int res = ::rename(fullpath(path).c_str(), fullpath(newpath).c_str());
	if (res < 0)
		res = -errno;
	return res;
}
int TransparentFuse::link(const char *path, const char *link) {
	int res = ::link(fullpath(path).c_str(), fullpath(link).c_str());
	if (res < 0)
		res = -errno;
	return res;
}
int TransparentFuse::chmod(const char *path, mode_t mode) {
	int res = ::chmod(fullpath(path).c_str(), mode);
	if (res < 0)
		res = -errno;
	return res;
}
int TransparentFuse::chown(const char *path, uid_t uid, gid_t gid) {
	int res = ::chown(fullpath(path).c_str(), uid, gid);
	if (res < 0)
		res = -errno;
	return res;
}
int TransparentFuse::truncate(const char *path, off_t offset) {
	return ::truncate(fullpath(path).c_str(), offset);
}
int TransparentFuse::utimens(const char *path, const struct timespec tv[2]) {
	int res = ::utimensat(AT_FDCWD, fullpath(path).c_str(), tv, 0);
	if (res < 0)
		res = -errno;
	return res;
}
int TransparentFuse::open(const char *path, struct fuse_file_info *fi) {
	int res = 0;
	int fh = ::open(fullpath(path).c_str(), fi->flags);
	if (fh < 0)
		res = -errno;
	fi->fh = fh;
	return res;
}
int TransparentFuse::read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	int res = ::pread(fi->fh, buf, size, offset);
	if (res < 0)
		res = -errno;
	return res;
}
int TransparentFuse::write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	int res = ::pwrite(fi->fh, buf, size, offset);
	if (res < 0)
		res = -errno;
	return res;
}
int TransparentFuse::statfs(const char *path, struct statvfs *statv) {
	int res = ::statvfs(fullpath(path).c_str(), statv);
	if (res < 0)
		res = -errno;
	return res;
}
int TransparentFuse::flush(const char *path, struct fuse_file_info *fi) {
	return 0;
}
int TransparentFuse::release(const char *path, struct fuse_file_info *fi) {
	return ::close(fi->fh);
}
int TransparentFuse::fsync(const char *path, int datasync, struct fuse_file_info *fi) {
	int res;
#ifdef HAVE_FDATASYNC
	if (datasync)
		res = ::fdatasync(fi->fh);
	else
#endif
		res = ::fsync(fi->fh);
	return res;
}
#ifdef HAVE_SYS_XATTR_H
int TransparentFuse::setxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
	int res = ::lsetxattr(fullpath(path).c_str(), name, value, size, flags);
	if (res < 0)
		res = -errno;
	return res;
}
int TransparentFuse::getxattr(const char *path, const char *name, char *value, size_t size) {
	int res = ::lgetxattr(fullpath(path).c_str(), name, value, size);
	if (res < 0)
		res = -errno;
	return res;
}
int TransparentFuse::listxattr(const char *path, char *name, size_t size) {
	int res = ::llistxattr(fullpath(path).c_str(), name, size);
	if (res < 0)
		res = -errno;
	return res;
}
int TransparentFuse::removexattr(const char *path, const char *name) {
	int res = ::lsetxattr(fullpath(path).c_str(), name);
	if (res < 0)
		res = -errno;
	return res;
}
#endif
int TransparentFuse::opendir(const char *path, struct fuse_file_info *fi) {
	int res = 0;
	DIR *dp = ::opendir(fullpath(path).c_str());
	if (dp == NULL)
		res = -errno;
	fi->fh = (uint64_t) dp;
	return res;
}
int TransparentFuse::readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
	DIR *dp = (DIR*) fi->fh;
	struct dirent *de = ::readdir(dp);
	if (de == 0)
		return -errno;
	do {
		if (filler(buf, de->d_name, NULL, 0) != 0)
			return -ENOMEM;
	} while ((de = ::readdir(dp)) != NULL);
	return 0;
}
int TransparentFuse::releasedir(const char *path, struct fuse_file_info *fi) {
	::closedir((DIR*)fi->fh);
	return 0;
}
int TransparentFuse::fsyncdir(const char *path, int datasync, struct fuse_file_info *fi) {
	return 0;
}
int TransparentFuse::access(const char *path, int mask) {
	int res = ::access(fullpath(path).c_str(), mask);
	if (res < 0)
		res = -errno;
	return 0;
}
int TransparentFuse::create(const char *path, mode_t mode, struct fuse_file_info *fi) {
	int res = 0;
	int fh = creat(fullpath(path).c_str(), mode);
	if (fh < 0)
		res = -errno;
	fi->fh = fh;
	return res;
}
int TransparentFuse::ftruncate(const char *path, off_t offset, struct fuse_file_info *fi) {
	int res = ::ftruncate(fi->fh, offset);
	if (res < 0)
		res = -errno;
	return res;
}
int TransparentFuse::fgetattr(const char *path, struct stat *statbuf, struct fuse_file_info *fi) {
	if (path && path[0] == '/' && path[1] == '\0')
		return this->getattr(path, statbuf);
	
	int res = fstat(fi->fh, statbuf);
	if (res < 0)
		res = -errno;
	return res;
}
