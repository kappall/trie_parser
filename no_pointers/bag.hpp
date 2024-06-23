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

        bool operator==(Node const& rhs){
            return *(trie.get_label()) == *(rhs.trie.get_label());
        }
        bool operator!=(Node const& rhs){
            return *(trie.get_label()) != *(rhs.trie.get_label());;
        }
    };
    Node* m_head;

    bag(){
        m_head = nullptr;
    }

    bag(bag<Val> const& rhs) : m_head(nullptr){
        if(rhs.m_head) {
            Node* pc = rhs.m_head;
            while(pc){
                push_back(pc->trie);
                pc = pc->next;
            }
        }
    }

    ~bag(){
        while(m_head){
            Node* temp = m_head;
            m_head = m_head->next;
            delete temp;
        }
    }

    void bag_prepend(Node* new_node){
        new_node->next = m_head;
        m_head = new_node;
    }
    void push_back(Val const& t){//t è da copiare
        Node* n = new Node(t);
        if(!m_head)
            m_head = n;
        else {
            Node* pc = m_head;
            while (pc->next){
                pc = pc->next;
            }
            pc->next = n;
        }
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
                ret = (*it == *r_it);
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
        m_head = rhs.m_head;
        rhs.m_head = nullptr;
        return *this;
    }

    bag<Val>& operator=(bag<Val> const& rhs){
        if(*this!=rhs){
            //free this
            while(m_head){
                Node* temp = m_head;
                m_head = m_head->next;
                delete temp;
            }

            if(rhs.m_head){
                m_head = new Node(rhs.m_head->trie);
                Node* pc = m_head;
                Node* pr = rhs.m_head->next;
                while(pr){
                    pc->next = new Node(pr->trie);
                    pc = pc->next;
                    pr = pr->next;
                }
            }
            else
                m_head = nullptr;
        }
        return *this;
    }
    Node* create_node(Val const& new_child){
        Node* new_n = new Node(new_child);
        return new_n;
    }

    void delete_node(Val& n){
        if(*(n.get_label())!=*(m_head->trie.get_label())) {
            Node *pc = m_head;
            while (pc->next && *(n.get_label())!=*(pc->next->trie.get_label())){
                pc = pc->next;
            }
            if(pc->next){
                Node* temp = pc->next;
                pc->next = pc->next->next;
                delete temp;
            }
        }else{
            Node* temp = m_head;
            m_head = m_head->next;
            delete temp;
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
        const Val& get_trie(){
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