#include <iostream>
#include <exception>
#include <string>


struct Library {
	std::string Writer;
	std::string Book;

	bool operator <(const Library& rhs) const {
		return this->Book < rhs.Book;
	}

	bool operator >(const Library& rhs)const {
		return this->Book > rhs.Book;
	}

	bool operator ==(const Library& rhs)const {
		return this->Book == rhs.Book;
	}
};

std::ostream& operator<<(std::ostream& os, const Library& v) {
	os << '(';

	if (v.Book != "") {
		//os << " Writer: " << v.Writer << ", Book: " << v.Book;
		os << v.Book;
	}

	os << " )";
	return os;
}


template <typename T>
struct treeNode {
	T key;

	treeNode* parent;
	treeNode* left = nullptr;
	treeNode* right = nullptr;

	treeNode(T key)
	{
		this->key = key;
		this->parent = NULL;
		this->left = NULL;
		this->right = NULL;
	}
};

template <typename T>
class SplayTree
{
	treeNode<T>* root;
	void print2DUtil(treeNode<T>* root, int space);
	void zig(treeNode<T>*);
	void zig_zig(treeNode<T>*);
	void zig_zag(treeNode<T>*);
	void Delete(T x);
	SplayTree<T> split(T x);

	treeNode<T>* treeSuccessor(treeNode<T>* x) {
		if (x->right != NULL) {
			return subtree_min(x->right);
		}
		treeNode<T>* y = x->parent;
		while (y != NULL && x == y->right) {
			x = y;
			y = y->parent;
		}
		return y;
	}
	void splay(treeNode<T>*);

	void getGraphInfo(treeNode<T>* x, std::string& text, int counter) {
		if (x != NULL) {
			std::string counterString = std::to_string(counter);
			if (x->left != NULL)
				text += "\"" + x->key.Book + "\"" + " -> " + "\"" + x->left->key.Book + "\"" + ";\n";
			if (x->right != NULL)
				text += "\"" + x->key.Book + "\"" + " -> " + "\"" + x->right->key.Book + "\"" + ";\n";
			getGraphInfo(x->left, text, counter + 1);
			getGraphInfo(x->right, text, counter + 2);
		}
	}

public:
	void print2D();
	SplayTree();
	SplayTree(treeNode<T>*);
	treeNode<T>* find(T);
	treeNode<T>* find(std::string x);
	treeNode<T>* getRoot() {
		return root;
	};

	SplayTree<T> split(std::string x);
	void merge(SplayTree<T>& anotherOne);

	void insert(T);
	void Delete(std::string x);
	void inOrderPrint();

	template<typename U>
	friend std::ostream& operator<<(std::ostream& os, SplayTree<U>& tree) {
		tree.inOrderPrint();
		return os;
	}

	std::string getWebGraphviz(std::string graphName = "G") {
		std::string graphText = "digraph " + graphName + " {\n";
		int n = 0;
		getGraphInfo(root, graphText, n);
		return graphText += "}";
	}
};

template <typename T>
void SplayTree<T>::zig(treeNode<T>* x)
{
	treeNode<T>* p = x->parent;
	if (p->left == x)
	{
		treeNode<T>* A = x->left;
		treeNode<T>* B = x->right;
		treeNode<T>* C = p->right;

		x->parent = NULL;
		x->right = p;

		p->parent = x;
		p->left = B;

		if (B != NULL) B->parent = p;
	}
	else
	{
		treeNode<T>* A = p->left;
		treeNode<T>* B = x->left;
		treeNode<T>* C = x->right;

		x->parent = NULL;
		x->left = p;

		p->parent = x;
		p->right = B;

		if (B != NULL) B->parent = p;
	}
}

template <typename T>
void SplayTree<T>::zig_zig(treeNode<T>* x)
{
	treeNode<T>* p = x->parent;
	treeNode<T>* g = p->parent;
	if (p->left == x)
	{
		treeNode<T>* A = x->left;
		treeNode<T>* B = x->right;
		treeNode<T>* C = p->right;
		treeNode<T>* D = g->right;

		x->parent = g->parent;
		x->right = p;

		p->parent = x;
		p->left = B;
		p->right = g;

		g->parent = p;
		g->left = C;


		if (x->parent != NULL)
		{
			if (x->parent->left == g) x->parent->left = x;
			else x->parent->right = x;
		}

		if (B != NULL) B->parent = p;

		if (C != NULL) C->parent = g;
	}
	else
	{
		treeNode<T>* A = g->left;
		treeNode<T>* B = p->left;
		treeNode<T>* C = x->left;
		treeNode<T>* D = x->right;

		x->parent = g->parent;
		x->left = p;

		p->parent = x;
		p->left = g;
		p->right = C;

		g->parent = p;
		g->right = B;

		if (x->parent != NULL)
		{
			if (x->parent->left == g) x->parent->left = x;
			else x->parent->right = x;
		}

		if (B != NULL) B->parent = g;

		if (C != NULL) C->parent = p;
	}
}

