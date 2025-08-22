#pragma once

// This file contains the template implementation for trie.hpp
// Include this file only once through trie.hpp

#include <iostream>
#include <string>
#include <stdexcept>
#include "trie.hpp"

// Forward declarations for parsing functions
template <typename T>
trie<T> parse_child(std::istream& is);

template <typename T>
trie<T> parse_tree(std::istream& is);

template <typename T>
trie<T> parse_leaf(std::istream& is);

/**
 * @brief Skip whitespace characters in input stream
 */
inline void skip_whitespace(std::istream& is) {
    char c;
    is >> c;
    is.putback(c);
}

// Parser implementation
template <typename T>
trie<T> parse_leaf(std::istream& is) {
    skip_whitespace(is);

    double weight = 0.0;
    is >> weight;
    trie<T> leaf(weight);

    skip_whitespace(is);

    std::string keyword;
    is >> keyword;
    if (keyword != "children") {
        throw parser_exception("Expected keyword 'children', got '" + keyword + "'");
    }

    char equals_sign;
    is >> equals_sign;
    if (equals_sign != '=') {
        throw parser_exception("Expected '=' after 'children'");
    }

    skip_whitespace(is);
    
    char open_brace;
    is >> open_brace;
    if (open_brace != '{') {
        throw parser_exception("Expected '{' after 'children ='");
    }

    skip_whitespace(is);
    
    char close_brace;
    is >> close_brace;
    if (close_brace != '}') {
        throw parser_exception("Expected '}' for empty children");
    }

    return leaf;
}

template <typename T>
trie<T> parse_child(std::istream& is) {
    skip_whitespace(is);

    T label;
    is >> label;

    skip_whitespace(is);
    
    // Check if next character is a digit (leaf) or letter (subtree)
    int next_int = is.peek();
    if (next_int == EOF) throw parser_exception("Unexpected end of input");
    char next_char = static_cast<char>(next_int);
    trie<T> child;
    
    if ((next_char >= '0' && next_char <= '9') || next_char == '-') {
        // It's a leaf node
        child = parse_leaf<T>(is);
    } else {
        // It's an internal node
        child = parse_tree<T>(is);
    }
    
    child.set_label(&label);
    return child;
}

template <typename T>
trie<T> parse_tree(std::istream& is) {
    skip_whitespace(is);

    trie<T> tree;
    int first_int = is.peek();
    if (first_int == EOF) throw parser_exception("Unexpected end of input");
    char first_char = static_cast<char>(first_int);

    if (first_char == '-' || (first_char >= '0' && first_char <= '9')) {
        // It's a leaf (weight starts with digit or minus)
        return parse_leaf<T>(is);
    }

    std::string keyword;
    is >> keyword;
    if (keyword != "children") {
        throw parser_exception("Expected keyword 'children', got '" + keyword + "'");
    }

    skip_whitespace(is);

    char equals_sign;
    is >> equals_sign;
    if (equals_sign != '=') {
        throw parser_exception("Expected '=' after 'children'");
    }

    skip_whitespace(is);

    char open_brace;
    is >> open_brace;
    if (open_brace != '{') {
        throw parser_exception("Expected '{' after 'children ='");
    }

    skip_whitespace(is);
    
    int next_int = is.peek();
    if (next_int == EOF) throw parser_exception("Unexpected end of input");
    char next_char = static_cast<char>(next_int);
    bool has_children = false;
    
    while (next_char != '}') {
        has_children = true;
        tree.add_child(parse_child<T>(is));
        skip_whitespace(is);
        
        char separator;
        is >> separator;
        
        if (separator != '}' && separator != ',') {
            throw parser_exception("Expected '}' or ',' after child");
        }
        
        if (separator == '}') {
            break;
        }
        
        skip_whitespace(is);
        next_int = is.peek();
        if (next_int == EOF) throw parser_exception("Unexpected end of input");
        next_char = static_cast<char>(next_int);
    }

    if (!has_children) {
        throw parser_exception("Expected at least one child or weight");
    }

    return tree;
}

