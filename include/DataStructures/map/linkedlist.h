#ifndef LINKED_LIST_H
#define LINKED_LIST_H

template <typename T>
class linkedlist {
    public:
        struct Node {
            friend class linkedlist<T>;

            private:
                T data;
                Node* next;
                Node* prev;

                Node(const T& value, Node* next_node, Node* prev_node);

            public:
                const T& get_data() const;
                Node* get_next() const;
                Node* get_prev() const;
            };

    private:
        Node* head;
        Node* tail;
        int size;
        void clear();

    public:
        linkedlist();
        linkedlist(const linkedlist& other);
        ~linkedlist();
        linkedlist& operator=(const linkedlist& other);
        void swap(linkedlist& other);
        bool isEmpty() const;
        int getSize() const;
        void push(const T& value);
        template <typename Predicate>
        void remove_if(Predicate predicate);
        Node* get_head() const;
        Node* get_tail() const;
};

// Định nghĩa các phương thức của Node //
// Khởi tạo node
template <typename T>
linkedlist<T>::Node::Node(const T& value, Node* next_node, Node* prev_node)
    : data(value), next(next_node), prev(prev_node) {}
// Getter
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

// Định nghĩa các phương thức của Linkedlist //
// Xóa Linkedlist
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

// Khởi tạo Linkedlist
template <typename T>
linkedlist<T>::linkedlist() : head(nullptr), tail(nullptr), size(0) {}

// Copy constructor / Hàm dựng sao chép của Linkedlist
template <typename T>
linkedlist<T>::linkedlist(const linkedlist& other) : linkedlist() {
    for (Node* current = other.head; current != nullptr; current = current->next) {
        push(current->data);
    }
}

// Destructor
template <typename T>
linkedlist<T>::~linkedlist() {
    clear();
}

//
template <typename T>
void linkedlist<T>::swap(linkedlist& other) {
    Node* tempHead = head;
    head = other.head;
    other.head = tempHead;

    Node* tempTail = tail;
    tail = other.tail;
    other.tail = tempTail;

    int tempSize = size;
    size = other.size;
    other.size = tempSize;
}

// Toán tử = của Linkedlist
template <typename T>
linkedlist<T>& linkedlist<T>::operator=(const linkedlist& other) {
    if (this == &other) {
        return *this;
    }

    linkedlist copy(other);
    swap(copy);
    return *this;
}


// Kiểm tra Linkedlist rỗng
template <typename T>
bool linkedlist<T>::isEmpty() const {
    return size == 0;
}

// Getter kích thước Linkedlist
template <typename T>
int linkedlist<T>::getSize() const {
    return size;
}

// Thêm phần tử vào cuối Linkedlist
template <typename T>
void linkedlist<T>::push(const T& value) {
    Node* node = new Node(value, nullptr, tail);
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

//Getter head Linkedlist
template <typename T>
typename linkedlist<T>::Node* linkedlist<T>::get_head() const {
    return head;
}

//Getter tail Linkedlist
template <typename T>
typename linkedlist<T>::Node* linkedlist<T>::get_tail() const {
    return tail;
}


#endif
