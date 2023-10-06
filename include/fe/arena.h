#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "fe/assert.h"

namespace fe {

/// An arena pre-allocates so-called *pages* of size @p p.
/// You can use Arena::alloc to obtain memory from this.
/// When a page runs out of memory, the next page will be (pre-)allocated.
/// You cannot directly release memory obtained via this method.
/// Instead, *all* memory acquired via this Arena will be released as soon as this Arena will be destroyed.
/// As an exception, you can Arena::dealloc memory that just as been acquired.
///
/// Use Allocator to adopt it in [containers](https://en.cppreference.com/w/cpp/named_req/AllocatorAwareContainer).
/// Construct it via Arena::allocator.
/// @note The Arena assumes a consistent alignment of @p A for all  allocated objects.
class Arena {
public:
    static constexpr size_t Default_Page_Size = 1024 * 1024; ///< 1MB.

    /// @name Allocator
    ///@{
    /// An [allocator](https://en.cppreference.com/w/cpp/named_req/Allocator) in order to use this Arena for containers.
    template<class T> struct Allocator {
        using value_type = T;

        Allocator() noexcept = delete;
        template<class U>
        Allocator(const Arena::Allocator<U>& allocator) noexcept
            : arena(allocator.arena) {}
        Allocator(Arena& arena) noexcept
            : arena(arena) {}

        [[nodiscard]] T* allocate(size_t num_elems) { return arena.allocate<T>(num_elems); }

        void deallocate(T*, size_t) noexcept {}

        template<class U> bool operator==(const Allocator<U>& a) const noexcept { return &arena == &a.arena; }
        template<class U> bool operator!=(const Allocator<U>& a) const noexcept { return &arena != &a.arena; }

        Arena& arena;
    };

    /// Create Allocator from Arena.
    template<class T> Allocator<T> allocator() { return Allocator<T>(*this); }
    ///@}

    /// @name Smart pointer
    ///@{
    /// This is a [std::unique_ptr](https://en.cppreference.com/w/cpp/memory/unique_ptr)
    /// that uses the Arena under the hood
    /// and whose deleter will *only* invoke the destructor but *not* `delete` anything;
    /// memory will be released upon destruction of the Arena.
    ///
    /// Use like this:
    /// ```
    /// auto ptr = arena.mk<Foo>(a, b, c); // new Foo(a, b, c) placed into arena
    /// ```
    template<class T> struct Deleter {
        constexpr Deleter() noexcept = default;
        template<class U> constexpr Deleter(const Deleter<U>&) noexcept {}
        void operator()(T* ptr) { ptr->~T(); }
    };

    template<class T> using Ptr = std::unique_ptr<T, Deleter<T>>;
    template<class T, class... Args> Ptr<T> mk(Args&&... args) {
        auto ptr = new (allocate(sizeof(T))) T(std::forward<Args&&>(args)...);
        return Ptr<T>(ptr, Deleter<T>());
    }
    ///@}

    /// @name Construction/Destruction
    ///@{
    Arena(size_t page_size = Default_Page_Size)
        : page_size_(page_size)
        , index_(page_size) {}
    ~Arena() {
        for (auto p : pages_) delete[] p;
    }
    ///@}

    /// @name Alloc
    ///@{

    /// Align next allocate(size_t) to @p a.
    void align(size_t a) { index_ = (index_ + (a - 1)) & ~(a - 1); }

    /// Get @p n bytes of fresh memory.
    [[nodiscard]] void* allocate(size_t num_bytes) {
        if (index_ + num_bytes > page_size_) {
            pages_.emplace_back(new char[std::max(page_size_, num_bytes)]);
            index_ = 0;
        }

        auto result = pages_.back() + index_;
        index_ += num_bytes;
        return result;
    }

    template<class T>
    [[nodiscard]] T* allocate(size_t num_elems) {
        align(alignof(T));
        return static_cast<T*>(allocate(num_elems * std::max(sizeof(T), alignof(T))));
    }

    ///@}

    /// @name Dealloc
    ///@{
    /// Tries to remove bytes again.
    /// If this crosses a page boundary, nothing happens.
    /// Use like this:
    /// ```
    /// auto state = arena.state();
    /// auto ptr = arena.allocate(n);
    /// if (/* I don't want that */) arena.deallocate(state);
    /// @warning Only use, if you really know what you are doing.
    using State = std::pair<size_t, size_t>;
    State state() const { return {pages_.size(), index_}; }
    void deallocate(State state) {
        if (state.first == pages_.size()) index_ = state.second; // don't care otherwise
    }
    ///@}

    friend void swap(Arena& a1, Arena& a2) {
        using std::swap;
        swap(a1.pages_, a2.pages_);
        swap(a1.index_, a2.index_);
    }

private:
    std::vector<char*> pages_;
    size_t page_size_;
    size_t index_;
};

} // namespace fe
