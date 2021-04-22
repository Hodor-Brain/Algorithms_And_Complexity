#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>
#include <stack>
#include <cmath>
#include <exception>
#include <sstream>

template<typename T>
struct fibNode {
	T key;

	fibNode<T>* left = nullptr;
	fibNode<T>* right = nullptr;

	fibNode<T>* parent = nullptr;
	fibNode<T>* child = nullptr;

	int degree = 0;
	bool mark = false;

	std::string getText();
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
	void insertToChildlist(fibNode<T>* parent, fibNode<T>* toInsert);
	void rootListConcatenation(fibNode<T>* min2);
	void deleteFromRootlist(fibNode<T>* toDelete);
	void deleteFromChildlist(fibNode<T>* parent, fibNode<T>* child);
	void consolidate();
	void fibHeapLink(fibNode<T>* y, fibNode<T>* x);
	std::vector<fibNode<T>*> allMembers(fibNode<T>* current);
	fibNode<T>* search(T value);
	fibNode<T>* recursiveSearch(T value, fibNode<T>* current, fibNode<T>* result);
	void cut(fibNode<T>* x, fibNode<T>* y);
	void cascadingCut(fibNode<T>* y);
	void getGraphInfo(fibNode<T>* x, std::string& text);

public:
	FibHeap();
	FibHeap(const std::initializer_list<T>& list);
	void fibHeapInsert(T value);
	fibNode<T>* findMin();
	FibHeap<T>* fibHeapUnion(FibHeap<T>* anotherHeap);
	fibNode<T>* fibHeapExtractMin();
	void fibHeapDecreaseKey(T current, T decreased);
	void fibHeapDelete(T value);
	std::string getWebGraphviz(std::string graphName);

};

template<typename T>
void FibHeap<T>::getGraphInfo(fibNode<T>* x, std::string& text) {
	if (x == nullptr)
		return;

	for (auto item : allMembers(x)) {
		std::string currentNodeText = item->getText();
		std::stringstream stream;

		if(item == min)
			stream << "\"" << currentNodeText << "\" [color = \"red\"]" << ";\n";
		else{
			stream << "\"" << currentNodeText  << "\";\n";
		}

		if (item->right)
			stream << "\"" << currentNodeText << "\"" << " -> " << "\"" << item->right->getText() << "\";\n";

		if (item->left)
			stream << "\"" << currentNodeText << "\"" << " -> " << "\"" << item->left->getText() << "\";\n";

		if (item->parent)
			stream << "\"" << currentNodeText << "\"" << " -> " << "\"" << item->parent->getText() << "\";\n";

		if (item->child) {
			stream << "\"" << currentNodeText << "\"" << " -> " << "\"" << item->child->getText() << "\";\n";

			text += stream.str();
			stream.str(std::string());
			getGraphInfo(item->child, text);
		}

		text += stream.str();
		stream.str(std::string());
	}
}

template<typename T>
std::string FibHeap<T>::getWebGraphviz(std::string graphName) {
	std::string graphText = "digraph " + graphName + " {\n";

	getGraphInfo(min, graphText);

	return graphText += "}";
}

template<typename T>
std::string fibNode<T>::getText() {
	std::stringstream result;

	result << key;

	return result.str();
}

template<typename T>
FibHeap<T>::FibHeap() {
	min = nullptr;
	nodeCount = 0;
}

template<typename T>
FibHeap<T>::FibHeap(const std::initializer_list<T>& list) {
	min = nullptr;
	nodeCount = 0;
	for (const auto& item : list)
		fibHeapInsert(item);
}

template<typename T>
void FibHeap<T>::fibHeapInsert(T value){
	fibNode<T>* newNode = new fibNode<T>;
	newNode->key = value;

	fibHeapInsert(newNode);
}

template<typename T>
fibNode<T>* FibHeap<T>::findMin(){
	return min;
}

template<typename T>
FibHeap<T>* FibHeap<T>::fibHeapUnion(FibHeap<T>* anotherHeap){
	if (anotherHeap == nullptr)
		return this;

	FibHeap<T>* newHeap = new FibHeap<T>();
	newHeap->min = min;
	newHeap->rootListConcatenation(anotherHeap->min);

	if (min == nullptr || (anotherHeap->min != nullptr && anotherHeap->min->key < min->key)) {
		newHeap->min = anotherHeap->min;
	}

	newHeap->nodeCount = nodeCount + anotherHeap->nodeCount;
	return newHeap;
}

template<typename T>
fibNode<T>* FibHeap<T>::fibHeapExtractMin(){
	fibNode<T>* z = min;

	if (z != nullptr) {
		fibNode<T>* x = z->child;

		if (x != nullptr) {
			fibNode<T>* temp = x->right;

			insertToRootlist(x);

			for (x = temp; x != z->child; x = temp) {
				temp = x->right;
				insertToRootlist(x);
			}
		}

		deleteFromRootlist(z);

		if (z != z->left) {
			min = z->right;
			consolidate();
		}

		nodeCount--;
	}

	return z;
}

template<typename T>
void FibHeap<T>::fibHeapDecreaseKey(T current, T decreased){
	if (current <= decreased) {
		return;
	}

	fibNode<T>* toChange = search(current);

	if (toChange == nullptr) {
		return;
	}

	toChange->key = decreased;
	fibNode<T>* y = toChange->parent;

	if (y != nullptr && toChange->key < y->key) {
		cut(toChange, y);
		cascadingCut(y);
	}
	if (toChange->key < min->key) {
		min = toChange;
	}
}

