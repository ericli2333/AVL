#include <bits/stdc++.h>
/*
 * 记住，bf是右减左
 */

using namespace std;

struct Node {
    Node *leftChild;
    Node *rightChild;
    Node *parent;
    int value;
    int bf;

    Node(int val_ = 0) : value(val_), leftChild(nullptr), rightChild(nullptr), parent(nullptr), bf(0) {};

    void print() {
        cout << "Node at " << this << " Value = " << value << " bf = " << bf << endl;
    }
};

class AVL {
private:
    Node *root;
    void roteLeft(Node *A, Node *C);
    void roteRight(Node *A, Node *B);
    void print(Node *node);
    void print2(Node *node);
public:
    bool insert(int val);
    AVL();
    void print();
    void print2();
};

AVL::AVL() : root(nullptr) {}

/*
 * 此处的A和C的定义和PPT上的一致，C应为A的右子节点
 * 注意在左旋的过程中不做balance factor的调整，只完成连接过程
 */
void AVL::roteLeft(Node *A, Node *C) {
//    cout << "Rote left \n";
    if (A == this->root) {
        //处理A是根节点的情况
        Node *B = A->leftChild;
        Node *D = C->leftChild;
        Node *E = C->rightChild;
        //开始重新连接
        C->leftChild = A;
        C->rightChild = E;
        A->leftChild = B;
        A->rightChild = D;
        //重新设立父节点
        A->parent = C;
        if (B != nullptr)
            B->parent = A;
        C->parent = nullptr;
        if (D != nullptr)
            D->parent = A;
        E->parent = C;
        this->root = C;
    } else {
        Node *parent = A->parent;
        bool isLeft = (A == parent->leftChild);
        Node *B = A->leftChild;
        Node *D = C->leftChild;
        Node *E = C->rightChild;
        //开始重新连接
        C->leftChild = A;
        C->rightChild = E;
        A->leftChild = B;
        A->rightChild = D;
        //重新设立父节点
        A->parent = C;
        if (B != nullptr)
            B->parent = A;
        C->parent = parent;
        if (D != nullptr)
            D->parent = A;
        E->parent = C;
        //连接上一个父节点
        if (isLeft) {
            parent->leftChild = C;
        } else {
            parent->rightChild = C;
        }
    }
}

/*
 *B是A的左子节点，同样只做连接，不做balance factor的调整
 */
void AVL::roteRight(Node *A, Node *B) {
//    cout << "rote right \n";
    if (A == this->root) {
        //TODO:记得处理A是根节点
        Node *D = B->leftChild;
        Node *E = B->rightChild;
        Node *C = A->rightChild;
        //重新连接
        B->leftChild = D;
        B->rightChild = A;
        A->leftChild = E;
        A->rightChild = C;
        //重新设立父节点的连接
        A->parent = B;
        B->parent = nullptr;
        if (C != nullptr)
            C->parent = A;
        D->parent = B;
        if (E != nullptr)
            E->parent = A;
        this->root = B;
    } else {
        Node *parent = A->parent;
        bool isLeft = (A == parent->leftChild);
        Node *D = B->leftChild;
        Node *E = B->rightChild;
        Node *C = A->rightChild;
        //重新连接
        B->leftChild = D;
        B->rightChild = A;
        A->leftChild = E;
        A->rightChild = C;
        //重新设立父节点的连接
        A->parent = B;
        B->parent = parent;
        if (C != nullptr)
            C->parent = A;
        D->parent = B;
        if (E != nullptr)
            E->parent = A;
        if (isLeft) {
            parent->leftChild = B;
        } else {
            parent->rightChild = B;
        }
    }
}

