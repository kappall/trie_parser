#include <iostream>

template <typename Val>
class bag{
public:

    struct Node
    {
        Val trie;
        Node* next;

        Node() : trie(), next(nullptr){}

        Node(Val const& tr) : trie(tr), next(nullptr){}
    };
    Node* m_head;

    bag(){
        m_head = nullptr;
    }

    ~bag(){
        clear();
    }

    void bag_prepend(Node* new_node){
        new_node->next = m_head;
        m_head = new_node;
    }

    bool operator==(bag<Val> const& rhs) const{
        bool ret = true;
        if(m_head == nullptr && rhs.m_head == nullptr)
            ret = true;
        else if((m_head != nullptr && rhs.m_head == nullptr) || (m_head == nullptr && rhs.m_head != nullptr))
            ret = false;
        else {
            const_bag_iterator it = begin();
            const_bag_iterator r_it = rhs.begin();
            while (ret && it!=end() && r_it!=rhs.end()) {
                ret = ( (*(*it).get_label()) == *((*r_it).get_label()) ) && (*it == *r_it);
                ++it;
                ++r_it;
            }
        }
        return ret;
    }
    bool operator!=(bag<Val> const& rhs) const{
        return !(*this == rhs);
    }
    bag<Val>& operator=(bag<Val>&& rhs){
        Node* tmp = m_head;
        m_head = rhs.m_head;
        rhs.m_head = tmp;

        return *this;
    }

    void clear(){
        while(m_head){
            Node* temp = m_head;
            m_head = m_head->next;
            delete temp;
        }
    }

    Node* create_node(Val const& src){
        Node* new_n = new Node(src);
        return new_n;
    }

    void delete_node(Val& n){
        if(m_head) {
            if (*(n.get_label()) != *(m_head->trie.get_label())) {
                Node *pc = m_head;
                while (pc->next && *(n.get_label()) != *(pc->next->trie.get_label())) {
                    pc = pc->next;
                }
                if (pc->next) {
                    Node* temp = pc->next;
                    pc->next = pc->next->next;
                    delete temp;
                }
            } else {
                Node* temp = m_head;
                m_head = m_head->next;
                delete temp;
            }
        }
    }


    struct bag_iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = Val;
        using pointer = Val*;
        using reference = Val&;

        bag_iterator(Node* ptr){
            m_ptr = ptr;
        }
        reference operator*() {
            return m_ptr->trie;
        }
        pointer operator->() {
            return &(m_ptr->trie);
        }
        bag_iterator& operator++(){
            m_ptr = m_ptr->next;
            return *this;
        }
        bag_iterator operator++(int){
            auto temp = m_ptr;
            m_ptr = m_ptr->next;
            return temp;
        }
        bool operator==(bag_iterator const& rhs){
            return m_ptr == rhs.m_ptr;
        }
        bool operator!=(bag_iterator const& rhs){
            return m_ptr != rhs.m_ptr;
        }
        Val& get_trie(){
            return m_ptr->trie;
        }

    private:
        Node* m_ptr;
    };


    bag_iterator begin(){
        return m_head;
    }
    bag_iterator end(){
        return nullptr;
    }

    struct const_bag_iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = Val const;
        using pointer = Val const*;
        using reference = Val const&;

        const_bag_iterator(Node* const ptr){
            m_ptr = ptr;
        }
        reference operator*() const{
            return m_ptr->trie;
        }
        pointer operator->() const{
            return &(m_ptr->trie);
        }

        bool operator==(const_bag_iterator const& rhs) const{
            return m_ptr == rhs.m_ptr;
        }
        bool operator!=(const_bag_iterator const& rhs) const{
            return m_ptr != rhs.m_ptr;
        }
        const Val& get_trie() const{
            return m_ptr->trie;
        }
        const_bag_iterator& operator++(){
            m_ptr = m_ptr->next;
            return *this;
        }
        const_bag_iterator operator++(int){
            auto temp = m_ptr;
            m_ptr = m_ptr->next;
            return temp;
        }

    private:
        Node* m_ptr;
    };

    const_bag_iterator begin() const{
        return m_head;
    }
    const_bag_iterator end() const{
        return nullptr;
    }

};