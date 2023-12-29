#ifndef LIB_SRC_RING_BUFFER_HPP_
#define LIB_SRC_RING_BUFFER_HPP_

#include <stdexcept>
#include <vector>

#include <glog/logging.h>

#define CHECK_RING_BUFFER(condition) if (!(condition)) throw std::runtime_error("RingBuffer CHECK failed: " #condition " ")

namespace detail {

/// struct to perform circular arithmetic, i.e.: perform modulo after each operation.
/// A faster version of this struct would be to always use power of two for @param{size}.
/// We could check at construction/Resize of the RingBuffer:
///     if its size is 2^n then use the faster indexing scheme
///     else use regular indexing
struct CircularArithmeticSimple {
    /// Increment @param{index} modulo @param{size}
    static inline size_t increment(size_t index, size_t size) {
        ++index;
        if (index == size) {
            index = 0;
        }
        return index;
    }
    /// Decrement @param{index} modulo @param{size}
    static inline size_t decrement(size_t index, size_t size) {
        if (index != 0) {
            --index;
        } else {
            index = size - 1;
        }
        return index;
    }
    /// Add @param{delta} to @param{index} modulo @param{size}
    static inline size_t plus(size_t index, size_t delta, size_t size) { return (index + delta) % size; }
    /// Subtract @param{delta} to @param{index} modulo @param{size}
    static inline size_t minus(size_t index, size_t delta, size_t size) {
        delta = delta % size;
        index = index + size;
        return (index - delta) % size;
    }
};

/// struct that implements finite counting so that @param{lower} <= @param{index} <= @param{upper}.
struct SaturatingCounter {
    /// Increment @param{index} only if @param{index} < @param{upper}
    static inline size_t increment(size_t index, size_t upper) {
        if (index < upper) {
            ++index;
        }
        return index;
    }
    /// Decrement @param{index} only if @param{index} > @param{lower}
    static inline size_t decrement(size_t index, size_t lower) {
        if (index > lower) {
            --index;
        }
        return index;
    }
};

}  // namespace detail

/// Not thread-safe.
/// Push will overwrite the oldest element in the buffer with a new element.
/// Pop will remove and return the oldest element in the buffer.
///
/// \tparam T                   Date type in the buffer.
/// \tparam Alloc               Allocator passed directly to std::vector.
/// \tparam CircularArithmetic  This can be easily changed to a more efficient version later. The "Circularity" of the
///                             buffer is completely defined by the circular indexing scheme.
template <typename T, typename Alloc = std::allocator<T>, class CircularArithmetic = detail::CircularArithmeticSimple>
class RingBuffer {
public:
    RingBuffer() : index_{0}, count_{0}, push_count_{0}, pop_count_{0}, buffer_() {}
    explicit RingBuffer(size_t size) : index_{0}, count_{0}, push_count_{0}, pop_count_{0},  buffer_(size) {}

    RingBuffer(RingBuffer const& other)
            : index_{other.index_}
            , count_{other.count_}
            , push_count_{other.push_count_}
            , pop_count_{other.pop_count_}
            , buffer_{other.buffer_}
    {}

    RingBuffer& operator=(RingBuffer const& other) noexcept {
        index_ = other.index_;
        count_ = other.count_;
        push_count_ = other.push_count_;
        pop_count_ = other.pop_count_;
        buffer_ = other.buffer_;
        return *this;
    }

    RingBuffer(RingBuffer&& other) noexcept
            : index_{other.index_}
            , count_{other.count_}
            , push_count_{other.push_count_}
            , pop_count_{other.pop_count_}
            , buffer_{std::move(other.buffer_)}
    {}

    RingBuffer& operator=(RingBuffer&& other) noexcept {
        index_ = other.index_;
        count_ = other.count_;
        push_count_ = other.push_count_;
        pop_count_ = other.pop_count_;
        std::swap(buffer_, other.buffer_);
        return *this;
    }

    /// Add element to the buffer overwriting the oldest element if the buffer is full.
    inline void push_back(T&& in) {
        buffer_[index_] = std::move(in);
        count_ = detail::SaturatingCounter::increment(count_, buffer_.size());
        index_ = CircularArithmetic::increment(index_, buffer_.size());
        ++push_count_;
    }

    /// Add element to the buffer overwriting the oldest element if the buffer is full.
    void push_back(T const& in) {
        buffer_[index_] = in;
        count_ = detail::SaturatingCounter::increment(count_, buffer_.size());
        index_ = CircularArithmetic::increment(index_, buffer_.size());
        ++push_count_;
    }

    /// Remove the newest element from the buffer.
    inline bool pop_back(T* out) {
        if (count_ == 0) {
            return false;
        } else {
            count_ = detail::SaturatingCounter::decrement(count_, 0);
            index_ = CircularArithmetic::decrement(index_, buffer_.size());
            ++pop_count_;
            *out = std::move(buffer_[index_]);
            return true;
        }
    }

    /// Remove the oldest element from the buffer.
    inline bool pop_front(T* out) {
        if (count_ == 0) {
            return false;
        } else {
            size_t const index = CircularArithmetic::minus(index_, count_, buffer_.size());
            count_ = detail::SaturatingCounter::decrement(count_, 0);
            ++pop_count_;
            *out = std::move(buffer_[index]);
            return true;
        }
    }

    inline void clear() {
        count_ = 0;
        index_ = 0;
    }

    /// Returns the newest element in the buffer.
    inline T const& back() const {
        size_t const index = CircularArithmetic::decrement(index_, buffer_.size());
        return buffer_[index];
    }
    inline T& back() {
        size_t const index = CircularArithmetic::decrement(index_, buffer_.size());
        return buffer_[index];
    }

    /// Returns the n-th element in the buffer starting from the newest element.
    inline T const& back(size_t const n) const {
        CHECK_RING_BUFFER(n < count());
        size_t const index = CircularArithmetic::minus(index_, n+1, buffer_.size());
        return buffer_[index];
    }
    inline T& back(size_t const n) {
        CHECK_RING_BUFFER(n < count());
        size_t const index = CircularArithmetic::minus(index_, n+1, buffer_.size());
        return buffer_[index];
    }

    /// Returns the oldest elements in the buffer.
    inline T const& front() const {
        size_t index = CircularArithmetic::minus(index_, count_, buffer_.size());
        return buffer_[index];
    }
    inline T& front() {
        size_t index = CircularArithmetic::minus(index_, count_, buffer_.size());
        return buffer_[index];
    }

    /// Returns the n-th element in the buffer starting from the oldest element.
    inline T const& front(size_t const n) const {
        CHECK_RING_BUFFER(n < count());
        size_t index = CircularArithmetic::minus(index_, count_-n, buffer_.size());
        return buffer_[index];
    }
    inline T& front(size_t const n) {
        CHECK_RING_BUFFER(n < count());
        size_t index = CircularArithmetic::minus(index_, count_-n, buffer_.size());
        return buffer_[index];
    }

    /// Returns the number of elements currently in the buffer.
    inline size_t count() const { return count_; }

    /// Returns size of the buffer including empty elements.
    inline size_t size() const { return buffer_.size(); }

    inline bool empty() const { return count_ == 0; }

    inline bool full() const { return count_ == buffer_.size(); }

    /// Returns underlying data.
    inline T * data() const { return buffer_.data(); }

private:
    size_t index_;
    size_t count_;
    size_t push_count_;
    size_t pop_count_;
    std::vector<T, Alloc> buffer_;

};

#endif //LIB_SRC_RING_BUFFER_HPP_
