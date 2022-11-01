#pragma once

#include <memory>
#include <utility>
#include <cassert>
#include <optional>
#include <stdexcept>
#include <functional>
#include <type_traits>

#include "utils.hpp"
#include "genindex_array.hpp"

namespace Bindings {
template<typename... Args>
class Signal;

namespace Private {
/**
 * 这个类定义了一个虚拟接口，这个连接句柄引用的信号必须实现。它允许ConnectionHandle引用这个非模板类，然后使用虚函数调用将其分发到模板实现。
 * 它允许ConnectionHandle成为一个非模板类
 */
class SignalImplBase {
public:
    SignalImplBase() = default;
    virtual ~SignalImplBase() = default;

    virtual void disconnect(const GenerationalIndex &id) = 0;
    virtual bool blockConnection(const GenerationalIndex &id, bool blocked) = 0;

    virtual bool isConnectionActive(const GenerationalIndex &id) const = 0;
    virtual bool isConnectionBlocked(const GenerationalIndex &id) const = 0;
};
}

/**
 * ConnectionHandle表示信号到插槽的连接(即信号发出时调用的函数)。当创建连接时，它从一个Signal返回，
 * 并用于通过断开连接、(不)阻塞连接和检查其状态来管理连接
 */
class ConnectionHandle {
public:
    ConnectionHandle() = default;

    ConnectionHandle(const ConnectionHandle &) = default;
    ConnectionHandle &operator=(const ConnectionHandle &) = default;

    ConnectionHandle(ConnectionHandle &&) = default;
    ConnectionHandle &operator=(ConnectionHandle &&) = default;

    /**
     * 断开连接的插槽。当这个函数被调用时，传递给Signal::connect以创建这个ConnectionHandle的函数在发出Signal时将不再被调用。
     * 如果ConnectionHandle未激活或连接已经断开，则不会发生任何事情。
     * 在这个调用之后，ConnectionHandle将是不活动的(即isActive()返回false)，不再属于任何信号(即belongsTo返回false)
     */
    void disconnect() {
        if (auto shared_impl = checkedLock()) {
            shared_impl->disconnect(m_id);
        }

        m_signalImpl.reset();
    }

    /**
     * 检查该ConnectionHandle的连接是否处于活动状态。
     * 如果ConnectionHandle引用一个活动信号并且连接之前没有断开，则为true，否则为false
     */
    bool isActive() const {
        return static_cast<bool>(checkedLock());
    }

    /**
     * 设置连接的块状态。如果连接被阻塞，发出信号将不再调用该连接槽，直到连接被解除阻塞。
     * 行为与使用此ConnectionHandle作为参数调用Signal::blockConnection相同。
     * 要临时阻塞连接，可以考虑使用ConnectionBlocker的实例，它提供了一个rai风格的实现，确保连接总是返回到其原始状态
     *
     * @param blocked 连接的新阻塞状态
     * @return        连接以前是否被阻塞
     * @throw         如果连接不是活动的抛出(即isActive()返回false)std::out_of_range异常
     */
    bool block(bool blocked) {
        if (auto shared_impl = checkedLock()) {
            return shared_impl->blockConnection(m_id, blocked);
        }

        throw std::out_of_range("Cannot block a non-active connection!");
    }

    /**
     * 检查连接当前是否被阻塞。要改变连接的阻塞状态，调用ConnectionHandle::block
     */
    bool isBlocked() const {
        if (auto shared_impl = checkedLock()) {
            return shared_impl->isConnectionBlocked(m_id);
        }

        throw std::out_of_range("Cannot check whether a non-active connection is blocked!");
    }

    /**
     * 检查这个ConnectionHandle是否属于给定的信号。如果此ConnectionHandle引用给定信号中的连接，则为true
     */
    template<typename... Args>
    bool belongsTo(const Signal<Args...> &signal) const {
        auto shared_impl = m_signalImpl.lock();
        return shared_impl && shared_impl == std::static_pointer_cast<Private::SignalImplBase>(signal.m_impl);
    }

private:
    template<typename...>
    friend class Signal;

    std::weak_ptr<Private::SignalImplBase> m_signalImpl;
    Private::GenerationalIndex             m_id;

    ConnectionHandle(std::weak_ptr<Private::SignalImplBase> signalImpl, Private::GenerationalIndex id)
        : m_signalImpl{ std::move(signalImpl) }, m_id{ std::move(id) }
    {

    }

    /* 检查weak_ptr是否可以锁定，连接是否仍然处于活动状态 */
    std::shared_ptr<Private::SignalImplBase> checkedLock() const {
        auto shared_impl = m_signalImpl.lock();
        if (shared_impl && shared_impl->isConnectionActive(m_id)) {
            return shared_impl;
        }

        return nullptr;
    }
};

/**
 * Signal为对象之间的通信提供了一种机制。
 * Signal可以用来通知任意数量的插槽发生了某个事件。插槽几乎可以是任何可调用对象，包括成员函数和lambda。
 * 这种连接以类型安全的方式进行，因为只有当槽的参数与Signal发出的值相匹配时，才能将槽连接到Signal。
 * Args类型参数包描述信号将发出的值类型
 */
template<typename... Args>
class Signal {
    static_assert(std::conjunction<std::negation<std::is_rvalue_reference<Args>>...>::value, "R-value references are not allowed as Signal parameters!");

