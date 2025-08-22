#pragma once

#include <iostream>

/**
 * @brief A container implemented as a singly-linked list for storing values in sorted order
 * 
 * This container maintains elements in sorted order and provides forward iterators.
 * It's specifically designed to work as a children container for the trie implementation.
 * 
 * @tparam Val The value type to be stored (must be comparable)
 */
template <typename Val>
class bag {
public:
    /**
     * @brief Node structure for the linked list implementation
     */
    struct Node {
        Val trie;           /// The stored value
        Node* next;         /// Pointer to the next node

        /// Default constructor
        Node() : trie(), next(nullptr) {}
        
        /// Constructor with value
        explicit Node(const Val& tr) : trie(tr), next(nullptr) {}
    };

private:
    Node* m_head;  /// Pointer to the first node

public:
    // Constructors and Destructor
    /// Default constructor - creates empty bag
    bag() : m_head(nullptr) {}
    
    /// Destructor - cleans up all nodes
    ~bag() { clear(); }
    
    /// Copy constructor
    bag(const bag& other) : m_head(nullptr) {
        *this = other;
    }
    
    /// Move constructor
    bag(bag&& other) noexcept : m_head(other.m_head) {
        other.m_head = nullptr;
    }

    /// Public head
    Node* head() { return m_head; }

    /// Public const head
    const Node* head() const { return m_head; }

    // Assignment Operators
    /// Copy assignment operator
    bag& operator=(const bag& rhs) {
        if (this != &rhs) {
            clear();
            for (const auto& item : rhs) {
                Node* new_node = create_node(item);
                bag_append_sorted(new_node);
            }
        }
        return *this;
    }
    
    /// Move assignment operator
    bag& operator=(bag&& rhs) noexcept {
        if (this != &rhs) {
            clear();
            m_head = rhs.m_head;
            rhs.m_head = nullptr;
        }
        return *this;
    }

    // Comparison Operators
    /// Equality operator
    bool operator==(const bag& rhs) const {
        if (m_head == nullptr && rhs.m_head == nullptr) {
            return true;
        }
        if ((m_head == nullptr) != (rhs.m_head == nullptr)) {
            return false;
        }
        
        auto it = begin();
        auto r_it = rhs.begin();
        while (it != end() && r_it != rhs.end()) {
            if (!((*it).get_label() && (*r_it).get_label() && 
                  *(*it).get_label() == *(*r_it).get_label() && 
                  *it == *r_it)) {
                return false;
            }
            ++it;
            ++r_it;
        }
        return it == end() && r_it == rhs.end();
    }
    
    /// Inequality operator
    bool operator!=(const bag& rhs) const {
        return !(*this == rhs);
    }

    // Container Operations
    /**
     * @brief Prepends a node to the front of the list
     * @param new_node Node to prepend
     */
    void bag_prepend(Node* new_node) {
        if (!new_node) return;
        new_node->next = m_head;
        m_head = new_node;
    }

    /**
     * @brief Clears all nodes from the container
     */
    void clear() {
        while (m_head) {
            Node* temp = m_head;
            m_head = m_head->next;
            delete temp;
        }
    }

    /**
     * @brief Creates a new node with the given value
     * @param src Value to store in the node
     * @return Pointer to the newly created node
     */
    Node* create_node(const Val& src) {
        return new Node(src);
    }

    /**
     * @brief Deletes a node with the specified value
     * @param n Value to delete (must have get_label() method)
     */
    void delete_node(const Val& n) {
        if (!m_head || !n.get_label()) return;

        if (*n.get_label() == *m_head->trie.get_label()) {
            Node* temp = m_head;
            m_head = m_head->next;
            delete temp;
        } else {
            Node* pc = m_head;
            while (pc->next && *n.get_label() != *pc->next->trie.get_label()) {
                pc = pc->next;
            }
            if (pc->next) {
                Node* temp = pc->next;
                pc->next = pc->next->next;
                delete temp;
            }
        }
    }

    /**
     * @brief Checks if the container is empty
     * @return true if empty, false otherwise
     */
    bool empty() const {
        return m_head == nullptr;
    }

    // Iterator Implementations
    /**
     * @brief Forward iterator for bag container
     */
    struct bag_iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = Val;
        using pointer = Val*;
        using reference = Val&;

        explicit bag_iterator(Node* ptr) : m_ptr(ptr) {}

        reference operator*() { return m_ptr->trie; }
        pointer operator->() { return &(m_ptr->trie); }

        bag_iterator& operator++() {
            if (m_ptr) m_ptr = m_ptr->next;
            return *this;
        }

        bag_iterator operator++(int) {
            bag_iterator temp(m_ptr);
            if (m_ptr) m_ptr = m_ptr->next;
            return temp;
        }

        bool operator==(const bag_iterator& rhs) const {
            return m_ptr == rhs.m_ptr;
        }

        bool operator!=(const bag_iterator& rhs) const {
            return m_ptr != rhs.m_ptr;
        }

        Val& get_trie() { return m_ptr->trie; }

    private:
        Node* m_ptr;
    };

    /**
     * @brief Const forward iterator for bag container
     */
    struct const_bag_iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = const Val;
        using pointer = const Val*;
        using reference = const Val&;

        explicit const_bag_iterator(const Node* ptr) : m_ptr(ptr) {}

        reference operator*() const { return m_ptr->trie; }
        pointer operator->() const { return &(m_ptr->trie); }

        const_bag_iterator& operator++() {
            if (m_ptr) m_ptr = m_ptr->next;
            return *this;
        }

        const_bag_iterator operator++(int) {
            const_bag_iterator temp(m_ptr);
            if (m_ptr) m_ptr = m_ptr->next;
            return temp;
        }

        bool operator==(const const_bag_iterator& rhs) const {
            return m_ptr == rhs.m_ptr;
        }

        bool operator!=(const const_bag_iterator& rhs) const {
            return m_ptr != rhs.m_ptr;
        }

        const Val& get_trie() const { return m_ptr->trie; }

    private:
        const Node* m_ptr;
    };

    // Iterator Access Methods
    bag_iterator begin() { return bag_iterator(m_head); }
    bag_iterator end() { return bag_iterator(nullptr); }
    const_bag_iterator begin() const { return const_bag_iterator(m_head); }
    const_bag_iterator end() const { return const_bag_iterator(nullptr); }
    const_bag_iterator cbegin() const { return begin(); }
    const_bag_iterator cend() const { return end(); }

private:
    /**
     * @brief Appends a node in sorted order
     * @param new_node Node to append
     */
    void bag_append_sorted(Node* new_node) {
        if (!new_node) return;
        
        if (!m_head || !new_node->trie.get_label()) {
            bag_prepend(new_node);
            return;
        }
        
        if (!m_head->trie.get_label() || 
            *new_node->trie.get_label() < *m_head->trie.get_label()) {
            bag_prepend(new_node);
            return;
        }
        
        Node* current = m_head;
        while (current->next && 
               current->next->trie.get_label() &&
               *current->next->trie.get_label() < *new_node->trie.get_label()) {
            current = current->next;
        }
        
        new_node->next = current->next;
        current->next = new_node;
    }
};