#ifndef BST_H
#define BST_H

#include <initializer_list>
#include <cstddef>
#include <stdexcept>
#include <queue>

template <typename T>
class BST {
public:
    struct Node {
        Node() : parent{}, left{}, right{}, value{} {}
        explicit Node(const T& v) : parent{}, left{}, right{}, value{v} {} 
        Node* parent;
        Node* left;
        Node* right;
        T value;
    };
    BST() : m_root{}, m_size{} {}
    BST(const std::initializer_list<T>& list) : BST() {
        for (const auto& el : list) {
            insert(el);
        }
    }
    BST(const BST& other) : BST() {
        auto copy = [this](const T& value) { this->insert(value); };
        other.inorderTraverse(copy);
    }
    BST(BST&& other) noexcept : BST() {
        swap(other);
    }
    BST& operator=(const BST& rhs) {
        auto tmp = rhs;
        swap(tmp);
        return *this;
    }
    BST& operator=(BST&& rhs) noexcept {
        if (&rhs == this) {
            return *this;
        }
        swap(rhs);
        rhs.clear();
        return *this;
    }
    void insert(const T& value) {
        if (empty()) {
            m_root = new Node(value);
        }
        else {
            Node* current = m_root;
            Node* parent = nullptr;
            while (current) {
                parent = current;
                if (current->value > value) {
                    current = current->left;
                }
                else {
                    current = current->right;
                }
            }
            if (value < parent->value) {
                parent->left = new Node(value);
                parent->left->parent = parent;
            }
            else {
                parent->right = new Node(value);
                parent->right->parent = parent;
            }
        }
        ++m_size;
    }
    void clear() {
        doClear(m_root);
        m_root = nullptr;
        m_size = 0;
    }
    void swap(BST& other) noexcept {
        std::swap(m_size, other.m_size);
        std::swap(m_root, other.m_root);
    }
    void remove(const T& value) {
        removeNode(getNode(value, m_root));
    }
    template <typename F>
   // typename std::enable_if_t<std::is_invocable_v<F>, void>
    void inorderTraverse(F f) const {
        doInorder(f, m_root);
    }
    template <typename F>
    void preorderTraverse(F f) const {
        doPreorder(f, m_root);
    }
    template <typename F>
    void postorderTraverse(F f) const {
        doPostorder(f, m_root);
    }
    template <typename F>
    void levelorderTraverse(F f) const {
        doLevelorder(f, m_root);
    }
    const T& max() const {
        if (empty()) {
            throw std::logic_error("BST is empty. Fail to get max value");
        }
        return getMax(m_root)->value;
    }
    const T& min() const {
        if (empty()) {
            throw std::logic_error("BST is empty. Fail to get min value");
        }
        return getMin(m_root)->value;
    }
    bool search(const T& value) { return getNode(value, m_root); }
    const T& pedecessorOf(const T& value) const {
        if (empty()) {
            throw std::logic_error("BST is empty. Fail to get pedecessor of value");
        }
        auto res = getPedecessor(getNode(value, m_root));
        return res ? res->value : value; // If nullptr return same value
    }
    const T& successorOf(const T& value) const {
        if (empty()) {
            throw std::logic_error("BST is empty. Fail to successor of value");
        }
        auto res = getSuccessor(getNode(value, m_root));
        return res ? res->value : value; // If nullptr return same value
    }
    size_t height() const { return !empty() ? getHeightHelper(m_root) - 1 : 0; }
    constexpr size_t size() const { return m_size; }
    constexpr bool empty() const { return !m_size; }
    ~BST() { clear(); }
private:
    template <typename F>
    static void doInorder(F f, Node* node) {
        if (!node) {
            return;
        }
        doInorder(f, node->left);
        f(node->value);
        doInorder(f, node->right);
    }
    template <typename F>
    static void doPreorder(F f, Node* node) {
        if (!node) {
            return;
        }
        f(node->value);
        doPreorder(f, node->left);
        doPreorder(f, node->right);
    }
    template <typename F>
    static void doPostorder(F f, Node* node) {
        if (!node) {
            return;
        }
        doPostorder(f, node->left);
        doPostorder(f, node->right);
        f(node->value);
    }
    template <typename F>
    static void doLevelorder(F f, Node* node) { 
        if (!node) {
            return;
        }
        std::queue<Node*> queue;
        queue.push(node);
        while (!queue.empty()) {
            auto tmp = queue.front();
            queue.pop();
            f(tmp->value);
            if (tmp->left) {
                queue.push(tmp->left);
            }
            if (tmp->right) {
                queue.push(tmp->right);
            }
        }
    }
    void doClear(Node* node) {
         if (!node) {
            return;
        }
        doClear(node->left);
        doClear(node->right);
        delete node;
    }
    void transplant(Node* dest, Node* node) {
        if (!dest) {
            return;
        }
        if (!dest->parent) {
            m_root = node;
        }
        else if (dest == dest->parent->left) {
            dest->parent->left = node;
        }
        else {
            dest->parent->right = node;
        }
        if (node) {
            node->parent = dest->parent;
        }
    }
    void removeNode(Node* node) {
        if (!node) {
            return;
        }
        if (!node->left) {
            transplant(node, node->right);
        }
        else if (!node->right) {
            transplant(node, node->left);
        }
        else {
            Node* successor = getMin(node->right);
            if (successor != node->right) {
                transplant(successor, successor->right);
                successor->right = node->right;
                successor->right->parent = successor;
            }
            transplant(node, successor);
            successor->left = node->left;
            successor->left->parent = successor;
        }
        delete node;
        --m_size;
    }
    static Node* getSuccessor(Node* node) {
        if (!node) {
            return nullptr;
        }
        if (node->right) {
            return getMin(node->right);
        }
        while (node->parent && node == node->parent->right) {
            node = node->parent;
        }
        return node->parent;
    }
    static Node* getPedecessor(Node* node) {
        if (!node) {
            return nullptr;
        }
        if (node->left) {
            return getMax(node->left);
        }
        while (node->parent && node == node->parent->left) {
            node = node->parent;
        }
        return node->parent;
    }
    static Node* getMax(Node* node) {
        if (!node) {
            return nullptr;
        }
        while (node->right) {
            node = node->right;
        }
        return node;
    }
    static Node* getMin(Node* node) {
        if (!node) {
            return nullptr;
        }
        while (node->left) {
            node = node->left;
        }
        return node;
    }
    static Node* getNode(const T& value, Node* node) {
        while (node && node->value != value) {
            if (value > node->value) {
                node = node->right;
            }
            else {
                node = node->left;
            }
        }
        return node;
    }
    static size_t getHeightHelper(Node* node) {
        if (!node) {
            return 0;
        }
        return 1 + std::max(getHeightHelper(node->left), getHeightHelper(node->right));
    } 
    Node* m_root;
    size_t m_size;    
};

#endif