// Trie Implementation

// Constructors
template <typename T>
trie<T>::trie(const trie<T>& other) : m_parent(nullptr), m_label(nullptr), m_children(), m_weight(other.m_weight) {
    if (other.m_label) {
        m_label = new T(*other.m_label);
    }
    for (const auto& child : other.m_children) {
        add_child(child);
    }
}

template <typename T>
trie<T>::trie(trie<T>&& other) noexcept : m_parent(nullptr), m_label(other.m_label), m_children(std::move(other.m_children)), m_weight(other.m_weight) {
    other.m_label = nullptr;
    
    // Update parent pointers of all children
    for (auto& child : m_children) {
        child.m_parent = this;
    }
}

// Destructor
template <typename T>
trie<T>::~trie() {
    delete m_label;
}

// Assignment Operators
template <typename T>
trie<T>& trie<T>::operator=(const trie<T>& rhs) {
    if (this != &rhs) {
        // Clean up current state
        delete m_label;
        m_label = nullptr;
        m_children.clear();
        
        // Copy from rhs
        m_weight = rhs.m_weight;
        if (rhs.m_label) {
            m_label = new T(*rhs.m_label);
        }
        
        for (const auto& child : rhs.m_children) {
            add_child(child);
        }
    }
    return *this;
}

template <typename T>
trie<T>& trie<T>::operator=(trie<T>&& rhs) noexcept {
    if (this != &rhs) {
        // Clean up current state
        delete m_label;
        m_children.clear();
        
        // Move from rhs
        m_label = rhs.m_label;
        m_children = std::move(rhs.m_children);
        m_weight = rhs.m_weight;
        
        rhs.m_label = nullptr;
        
        // Update parent pointers
        for (auto& child : m_children) {
            child.m_parent = this;
        }
    }
    return *this;
}

// Setters
template <typename T>
void trie<T>::set_weight(double weight) {
    m_weight = weight;
}

template <typename T>
void trie<T>::set_label(const T* label) {
    delete m_label;
    m_label = label ? new T(*label) : nullptr;
}

template <typename T>
void trie<T>::set_parent(trie<T>* parent) {
    m_parent = parent;
}

// Getters
template <typename T>
double trie<T>::get_weight() const {
    return m_weight;
}

template <typename T>
const T* trie<T>::get_label() const {
    return m_label;
}

template <typename T>
const trie<T>* trie<T>::get_parent() const {
    return m_parent;
}

template <typename T>
const bag<trie<T>>& trie<T>::get_children() const {
    return m_children;
}

template <typename T>
T* trie<T>::get_label() {
    return m_label;
}

template <typename T>
trie<T>* trie<T>::get_parent() {
    return m_parent;
}

template <typename T>
bag<trie<T>>& trie<T>::get_children() {
    return m_children;
}

// Node Iterator Implementation
template <typename T>
trie<T>::node_iterator::node_iterator(trie<T>* ptr) : m_ptr(ptr) {}

template <typename T>
typename trie<T>::node_iterator::reference trie<T>::node_iterator::operator*() const {
    if (!m_ptr || !m_ptr->m_label) {
        throw std::runtime_error("Dereferencing invalid node iterator");
    }
    return *m_ptr->m_label;
}

template <typename T>
typename trie<T>::node_iterator::pointer trie<T>::node_iterator::operator->() const {
    if (!m_ptr) {
        throw std::runtime_error("Accessing invalid node iterator");
    }
    return m_ptr->m_label;
}

template <typename T>
typename trie<T>::node_iterator& trie<T>::node_iterator::operator++() {
    if (m_ptr && m_ptr->m_parent) {
        m_ptr = m_ptr->m_parent;
    } else {
        m_ptr = nullptr;
    }
    return *this;
}

template <typename T>
typename trie<T>::node_iterator trie<T>::node_iterator::operator++(int) {
    node_iterator temp(m_ptr);
    ++(*this);
    return temp;
}

