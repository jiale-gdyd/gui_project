#pragma once

#include <map>
#include <mutex>
#include <memory>
#include <thread>

namespace hope {
class Object;

namespace detail {
class ObjectData {
public:
    ObjectData(std::thread::id id) : m_thread_id(std::move(id)) {

    }

    std::unique_lock<std::mutex> lock() {
        return lock(*this);
    }

    static std::unique_lock<std::mutex> lock(const std::shared_ptr<ObjectData> &data) {
        return data ? lock(*data) : std::unique_lock<std::mutex>();
    }

    static std::unique_lock<std::mutex> lock(ObjectData &data) {
        return std::unique_lock<std::mutex>(data.m_mutex);
    }

    static std::pair<std::unique_lock<std::mutex>, std::unique_lock<std::mutex>> lock(const std::shared_ptr<ObjectData> &first, const std::shared_ptr<ObjectData> &second) {
        if (first && second) {
            return lock(*first, *second);
        } else if (first) {
            return { lock(*first), std::unique_lock<std::mutex>() };
        } else if (second) {
            return { lock(*second), std::unique_lock<std::mutex>() };
        } else {
            return { std::unique_lock<std::mutex>(), std::unique_lock<std::mutex>() };
        }
    }

    static std::pair<std::unique_lock<std::mutex>, std::unique_lock<std::mutex>> lock(ObjectData &first, ObjectData &second) {
        ObjectData *first_address = &first;
        ObjectData *second_address = &second;

        if (first_address >= second_address) {
            std::swap(first_address, second_address);
        }

        std::unique_lock<std::mutex> first_lock = lock(*first_address);
        std::unique_lock<std::mutex> second_lock;
        if (first_address != second_address) {
            second_lock = lock(*second_address);
        }

        return {std::move(first_lock), std::move(second_lock)};
    }

    std::thread::id m_thread_id;
    std::mutex      m_mutex;
};

class ObjectDataRegistry {
public:
    static ObjectDataRegistry &instance();

    ObjectDataRegistry(const ObjectDataRegistry &) = delete;
    ObjectDataRegistry(ObjectDataRegistry &&) = delete;

    ObjectDataRegistry &operator=(const ObjectDataRegistry &) = delete;
    ObjectDataRegistry &operator=(ObjectDataRegistry &&) = delete;

    std::weak_ptr<ObjectData> data(Object *object) {
        auto it = m_data.find(object);
        return it != m_data.end() ? it->second : std::weak_ptr<ObjectData>();
    }

    void register_object_data(Object *object, const std::shared_ptr<ObjectData> &data) {
        m_data.emplace(object, data);
    }

    void unregister_object_data(Object *object) {
        m_data.erase(object);
    }

private:
    ObjectDataRegistry() = default;

    std::map<Object *, std::weak_ptr<ObjectData>> m_data;
};
}
}
