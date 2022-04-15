//============================================================================
// Name        : memman.cpp
// Author      : Anton
// Version     :
// Copyright   : Your copyright notice
// Description : container class with iterator
// Reference doc: https://www.internalpointers.com/post/writing-custom-iterators-modern-cpp
//============================================================================

#include <algorithm>
#include <iostream>
#include <vector>

using std::vector;

template<typename T>

class vector2diter {
private:
	static const int chunk_size = (512ULL > sizeof(T) ? 512ULL : sizeof(T))
			/ sizeof(T);
	typename vector<T*>::iterator chunk_iter;
	const typename vector<T*>::iterator chunk_iter_end;
	T *elem_iter;
public:
	vector2diter(typename vector<T*>::iterator c_it, typename vector<T*>::iterator c_it_end, T *e_it) :
			chunk_iter(c_it), chunk_iter_end(c_it_end), elem_iter(e_it) {
	}
	typedef std::bidirectional_iterator_tag iterator_category;
	typedef T value_type;
	typedef T difference_type; //TODO: fix this
	typedef T &reference;
	typedef T *pointer;

	vector2diter& operator++() {
		/*
		 if (chunk_size - (elem_iter - *chunk_iter) == 0) {
		 chunk_iter++;
		 elem_iter = *chunk_iter;
		 } else
		 elem_iter++;
		 */
		elem_iter++;
		if (elem_iter - *chunk_iter == chunk_size) {
			// if (*(++chunk_iter) != nullptr) {
			if (++chunk_iter != chunk_iter_end) {
				elem_iter = *chunk_iter;
			}
		}
		return *this;
	}

	vector2diter& operator--() {
		/*
		 if (elem_iter == *chunk_iter) {
		 chunk_iter--;
		 elem_iter = *chunk_iter;
		 for (int j = 0; j < chunk_size - 1; j++)
		 elem_iter++;
		 } else
		 elem_iter--;
		 */

		elem_iter--;
		if (elem_iter < *chunk_iter) {
			elem_iter = *(--chunk_iter) + chunk_size - 1;
		}

		return *this;
	}

	T& operator*() {
		return *elem_iter;
	}

	friend bool operator==(const vector2diter<T> &a, const vector2diter<T> &b) {
		if (a.elem_iter == b.elem_iter)
			return true;
		else
			return false;
	}
	friend bool operator!=(const vector2diter<T> &a, const vector2diter<T> &b) {
		if (a.elem_iter == b.elem_iter) {
			return false;
		}
		return true;
	}

};

template<typename T>
class vector2d {
private:
	static const int chunk_size = (512ULL > sizeof(T) ? 512ULL : sizeof(T))
			/ sizeof(T);
	int n;
public:
	vector<T*> chunks;

	vector2d() :
			n(0) {
		chunks.push_back(new T[chunk_size]);
	}

	vector2d(int _n, T val = T()) :
			n(_n) {
		int chunk_count = 1 + n / chunk_size;

		for (int i = 0; i < chunk_count - 1; i++) {
			chunks.push_back(new T[chunk_size]);
			for (int j = 0; j < chunk_size; j++)
				chunks[i][j] = val;
		}
		chunks.push_back(new T[chunk_size]);
		for (int i = 0; i < _n - (chunk_count - 1) * chunk_size; i++) {
			chunks[chunk_count - 1][i] = val;
		}

	}

	~vector2d() {
		for (int i = chunks.size() - 1; i <= 0; i--) {
			delete[] chunks[i];
			chunks.pop_back();
		}
	}

	T& operator[](int i) {
		//return chunks[i / (512 / sizeof(T))][i % (512 / sizeof(T))];
		return chunks[i / chunk_size][i % chunk_size];
	}

	int size() {
		return n;
	}

	void resize(int newsize, const T &val = T()) {
		int new_chunk_count = 1 + newsize / chunk_size;
		int chunk_count = 1 + n / chunk_size;
		int razn = newsize - n;
		if (razn > 0) {
			for (int j = chunk_size
					- (chunk_size - (n - (chunk_count - 1) * chunk_size));
					j < chunk_size; j++) {
				chunks[chunk_count - 1][j] = val;
				razn--;
				if (razn == 0)
					break;
			}

			if (new_chunk_count > chunk_count) {
				for (int i = 0; i < new_chunk_count - chunk_count; i++) {
					chunks.push_back(new T[chunk_size]);
					for (int r = 0; r < chunk_size; r++) {
						if (razn > 0) {
							chunks[chunk_count + i][r] = val;
							razn--;
						}

					}
					if (razn == 0)
						break;
				}
			}

		}

		if (razn < 0) {
			razn = abs(razn);
			if (razn
					- (chunk_size
							- (chunk_size - (n - (chunk_count - 1) * chunk_size)))
					- 1 >= 0) {

				razn = razn
						- (chunk_size
								- (chunk_size
										- (n - (chunk_count - 1) * chunk_size)))
						- 1;
				std::cout << "razn: " << razn << std::endl;
				delete[] chunks[chunks.size() - 1];
				chunks.pop_back();
			}
			while (razn - chunk_size >= 0) {
				razn = razn - chunk_size;
				delete[] chunks[chunks.size() - 1];
				chunks.pop_back();
			}
			if (razn > 0) {
				for (int u = chunk_size - 2; u > 0; u--) {
					razn--;
					chunks[chunks.size() - 1][u] = val;
					if (razn == 0)
						break;
				}

			}
		}
		n = newsize;
	}