template <typename T>
bool trie<T>::node_iterator::operator==(const node_iterator& other) const {
    return m_ptr == other.m_ptr;
}

template <typename T>
bool trie<T>::node_iterator::operator!=(const node_iterator& other) const {
    return m_ptr != other.m_ptr;
}

// Const Node Iterator Implementation
template <typename T>
trie<T>::const_node_iterator::const_node_iterator(const trie<T>* ptr) : m_ptr(ptr) {}

template <typename T>
typename trie<T>::const_node_iterator::reference trie<T>::const_node_iterator::operator*() const {
    if (!m_ptr || !m_ptr->m_label) {
        throw std::runtime_error("Dereferencing invalid const node iterator");
    }
    return *m_ptr->m_label;
}

template <typename T>
typename trie<T>::const_node_iterator::pointer trie<T>::const_node_iterator::operator->() const {
    if (!m_ptr) {
        throw std::runtime_error("Accessing invalid const node iterator");
    }
    return m_ptr->m_label;
}

template <typename T>
typename trie<T>::const_node_iterator& trie<T>::const_node_iterator::operator++() {
    if (m_ptr && m_ptr->m_parent) {
        m_ptr = m_ptr->m_parent;
    } else {
        m_ptr = nullptr;
    }
    return *this;
}

template <typename T>
typename trie<T>::const_node_iterator trie<T>::const_node_iterator::operator++(int) {
    const_node_iterator temp(m_ptr);
    ++(*this);
    return temp;
}

template <typename T>
bool trie<T>::const_node_iterator::operator==(const const_node_iterator& other) const {
    return m_ptr == other.m_ptr;
}

template <typename T>
bool trie<T>::const_node_iterator::operator!=(const const_node_iterator& other) const {
    return m_ptr != other.m_ptr;
}

// Leaf Iterator Implementation
template <typename T>
trie<T>::leaf_iterator::leaf_iterator(trie<T>* ptr) : m_ptr(ptr) {}

template <typename T>
typename trie<T>::leaf_iterator::reference trie<T>::leaf_iterator::operator*() const {
    if (!m_ptr || !m_ptr->m_label) {
        throw std::runtime_error("Dereferencing invalid leaf iterator");
    }
    return *m_ptr->m_label;
}

template <typename T>
typename trie<T>::leaf_iterator::pointer trie<T>::leaf_iterator::operator->() const {
    if (!m_ptr) {
        throw std::runtime_error("Accessing invalid leaf iterator");
    }
    return m_ptr->m_label;
}

template <typename T>
typename trie<T>::leaf_iterator& trie<T>::leaf_iterator::operator++() {
    if (!m_ptr) return *this;
    
    if (m_ptr->m_parent) {
        // Find current node in parent's children
        auto parent_children = &(m_ptr->m_parent->m_children);
        auto it = parent_children->begin();
        
        // Find current node
        while (it != parent_children->end() && &(it.get_trie()) != m_ptr) {
            ++it;
        }
        
        // Move to next sibling
        if (it != parent_children->end()) {
            ++it;
            if (it != parent_children->end()) {
                m_ptr = &(it.get_trie());
                // Go to leftmost leaf of this subtree
                while (!m_ptr->m_children.empty()) {
                    m_ptr = &(m_ptr->m_children.begin().get_trie());
                }
                return *this;
            }
        }
        
        // No more siblings, go up to parent
        if (m_ptr->m_parent->m_parent) {
            m_ptr = m_ptr->m_parent;
            ++(*this);
        } else {
            m_ptr = nullptr;
        }
    } else {
        m_ptr = nullptr;
    }
    
    return *this;
}

template <typename T>
typename trie<T>::leaf_iterator trie<T>::leaf_iterator::operator++(int) {
    leaf_iterator temp(m_ptr);
    ++(*this);
    return temp;
}

template <typename T>
bool trie<T>::leaf_iterator::operator==(const leaf_iterator& other) const {
    return m_ptr == other.m_ptr;
}

template <typename T>
bool trie<T>::leaf_iterator::operator!=(const leaf_iterator& other) const {
    return m_ptr != other.m_ptr;
}

