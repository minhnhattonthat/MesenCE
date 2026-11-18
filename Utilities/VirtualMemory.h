#pragma once
#include "pch.h"
#include <new>
#include <type_traits>

class VirtualMemory
{
public:
	//Zero-filled memory straight from the OS: a page only takes physical RAM once it is touched.
	//Throws std::bad_alloc on failure. Release with Free(ptr, size).
	static void* AllocZeroed(size_t size);
	static void Free(void* ptr, size_t size);
};

//Fixed-size array of T backed by VirtualMemory, for large tables indexed by address (one entry per byte of
//ROM/RAM) of which only a small part is ever used. std::vector<T>(n) writes every element up front, which
//commits all of it to physical RAM; here untouched entries cost nothing.
//An all-zero T must be a valid default value.
template<typename T>
class ZeroedArray
{
	static_assert(std::is_trivially_copyable<T>::value, "ZeroedArray needs a trivially copyable type");

private:
	T* _data = nullptr;
	size_t _size = 0;

	void Release()
	{
		if(_data) {
			VirtualMemory::Free(_data, _size * sizeof(T));
			_data = nullptr;
		}
	}

public:
	ZeroedArray() {}

	explicit ZeroedArray(size_t size)
	{
		if(size > 0) {
			_data = (T*)VirtualMemory::AllocZeroed(size * sizeof(T));
			_size = size;
		}
	}

	~ZeroedArray() { Release(); }

	ZeroedArray(const ZeroedArray&) = delete;
	ZeroedArray& operator=(const ZeroedArray&) = delete;

	ZeroedArray(ZeroedArray&& other) noexcept : _data(other._data), _size(other._size)
	{
		other._data = nullptr;
		other._size = 0;
	}

	ZeroedArray& operator=(ZeroedArray&& other) noexcept
	{
		if(this != &other) {
			Release();
			_data = other._data;
			_size = other._size;
			other._data = nullptr;
			other._size = 0;
		}
		return *this;
	}

	//Zero every entry by handing the pages back to the OS (memset would touch, and so commit, all of them).
	//Moves the data: callers must not hold pointers or references into the array across this call.
	void Clear()
	{
		size_t size = _size;
		Release();
		_size = 0;
		*this = ZeroedArray(size);
	}

	T& operator[](size_t i) { return _data[i]; }
	const T& operator[](size_t i) const { return _data[i]; }
	T* data() { return _data; }
	const T* data() const { return _data; }
	size_t size() const { return _size; }
};
