#include "bits/stdc++.h"
int cnt = 0;
template<class KeyType, class ValueType, class Hash = std::hash<KeyType> > class HashMap{
    using KV = std::pair<const KeyType,ValueType>;
    struct Node{
        size_t bucket_index;
        Node* last = nullptr;
        Node* next = nullptr;
        size_t key_hash;
        KV key_value;

        Node(HashMap* mp, const KV& key_value , size_t bucket_index = 0): bucket_index(bucket_index), key_value(key_value){
            key_hash = mp->hasher_(this->key_value.first);
        }
        bool operator==(const Node& rhs){
            if (key_hash != rhs.key_hash){
                return false;
            }
            return key_value.first == rhs.key;
        }
        bool does_same_as(const KeyType& rhs_key){
            return key_value.first == rhs_key;
        }
        bool does_same_as(const KeyType& rhs_key, size_t rhs_hash){

            if (key_hash != rhs_hash){
                return false;
            }
            return does_same_as(rhs_key);
        }
    };
    Hash hasher_;
    size_t capacity_ = 1;
    size_t size_ = 0;
    Node* last_element_ = nullptr;
    Node* first_element_ = nullptr;
    std::vector<Node*>buckets_ = std::vector<Node*>(capacity_, nullptr);

    int get_psl(int index){
        if (buckets_[index] == nullptr){
            return -1;
        }
        return (capacity_ + index - buckets_[index]->key_hash%capacity_)%capacity_;
    }
    void delete_all_nodes(){
        Node* element = first_element_;
        if (element != nullptr){
            while (element->next != nullptr){
                element = element->next;
                buckets_[element->last->bucket_index] = nullptr;
                delete element->last;
            }
            buckets_[last_element_->bucket_index] = nullptr;
            delete last_element_;
        }
        size_ = 0;
        first_element_ = nullptr;
        last_element_ = nullptr;
    }
    void resize(){
        capacity_*=2;
        buckets_ = std::vector<Node*>(capacity_, nullptr);
        auto element = first_element_;
        int iter = 0;
        while (element != nullptr){
            iter++;
            insert(element, false);
            element = element->next;
        }

    }
public:

    struct iterator {
        Node* node = nullptr;
        iterator() = default;
        iterator(const Node& node): node(&node){
        }
        iterator(Node* node): node(node){
        }

        iterator operator++(){
            node = node->next;
            return *this;
        }
        iterator operator++(int){
            auto copy = *this;
            node = node->next;
            return copy;
        }
        KV& operator*(){
            return (node->key_value);
        }

        bool operator==(const iterator &rhs){
            return (this->node == rhs.node);
        }
        bool operator!=(const iterator &rhs){
            return this->node != rhs.node;
        }
        KV* operator->(){
            return &(node->key_value);
        }
    };
    struct const_iterator {
        Node* node = nullptr;
        const_iterator() = default;
        const_iterator(const Node& node): node(&node){
        }
        const_iterator(Node* node): node(node){
        }

        const_iterator operator++() {
            node = node->next;
            return *this;
        }
        const_iterator operator++(int){
            auto copy = *this;
            node = node->next;
            return copy;
        }
        const KV& operator*(){
            return (node->key_value);
        }

        bool operator==(const const_iterator &rhs){
            return (this->node == rhs.node);
        }
        bool operator!=(const const_iterator &rhs){
            return this->node != rhs.node;
        }
        KV* operator->(){
            return &(node->key_value);
        }
    };

    iterator begin(){
        return iterator(first_element_);
    }
    iterator end() {
        return iterator(nullptr);
    }

    const_iterator begin() const{
        return const_iterator(first_element_);
    }
    const_iterator end() const {
        return const_iterator(nullptr);
    }
    HashMap(){
        hasher_ = Hash();
    }
    HashMap(Hash hasher): hasher_(hasher){

    }
    HashMap(const HashMap&rhs){
        this->operator=(rhs);
    }

    HashMap(std::initializer_list<std::pair<const KeyType, ValueType>> init_list){
        capacity_ = init_list.size()*4;
        buckets_.resize(capacity_, nullptr);
        for (auto el: init_list){
            insert(el);
        }
    }

    template<class Forward_itarator>
    HashMap(Forward_itarator begin, Forward_itarator end, Hash hash = Hash()): hasher_(hash){
        for (auto it = begin; it != end; ++it){
            insert(*it);
        }
    }

    ~HashMap(){
        delete_all_nodes();
    }
    size_t size() const {
        return size_;
    }
    bool empty() const{
        return size_ == 0;
    }
    Hash hash_function() const{
        return hasher_;
    }
    void insert(const KV& key_value){
        insert(new Node(this, key_value));
    }
    void insert(Node *node, bool add_to_linked_list = true){
        if (find(node->key_value.first) != end()){
            delete node;
            return;
        }
        if (add_to_linked_list) {
            bool resized = add(node);
            if (resized){
                return;
            }
        }
        int index = node->key_hash % capacity_;
        int current_pls = 0;
        while (node != nullptr){
            if (get_psl(index) < current_pls){
                current_pls = get_psl(index);
                node->bucket_index = index;
                std::swap(node, buckets_[index]);
            }
            current_pls++;
            index++;
            index%=capacity_;
        }
    }
    void erase(const KeyType& key){
        iterator iter = find(key);
        if (iter != end()){
            erase(iter);
        }
    }
    void erase(const iterator& iter){

        size_t index = iter.node->bucket_index;
        if (buckets_[index] == nullptr){
            delete iter.node;
            return;
        } else {
            remove(buckets_[index]);
            delete buckets_[index];
            buckets_[index] = nullptr;

            int new_index = (index+1) % capacity_;
            while (get_psl(new_index) > 0){
                std::swap(buckets_[index], buckets_[new_index]);
                buckets_[index]->bucket_index = index;

                index = (index+1) % capacity_;
                new_index = (new_index+1) % capacity_;
            }

        }

    }
    bool add(Node* node){
        if (size_ == 0){
            first_element_ = node;
        } else {
            last_element_->next = node;
            node->last = last_element_;
        }
        last_element_ = node;
        ++size_;
        if (size_ > capacity_*0.8){
            resize();
            return true;
        }
        return false;
    }
    void remove(Node* node){
        --size_;
        if (node->next != nullptr){
            node->next->last = node->last;
        } else {
            last_element_ = node->last;
        }
        if (node->last != nullptr){
            node->last->next = node->next;
        } else {
            first_element_ = node->next;
        }
    }
    void clear(){
        delete_all_nodes();
    }
    //TODO: дублируется код, хуета
    iterator find(const KeyType& key){
        size_t hash = hasher_(key);
        size_t index = hash % capacity_;
        for (size_t iter = 0 ; iter < capacity_; iter++){
            int current_index = (index + iter) % capacity_;
            if (buckets_[current_index] == nullptr){
                return end(); // false;
            }
            if (buckets_[current_index]->does_same_as(key, hash)){
                return iterator(buckets_[current_index]); // true
            }
        };
        return end();
    }
    const_iterator find(const KeyType& key) const{
        size_t hash = hasher_(key);
        size_t index = hash % capacity_;
        for (size_t iter = 0 ; iter < capacity_; iter++){
            int current_index = (index + iter) % capacity_;
            if (buckets_[current_index] == nullptr){
                return end(); // false;
            }
            if (buckets_[current_index]->does_same_as(key, hash)){
                return const_iterator(buckets_[current_index]); // true
            }
        };
        return end();
    }

    ValueType& operator[](const KeyType& key){
        auto iter = find(key);
        if (iter == nullptr){
            insert({key, ValueType()});
        }
        return find(key).node->key_value.second;
    }
    const ValueType& at(const KeyType& key) const{
        auto iter = find(key);
        if (iter != end()){
            return (*iter).second;
        } else {
            throw std::out_of_range("out of range");
        }
    }
    HashMap<KeyType, ValueType, Hash>& operator=(const HashMap<KeyType, ValueType, Hash>&rhs){
        if (this == &rhs){
            // антидебил
            return *this;
        }
        buckets_ = std::vector<Node*>(rhs.capacity_);
        capacity_ = rhs.capacity_;
        size_ = rhs.size_;
        hasher_ = rhs.hasher_;
        Node* element = rhs.first_element_;
        if (element != nullptr){
            while (element != nullptr){
                auto newNode = new Node(this, element->key_value, element->bucket_index);
                buckets_[element->bucket_index] = newNode;
                if (first_element_ == nullptr){
                    first_element_ = newNode;
                } else {
                    last_element_->next = newNode;
                    newNode->last = last_element_;
                }
                last_element_ = newNode;
                element = element->next;
            }
        }
        return *this;
    }

};