	void push_back(const T &val) {
		int chunk_count = 1 + n / chunk_size;
		n++;
		if (chunk_count != 1 + n / chunk_size) {
			chunks[chunks.size() - 1][chunk_size - 1] = val;
			chunks.push_back(new T[chunk_size]);
		}
		if (chunk_count == 1 + n / chunk_size) {
			chunks[chunks.size() - 1][chunk_size
					- (chunk_size - (n - (chunk_count - 1) * chunk_size)) - 1] =
					val;
		}

	}

	void pop_back() {
		int chunk_count = 1 + n / chunk_size;
		n--;
		if (chunk_count != 1 + n / chunk_size) {
			delete[] chunks[chunks.size() - 1];
			chunks.pop_back();
			chunks[chunks.size() - 1][chunk_size - 1] = 0;
		}
		if (chunk_count == 1 + n / chunk_size) {
			chunks[chunks.size() - 1][chunk_size
					- (chunk_size - (n - (chunk_count - 1) * chunk_size))] = 0;
		}

	}

	vector2diter<T> begin() {
		return vector2diter<T>(chunks.begin(), chunks.end(), chunks[0]);
	}

	/**
	 * Returns an iterator to the end (i.e. the element after the last element) of the given range.
	 * https://en.cppreference.com/w/cpp/iterator/end
	 */
	vector2diter<T> end() {
		int last_used_chank = n / chunk_size;
		int element_offset = n - last_used_chank * chunk_size - 1;
		if (element_offset < 0) {
			element_offset += chunk_size;
			--last_used_chank;
		}
		T *last_element_ptr = chunks[last_used_chank] + element_offset;
		return vector2diter<T>(chunks.begin() + last_used_chank,
				chunks.end(),
				last_element_ptr + 1
				);
	}

	void log() {
		std::cout << "size=" << n << std::endl;
		std::cout << "chunks=" << chunks.size() << std::endl;
		//std::cout << "type " << std::type_info(chunks) << std::endl;
		for (auto p : chunks)
			std::cout << p << ' ';
		std::cout << std::endl;
	}
};

using namespace std;
int main() {
	vector2d<double> v(100, 0.5);
	//v.log();
	cout << "v[64]=" << v[64] << endl;
	v.resize(50);
	//v.log();
	v.resize(128, 0.1);
	cout << "v[64]=" << v[64] << endl;
	//v.log;
	int cnt = 0;
	cout << endl;
	for (const auto &x : v) {
		cout << cnt << ": " << x << std::endl;
		++cnt;
	}
	std::cout << "Total elements: " << cnt << std::endl;

	cnt = 0;
	auto it = v.end();
	while (it != v.begin()) {
		--it;
		cout << cnt << ": " << *it << std::endl;
		++cnt;
	}
	std::cout << "Total elements: " << cnt << std::endl;

	printf("----------------------------------\n");

	//std::fill_n(v.begin(), 17, -0.11);
	std::generate(v.begin(), v.end(), [n = 0.0f] () mutable { return n++/10.0f; });
	cnt = 0;
	for (const auto &x : v) {
		cout << cnt << ": " << x << std::endl;
		++cnt;
	}
	std::cout << "Total elements: " << cnt << std::endl;

	printf("----------------------------------\n");

	std::reverse(v.begin(), v.end());
	cnt = 0;
	for (const auto &x : v) {
		cout << cnt << ": " << x << std::endl;
		++cnt;
	}
	std::cout << "Total elements: " << cnt << std::endl;

	auto it_start = v.begin();
	for (int i = 0; i <= 60; ++i)
		++it_start;
	auto it_end = it_start;
	for (int i = 0; i < 6; ++i)
		++it_end;
	cnt = 0;
	for (auto it = it_start; it != it_end; ++it) {
		cout << cnt << ": " << *it<< std::endl;
		++cnt;
	}
	std::cout << "Total elements: " << cnt << std::endl;
}
