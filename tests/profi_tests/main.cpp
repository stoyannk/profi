#include <gtest/gtest.h>

#include <mutex>
#include <memory>

#include <../src/precompiled.h>
#include <profi.h>
#include <STLAllocator.h>
#include <ProfileScope.h>

using namespace profi;

#ifdef DEBUG
typedef profi::DebugMallocAllocator ProfiAllocator;
#else
typedef profi::DefaultMallocAllocator ProfiAllocator;
#endif

class HashMapTest : public ::testing::Test
{
protected:
	virtual void SetUp()
	{
		profi::Initialize(&allocator);
	}

	virtual void TearDown()
	{
		profi::Deinitialize();
	}

	std::mutex Mutex;
	ProfiAllocator allocator;
};

TEST_F(HashMapTest, CreateMap) {
	HashMap map(Mutex);
}

TEST_F(HashMapTest, Insert) {
	HashMap map(Mutex);

	ProfileScope* scope0 = profi_new(ProfileScope, "scope0", Mutex);
	ProfileScope* scope1 = profi_new(ProfileScope, "scope1", Mutex);

	map.Insert(scope0);
	map.Insert(scope1);

	ASSERT_EQ(map.Get("scope0"), scope0);
	ASSERT_EQ(map.Get("scope1"), scope1);
	ASSERT_EQ(map.Get("missing"), nullptr);

	std::for_each(map.begin(), map.end(), [] (const HashMap::value_type& scope) {
		profi_delete(scope);
	});
}

TEST_F(HashMapTest, Overflow) {
	HashMap map(Mutex);

	const auto sz = 32;

	std::string scopeName = "scope";
	std::vector<std::string> names;	// NB: We work with const char* so we need the addresses of the strings to be
									// fixed in memory
	for(unsigned i = 0; i < sz; ++i) {
		std::ostringstream str;
		str << scopeName << i;
		names.push_back(str.str());
	}

	for(unsigned i = 0; i < sz; ++i) {
		ProfileScope* scope = profi_new(ProfileScope, names[i].c_str(), Mutex);
		map.Insert(scope);
	}
	
	for(unsigned i = 0; i < sz; ++i) {
		auto scope = map.Get(names[i].c_str());
		ASSERT_NE(scope, nullptr);
	}
	
	std::for_each(map.begin(), map.end(), [] (const HashMap::value_type& scope) {
		profi_delete(scope);
	});
}

TEST_F(HashMapTest, Iterate) {
	HashMap map(Mutex);
	const auto sz = 32;
	
	std::vector<ProfileScope*> scopesVec;
	std::string scopeName = "scope";
	std::vector<std::string> names;
	for(unsigned i = 0; i < sz; ++i) {
		std::ostringstream str;
		str << scopeName << i;
		names.push_back(str.str());
	}

	for(unsigned i = 0; i < sz; ++i) {
		ProfileScope* scope = profi_new(ProfileScope, names[i].c_str(), Mutex);
		map.Insert(scope);
		scopesVec.push_back(scope);
	}

	unsigned count = 0;
	std::for_each(map.begin(), map.end(), [&count, &scopesVec](const HashMap::value_type& value) {
		auto scope = std::find(scopesVec.cbegin(), scopesVec.cend(), value);
		ASSERT_NE(scope, scopesVec.cend());
		++count;
	});

	auto it = map.begin();
	++it;
	auto it2 = map.Get(names[2].c_str());

	std::for_each(map.begin(), map.end(), [] (const HashMap::value_type& scope) {
		profi_delete(scope);
	});
}

