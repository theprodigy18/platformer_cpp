#include "utils/file_io.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/stat.h>

namespace drop::utils
{
    i64 GetTimestamp(char* file)
    {
        struct stat fileStat {};
        stat(file, &fileStat);
        return fileStat.st_mtime;
    }

    bool FileExists(char* filePath)
    {
        D_ASSERT(filePath, "File path is null.");

        std::ifstream file(filePath);
        return file.good();
    }

    i32 GetFileSize(char* filePath)
    {
        D_ASSERT(filePath, "File path is null.");

        std::ifstream file(filePath, std::ios::binary | std::ios::ate);
        if (!file)
        {
            D_ERROR("Failed to open file: %s", filePath);
            return 0;
        }

        return file.tellg();
    }

    // Reads a file into a buffer.
    char* ReadFile(char* filePath, char* buffer, i32* outSize)
    {
        D_ASSERT(filePath, "File path is null.");
        D_ASSERT(outSize, "Size pointer is null.");
        D_ASSERT(buffer, "Buffer is null.");

        *outSize = 0;
        std::ifstream file(filePath, std::ios::binary);
        if (!file)
        {
            D_ERROR("Failed to open file: %s", filePath);
            return nullptr;
        }

        file.seekg(0, std::ios::end);
        *outSize = file.tellg();
        file.seekg(0, std::ios::beg);

        file.read(buffer, *outSize);
        buffer[*outSize] = '\0'; // Add null terminator.

        return buffer;
    }

    char* ReadFile(char* filePath, BumpAllocator* ba, i32* outSize)
    {
        char* file {nullptr};
        i32   fileSize {GetFileSize(filePath)};

        if (fileSize)
        {
            char* buffer {BumpAlloc(ba, fileSize + 1)};
            file = ReadFile(filePath, buffer, outSize);
        }

        return file;
    }

    void WriteFile(char* filePath, char* buffer, i32 size)
    {
        D_ASSERT(filePath, "File path is null.");
        D_ASSERT(buffer, "Buffer is null.");

        std::ofstream file(filePath, std::ios::binary);
        if (!file)
        {
            D_ERROR("Failed to open file: %s", filePath);
            return;
        }

        file.write(buffer, size);
    }

    bool CopyingFile(char* fileName, char* destName, char* buffer)
    {
        i32   fileSize {0};
        char* data {ReadFile(fileName, buffer, &fileSize)};

        std::ofstream file(destName, std::ios::binary);
        if (!file)
        {
            D_ERROR("Failed to open file: %s", destName);
            return false;
        }

        file.write(data, fileSize);
        return true;
    }

    bool CopyingFile(char* fileName, char* destName, BumpAllocator* ba)
    {
        i32 fileSize {GetFileSize(fileName)};

        if (fileSize)
        {
            char* buffer {BumpAlloc(ba, fileSize + 1)};
            return CopyingFile(fileName, destName, buffer);
        }

        return false;
    }

} // namespace drop::utils