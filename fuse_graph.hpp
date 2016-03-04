#include <unordered_map>	// subnodes
#include <unordered_set>	// nodelist (graph)

#include "cppfuse.hpp"
#include "path_splitter.hpp"
#include "file_descriptor.hpp"


template <typename dataType>
class IFuseGraph;

template <typename dataType>
class IFuseNode;


typedef std::pair<int,FuseFDCallback*> openres;

template <typename dataType>
class IFuseNode {
protected:
	IFuseGraph<dataType> *graph;
public:
	IFuseNode(IFuseGraph<dataType>*);

	IFuseNode<dataType>* findNode(PathObject<dataType> &path);
	virtual std::pair<bool,IFuseNode<dataType>*> getNextNode(PathObject<dataType> &path);

	virtual int	getattr   	(PathObject<dataType> path, struct stat *statbuf) {return -ENOENT;}
	virtual int 	readlink  	(PathObject<dataType> path, char *link, size_t size) {return -ENOENT;}
	virtual int 	mknod     	(PathObject<dataType> path, mode_t mode, dev_t dev) {return -ENOENT;}
	virtual int 	mkdir    	(PathObject<dataType> path, mode_t mode) {return -ENOENT;}
	virtual int 	unlink    	(PathObject<dataType> path) {return -ENOENT;}
	virtual int 	rmdir    	(PathObject<dataType> path) {return -ENOENT;}
	virtual int 	symlink    	(const char *path, PathObject<dataType> link) {return -ENOENT;}
	virtual int 	rename    	(PathObject<dataType> path, const char *newpath) {return -ENOENT;}
	virtual int 	link    	(PathObject<dataType> path, const char *link) {return -ENOENT;}
	virtual int 	chmod    	(PathObject<dataType> path, mode_t mode) {return -ENOENT;}
	virtual int 	chown    	(PathObject<dataType> path, uid_t uid, gid_t gid) {return -ENOENT;}
	virtual int 	truncate    	(PathObject<dataType> path, off_t offset) {return -ENOENT;}
	virtual int 	utimens    	(PathObject<dataType> path, const struct timespec tv[2]) {return -ENOENT;}
	virtual openres	open    	(PathObject<dataType> path, struct fuse_file_info *fi) {return openres(-ENOENT,nullptr);}
	virtual int 	statfs  	(PathObject<dataType> path, struct statvfs *statv) {return -ENOENT;}
#ifdef HAVE_SYS_XATTR_H
	virtual int 	setxattr    	(PathObject<dataType> path, const char *name, const char *value, size_t size, int flags) {return -ENOENT;}
	virtual int 	getxattr  	(PathObject<dataType> path, const char *name, char *value, size_t size) {return -ENOENT;}
	virtual int 	listxattr  	(PathObject<dataType> path, char *name, size_t size) {return -ENOENT;}
	virtual int 	removexattr    	(PathObject<dataType> path, const char *name) {return -ENOENT;}
#endif
	virtual openres	opendir  	(PathObject<dataType> path, struct fuse_file_info *fi) {return openres(-ENOENT,nullptr);}
	virtual int 	access  	(PathObject<dataType> path, int mask) {return -ENOENT;}
	virtual openres	create    	(PathObject<dataType> path, mode_t mode, struct fuse_file_info *fi) {return openres(-ENOENT,nullptr);}

	virtual ~IFuseNode<dataType>() {}
};

template <typename dataType>
IFuseNode<dataType>::IFuseNode(IFuseGraph<dataType> *graph) : graph(graph)
{}

template <typename dataType>
std::pair<bool,IFuseNode<dataType>*> IFuseNode<dataType>::getNextNode(PathObject<dataType> &path) {
	return std::pair<bool,IFuseNode<dataType>*>(false,nullptr);
}

template <typename dataType>
IFuseNode<dataType>* IFuseNode<dataType>::findNode(PathObject<dataType> &path) {
	auto next = getNextNode(path);
	if (!next.first)
		return next.second;
	else if (next.second)
		return next.second->findNode(path);
	else
		return nullptr;
}

