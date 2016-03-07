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

class VidFilesNode : public VidNode {
	TransparentFuse trans;
public:
	VidFilesNode(IVidGraph *graph, const std::string &baseDir) : VidNode(graph), trans(baseDir) {}

	std::pair<bool,IFuseNode*> getNextNode(VidPath &path) {
		return std::pair<bool,IFuseNode*>(false, this);
	}

	int getattr(VidPath path, struct stat *statbuf) {
		return trans.getattr(path.rest(), statbuf);
	}

	openres opendir(VidPath path, struct fuse_file_info *fi) {
		return openres(trans.opendir(path.rest(), fi), this);
	}

	int readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
		// no need to split path, cause it won't be used anyway
		return trans.readdir(path, buf, filler, offset, fi);
	}

	int releasedir(const char *path, fuse_file_info *fi) {
		// no need to split path, cause it won't be used anyway
		return trans.releasedir(path, fi);
	}
};

class VidParamDumpNode : public VidNode {
public:
	VidParamDumpNode(IVidGraph *graph) : VidNode(graph, 1000, 1000, S_IFREG | 0444) {}

	int getattr(VidPath path, struct stat *statbuf) {
		statbuf->st_size = 1;
		return this->VidNode::getattr(path, statbuf);
	}

	openres open(VidPath path, struct fuse_file_info *fi) {
		VidPath *np = new VidPath(path);
		fi->fh = (fuse_fh_t) np;
		np->data->dumpsize = 100;
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
};

class VidFuse : public VidGraph {
public:
	VidFuse() {
		//this->root = this->registerNewNode(new VidRootNode(this));
		VidNode *vidroot = new VidNode(this, 1000, 1000);
		VidNode *optnode = new VidNode(this, 1000, 1000);
		VidNode *filesnode = new VidFilesNode(this, "/mnt/Akame/Videos");
		vidroot->registerNewNode("files", filesnode);
		vidroot->registerNewNode("options", optnode);
		optnode->registerNewNode("bitrate", new VidBitrateNode(this, optnode));
		optnode->registerNewNode("dump", new VidParamDumpNode(this));
		optnode->addExistingNode("files", filesnode);
		this->root = vidroot;
		this->registerNewNode(vidroot);
	}
	
	std::shared_ptr<VidParams> newData() {
		return std::shared_ptr<VidParams>(new VidParams);
	}
};


