#pragma  once

template<typename T>
class Queue {
public:
    unsigned size() const;

    T peekFirst() const;

    void addLast(const T &data);

    T removeFirst();

    void clear();

    ~Queue();

private:
    struct Node {
        T data;
        Node *next;

        explicit Node(T data, Node *next = nullptr) : data(data), next(next) { }
    };

    unsigned sz = 0;
    Node *head = nullptr, *tail = nullptr;
};

template<typename T>
inline T Queue<T>::peekFirst() const {
    return (head ? head->data : nullptr);
}

template<typename T>
Queue<T>::~Queue() {
    clear();
}

template<typename T>
void Queue<T>::clear() {
    while(head) {
        Node *prev = head;
        head = head->next;
        delete prev;
    }
    tail = nullptr;
}

template<typename T>
inline unsigned Queue<T>::size() const {
    return sz;
}

template<typename T>
void Queue<T>::addLast(const T &data) {
    Node *node = new Node(data);

    if(!head)
        head = tail = node;
    else
        tail = tail->next = node;

    sz++;
}

template<typename T>
T Queue<T>::removeFirst() {
    if(!head)return 0;

    Node *node = head;
    T data = head->data;

    head = (head == tail) ? tail = nullptr : head->next;

    delete node;

    sz--;
    return data;
}

