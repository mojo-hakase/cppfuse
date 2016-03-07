#include "cppfuse.hpp"

class SpeedTest : public Fuse {
public:
	int getattr(const char *path, struct stat *statbuf) {
		statbuf->st_uid = 1000;
		statbuf->st_gid = 1000;
		statbuf->st_size = -1;
		statbuf->st_mode = 0555 | (path[1] ? S_IFREG : S_IFDIR);
		return 0;
	}
	int access(const char *path, int mask) {
		return 0;
	}
	int open(const char *path, struct fuse_file_info *fi) {
		fi->nonseekable = 1;
		fi->direct_io = 1;
		return 0;
	}
	int read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
		return size;
	}
	int release(const char *path, struct fuse_file_inf *fi) {
		return 0;
	}
};

int main(int argc, char **argv) {
	SpeedTest t;
	t.mount(argc, argv);
}