bool AVL::insert(int val) {
    Node *insertNode = new Node(val);
    //处理空树的情况
    if (this->root == nullptr) {
        this->root = insertNode;
        return true;
    } else {
        Node *cur = this->root;
        Node *parentOfCur = nullptr;
        Node *lastNonZero = nullptr;
        Node *parentOfLastNonZero = nullptr;
        //搜索插入节点的位置
        while (cur != nullptr) {
            if (cur->bf != 0) {
                lastNonZero = cur;
                parentOfLastNonZero = parentOfCur;
            }
            if (cur->value < val) {
                parentOfCur = cur;
                cur = cur->rightChild;
            } else if (cur->value == val) {
                return false;
            } else {
                parentOfCur = cur;
                cur = cur->leftChild;
            }
        }
        if (parentOfCur->value > val) {
            parentOfCur->leftChild = insertNode;
            insertNode->parent = parentOfCur;
        } else {
            parentOfCur->rightChild = insertNode;
            insertNode->parent = parentOfCur;
        }
        //开始判断bf的调整
        if (lastNonZero == nullptr) {
//            cout << "Modify on root \n";
            cur = this->root;
            lastNonZero = root;
            while (cur != nullptr) {
                if (cur->value < val) {
                    cur->bf++;
                    cur = cur->rightChild;
                } else if (cur->value == val) {
                    //注意这里应该是搜索到了插入的节点
                    break;
                } else {
                    cur->bf--;
                    cur = cur->leftChild;
                }
            }
//            cout << "The bf of last none zero is " << lastNonZero->bf << endl;
            if (lastNonZero->bf == 2) {

                if (lastNonZero->rightChild->bf == 1) {
                    Node *A = lastNonZero;
                    Node *C = lastNonZero->rightChild;
                    roteLeft(lastNonZero, lastNonZero->rightChild);
                    //调整bf
                    A->bf = 0;
                    C->bf = 0;
                } else {
                    Node *A = lastNonZero;
                    Node *C = A->rightChild;
                    Node *D = C->leftChild;
                    roteRight(C, D);
                    roteLeft(A, D);
                    A->bf = -1;
                    D->bf = 0;
                    C->bf = 0;
                }
            } else if (lastNonZero->bf == -2) {
                if (lastNonZero->rightChild->bf == -1) {
                    Node *A = lastNonZero;
                    Node *B = A->leftChild;
                    roteRight(A, B);
                    A->bf = 0;
                    B->bf = 0;
                } else {
                    Node *A = lastNonZero;
                    Node *B = A->leftChild;
                    Node *E = B->rightChild;
                    roteLeft(B, E);
                    roteRight(A, E);
                    A->bf = -1;
                    B->bf = 0;
                    E->bf = 0;
                }
            }
        } else {
//            cout << "Modify on node " << lastNonZero->value << endl;
//            lastNonZero->print();
            cur = lastNonZero;
            while (cur != nullptr) {
                if (cur->value < val) {
                    cur->bf++;
                    cur = cur->rightChild;
                } else if (cur->value == val) {
                    //注意这里应该是搜索到了插入的节点
                    break;
                } else {
                    cur->bf--;
                    cur = cur->leftChild;
                }
            }
//            cout << "The bf of last none zero is " << lastNonZero->bf << endl;
            if (lastNonZero->bf == 2) {

                if (lastNonZero->rightChild->bf == 1) {
                    Node *A = lastNonZero;
                    Node *C = lastNonZero->rightChild;
                    roteLeft(lastNonZero, lastNonZero->rightChild);
                    //调整bf
                    A->bf = 0;
                    C->bf = 0;
                } else {
                    Node *A = lastNonZero;
                    Node *C = A->rightChild;
                    Node *D = C->leftChild;
                    roteRight(C, D);
                    roteLeft(A, D);
                    A->bf = -1;
                    D->bf = 0;
                    C->bf = 0;
                }
            } else if (lastNonZero->bf == -2) {
                if (lastNonZero->leftChild->bf == -1) {
                    Node *A = lastNonZero;
                    Node *B = A->leftChild;
                    roteRight(A, B);
                    A->bf = 0;
                    B->bf = 0;
                } else {
                    Node *A = lastNonZero;
                    Node *B = A->leftChild;
                    Node *E = B->rightChild;
                    roteLeft(B, E);
                    roteRight(A, E);
                    A->bf = -1;
                    B->bf = 0;
                    E->bf = 0;
                }
            }
        }
    }
    return true;
}

void AVL::print() {
    if (this->root == nullptr) {
        cout << "Empty Tree\n";
        return;
    }
    print(this->root);
    cout << endl;
}

void AVL::print(Node *node) {
    if (node == nullptr) return;
    print(node->leftChild);
    cout << node->value << " ";
    print(node->rightChild);
}

void AVL::print2() {
    if (this->root == nullptr) {
        cout << "Empty Tree\n";
        return;
    }
    cout << "root is " << this->root->value << endl;
    print2(this->root);
    cout << endl;
}

void AVL::print2(Node *node) {
    if (node == nullptr) return;
    print2(node->leftChild);
    cout << node->value << " Left child is ";
    if (node->leftChild == nullptr) {
        cout << " nullptr ";
    } else {
        cout << node->leftChild->value << ' ';
    }
    cout << " Right child is ";
    if (node->rightChild == nullptr) {
        cout << "nullptr ";
    } else {
        cout << node->rightChild->value << ' ';
    }
    cout << "bf = " << node->bf;
    cout << endl;
    print2(node->rightChild);
}

int main() {
    AVL avl;
//    avl.insert(1);
//    avl.print2();
//    cout << "Insert 3" << endl;
//    avl.insert(3);
//    avl.print2();
//    cout << "Insert 5" << endl;
//    avl.insert(5);
//    avl.print2();
//    cout << "Insert 2" << endl;
//    avl.insert(2);
//    avl.print2();
//    cout << "Insert 4" << endl;
//    avl.insert(4);
//    for (int i = 6; i < 10; ++i) {
//        avl.insert(i);
//    }
    for (int i = 10; i > 0; --i) {
        avl.insert(i);
    }
    avl.print();
    avl.print2();
    return 0;
}
