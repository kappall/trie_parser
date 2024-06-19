#include "../src/trie.cpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

int main(int argc, char const** argv) {
    if (argc < 2) return 1;

    trie<char> t;  // Crea un oggetto trie vuoto
    std::string filename(argv[1]);
    std::ifstream in(filename);
    if (!in.good()) {
        std::cerr << "cannot open file '" << filename << "'" << std::endl;
        return 1;
    }
    try {
        in >> t;  // Parso il trie da input
    } catch (parser_exception const& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    std::cout << t << std::endl;
    /* assume t is a trie<T> */
    for (auto leaf_it = t.begin(); leaf_it != t.end(); ++leaf_it) {
        trie<char>::node_iterator node_it = leaf_it; // we convert leaf_it into node_it to navigate from leaf to root
        std::vector<char> s;
        while (node_it != t.root()) {
            s.push_back(*node_it);
            ++node_it;
        }
        for (auto const& x: s) std::cout << x << ' ';
        std::cout << '\n';
    }
    std::cout<<std::endl;
    std::vector<char> prefix{'b'};
    auto t2 = t[prefix];
    for (auto it2 = t2.begin(); it2 != t2.end(); ++it2) {
        std::cout << *it2 << ' ';
    }
    std::cout<<std::endl;
    //t.path_compress();
    std::cout << t << std::endl;
    //std::cout << t.max()<< std::endl;

    trie<char> c = t[{'1'}];
    std::cout << *(c.get_label()) << std::endl;
    trie<char> s = t;
    /*
    char i = 'e';
    s.get_children().m_head->trie->set_label(&i);
    trie<char> sum = s+c;
    std::cout << (s==c) << std::endl;
    std::cout << sum<<std::endl;
    const trie<char> l = sum;
    for(trie<char>::const_node_iterator it = l.get_children().m_head->trie; it!= l.root(); ++it){
        std::cout<<*it<<std::endl;
    }

    std::cout<< l.max() << " "q<< s.max()<<std::endl;*/
    for(auto it = t.begin(); it!= t.end(); ++it){
        std::cout <<*it<< " ";
    }


    std::cout<<std::endl<<s<<std::endl;
    std::cout<<t<<std::endl;
    std::cout<<(s==t);
    return 0;
}