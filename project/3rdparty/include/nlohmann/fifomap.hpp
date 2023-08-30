#ifndef NLOHMANN_FIFO_MAP_HPP
#define NLOHMANN_FIFO_MAP_HPP

#include <map>
#include <vector>
#include <memory>
#include <limits>
#include <cstdlib>
#include <utility>
#include <iostream>
#include <algorithm>
#include <functional>
#include <unordered_map>

namespace nlohmann {
template<class Key>
class fifo_map_compare {
public:
    fifo_map_compare(std::unordered_map<Key, std::size_t> *keys, std::size_t timestamp = 1) : m_timestamp(timestamp), m_keys(keys) {}

    bool operator()(const Key &lhs, const Key &rhs) const {
        const auto timestamp_lhs = m_keys->find(lhs);
        const auto timestamp_rhs = m_keys->find(rhs);

        if (timestamp_lhs == m_keys->end()) {
            return false;
        }

        if (timestamp_rhs == m_keys->end()) {
            return true;
        }

        return (timestamp_lhs->second < timestamp_rhs->second);
    }

    void add_key(const Key& key) {
        m_keys->insert({key, m_timestamp++});
    }

    void remove_key(const Key& key) {
        m_keys->erase(key);
    }

private:
    template <class MapKey, class MapT, class MapCompare, class MapAllocator> friend class fifo_map;

private:
    std::size_t                          m_timestamp = 1;
    std::unordered_map<Key, std::size_t> *m_keys = nullptr;
};

template <class Key, class T, class Compare = fifo_map_compare<Key>, class Allocator = std::allocator<std::pair<const Key, T>>>
class fifo_map {
public:
    using key_type = Key;
    using mapped_type = T;
    using value_type = std::pair<const Key, T>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using key_compare = Compare;
    using allocator_type = Allocator;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;

    using internal_map_type = std::map<Key, T, Compare, Allocator>;

    using iterator = typename internal_map_type::iterator;
    using const_iterator = typename internal_map_type::const_iterator;
    using reverse_iterator = typename internal_map_type::reverse_iterator;
    using const_reverse_iterator = typename internal_map_type::const_reverse_iterator;

public:
    fifo_map() : m_keys(), m_compare(&m_keys), m_map(m_compare) {}
    fifo_map(const fifo_map &f) : m_keys(f.m_keys), m_compare(&m_keys, f.m_compare.m_timestamp), m_map(f.m_map.begin(), f.m_map.end(), m_compare) {}

    template<class InputIterator>
    fifo_map(InputIterator first, InputIterator last) : m_keys(), m_compare(&m_keys), m_map(m_compare) {
        for (auto it = first; it != last; ++it) {
            insert(*it);
        }
    }

    fifo_map(std::initializer_list<value_type> init) : fifo_map() {
        for (auto x : init) {
            insert(x);
        }
    }

    T &at(const Key &key) {
        return m_map.at(key);
    }

    const T &at(const Key &key) const {
        return m_map.at(key);
    }

    T &operator[](const Key &key) {
        m_compare.add_key(key);
        return m_map[key];
    }

    T &operator[](Key &&key) {
        m_compare.add_key(key);
        return m_map[key];
    }

    iterator begin() noexcept {
        return m_map.begin();
    }

    iterator end() noexcept {
        return m_map.end();
    }

    const_iterator begin() const noexcept {
        return m_map.begin();
    }

    const_iterator end() const noexcept {
        return m_map.end();
    }

    const_iterator cbegin() const noexcept {
        return m_map.cbegin();
    }

    const_iterator cend() const noexcept {
        return m_map.cend();
    }

    reverse_iterator rbegin() noexcept {
        return m_map.rbegin();
    }

    reverse_iterator rend() noexcept {
        return m_map.rend();
    }

    const_reverse_iterator rbegin() const noexcept {
        return m_map.rbegin();
    }

    const_reverse_iterator rend() const noexcept {
        return m_map.rend();
    }

    const_reverse_iterator crbegin() const noexcept {
        return m_map.crbegin();
    }

    const_reverse_iterator crend() const noexcept {
        return m_map.crend();
    }

    bool empty() const noexcept {
        return m_map.empty();
    }

    size_type size() const noexcept {
        return m_map.size();
    }

    size_type max_size() const noexcept {
        return m_map.max_size();
    }

    void clear() noexcept {
        m_map.clear();
        m_keys.clear();
    }