template <typename dataType>
class FuseNode : public IFuseNode<dataType>, public FuseFDCallback {
protected:
	std::unordered_map<std::string,IFuseNode<dataType>*> subnodes;
	uid_t uid;
	gid_t gid;
	mode_t mode;
	static const mode_t default_mode = S_IFDIR | 0555;

public:
	FuseNode<dataType>(IFuseGraph<dataType> *graph, uid_t uid = 0, gid_t gid = 0, mode_t mode = default_mode);
	IFuseNode<dataType>* registerNewNode(const std::string &name, IFuseNode<dataType>*);
	IFuseNode<dataType>* addExistingNode(const std::string &name, const std::string &path);

	virtual std::pair<bool,IFuseNode<dataType>*> getNextNode(PathObject<dataType> &path);

	virtual int	getattr   	(PathObject<dataType> path, struct stat *statbuf);
/*
	virtual int 	readlink  	(PathObject<dataType> path, char *link, size_t size);
	virtual int 	mknod     	(PathObject<dataType> path, mode_t mode, dev_t dev);
	virtual int 	mkdir    	(PathObject<dataType> path, mode_t mode);
	virtual int 	unlink    	(PathObject<dataType> path);
	virtual int 	rmdir    	(PathObject<dataType> path);
	virtual int 	symlink    	(const char *path, PathObject<dataType> link);
	virtual int 	rename    	(PathObject<dataType> path, const char *newpath);
	virtual int 	link    	(PathObject<dataType> path, const char *link);
	virtual int 	chmod    	(PathObject<dataType> path, mode_t mode);
	virtual int 	chown    	(PathObject<dataType> path, uid_t uid, gid_t gid);
	virtual int 	truncate    	(PathObject<dataType> path, off_t offset);
	virtual int 	utimens    	(PathObject<dataType> path, const struct timespec tv[2]);
*/
	virtual openres	open    	(PathObject<dataType> path, struct fuse_file_info *fi);
	virtual int flush       	(const char *path, struct fuse_file_info *fi);
	virtual int release     	(const char *path, struct fuse_file_info *fi);
/*
	virtual int 	statfs  	(PathObject<dataType> path, struct statvfs *statv);
#ifdef HAVE_SYS_XATTR_H
	virtual int 	setxattr    	(PathObject<dataType> path, const char *name, const char *value, size_t size, int flags);
	virtual int 	getxattr  	(PathObject<dataType> path, const char *name, char *value, size_t size);
	virtual int 	listxattr  	(PathObject<dataType> path, char *name, size_t size);
	virtual int 	removexattr    	(PathObject<dataType> path, const char *name);
#endif
*/
	virtual openres	opendir  	(PathObject<dataType> path, struct fuse_file_info *fi);
	virtual int 	readdir 	(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi);
	virtual int 	releasedir	(const char *path, struct fuse_file_info *fi);
/*
	virtual int 	access  	(PathObject<dataType> path, int mask);
	virtual int 	create    	(PathObject<dataType> path, mode_t mode, struct fuse_file_info *fi);
*/
	virtual int 	fgetattr  	(const char *path, struct stat *statbuf, struct fuse_file_info *fi);

	virtual ~FuseNode<dataType>() {}
};

template <typename dataType>
FuseNode<dataType>::FuseNode(IFuseGraph<dataType> *graph, uid_t uid, gid_t gid, mode_t mode)
	: IFuseNode<dataType>(graph), uid(uid), gid(gid), mode(mode)
{}

template <typename dataType>
IFuseNode<dataType> *FuseNode<dataType>::registerNewNode(const std::string &name, IFuseNode<dataType> *node) {
	if (!node)
		return node;
	this->graph->registerNewNode(node);
	subnodes.insert(std::pair<std::string,IFuseNode<dataType>*>(name, node));
	return node;
}

template <typename dataType>
IFuseNode<dataType> *FuseNode<dataType>::addExistingNode(const std::string &name, const std::string &path) {
	IFuseNode<dataType> *node = this->graph->findNode(path);
	if (node)
		subnodes.insert(std::pair<std::string,IFuseNode<dataType>*>(name, node));
	return node;
}