template<typename T>
void FibHeap<T>::fibHeapDelete(T value){
	if (search(value) != nullptr) {
		fibHeapDecreaseKey(value, INT_MIN);
		delete fibHeapExtractMin();
	}
}

template<typename T>
void FibHeap<T>::rootListConcatenation(fibNode<T>* min2){
	if (min != nullptr && min2 != nullptr) {
		min->left->right = min2;
		min2->left->right = min;

		fibNode<T>* temp = min->left;

		min->left = min2->left;
		min2->left = temp;
	}
}

template<typename T>
void FibHeap<T>::deleteFromRootlist(fibNode<T>* toDelete){
	if (toDelete->left == toDelete) {
		min = nullptr;
		return;
	}

	toDelete->left->right = toDelete->right;
	toDelete->right->left = toDelete->left;
}

template<typename T>
void FibHeap<T>::deleteFromChildlist(fibNode<T>* parent, fibNode<T>* child){
	parent->child = child->right;

	if (child->left == child) {
		parent->child = nullptr;
	}
	else {
		child->left->right = child->right;
		child->right->left = child->left;
	}

	parent->degree--;
}

template<typename T>
void FibHeap<T>::consolidate(){
	int size = (int)floor(log2(nodeCount)) + 1;
	std::vector<fibNode<T>*> A(size, nullptr);

	for (auto item : allMembers(min)) {
		fibNode<T>* x = item;
		int d = x->degree;

		while (A[d] != nullptr) {
			fibNode<T>* y = A[d];

			if (x->key > y->key) {
				std::swap(x, y);
			}

			fibHeapLink(y, x);
			A[d] = nullptr;
			d++;
		}

		A[d] = x;
	}

	min = nullptr;

	for (auto item : A) {
		if (item != nullptr) {
			if (min == nullptr) {
				min = item;
				min->left = min;
				min->right = min;
			}
			else {
				insertToRootlist(item);

				if (item->key < min->key) {
					min = item;
				}
			}
		}
	}
}

template<typename T>
void FibHeap<T>::fibHeapLink(fibNode<T>* y, fibNode<T>* x){
	deleteFromRootlist(y);
	insertToChildlist(x, y);
	x->degree++;
	y->mark = false;
}

template<typename T>
std::vector<fibNode<T>*> FibHeap<T>::allMembers(fibNode<T>* current)
{
	std::vector<fibNode<T>*> result;
	
	fibNode<T>* temp = current;

	if (current == nullptr) {
		return result;
	}

	result.push_back(current);

	temp = current->right;

	while (temp != current) {
		result.push_back(temp);
		temp = temp->right;
	}

	return result;
}

template<typename T>
fibNode<T>* FibHeap<T>::search(T value){
	fibNode<T>* temp = min;
	fibNode<T>* result = nullptr;
	result = recursiveSearch(value, temp, result);
	return result;
}

template<typename T>
fibNode<T>* FibHeap<T>::recursiveSearch(T value, fibNode<T>* current, fibNode<T>* result) {
	for (auto item : allMembers(current)) {
		if (item->key == value) {
			result = item;
			return result;
		}
		if (item->child != nullptr && item->key < value) {
			result = recursiveSearch(value, item->child, result);
		}
		if (result != nullptr) {
			return result;
		}
	}
	return result;
}

template<typename T>
void FibHeap<T>::cut(fibNode<T>* x, fibNode<T>* y){
	deleteFromChildlist(y, x);
	insertToRootlist(x);
	x->mark = false;
}

template<typename T>
void FibHeap<T>::cascadingCut(fibNode<T>* y){
	fibNode<T>* z = y->parent;

	if (z != nullptr) {
		if (y->mark == false) {
			y->mark = true;
		}
		else {
			cut(y, z);
			cascadingCut(z);
		}
	}
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

	x->parent = nullptr;
}

template<typename T>
void FibHeap<T>::insertToChildlist(fibNode<T>* parent, fibNode<T>* toInsert){
	if (parent->child == nullptr) {
		parent->child = toInsert;
		toInsert->left = toInsert;
		toInsert->right = toInsert;
	}
	else if (parent->child->left == parent->child) {
		parent->child->left = toInsert;
		parent->child->right = toInsert;
		toInsert->left = parent->child;
		toInsert->right = parent->child;
	}
	else {
		toInsert->left = parent->child->left;
		parent->child->left->right = toInsert;
		parent->child->left = toInsert;
		toInsert->right = parent->child;
	}

	toInsert->parent = parent;
}


int main()
{
	FibHeap<int> heap({5, 8, 2, 9, 0, 24, 3});
	FibHeap<int> heap2({27, 10, 2001});

	FibHeap<int> heap3 = *heap.fibHeapUnion(&heap2);
	
	std::cout << heap3.getWebGraphviz("g");

	std::cout << "\n";

	heap3.fibHeapExtractMin();
	/*std::cout << heap3.fibHeapExtractMin()->key << " ";
	std::cout << heap3.fibHeapExtractMin()->key << " ";
	std::cout << heap3.fibHeapExtractMin()->key << " ";
	std::cout << heap3.fibHeapExtractMin()->key;*/

	heap3.fibHeapDecreaseKey(9, 6);

	heap3.fibHeapDelete(6);

	std::cout << heap3.getWebGraphviz("g");
	
	/*std::cout << heap3.search(9)->key << " ";
	std::cout << heap3.search(8)->key << " ";
	std::cout << heap3.search(2001)->key << " ";
	std::cout << heap3.search(3)->key << " ";
	std::cout << heap3.search(7)->key << " ";*/

	return 0;
}

// 0 2 5 8 9 24 3 // 10 27 2001