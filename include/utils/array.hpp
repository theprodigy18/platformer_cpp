#pragma once

#include "common/common_header.hpp"

namespace drop::utils
{
    template <typename T, u32 N>
    struct Array
    {
        static constexpr u32 maxElements {N};
        u32                  count {0};
        T                    elements[N];

        T& operator[](i32 idx)
        {
            D_ASSERT(idx >= 0 && idx < count, "Index out of range.");
            return elements[idx];
        }

        i32 Add(T element)
        {
            D_ASSERT(count < maxElements, "Array is full.");
            elements[count] = element;
            return count++;
        }

        void RemoveIndexAndSwap(i32 idx)
        {
            D_ASSERT(idx >= 0 && idx < count, "Index out of range.");
            elements[idx] = elements[--count];
        }

        void Clear()
        {
            count = 0;
        }

        bool IsFull()
        {
            return count == maxElements;
        }
    };

} // namespace drop::utils
