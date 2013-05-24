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

TEST(HashMapTest, CreateMap) {
	HashMap map;
}

TEST(HashMapTest, Insert) {
	ProfiAllocator allocator;
	profi::Initialize(&allocator);

	HashMap map;

	ProfileScope* scope0 = profi_new(ProfileScope, "scope0");
	ProfileScope* scope1 = profi_new(ProfileScope, "scope1");

	map.Insert(scope0);
	map.Insert(scope1);

	ASSERT_EQ(map.Get("scope0"), scope0);
	ASSERT_EQ(map.Get("scope1"), scope1);
	ASSERT_EQ(map.Get("missing"), nullptr);

	profi::Deinitialize();
}

TEST(HashMapTest, Overflow) {
	ProfiAllocator allocator;
	profi::Initialize(&allocator);

	HashMap map;

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
		ProfileScope* scope = profi_new(ProfileScope, names[i].c_str());
		map.Insert(scope);
	}
	
	for(unsigned i = 0; i < sz; ++i) {
		auto scope = map.Get(names[i].c_str());
		ASSERT_NE(scope, nullptr);
	}
	
	profi::Deinitialize();
}

TEST(HashMapTest, Iterate) {
	ProfiAllocator allocator;
	profi::Initialize(&allocator);

	HashMap map;
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
		ProfileScope* scope = profi_new(ProfileScope, names[i].c_str());
		map.Insert(scope);
		scopesVec.push_back(scope);
	}

	unsigned count = 0;
	std::for_each(map.cbegin(), map.cend(), [&count, &scopesVec](const HashMap::value_type& value) {
		auto scope = std::find(scopesVec.cbegin(), scopesVec.cend(), value.second);
		ASSERT_NE(scope, scopesVec.cend());
		++count;
	});

	auto it = map.cbegin();
	++it;
	auto it2 = map.Get(names[2].c_str());

	profi::Deinitialize();
}