template <typename T>
void SplayTree<T>::zig_zag(treeNode<T>* x)
{
	treeNode<T>* p = x->parent;
	treeNode<T>* g = p->parent;
	if (p->right == x)
	{
		treeNode<T>* A = p->left;
		treeNode<T>* B = x->left;
		treeNode<T>* C = x->right;
		treeNode<T>* D = g->right;

		x->parent = g->parent;
		x->left = p;
		x->right = g;

		p->parent = x;
		p->right = B;

		g->parent = x;
		g->left = C;

		if (x->parent != NULL)
		{
			if (x->parent->left == g) x->parent->left = x;
			else x->parent->right = x;
		}

		if (B != NULL) B->parent = p;

		if (C != NULL) C->parent = g;
	}
	else
	{
		treeNode<T>* A = g->left;
		treeNode<T>* B = x->left;
		treeNode<T>* C = x->right;
		treeNode<T>* D = p->right;

		x->parent = g->parent;
		x->left = g;
		x->right = p;

		p->parent = x;
		p->left = C;

		g->parent = x;
		g->right = B;

		if (x->parent != NULL)
		{
			if (x->parent->left == g) x->parent->left = x;
			else x->parent->right = x;
		}

		if (B != NULL) B->parent = g;

		if (C != NULL) C->parent = p;
	}
}

template <typename T>
void SplayTree<T>::splay(treeNode<T>* x)
{
	while (x->parent != NULL)
	{
		treeNode<T>* p = x->parent;
		treeNode<T>* g = p->parent;
		if (g == NULL) zig(x);
		else if (g->left == p && p->left == x) zig_zig(x);
		else if (g->right == p && p->right == x) zig_zig(x);
		else zig_zag(x);
	}
	this->root = x;
}

template <typename T>
SplayTree<T>::SplayTree()
{
	this->root = NULL;
}

template <typename T>
SplayTree<T>::SplayTree(treeNode<T>* rt)
{
	this->root = rt;
}

template <typename T>
treeNode<T>* SplayTree<T>::find(std::string x) {
	T inputData;
	inputData.Book = x;

	return this->find(inputData);
}

template <typename T>
treeNode<T>* SplayTree<T>::find(T x)
{
	treeNode<T>* ret = NULL;
	treeNode<T>* curr = this->root;
	treeNode<T>* prev = NULL;
	while (curr != NULL)
	{
		prev = curr;
		if (x < curr->key) curr = curr->left;
		else if (x > curr->key) curr = curr->right;
		else
		{
			ret = curr;
			break;
		}
	}
	if (ret != NULL) splay(ret);
	else if (prev != NULL) splay(prev);
	return ret;
}

template <typename T>
void SplayTree<T>::insert(T x)
{
	if (root == NULL)
	{
		root = new treeNode<T>(x);
		return;
	}
	treeNode<T>* curr = this->root;
	while (curr != NULL)
	{
		if (x < curr->key)
		{
			if (curr->left == NULL)
			{
				treeNode<T>* newNode = new treeNode<T>(x);
				curr->left = newNode;
				newNode->parent = curr;
				splay(newNode);
				return;
			}
			else curr = curr->left;
		}
		else if (x > curr->key)
		{
			if (curr->right == NULL)
			{
				treeNode<T>* newNode = new treeNode<T>(x);
				curr->right = newNode;
				newNode->parent = curr;
				splay(newNode);
				return;
			}
			else curr = curr->right;
		}
		else
		{
			splay(curr);
			return;
		}
	}
}

template <typename T>
void SplayTree<T>::Delete(std::string x) {
	T inputData;
	inputData.Book = x;
	Delete(inputData);
}

template <typename T>
void SplayTree<T>::Delete(T x) {
	treeNode<T>* del = find(x);
	if (del == NULL)
		return;

	treeNode<T>* L = del->left;
	treeNode<T>* R = del->right;

	/*L->parent = nullptr;
	R->parent = nullptr;*/

	if (L == NULL && R == NULL)
		root = NULL;
	else if (L == NULL) {
		treeNode<T>* M = subtree_min(R);
		splay(M);
	}
	else if (R == NULL) {
		treeNode<T>* M = subtree_max(L);
		splay(M);
	}
	else {
		treeNode<T>* M = subtree_max(L);
		splay(M);
		M->right = R;
		R->parent = M;
	}


	delete del;
}


template <typename T>
void printTree(treeNode<T>* x, int depth = 0) {
	if (x != NULL) {
		printTree(x->left, depth + 1);

		if (x->parent != NULL) {
			std::cout << (x->parent->left == x ? "Left " : "Right ") << "son of " << x->parent->key << "       ";
		}
		std::cout << x->key << ";  Node Depth = " << depth << std::endl;

		printTree(x->right, depth + 1);
	}
}

template <typename T>
void SplayTree<T>::inOrderPrint()
{
	printTree(this->root);
}



template <typename T>
void SplayTree<T>::print2DUtil(treeNode<T>* root, int space)
{
	// Base case  
	if (root == NULL)
		return;

	// Increase distance between levels  
	space += 15;

	// Process right child first  
	print2DUtil(root->right, space);

	// Print current node after space  
	// count  
	std::cout << std::endl;
	for (int i = 15; i < space; i++)
		std::cout << " ";
	std::cout << root->key << "\n";

	// Process left child  
	print2DUtil(root->left, space);
}

