﻿#pragma once

#include <profi_decls.h>
#include <STLAllocator.h>

namespace profi {

class ProfileScope;

//TODO: Remove mutex per-scope; could be done with just one per thread
//TODO: Check for perf. improvement if we explicitly save the key in the map instead of derefing it all the time

// Does not support deletion but it's not needed!
class HashMap
{
public:
	typedef ProfileScope* value_type;
	typedef std::vector<value_type, STLAllocator<value_type>> storage_t;

	class const_iterator : public std::iterator<std::bidirectional_iterator_tag, ProfileScope*>
	{
	public:
		const_iterator(const storage_t& owner, size_t id);
		const_iterator(const const_iterator& other);
		
		const HashMap::value_type& operator*() const;

		const_iterator& operator++();
		const_iterator operator++(int);

		const_iterator&  operator--();
		const_iterator operator--(int);

		bool operator==(const const_iterator& other) const;
		bool operator!=(const const_iterator& other) const;

	private:
		size_t m_Id;
		const storage_t& m_Storage;
	};

	HashMap();
	// safe
	HashMap(const HashMap& other);
	// safe - invalidates iterators
	HashMap& operator=(const HashMap& other);

	// unsafe
	ProfileScope* Get(const char* name);
	
	// Takes ownership - safe; invalidates iterators!
	bool Insert(ProfileScope* scope);
	
	// unsafe
	const_iterator cbegin() const;
	// unsafe
	const_iterator cend() const;
	// unsafe
	size_t size() const { return m_Allocated; }

private:
	void Rehash();
	static bool InsertInternal(ProfileScope* scope, storage_t& storage, size_t& counter);
		
	storage_t m_Storage;
	size_t m_Allocated;
	mutable std::mutex m_Mutex;
};

}
