#pragma once

#include "linked_list.h"

#include <stdexcept>

template <typename T>
LinkedList<T>::~LinkedList() noexcept {
    while (_head != nullptr) CutNode(_head);
}

template <typename T>
T& LinkedList<T>::Head() {
    if (_head == nullptr)
        throw std::runtime_error("LinkedList: Head() index out of range");
    return _head->value;
}

template <typename T>
const T& LinkedList<T>::Head() const {
    if (_head == nullptr)
        throw std::runtime_error("LinkedList: Head() index out of range");
    return _head->value;
}

template <typename T>
T& LinkedList<T>::Tail() {
    if (_tail == nullptr)
        throw std::runtime_error("LinkedList: Tail() index out of range");
    return _tail->value;
}

template <typename T>
const T& LinkedList<T>::Tail() const {
    if (_tail == nullptr)
        throw std::runtime_error("LinkedList: Tail() index out of range");
    return _tail->value;
}

template <typename T>
void LinkedList<T>::Push(T element) noexcept {
    /* If the list was empty */
    if (_head == nullptr) {
        _head = new Node(element);
        _tail = _head;
        return;
    }

    /* If the list was NOT empty, add a pointer of the new node to the last one
     * and replace the last node with the new one */
    _tail->next = new Node(element);
    _tail = _tail->next;
}

template <typename T>
void LinkedList<T>::Pop() {
    if (_head == nullptr)
        throw std::runtime_error("LinkedList: Pop() index out of range");
    CutNode(_head);
    if (_head == nullptr) _tail = nullptr;
}

template <typename T>
bool LinkedList<T>::TryPop() noexcept {
    if (_head == nullptr) return false;
    CutNode(_head);
    if (_head == nullptr) _tail = nullptr;
    return true;
}

template <typename T>
void LinkedList<T>::Pop(unsigned int number) {
    while (number-- > 0) {
        if (_head == nullptr) {
            _tail = nullptr;
            throw std::runtime_error(
                "LinkedList: Pop(unsigned int) index out of range"
            );
        }
        CutNode(_head);
    }
    if (_head == nullptr) _tail = nullptr;
}

template <typename T>
unsigned int LinkedList<T>::TryPop(const unsigned int number) noexcept {
    for (unsigned int i = 0; i < number; i++) {
        if (_head == nullptr) {
            _tail = nullptr;
            return i;
        }
        CutNode(_head);
    }
    if (_head == nullptr) _tail = nullptr;
    return number;
}

template <typename T>
void LinkedList<T>::Remove(unsigned int index, unsigned int number) {
    /* If we are deleting first elements */
    if (index == 0) {
        try {
            Pop(number);
        } catch (const std::runtime_error&) {
            throw std::runtime_error(
                "LinkedList: Remove(unsigned int, unsigned int) "
                "index out of range"
            );
        }
        return;
    }

    /* Get the element before the removable */
    Node* before_removable = _head;
    while (index-- > 1) {
        if (before_removable == nullptr) throw std::runtime_error(
                "LinkedList: Remove(unsigned int, unsigned int) "
                "index out of range"
            );
        before_removable = before_removable->next;
    }

    /* Remove elements */
    while (number-- > 0) {
      if ((before_removable == nullptr) || !before_removable->next)
        throw std::runtime_error(
            "LinkedList: Remove(unsigned int, unsigned int) "
            "index out of range");
      CutNode(before_removable->next);

      /* If we got the last element, update the tail */
      if (!before_removable->next)
        _tail = before_removable;
    }
}

template <typename T>
unsigned int LinkedList<T>::TryRemove(unsigned int index,
    unsigned int number) noexcept {
    /* If we are deleting first elements */
    if (index == 0) return TryPop(number);

    /* Get the element before the removable */
    Node* before_removable = _head;
    while (index-- > 1) {
        if (before_removable == nullptr) return 0;
        before_removable = before_removable->next;
    }

    /* A variable for store the result */
    unsigned int result = 0;

    /* Remove elements */
    while (number-- > 0) {
        if ((before_removable == nullptr) || !before_removable->next)
            return result;
        CutNode(before_removable->next);
        result++;

        /* If we get the last element, update the tail */
        if (!before_removable->next) _tail = before_removable;
    }

    /* If the head is nullptr, _tail must be nullptr too */
    if (_head == nullptr) _tail = nullptr;

    /* Return the result */
    return result;
}

template <typename T>
void LinkedList<T>::PopTail() {
    /* If the list is empty */
    if (_head == nullptr)
        throw std::runtime_error("LinkedList: PopTail() index out of range");

    /* If we have only one element */
    if (!_head->next) {
        CutNode(_head);
        _tail = nullptr;
        return;
    }

    /* Get the second last node */
    Node* second_last_node = _head;
    while (second_last_node->next->next)
        second_last_node = second_last_node->next;

    /* Change the tail */
    CutNode(second_last_node->next);
    _tail = second_last_node;
}

template <typename T>
bool LinkedList<T>::TryPopTail() noexcept {
    /* If the list is empty */
    if (_head == nullptr) return false;

    /* If we have only one element */
    if (!_head->next) {
        CutNode(_head);
        _tail = nullptr;
        return true;
    }

    /* Get the second last node */
    Node* second_last_node = _head;
    while (second_last_node->next->next)
        second_last_node = second_last_node->next;

    /* Change the tail */
    CutNode(second_last_node->next);
    _tail = second_last_node;
    return true;
}

template <typename T>
T& LinkedList<T>::Get(unsigned int index) {
    Node* node_ptr = _head;
    while (index-- > 0) {
        node_ptr = node_ptr->next;
        if (node_ptr == nullptr)
            throw std::runtime_error(
                "LinkedList: operator[](unsigned int) index out of range");
    }
    return node_ptr->value;
}

template <typename T>
const T& LinkedList<T>::Get(unsigned int index) const {
    Node* node_ptr = _head;
    while (index-- > 0) {
        node_ptr = node_ptr->next;
        if (node_ptr == nullptr)
            throw std::runtime_error(
                "LinkedList: operator[](unsigned int) index out of range");
    }
    return node_ptr->value;
}

template <typename T>
LinkedList<T>::Iterator LinkedList<T>::begin() const noexcept {
    return Iterator(_head);
}

template <typename T>
LinkedList<T>::Iterator LinkedList<T>::end() const noexcept {
    return Iterator(nullptr);
}

template <typename T>
LinkedList<T>::Node::Node(const T& value) noexcept : value(value) { }

template <typename T>
LinkedList<T>::Iterator::Iterator(Node* node_ptr) noexcept :
    _node(node_ptr) { }

template <typename T>
bool
LinkedList<T>::Iterator::operator!=(const Iterator& other) const noexcept {
    return _node != other._node;
}

template <typename T>
T&
LinkedList<T>::Iterator::operator*() noexcept { return _node->value; }

template <typename T>
const T&
LinkedList<T>::Iterator::operator*() const noexcept { return _node->value; }

template <typename T>
LinkedList<T>::Iterator& LinkedList<T>::Iterator::operator++() noexcept {
    _node = _node->next;
    return *this;
}

template <typename T>
void LinkedList<T>::CutNode(Node*& node) noexcept {
    Node* const next_node = node->next;
    FreeNode(node);
    node = next_node;
}

template <typename T>
void LinkedList<T>::FreeNode(Node* const node) const noexcept {
    delete node;
}