template <typename T>
void SplayTree<T>::print2D()
{
	// Pass initial space count as 0  
	this->print2DUtil(root, 0);
}



template <typename T>
SplayTree<T> SplayTree<T>::split(std::string x) {
	T inputData;
	inputData.Book = x;
	return split(inputData);
}


template <typename T>
SplayTree<T> SplayTree<T>::split(T x) {
	SplayTree<T> splitTree;
	treeNode<T>* spl = find(x);
	if (spl == NULL)
		return splitTree;


	//splay(spl);


	splitTree = SplayTree<T>(spl->left);
	spl->left->parent = nullptr;
	spl->left = nullptr;

	return splitTree;
}



template <typename T>
treeNode<T>* subtree_max(treeNode<T>* subRoot)
{
	treeNode<T>* curr = subRoot;
	while (curr->right != NULL) curr = curr->right;
	return curr;
}

template <typename T>
treeNode<T>* subtree_min(treeNode<T>* subRoot)
{
	treeNode<T>* curr = subRoot;
	while (curr->left != NULL) curr = curr->left;
	return curr;
}


template <typename T>
void SplayTree<T>::merge(SplayTree<T>& anotherOne) {
	treeNode<T>* a = subtree_max(this->getRoot());
	treeNode<T>* b = subtree_min(anotherOne.getRoot());

	if (a->key > b->key) {
		return;
	}
	splay(a);
	this->getRoot()->right = anotherOne.getRoot();
}



int main() {
	SplayTree<Library> tree;

	tree.insert({ "James Joyce", "Ulysses" });
	//std::cout << tree << std::endl;


	tree.insert({ "Miguel de Cervantes", "Don Quixote" });
	//std::cout << tree << std::endl;


	tree.insert({ "Herman Melville", "Moby Dick" });
	//std::cout << tree << std::endl;


	tree.insert({ "Leo Tolstoy", "War and Peace" });
	//std::cout << tree << std::endl;
	//std::cout << tree.getWebGraphviz() << std::endl;

	tree.insert({ "William Shakespeare", "Hamlet" });
	//std::cout << tree << std::endl;
	//std::cout << tree.getWebGraphviz() << std::endl;

	tree.insert({ "Homer", "The Odyssey" });
	std::cout << tree << std::endl;
	std::cout << tree.getWebGraphviz() << std::endl;

	std::cout << std::endl << std::endl;

	std::cout << "Deleting Moby Dick" << std::endl << std::endl;
	tree.Delete("Hamlet");
	std::cout << tree << std::endl;
	std::cout << tree.getWebGraphviz() << std::endl;

	std::cout << std::endl << std::endl;

	std::cout << "Inserting The Divine Comedy" << std::endl << std::endl;
	tree.insert({ "Dante Alighieri", "The Divine Comedy" });
	std::cout << tree << std::endl;
	std::cout << tree.getWebGraphviz() << std::endl;






	/*SplayTree<Library> tree1;
	SplayTree<Library> tree2;

	tree1.insert({ "William Shakespeare", "Hamlet" });
	tree1.insert({ "Miguel de Cervantes", "Don Quixote" });
	tree1.insert({ "Herman Melville", "Moby Dick" });

	std::cout << tree1.getWebGraphviz() << std::endl;
	//tree1.print2D();
	//std::cout << "END" << std::endl;

	tree2.insert({ "James Joyce", "Ulysses" });
	tree2.insert({ "Homer", "The Odyssey" });
	tree2.insert({ "Leo Tolstoy", "War and Peace" });

	std::cout << tree2.getWebGraphviz() << std::endl;
	//tree2.print2D();
	//std::cout << "END" << std::endl;

	tree1.merge(tree2);

	std::cout << tree1.getWebGraphviz() << std::endl;
	//tree1.print2D();
	std::cout << std::endl;*/





	
	/*SplayTree<Library> tree1;
	SplayTree<Library> tree2;

	tree1.insert({ "James Joyce", "Ulysses" });
	tree1.insert({ "Miguel de Cervantes", "Don Quixote" });
	tree1.insert({ "Herman Melville", "Moby Dick" });
	tree1.insert({ "Leo Tolstoy", "War and Peace" });
	tree1.insert({ "William Shakespeare", "Hamlet" });
	tree1.insert({ "Homer", "The Odyssey" });

	std::cout << tree1.getWebGraphviz() << std::endl;
	//tree1.print2D();
	std::cout << "END" << std::endl;

	tree2 = tree1.split("The Odyssey");

	std::cout << tree1.getWebGraphviz() << std::endl;
	//tree1.print2D();
	std::cout << "END" << std::endl;
	std::cout << tree2.getWebGraphviz() << std::endl;
	//tree2.print2D();*/







	/*std::cout << std::endl << std::endl;

	std::cout << std::endl << std::endl;
	tree.print2D();
	std::cout << std::endl << std::endl;*/


	return 0;
}