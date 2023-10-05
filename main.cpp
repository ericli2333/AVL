#include <bits/stdc++.h>
//TODO:插入部分还有一个地方的bf修正有问题
//#define DEBUG
//#define TEST
#define SUBMIT
#define T2

#include <utility>
/*
 * 记住，bf是右减左
 */

using namespace std;


class Exception : std::exception {
private:
    std::string msg;
    int code;
public:
    static const int NO_NODE_TO_DELETE = 0x1;
    static const int CANNOT_REACH_HERE = 0x2;

    explicit Exception(const char *message, int code = 0) : msg(message), code(code) {}

    explicit Exception(std::string message, int code = 0) : msg(std::move(message)), code(code) {}

    [[nodiscard]] const char *what() const noexcept override {
        return msg.c_str();
    }

    int getcode() { return code; }
};

struct Node {
    Node *leftChild;
    Node *rightChild;
    Node *parent;
    int value;
    int bf;

    explicit Node(int val_ = 0) : value(val_), leftChild(nullptr), rightChild(nullptr), parent(nullptr), bf(0) {};

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
    void printAns(Node *node);
    static bool isShorterOnLeft(Node *node);
    static bool isShorterOnRight(Node *node);
    Node *findNoShorter(int val, Node *&toDelete, Node *&theNode);
public:
    bool Insert(int val);
    bool Delete(int val_);
    AVL();
    void print();
    void print2();
    void printAns();
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
        if (E != nullptr)
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
        if (E != nullptr)
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
        //处理A是根节点
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
        if (D != nullptr)
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

bool AVL::Insert(int val) {
    Node *insertNode = new Node(val);
    //处理空树的情况
    if (this->root == nullptr) {
        this->root = insertNode;
        return true;
    } else {
        Node *cur = this->root;
        Node *parentOfCur = nullptr;
        Node *lastNonZero = nullptr;
        //搜索插入节点的位置
        while (cur != nullptr) {
            if (cur->bf != 0) {
                lastNonZero = cur;
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
        } else {
//            cout << "Modify on node " << lastNonZero->value << endl;
//            lastNonZero->print();
            cur = lastNonZero;
        }
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
            //向右侧插入

            if (lastNonZero->rightChild->bf == 1) {
                Node *A = lastNonZero;
                Node *C = lastNonZero->rightChild;
                roteLeft(lastNonZero, lastNonZero->rightChild);
                //调整bf
                A->bf = 0;
                C->bf = 0;
            } else if (lastNonZero->rightChild->bf == -1){
                Node *A = lastNonZero;
                Node *C = A->rightChild;
                Node *D = C->leftChild;
                roteRight(C, D);
                roteLeft(A, D);
                if (D->bf == 0) {
                    A->bf = D->bf = C->bf = 0;
                } else if (D->bf == 1) {
                    A->bf = -1;
                    D->bf = C->bf = 0;
                } else if (D->bf == -1) {
                    C->bf = -1;
                    A->bf = D->bf = 0;
                }
            } else {
                Node *A = lastNonZero;
                Node *C = lastNonZero->rightChild;
                roteLeft(lastNonZero, lastNonZero->rightChild);
                //调整bf
                A->bf = 1;
                C->bf = -1;
            }
        } else if (lastNonZero->bf == -2) {
            if (lastNonZero->leftChild->bf == -1) {
                Node *A = lastNonZero;
                Node *B = A->leftChild;
                roteRight(A, B);
                A->bf = 0;
                B->bf = 0;
            } else if (lastNonZero->leftChild->bf == 1){
                Node *A = lastNonZero;
                Node *B = A->leftChild;
                Node *E = B->rightChild;
                roteLeft(B, E);
                roteRight(A, E);
                if (E->bf == 0) {
                    A->bf = B->bf = E->bf = 0;
                } else if (E->bf == 1) {
                    E->bf = A->bf = 0;
                    B->bf = 1;
                } else if (E->bf == -1) {
                    A->bf = 1;
                    B->bf = 0;
                    E->bf = 0;
                } else throw Exception("Not a valid bf",Exception::CANNOT_REACH_HERE);
            } else {
                Node *A = lastNonZero;
                Node *B = A->leftChild;
                roteRight(A, B);
                A->bf = -1;
                B->bf = 1;
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

bool AVL::Delete(int val) {
#ifdef DEBUG
    cout << "Start delete\n";
#endif
    Node *toDelete = nullptr;
    Node *noShorter = nullptr;
    Node *theNode = nullptr;
    try {
        noShorter = findNoShorter(val, toDelete, theNode);
    }
    catch (Exception &exp) {
        if (exp.getcode() & Exception::NO_NODE_TO_DELETE) {
            return false;
        } else throw std::move(exp);
    }
#ifdef DEBUG
    cout << "no shorter node is\n";
    noShorter->print();
    cout << "to delete node is\n";
    toDelete->print();
#endif
    int val_ = toDelete->value;
    Node *cur = noShorter;
    while (true) {
//        cout << "now cur is\n";
//        cur->print();
        if (cur == toDelete) {
            bool isLeft = (cur == cur->parent->leftChild);
            if (cur->leftChild == nullptr && cur->rightChild == nullptr) {
                if (isLeft) cur->parent->leftChild = nullptr;
                else cur->parent->rightChild = nullptr;
                delete cur;
                theNode->value = val_;
                return true;
            } else if (cur->leftChild == nullptr) {
                if (isLeft) cur->parent->leftChild = cur->rightChild;
                else cur->parent->rightChild = cur->rightChild;
                delete cur;
                theNode->value = val_;
                return true;
            } else if (cur->rightChild == nullptr) {
                if (isLeft) cur->parent->leftChild = cur->leftChild;
                else cur->parent->rightChild = cur->leftChild;
                delete cur;
                theNode->value = val_;
                return true;
            } else throw Exception("delete node with two child", Exception::CANNOT_REACH_HERE);
            break;
        }
        if (cur->value > val_) {
            if (cur->bf <= 0) {
                cur->bf++;
                cur = cur->leftChild;
            } else {
                if (cur->rightChild->bf == 1) {
                    Node *P = cur;
                    Node *Q = cur->rightChild;
                    roteLeft(P, Q);
                    P->bf = 0;
                    Q->bf = 0;
                } else if (cur->rightChild->bf == -1) {
                    Node *P = cur;
                    Node *Q = cur->rightChild;
                    Node *R = Q->leftChild;
                    roteRight(Q, R);
                    roteLeft(P, R);
                    P->bf = R->bf == 1 ? -1 : 0;
                    Q->bf = R->bf == -1 ? 1 : 0;
                    R->bf = 0;
                } else {
                    Node *P = cur;
                    Node *Q = P->rightChild;
                    roteLeft(P, Q);
                    Q->bf = -1;
                    P->bf = 1;
                }
                cur = cur->leftChild;
            }
        } else if (cur->value < val_) {
            if (cur->bf >= 0) {
                cur->bf--;
                cur = cur->rightChild;
            } else {
                if (cur->leftChild->bf == -1) {
                    Node *P = cur;
                    Node *Q = cur->leftChild;
                    roteRight(P, Q);
                    P->bf = 0;
                    Q->bf = 0;
                } else if (cur->leftChild->bf == 1) {
                    Node *P = cur;
                    Node *Q = cur->leftChild;
                    Node *R = Q->rightChild;
                    roteLeft(Q, R);
                    roteRight(P, R);
                    if (R->bf == 0) {
                        P->bf = Q->bf = R->bf = 0;
                    } else if (R->bf == 1) {
                        R->bf = 0;
                        P->bf = 0;
                        Q->bf = -1;
                    } else {
                        R->bf = 0;
                        Q->bf = 0;
                        P->bf = 1;
                    }
                } else {
                    Node *P = cur;
                    Node *Q = P->leftChild;
                    roteRight(P, Q);
                    Q->bf = 1;
                    P->bf = -1;
                }
                cur = cur->rightChild;
            }
        } else {
            if (cur == toDelete) {
                bool isLeft = (cur == cur->parent->leftChild);
                if (cur->leftChild == nullptr && cur->rightChild == nullptr) {
                    if (isLeft) cur->parent->leftChild = nullptr;
                    else cur->parent->rightChild = nullptr;
                    delete cur;
                    return true;
                } else if (cur->leftChild == nullptr) {
                    if (isLeft) cur->parent->leftChild = cur->rightChild;
                    else cur->parent->rightChild = cur->rightChild;
                    delete cur;
                    return true;
                } else if (cur->rightChild == nullptr) {
                    throw Exception("should not have right child", Exception::CANNOT_REACH_HERE);
                } else throw Exception("delete node with two child", Exception::CANNOT_REACH_HERE);
            } else {
                throw Exception("error value", Exception::CANNOT_REACH_HERE);
            }
        }
    }
    theNode->value = val_;
    return true;
}

bool AVL::isShorterOnLeft(Node *node) {
    if (node->bf == -1) return true;
    else if (node->bf == 1) {
        if (node->rightChild != nullptr && node->rightChild->bf != 0)
            return true;
    }
    return false;
}

bool AVL::isShorterOnRight(Node *node) {
    if (node->bf == 1) return true;
    else if (node->bf == -1) {
        if (node->leftChild != nullptr && node->leftChild->bf != 0)
            return true;
    }
    return false;
}

//预处理阶段，寻找不再变矮的最小子树
Node *AVL::findNoShorter(int val, Node *&toDelete, Node *&theNode) {
    stack<Node *> stack1;
    Node *noShorter = nullptr;
//    Node *toDelete = nullptr;
    Node *cur = this->root;
    bool isFind = false;
    while (cur != nullptr) {
        stack1.push(cur);
        if (cur->value > val) {
            cur = cur->leftChild;
        } else if (cur->value < val) {
            cur = cur->rightChild;
        } else {
            isFind = true;
            theNode = cur;
            if (cur->leftChild == nullptr || cur->rightChild == nullptr) {
                toDelete = cur;
                break;
            } else {
                cur = cur->leftChild;
                stack1.push(cur);
                while (cur->rightChild != nullptr) {
                    cur = cur->rightChild;
                    stack1.push(cur);
                }
                toDelete = cur;
                break;
            }
        }
    }
    if (!isFind) {
        throw Exception("No node to delete", Exception::NO_NODE_TO_DELETE);
    } else {
        Node *next = nullptr;
        bool shorter = true;
        while (!stack1.empty()) {
            cur = stack1.top();
            stack1.pop();
            if (cur == toDelete) {
                next = cur;
                continue;
            }
            if (cur->leftChild == next) {
                if (shorter && (cur->bf == -1 || (cur->bf == 1 && cur->rightChild->bf != 0))) {
                    shorter = true;
                } else {
                    shorter = false;
                    while (!stack1.empty()) stack1.pop();
                    noShorter = cur;
                    break;
                }
            } else if (cur->rightChild == next) {
                if (shorter && (cur->bf == 1 || (cur->bf == -1 && cur->rightChild->bf != 0))) {
                    shorter = true;
                } else {
                    shorter = false;
                    while (!stack1.empty()) stack1.pop();
                    noShorter = cur;
                    break;
                }
            }
            next = cur;
        }
    }
    if (noShorter == nullptr) noShorter = this->root;
    return noShorter;
}

void AVL::printAns() {
    printAns(this->root);
    cout << '\n';
}

void AVL::printAns(Node *node) {
    if (node == nullptr) return;
    cout << "(";
    if (node->leftChild != nullptr) {
        printAns(node->leftChild);
    }
    cout << node->value << "," << node->bf;
    if (node->rightChild != nullptr) {
        printAns(node->rightChild);
    }
    cout << ")";
}

int main() {
#ifdef T2
    // 备份cin和cout的默认buf
    streambuf *cin_backup, *cout_backup;
    cin_backup = cin.rdbuf();
    cout_backup = cout.rdbuf();

    // 打开要参与重定向的文件
    fstream in, out;
    in.open("../2.in", ios::in);
    out.open("../my2.out", ios::out);
    if (in.fail() || out.fail())
        return -1;

    // 将in.txt内容重定向到cin
    // 将cout重定向到out.txt
    cin.rdbuf(in.rdbuf());
    cout.rdbuf(out.rdbuf());
#endif
//    avl.Insert(1);
//    avl.print2();
//    cout << "Insert 3" << endl;
//    avl.Insert(3);
//    avl.print2();
//    cout << "Insert 5" << endl;
//    avl.Insert(5);
//    avl.print2();
//    cout << "Insert 2" << endl;
//    avl.Insert(2);
//    avl.print2();
//    cout << "Insert 4" << endl;
//    avl.Insert(4);
//    for (int i = 6; i < 10; ++i) {
//        avl.Insert(i);
//    }
#ifdef TEST
    AVL avl;
    avl.Insert(1);
    avl.Insert(3);
    avl.Insert(2);
    avl.Insert(4);
    avl.print();
    avl.print2();
    try {
        avl.Delete(2);
        avl.print();
        avl.print2();
    } catch (Exception &exp) {
        cout << exp.what() << endl;
    }
#endif
#ifdef SUBMIT
    int N;
    AVL avl1;
    cin >> N;
    int operation, value;
    for (int i = 0; i < N; ++i) {
        cin >> operation >> value;
#ifdef DEBUG
        cout << operation << ' ' << value << endl;
#endif
        if (operation == 1) {
#ifdef DEBUG
            cout << "Insert" << endl;
#endif
            avl1.Insert(value);
#ifdef DEBUG
            avl1.print2();
#endif
            avl1.printAns();
        } else {
#ifdef DEBUG
            cout << "Delete" << endl;
#endif
            avl1.Delete(value);
            avl1.printAns();
        }
    }
#endif
    return 0;
}