    /* Signal::Impl类存在，所以Signals可以以类似于pimpl的方式实现。这允许我们轻松地移动信号而不丢失它们的连接句柄，以及使未连接的信号只有sizeof(shared_ptr) */
    class Impl : public Private::SignalImplBase {
    public:
        Impl() noexcept {}
        ~Impl() noexcept {}

        Impl(Impl const &other) = delete;
        Impl &operator=(Impl const &other) = delete;

        Impl(Impl &&other) = delete;
        Impl &operator=(Impl &&other) = delete;

        /* 将std::function连接到信号。返回值可用于再次断开函数连接 */
        Private::GenerationalIndex connect(std::function<void(Args...)> const &slot) {
            return m_connections.insert({ slot });
        }

        /* 断开先前连接的函数 */
        void disconnect(const Private::GenerationalIndex &id) override {
            m_connections.erase(id);
        }

        /* 断开以前连接的所有函数 */
        void disconnectAll() {
            m_connections.clear();
        }

        bool blockConnection(const Private::GenerationalIndex &id, bool blocked) override {
            Connection *connection = m_connections.get(id);
            if (connection) {
                const bool wasBlocked = connection->blocked;
                connection->blocked = blocked;
                return wasBlocked;
            } else {
                throw std::out_of_range("Provided ConnectionHandle does not match any connection\nLikely the connection was deleted before!");
            }
        }

        bool isConnectionActive(const Private::GenerationalIndex &id) const override {
            return m_connections.get(id);
        }

        bool isConnectionBlocked(const Private::GenerationalIndex &id) const override {
            auto connection = m_connections.get(id);
            if (connection) {
                return connection->blocked;
            } else {
                throw std::out_of_range("Provided ConnectionHandle does not match any connection\nLikely the connection was deleted before!");
            }
        }

        /* 调用所有连接的函数 */
        void emit(Args... p) const {
            const auto numEntries = m_connections.entriesSize();

            /* 这个循环可以容忍信号句柄在槽内断开，但向槽内的信号添加新连接仍然是未定义的行为 */
            for (auto i = decltype(numEntries){ 0 }; i < numEntries; ++i) {
                const auto index = m_connections.indexAtEntry(i);
                if (index) {
                    const auto con = m_connections.get(*index);
                    if (!con->blocked) {
                        con->slot(p...);
                    }
                }
            }
        }

    private:
        struct Connection {
            std::function<void (Args...)> slot;
            bool                          blocked{ false };
        };

        mutable Private::GenerationalIndexArray<Connection> m_connections;
    };

public:
    Signal() = default;

    Signal(const Signal &) = delete;
    Signal &operator=(Signal const &other) = delete;

    Signal(Signal &&other) noexcept = default;
    Signal &operator=(Signal &&other) noexcept = default;

    /* 当信号被破坏时，它会断开所有槽的连接。因此，属于这个信号的所有活动的ConnectionHandle将不再是活动的(即ConnectionHandle::isActive将返回false) */
    ~Signal() {
        disconnectAll();
    }

    /**
     * 将std::function连接到信号。当在Signal上调用emit()时，将使用提供给emit()的参数调用函数。
     * 返回ConnectionHandle的实例，可用于断开连接或临时阻塞连接
     */
    ConnectionHandle connect(std::function<void(Args...)> const &slot) {
        ensureImpl();
        return ConnectionHandle{ m_impl, m_impl->connect(slot) };
    }

    /**
     * Signal::connect的模板重载，可以更容易地将任意函数连接到这个Signal。它将一个函数连接到这个信号，将任何提供的参数绑定到那个函数，
     * 并丢弃由这个信号发出的结果函数不需要的任何值。这对于将成员函数连接到信号特别有用
     *
     * 返回引用此连接的Signal::ConnectionHandle的实例。警告: 当连接成员函数时，当包含插槽的对象超出作用域时，必须使用返回的ConnectionHandle断开连接!
     * 
     * enable_if_t确保只有当我们提供的函数不能以其他方式转换为std::function<void(args...)>时，这个连接函数专门化才可用，因为它会尝试优先于普通的连接函数
     **/
    template<typename Func, typename... FuncArgs, typename = std::enable_if_t<std::disjunction_v<std::negation<std::is_convertible<Func, std::function<void(Args...)>>>, std::integral_constant<bool, sizeof...(FuncArgs) /*Also enable this function if we want to bind at least one argument*/>>>>
    ConnectionHandle connect(Func &&slot, FuncArgs &&...args) {
        std::function<void(Args...)> bound = Private::bind_first(std::forward<Func>(slot), std::forward<FuncArgs>(args)...);
        return connect(bound);
    }

