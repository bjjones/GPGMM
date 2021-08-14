// Copyright 2020 The Dawn Authors
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

#ifndef GPGMM_POOLEDMEMORYALLOCATOR_H_
#define GPGMM_POOLEDMEMORYALLOCATOR_H_

#include "src/MemoryAllocator.h"

#include <deque>

namespace gpgmm {

    // |PooledMemoryAllocator| allocates a fixed-size resource memory from a resource memory
    // pool. Internally, it manages a list of heaps using LIFO (newest heaps are recycled first).
    // The heap is in one of two states: AVAILABLE or not. Upon de-allocate, the heap is returned
    // the pool and made AVAILABLE.
    class PooledMemoryAllocator : public MemoryAllocator {
      public:
        PooledMemoryAllocator(MemoryAllocator* memoryAllocator);
        ~PooledMemoryAllocator() override = default;

        // MemoryAllocator interface
        void AllocateMemory(MemoryAllocation& allocation) override;
        void DeallocateMemory(MemoryAllocation& allocation) override;
        void Release() override;

        uint64_t GetMemorySize() const override;
        uint64_t GetMemoryAlignment() const override;

        // For testing purposes.
        uint64_t GetPoolSizeForTesting() const;

      private:
        MemoryAllocator* mMemoryAllocator = nullptr;

        std::deque<MemoryAllocation> mPool;
    };

}  // namespace gpgmm

#endif  // GPGMM_POOLEDMEMORYALLOCATOR_H_
