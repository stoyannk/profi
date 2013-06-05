#pragma once

#include <GlobalAllocator.h>

#define profi_new(TYPE, ...) new (profi::GetGlobalAllocator()->Allocate(sizeof(TYPE))) TYPE(__VA_ARGS__)

namespace profi {

template<typename T>
void profi_delete(T* ptr)
{
	ptr->~T();
	GetGlobalAllocator()->Deallocate(ptr);
}

template <typename T>
class STLAllocator
{
public:
	typedef T value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef T* pointer;
	typedef const T* const_pointer;

	typedef T& reference;
	typedef const T& const_reference;

	pointer address(reference x) const
	{
		return &x;
	}

	const_pointer address(const_reference x) const
	{
		return &x;
	}

	STLAllocator()
	{}

	STLAllocator(const STLAllocator&)
	{}

	template <typename U>
	STLAllocator(const U&)
	{}

	template <typename U>
	STLAllocator(const STLAllocator<U>&)
	{}

	void construct(pointer p, const_reference value)
	{
		new (p) T(value);
	}

#ifdef __clang__
	template <typename U, typename... Args>
	void construct(U* p, Args&&... args)
	{
		new (p) U(std::forward<Args>(args)...);
	}
#else
	void construct(pointer p, value_type&& value)
	{
		new (p) value_type(std::forward<value_type>(value));
	}

	template <typename Other>
	void construct(pointer p, Other&& value)
	{
		new (p) value_type(std::forward<Other>(value));
	}
#endif

	void destroy(pointer p)
	{
		p->~T();
	}

	size_type max_size() const
	{
		return size_type(-1) / sizeof(T);
	}

	template <typename U>
	struct rebind
	{
		typedef STLAllocator<U> other;
	};
	
	pointer allocate(size_type n, const void* = 0)
	{
		return static_cast<pointer>(GetGlobalAllocator()->Allocate(n * sizeof(value_type)));
	}

	void deallocate(pointer p, size_type)
	{
		GetGlobalAllocator()->Deallocate(p);
	}

	bool equal(const STLAllocator& rhs) const
	{
		return true;
	}
};

template <>
class STLAllocator<void>
{
public:
	typedef void value_type;
	typedef void* pointer;
	typedef const void* const_pointer;

	template <typename T>
	struct rebind
	{
		typedef STLAllocator<T> other;
	};
};

template <typename T>
bool operator==(const STLAllocator<T>& lhs, const STLAllocator<T>& rhs)
{
	return lhs.equal(rhs);
}

template <typename T>
bool operator!=(const STLAllocator<T>& lhs, const STLAllocator<T>& rhs)
{
	return !(lhs == rhs);
}

template<typename T>
struct profi_deleter 
{
	void operator()(T* ptr) {
		ptr->~T();
		GetGlobalAllocator()->Deallocate(ptr);
	}
};

typedef std::basic_string<char, std::char_traits<char>, STLAllocator<char>> pstring;
typedef std::basic_ostringstream<char, std::char_traits<char>, STLAllocator<char>> opstringstream;

}