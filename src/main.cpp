#include "trie.hpp"
#include <iostream>

int main() {
    trie<char> t('a', 1.0);
    trie<char> child('b', 2.0);
    t.add_child(child);

    std::cout << "Root label: " << *t.get_label() << "\n";
    std::cout << "Child label: " << *t.get_children().begin()->get_label() << "\n";
    std::cout << "Max weight: " << t.max().get_weight() << "\n";
}