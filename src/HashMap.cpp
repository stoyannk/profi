#include "precompiled.h"

#include <HashMap.h>
#include <ProfileScope.h>

namespace profi
{

//TODO: This is unsuitable for the DLL build
size_t hash_func(const char* key)
{
	return (size_t)key << 5;
}

HashMap::HashMap(std::mutex& mutex)
	: m_Allocated(0)
	, m_Storage(4)
	, m_Mutex(mutex)
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
	std::for_each(m_Storage.cbegin(), m_Storage.cend(), [&newStorage, &newCounter](ProfileScope* scope) {
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

		current_bucket = ++current_bucket % storageSz;
	} while(bucket != current_bucket);

	assert(false && "Hash map internal error"); // we should never come here

	return false;
}

HashMap::const_iterator HashMap::cbegin() const
{
	return HashMap::const_iterator(m_Storage, 0u);
}

HashMap::const_iterator HashMap::cend() const
{
	return HashMap::const_iterator(m_Storage, m_Storage.size());
}

///////////////////////////////

HashMap::const_iterator::const_iterator(const storage_t& owner, size_t id)
	: m_Id(id)
	, m_Storage(owner)
{
	// navigate to the first non-empty storage
	while(m_Id < m_Storage.size() && m_Storage[m_Id] == nullptr)
		++m_Id;
}

HashMap::const_iterator::const_iterator(const const_iterator& other)
	: m_Id(other.m_Id)
	, m_Storage(other.m_Storage)
{}

const HashMap::value_type& HashMap::const_iterator::operator*() const
{
	return m_Storage[m_Id];
}

HashMap::const_iterator& HashMap::const_iterator::operator++()
{
	do {
		++m_Id;
	} while(m_Id < m_Storage.size() && m_Storage[m_Id] == nullptr);

	return *this;
}

HashMap::const_iterator HashMap::const_iterator::operator++(int)
{
	const auto idNow = m_Id;
	
	operator++();

	return const_iterator(m_Storage, idNow);
}

HashMap::const_iterator& HashMap::const_iterator::operator--()
{
	do {
		--m_Id;
	} while(m_Id > 0 && m_Storage[m_Id] == nullptr);

	return *this;
}

HashMap::const_iterator HashMap::const_iterator::operator--(int)
{
	const auto idNow = m_Id;
	
	operator--();

	return const_iterator(m_Storage, idNow);
}

bool HashMap::const_iterator::operator==(const const_iterator& other) const
{
	return m_Id == other.m_Id && (&m_Storage == &other.m_Storage);
}

bool HashMap::const_iterator::operator!=(const const_iterator& other) const
{
	return !(*this == other);
}

}