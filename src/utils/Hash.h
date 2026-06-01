//
// Created by zykov on 5/1/2026.
//

#ifndef MAXENGINE_HASH_H
#define MAXENGINE_HASH_H

#include <string>
#include <cstdint>

class Hash {
public:
    static constexpr uint64_t FNV_OFFSET_BASIS = 0xcbf29ce484222325;
    static constexpr uint64_t FNV_PRIME = 0x100000001b3;

    static uint64_t Generate(const std::string &str) {
        uint64_t hash = FNV_OFFSET_BASIS;

        for (const char c : str) {
            hash ^= static_cast<uint64_t>(c);
            hash *= FNV_PRIME;
        }

        return hash;
    }
};


#endif //MAXENGINE_HASH_H
