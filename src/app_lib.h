#pragma once

//Includes
#include <stdio.h>
#include <stdlib.h> //For malloc
#include <string.h> //For memset
#include <sys/stat.h> //For timestamps

//defines
#ifdef _WIN32
#define DEBUG_BREAK() __debugbreak()
#elif __linix__
#define DEBUG_BREAK() __builtin_debugtrap()
#elif __APPLE__
#define DEBUG_BREAK() __builtin_trap()
#endif

//logging
enum TextColor {
    TEXT_COLOR_BLACK,
    TEXT_COLOR_RED,
    TEXT_COLOR_GREEN,
    TEXT_COLOR_YELLOW,
    TEXT_COLOR_BLUE,
    TEXT_COLOR_MAGENTA,
    TEXT_COLOR_CYAN,
    TEXT_COLOR_WHITE,
    TEXT_COLOR_BRIGHT_BLACK,
    TEXT_COLOR_BRIGHT_RED,
    TEXT_COLOR_BRIGHT_GREEN,
    TEXT_COLOR_BRIGHT_YELLOW,
    TEXT_COLOR_BRIGHT_BLUE,
    TEXT_COLOR_BRIGHT_MAGENTA,
    TEXT_COLOR_BRIGHT_CYAN,
    TEXT_COLOR_BRIGHT_WHITE,
    TEXT_COLOR_COUNT
};

template <typename ...Args>
void _log(char* prefix, char* msg, TextColor textColor, Args...args) {
    static char* TextColorTable[TEXT_COLOR_COUNT] = {
        "\x1b[30m",
        "\x1b[31m",
        "\x1b[32m",
        "\x1b[33m",
        "\x1b[34m",
        "\x1b[35m",
        "\x1b[36m",
        "\x1b[37m",
        "\x1b[90m",
        "\x1b[91m",
        "\x1b[92m",
        "\x1b[93m",
        "\x1b[94m",
        "\x1b[95m",
        "\x1b[96m",
        "\x1b[97m"
    };

    char formatBuffer[8192] = {};
    sprintf(formatBuffer,"%s %s %s \033[0m", TextColorTable[textColor], prefix, msg);

    char textBuffer[8192] = {};
    sprintf(textBuffer, formatBuffer, args...);

    puts(textBuffer);
}

#define Trace(msg, ...) _log("Trace: ", msg, TEXT_COLOR_BRIGHT_GREEN, ##__VA_ARGS__);
#define Warn(msg, ...) _log("Warn: ", msg, TEXT_COLOR_BRIGHT_YELLOW, ##__VA_ARGS__);
#define Error(msg, ...) _log("Error: ", msg, TEXT_COLOR_BRIGHT_RED, ##__VA_ARGS__);

#define Assert(x, msg, ...) {     \
    if(!x){                          \
        Error(msg, ##__VA_ARGS__);\
        DEBUG_BREAK();               \
    }                                \
}

//Bump Allocator
struct BumpAllocator {
    size_t capacity;
    size_t used;
    char* memory;
};

BumpAllocator make_bump_allocator(size_t size) {
    BumpAllocator ba = {};
   
    ba.memory = (char*)malloc(size);
    if (!ba.memory) {
        Assert(false, "Memory allocation failed!");
    }

    ba.capacity = size;
    memset(ba.memory, 0, size);

    return ba;
}

char* bump_alloc(BumpAllocator* bumpAllocator, size_t size) {
    char* result = nullptr;

    size_t allignedSize = (size + 7) & ~7;
    if (bumpAllocator->used + allignedSize>bumpAllocator->capacity) {
        Assert(false, "BumpAllocator is full!");
    }
    result = bumpAllocator->memory+bumpAllocator->used;
    bumpAllocator->used += allignedSize;

    return result;
}

//File In/Out
long long get_timestamp(char* file) {
    struct stat file_stat = {};
    stat(file, &file_stat);
    return file_stat.st_mtime;
}

bool file_exists(char* filePath) {
    Assert(filePath, "No filepath supplied!");

    auto file = fopen(filePath, "rb");
    if(!file) {
        return false;
    }
    fclose(file);

    return true;
}

long get_file_size(char* filePath) {
    Assert(filePath, "No filepath supplied!");

    long fileSize = 0;
    auto file = fopen(filePath, "rb");
    if(!file) {
        Error("Failed opening file: %s", filePath);
        return 0;
    }

    fseek(file,0,SEEK_END);
    fileSize=ftell(file);
    fseek(file,0,SEEK_SET);
    fclose(file);

    return fileSize;
}

char* read_file(char* filePath, int* fileSize, char* buffer) {
    Assert(filePath, "No filepath supplied!");
    Assert(fileSize, "No filesize supplied!");
    Assert(buffer, "No buffer supplied!");

    *fileSize = 0;
    auto file = fopen(filePath, "rb");
    if(!file) {
        Error("Failed opening file: %s", filePath);
        return nullptr;
    }

    fseek(file,0,SEEK_END);
    *fileSize = ftell(file);
    fseek(file,0,SEEK_SET);

    memset(buffer,0,*fileSize+1);
    fread(buffer,sizeof(char),*fileSize,file);

    fclose(file);

    return buffer;
}

char* read_file(char* filePath, int* fileSize, BumpAllocator* bumpAllocator) {
    char* file = 0;
    long fileSize2 = get_file_size(filePath);
    if(!fileSize2) {
        return file;
    }

    char* buffer = bump_alloc(bumpAllocator,fileSize2+1);
    file = read_file(filePath,fileSize,buffer);

    return file;
}

void write_file(char* filePath, char* buffer, int size) {
    Assert(filePath, "No filepath supplied!");
    Assert(buffer, "No buffer supplied!");
    auto file = fopen(filePath, "wb");
    if(!file) {
        Error("Failed opening file %s", filePath);
        return;
    }
    fwrite(buffer, sizeof(char), size, file);
    return;
}

