#pragma once

#include "common/common_header.hpp"

namespace drop::utils
{
    using Size = u64;

    struct BumpAllocator
    {
        Size  capacity {0};
        Size  used {0};
        char* memory {nullptr};
    };

    BumpAllocator MakeBumpAllocator(Size size);
    char*         BumpAlloc(BumpAllocator* ba, Size size);
} // namespace drop::utils
