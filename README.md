# GPGMM

GPGMM is a General-Purpose GPU Memory Management library.

## Build and Run

### Install `depot_tools`

GPGMM uses the Chromium build system and dependency management so you need to [install depot_tools] and add it to the PATH.

[install depot_tools]: http://commondatastorage.googleapis.com/chrome-infra-docs/flat/depot_tools/docs/html/depot_tools_tutorial.html#_setting_up

**Notes**:
 * On Windows, you'll need to set the environment variable `DEPOT_TOOLS_WIN_TOOLCHAIN=0`. This tells depot_tools to use your locally installed version of Visual Studio (by default, depot_tools will try to download a Google-internal version).

### Get the code

Get the source code as follows:

```sh
# Clone the repo as "GPGMM"
> git clone https://github.com/intel/GPGMM.git GPGMM && cd GPGMM

# Bootstrap the gclient configuration
> cp scripts/standalone.gclient .gclient

# Fetch external dependencies and toolchains with gclient
> gclient sync
```

### Setting up the build

### Build

### Run examples

## How do I use it?

To allocate, you create an allocator then create allocations from it:
```cpp
D3D12_FEATURE_DATA_ARCHITECTURE arch = {};
device->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE, &arch, sizeof(arch)

D3D12_FEATURE_DATA_D3D12_OPTIONS options = {};
device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(options)));

gpgmm::d3d12::ALLOCATOR_DESC allocatorDesc = {};
allocatorDesc.Adapter = adapter;
allocatorDesc.Device = device;
allocatorDesc.IsUMA =  arch.UMA;
allocatorDesc.ResourceHeapTier = options.ResourceHeapTier;

gpgmm::d3d12::ResourceAllocator allocator(desc);
```

```cpp
/* Fill this stuff out */
D3D12_RESOURCE_DESC& resourceDescriptor = {...};
D3D12_RESOURCE_STATES initialState = {...}
D3D12_CLEAR_VALUE zero{};

gpgmm::d3d12::ALLOCATION_DESC allocationDesc = {};
desc.HeapType = heapType;

ComPtr<gpgmm::d3d12::ResourceAllocation> allocation;
allocator->CreateResource(allocationDesc, resourceDescriptor, initialState, &zero, &allocation);
```

Then de-allocate:
```cpp
/* Must make sure GPU is finished using it */
allocation.Release();
```

To use residency:
1. Create a `d3d12::ResourceAllocator` with `ALLOCATOR_ALWAYS_IN_BUDGET` flag.
2. Use `d3d12::ResourceAllocator::CreateResource` for every resource you want residency managed.
3. Create a `d3d12::ResidencySet` to track a collection of allocations that should be resident for a given command-list (1:1 relationship).
4. `d3d12::ResourceAllocation::UpdateResidency` tracks the underlying heap for the resident set.
5. Use `d3d12::ResidencyManager::ExecuteCommandLists` with the residency set, queue, and command list.

What about residency for other heaps (SV descriptor or query heaps)?
1. Sub-class `d3d12::Heap`.
2. Call `d3d12::ResidencyManager::TrackResidentHeap` on it after creation.
3. Use `d3d12::ResidencyManager::[Lock|UnlockHeap]` to keep heap resident or not (ex. SV heaps).

# Prerequisites
* Error handing uses API error codes (`HRESULT` and `VkResult` for D3D12 and Vulkan, respectively).
* `d3d12::ResourceAllocation` is a ref-counted object.

# API Integration Testing

You can test local GPGMM changes through existing end2end tests.

Modify `.gclient`
```json
"custom_vars": {
    "checkout_<Project>": True,
},
```

```sh
> gclient sync
> ninja -C out/Debug
> out/Debug/<Test>
```

| Project     | Backend                   | Test               |
|-------------|---------------------------|--------------------|
| Dawn        | D3D12: yes, Vulkan: TBD   | dawn_end2end_tests |
| Skia        | D3D12: TODO, Vulkan: TBD  |                    |
| WebNN       | DML: TODO                 | webnn_end2end_tests|

## License

Apache 2.0 Public License, please see [LICENSE](/LICENSE).