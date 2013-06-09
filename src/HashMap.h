#pragma once

#include <profi_decls.h>
#include <STLAllocator.h>

#ifndef NDEBUG
#define DEBUG_HASHMAP
#endif

namespace profi {

class ProfileScope;

//TODO: Check for perf. improvement if we explicitly save the key in the map instead of derefing it all the time

// Does not support deletion but it's not needed!
class HashMap
{
public:
	typedef ProfileScope* value_type;
	typedef std::vector<value_type, STLAllocator<value_type>> storage_t;

	class iterator : public std::iterator<std::bidirectional_iterator_tag, ProfileScope*>
	{
	public:
		iterator(storage_t& owner, size_t id);
		iterator(const iterator& other);
		
		HashMap::value_type& operator*();

		iterator& operator++();
		iterator operator++(int);

		iterator&  operator--();
		iterator operator--(int);

		bool operator==(const iterator& other) const;
		bool operator!=(const iterator& other) const;

	private:
		size_t m_Id;
		storage_t& m_Storage;
	};

	HashMap(std::mutex& mutex);
	// safe
	HashMap(const HashMap& other);
	// safe - invalidates iterators
	HashMap& operator=(const HashMap& other);

	// unsafe
	ProfileScope* Get(const char* name);
	
	// Takes ownership - safe; invalidates iterators!
	bool Insert(ProfileScope* scope);
	
	// unsafe
	iterator begin();
	// unsafe
	iterator end();
	// unsafe
	size_t size() const { return m_Allocated; }

#ifdef DEBUG_HASHMAP
	unsigned Collisions;
#endif

private:
	void Rehash();
	bool InsertInternal(ProfileScope* scope, storage_t& storage, size_t& counter);
		
	storage_t m_Storage;
	size_t m_Allocated;
	mutable std::mutex& m_Mutex;
};

}
