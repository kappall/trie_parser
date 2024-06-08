//todo:  controlla getters e setters, operator<<, il problema è che un nodo viene scritto 2 volte, il tire è lo stesso ma con oggetti diversi

#include <iostream>
#include "trie.hpp"

//parser
/*
 ALBERO -> LEAF | {BAG}
 BAG -> n | CHILD | CHILD, BAG
 CHILD -> ALBERO

 */

template <typename T>
trie<T> CHILD(std::istream&);
template <typename T>
trie<T> ALBERO(std::istream&);
template <typename T>
trie<T> LEAF(std::istream&);

void skip_blank_spaces(std::istream& is) {
   char c = 0;
   is >> c;
   is.putback(c);
}

template <typename T>
trie<T> LEAF(std::istream& is){
    skip_blank_spaces(is);


    double weight = 0.0;
    is >> weight;
    trie<T> leaf(weight);

    skip_blank_spaces(is);

    std::string s("");
    is >> s;

    if(s!="children") {
        s = "expecting keyword \"children\" instead of \"" + s +"\"";
        throw parser_exception(s);
    }
    char c = '0';
    is >> c;
    if(c!='=') {
        throw parser_exception("expecting operand '='");
        }
    skip_blank_spaces(is);
    is >> c;
    if(c!='{') throw parser_exception("expecting operand '{'");
    skip_blank_spaces(is);
    is >> c;
    if(c!='}') throw parser_exception("expecting operand '}'");
    skip_blank_spaces(is);

    return leaf;
}

template <typename T>
trie<T> CHILD(std::istream& is){
   skip_blank_spaces(is);

   trie<T> child;

   T label;
   is >> label;

   skip_blank_spaces(is);
   //if i read a number than it's a leaf else it's a node(a sub-trie)
   char c = is.peek();
   if(c>='0' && c<= '9')//is a leaf
   {
       child = LEAF<T>(is);
   }else {
       child = ALBERO<T>(is);
   }
    child.set_label(new T(label));


   return child;
}


template <typename T>
trie<T> ALBERO(std::istream& is){
   skip_blank_spaces(is);


   trie<T> t;
   std::string s("");
   is >> s;
   if(s!="children") {
       s = "expecting keyword \"children\" instead of \"" + s +"\"";
       throw parser_exception(s);
   }
   skip_blank_spaces(is);

   char c = '0';
   is >> c;
   if(c!='=') throw parser_exception("expecting symbol '='");
   skip_blank_spaces(is);

   is >> c;
   if(c!='{') throw parser_exception("expecting symbol '{'");
   skip_blank_spaces(is);
   c = is.peek();
   bool in = false;
   while(c!='}'){
       in = true;
       t.add_child(CHILD<T>(is));
       skip_blank_spaces(is);
       is >> c;

       if(c!='}' && c!=',')  throw parser_exception("expecting symbol '}' or ','");
   }
   if(!in)
       throw parser_exception("expecting weight");

   return t;
}


//implemntation of trie.hpp
template <typename T>
trie<T>::trie() : m_p(nullptr), m_l(nullptr), m_c(), m_w(0.0) {}

template <typename T>
trie<T>::trie(double w) : m_p(nullptr), m_l(nullptr), m_c(), m_w(w) {}

template <typename T>
trie<T>::trie(trie<T> const& rhs) : m_p(nullptr), m_c(rhs.m_c), m_w(rhs.m_w){
    if(rhs.m_l)
        m_l = new T(*(rhs.m_l));
    else
        m_l = nullptr;

    auto pc = m_c.m_head;
    while(pc){
        pc->trie->set_parent(this);
        pc = pc->next;
    }

}

template <typename T>
trie<T>::trie(trie<T>&& rhs): m_p(nullptr), m_l(rhs.m_l), m_c(), m_w(rhs.m_w) {

    m_c.m_head = rhs.m_c.m_head;
    auto pc = m_c.m_head;
    while(pc){
        pc->trie->set_parent(this);
        pc = pc->next;
    }

}


template <typename T>
trie<T>::~trie(){
    if(m_l)
        delete m_l;
}//automatically calls all destructors

