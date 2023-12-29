# Ring Buffer
Simple implementation of a ring buffer.

## Usage
```C++
RingBuffer<size_t> ringbuffer{5};
ringbuffer.push_back(123);

size_t pop;
ringbuffer.pop_back(&pop);

assert(pop == 123);


ringbuffer.push_back(1);
ringbuffer.push_back(2);
ringbuffer.push_back(3);
ringbuffer.push_back(4);
ringbuffer.push_back(5);

assert(ringbuffer.front() == 1);
assert(ringbuffer.back() == 5);

ringbuffer.push_back(6);

assert(ringbuffer.front() == 2);
assert(ringbuffer.back() == 6);

```
