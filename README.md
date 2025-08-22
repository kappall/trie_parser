# C++ Trie Implementation

A comprehensive C++ implementation of a trie (prefix tree) data structure with advanced features including iterators, path compression, and trie union operations.

## Features

- **Generic Template Implementation**: Works with any comparable type
- **Multiple Iterator Types**: 
  - Leaf iterators for traversing only leaf nodes
  - Node iterators for traversing the path to root
- **Advanced Operations**:
  - Prefix search with `operator[]`
  - Maximum weight leaf search
  - Trie union operations (`operator+`, `operator+=`)
  - Path compression for optimization
- **Custom Container**: Uses a custom `bag` container for storing children
- **Stream I/O Support**: Built-in parsing and serialization
- **Exception Safety**: Proper RAII and exception handling

## Project Structure

```
├── bag.hpp          # Custom container for storing trie children
├── trie.hpp         # Trie class declaration
├── trie.cpp         # Trie class implementation
└── README.md        # This file
```

## Usage

### Basic Example

```cpp
#include "trie.hpp"
#include <iostream>
#include <string>

int main() {
    // Create a trie with string labels
    trie<std::string> t;
    
    // Create child nodes
    trie<std::string> child1(5.0);  // weight = 5.0
    std::string label1 = "hello";
    child1.set_label(&label1);
    
    trie<std::string> child2(3.0);  // weight = 3.0
    std::string label2 = "world";
    child2.set_label(&label2);
    
    // Add children to root
    t.add_child(child1);
    t.add_child(child2);
    
    // Find maximum weight leaf
    auto& max_leaf = t.max();
    std::cout << "Max weight: " << max_leaf.get_weight() << std::endl;
    
    // Iterate through leaves
    for (auto it = t.begin(); it != t.end(); ++it) {
        std::cout << "Leaf weight: " << it.get_leaf().get_weight() << std::endl;
    }
    
    return 0;
}
```

### Prefix Search

```cpp
// Search for a path in the trie
std::vector<std::string> path = {"hello", "world"};
auto& subtrie = t[path];
```

### Stream I/O

The trie supports parsing from streams using a specific grammar:

```
TREE -> LEAF | {BAG}
BAG -> TREE | TREE, BAG | LEAF | LEAF, BAG | ε
LEAF -> weight children = {}
NODE -> children = {BAG}
```

Example input format:
```
children = { 
    label1 5.0 children = {}, 
    label