template <typename T>
trie<T>& trie<T>::operator=(trie<T> const& rhs){
    if(this!=&rhs){
        //no delete of m_p, it might have other children
        delete m_l;

        m_p = nullptr;
        if(rhs.m_l)
            m_l = new T(*rhs.m_l);
        else
            m_l = nullptr;
        m_w = rhs.m_w;

        m_c = rhs.m_c;
        auto pc = m_c.m_head;
        while(pc){
            pc->trie->set_parent(this);
            pc = pc->next;
        }
    }

    return *this;
}

template <typename T>
trie<T>& trie<T>::operator=(trie<T>&& rhs){
    //no delete of m_p, it might have other children
    delete m_l;

    m_p = nullptr;
    m_l = rhs.m_l;
    rhs.m_l= nullptr;
    m_w = rhs.m_w;

    m_c.m_head = rhs.m_c.m_head;
    rhs.m_c.m_head = nullptr;
    auto pc = m_c.m_head;
    while(pc){
        pc->trie->set_parent(this);
        pc = pc->next;
    }

    return *this;
}

//setters and getters
template <typename T>
void trie<T>::set_weight(double w){
    m_w = w;
}

template <typename T>
double trie<T>::get_weight() const {
    return m_w;
}

template <typename T>
void trie<T>::set_label(T* l){
    delete m_l;
    m_l = l;
}
template <typename T>
T const* trie<T>::get_label() const{
    //if root it will return nullptr
    return m_l;
}
template <typename T>
T* trie<T>::get_label(){
    //if root it will return nullptr
    return m_l;
}

template <typename T>
void trie<T>::set_parent(trie<T>* p){
    m_p = p;
}

template <typename T>
trie<T> const* trie<T>::get_parent() const{
    //if root it will return nullptr
    return m_p;
}
template <typename T>
trie<T>* trie<T>::get_parent(){
    //if root it will return nullptr
    return m_p;
}
template <typename T>
bag<trie<T>> const& trie<T>::get_children() const{
    return m_c;
}
template <typename T>
bag<trie<T>>& trie<T>::get_children(){
    return m_c;
}
template <typename T>
bool trie<T>::operator==(trie<T> const& rhs) const{
    if(m_l && rhs.m_l)
        return *m_l == *(rhs.m_l) && m_c == rhs.m_c;
    else if(!m_l && !rhs.m_l)
        return m_c == rhs.m_c;
    else
        return false;
}

template <typename T>
bool trie<T>::operator!=(trie<T> const& rhs) const{
    return *m_l != *(rhs.m_l) || m_c != rhs.m_c;
}


template <typename T>
void trie<T>::add_child(trie<T> const& c){

    trie<T> *new_child = new trie<T>(c);
    if(m_c.m_head) {
        if (*(new_child->m_l) < *(m_c.m_head->trie->m_l)) {//if i need to add it as the head
            m_c.bag_prepend(new_child);
        }else {
            if (*(new_child->m_l) == *(m_c.m_head->trie->m_l))
                throw parser_exception("It is not possible for 2 children with the same father to have the same label");
            auto pc = m_c.m_head;
            while (pc->next != nullptr && *(pc->next->trie->m_l) < *(new_child->m_l)) {
                pc = pc->next;
                if (pc->next && *(pc->next->trie->m_l) == *(new_child->m_l))
                    throw parser_exception(
                            "It is not possible for 2 children with the same father to have the same label");
            }
            pc->next = m_c.add_node(new_child, pc->next);
        }
    } else {
        m_c.bag_prepend(new_child);
    }
    new_child->set_parent(this);
    (this)->set_weight(0.0);//in case it was a leaf
}



template <typename T>
std::istream& operator>>(std::istream& is, trie<T>& t){
    t = ALBERO<T>(is);
    return is;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, trie<T> const& t){
    os << "children={";
    if(t.get_children().m_head) {
        for (typename bag<trie<T>>::const_bag_iterator it = t.get_children().begin(); it != t.get_children().end(); ) {
            os << (*it)->get_label();
            if (!(*it)->get_children().m_head) {
                os << (*it)->get_weight() << "children={}";
            } else {
                std::cout << *(*it);
            }
            if (++it != t.get_children().end())
                os << ",";

        }
    }
    os << '}';

    return os;
}


/*
template <typename T>
struct trie {
struct node_iterator {
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using pointer = T*;
    using reference = T&;

    node_iterator(trie<T>*);
    reference operator*() const;
    pointer operator->() const;
    node_iterator& operator++();
    node_iterator operator++(int);
    bool operator==(node_iterator const&) const;
    bool operator!=(node_iterator const&) const;

private:
    trie<T>* m_ptr;
};*/

