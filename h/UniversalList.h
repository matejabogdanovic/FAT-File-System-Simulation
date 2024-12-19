#pragma  once

/**
 * @brief Can be used as list, queue, stack.
 * @tparam T
 */
template<typename T>
class UniversalList {
public:
    unsigned size() const;

    T peekFirst() const;

    T peekLast() const;

    void addLast(const T &data);

    void addFirst(const T &data);

    T removeLast();

    T removeFirst();

    void clear();

    ~UniversalList();

private:
    struct Node {
        T data;
        Node *next, *prev;

        explicit Node(T data, Node *next = nullptr, Node *prev = nullptr) : data(data), next(next), prev(prev) { }
    };

    unsigned sz = 0;
    Node *head = nullptr, *tail = nullptr;
};

template<typename T>
inline T UniversalList<T>::peekFirst() const {
    return (head ? head->data : nullptr);
}

template<typename T>
inline T UniversalList<T>::peekLast() const {
    return (tail ? tail->data : nullptr);
}

template<typename T>
UniversalList<T>::~UniversalList() {
    clear();
}

template<typename T>
inline unsigned UniversalList<T>::size() const {
    return sz;
}

template<typename T>
void UniversalList<T>::clear() {
    while(head) {
        Node *prev = head;
        head = head->next;
        delete prev;
    }
    tail = nullptr;
}

template<typename T>
void UniversalList<T>::addFirst(const T &data) {
    Node *node = new Node(data);

    if(!head) {
        head = tail = node;
    } else {
        head->prev = node; // link backwards
        node->next = head; // link forwards
        head = node;
    }


    sz++;
}

template<typename T>
void UniversalList<T>::addLast(const T &data) {
    Node *node = new Node(data);

    if(!head) {
        head = tail = node;
    } else {
        Node *prev = tail;
        tail = tail->next = node; // link forwards
        node->prev = prev; // link backwards
    }


    sz++;
}

template<typename T>
T UniversalList<T>::removeFirst() {
    if(!head)return 0;

    Node *node = head;
    T data = head->data;

    head = (head == tail) ? tail = nullptr : head->next;


    delete node;

    sz--;
    return data;
}

template<typename T>
T UniversalList<T>::removeLast() {
    if(!tail)return 0;

    Node *node = tail;
    T data = tail->data;

    tail = (head == tail) ? head = nullptr : tail->prev;


    delete node;

    sz--;
    return data;
}
