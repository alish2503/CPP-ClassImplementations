#include <iostream>
#include <memory>
#include <cstddef>
#include <cassert>

template<typename T, std::size_t StackSize>
class StackAllocator {
public:
    using value_type = T;

    StackAllocator() : offset_(0) {}

    template<typename U>
    StackAllocator(const StackAllocator<U, StackSize>& other) : offset_(other.offset_) {}

    T* allocate(size_t n) {
        size_t bytes_needed = n * sizeof(T);
        if (offset_ + bytes_needed > StackSize) {
            throw std::bad_alloc();
        }
        T* ptr = reinterpret_cast<T*>(stack_memory_ + offset_);
        offset_ += bytes_needed;
        return ptr;
    }

    void deallocate(T* p, size_t n) {

    }

    template<typename U>
    struct rebind {
        using other = StackAllocator<U, StackSize>;
    };

private:
    template<typename, std::size_t>
    friend class StackAllocator;

    alignas(alignof(std::max_align_t)) char stack_memory_[StackSize]; 
    size_t offset_; 
};

template <typename T, typename Allocator = std::allocator<T>>
class List {
public:
    struct Node {
        T data;
        Node* next;
        Node* prev;

        Node() : next(nullptr), prev(nullptr) {}
        Node(const T& data) : data(data), next(nullptr), prev(nullptr) {}
        Node(T&& data) : data(std::move(data)), next(nullptr), prev(nullptr) {}
    };

    class Iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using pointer = T*;
        using reference = T&;

        Iterator(Node* node) : current(node) {}

        Node* getNode() const {
            return current;
        }

        reference operator*() const {
            return current->data;
        }

        pointer operator->() const {
            return &(current->data);
        }

