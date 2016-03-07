#ifndef _PATH_SPLITTER_HPP_
#define _PATH_SPLITTER_HPP_

#include <string>
#include <vector>
#include <memory>

template <typename dataType = void>
class PathObject;

typedef PathObject<void> PathSplit;

class PathSplitter {
	template <typename T>
	friend class PathObject;

	std::weak_ptr<PathSplitter> weak_this;

	std::string path;
	std::string split;
	std::vector<size_t> slashes;
	bool complete;

	int slash(size_t i);

	PathSplitter(const char *path);
	PathSplitter(const std::string &path);
	PathSplitter(const PathSplitter&);
	PathSplitter& operator=(const PathSplitter&) = delete;
public:
	template <typename... Args>
	static std::shared_ptr<PathSplitter> New(Args&&... args) {
		std::shared_ptr<PathSplitter> ptr(new PathSplitter(args)...);
		ptr->weak_this = ptr;
		return ptr;
	}

	const char* operator[](size_t i);
	const std::string& fullpath();
	bool isDir();

	template <typename dataType = void>
	PathObject<dataType> begin(std::shared_ptr<dataType> data = nullptr);
};

template <typename dataType>
class PathObject {
	friend PathSplitter;

	std::shared_ptr<PathSplitter> path;
	size_t depth;
	PathObject(std::shared_ptr<PathSplitter> path, size_t depth = 0, std::shared_ptr<dataType> data = nullptr);

	void ensure_depth();

public:
	template <typename convType>
	PathObject<dataType>(const PathObject<convType>&);
	PathObject<dataType>(const PathObject<dataType>&);
	template <typename convType>
	PathObject<dataType>& operator=(const PathObject<convType>&);
	PathObject<dataType>& operator=(const PathObject<dataType>&);

	PathObject<dataType>& operator++();
	PathObject<dataType>& operator--();
	PathObject<dataType>  operator++(int); // postfix
	PathObject<dataType>  operator--(int); // postifx
	PathObject<dataType>  operator+(size_t off);
	PathObject<dataType>  operator-(size_t off);
	PathObject<dataType>& operator+=(size_t off);
	PathObject<dataType>& operator-=(size_t off);
	PathObject<dataType>  next();
	PathObject<dataType>  prev();

	const char* operator*();
	const char* operator[](int i);
	const char* rest();
	const char* full();

	size_t getDepth() {return depth;}

	bool isDir();
	bool isEnd();

	bool operator==(const PathObject &it);

	std::shared_ptr<dataType> data;
};



int PathSplitter::slash(size_t i) {
	while (!complete && i >= slashes.size() - 1) {
		size_t pos = path.find('/', slashes.back() + 1);
		if (pos == path.npos) {
			complete = true;
			slashes.push_back(path.size() - 1);
		} else if (pos == path.size() - 1) {
			complete = true;
			split[pos] = '\0';
			slashes.push_back(pos);
		} else {
			slashes.push_back(pos);
			split[pos] = '\0';
		}
	}
	if (i < slashes.size())
		return slashes[i] + 1;
	return 0;
}

PathSplitter::PathSplitter(const char *path) : slashes(1, 0), complete(false) {
	if (!path)
		return;
	if (path[0] != '/')
		this->path = '/';
	this->path += path;
	split = this->path;
	split[0] = '\0';
	if (split.size() == 1) {
		complete = true;
	}
}
PathSplitter::PathSplitter(const std::string &path) : slashes(1, 0), complete(false) {
	if (path[0] != '/')
		this->path = '/';
	this->path += path;
	split = this->path;
	split[0] = '\0';
	if (path.size() == 1) {
		complete = true;
	}
}
PathSplitter::PathSplitter(const PathSplitter &o) : path(o.path), split(o.split), slashes(o.slashes), complete(o.complete)
{}

const char* PathSplitter::operator[](size_t i) {
	return split.c_str() + slash(i);
}

const std::string& PathSplitter::fullpath() {
	return path;
}

