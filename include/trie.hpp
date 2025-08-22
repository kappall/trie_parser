#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <string>

#include "bag.hpp"

/**
 * @brief Exception class for trie parsing errors
 */
struct parser_exception {
    explicit parser_exception(const std::string& str) : m_str(str) {}
    std::string what() const { return m_str; }
private:
    std::string m_str;
};

/**
 * @brief A generic trie (prefix tree) implementation
 * 
 * This trie implementation supports:
 * - Generic template types (must be comparable)
 * - Multiple iterator types (leaf and node iterators)
 * - Weighted nodes
 * - Path compression
 * - Trie union operations
 * - Stream I/O with custom parsing grammar
 * 
 * @tparam T The label type (must support <, ==, and << operators)
 */
template <typename T>
class trie {
public:
    // Forward declarations for iterator types
    struct node_iterator;
    struct const_node_iterator;
    struct leaf_iterator;
    struct const_leaf_iterator;

    /**
     * @brief Iterator for traversing from current node to root
     */
    struct node_iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        explicit node_iterator(trie<T>* ptr);
        reference operator*() const;
        pointer operator->() const;
        node_iterator& operator++();
        node_iterator operator++(int);
        bool operator==(const node_iterator& other) const;
        bool operator!=(const node_iterator& other) const;

