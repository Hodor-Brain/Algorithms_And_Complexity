#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>
#include <stack>
#include <cmath>
#include <exception>

template<typename T>
struct fibNode {
	fibNode<T>* left = nullptr;
	fibNode<T>* right = nullptr;

	fibNode<T>* parent = nullptr;
	fibNode<T>* child = nullptr;

	int degree = 0;
	bool mark = false;

	T key;
};

/*template<typename T>
struct DLCList {
	fibNode<T>* head = nullptr;
	int listNodes = 0;
};*/

template<typename T>
class FibHeap {
private:
	fibNode<T>* min;
	int nodeCount;

	void fibHeapInsert(fibNode<T>* x);
	void insertToRootlist(fibNode<T>* x);

public:
	FibHeap();
	void fibHeapInsert(T value);
};

template<typename T>
FibHeap<T>::FibHeap() {
	min = nullptr;
	nodeCount = 0;
}

template<typename T>
void FibHeap<T>::fibHeapInsert(T value){
	fibNode<T>* newNode = new fibNode<T>;
	newNode->key = value;

	fibHeapInsert(newNode);
}

template<typename T>
void FibHeap<T>::fibHeapInsert(fibNode<T>* x) {
	if (min == nullptr) {
		min = x;
		x->left = x;
		x->right = x;
	}
	else {
		insertToRootlist(x);

		if (x->key < min->key) {
			min = x;
		}
	}

	nodeCount++;
}

template<typename T>
void FibHeap<T>::insertToRootlist(fibNode<T>* x) {
	if (min->left == min) {
		min->left = x;
		min->right = x;
		x->left = min;
		x->right = min;
	}
	else {
		x->left = min->left;
		min->left->right = x;
		min->left = x;
		x->right = min;
	}
}


int main()
{
    
}

