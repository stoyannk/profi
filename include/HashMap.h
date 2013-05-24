#pragma once

#include <profi_decls.h>

namespace profi {

class ProfileScope;

class HashMap
{
public:
	typedef ProfileScope* value_type;

	class const_iterator
	{
	public:
		const_iterator(HashMap& owner, size_t id);
		const_iterator(const const_iterator& other);
		const_iterator& operator=(const const_iterator& other);

		const HashMap::value_type& operator*() const;

		void operator++();
		const HashMap::value_type& operator++(int);

		void operator--();
		const HashMap::value_type& operator--(int);

	private:
		size_t m_Id;
		HashMap& m_Owner;
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
	typedef std::vector<ProfileScope*> storage_t;
	
	storage_t m_Storage;
	size_t m_Allocated;
	mutable std::mutex m_Mutex;
};

}