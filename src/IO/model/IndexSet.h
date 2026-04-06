//
// Created by zykov on 4/4/2026.
//
#pragma once

struct IndexSet {
    int PositionIndex;
    int NormalIndex;
    int TextureIndex;

    bool operator!=(const IndexSet& indexSet) const {
        return !(*this == indexSet);
    }

    bool operator==(const IndexSet& other) const {
        return PositionIndex == other.PositionIndex && NormalIndex == other.NormalIndex && TextureIndex == other.TextureIndex;
    }
};

template<>
struct std::hash<IndexSet> {
    constexpr size_t operator() (const IndexSet& indexSet) const noexcept {
        return std::hash<int>()(indexSet.PositionIndex) ^ std::hash<int>()(indexSet.NormalIndex << 14) ^ std::hash<int>()(indexSet.TextureIndex << 23);
    }
};