    /**
     * 断开先前连接的槽位。成功断开槽位后，ConnectionHandle将不再是活动的。(例如，ConnectionHandle::isActive将返回false)。
     * 如果ConnectionHandle不属于这个信号(即ConnectionHandle::belongsTo返回false)，则抛出std::out_of_range异常
     */
    void disconnect(const ConnectionHandle &handle) {
        if (m_impl && handle.belongsTo(*this)) {
            m_impl->disconnect(handle.m_id);
        } else {
            throw std::out_of_range("Provided ConnectionHandle does not match any connection\nLikely the connection was deleted before!");
        }
    }

    /* 断开以前连接的所有功能。属于该信号的所有当前活动的连接句柄之后将不再活动。(例如，ConnectionHandle::isActive将返回false) */
    void disconnectAll() {
        if (m_impl) {
            m_impl->disconnectAll();

            /* 一旦断开所有连接，我们就可以释放Impl的所有权。这并不会破坏Signal本身，只会破坏Impl对象。如果连接了另一个槽位，则将构造另一个Impl对象 */
            m_impl.reset();
        }
    }

    /**
     * 设置连接的块状态。如果连接被阻塞，发出信号将不再调用该连接槽，直到连接被解除阻塞。块可以用作替代方法。
     * 要临时阻塞连接，可以考虑使用ConnectionBlocker的实例，它提供了一个rai风格的实现，确保连接总是返回到其原始状态
     *
     * @param blocked 是否应该从现在开始阻止连接
     * @param handle  要阻塞的ConnectionHandle
     * @return        连接以前是否被阻塞
     * @throw         如果ConnectionHandle不属于这个信号(即ConnectionHandle::belongsTo返回false)，则抛出std::out_of_range异常
     */
    bool blockConnection(const ConnectionHandle &handle, bool blocked) {
        if (m_impl && handle.belongsTo(*this)) {
            return m_impl->blockConnection(handle.m_id, blocked);
        } else {
            throw std::out_of_range("Provided ConnectionHandle does not match any connection\nLikely the connection was deleted before!");
        }
    }

    /**
     * 检查连接当前是否被阻塞。要改变连接的阻塞状态，调用blockConnection()
     *
     * @return 连接当前是否阻塞
     * @throw  如果ConnectionHandle不属于这个信号(即ConnectionHandle::belongsTo返回false)，则抛出std::out_of_range异常
     */
    bool isConnectionBlocked(const ConnectionHandle &handle) const {
        assert(handle.belongsTo(*this));

        if (!m_impl) {
            throw std::out_of_range("Provided ConnectionHandle does not match any connection\nLikely the connection was deleted before!");
        }

        return m_impl->isConnectionBlocked(handle.m_id);
    }

    /**
     * 发出信号，只要没有阻塞，就会调用所有连接的槽。提供给emit的实参将通过复制传递到每个插槽，
     * 因此尽可能考虑使用(const)引用作为Signal的参数。注意:插槽可能在发射过程中断开自己，
     * 但是在信号的发射函数期间连接的插槽是否也将在这次发射期间被调用，还是只在下一次发射时被调用是未定义的
     */
    void emit(Args... p) const {
        if (m_impl) {
            m_impl->emit(p...);
        }
    }

private:
    friend class ConnectionHandle;

    void ensureImpl() {
        if (!m_impl) {
            m_impl = std::make_shared<Impl>();
        }
    }

    /**
     * 这里使用的是shared_ptr而不是unique_ptr，因此ConnectionHandle实例可以使用weak_ptr来检查他们引用的Signal::Impl是否仍然存在。
     * 这使得信号在理论上很容易被复制，但其语义尚不清楚。复制可以简单地复制shared_ptr，这意味着副本将共享原始文件的连接，
     * 这可能是不直观的，或者也必须复制Impl。然而，这将使连接没有句柄来断开连接。所以现在是禁止复制的。
     * 这个shared_ptr更像是一个unique_ptr，在ConnectionHandle中有附加的weak_ptr，可以检查Impl对象是否仍然存在
     */
    mutable std::shared_ptr<Impl> m_impl;
};

/**
 * ConnectionBlocker是一种方便的raii式机制，用于临时阻塞连接。当ConnectionBlocker被构造时，它将阻塞连接。
 * 当它被销毁时，它将把连接返回到构造ConnectionBlocker之前的阻塞状态
 */
class ConnectionBlocker {
public:
    /* 构造一个新的ConnectionBlocker并阻塞此ConnectionHandle引用的连接 */
    explicit ConnectionBlocker(const ConnectionHandle &handle) : m_handle{ handle } {
        m_wasBlocked = m_handle.block(true);
    }

    /* 销毁ConnectionBlocker并将连接返回到构造ConnectionBlocker之前的阻塞状态 */
    ~ConnectionBlocker() {
        m_handle.block(m_wasBlocked);
    }

private:
    ConnectionHandle m_handle;
    bool             m_wasBlocked{ false };
};
}
