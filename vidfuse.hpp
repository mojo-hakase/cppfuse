#include "fuse_graph.hpp"
#include "transparent_fuse.hpp"

#include <iostream>
#include <sstream>
#include <cstring>
using namespace std;

struct VidParams {
	unsigned int bitrate;
	bool hardsubbed;
	size_t dumpsize;
	std::string filepath;
};

typedef IFuseNode<VidParams>  IVidNode;
typedef  FuseNode<VidParams>   VidNode;
typedef IFuseGraph<VidParams> IVidGraph;
typedef  FuseGraph<VidParams>  VidGraph;

typedef PathObject<VidParams> VidPath;

class VidFuse;
class VidRootNode;
class VidOptNode;
class VidBitrateNode;
class VidParamDumpNode;
class VidFilesNode;

class VidFilesReadDirProxy : public FuseFDCallback {
	FuseFDCallback *clbk;
public:
	VidFilesReadDirProxy(FuseFDCallback *clbk) : clbk(clbk) {}
	struct filler_buf {
		void *buf;
		fuse_fill_dir_t filler;
		filler_buf(void *buf, fuse_fill_dir_t filler) : buf(buf), filler(filler) {}
	};

	static int filler(void *_buf, const char *name, const struct stat *statbuf, off_t off) {
		filler_buf *buf = (filler_buf*)_buf;
		struct stat statcpy;
		if (statbuf) {
			statcpy = *statbuf;
			statcpy.st_mode = (statcpy.st_mode & 07777) | S_IFDIR | 0111;
			statbuf = &statcpy;
		}
		return buf->filler(buf->buf, name, statbuf, off);
	}

	int readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
		// no need to split path, cause it won't be used anyway
		filler_buf mybuf(buf, filler);
		return clbk->readdir(path, &mybuf, VidFilesReadDirProxy::filler, offset, fi);
	}

	int releasedir(const char *path, fuse_file_info *fi) {
		// no need to split path, cause it won't be used anyway
		return clbk->releasedir(path, fi);
	}
};

class VidFilesNode : public IVidNode, public FuseFDCallback {
	TransparentFuse trans;
	VidFilesReadDirProxy proxy;
public:
	VidFilesNode(IVidGraph *graph, const std::string &baseDir) : IVidNode(graph), trans(baseDir), proxy(&trans) {}

	std::pair<bool,IFuseNode*> getNextNode(VidPath &path) {
		VidPath end = path.end();
		std::string subpath = path.to(end);
		--end;
		int res = trans.access(subpath.c_str(), F_OK);
		cout << subpath << ", res: " << res << endl;
		while (res == -ENOTDIR && end.getDepth() > path.getDepth()) {
			subpath = path.to(end);
			cout << subpath << endl;
			res = trans.access(subpath.c_str(), F_OK);
			--end;
		}
		if (res == 0) {
			path = end;
			++path;
			path.data->filepath = subpath;
			return std::pair<bool,IFuseNode*>(false, this);
		}
		return std::pair<bool,IFuseNode*>(false, nullptr);
	}

	int getattr(VidPath path, struct stat *statbuf) {
		cout << path.data->filepath.c_str() << endl;
		int res = trans.getattr(path.data->filepath.c_str(), statbuf);
		if (path.isEnd()) {
			statbuf->st_mode = (statbuf->st_mode & 07777) | S_IFDIR | 0111;
			return res;
		}
		return res;
	}

	int access(VidPath path, int mask) {
		return trans.access(path.data->filepath.c_str(), mask);
	}

	openres open(VidPath path, struct fuse_file_info *fi) {
		if (path.isEnd())
			return openres(-EISDIR,nullptr);
		return openres(trans.open(path.data->filepath.c_str(), fi), &trans);
	}

	openres opendir(VidPath path, struct fuse_file_info *fi) {
		if (path.isEnd()) {
			struct stat stats;
			trans.getattr(path.data->filepath.c_str(), &stats);
			if (S_ISDIR(stats.st_mode))
				return openres(trans.opendir(path.data->filepath.c_str(), fi), &proxy);
		}
		fi->fh = (fuse_fh_t) new VidPath(path);
		return openres(0, this);
	}
	int readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
		VidPath *vp = (VidPath*)fi->fh;
		cout << *((*vp).prev()) << endl;
		filler(buf, *((*vp).prev()), NULL, 0);
		return 0;
	}
	int releasedir(const char *path, fuse_file_info *fi) {
		delete ((VidPath*)fi->fh);
		return 0;
	}
};

class VidParamDumpNode : public VidNode {
public:
	VidParamDumpNode(IVidGraph *graph) : VidNode(graph, 1000, 1000, S_IFREG | 0444) {}

