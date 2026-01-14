#pragma once

#include "dictionary.h"
#include "util/hash.h"
#include <netinet/in.h>

template <typename K, typename T>
Dictionary<K, T>::Dictionary(const unsigned short capacity) noexcept :
        _buckets(new LinkedList<Node>[capacity]), _capacity(capacity) { }

template <typename K, typename T>
Dictionary<K, T>::~Dictionary() noexcept { delete[] _buckets; }

template <typename K, typename T>
void Dictionary<K, T>::Put(const K& key, T element) {
    /* Calculate the key hash */
    const unsigned short hash = Hash::Get(key) % _capacity;

    /* Try to get the node from the linked list */
    for (const Node& node : _buckets[hash])
        if (Equal(node.key, key))
            throw std::runtime_error(
                "Dictionary: Put() an element with such a key already exists"
            );

    /* If there is no an element with such a key yet,
     * put the node to the one of the buckets, according to the hash */
    _buckets[hash].Push({ key, element });
}

template <typename K, typename T>
T& Dictionary<K, T>::Get(const K& key) {
    /* Calculate the key hash */
    const unsigned short hash = Hash::Get(key) % _capacity;

    /* Try to get the node from the linked list */
    for (Node& node : _buckets[hash])
        if (Equal(node.key, key))
            return node.element;

    /* If there is NOT an element in the linked list, throw an error */
    throw std::runtime_error("Dictionary::Get() no such an element");
}

template <typename K, typename T>
const T& Dictionary<K, T>::Get(const K& key) const {
    /* Calculate the key hash */
    const unsigned short hash = Hash::Get(key) % _capacity;

    /* Try to get the node from the linked list */
    for (const Node& node : _buckets[hash])
        if (Equal(node.key, key))
            return node.element;

    /* If there is NOT an element in the linked list, throw an error */
    throw std::runtime_error("Dictionary::Get() no such an element");
}

template <typename K, typename T>
bool Dictionary<K, T>::Has(const K& key) const noexcept {
    /* Calculate the key hash */
    const unsigned short hash = Hash::Get(key) % _capacity;

    /* Try to get the node from the linked list */
    for (const Node& node : _buckets[hash])
        if (Equal(node.key, key))
            return true;

    /* If there is NOT an element in the linked list, return false */
    return false;
}

template <typename K, typename T>
Dictionary<K, T>::Iterator Dictionary<K, T>::begin() const noexcept {
    /* Iterate over all buckets */
    for (unsigned short i = 0; i < _capacity; ++i)
        /* If the current bucket is not empty,
         * return iterator to its first element */
        if (_buckets[i].begin() != _buckets[i].end())
            return Iterator(i, _capacity, _buckets, _buckets[i].begin());

    /* If all buckets are empty, return end() iterator */
    return end();
}

template <typename K, typename T>
Dictionary<K, T>::Iterator Dictionary<K, T>::end() const noexcept {
    /* Construct an iterator representing the end position:
     * index == capacity and internal list iterator == nullptr */
    return Iterator(_capacity, _capacity, _buckets,
                    typename LinkedList<Node>::Iterator(nullptr));
}

template <typename K, typename T>
bool Dictionary<K, T>::Equal(const K& key1, const K& key2) noexcept {
    if constexpr (std::is_same_v<K, const char*>)
        return strcmp(key1, key2) == 0;
    else if constexpr (std::is_same_v<K, sockaddr_in>)
        return ((sockaddr_in)key1).sin_port == ((sockaddr_in)key2).sin_port &&
               ((sockaddr_in)key1).sin_addr.s_addr ==
               ((sockaddr_in)key2).sin_addr.s_addr;
    else
        return key1 == key2;
}

template <typename K, typename T>
Dictionary<K, T>::Iterator::Iterator(
    const unsigned short index,
    const unsigned short capacity,
    const LinkedList<Node>* const lists,
    const typename LinkedList<Node>::Iterator iterator
) noexcept : _index(index), _capacity(capacity),
             _buckets(lists), _iterator(iterator) { }

template <typename K, typename T>
bool Dictionary<K, T>::
Iterator::operator!=(const Iterator& other) const noexcept {
    /* If both iterators are "end" iterators, they are equal */
    if (_index == _capacity && other._index == other._capacity) return false;

    /* If they point to different buckets, they are not equal */
    if (_index != other._index) return true;

    /* Otherwise compare underlying list iterators */
    return _iterator != other._iterator;
}

template <typename K, typename T>
Dictionary<K, T>::Node&
Dictionary<K, T>::Iterator::operator*() noexcept { return *_iterator; }

template <typename K, typename T>
const Dictionary<K, T>::Node&
Dictionary<K, T>::Iterator::operator*() const noexcept { return *_iterator; }

template <typename K, typename T>
Dictionary<K, T>::Iterator&
Dictionary<K, T>::Iterator::operator++() noexcept {
    /* If iterator is already at end(), do nothing */
    if (_index == _capacity) return *this;

    /* If still inside a non-empty bucket, stop here */
    if (++_iterator != _buckets[_index].end()) return *this;

    /* Skip empty buckets until a non-empty one is found */
    while (++_index < _capacity) {
        if (_buckets[_index].begin() != _buckets[_index].end()) {
            _iterator = _buckets[_index].begin();
            return *this;
        }
    }

    /* If no more buckets contain elements, set iterator to end() */
    _index = _capacity;
    _iterator = typename LinkedList<Node>::Iterator(nullptr);
    return *this;
}