        Iterator& operator++() {
            if (current) current = current->next;
            return *this;
        }

        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }

        Iterator& operator--() {
            if (current) current = current->prev;
            return *this;
        }

        Iterator operator--(int) {
            Iterator temp = *this;
            --(*this);
            return temp;
        }

        bool operator==(const Iterator& other) const {
            return current == other.current;
        }

        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }

    private:
        Node* current;
    };

    using iterator = Iterator;
    using traits = std::allocator_traits<Allocator>;
    using NodeAllocator = typename traits::rebind_alloc<Node>;
    using node_traits = typename traits::rebind_traits<Node>;
    
    List(const Allocator& alloc = Allocator()) : allocator(alloc), sz(0) {
        sentinel = node_traits::allocate(allocator, 1);
        node_traits::construct(allocator, sentinel);
        head = tail = sentinel;
        sentinel->next = sentinel->prev = nullptr;
    }

    List(std::initializer_list<T> init, const Allocator& alloc = Allocator()) : List(alloc) {
        for (const T& value : init) {
            push_back(value);
        }
    }
    template <typename U>
    List(size_t count, U&& value, const Allocator& alloc = Allocator()) : List(alloc) {
        for (size_t i = 0; i < count - 1; ++i) {
            push_back(value);
        }
        push_back(std::forward<U>(value));
    }

    List(const List& other) : List(other.allocator) {
        copy_from(other);
    }

    List(List&& other) noexcept 
        : allocator(std::move(other.allocator)), head(other.head), tail(other.tail), sentinel(other.sentinel), sz(other.sz) {
        other.head = other.tail = other.sentinel = nullptr;
        other.sz = 0;
    }

    List& operator=(const List& other) {
        if (this != &other) {
            clear();
            allocator = other.allocator;
            copy_from(other);
        }
        return *this;
    }

    List& operator=(List&& other) noexcept {
        if (this != &other) {
            clear();
            allocator = std::move(other.allocator);
            head = other.head;
            tail = other.tail;
            sentinel = other.sentinel;
            sz = other.sz;
            
            other.head = other.tail = other.sentinel = nullptr;
            other.sz = 0;
        }
        return *this;
    }

    ~List() {
        clear();
        if (sentinel) {
            node_traits::destroy(allocator, sentinel);
            node_traits::deallocate(allocator, sentinel, 1);
        }
    }

    iterator insert(iterator pos, const T& value) {
        Node* current = pos.getNode();

        if (current == head) {
            push_front(value);
            return begin();
        } else if (current == sentinel) {
            push_back(value);
            return iterator(tail);
        }

        Node* newNode = node_traits::allocate(allocator, 1);
        node_traits::construct(allocator, newNode, value);

        newNode->next = current;
        newNode->prev = current->prev;

        current->prev->next = newNode;
        current->prev = newNode;

        sz++;
        return iterator(newNode);
    }

    template <typename U>
    void push_back(U&& value) {
        Node* newNode = node_traits::allocate(allocator, 1);
        node_traits::construct(allocator, newNode, std::forward<U>(value));
        if (head == sentinel) {
            head = newNode;
            tail = newNode;
            sentinel->prev = newNode;
            newNode->next = sentinel;
            newNode->prev = sentinel;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            newNode->next = sentinel;
            tail = newNode;
            sentinel->prev = newNode;
        }
        sz++;
    }

    void pop_back() {
        if (head != sentinel) {
            Node* oldTail = tail;
            tail = tail->prev;
            if (tail != sentinel) {
                tail->next = sentinel;
            } else {
                head = sentinel;
            }
            sentinel->prev = tail;
            node_traits::destroy(allocator, oldTail);
            node_traits::deallocate(allocator, oldTail, 1);
            sz--;
        }
    }
    template <typename U>
    void push_front(U&& value) {
        Node* newNode = node_traits::allocate(allocator, 1);
        node_traits::construct(allocator, newNode, std::forward<U>(value));
        if (head == sentinel) {
            head = newNode;
            tail = newNode;
            sentinel->prev = newNode;
            newNode->next = sentinel;
            newNode->prev = sentinel;
        } else {
            newNode->next = head;
            newNode->prev = sentinel;
            head->prev = newNode;
            sentinel->next = newNode;
            head = newNode;
        }
        sz++;
    }
    void pop_front() {
        if (head != sentinel) {
            Node* oldHead = head;
            head = head->next;
            if (head != sentinel) {
                head->prev = sentinel;
            } else {
                sentinel->prev = nullptr;
                tail = sentinel;
            }
            sentinel->next = head;
            node_traits::destroy(allocator, oldHead);
            node_traits::deallocate(allocator, oldHead, 1);
            sz--;
        }
    }

    void print() const {
        Node* current = head;
        while (current != sentinel) {
            std::cout << current->data << " ";
            current = current->next;
        }
        std::cout << std::endl;
    }
    size_t size() const {
        return sz;
    }

    iterator begin() {
        return iterator(head);
    }

    iterator end() {
        return iterator(sentinel);
    }

private:
    Node* head;
    Node* tail;
    Node* sentinel;
    NodeAllocator allocator;
    size_t sz;

    void copy_from(const List& other) {
        Node* current = other.head;
        while (current != other.sentinel) {
            push_back(current->data);
            current = current->next;
        }
    }

    void clear() {
        while (head != sentinel) {
            pop_back();
        }
    }
};

int main() {
    List<int> list1;
    List<int> list2 = {1, 2, 3, 4, 5};
    List<int> list3(3, 42);
    List<int, StackAllocator<int, 1000>> list;

    list.push_back(1);
    list.push_back(2);
    list.push_back(3);

    std::cout << "List after push_back: ";
    list.print(); 

    list.pop_back();
    std::cout << "List after pop_back: ";
    list.print();  

    list.push_front(0);
    std::cout << "List after push_front: ";
    list.print();  

    list.pop_front();
    std::cout << "List after pop_front: ";
    list.print();  

    auto it = list.begin();
    ++it; 
    list.insert(it, 10);
    std::cout << "List after insert: ";
    list.print();  

    List<int, StackAllocator<int, 1000>> listCopy(list);
    std::cout << "ListCopy after copy: ";
    listCopy.print(); 
    return 0;
}



