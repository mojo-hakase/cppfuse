#include "cppfuse.hpp"

struct fuse_operations Fuse::flags2struct(fs_use_flag_t f) {
	struct fuse_operations res;
	res.init = s_init;
	res.destroy = s_destroy;
	res.getattr = (f & FS_USE_GETATTR) ? s_getattr : NULL;
	res.readlink = (f & FS_USE_READLINK) ? s_readlink : NULL;
	res.mknod = (f & FS_USE_MKNOD) ? s_mknod : NULL;
	res.mkdir = (f & FS_USE_MKDIR) ? s_mkdir : NULL;
	res.unlink = (f & FS_USE_UNLINK) ? s_unlink : NULL;
	res.rmdir = (f & FS_USE_RMDIR) ? s_rmdir : NULL;
	res.symlink = (f & FS_USE_SYMLINK) ? s_symlink : NULL;
	res.rename = (f & FS_USE_RENAME) ? s_rename : NULL;
	res.link = (f & FS_USE_LINK) ? s_link : NULL;
	res.chmod = (f & FS_USE_CHMOD) ? s_chmod : NULL;
	res.chown = (f & FS_USE_CHOWN) ? s_chown : NULL;
	res.truncate = (f & FS_USE_TRUNCATE) ? s_truncate : NULL;
	res.utimens = (f & FS_USE_UTIMENS) ? s_utimens : NULL;
	res.open = (f & FS_USE_OPEN) ? s_open : NULL;
	res.read = (f & FS_USE_READ) ? s_read : NULL;
	res.write = (f & FS_USE_WRITE) ? s_write : NULL;
	res.statfs = (f & FS_USE_STATFS) ? s_statfs : NULL;
	res.flush = (f & FS_USE_FLUSH) ? s_flush : NULL;
	res.release = (f & FS_USE_RELEASE) ? s_release : NULL;
	res.fsync = (f & FS_USE_FSYNC) ? s_fsync : NULL;
#ifdef HAVE_SYS_XATTR_H
	res.setxattr = (f & FS_USE_SETXATTR) ? s_setxattr : NULL;
	res.getxattr = (f & FS_USE_GETXATTR) ? s_getxattr : NULL;
	res.listxattr = (f & FS_USE_LISTXATTR) ? s_listxattr : NULL;
	res.removexattr = (f & FS_USE_REMOVEXATTR) ? s_removexattr : NULL;
#else
	res.setxattr = NULL;
	res.getxattr = NULL;
	res.listxattr = NULL;
	res.removexattr = NULL;
#endif
	res.opendir = (f & FS_USE_OPENDIR) ? s_opendir : NULL;
	res.readdir = (f & FS_USE_READDIR) ? s_readdir : NULL;
	res.releasedir = (f & FS_USE_RELEASEDIR) ? s_releasedir : NULL;
	res.fsyncdir = (f & FS_USE_FSYNCDIR) ? s_fsyncdir : NULL;
	res.access = (f & FS_USE_ACCESS) ? s_access : NULL;
	res.create = (f & FS_USE_CREATE) ? s_create : NULL;
	res.ftruncate = (f & FS_USE_FTRUNCATE) ? s_ftruncate : NULL;
	res.fgetattr = (f & FS_USE_FGETATTR) ? s_fgetattr : NULL;
	res.flag_nullpath_ok = 0;
	res.flag_nopath = 0;
	res.flag_utime_omit_ok = 0;
	res.lock = NULL;
	res.bmap = NULL;
	res.ioctl = NULL;
	res.poll = NULL;
	res.write_buf = NULL;
	res.read_buf = NULL;
	res.flock = NULL;
	res.fallocate = NULL;
	return res;
}

Fuse::Fuse(fs_use_flag_t opflags) : opflags(opflags)
{}

int Fuse::mount(int argc, char **argv) {
	struct fuse_operations ops = flags2struct(opflags);
	return fuse_main(argc, argv, &ops, this);
}