template <typename dataType>
std::pair<bool,IFuseNode<dataType>*> FuseNode<dataType>::getNextNode(PathObject<dataType> &path) {
	if (path.isEnd())
		return std::pair<bool,IFuseNode<dataType>*>(false, this);
	auto it = subnodes.find(*path);
	if (it == subnodes.end())
		return std::pair<bool,IFuseNode<dataType>*>(false, nullptr);
	++path;
	return std::pair<bool,IFuseNode<dataType>*>(true, it->second);
}

template <typename dataType>
int FuseNode<dataType>::getattr(PathObject<dataType> path, struct stat *statbuf) {
	statbuf->st_uid = uid;
	statbuf->st_gid = gid;
	statbuf->st_mode = mode;
	return 0;
}

template <typename dataType>
openres FuseNode<dataType>::open(PathObject<dataType> path, struct fuse_file_info *fi) {
	fi->fh = 0;
	return openres(0, this);
}

template <typename dataType>
int FuseNode<dataType>::flush(const char *path, struct fuse_file_info *fi) {
	return 0;
}

template <typename dataType>
int FuseNode<dataType>::release(const char *path, struct fuse_file_info *fi) {
	return 0;
}

template <typename dataType>
openres FuseNode<dataType>::opendir(PathObject<dataType> path, struct fuse_file_info *fi) {
	fi->fh = (fuse_fh_t) new PathObject<dataType>(path);
	return openres(0, this);
}

template <typename dataType>
int FuseNode<dataType>::readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
	int i = 0;
	auto it = subnodes.begin();
	for (; i < offset && it != subnodes.end(); ++i)
		++it;

	struct stat stats;
	struct stat *statbuf = &stats;
	PathObject<dataType> *pobj = (PathObject<dataType>*) fi->fh;
	for (; it != subnodes.end(); ++it) {
		it->second->getattr(*pobj, statbuf);
		if (filler(buf, it->first.c_str(), statbuf, ++i))
			return 0;
	}
	return 0;
}

template <typename dataType>
int FuseNode<dataType>::releasedir(const char *path, struct fuse_file_info *fi) {
	delete ((PathObject<dataType>*)fi->fh);
	return 0;
}

template <typename dataType>
int FuseNode<dataType>::fgetattr(const char *path, struct stat *statbuf, struct fuse_file_info *fi) {
	statbuf->st_uid = uid;
	statbuf->st_gid = gid;
	statbuf->st_mode = mode;
	return 0;
}


template <typename dataType>
class IFuseGraph {
	std::unordered_set<IFuseNode<dataType>*> nodes;
protected:
	IFuseNode<dataType> *root;
	virtual std::shared_ptr<dataType> newData();
public:
	IFuseNode<dataType> *findNode(const char *path);
	IFuseNode<dataType> *findNode(PathObject<dataType> &path);
	IFuseNode<dataType> *registerNewNode(IFuseNode<dataType> *node);

	virtual ~IFuseGraph<dataType>();
};

template <typename dataType>
std::shared_ptr<dataType> IFuseGraph<dataType>::newData() {
	// not sure if default constructor is available
	//return new dataType;
	return nullptr;
}

template <typename dataType>
IFuseNode<dataType> *IFuseGraph<dataType>::findNode(const char *path) {
	if (!root)
		return nullptr;
	return root->findNode(PathSplitter::New(path)->begin<dataType>(this->newData()));
}

template <typename dataType>
IFuseNode<dataType> *IFuseGraph<dataType>::findNode(PathObject<dataType> &path) {
	if (!root)
		return nullptr;
	return root->findNode(path);
}

template <typename dataType>
IFuseNode<dataType> *IFuseGraph<dataType>::registerNewNode(IFuseNode<dataType> *node) {
	if (node)
		nodes.insert(node);
	return node;
}

template <typename dataType>
IFuseGraph<dataType>::~IFuseGraph<dataType>() {
	for (auto it = nodes.begin(); it != nodes.end(); ++it) {
		delete *it;
	}
}


template <typename dataType>
class FuseGraph : public IFuseGraph<dataType>, public Fuse {
	FDManager fdm;
protected:
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
	virtual int 	fgetattr  	(const char *path, struct stat *statbuf, struct fuse_file_info *fi);

	virtual ~FuseGraph<dataType>() {}
};

