// Copyright 2021 The GPGMM Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>

#include "gpgmm/common/PooledMemoryAllocator.h"
#include "tests/DummyMemoryAllocator.h"

using namespace gpgmm;

static constexpr uint64_t kDefaultMemorySize = 128u;
static constexpr uint64_t kDefaultMemoryAlignment = 1u;

class PooledMemoryAllocatorTests : public testing::Test {
  public:
    MEMORY_ALLOCATION_REQUEST CreateBasicRequest(uint64_t size,
                                                 uint64_t alignment,
                                                 bool neverAllocate = false) {
        MEMORY_ALLOCATION_REQUEST request = {};
        request.SizeInBytes = size;
        request.Alignment = alignment;
        request.NeverAllocate = neverAllocate;
        request.AlwaysCacheSize = false;
        request.AlwaysPrefetch = false;
        request.AvailableForAllocation = kInvalidSize;
        return request;
    }
};

TEST_F(PooledMemoryAllocatorTests, SingleHeap) {
    PooledMemoryAllocator allocator(kDefaultMemorySize, std::make_unique<DummyMemoryAllocator>());

    std::unique_ptr<MemoryAllocation> invalidAllocation =
        allocator.TryAllocateMemory(CreateBasicRequest(0, kDefaultMemoryAlignment));
    ASSERT_EQ(invalidAllocation, nullptr);

    std::unique_ptr<MemoryAllocation> allocation = allocator.TryAllocateMemory(
        CreateBasicRequest(kDefaultMemorySize, kDefaultMemoryAlignment));
    ASSERT_NE(allocation, nullptr);
    EXPECT_EQ(allocation->GetSize(), kDefaultMemorySize);
    EXPECT_EQ(allocation->GetMethod(), AllocationMethod::kStandalone);
    EXPECT_EQ(allocator.GetInfo().UsedMemoryCount, 1u);

    allocator.DeallocateMemory(std::move(allocation));
    EXPECT_EQ(allocator.GetInfo().UsedMemoryCount, 0u);

    EXPECT_EQ(allocator.ReleaseMemory(), kDefaultMemorySize);
    EXPECT_EQ(allocator.GetInfo().FreeMemoryUsage, 0u);
}

TEST_F(PooledMemoryAllocatorTests, MultipleHeaps) {
    PooledMemoryAllocator allocator(kDefaultMemorySize, std::make_unique<DummyMemoryAllocator>());

    std::unique_ptr<MemoryAllocation> firstAllocation = allocator.TryAllocateMemory(
        CreateBasicRequest(kDefaultMemorySize, kDefaultMemoryAlignment));
    ASSERT_NE(firstAllocation, nullptr);
    EXPECT_EQ(firstAllocation->GetSize(), kDefaultMemorySize);

    std::unique_ptr<MemoryAllocation> secondAllocation = allocator.TryAllocateMemory(
        CreateBasicRequest(kDefaultMemorySize, kDefaultMemoryAlignment));
    ASSERT_NE(secondAllocation, nullptr);
    EXPECT_EQ(secondAllocation->GetSize(), kDefaultMemorySize);

    EXPECT_EQ(allocator.GetInfo().UsedMemoryCount, 2u);

    allocator.DeallocateMemory(std::move(firstAllocation));
    allocator.DeallocateMemory(std::move(secondAllocation));

    EXPECT_EQ(allocator.ReleaseMemory(kDefaultMemorySize), kDefaultMemorySize);
    EXPECT_EQ(allocator.ReleaseMemory(kDefaultMemorySize), kDefaultMemorySize);
    EXPECT_EQ(allocator.ReleaseMemory(kDefaultMemorySize), 0u);

    EXPECT_EQ(allocator.GetInfo().UsedMemoryCount, 0u);
    EXPECT_EQ(allocator.GetInfo().FreeMemoryUsage, 0u);
}

TEST_F(PooledMemoryAllocatorTests, ReuseFreedHeaps) {
    PooledMemoryAllocator allocator(kDefaultMemorySize, std::make_unique<DummyMemoryAllocator>());
    {
        std::unique_ptr<MemoryAllocation> allocation = allocator.TryAllocateMemory(
            CreateBasicRequest(kDefaultMemorySize, kDefaultMemoryAlignment));
        ASSERT_NE(allocation, nullptr);
        EXPECT_EQ(allocation->GetSize(), kDefaultMemorySize);
        EXPECT_EQ(allocation->GetMethod(), AllocationMethod::kStandalone);
        allocator.DeallocateMemory(std::move(allocation));
    }

    EXPECT_EQ(allocator.GetInfo().FreeMemoryUsage, kDefaultMemorySize);

    {
        std::unique_ptr<MemoryAllocation> allocation = allocator.TryAllocateMemory(
            CreateBasicRequest(kDefaultMemorySize, kDefaultMemoryAlignment));
        ASSERT_NE(allocation, nullptr);
        EXPECT_EQ(allocation->GetSize(), kDefaultMemorySize);
        EXPECT_EQ(allocation->GetMethod(), AllocationMethod::kStandalone);
        allocator.DeallocateMemory(std::move(allocation));
    }

    EXPECT_EQ(allocator.GetInfo().FreeMemoryUsage, kDefaultMemorySize);
}

TEST_F(PooledMemoryAllocatorTests, GetInfo) {
    PooledMemoryAllocator allocator(kDefaultMemorySize, std::make_unique<DummyMemoryAllocator>());

    std::unique_ptr<MemoryAllocation> allocation = allocator.TryAllocateMemory(
        CreateBasicRequest(kDefaultMemorySize, kDefaultMemoryAlignment));
    EXPECT_NE(allocation, nullptr);

    // Single memory block should be allocated.
    EXPECT_EQ(allocator.GetInfo().UsedBlockCount, 0u);
    EXPECT_EQ(allocator.GetInfo().UsedBlockUsage, 0u);
    EXPECT_EQ(allocator.GetInfo().UsedMemoryCount, 1u);
    EXPECT_EQ(allocator.GetInfo().UsedMemoryUsage, kDefaultMemorySize);
    EXPECT_EQ(allocator.GetInfo().FreeMemoryUsage, 0u);

    allocator.DeallocateMemory(std::move(allocation));

    // Single memory is made available as free after being released.
    EXPECT_EQ(allocator.GetInfo().UsedBlockCount, 0u);
    EXPECT_EQ(allocator.GetInfo().UsedBlockUsage, 0u);
    EXPECT_EQ(allocator.GetInfo().UsedMemoryCount, 0u);
    EXPECT_EQ(allocator.GetInfo().UsedMemoryUsage, 0u);
    EXPECT_EQ(allocator.GetInfo().FreeMemoryUsage, kDefaultMemorySize);
}