template <typename T>
trie<T>::leaf_iterator::operator node_iterator() const {
    return node_iterator(m_ptr);
}

template <typename T>
trie<T>& trie<T>::leaf_iterator::get_leaf() const {
    if (!m_ptr) {
        throw std::runtime_error("Getting leaf from invalid iterator");
    }
    return *m_ptr;
}

// Const Leaf Iterator Implementation
template <typename T>
trie<T>::const_leaf_iterator::const_leaf_iterator(const trie<T>* ptr) : m_ptr(ptr) {}

template <typename T>
typename trie<T>::const_leaf_iterator::reference trie<T>::const_leaf_iterator::operator*() const {
    if (!m_ptr || !m_ptr->m_label) {
        throw std::runtime_error("Dereferencing invalid const leaf iterator");
    }
    return *m_ptr->m_label;
}

template <typename T>
typename trie<T>::const_leaf_iterator::pointer trie<T>::const_leaf_iterator::operator->() const {
    if (!m_ptr) {
        throw std::runtime_error("Accessing invalid const leaf iterator");
    }
    return m_ptr->m_label;
}

template <typename T>
typename trie<T>::const_leaf_iterator& trie<T>::const_leaf_iterator::operator++() {
    if (!m_ptr) return *this;
    
    if (m_ptr->m_parent) {
        // Find current node in parent's children
        const auto& parent_children = m_ptr->m_parent->m_children;
        auto it = parent_children.begin();
        
        // Find current node
        while (it != parent_children.end() && &(it.get_trie()) != m_ptr) {
            ++it;
        }
        
        // Move to next sibling
        if (it != parent_children.end()) {
            ++it;
            if (it != parent_children.end()) {
                m_ptr = &(it.get_trie());
                // Go to leftmost leaf of this subtree
                while (!m_ptr->m_children.empty()) {
                    m_ptr = &(m_ptr->m_children.begin().get_trie());
                }
                return *this;
            }
        }
        
        // No more siblings, go up to parent
        if (m_ptr->m_parent->m_parent) {
            m_ptr = m_ptr->m_parent;
            ++(*this);
        } else {
            m_ptr = nullptr;
        }
    } else {
        m_ptr = nullptr;
    }
    
    return *this;
}

template <typename T>
typename trie<T>::const_leaf_iterator trie<T>::const_leaf_iterator::operator++(int) {
    const_leaf_iterator temp(m_ptr);
    ++(*this);
    return temp;
}

template <typename T>
bool trie<T>::const_leaf_iterator::operator==(const const_leaf_iterator& other) const {
    return m_ptr == other.m_ptr;
}

template <typename T>
bool trie<T>::const_leaf_iterator::operator!=(const const_leaf_iterator& other) const {
    return m_ptr != other.m_ptr;
}

template <typename T>
trie<T>::const_leaf_iterator::operator const_node_iterator() const {
    return const_node_iterator(m_ptr);
}

template <typename T>
const trie<T>& trie<T>::const_leaf_iterator::get_leaf() const {
    if (!m_ptr) {
        throw std::runtime_error("Getting leaf from invalid const iterator");
    }
    return *m_ptr;
}

// Iterator Access Methods
template <typename T>
typename trie<T>::leaf_iterator trie<T>::begin() {
    if (m_children.empty()) {
        return leaf_iterator(nullptr);
    }
    
    // Find leftmost leaf
    trie<T>* current = &(m_children.begin().get_trie());
    while (!current->m_children.empty()) {
        current = &(current->m_children.begin().get_trie());
    }
    return leaf_iterator(current);
}

template <typename T>
typename trie<T>::leaf_iterator trie<T>::end() {
    return leaf_iterator(nullptr);
}

template <typename T>
typename trie<T>::node_iterator trie<T>::root() {
    return node_iterator(this);
}