bool PathSplitter::isDir() {
	return path.back() == '/';
}

template <typename dataType>
PathObject<dataType> PathSplitter::begin(std::shared_ptr<dataType> data) {
	return PathObject<dataType>(weak_this.lock(), 0, data);
}





template <typename T>
PathObject<T>::PathObject(std::shared_ptr<PathSplitter> path, size_t depth, std::shared_ptr<T> data) : path(path), depth(depth), data(data) {
	ensure_depth();
}

template <typename dataType>
template <typename convType>
PathObject<dataType>::PathObject(const PathObject<convType> &o) : path(o.path), depth(o.depth), data(nullptr) {}

template <typename dataType>
PathObject<dataType>::PathObject(const PathObject<dataType>  &o) : path(o.path), depth(o.depth), data(o.data) {}

template <typename dataType>
template <typename convType>
PathObject<dataType>& PathObject<dataType>::operator=(const PathObject<convType>& o) {
	path = o.path;
	depth = o.depth;
	data = nullptr;
}

template <typename dataType>
PathObject<dataType>& PathObject<dataType>::operator=(const PathObject<dataType>& o) {
	path = o.path;
	depth = o.depth;
	data = o.data;
}

template <typename T>
void PathObject<T>::ensure_depth() {
	if (path->slash(depth) == 0)
		depth = path->slashes.size() - 1;
}

template <typename T>
PathObject<T>& PathObject<T>::operator++() {
	ensure_depth();
	depth++;
	return *this;
}
template <typename T>
PathObject<T>& PathObject<T>::operator--() {
	if (depth > 0)
		depth--;
	return *this;
}
template <typename T>
PathObject<T> PathObject<T>::operator++(int) {
	ensure_depth();
	int old_depth = depth;
	depth++;
	return PathObject<T>(path, old_depth, data);
}
template <typename T>
PathObject<T> PathObject<T>::operator--(int) {
	int old_depth = depth;
	if (depth > 0) {
		depth--;
		return PathObject<T>(path, old_depth, data);
	}
	else
		return *this;
}
template <typename T>
PathObject<T> PathObject<T>::operator+(size_t off) {
	return PathObject(path, depth + off);
}
template <typename T>
PathObject<T> PathObject<T>::operator-(size_t off) {
	if (depth <= off)
		return PathObject(path);
	return PathObject(path, depth - off);
}
template <typename T>
PathObject<T>& PathObject<T>::operator+=(size_t off) {
	depth += off;
	ensure_depth();
	return *this;
}
template <typename T>
PathObject<T>& PathObject<T>::operator-=(size_t off) {
	depth = (depth < off) ? 0 : depth - off;
	return *this;
}
template <typename T>
PathObject<T> PathObject<T>::next() {
	return PathObject(path, depth + 1);
}
template <typename T>
PathObject<T> PathObject<T>::prev() {
	if (depth)
		return PathObject(path, depth - 1);
	return PathObject(path);
}

template <typename T>
const char* PathObject<T>::operator*() {
	return (*path)[depth];
}
template <typename T>
const char* PathObject<T>::operator[](int i) {
	return (*path)[depth + i];
}
template <typename T>
const char* PathObject<T>::rest() {
	if (depth == path->slashes.size() - 1)
		return "/";
	return path->path.c_str() + path->slashes[depth];
}
template <typename T>
const char* PathObject<T>::full() {
	return path->path.c_str();
}

template <typename T>
bool PathObject<T>::isDir() {
	return !isEnd() || path->isDir();
}

template <typename T>
bool PathObject<T>::isEnd() {
	return path->complete && depth == path->slashes.size() - 1;
}

template <typename T>
bool PathObject<T>::operator==(const PathObject<T> &it) {
	if (path != it.path)
		return false;
	ensure_depth();
	return (depth == it.depth ||
		(it.depth > depth && depth >= path->slashes.size())
		);
}



#endif // _PATH_SPLITTER_HPP_