    private:
        trie<T>* m_ptr;
    };

    /**
     * @brief Const iterator for traversing from current node to root
     */
    struct const_node_iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = const T;
        using pointer = const T*;
        using reference = const T&;

        explicit const_node_iterator(const trie<T>* ptr);
        reference operator*() const;
        pointer operator->() const;
        const_node_iterator& operator++();
        const_node_iterator operator++(int);
        bool operator==(const const_node_iterator& other) const;
        bool operator!=(const const_node_iterator& other) const;

    private:
        const trie<T>* m_ptr;
    };

    /**
     * @brief Iterator for traversing only leaf nodes
     */
    struct leaf_iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        explicit leaf_iterator(trie<T>* ptr);
        reference operator*() const;
        pointer operator->() const;
        leaf_iterator& operator++();
        leaf_iterator operator++(int);
        bool operator==(const leaf_iterator& other) const;
        bool operator!=(const leaf_iterator& other) const;
        
        /// Conversion to node_iterator
        operator node_iterator() const;
        
        /// Get reference to the leaf node
        trie<T>& get_leaf() const;

    private:
        trie<T>* m_ptr;
    };

    /**
     * @brief Const iterator for traversing only leaf nodes
     */
    struct const_leaf_iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = const T;
        using pointer = const T*;
        using reference = const T&;

        explicit const_leaf_iterator(const trie<T>* ptr);
        reference operator*() const;
        pointer operator->() const;
        const_leaf_iterator& operator++();
        const_leaf_iterator operator++(int);
        bool operator==(const const_leaf_iterator& other) const;
        bool operator!=(const const_leaf_iterator& other) const;
        
        /// Conversion to const_node_iterator
        operator const_node_iterator() const;
        
        /// Get const reference to the leaf node
        const trie<T>& get_leaf() const;

    private:
        const trie<T>* m_ptr;
    };

    // Constructors and Destructor
    /// Default constructor - creates empty trie
    trie();
    
    /// Constructor with weight
    explicit trie(double weight);

    /// Constructor with label
    explicit trie(T const& label);

    /// Constructor wuth label and weight
    explicit trie(T const& label, double weight);
    
    /// Copy constructor
    trie(const trie<T>& other);
    
    /// Move constructor
    trie(trie<T>&& other) noexcept;

    /// Destructor
    ~trie();

    // Assignment Operators
    /// Copy assignment operator
    trie<T>& operator=(const trie<T>& rhs);
    
    /// Move assignment operator
    trie<T>& operator=(trie<T>&& rhs) noexcept;

    // Setters
    /// Set the weight of this node
    void set_weight(double weight);
    
    /// Set the label of this node (creates a copy)
    void set_label(const T* label);
    
    /// Set the parent of this node (internal use)
    void set_parent(trie<T>* parent);
    
    /// Add a child node in sorted order
    void add_child(const trie<T>& child);

    // Getters
    /// Get the weight of this node
    double get_weight() const;
    
    /// Get the label of this node (const version)
    const T* get_label() const;
    
    /// Get the parent of this node (const version)
    const trie<T>* get_parent() const;
    
    /// Get children container (const version)
    const bag<trie<T>>& get_children() const;
    
    /// Get the label of this node (non-const version)
    T* get_label();
    
    /// Get the parent of this node (non-const version)
    trie<T>* get_parent();
    
    /// Get children container (non-const version)
    bag<trie<T>>& get_children();

    // Comparison Operators
    /// Equality comparison
    bool operator==(const trie<T>& other) const;
    
    /// Inequality comparison
    bool operator!=(const trie<T>& other) const;

    // Search Operations
    /**
     * @brief Prefix search - find subtrie matching the given path
     * @param path Vector of labels representing the path to search
     * @return Reference to the subtrie at the end of the path (or this if not found)
     */
    trie<T>& operator[](const std::vector<T>& path);
    
    /// Const version of prefix search
    const trie<T>& operator[](const std::vector<T>& path) const;

    // Optimization Operations
    /**
     * @brief Find the leaf node with maximum weight
     * @return Reference to the leaf with maximum weight
     */
    trie<T>& max();
    
    /// Const version of max weight search
    const trie<T>& max() const;

    // Iterator Methods
    /// Get iterator to first leaf
    leaf_iterator begin();
    
    /// Get end iterator for leaves
    leaf_iterator end();
    
    /// Get iterator starting from root
    node_iterator root();
    
    /// Get const iterator to first leaf
    const_leaf_iterator begin() const;
    
    /// Get const end iterator for leaves
    const_leaf_iterator end() const;
    
    /// Get const iterator starting from root
    const_node_iterator root() const;

    // Advanced Operations
    /**
     * @brief Union operation - combines two tries
     * @param other The trie to union with
     * @return A new trie containing the union of both tries
     */
    trie<T> operator+(const trie<T>& other) const;
    
    /**
     * @brief In-place union operation
     * @param other The trie to union with
     * @return Reference to this trie after union
     */
    trie<T>& operator+=(const trie<T>& other);

    /**
     * @brief Path compression optimization
     * 
     * Compresses paths where nodes have only one child by combining
     * the labels and removing intermediate nodes.
     */
    void path_compress();

    // Utility Methods
    /// Check if this is a leaf node (has no children)
    bool is_leaf() const { return m_children.empty(); }
    
    /// Check if this is the root node (has no parent)
    bool is_root() const { return m_parent == nullptr; }

private:
    trie<T>* m_parent;          /// Pointer to parent node (nullptr for root)
    T* m_label;                 /// Label of this node (nullptr for root)
    bag<trie<T>> m_children;    /// Children container
    double m_weight;            /// Weight of this node

    // Helper methods for iterator implementation
    trie<T>* find_next_leaf(trie<T>* current);
    const trie<T>* find_next_leaf(const trie<T>* current) const;
};

// Stream I/O Operators
/**
 * @brief Output operator for trie serialization
 * @param os Output stream
 * @param t Trie to output
 * @return Reference to the output stream
 */
template <typename T>
std::ostream& operator<<(std::ostream& os, const trie<T>& t);

/**
 * @brief Input operator for trie parsing
 * 
 * Parses trie from input stream using the grammar:
 * TREE -> LEAF | {BAG}
 * BAG -> TREE | TREE, BAG | LEAF | LEAF, BAG | ε
 * LEAF -> weight children = {}
 * 
 * @param is Input stream
 * @param t Trie to populate
 * @return Reference to the input stream
 */
template <typename T>
std::istream& operator>>(std::istream& is, trie<T>& t);

// Include implementation (since this is a template)
#include "trie.cpp"