template <typename dataType>
int FuseGraph<dataType>::getattr(const char *path, struct stat *statbuf) {
	PathObject<dataType> pobj(PathSplitter::New(path)->begin<dataType>(this->newData()));
	IFuseNode<dataType> *node = this->findNode(pobj);
	if (node)
		return node->getattr(pobj, statbuf);
	return -ENOENT;
}
template <typename dataType>
int FuseGraph<dataType>::readlink(const char *path, char *link, size_t size) {
	PathObject<dataType> pobj(PathSplitter::New(path)->begin<dataType>(this->newData()));
	IFuseNode<dataType> *node = this->findNode(pobj);
	if (node)
		return node->readlink(pobj,link,size);
	return -ENOENT;
}
template <typename dataType>
int FuseGraph<dataType>::mknod(const char *path, mode_t mode, dev_t dev) {
	PathObject<dataType> pobj(PathSplitter::New(path)->begin<dataType>(this->newData()));
	IFuseNode<dataType> *node = this->findNode(pobj);
	if (node)
		return node->mknod(pobj,mode,dev);
	return -ENOENT;
}
template <typename dataType>
int FuseGraph<dataType>::mkdir(const char *path, mode_t mode) {
	PathObject<dataType> pobj(PathSplitter::New(path)->begin<dataType>(this->newData()));
	IFuseNode<dataType> *node = this->findNode(pobj);
	if (node)
		return node->mkdir(pobj,mode);
	return -ENOENT;
}
template <typename dataType>
int FuseGraph<dataType>::unlink(const char *path) {
	PathObject<dataType> pobj(PathSplitter::New(path)->begin<dataType>(this->newData()));
	IFuseNode<dataType> *node = this->findNode(pobj);
	if (node)
		return node->unlink(pobj);
	return -ENOENT;
}
template <typename dataType>
int FuseGraph<dataType>::rmdir(const char *path) {
	PathObject<dataType> pobj(PathSplitter::New(path)->begin<dataType>(this->newData()));
	IFuseNode<dataType> *node = this->findNode(pobj);
	if (node)
		return node->rmdir(pobj);
	return -ENOENT;
}
template <typename dataType>
int FuseGraph<dataType>::symlink(const char *path, const char *link) {
	PathObject<dataType> pobj(PathSplitter::New(path)->begin<dataType>(this->newData()));
	IFuseNode<dataType> *node = this->findNode(pobj);
	if (node)
		return node->symlink(path,pobj);
	return -ENOENT;
}
template <typename dataType>
int FuseGraph<dataType>::rename(const char *path, const char *newpath) {
	PathObject<dataType> pobj(PathSplitter::New(path)->begin<dataType>(this->newData()));
	IFuseNode<dataType> *node = this->findNode(pobj);
	if (node)
		return node->rename(pobj,newpath);
	return -ENOENT;
}
template <typename dataType>
int FuseGraph<dataType>::link(const char *path, const char *link) {
	PathObject<dataType> pobj(PathSplitter::New(path)->begin<dataType>(this->newData()));
	IFuseNode<dataType> *node = this->findNode(pobj);
	if (node)
		return node->link(pobj,link);
	return -ENOENT;
}
template <typename dataType>
int FuseGraph<dataType>::chmod(const char *path, mode_t mode) {
	PathObject<dataType> pobj(PathSplitter::New(path)->begin<dataType>(this->newData()));
	IFuseNode<dataType> *node = this->findNode(pobj);
	if (node)
		return node->chmod(pobj,mode);
	return -ENOENT;
}
template <typename dataType>
int FuseGraph<dataType>::chown(const char *path, uid_t uid, gid_t gid) {
	PathObject<dataType> pobj(PathSplitter::New(path)->begin<dataType>(this->newData()));
	IFuseNode<dataType> *node = this->findNode(pobj);
	if (node)
		return node->chown(pobj,uid,gid);
	return -ENOENT;
}
template <typename dataType>
int FuseGraph<dataType>::truncate(const char *path, off_t offset) {
	PathObject<dataType> pobj(PathSplitter::New(path)->begin<dataType>(this->newData()));
	IFuseNode<dataType> *node = this->findNode(pobj);
	if (node)
		return node->truncate(pobj,offset);
	return -ENOENT;
}
template <typename dataType>
int FuseGraph<dataType>::utimens(const char *path, const struct timespec tv[2]) {
	PathObject<dataType> pobj(PathSplitter::New(path)->begin<dataType>(this->newData()));
	IFuseNode<dataType> *node = this->findNode(pobj);
	if (node)
		return node->utimens(pobj,tv);
	return -ENOENT;
}