template <typename T>
trie<T>::node_iterator::node_iterator(trie<T>* ptr) : m_ptr(ptr){}

template <typename T>
typename trie<T>::node_iterator::reference trie<T>::node_iterator::operator*() const{
    return *(m_ptr->m_l);
}

template <typename T>
typename trie<T>::node_iterator::pointer trie<T>::node_iterator::operator->() const{
    return m_ptr->m_l;
}

template <typename T>
typename trie<T>::node_iterator& trie<T>::node_iterator::operator++() {
    if(m_ptr->m_p)
        m_ptr = m_ptr->m_p;
    return *this;
}

template <typename T>
typename trie<T>::node_iterator trie<T>::node_iterator::operator++(int dummy) {
    auto ptr = m_ptr;
    if(m_ptr->m_p)
        m_ptr = m_ptr->m_p;
    return ptr;
}
template <typename T>
bool trie<T>::node_iterator::operator==(node_iterator const& rhs) const{
    return m_ptr == rhs.m_ptr;
}
template <typename T>
bool trie<T>::node_iterator::operator!=(node_iterator const& rhs) const{
    return m_ptr != rhs.m_ptr;
}

template <typename T>
typename trie<T>::node_iterator trie<T>::root(){
    node_iterator n_it(this);
    node_iterator previus(this);
    do {
        previus = n_it;
        n_it++;
    }while(n_it != previus);
    return n_it;
}

/*
struct const_node_iterator {
    using iterator_category = std::forward_iterator_tag;
    using value_type = const T;
    using pointer = T const*;
    using reference = T const&;

    const_node_iterator(trie<T> const* ptr);
    reference operator*() const;
    pointer operator->() const;
    const_node_iterator& operator++();
    const_node_iterator operator++(int);
    bool operator==(const_node_iterator const&) const;
    bool operator!=(const_node_iterator const&) const;
    */
template <typename T>
trie<T>::const_node_iterator::const_node_iterator(trie<T> const* ptr) : m_ptr(ptr){}

template <typename T>
typename trie<T>::const_node_iterator::reference trie<T>::const_node_iterator::operator*() const{
    return *(m_ptr->m_l);
}

template <typename T>
typename trie<T>::const_node_iterator::pointer trie<T>::const_node_iterator::operator->() const{
    return m_ptr->m_l;
}

template <typename T>
typename trie<T>::const_node_iterator& trie<T>::const_node_iterator::operator++() {
    if(m_ptr->get_parent())
        m_ptr = m_ptr->get_parent();
    return *this;
}

template <typename T>
typename trie<T>::const_node_iterator trie<T>::const_node_iterator::operator++(int dummy) {
    auto ptr = m_ptr;
    if(m_ptr->get_parent())
        m_ptr = m_ptr->get_parent();
    return ptr;
}
template <typename T>
bool trie<T>::const_node_iterator::operator==(const_node_iterator const& rhs) const{
    return m_ptr == rhs.m_ptr;
}
template <typename T>
bool trie<T>::const_node_iterator::operator!=(const_node_iterator const& rhs) const{
    return m_ptr != rhs.m_ptr;
}

/*
struct leaf_iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        leaf_iterator(trie<T>*);
        reference operator*() const;
        pointer operator->() const;
        leaf_iterator& operator++();
        leaf_iterator operator++(int);
        bool operator==(leaf_iterator const&) const;
        bool operator!=(leaf_iterator const&) const;
        operator node_iterator() const;

        trie<T>& get_leaf() const;

    private:
        trie<T>* m_ptr;
    };*/

template <typename T>
trie<T>::leaf_iterator::leaf_iterator(trie<T>* ptr)  {
    m_ptr = ptr;
}

template <typename T>
typename trie<T>::leaf_iterator::reference trie<T>::leaf_iterator::operator*() const{
    return m_ptr->get_label();
}

template <typename T>
typename trie<T>::leaf_iterator::pointer trie<T>::leaf_iterator::operator->() const{
    return m_ptr->m_l;
}

