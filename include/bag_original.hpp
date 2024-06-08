template <typename Val>
class bag{
public:

    struct node
    {
        Val* trie;
        node* next;
        node() : trie(nullptr), next(nullptr){
            //std::cout<<"node constructor"<<std::endl;
        }
        node(Val const& r){
            trie = new Val(r);
        }

        Val* get_trie(){
            return trie;
        }

        node* add_node(Val* new_child){
            node* new_n = new node;
            new_n->trie = new_child;
            return new_n;
        }
        bool operator==(node const& rhs){
            return trie == (rhs.trie);
        }
        bool operator!=(node const& rhs){
            if(trie == nullptr && rhs.trie == nullptr)
                return false;
            else if((trie == nullptr && rhs.trie!= nullptr) || (trie!= nullptr && rhs.trie == nullptr))
                return true;
            else
                return trie != (rhs.trie);
        }
    };
    node* m_head;
    node* m_node_ptr;
    bag(){
        //std::cout<<"bag constructor"<<std::endl;
        m_head = new node;
        m_node_ptr = m_head;
    }
    ~bag(){
        //std::cout<<"bag destructor"<<std::endl;
        while(m_head){
            node* temp = m_head->next;
            delete m_head->trie;
            delete m_head;
            m_head = temp;
        }
        m_node_ptr = m_head;
    }

    bool operator==(bag<Val> const& rhs) const{
        bool ret = true;
        node* it = m_head;
        node* r_it = rhs.m_head;
        while(ret && it != nullptr && r_it != nullptr){
            ret = (it == r_it);
            it++;
            r_it++;
            if((it != nullptr && r_it == nullptr) || (it == nullptr && r_it != nullptr))
                ret = false;
        }
        return ret;
    }
    bool operator!=(bag<Val> const& rhs) const{
        return !(*this==rhs);
    }
    bag<Val>& operator=(bag<Val> const& rhs){
        if(this!=&rhs){
            //free this
            while(m_head->trie){
                node* temp = m_head;
                m_head = m_head->next;
                delete temp;
            }

            if(rhs.m_head->trie){
                m_head = new node{*(rhs.m_head->trie)};
                node* pc = m_head;
                node* pr = rhs.m_head;
                while(pr->next){
                    pc->next = new node{*(pr->trie)};
                    pc = pc->next;
                    pr = pr->next;
                }
            }
        }
        return *this;
    }

    bag<Val>& operator=(bag<Val>* rhs){
        if(this!=rhs){
            //free thia
            while(m_head){
                node* temp = m_head;
                m_head = m_head ->next;
                delete temp;
            }
            m_head = rhs->m_head;

        }
        return *this;
    }

    struct bag_iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = node;
        using pointer = node*;
        using reference = node&;

        bag_iterator(node* ptr){
            m_ptr = ptr;
        }
        reference operator*() const{
            return *m_ptr;
        }
        pointer operator->() const{
            return ((m_ptr));
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
        bool operator==(bag_iterator const& rhs) const{
            return m_ptr == rhs.m_ptr;
        }
        bool operator!=(bag_iterator const& rhs) const{
            return m_ptr != rhs.m_ptr;
        }
        bool operator!=(Val const& rhs) const{
            return *(m_ptr->trie) != rhs;
        }

    private:
        node* m_ptr;
    };
    bag_iterator const begin(){
        return m_head;
    }
    bag_iterator end(){
        return nullptr;
    }

    template <typename T>
    Val* get_child(T const& label){
        node* it = begin();
        while(it!=end() && label<it->trie->get_label()){
            it++;
        }
        return it;
    }
};
