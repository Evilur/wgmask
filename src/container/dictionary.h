#pragma once

#include "linked_list.h"
#include "util/class.h"

/**
 * Simple minimalistic implementation of hash map
 * @author Evilur <the.evilur@gmail.com>
 * @tparam K Key typename
 * @tparam T Element typename
 */
template <typename K, typename T>
class Dictionary final {
struct Node;
public:
    PREVENT_COPY_ALLOW_MOVE(Dictionary);

    /**
     * @param capacity Maximum number of elements without resizing
     */
    explicit Dictionary(unsigned short capacity) noexcept;

    /**
     * Free the memory
     */
    ~Dictionary() noexcept;

    /**
     * Put an element into the hash map
     * @param key The key that can be used to retrieve the element
     * @param element Element to put into the map
     * @throw std::runtime_error If there is already an element with such a key
     */
    void Put(const K& key, T element);

    /**
     * Get the element from the hash map by the key
     * @param key The key to get the element by
     * @throw std::runtime_error If there is no an element with such a key
     * @return Element with such a key, nullptr if there is no such element
     */
    T& Get(const K& key);

    /**
     * Get the element from the hash map by the key
     * @param key The key to get the element by
     * @throw std::runtime_error If there is no an element with such a key
     * @return Element with such a key, nullptr if there is no such element
     */
    const T& Get(const K& key) const;

    /**
     * Check the existence of the element in the hash map
     * @param key The key to check the element by
     * @return true if the element exists, false otherwise
     */
    bool Has(const K& key) const noexcept;

    /**
     * Iterator to go through the hash map
     */
    class Iterator {
    public:
        explicit Iterator(unsigned short index,
                          unsigned short capacity,
                          const LinkedList<Node>* lists,
                          LinkedList<Node>::Iterator iterator) noexcept;

        bool operator!=(const Iterator& other) const noexcept;

        Node& operator*() noexcept;

        const Node& operator*() const noexcept;

        Iterator& operator++() noexcept;

    private:
        /**
         * Index of the current bucket in the hash table
         */
        unsigned short _index;
        /**
         * Total number of buckets in the hash table
         */
        const unsigned short _capacity;
        /**
         * Pointer to the array of linked lists (buckets)
         * that make up the hash table
         */
        const LinkedList<Node>* const _buckets;
        /**
         * Iterator pointing to the current node
         * inside the current bucket's linked list
         */
        LinkedList<Node>::Iterator _iterator;
    };

    /**
     * Get the iterator for the first element
     * @return iterator for the first element
     */
    Iterator begin() const noexcept;

    /**
     * Get the iterator after the last element
     * @return iterator after the last element
     */
    Iterator end() const noexcept;

private:
    /**
     * Node to keep the element and its key in the linked list
     */
    struct Node {
        K key;
        T element;
    };

    /**
     * A pointer to the dynamic array with linked lists for resolving collisions
     */
    LinkedList<Node>* const _buckets;

    /**
     * A size of the array with linked lists
     */
    const unsigned short _capacity;

    /**
     * Compare two keys
     * @return true if the keys are equal, otherwise - false
     */
    static bool Equal(const K& key1, const K& key2) noexcept;
};

#include "dictionary_imp.h"
