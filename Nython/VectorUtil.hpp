#ifndef _UTIL
#define _UTIL

#include <vector>
#include <iostream>

template<class T>
std::ostream& operator<<(std::ostream& lhs, std::vector<T> list) {
	for(unsigned i = 0; i < list.size(); i++) {
		lhs << list.at(i);
		if(i != list.size() - 1) {
			lhs << ", ";
		}
	}
	return lhs;
}

namespace VectorUtil {
	template<class T>
	int position(const std::vector<T>& list, T find) {
		int pos = -1;
		for(unsigned i = 0; i < list.size(); i++) {
			if(list.at(i) == find) {
				pos = i;
			}
		}
		return pos;
	}

	template<class T>
	inline bool contains(const std::vector<T>& list, T find) {
		return position(list, find) != -1;
	}
}
#endif