void * Fuse::s_init(struct fuse_conn_info *conn) {
	Fuse *ctx = (Fuse*)fuse_get_context()->private_data;
	ctx->init(conn);
	return ctx;
}
void  Fuse::s_destroy(void *userdata) {
	((Fuse*)userdata)->destroy(userdata);
}
int Fuse::s_getattr(const char *path, struct stat *statbuf) {
	return ((Fuse*)fuse_get_context()->private_data)->getattr(path,statbuf);
}
int Fuse::s_readlink(const char *path, char *link, size_t size) {
	return ((Fuse*)fuse_get_context()->private_data)->readlink(path,link,size);
}
int Fuse::s_mknod(const char *path, mode_t mode, dev_t dev) {
	return ((Fuse*)fuse_get_context()->private_data)->mknod(path,mode,dev);
}
int Fuse::s_mkdir(const char *path, mode_t mode) {
	return ((Fuse*)fuse_get_context()->private_data)->mkdir(path,mode);
}
int Fuse::s_unlink(const char *path) {
	return ((Fuse*)fuse_get_context()->private_data)->unlink(path);
}
int Fuse::s_rmdir(const char *path) {
	return ((Fuse*)fuse_get_context()->private_data)->rmdir(path);
}
int Fuse::s_symlink(const char *path, const char *link) {
	return ((Fuse*)fuse_get_context()->private_data)->symlink(path,link);
}
int Fuse::s_rename(const char *path, const char *newpath) {
	return ((Fuse*)fuse_get_context()->private_data)->rename(path,newpath);
}
int Fuse::s_link(const char *path, const char *link) {
	return ((Fuse*)fuse_get_context()->private_data)->link(path,link);
}
int Fuse::s_chmod(const char *path, mode_t mode) {
	return ((Fuse*)fuse_get_context()->private_data)->chmod(path,mode);
}
int Fuse::s_chown(const char *path, uid_t uid, gid_t gid) {
	return ((Fuse*)fuse_get_context()->private_data)->chown(path,uid,gid);
}
int Fuse::s_truncate(const char *path, off_t offset) {
	return ((Fuse*)fuse_get_context()->private_data)->truncate(path,offset);
}
int Fuse::s_utimens(const char *path, const struct timespec tv[2]) {
	return ((Fuse*)fuse_get_context()->private_data)->utimens(path,tv);
}
int Fuse::s_open(const char *path, struct fuse_file_info *fi) {
	return ((Fuse*)fuse_get_context()->private_data)->open(path,fi);
}
int Fuse::s_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	return ((Fuse*)fuse_get_context()->private_data)->read(path,buf,size,offset,fi);
}
int Fuse::s_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	return ((Fuse*)fuse_get_context()->private_data)->write(path,buf,size,offset,fi);
}
int Fuse::s_statfs(const char *path, struct statvfs *statv) {
	return ((Fuse*)fuse_get_context()->private_data)->statfs(path,statv);
}
int Fuse::s_flush(const char *path, struct fuse_file_info *fi) {
	return ((Fuse*)fuse_get_context()->private_data)->flush(path,fi);
}
int Fuse::s_release(const char *path, struct fuse_file_info *fi) {
	return ((Fuse*)fuse_get_context()->private_data)->release(path,fi);
}
int Fuse::s_fsync(const char *path, int datasync, struct fuse_file_info *fi) {
	return ((Fuse*)fuse_get_context()->private_data)->fsync(path,datasync,fi);
}
#ifdef HAVE_SYS_XATTR_H
int Fuse::s_setxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
	return ((Fuse*)fuse_get_context()->private_data)->setxattr(path,name,value,size,flags);
}
int Fuse::s_getxattr(const char *path, const char *name, char *value, size_t size) {
	return ((Fuse*)fuse_get_context()->private_data)->getxattr(path,name,value,size);
}
int Fuse::s_listxattr(const char *path, char *name, size_t size) {
	return ((Fuse*)fuse_get_context()->private_data)->listxattr(path,name,size);
}
int Fuse::s_removexattr(const char *path, const char *name) {
	return ((Fuse*)fuse_get_context()->private_data)->removexattr(path,name);
}
#endif
int Fuse::s_opendir(const char *path, struct fuse_file_info *fi) {
	return ((Fuse*)fuse_get_context()->private_data)->opendir(path,fi);
}
int Fuse::s_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
	return ((Fuse*)fuse_get_context()->private_data)->readdir(path,buf,filler,offset,fi);
}
int Fuse::s_releasedir(const char *path, struct fuse_file_info *fi) {
	return ((Fuse*)fuse_get_context()->private_data)->releasedir(path,fi);
}
int Fuse::s_fsyncdir(const char *path, int datasync, struct fuse_file_info *fi) {
	return ((Fuse*)fuse_get_context()->private_data)->fsyncdir(path,datasync,fi);
}
int Fuse::s_access(const char *path, int mask) {
	return ((Fuse*)fuse_get_context()->private_data)->access(path,mask);
}
int Fuse::s_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
	return ((Fuse*)fuse_get_context()->private_data)->create(path,mode,fi);
}
int Fuse::s_ftruncate(const char *path, off_t offset, struct fuse_file_info *fi) {
	return ((Fuse*)fuse_get_context()->private_data)->ftruncate(path,offset,fi);
}
int Fuse::s_fgetattr(const char *path, struct stat *statbuf, struct fuse_file_info *fi) {
	return ((Fuse*)fuse_get_context()->private_data)->fgetattr(path,statbuf,fi);
}