template <typename T>
typename trie<T>::leaf_iterator& trie<T>::leaf_iterator::operator++(){
    if(m_ptr && m_ptr->m_p) {//checking if m_ptr is pointing to nullpre or to the root
        auto pc = m_ptr->m_p->m_c.m_head;
        while (pc && pc->trie != m_ptr)
            pc = pc->next;
        if (pc && pc->next) {
            m_ptr = pc->next->trie;
        }
        else {
            if(m_ptr->m_p->m_p) {//if father is not root
                m_ptr = m_ptr->m_p;
                ++(*this);
            }else
                m_ptr = nullptr;
        }if(m_ptr && m_ptr->get_children().m_head) {//if it is not a leaf
            while (m_ptr->get_children().m_head) {
                m_ptr = m_ptr->get_children().m_head->trie;
            }
        }
    } else{//in case m_ptr was pointing to the root
        m_ptr = nullptr;
    }
    return *this;
}

template <typename T>
typename trie<T>::leaf_iterator trie<T>::leaf_iterator::operator++(int dummy){
    auto temp = m_ptr;
    if(m_ptr && m_ptr->m_p) {//checking if m_ptr is pointing to nullpre or to the root
        auto pc = m_ptr->m_p->m_c.m_head;
        while (pc && pc->trie != m_ptr)
            pc = pc->next;
        if (pc && pc->next) {
            m_ptr = pc->next->trie;
        }
        else {
            if(m_ptr->m_p->m_p) {//if father is not root
                m_ptr = m_ptr->m_p;
                ++(*this);
            }else
                m_ptr = nullptr;
        }if(m_ptr && m_ptr->get_children().m_head) {//if it is not a leaf
            while (m_ptr->get_children().m_head) {
                m_ptr = m_ptr->get_children().m_head->trie;
            }
        }
    } else{//in case m_ptr was pointing to the root
        m_ptr = nullptr;
    }
    return temp;
}


template <typename T>
bool trie<T>::leaf_iterator::operator==(leaf_iterator const& rhs) const {
    return m_ptr == rhs.m_ptr;
}

template <typename T>
bool trie<T>::leaf_iterator::operator!=(leaf_iterator const& rhs) const {
    return m_ptr != rhs.m_ptr;
}

template <typename T>
typename trie<T>::leaf_iterator trie<T>::begin(){
    if(m_c.m_head) { //if there is any child
        auto pc = m_c.m_head->trie;//first child of this
        while (pc->m_c.m_head)
            pc = pc->m_c.m_head->trie;//first child of pc
        return pc;
    }else {
        return nullptr;
    }
}
template <typename T>
typename trie<T>::leaf_iterator trie<T>::end(){
    return nullptr;
}
template <typename T>
trie<T>::leaf_iterator::operator node_iterator() const{
    node_iterator n_it(m_ptr);
    return n_it;
}

template <typename T>
trie<T>& trie<T>::leaf_iterator::get_leaf() const{
    return *m_ptr;
}

/*
 struct const_leaf_iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = const T;
        using pointer = T const*;
        using reference = T const&;

        const_leaf_iterator(trie<T> const* ptr);
        reference operator*() const;
        pointer operator->() const;
        const_leaf_iterator& operator++();
        const_leaf_iterator operator++(int);
        bool operator==(const_leaf_iterator const&) const;
        bool operator!=(const_leaf_iterator const&) const;
        operator const_node_iterator() const;

        trie<T> const& get_leaf() const;

    private:
        trie<T> const* m_ptr;
    };
 */

template <typename T>
trie<T>::const_leaf_iterator::const_leaf_iterator(trie<T> const* ptr) : m_ptr(ptr){}

template <typename T>
typename trie<T>::const_leaf_iterator::reference trie<T>::const_leaf_iterator::operator*() const{
    return *(m_ptr->get_label());
}

template <typename T>
typename trie<T>::const_leaf_iterator::pointer trie<T>::const_leaf_iterator::operator->() const{
    return m_ptr->m_l;
}

template <typename T>
typename trie<T>::const_leaf_iterator& trie<T>::const_leaf_iterator::operator++(){
    if(m_ptr && m_ptr->m_p) {//checking if m_ptr is pointing to nullpre or to the root
        auto pc = m_ptr->m_p->m_c.m_head;
        while (pc && pc->trie != m_ptr)
            pc = pc->next;
        if (pc && pc->next) {
            m_ptr = pc->next->trie;
        }
        else {
            if(m_ptr->m_p->m_p) {//if father is not root
                m_ptr = m_ptr->m_p;
                ++(*this);
            }else
                m_ptr = nullptr;
        }if(m_ptr && m_ptr->get_children().m_head) {//if it is not a leaf
            while (m_ptr->get_children().m_head) {
                m_ptr = m_ptr->get_children().m_head->trie;
            }
        }
    } else{//in case m_ptr was pointing to the root
        m_ptr = nullptr;
    }
    return *this;
}