	int getattr(VidPath path, struct stat *statbuf) {
		statbuf->st_size = 0;
		return this->VidNode::getattr(path, statbuf);
	}

	openres open(VidPath path, struct fuse_file_info *fi) {
		VidPath *np = new VidPath(path);
		fi->fh = (fuse_fh_t) np;
		fi->direct_io = 1;
		fi->nonseekable = 1;
		np->data->dumpsize = 0;
		return openres(0, this);
	}

	int read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
		VidPath *p = (VidPath*) fi->fh;
		std::stringstream ss;
		ss << "Bitrate: " << p->data->bitrate << endl;
		cout << "\t\t\tOFFSET: " << offset << endl;
		if ((size_t)offset >= ss.str().size())
			return 0;
		p->data->dumpsize = ss.str().size();
		strncpy(buf, ss.str().c_str() + offset, size);
		return ((ss.str().size() - offset) > size) ? size : ss.str().size() - offset;
	}
	
	int release(const char *path, struct fuse_file_info *fi) {
		delete (VidPath*)fi->fh;
		return 0;
	}

	int fgetattr(const char *path, struct stat *statbuf, struct fuse_file_info *fi) {
		statbuf->st_size = ((VidPath*)fi->fh)->data->dumpsize;
		return this->VidNode::fgetattr(path, statbuf, fi);
	}
};

class VidBitrateNode : public VidNode {
	VidNode *optNode;
public:
	VidBitrateNode(IVidGraph *graph, VidNode *optNode) : VidNode(graph, 1000, 1000), optNode(optNode) {}

	std::pair<bool,IVidNode*> getNextNode(VidPath &path) {
		if (path.isEnd())
			return std::pair<bool,IVidNode*>(false, this);
		// parse bitrate
		unsigned int bitrate = 0;
		for (const char *c = *path; *c; ++c) {
			if (*c < '0' || *c > '9')
				return std::pair<bool,IVidNode*>(false,nullptr);
			bitrate = bitrate * 10 + (*c - '0');
		}
		path.data->bitrate = bitrate;
		++path;
		return std::pair<bool,IVidNode*>(true,optNode);
	}

	int readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
		struct stat stats, *statbuf = nullptr;
		if (0 == optNode->getattr(PathSplitter::New("")->begin<VidParams>(), &stats))
			statbuf = &stats;
		const char *bitrates[] = {"00240", "00800", "01200", "02000", "04000", "08000", "11650"};
		while (offset < 7) {
			cout << offset << endl;
			if (filler(buf, bitrates[offset], statbuf, offset + 1))
				return 0;
			++offset;
		}
		return 0;
	}
};

class SpeedTestNode : public IVidNode, public FuseFDCallback {
public:
	SpeedTestNode(IVidGraph *graph) : IVidNode(graph) {}

	std::pair<bool,IVidNode*> getNextNode(VidPath &path) {
		if (path.isEnd())
			return std::pair<bool,IVidNode*>(false, this);
		return std::pair<bool,IVidNode*>(false,nullptr);
	}
	int getattr(VidPath path, struct stat *statbuf) {
		statbuf->st_uid = 1000;
		statbuf->st_gid = 1000;
		statbuf->st_mode = S_IFREG | 0555;
		statbuf->st_size = -1;
		return 0;
	}
	int access(VidPath path, int mask) {
		return 0;
	}
	openres open(VidPath path, struct fuse_file_info *fi) {
		fi->nonseekable = 1;
		fi->direct_io = 1;
		return openres(0, this);
	}
	int read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
		return size;
	}
	int release(const char *path, struct fuse_file_inf *fi) {
		return 0;
	}
};

class VidFuse : public VidGraph {
public:
	VidFuse() {
		//this->root = this->registerNewNode(new VidRootNode(this));
		VidNode *vidroot = new VidNode(this, 1000, 1000);
		VidNode *optnode = new VidNode(this, 1000, 1000);
		VidFilesNode *filesnode = new VidFilesNode(this, "/");
		vidroot->registerNewNode("files", filesnode);
		vidroot->registerNewNode("options", optnode);
		optnode->registerNewNode("bitrate", new VidBitrateNode(this, optnode));
		optnode->registerNewNode("dump", new VidParamDumpNode(this));
		optnode->addExistingNode("files", filesnode);
		vidroot->registerNewNode("speedtest", new SpeedTestNode(this));
		this->root = vidroot;
		this->registerNewNode(vidroot);
	}
	
	std::shared_ptr<VidParams> newData() {
		return std::shared_ptr<VidParams>(new VidParams);
	}
};


