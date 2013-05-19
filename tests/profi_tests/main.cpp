#include <gtest/gtest.h>

#include <mutex>
#include <memory>

#include <../src/precompiled.h>
#include <profi.h>
#include <STLAllocator.h>
#include <ProfileScope.h>

using namespace profi;

TEST(HashMapTest, CreateMap) {
	HashMap map;
}

TEST(HashMapTest, Insert) {
	profi::DefaultAllocator allocator;
	profi::Initialize(&allocator);

	HashMap map;

	ProfileScope* scope0 = profi_new(ProfileScope, "scope0");
	ProfileScope* scope1 = profi_new(ProfileScope, "scope1");

	map.Insert(scope0);
	map.Insert(scope1);

	ASSERT_EQ(map.Get("scope0"), scope0);
	ASSERT_EQ(map.Get("scope1"), scope1);
	ASSERT_EQ(map.Get("missing"), nullptr);
}