    std::pair<iterator, bool> insert(const value_type &value) {
        m_compare.add_key(value.first);
        return m_map.insert(value);
    }

    template<class P>
    std::pair<iterator, bool> insert(P &&value) {
        m_compare.add_key(value.first);
        return m_map.insert(value);
    }

    iterator insert(const_iterator hint, const value_type &value) {
        m_compare.add_key(value.first);
        return m_map.insert(hint, value);
    }

    iterator insert(const_iterator hint, value_type&& value) {
        m_compare.add_key(value.first);
        return m_map.insert(hint, value);
    }

    template<class InputIt>
    void insert(InputIt first, InputIt last) {
        for (const_iterator it = first; it != last; ++it) {
            m_compare.add_key(it->first);
        }

        m_map.insert(first, last);
    }

    void insert(std::initializer_list<value_type> ilist) {
        for (auto value : ilist) {
            m_compare.add_key(value.first);
        }

        m_map.insert(ilist);
    }

    template<class... Args>
    std::pair<iterator, bool> emplace(Args && ... args) {
        typename fifo_map::value_type value(std::forward<Args>(args)...);
        m_compare.add_key(value.first);
        return m_map.emplace(std::move(value));
    }

    template<class... Args>
    iterator emplace_hint(const_iterator hint, Args&& ... args) {
        typename fifo_map::value_type value(std::forward<Args>(args)...);
        m_compare.add_key(value.first);
        return m_map.emplace_hint(hint, std::move(value));
    }

    iterator erase(const_iterator pos) {
        m_compare.remove_key(pos->first);
        return m_map.erase(pos);
    }

    iterator erase(const_iterator first, const_iterator last) {
        for (const_iterator it = first; it != last; ++it) {
            m_compare.remove_key(it->first);
        }

        return m_map.erase(first, last);
    }

    size_type erase(const key_type& key) {
        size_type res = m_map.erase(key);

        if (res > 0) {
            m_compare.remove_key(key);
        }

        return res;
    }

    void swap(fifo_map &other) {
        std::swap(m_map, other.m_map);
        std::swap(m_compare, other.m_compare);
        std::swap(m_keys, other.m_keys);
    }

    size_type count(const Key &key) const {
        return m_map.count(key);
    }

    iterator find(const Key& key) {
        return m_map.find(key);
    }

    const_iterator find(const Key& key) const {
        return m_map.find(key);
    }

    std::pair<iterator, iterator> equal_range(const Key &key) {
        return m_map.equal_range(key);
    }

    std::pair<const_iterator, const_iterator> equal_range(const Key &key) const {
        return m_map.equal_range(key);
    }

    iterator lower_bound(const Key &key) {
        return m_map.lower_bound(key);
    }

    const_iterator lower_bound(const Key &key) const {
        return m_map.lower_bound(key);
    }

    iterator upper_bound(const Key& key) {
        return m_map.upper_bound(key);
    }

    const_iterator upper_bound(const Key& key) const {
        return m_map.upper_bound(key);
    }

    key_compare key_comp() const {
        return m_compare;
    }

    friend bool operator==(const fifo_map &lhs, const fifo_map &rhs) {
        return (lhs.m_map == rhs.m_map);
    }

    friend bool operator!=(const fifo_map &lhs, const fifo_map &rhs) {
        return (lhs.m_map != rhs.m_map);
    }

    friend bool operator<(const fifo_map &lhs, const fifo_map &rhs) {
        return (lhs.m_map < rhs.m_map);
    }

    friend bool operator<=(const fifo_map &lhs, const fifo_map &rhs) {
        return (lhs.m_map <= rhs.m_map);
    }

    friend bool operator>(const fifo_map &lhs, const fifo_map &rhs) {
        return (lhs.m_map > rhs.m_map);
    }

    friend bool operator>=(const fifo_map &lhs, const fifo_map &rhs) {
        return (lhs.m_map >= rhs.m_map);
    }

private:
    std::unordered_map<Key, std::size_t> m_keys;
    Compare                              m_compare;
    internal_map_type                    m_map;
};
}

namespace std {
template <class Key, class T, class Compare, class Allocator>
inline void swap(nlohmann::fifo_map<Key, T, Compare, Allocator> &m1, nlohmann::fifo_map<Key, T, Compare, Allocator> &m2)
{
    m1.swap(m2);
}
}

#endif