//todo copiare i figli senza creare un padre per ogni figlio
template <typename Val>
class bag{
public:

    struct Node
    {
        Val* trie;
        Node* next;

        Node(Val* trie, Node* next) : trie(trie), next(next){}

        Node(Val const& tr) : trie(new Val(tr)), next(nullptr){}

        ~Node(){
            delete trie;
        }

        bool operator==(Node const& rhs){
            return *(trie->get_label()) == *(rhs.trie->get_label());
        }
        bool operator!=(Node const& rhs){
            return !(*this == rhs);
        }
    };
    Node* m_head;

    bag(){
        m_head = nullptr;
    }

    bag(bag<Val> const& rhs){
        *this = rhs;
    }

    ~bag(){
        while(m_head){
            Node* temp = m_head;
            m_head = m_head->next;
            delete temp;
        }
    }

    void bag_prepend(Val* trie){
        Node* new_node = new Node(trie,m_head);
        m_head = new_node;
    }

    bool operator==(bag<Val> const& rhs) const{
        bool ret = true;
        if(m_head == nullptr && rhs.m_head == nullptr)
            ret = true;
        else if((m_head != nullptr && rhs.m_head == nullptr) || (m_head== nullptr && rhs.m_head != nullptr))
            ret = false;
        Node* it = m_head;
        Node* r_it = rhs.m_head;
        while(ret && it && r_it){
            ret = (*it == *r_it);
            it++;
            r_it++;
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
                m_head = new Node(*(rhs.m_head->trie));
                Node* pc = m_head;
                Node* pr = rhs.m_head->next;
                while(pr){
                    pc->next = new Node(*(pr->trie));
                    pc = pc->next;
                    pr = pr->next;
                }
            }
            else
                m_head = nullptr;
        }
        return *this;
    }
    Node* add_node(Val* new_child, Node* next){
        Node* new_n = new Node(new_child, next);
        return new_n;
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
            return *(m_ptr->trie);
        }
        pointer operator->() {
            return m_ptr->trie;
        }
        bag_iterator& operator++(){
            m_ptr = m_ptr->next;
            return *this;
        }
        bag_iterator operator++(int dummy){
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
        Val* get_trie(){
            return m_ptr->trie;
        }

    private:
        Node* m_ptr;
    };


    bag_iterator const begin(){
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
            return *(m_ptr->trie);
        }
        pointer operator->() const{
            return m_ptr->trie;
        }
        const_bag_iterator& operator++() {
            m_ptr = m_ptr->next;
            return *this;
        }
        bool operator==(const_bag_iterator const& rhs) const{
            return m_ptr == rhs.m_ptr;
        }
        bool operator!=(const_bag_iterator const& rhs) const{
            return m_ptr != rhs.m_ptr;
        }
        const Val* get_trie(){
            return m_ptr->trie;
        }

    private:
        Node* m_ptr;
    };

    const_bag_iterator const begin() const{
        return m_head;
    }
    const_bag_iterator const end() const{
        return nullptr;
    }
};