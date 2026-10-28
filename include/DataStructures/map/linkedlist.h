#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <cstddef>
#include <utility>

template <typename T>
class linkedlist {
public:
    class Node {
        friend class linkedlist<T>;

    private:
        T data;
        Node* next;
        Node* prev;

        template <typename Value>
        Node(Value&& value, Node* next_node, Node* prev_node);

    public:
        const T& get_data() const;
        Node* get_next() const;
        Node* get_prev() const;
    };

private:
    Node* head;
    Node* tail;
    std::size_t size;
    void clear();

public:
    linkedlist();
    linkedlist(const linkedlist& other);
    ~linkedlist();
    linkedlist& operator=(const linkedlist& other);
    void swap(linkedlist& other) noexcept;
    bool isEmpty() const;
    std::size_t getSize() const;
    template <typename Value>
    void push(Value&& value);
    template <typename Predicate>
    void remove_if(Predicate predicate);
    Node* get_head() const;
    Node* get_tail() const;
};

template <typename T>
template <typename Value>
linkedlist<T>::Node::Node(Value&& value, Node* next_node, Node* prev_node)
    : data(std::forward<Value>(value)), next(next_node), prev(prev_node) {}

template <typename T>
const T& linkedlist<T>::Node::get_data() const {
    return data;
}

template <typename T>
typename linkedlist<T>::Node* linkedlist<T>::Node::get_next() const {
    return next;
}

template <typename T>
typename linkedlist<T>::Node* linkedlist<T>::Node::get_prev() const {
    return prev;
}

template <typename T>
void linkedlist<T>::clear() {
    while (tail != nullptr) {
        Node* current = tail;
        tail = tail->prev;
        delete current;
    }
    head = nullptr;
    size = 0;
}

template <typename T>
linkedlist<T>::linkedlist() : head(nullptr), tail(nullptr), size(0) {}

template <typename T>
linkedlist<T>::linkedlist(const linkedlist& other) : linkedlist() {
    for (Node* current = other.head; current != nullptr; current = current->next) {
        push(current->data);
    }
}

template <typename T>
linkedlist<T>::~linkedlist() {
    clear();
}

template <typename T>
linkedlist<T>& linkedlist<T>::operator=(const linkedlist& other) {
    if (this == &other) {
        return *this;
    }

    linkedlist copy(other);
    swap(copy);
    return *this;
}

template <typename T>
void linkedlist<T>::swap(linkedlist& other) noexcept {
    using std::swap;
    swap(head, other.head);
    swap(tail, other.tail);
    swap(size, other.size);
}

template <typename T>
bool linkedlist<T>::isEmpty() const {
    return size == 0;
}

template <typename T>
std::size_t linkedlist<T>::getSize() const {
    return size;
}

template <typename T>
template <typename Value>
void linkedlist<T>::push(Value&& value) {
    Node* node = new Node(std::forward<Value>(value), nullptr, tail);
    if (tail == nullptr) {
        head = node;
    } else {
        tail->next = node;
    }
    tail = node;
    ++size;
}

template <typename T>
template <typename Predicate>
void linkedlist<T>::remove_if(Predicate predicate) {
    Node* current = head;
    while (current != nullptr && !predicate(current->data)) {
        current = current->next;
    }
    if (current == nullptr) {
        return;
    }

    if (current->prev != nullptr) {
        current->prev->next = current->next;
    } else {
        head = current->next;
    }

    if (current->next != nullptr) {
        current->next->prev = current->prev;
    } else {
        tail = current->prev;
    }

    delete current;
    --size;
}

template <typename T>
typename linkedlist<T>::Node* linkedlist<T>::get_head() const {
    return head;
}

template <typename T>
typename linkedlist<T>::Node* linkedlist<T>::get_tail() const {
    return tail;
}

template <typename T>
void swap(linkedlist<T>& left, linkedlist<T>& right) noexcept {
    left.swap(right);
}

#endif
