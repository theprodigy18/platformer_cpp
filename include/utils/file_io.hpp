#pragma once

#include "common/common_header.hpp"
#include "bump_allocator.hpp"

namespace drop::utils
{
    i64   GetTimestamp(char* file);
    bool  FileExists(char* filePath);
    i32   GetFileSize(char* filePath);
    char* ReadFile(char* filePath, char* buffer, i32* outSize);
    char* ReadFile(char* filePath, BumpAllocator* ba, i32* outSize);
    void  WriteFile(char* filePath, char* buffer, i32 size);
    bool  CopyingFile(char* fileName, char* destName, char* buffer);
    bool  CopyingFile(char* fileName, char* destName, BumpAllocator* ba);
} // namespace drop::utils
