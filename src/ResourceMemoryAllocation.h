// Copyright 2018 The Dawn Authors
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

#ifndef GPGMM_RESOURCEMEMORYALLOCATION_H_
#define GPGMM_RESOURCEMEMORYALLOCATION_H_

#include <cstdint>

#define GPGMM_INVALID_ALLOCATION \
    ResourceMemoryAllocation {   \
    }

namespace gpgmm {

    class ResourceMemoryBase;
    class ResourceAllocatorBase;

    // Allocation method determines how memory was sub-divided.
    // Used by the device to get the allocator that was responsible for the allocation.
    enum class AllocationMethod {

        // Memory not sub-divided.
        kDirect,

        // Memory sub-divided using one or more blocks of various sizes.
        kSubAllocated,

        // Memory not allocated or freed.
        kInvalid
    };

    // Metadata that describes how the allocation was allocated.
    struct AllocationInfo {
        // AllocationInfo contains a separate offset to not confuse block vs memory offsets.
        // The block offset is within the entire allocator memory range and only required by the
        // buddy sub-allocator to get the corresponding memory. Unlike the block offset, the
        // allocation offset is always local to the memory.
        uint64_t mBlockOffset = 0;

        AllocationMethod mMethod = AllocationMethod::kInvalid;
    };

    // Handle into a resource heap pool.
    class ResourceMemoryAllocation {
      public:
        ResourceMemoryAllocation();
        ResourceMemoryAllocation(ResourceAllocatorBase* allocator,
                                 const AllocationInfo& info,
                                 uint64_t offset,
                                 ResourceMemoryBase* resourceMemory,
                                 uint8_t* mappedPointer = nullptr);
        virtual ~ResourceMemoryAllocation() = default;

        ResourceMemoryAllocation(const ResourceMemoryAllocation&) = default;
        ResourceMemoryAllocation& operator=(const ResourceMemoryAllocation&) = default;
        bool operator==(const ResourceMemoryAllocation&);

        ResourceMemoryBase* GetResourceMemory() const;
        uint64_t GetOffset() const;
        uint8_t* GetMappedPointer() const;
        AllocationInfo GetInfo() const;
        ResourceAllocatorBase* GetAllocator();

        virtual void Invalidate();

      private:
        ResourceAllocatorBase* mAllocator;
        AllocationInfo mInfo;
        uint64_t mOffset;
        ResourceMemoryBase* mResourceMemory;
        uint8_t* mMappedPointer;
    };
}  // namespace gpgmm

#endif  // GPGMM_RESOURCEMEMORYALLOCATION_H_