template <typename T>
typename trie<T>::const_leaf_iterator trie<T>::begin() const {
    if (m_children.empty()) {
        return const_leaf_iterator(nullptr);
    }
    
    // Find leftmost leaf
    const trie<T>* current = &(m_children.begin().get_trie());
    while (!current->m_children.empty()) {
        current = &(current->m_children.begin().get_trie());
    }
    return const_leaf_iterator(current);
}

template <typename T>
typename trie<T>::const_leaf_iterator trie<T>::end() const {
    return const_leaf_iterator(nullptr);
}

template <typename T>
typename trie<T>::const_node_iterator trie<T>::root() const {
    return const_node_iterator(this);
}

// Core Methods
template <typename T>
void trie<T>::add_child(const trie<T>& child) {
    if (!child.m_label) {
        throw std::invalid_argument("Child must have a label");
    }
    
    // Check for duplicate labels
    for (const auto& existing_child : m_children) {
        if (existing_child.m_label && *existing_child.m_label == *child.m_label) {
            throw parser_exception("Children with the same parent cannot have identical labels");
        }
    }
    
    auto node = m_children.create_node(child);
    
    // Insert in sorted order
    if (m_children.empty()) {
        m_children.bag_prepend(node);
    } else {
        auto head = m_children.begin();
        if (*child.m_label < *head->m_label) {
            m_children.bag_prepend(node);
        } else {
            // Find insertion point
            typename bag<trie<T>>::Node* prev = nullptr;
            typename bag<trie<T>>::Node* current = m_children.head();
            
            while (current && current->trie.m_label && *current->trie.m_label < *child.m_label) {
                prev = current;
                current = current->next;
            }
            
            if (prev) {
                node->next = prev->next;
                prev->next = node;
            } else {
                m_children.bag_prepend(node);
            }
        }
    }
    
    // Set up parent-child relationship
    node->trie.m_parent = this;
    node->trie.set_label(child.m_label);
    
    // If this was a leaf, it's no longer a leaf
    if (m_children.begin() != m_children.end()) {
        m_weight = 0.0;
    }
}

template <typename T>
trie<T>& trie<T>::max() {
    if (m_children.empty()) {
        return *this;
    }
    
    double max_weight = std::numeric_limits<double>::lowest();
    trie<T>* max_leaf = nullptr;
    
    for (auto it = begin(); it != end(); ++it) {
        if (it.get_leaf().m_weight > max_weight) {
            max_weight = it.get_leaf().m_weight;
            max_leaf = &(it.get_leaf());
        }
    }
    
    return max_leaf ? *max_leaf : *this;
}

template <typename T>
const trie<T>& trie<T>::max() const {
    if (m_children.empty()) {
        return *this;
    }
    
    double max_weight = std::numeric_limits<double>::lowest();
    const trie<T>* max_leaf = nullptr;
    
    for (auto it = begin(); it != end(); ++it) {
        if (it.get_leaf().m_weight > max_weight) {
            max_weight = it.get_leaf().m_weight;
            max_leaf = &(it.get_leaf());
        }
    }
    
    return max_leaf ? *max_leaf : *this;
}

template <typename T>
trie<T>& trie<T>::operator[](const std::vector<T>& path) {
    trie<T>* current = this;
    
    for (const auto& label : path) {
        bool found = false;
        
        for (auto it = current->m_children.begin(); it != current->m_children.end(); ++it) {
            if (it->m_label && *it->m_label == label) {
                current = &(it.get_trie());
                found = true;
                break;
            }
        }
        
        if (!found) {
            return *this; // Return root if path not found
        }
    }
    
    return *current;
}

template <typename T>
const trie<T>& trie<T>::operator[](const std::vector<T>& path) const {
    const trie<T>* current = this;
    
    for (const auto& label : path) {
        bool found = false;
        
        for (auto it = current->m_children.begin(); it != current->m_children.end(); ++it) {
            if (it->m_label && *it->m_label == label) {
                current = &(it.get_trie());
                found = true;
                break;
            }
        }
        
        if (!found) {
            return *this; // Return root if path not found
        }
    }
    
    return *current;
}

