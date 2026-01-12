#pragma once

#include "util/class.h"

template <typename T>
class LinkedList {
protected:
    struct Node;

public:
    PREVENT_COPY_ALLOW_MOVE(LinkedList);

    LinkedList() = default;

    virtual ~LinkedList() noexcept;

    T& Head();

    const T& Head() const;

    T& Tail();

    const T& Tail() const;

    void Push(T element) noexcept;

    void Pop();

    bool TryPop() noexcept;

    void Pop(unsigned int number);

    unsigned int TryPop(unsigned int number) noexcept;

    void PopTail();

    bool TryPopTail() noexcept;

    void Remove(unsigned int index, unsigned int number);

    unsigned int TryRemove(unsigned int index, unsigned int number) noexcept;

    T& Get(unsigned int index);

    const T& Get(unsigned int index) const;

    class Iterator {
    public:
        explicit Iterator(Node* node_ptr) noexcept;

        bool operator!=(const Iterator& other) const noexcept;

        T& operator*() noexcept;

        const T& operator*() const noexcept;

        Iterator& operator++() noexcept;

    private:
        Node* _node;
    };

    Iterator begin() const noexcept;

    Iterator end() const noexcept;

protected:
    struct Node {
        explicit Node(const T& value) noexcept;

        T value;
        Node* next = nullptr;
    };

    Node* _head = nullptr;
    Node* _tail = nullptr;

    void CutNode(Node*& node) noexcept;

    virtual void FreeNode(Node* node) const noexcept;
};

#include "linked_list_imp.h"
