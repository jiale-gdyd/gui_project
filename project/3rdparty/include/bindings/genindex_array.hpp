#pragma once

#include <vector>
#include <limits>
#include <string>
#include <cstdint>
#include <cassert>
#include <optional>
#include <stdexcept>
#include <functional>

namespace Bindings {
namespace Private {
struct GenerationalIndex {
    uint32_t index = 0;
    uint32_t generation = 0;
};

class GenerationalIndexAllocator {
    struct AllocatorEntry {
        bool     isLive = false;
        uint32_t generation = 0;
    };

    std::vector<AllocatorEntry> m_entries;
    std::vector<uint32_t>       m_freeIndices;

public:
    GenerationalIndex allocate() {
        if (m_freeIndices.size() > 0) {
            uint32_t index = m_freeIndices.back();
            m_freeIndices.pop_back();

            m_entries[index].generation += 1;
            m_entries[index].isLive = true;

            return { index, m_entries[index].generation };
        } else {
            if ((m_entries.size() + 1) >= std::numeric_limits<uint32_t>::max()) {
                throw std::length_error(std::string("Maximum number of values inside GenerationalIndexArray reached: ") + std::to_string(m_entries.size()));
            }

            m_entries.push_back({ true, 0 });
            return { static_cast<uint32_t>(m_entries.size()) - 1, 0 };
        }
    }

    bool deallocate(GenerationalIndex index) {
        if (isLive(index)) {
            m_entries[index.index].isLive = false;
            m_freeIndices.emplace_back(index.index);
            return true;
        }

        return false;
    }

    bool isLive(GenerationalIndex index) const noexcept {
        return ((index.index < m_entries.size()) && (m_entries[index.index].generation == index.generation) && m_entries[index.index].isLive);
    }
};

/**
 * GenerationalIndexArray将元素存储在连续的内存中，就像std::vector一样，还允许通过索引访问在恒定的时间内检索项，
 * 但它会跟踪索引处值的版本生成，以便当它试图访问的索引处的项不再是它想要的项时，它可以通知访问器
 */
template<typename T>
class GenerationalIndexArray {
    struct Entry {
        uint32_t generation;
        T value;
    };

    /* TODO: m_entries从来不会在条目被删除后收缩，如果这成为一个问题，在某些时候添加一个"trim"函数可能是一个好主意 */

    std::vector<std::optional<Entry>> m_entries;
    GenerationalIndexAllocator        m_allocator;

public:
    /* 设置数组中特定索引处的值 */
    void set(const GenerationalIndex index, T &&value) {
        while (m_entries.size() <= index.index) {
            m_entries.emplace_back(std::nullopt);
        }

#ifndef NDEBUG
        uint32_t previousGeneration = 0;

        const auto &previousEntry = m_entries[index.index];
        if (previousEntry) {
            previousGeneration = previousEntry->generation;
        }

        assert(index.generation >= previousGeneration);
#endif

        m_entries[index.index] = std::optional<Entry>{ { index.generation, std::move(value) } };
    }

    /* 在第一个空闲索引处插入一个值并取回索引 */
    GenerationalIndex insert(T &&value) {
        const auto index = m_allocator.allocate();
        set(index, std::move(value));
        return index;
    }

    /* 擦除指定索引处的值并再次释放索引 */
    void erase(GenerationalIndex index) {
        if (m_allocator.deallocate(index)) {
            m_entries[index.index] = std::nullopt;
        }
    }

    /* 获取指向指定索引处值的指针 */
    T *get(GenerationalIndex index) {
        if (index.index >= m_entries.size()) {
            return nullptr;
        }

        auto &entry = m_entries[index.index];
        if (entry && (entry->generation == index.generation)) {
            return &entry->value;
        }

        return nullptr;
    }

    /* 获取指向指定索引处值的const指针 */
    const T *get(GenerationalIndex index) const noexcept {
        return const_cast<const T *>(const_cast<GenerationalIndexArray *>(this)->get(index));
    }

    /* 擦除数组中的所有值，从而释放所有下标 */
    void clear() {
        const auto numEntries = entriesSize();

        for (auto i = decltype(numEntries){ 0 }; i < numEntries; ++i) {
            const auto index = indexAtEntry(i);
            if (index != std::nullopt) {
                erase(*index);
            }
        }
    }

    /* 当前数组中的条目数量不一定都对应于有效的索引，可以使用"indexAtEntry"将条目索引转换为可选的GenerationalIndex */
    uint32_t entriesSize() const noexcept {
        return static_cast<uint32_t>(m_entries.size());
    }

    /* 将条目索引转换为GenerationalIndex，如果可能，否则返回nullopt */
    std::optional<GenerationalIndex> indexAtEntry(uint32_t entryIndex) const {
        if (entryIndex >= entriesSize()) {
            return std::nullopt;
        }

        const auto &entry = m_entries[entryIndex];
        if (!entry) {
            return std::nullopt;
        }

        GenerationalIndex index = { entryIndex, entry->generation };
        if (m_allocator.isLive(index)) {
            return index;
        }

        return std::nullopt;
    }
};
}
}
