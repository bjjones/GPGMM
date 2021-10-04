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

#ifndef GPGMM_MEMORY_ALLOCATOR_STACK_H_
#define GPGMM_MEMORY_ALLOCATOR_STACK_H_

#include "src/MemoryAllocator.h"

#include <vector>

namespace gpgmm {

    // Combines multiple (dependant) memory allocators together at run-time and
    // exposes them as a single memory allocator.
    class MemoryAllocatorStack : public MemoryAllocator {
      public:
        MemoryAllocatorStack() = default;
        ~MemoryAllocatorStack() override = default;

        MemoryAllocator* PushAllocator(std::unique_ptr<MemoryAllocator> allocator);

        std::unique_ptr<MemoryAllocation> AllocateMemory(uint64_t size,
                                                         uint64_t alignment) override;
        void DeallocateMemory(MemoryAllocation* allocation) override;

        void ReleaseMemory() override;

      private:
        std::vector<std::unique_ptr<MemoryAllocator>> mAllocators;
    };

}  // namespace gpgmm

#endif  // GPGMM_MEMORY_ALLOCATOR_STACK_H_