template <typename dataType>
int FuseGraph<dataType>::open(const char *path, struct fuse_file_info *fi) {
	PathObject<dataType> pobj(PathSplitter::New(path)->begin<dataType>(this->newData()));
	IFuseNode<dataType> *node = this->findNode(pobj);
	if (!node)
		return -ENOENT;
	openres result = node->open(pobj,fi);
	if (result.first == 0 && result.second) {
		fdm.registerFD(fi, result.second);
	}
	else if (result.first == 0 && !result.second) {
		return -ENOENT;
	}
	return result.first;
}
template <typename dataType>
int FuseGraph<dataType>::read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	FileDescriptor *fd = fdm.find(fi);
	if (!fd)
		return -EBADF;
	fuse_fh_t fh = fi->fh;
	fi->fh = fd->fh;
	int result = fd->node->read(path, buf, size, offset, fi);
	fi->fh = fh;
	return result;
}
template <typename dataType>
int FuseGraph<dataType>::write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	FileDescriptor *fd = fdm.find(fi);
	if (!fd)
		return -EBADF;
	fuse_fh_t fh = fi->fh;
	fi->fh = fd->fh;
	int result = fd->node->write(path, buf, size, offset, fi);
	fi->fh = fh;
	return result;
}
template <typename dataType>
int FuseGraph<dataType>::statfs(const char *path, struct statvfs *statv) {
	PathObject<dataType> pobj(PathSplitter::New(path)->begin<dataType>(this->newData()));
	IFuseNode<dataType> *node = this->findNode(pobj);
	if (node)
		return node->statfs(pobj,statv);
	return -ENOENT;
}
template <typename dataType>
int FuseGraph<dataType>::flush(const char *path, struct fuse_file_info *fi) {
	FileDescriptor *fd = fdm.find(fi);
	if (!fd)
		return -EBADF;
	fuse_fh_t fh = fi->fh;
	fi->fh = fd->fh;
	int result = fd->node->flush(path, fi);
	fi->fh = fh;
	return result;
}
template <typename dataType>
int FuseGraph<dataType>::release(const char *path, struct fuse_file_info *fi) {
	FileDescriptor *fd = fdm.find(fi);
	if (!fd)
		return -EBADF;	// ignored, but shouldn't happen anyway
	fuse_fh_t fh = fi->fh;
	fi->fh = fd->fh;
	fd->node->release(path, fi);
	fi->fh = fh;
	if (!fi->flock_release) {
		fdm.releaseFD(fh);
	}
	return 0;
}
template <typename dataType>
int FuseGraph<dataType>::fsync(const char *path, int datasync, struct fuse_file_info *fi) {
	FileDescriptor *fd = fdm.find(fi);
	if (!fd)
		return -EBADF;
	fuse_fh_t fh = fi->fh;
	fi->fh = fd->fh;
	int result = fd->node->fsync(path, datasync, fi);
	fi->fh = fh;
	return result;
}
#ifdef HAVE_SYS_XATTR_H
template <typename dataType>
int FuseGraph<dataType>::setxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
	PathObject<dataType> pobj(PathSplitter::New(path)->begin<dataType>(this->newData()));
	IFuseNode<dataType> *node = this->findNode(pobj);
	if (node)
		return node->setxattr(pobj,name,value,size,flags);
	return -ENOENT;
}
template <typename dataType>
int FuseGraph<dataType>::getxattr(const char *path, const char *name, char *value, size_t size) {
	PathObject<dataType> pobj(PathSplitter::New(path)->begin<dataType>(this->newData()));
	IFuseNode<dataType> *node = this->findNode(pobj);
	if (node)
		return node->getxattr(pobj,name,value,size);
	return -ENOENT;
}
template <typename dataType>
int FuseGraph<dataType>::listxattr(const char *path, char *name, size_t size) {
	PathObject<dataType> pobj(PathSplitter::New(path)->begin<dataType>(this->newData()));
	IFuseNode<dataType> *node = this->findNode(pobj);
	if (node)
		return node->listxattr(pobj,name,size);
	return -ENOENT;
}
template <typename dataType>
int FuseGraph<dataType>::removexattr(const char *path, const char *name) {
	PathObject<dataType> pobj(PathSplitter::New(path)->begin<dataType>(this->newData()));
	IFuseNode<dataType> *node = this->findNode(pobj);
	if (node)
		return node->removexattr(pobj,name);
	return -ENOENT;
}
#endif
template <typename dataType>
int FuseGraph<dataType>::opendir(const char *path, struct fuse_file_info *fi) {
	PathObject<dataType> pobj(PathSplitter::New(path)->begin<dataType>(this->newData()));
	IFuseNode<dataType> *node = this->findNode(pobj);
	if (!node)
		return -ENOENT;
	openres result = node->opendir(pobj,fi);
	if (result.first == 0 && result.second) {
		fdm.registerFD(fi, result.second);
	}
	else if (result.first == 0 && !result.second) {
		return -ENOENT;
	}
	return result.first;
}
template <typename dataType>
int FuseGraph<dataType>::readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
	FileDescriptor *fd = fdm.find(fi);
	if (!fd)
		return -EBADF;
	fuse_fh_t fh = fi->fh;
	fi->fh = fd->fh;
	int result = fd->node->readdir(path, buf, filler, offset, fi);
	fi->fh = fh;
	return result;
}
template <typename dataType>
int FuseGraph<dataType>::releasedir(const char *path, struct fuse_file_info *fi) {
	FileDescriptor *fd = fdm.find(fi);
	if (!fd)
		return -EBADF;
	fuse_fh_t fh = fi->fh;
	fi->fh = fd->fh;
	int result = fd->node->releasedir(path, fi);
	fi->fh = fh;
	fdm.releaseFD(fh);
	return result;
}
template <typename dataType>
int FuseGraph<dataType>::fsyncdir(const char *path, int datasync, struct fuse_file_info *fi) {
	FileDescriptor *fd = fdm.find(fi);
	if (!fd)
		return -EBADF;
	fuse_fh_t fh = fi->fh;
	fi->fh = fd->fh;
	int result = fd->node->fsyncdir(path, datasync, fi);
	fi->fh = fh;
	return result;
}
template <typename dataType>
int FuseGraph<dataType>::access(const char *path, int mask) {
	PathObject<dataType> pobj(PathSplitter::New(path)->begin<dataType>(this->newData()));
	IFuseNode<dataType> *node = this->findNode(pobj);
	if (node)
		return node->access(pobj,mask);
	return -ENOENT;
}
template <typename dataType>
int FuseGraph<dataType>::create(const char *path, mode_t mode, struct fuse_file_info *fi) {
	PathObject<dataType> pobj(PathSplitter::New(path)->begin<dataType>(this->newData()));
	IFuseNode<dataType> *node = this->findNode(pobj);
	if (!node)
		return -ENOENT;
	openres result = node->create(pobj,mode,fi);
	if (result.first == 0 && result.second) {
		fdm.registerFD(fi, result.second);
	}
	else if (result.first == 0 && !result.second) {
		return -ENOENT;
	}
	return result.first;
}
template <typename dataType>
int FuseGraph<dataType>::ftruncate(const char *path, off_t offset, struct fuse_file_info *fi) {
	FileDescriptor *fd = fdm.find(fi);
	if (!fd)
		return -EBADF;
	fuse_fh_t fh = fi->fh;
	fi->fh = fd->fh;
	int result = fd->node->ftruncate(path, offset, fi);
	fi->fh = fh;
	return result;
}
template <typename dataType>
int FuseGraph<dataType>::fgetattr(const char *path, struct stat *statbuf, struct fuse_file_info *fi) {
	FileDescriptor *fd = fdm.find(fi);
	if (!fd)
		return -EBADF;
	fuse_fh_t fh = fi->fh;
	fi->fh = fd->fh;
	int result = fd->node->fgetattr(path, statbuf, fi);
	fi->fh = fh;
	return result;
}

