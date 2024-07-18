//todo: capire perché non funzioni prefix iterator(da capire quale non funzioni)

#include <iostream>
#include "trie.hpp"

//parser
/*
 ALBERO -> LEAF | {BAG}
 BAG -> ALBERO | ALBERO, BAG | LEAF | LEAF, BAG | E
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
    child.set_label( &label);


    return child;
}


template <typename T>
trie<T> ALBERO(std::istream& is){
    skip_blank_spaces(is);


    trie<T> t;

    char i = is.peek();

    if(i=='-'|| (i>='0' && i<='9')){//ii's a root or a node with label type number
        t = LEAF<T>(is);
    }else {
        std::string s = "";
        is >> s;
        if (s != "children") {
            s = "expecting keyword \"children\" instead of \"" + s + "\"";
            throw parser_exception(s);
        }
        skip_blank_spaces(is);

        char c = '0';
        is >> c;
        if (c != '=') throw parser_exception("expecting symbol '='");
        skip_blank_spaces(is);

        is >> c;
        if (c != '{') throw parser_exception("expecting symbol '{'");
        skip_blank_spaces(is);
        c = is.peek();
        bool in = false;
        while (c != '}') {
            in = true;
            t.add_child(CHILD<T>(is));
            skip_blank_spaces(is);
            is >> c;

            if (c != '}' && c != ',') throw parser_exception("expecting symbol '}' or ','");
        }
        if (!in)
            throw parser_exception("expecting weight");
    }
    return t;
}


//implemntation of trie.hpp
//constructors
template <typename T>
trie<T>::trie() : m_p(nullptr), m_l(nullptr), m_c(), m_w(0.0) {}

template <typename T>
trie<T>::trie(double w) : m_p(nullptr), m_l(nullptr), m_c(), m_w(w) {}

template <typename T>
trie<T>::trie(trie<T> const& rhs) : m_p(nullptr), m_l(nullptr), m_c(), m_w(rhs.m_w){
    for(auto& it_c : rhs.m_c){
        add_child(it_c);
    }
}

//distructor
template <typename T>
trie<T>::~trie(){
    delete m_l;
}//automatically calls all destructors

template <typename T>
trie<T>::trie(trie<T>&& rhs): m_p(nullptr), m_l(nullptr), m_c(), m_w(rhs.m_w) {

    m_c.m_head = rhs.m_c.m_head;
    rhs.m_c.m_head = nullptr;

    for(auto& pc : m_c)
        pc.m_p = this;
}


//setters
template <typename T>
void trie<T>::set_weight(double w){
    m_w = w;
}
template <typename T>
void trie<T>::set_label(T* l){
    if(l) {
        delete m_l;
        m_l = new T(*l);
    }
    else
        m_l = nullptr;
}

template <typename T>
void trie<T>::set_parent(trie<T>* p){
    m_p = p;
}
//getters
template <typename T>
T* trie<T>::get_label(){
    //if root it will return nullptr
    return m_l;
}

template <typename T>
trie<T>* trie<T>::get_parent(){
    //if root it will return nullptr
    return m_p;
}

template <typename T>
bag<trie<T>>& trie<T>::get_children(){
    return m_c;
}

template <typename T>
double trie<T>::get_weight() const {
    return m_w;
}

//const getters
template <typename T>
T const* trie<T>::get_label() const{
    //if root it will return nullptr
    return m_l;
}

template <typename T>
trie<T> const* trie<T>::get_parent() const{
    //if root it will return nullptr
    return m_p;
}

template <typename T>
bag<trie<T>> const& trie<T>::get_children() const{
    return m_c;
}

//iterators
//node iterator
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
typename trie<T>::node_iterator trie<T>::node_iterator::operator++(int) {
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
    node_iterator ret = this;
    return ret;
}

//const node iterator
template <typename T>
trie<T>::const_node_iterator::const_node_iterator(trie<T> const* ptr) : m_ptr(ptr) {}

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
    if(m_ptr->m_p)
        m_ptr = m_ptr->m_p;
    return *this;
}

template <typename T>
typename trie<T>::const_node_iterator trie<T>::const_node_iterator::operator++(int) {
    auto ptr = m_ptr;
    if(m_ptr->m_p)
        m_ptr = m_ptr->m_p;
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

//leaf iterator
template <typename T>
trie<T>::leaf_iterator::leaf_iterator(trie<T>* ptr)  {
    m_ptr = ptr;
}

template <typename T>
typename trie<T>::leaf_iterator::reference trie<T>::leaf_iterator::operator*() const{
    return *(m_ptr->m_l);
}

template <typename T>
typename trie<T>::leaf_iterator::pointer trie<T>::leaf_iterator::operator->() const{
    return m_ptr->m_l;
}

template <typename T>
typename trie<T>::leaf_iterator& trie<T>::leaf_iterator::operator++(){
    if(m_ptr && m_ptr->m_p) {//checking if m_ptr is pointing to nullpre or to the root
        auto pc = m_ptr->m_p->m_c.m_head;
        while (pc && &(pc->trie) != m_ptr)
            pc = pc->next;
        if (pc->next) {
            m_ptr = &(pc->next->trie);
        }else {
            if(m_ptr->m_p->m_p) {//if father is not root
                m_ptr = m_ptr->m_p;
                ++(*this);
            }else
                m_ptr = nullptr;
        }if(m_ptr && m_ptr->m_c.m_head) {//if it is not a leaf
            while (m_ptr->m_c.m_head) {
                m_ptr = &(m_ptr->m_c.m_head->trie);
            }
        }
    } else{//in case m_ptr was pointing to the root
        m_ptr = nullptr;
    }
    return *this;
}

template <typename T>
typename trie<T>::leaf_iterator trie<T>::leaf_iterator::operator++(int){
    auto temp = m_ptr;
    if(m_ptr && m_ptr->m_p) {//checking if m_ptr is pointing to nullpre or to the root
        auto pc = m_ptr->m_p->m_c.m_head;
        while (pc && &(pc->trie) != m_ptr)
            pc = pc->next;
        if (pc->next) {
            m_ptr = &(pc->next->trie);
        }else {
            if(m_ptr->m_p->m_p) {//if father is not root
                m_ptr = m_ptr->m_p;
                ++(*this);
            }else
                m_ptr = nullptr;
        }if(m_ptr && m_ptr->m_c.m_head) {//if it is not a leaf
            while (m_ptr->m_c.m_head) {
                m_ptr = &(m_ptr->m_c.m_head->trie);
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
        auto pc = m_c.m_head;//first child of this
        while (pc->trie.m_c.m_head)
            pc = pc->trie.m_c.m_head;//first child of pc's trie
        return &(pc->trie);
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

//cobst leaf iterator
template <typename T>
trie<T>::const_leaf_iterator::const_leaf_iterator(trie<T> const* ptr) : m_ptr(ptr){}

template <typename T>
typename trie<T>::const_leaf_iterator::reference trie<T>::const_leaf_iterator::operator*() const{
    return *(m_ptr->m_l);
}

template <typename T>
typename trie<T>::const_leaf_iterator::pointer trie<T>::const_leaf_iterator::operator->() const{
    return m_ptr->m_l;
}

template <typename T>
typename trie<T>::const_leaf_iterator& trie<T>::const_leaf_iterator::operator++(){
    if(m_ptr && m_ptr->m_p) {//checking if m_ptr is pointing to nullpre or to the root
        auto pc = m_ptr->m_p->m_c.m_head;
        while (pc && &(pc->trie) != m_ptr)
            pc = pc->next;
        if (pc && pc->next) {
            m_ptr = &(pc->next->trie);
        }
        else {
            if(m_ptr->m_p->m_p) {//if father is not root
                m_ptr = m_ptr->m_p;
                ++(*this);
            }else
                m_ptr = nullptr;
        }
        if(m_ptr && m_ptr->m_c.m_head) {//if it is not a leaf
            while (m_ptr->m_c.m_head) {
                m_ptr = &(m_ptr->m_c.m_head->trie);
            }
        }
    } else{//in case m_ptr was pointing to the root
        m_ptr = nullptr;
    }
    return *this;
}

template <typename T>
typename trie<T>::const_leaf_iterator trie<T>::const_leaf_iterator::operator++(int){
    auto temp = m_ptr;
    if(m_ptr && m_ptr->m_p) {//checking if m_ptr is pointing to nullpre or to the root
        auto pc = m_ptr->m_p->m_c.m_head;
        while (pc && &(pc->trie) != m_ptr)
            pc = pc->next;
        if (pc && pc->next) {
            m_ptr = &(pc->next->trie);
        }
        else {
            if(m_ptr->m_p->m_p) {//if father is not root
                m_ptr = m_ptr->m_p;
                ++(*this);
            }else
                m_ptr = nullptr;
        }if(m_ptr && m_ptr->m_c.m_head) {//if it is not a leaf
            while (m_ptr->m_c.m_head) {
                m_ptr = &(m_ptr->m_c.m_head->trie);
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
    auto ret = this;
    if(m_c.m_head) { //if there is any child
        auto pc = m_c.m_head;//first child of this
        while (pc->trie.m_c.m_head)
            pc = pc->trie.m_c.m_head;//first child of pc
        ret = &(pc->trie);
    }
    return ret;
}
template <typename T>
typename trie<T>::const_leaf_iterator trie<T>::end() const{
    return nullptr;
}

template <typename T>
typename trie<T>::const_node_iterator trie<T>::root() const{
    const_node_iterator ret = this;
    return ret;
}

template <typename T>
trie<T> const& trie<T>::const_leaf_iterator::get_leaf() const{
    return *m_ptr;
}

//methods
template <typename T>
void trie<T>::add_child(trie<T> const& c){

    auto node = m_c.create_node(c);
    if(m_c.m_head) {
        if (*(c.m_l) < *(m_c.m_head->trie.m_l)) {//if i need to add it as the head
            m_c.bag_prepend(node);
        }else {
            if (*(c.m_l) == *(m_c.m_head->trie.m_l))
                throw parser_exception("It is not possible for 2 children with the same father to have the same label");
            else {
                auto pc = m_c.m_head;
                bool insert = false;
                while (pc->next && !insert) {
                    if (*(pc->next->trie.m_l) == *(c.m_l))
                        throw parser_exception(
                                "It is not possible for 2 children with the same father to have the same label");
                    else if (*(pc->next->trie.m_l) > *(c.m_l))
                        insert = true;
                    else
                        pc = pc->next;
                }
                node->next = pc->next;
                pc->next = node;
            }
        }
    } else {
        m_c.bag_prepend(node);
    }
    node->trie.m_p = this;
    node->trie.set_label(c.m_l);
    m_w = 0.0;//in case it was a leaf
}

template <typename T>
trie<T>& trie<T>::max(){
    double max = 0;
    trie<T>* ret = this;
    if(m_c.m_head) {
        trie<T>::leaf_iterator r = begin();
        for (trie<T>::leaf_iterator it = begin(); it != end(); ++it) {
            if (it.get_leaf().m_w > max) {
                max = it.get_leaf().m_w;
                r = it;
            }
        }
        return r.get_leaf();
    }
    return *ret;
}

template <typename T>
trie<T> const& trie<T>::max() const{
    double max = 0;
    const trie<T>* ret = this;
    if(m_c.m_head) {
        trie<T>::const_leaf_iterator r = nullptr;
        for (trie<T>::const_leaf_iterator it = begin(); it != end(); ++it) {
            if (it.get_leaf().m_w > max) {
                max = it.get_leaf().m_w;
                r = it;
            }
        }
        return r.get_leaf();
    }
    return *ret;
}

template <typename T>
trie<T>& trie<T>::operator[](std::vector<T> const& v){
    trie<T>* ret = this;
    if(m_c.m_head && v.size()>0){
        bool found = false;
        size_t i = 0;
        auto it = m_c.begin();
        while(!found && it != m_c.end() && i<v.size() ){
            if(*(it->m_l)==v[i]) {
                ret = &(it.get_trie());
                i++;
                if(i<v.size() && it->m_c.m_head){
                    it = it->m_c.begin();
                }else
                    found = true;
            }else
                ++it;
        }
    }
    return *ret;
}
template <typename T>
trie<T> const& trie<T>::operator[](std::vector<T> const& v) const{
    const trie<T>* ret = this;
    if(m_c.m_head && v.size()>0){
        bool found = false;
        size_t i = 0;
        auto it = m_c.begin();
        while(!found && it != m_c.end() && i<v.size() ){
            if(*(it->m_l)==v[i]) {
                ret = &(it.get_trie());
                i++;
                if(it->m_c.m_head && i<v.size()){
                    it = it->m_c.begin();
                }else
                    found = true;
            }else
                ++it;
        }
    }
    return *ret;
}

template <typename T>
trie<T>& trie<T>::operator=(trie<T> const& rhs){
    if(this!=&rhs){
        //m_p and m_l are not modified
        m_w = rhs.m_w;
        m_c.clear();
        for(auto& it_c : rhs.m_c){
            add_child(it_c);
        }
    }

    return *this;
}

//operators
template <typename T>
trie<T>& trie<T>::operator=(trie<T>&& rhs){
    //m_p and m_l are not modified

    m_w = rhs.m_w;
    auto temp = m_c.m_head;
    m_c.m_head = rhs.m_c.m_head;
    rhs.m_c.m_head = temp;

    for(auto& pc : m_c)
        pc.m_p = this;

    return *this;
}

template <typename T>
bool trie<T>::operator==(trie<T> const& rhs) const{
    bool ret = false;
    if(!m_c.m_head && !rhs.m_c.m_head){
        ret = (rhs.m_w==m_w);
    }else
        ret = m_c == rhs.m_c;

    return ret;
}

template <typename T>
bool trie<T>::operator!=(trie<T> const& rhs) const{
    return !(*this==rhs);
}

template <typename T>
std::ostream& operator<<(std::ostream& os, trie<T> const& t){
    if(t.get_children().m_head) {
        os << "children = { ";
        for (auto it = t.get_children().begin(); it != t.get_children().end(); ) {
            os << *(it->get_label())<< " ";
            if (!it->get_children().m_head) {
                os << it->get_weight() << " children = {}";
            } else {
                os << it.get_trie();
            }
            if (++it != t.get_children().end())
                os << ", ";
        }
    }else{
        os << t.get_weight() << " children = { ";
    }
    os << '}';

    return os;
}

template <typename T>
std::istream& operator>>(std::istream& is, trie<T>& t){
    t = ALBERO<T>(is);
    return is;
}

template <typename T>
trie<T> trie<T>::operator+(trie<T> const& rhs) const{
    trie<T> ret = *this;
    if(m_c.m_head){
        if(m_l)
            ret.m_l = new T(*m_l);
        if(rhs.m_c.m_head) {
            for (auto& r_it : rhs.m_c) {
                bool found = false;
                auto it = ret.m_c.begin();
                while (!found && it != ret.m_c.end()){
                    if(*(it->m_l)==*(r_it.m_l)){
                        found = true;
                        *it+=r_it;
                    }else
                        ++it;
                }
                if(!found){
                    ret.add_child(r_it);
                }
            }
        }else{
            for (auto& it : ret.m_c) {
                it+=rhs;
            }
        }
    }else if(rhs.m_c.m_head){
        ret = rhs;
        if(ret.m_l)
            ret.m_l = new T(*(ret.m_l));
        for(auto it = ret.begin() ; it != ret.end(); ++it){
            (it).get_leaf().m_w += m_w;
        }
    }else{
        ret.m_w += rhs.m_w;
    }

    return ret;
}

template <typename T>
trie<T>& trie<T>::operator+=(trie<T> const& rhs){
    (*this) = (*this) + rhs;

    return *this;
}

template <typename T>
void trie<T>::path_compress(){

    for(auto it = m_c.begin(); it!=m_c.end(); ){
        int children = 0;
        if(it->m_c.m_head) {
            if (!it->m_c.m_head->next)//only 1 child
                children = 1;
            else {
                (*it).path_compress();
            }
        }
        if(children==1){
            trie<T> child(((*it).m_c.m_head->trie));
            T l = *((*it).m_l) + *((*it).m_c.m_head->trie.m_l);
            child.set_label(&l);
            auto temp = it;
            ++it;
            m_c.delete_node(*temp);
            add_child(child);
        }else
            ++it;
    }
}