template <typename T>
typename trie<T>::const_leaf_iterator trie<T>::const_leaf_iterator::operator++(int dummy){
    auto temp = m_ptr;
    if(m_ptr && m_ptr->m_p) {//checking if m_ptr is pointing to nullpre or to the root
        auto pc = m_ptr->m_p->m_c.m_head;
        while (pc && pc->trie != m_ptr)
            pc = pc->next;
        if (pc && pc->next) {
            m_ptr = pc->next->trie;
        }
        else {
            if(m_ptr->m_p->m_p) {//if father is not root
                m_ptr = m_ptr->m_p;
                ++(*this);
            }else
                m_ptr = nullptr;
        }if(m_ptr && m_ptr->get_children().m_head) {//if it is not a leaf
            while (m_ptr->get_children().m_head) {
                m_ptr = m_ptr->get_children().m_head->trie;
            }
        }
    } else{//in case m_ptr was pointing to the root
        m_ptr = nullptr;
    }
    return temp;
}


template <typename T>
bool trie<T>::const_leaf_iterator::operator==(const_leaf_iterator const& rhs) const {
    return m_ptr == rhs.m_ptr;
}

template <typename T>
bool trie<T>::const_leaf_iterator::operator!=(const_leaf_iterator const& rhs) const {
    return m_ptr != rhs.m_ptr;
}

template <typename T>
trie<T>::const_leaf_iterator::operator const_node_iterator() const{
    const_node_iterator n_it(m_ptr);
    return n_it;
}

template <typename T>
typename trie<T>::const_leaf_iterator trie<T>::begin() const{
    if(m_c.m_head) { //if there is any child
        auto pc = m_c.m_head->trie;//first child of this
        while (pc->m_c.m_head)
            pc = pc->m_c.m_head->trie;//first child of pc
        return pc;
    }else {
        return nullptr;
    }
}
template <typename T>
typename trie<T>::const_leaf_iterator trie<T>::end() const{
    return nullptr;
}

template <typename T>
typename trie<T>::const_node_iterator trie<T>::root() const{
    const_node_iterator n_it(this);
    const_node_iterator pr(this);
    do {
        pr = n_it;
        n_it++;
    }while(n_it != pr);
    return n_it;
}

template <typename T>
trie<T> const& trie<T>::const_leaf_iterator::get_leaf() const{
    return *m_ptr;
}

template <typename T>
trie<T>& trie<T>::max(){
    int max = 0;
    trie<T>::leaf_iterator ret = begin();
    for(trie<T>::leaf_iterator it = begin(); it!= end(); ++it){
        if(it.get_leaf().get_weight()>max){
            max = it.get_leaf().get_weight();
            ret = it;
        }
    }
    return ret.get_leaf();
}
template <typename T>
trie<T> const& trie<T>::max() const{
    int max = 0;
    trie<T>::const_leaf_iterator ret = begin();
    for(trie<T>::const_leaf_iterator it = begin(); it!= end(); ++it){
        if(it.get_leaf().get_weight()>max){
            max = it.get_leaf().get_weight();
            ret = it;
        }
    }
    return ret.get_leaf();
}

template <typename T>
trie<T>& trie<T>::operator[](std::vector<T> const& v){
    trie<T>* ret = this;
    if(get_children().m_head && v.size()>0){
        int i = 0;
        for(auto it = get_children().begin(); it!= get_children().end() && i<v.size();){
            if(*(it->get_label())==v[i]) {
                ret = it.get_trie();
                if(it->get_children().m_head){
                    it = it->get_children().begin();
                }
                i++;
            }else
                ++it;
        }
    }
    return *ret;
}
template <typename T>
trie<T> const& trie<T>::operator[](std::vector<T> const& v) const{
    const trie<T>* ret = this;
    if(get_children().m_head && v.size()>0){
        int i = 0;
        for(auto it = get_children().begin(); it!= nullptr && i<v.size();){
            if(*(it->get_label())==v[i]) {
                ret = it.get_trie();
                if(it->get_children().m_head){
                    it = it->get_children().begin();
                }
                i++;
            }else
                ++it;
        }
    }
    return *ret;
}