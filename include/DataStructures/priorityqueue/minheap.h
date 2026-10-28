#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

template <typename T, typename Compare = std::less<T>>
class minheap {
private:
    std::vector<T> heap;
    Compare compare;

    void heapify_up(std::size_t index);
    void heapify_down(std::size_t index);

public:
    explicit minheap(int initial_capacity = 100, const Compare& comparator = Compare());

    void push(const T& value);
    void push(T&& value);
    T pop();
    T top() const;
    bool isEmpty() const;
    std::size_t getSize() const;
};

template <typename T, typename Compare>
void minheap<T, Compare>::heapify_up(std::size_t index) {
    while (index > 0) {
        std::size_t parent = (index - 1) / 2;
        if (!compare(heap[index], heap[parent])) {
            break;
        }
        std::swap(heap[parent], heap[index]);
        index = parent;
    }
}

template <typename T, typename Compare>
void minheap<T, Compare>::heapify_down(std::size_t index) {
    while (true) {
        std::size_t left = 2 * index + 1;
        std::size_t right = 2 * index + 2;
        std::size_t smallest = index;

        if (left < heap.size() && compare(heap[left], heap[smallest])) {
            smallest = left;
        }
        if (right < heap.size() && compare(heap[right], heap[smallest])) {
            smallest = right;
        }
        if (smallest == index) {
            break;
        }

        std::swap(heap[index], heap[smallest]);
        index = smallest;
    }
}

template <typename T, typename Compare>
minheap<T, Compare>::minheap(int initial_capacity, const Compare& comparator)
    : compare(comparator) {
    if (initial_capacity > 0) {
        heap.reserve(static_cast<std::size_t>(initial_capacity));
    }
}

template <typename T, typename Compare>
void minheap<T, Compare>::push(const T& value) {
    heap.push_back(value);
    heapify_up(heap.size() - 1);
}

template <typename T, typename Compare>
void minheap<T, Compare>::push(T&& value) {
    heap.push_back(std::move(value));
    heapify_up(heap.size() - 1);
}

template <typename T, typename Compare>
T minheap<T, Compare>::pop() {
    if (heap.empty()) {
        throw std::out_of_range("Cannot pop from an empty minheap");
    }

    T result = std::move(heap.front());
    if (heap.size() == 1) {
        heap.pop_back();
        return result;
    }

    heap.front() = std::move(heap.back());
    heap.pop_back();
    heapify_down(0);
    return result;
}

template <typename T, typename Compare>
T minheap<T, Compare>::top() const {
    if (heap.empty()) {
        throw std::out_of_range("Cannot read the top of an empty minheap");
    }
    return heap.front();
}

template <typename T, typename Compare>
bool minheap<T, Compare>::isEmpty() const {
    return heap.empty();
}

template <typename T, typename Compare>
std::size_t minheap<T, Compare>::getSize() const {
    return heap.size();
}

#endif
