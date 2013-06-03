#include "precompiled.h"

#include <HashMap.h>
#include <ProfileScope.h>

namespace profi
{

size_t hash_func(const char* key)
{
	return std::hash<const char*>()(key);
}

HashMap::HashMap(std::mutex& mutex)
	: m_Allocated(0)
	, m_Storage(4)
	, m_Mutex(mutex)
#ifdef DEBUG_HASHMAP
	, Collisions(0)
#endif
{}

HashMap::HashMap(const HashMap& other)
	: m_Mutex(other.m_Mutex)
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	m_Allocated = other.m_Allocated;
	m_Storage = other.m_Storage;
}

HashMap& HashMap::operator=(const HashMap& other)
{
	if(this != &other)
	{
		assert(&m_Mutex == &other.m_Mutex); // only maps generated from the same thread can be copied
		std::lock_guard<std::mutex> lock(m_Mutex);
		m_Allocated = other.m_Allocated;
		m_Storage = other.m_Storage;
	}

	return *this;
}

ProfileScope* HashMap::Get(const char* name)
{
	const auto hash = hash_func(name);
	const auto storageSz = m_Storage.size();
	const auto bucket = hash % storageSz;
	auto current_bucket = bucket;

	do
	{
		if(!m_Storage[current_bucket]) {
			return nullptr;
		}

		if((size_t)m_Storage[current_bucket]->GetName() == (size_t)name) {
			return m_Storage[current_bucket];
		}

		current_bucket = ++current_bucket % storageSz;
	} while(bucket != current_bucket);
	
	return nullptr;
}

bool HashMap::Insert(ProfileScope* scope)
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	static const float REHASH_LOAD = 0.75f;
	if(m_Allocated / (float)m_Storage.size() >= REHASH_LOAD) {
		Rehash();
	}
	
	return InsertInternal(scope, m_Storage, m_Allocated);
}

void HashMap::Rehash()
{
	storage_t newStorage(m_Storage.size() * 2);
	unsigned newCounter = 0;
	std::for_each(m_Storage.cbegin(), m_Storage.cend(), [&newStorage, &newCounter, this](ProfileScope* scope) {
		if(scope)
			InsertInternal(scope, newStorage, newCounter);
	});

	assert(newCounter == m_Allocated);

	std::swap(m_Storage, newStorage);
}

bool HashMap::InsertInternal(ProfileScope* scope, storage_t& storage, size_t& counter)
{
	const auto name = scope->GetName();
	const auto hash = hash_func(name);
	const auto storageSz = storage.size();
	const auto bucket = hash % storageSz;
	auto current_bucket = bucket;

	do
	{
		if(!storage[current_bucket]) {
			storage[current_bucket] = scope;
			++counter;
			return true;
		}

		if((size_t)storage[current_bucket]->GetName() == (size_t)name) {
			return false;
		}
		#ifdef DEBUG_HASHMAP
		++Collisions;
		#endif
		current_bucket = ++current_bucket % storageSz;
	} while(bucket != current_bucket);

	assert(false && "Hash map internal error"); // we should never come here

	return false;
}

HashMap::iterator HashMap::begin()
{
	return HashMap::iterator(m_Storage, 0u);
}

HashMap::iterator HashMap::end()
{
	return HashMap::iterator(m_Storage, m_Storage.size());
}

///////////////////////////////

HashMap::iterator::iterator(storage_t& owner, size_t id)
	: m_Id(id)
	, m_Storage(owner)
{
	// navigate to the first non-empty storage
	while(m_Id < m_Storage.size() && m_Storage[m_Id] == nullptr)
		++m_Id;
}

HashMap::iterator::iterator(const iterator& other)
	: m_Id(other.m_Id)
	, m_Storage(other.m_Storage)
{}

HashMap::value_type& HashMap::iterator::operator*()
{
	return m_Storage[m_Id];
}

HashMap::iterator& HashMap::iterator::operator++()
{
	do {
		++m_Id;
	} while(m_Id < m_Storage.size() && m_Storage[m_Id] == nullptr);

	return *this;
}

HashMap::iterator HashMap::iterator::operator++(int)
{
	const auto idNow = m_Id;
	
	operator++();

	return iterator(m_Storage, idNow);
}

HashMap::iterator& HashMap::iterator::operator--()
{
	do {
		--m_Id;
	} while(m_Id > 0 && m_Storage[m_Id] == nullptr);

	return *this;
}

HashMap::iterator HashMap::iterator::operator--(int)
{
	const auto idNow = m_Id;
	
	operator--();

	return iterator(m_Storage, idNow);
}

bool HashMap::iterator::operator==(const iterator& other) const
{
	return m_Id == other.m_Id && (&m_Storage == &other.m_Storage);
}

bool HashMap::iterator::operator!=(const iterator& other) const
{
	return !(*this == other);
}

}