// Comparison Operators
template <typename T>
bool trie<T>::operator==(const trie<T>& other) const {
    if (m_children.empty() && other.m_children.empty()) {
        return m_weight == other.m_weight;
    }
    return m_children == other.m_children;
}

template <typename T>
bool trie<T>::operator!=(const trie<T>& other) const {
    return !(*this == other);
}

// Union Operations
template <typename T>
trie<T> trie<T>::operator+(const trie<T>& other) const {
    trie<T> result = *this;
    result += other;
    return result;
}

template <typename T>
trie<T>& trie<T>::operator+=(const trie<T>& other) {
    if (m_children.empty()) {
        if (other.m_children.empty()) {
            // Both are leaves
            m_weight += other.m_weight;
        } else {
            // This is leaf, other has children
            *this = other;
            for (auto it = begin(); it != end(); ++it) {
                it.get_leaf().m_weight += m_weight;
            }
        }
    } else {
        if (other.m_children.empty()) {
            // This has children, other is leaf
            for (auto it = begin(); it != end(); ++it) {
                it.get_leaf().m_weight += other.m_weight;
            }
        } else {
            // Both have children
            for (const auto& other_child : other.m_children) {
                bool found = false;
                
                for (auto it = m_children.begin(); it != m_children.end(); ++it) {
                    if (it->m_label && other_child.m_label && 
                        *it->m_label == *other_child.m_label) {
                        it.get_trie() += other_child;
                        found = true;
                        break;
                    }
                }
                
                if (!found) {
                    add_child(other_child);
                }
            }
        }
    }
    
    return *this;
}

// Path Compression
template <typename T>
void trie<T>::path_compress() {
    for (auto it = m_children.begin(); it != m_children.end(); ) {
        auto& child = it.get_trie();
        
        // Recursively compress children first
        child.path_compress();
        
        // Check if this child has exactly one child
        if (child.m_children.begin() != child.m_children.end() &&
            child.m_children.begin().get_trie().m_children.begin() == child.m_children.begin().get_trie().m_children.end()) {
            
            // Get the single grandchild
            auto& grandchild = child.m_children.begin().get_trie();
            
            // Combine labels if possible (requires T to support +)
            if (child.m_label && grandchild.m_label) {
                T combined_label = *child.m_label + *grandchild.m_label;
                trie<T> compressed_child = grandchild;
                compressed_child.set_label(&combined_label);
                
                // Remove old child and add compressed version
                m_children.delete_node(child);
                add_child(compressed_child);
                
                // Restart iteration
                it = m_children.begin();
                continue;
            }
        }
        
        ++it;
    }
}

// Stream I/O Implementation
template <typename T>
std::ostream& operator<<(std::ostream& os, const trie<T>& t) {
    if (t.get_children().empty()) {
        os << t.get_weight() << " children = {}";
    } else {
        os << "children = { ";
        bool first = true;
        
        for (auto it = t.get_children().begin(); it != t.get_children().end(); ++it) {
            if (!first) os << ", ";
            first = false;
            
            if (it->get_label()) {
                os << *it->get_label() << " ";
            }
            os << it.get_trie();
        }
        
        os << " }";
    }
    
    return os;
}

template <typename T>
std::istream& operator>>(std::istream& is, trie<T>& t) {
    try {
        t = parse_tree<T>(is);
    } catch (const parser_exception& e) {
        is.setstate(std::ios::failbit);
        throw;
    }
    return is;
}

template <typename T>
trie<T>::trie() : m_parent(nullptr), m_label(nullptr), m_children(), m_weight(0.0) {}

template <typename T>
trie<T>::trie(double weight) : m_parent(nullptr), m_label(nullptr), m_children(), m_weight(weight) {}


template <typename T>
trie<T>::trie(T const& label)
: m_parent(nullptr), m_label(new T(label)), m_children(), m_weight(0.0) {}

template <typename T>
trie<T>::trie(T const& label, double weight)
    : m_parent(nullptr), m_label(new T(label)), m_children(), m_weight(weight) {}