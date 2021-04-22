#include <iostream>
#include <exception>

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
	os << '{';

	if (v.Book != "") {
		os << " Writer: " << v.Writer << ", Book: " << v.Book;
	}

	os << " }";
	return os;
}


enum Color { RED, BLACK };

template <typename T>
struct treeNode {
	T key;
	Color color = BLACK;
	treeNode* parent;
	treeNode* left = nullptr;
	treeNode* right = nullptr;

	int descendants = 0;
};

template <typename T>
class RBTree {
private:
	treeNode<T>* nill = new treeNode<T>();
	treeNode<T>* root = nill;

	void leftRotate(treeNode<T>* x) {
		treeNode<T>* y = x->right;
		x->right = y->left;

		int alpha = x->left->descendants + 1, gamma = y->right->descendants + 1;
		if (x->left == nill) {
			alpha--;
		}
		if (y->right == nill) {
			gamma--;
		}

		x->descendants -= (gamma + 1);
		y->descendants += (alpha + 1);

		if (y->left != nill) {
			y->left->parent = x;
		}

		y->parent = x->parent;

		if (x->parent == nill) {
			root = y;
		}
		else {
			if (x == x->parent->left) {
				x->parent->left = y;
			}
			else {
				x->parent->right = y;
			}
		}
		y->left = x;
		x->parent = y;
	}

	void rightRotate(treeNode<T>* y) {
		treeNode<T>* x = y->left;
		y->left = x->right;

		int alpha = x->left->descendants + 1, gamma = y->right->descendants + 1;
		if (x->left == nill) {
			alpha--;
		}
		if (y->right == nill) {
			gamma--;
		}

		x->descendants += (gamma + 1);
		y->descendants -= (alpha + 1);

		if (x->right != nill) {
			x->right->parent = y;
		}

		x->parent = y->parent;

		if (y->parent == nill) {
			root = x;
		}
		else {
			if (y == y->parent->right) {
				y->parent->right = x;
			}
			else {
				y->parent->left = x;
			}
		}
		x->right = y;
		y->parent = x;
	}

	void insertFixup(treeNode<T>* z) {
		treeNode<T>* y;

		while (z->parent->color == RED) {
			if (z->parent == z->parent->parent->left) {
				y = z->parent->parent->right;
				if (y->color == RED) {
					z->parent->color = BLACK;
					y->color = BLACK;
					z->parent->parent->color = RED;
					z = z->parent->parent;
				}
				else {
					if (z == z->parent->right) {
						z = z->parent;
						leftRotate(z);
					}
					z->parent->color = BLACK;
					z->parent->parent->color = RED;
					rightRotate(z->parent->parent);
				}
			}
			else {
				y = z->parent->parent->left;
				if (y->color == RED) {
					z->parent->color = BLACK;
					y->color = BLACK;
					z->parent->parent->color = RED;
					z = z->parent->parent;
				}
				else {
					if (z == z->parent->left) {
						z = z->parent;
						rightRotate(z);
					}
					z->parent->color = BLACK;
					z->parent->parent->color = RED;
					leftRotate(z->parent->parent);
				}
			}
		}

		root->color = BLACK;
	}

	void removeFixup(treeNode<T>* x) {
		while (x != root && x->color == BLACK) {
			treeNode<T>* w;
			if (x == x->parent->left) {
				w = x->parent->right;
				if (w->color == RED) {
					w->color = BLACK;
					x->parent->color = RED;
					leftRotate(x->parent);
					w = x->parent->right;
				}
				if (w->left->color == BLACK && w->right->color == BLACK) {
					w->color = RED;
					x = x->parent;
				}
				else {
					if (w->right->color == BLACK) {
						w->left->color = BLACK;
						w->color = RED;
						rightRotate(w);
						w = x->parent->right;
					}
					w->color = x->parent->color;
					x->parent->color = BLACK;
					w->right->color = BLACK;
					leftRotate(x->parent);
					x = root;
				}
			}
			else {
				w = x->parent->left;
				if (w->color == RED) {
					w->color = BLACK;
					x->parent->color = RED;
					rightRotate(x->parent);
					w = x->parent->left;
				}
				if (w->right->color == BLACK && w->left->color == BLACK) {
					w->color = RED;
					x = x->parent;
				}
				else {
					if (w->left->color == BLACK) {
						w->right->color = BLACK;
						w->color = RED;
						leftRotate(w);
						w = x->parent->left;
					}
					w->color = x->parent->color;
					x->parent->color = BLACK;
					w->left->color = BLACK;
					rightRotate(x->parent);
					x = root;
				}
			}
		}
		x->color = BLACK;
	}

	treeNode<T>* treeSuccessor(treeNode<T>* x) {
		if (x->right != nill) {
			return treeMinimum(x->right);
		}
		treeNode<T>* y = x->parent;
		while (y != nill && x == y->right) {
			x = y;
			y = y->parent;
		}
		return y;
	}

	treeNode<T>* treeMinimum(treeNode<T>* x) {
		while (x->left != nill) {
			x = x->left;
		}
		return x;
	}

	treeNode<T>* treeMaximum(treeNode<T>* x) {
		while (x->right != nill) {
			x = x->right;
		}
		return x;
	}

	treeNode<T>* treeSearch(treeNode<T>* x, const T& key) {
		if (x == nill || key == x->key) {
			return x;
		}
		if (key < x->key)
			return treeSearch(x->left, key);
		else
			return treeSearch(x->right, key);
	}

	void inorderTreeWalk(treeNode<T>* x, int depth = 0) {
		if (x != nill) {
			inorderTreeWalk(x->left, depth + 1);
			std::cout << x->key << ' ' << (x->color ? "black," : "red,") << " depth:" << depth << ", descendants " << x->descendants << std::endl;
			inorderTreeWalk(x->right, depth + 1);
		}
	}

public:
	RBTree() {
		root->parent = nill;
	}

	RBTree(const std::initializer_list<T>& list) {
		for (const auto& item : list)
			insert(item);
	}

	template<typename U>
	friend std::ostream& operator<<(std::ostream& os, RBTree<U>& tree) {
		tree.inorderTreeWalk(tree.root);
		return os;
	}

	T& operator [](size_t index) {
		if (index < 0 || index > size()) {
			throw std::invalid_argument("Out of range!");
		}

		treeNode<T>* current = root;
		while ((current->left->descendants + (current->left != nill)) != index) {
			if (current->left->descendants >= index) {
				current = current->left;
			}
			else {
				index -= (current->left->descendants + 1 + (current->left != nill));
				current = current->right;
			}
		}
		return current->key;
	}

	int size() const {
		if (root)
			return root->descendants + 1;
		else
			return 0;
	}

	treeNode<T>* treeSearch(const T& key) {
		return treeSearch(root, key);
	}

	void insert(const T& item) {
		treeNode<T>* z = new treeNode<T>();
		z->key = item;

		treeNode<T>* y = nill;
		treeNode<T>* x = root;

		while (x != nill) {
			y = x;
			x->descendants++;
			if (z->key < x->key) {
				x = x->left;
			}
			else {
				x = x->right;
			}
		}

		z->parent = y;
		if (y == nill) {
			root = z;
		}
		else {
			if (z->key < y->key) {
				y->left = z;
			}
			else {
				y->right = z;
			}
		}

		z->left = nill;
		z->right = nill;
		z->color = RED;

		insertFixup(z);
	}

	treeNode<T>* remove(const T& item) {
		treeNode<T>* z = treeSearch(root, item);
		treeNode<T>* decreaser;

		if (z == nill)
			throw std::invalid_argument("There's no element with such parameters.");

		treeNode<T>* x, * y;

		if (z->left == nill || z->right == nill)
			y = z;
		else
			y = treeSuccessor(z);

		decreaser = y->parent;

		if (y->left != nill)
			x = y->left;
		else
			x = y->right;

		x->parent = y->parent;

		if (y->parent == nill) {
			root = x;
		}
		else {
			if (y == y->parent->left) {
				y->parent->left = x;
			}
			else {
				y->parent->right = x;
			}
		}

		if (y != z) {
			z->key = y->key;
		}

		while (decreaser != nill) {
			decreaser->descendants--;
			decreaser = decreaser->parent;
		}

		if (y->color == BLACK) {
			removeFixup(x);
		}

		return y;
	}

	treeNode<T>* remove(treeNode<T>* z) {
		treeNode<T>* y;
		treeNode<T>* x;

		if (z->left == nill && z->right == nill) {
			y = z;
		}
		else {
			y = treeSuccessor(z);
		}

		if (y->left != nill) {
			x = y->left;
		}
		else {
			x = y->right;
		}

		x->parent = y->parent;

		if (y->parent == nill) {
			root = x;
		}
		else {
			if (y == y->parent->left) {
				y->parent->left = x;
			}
			else {
				y->parent->right = x;
			}
		}

		if (y != z) {
			z->key = y->key;
		}

		if (y->color == BLACK) {
			removeFixup(x);
		}

		return y;
	}
};

int main() {
	RBTree<Library> tree;

	tree.insert({ "James Joyce", "Ulysses" });
	//std::cout << tree << std::endl;
	tree.insert({ "Miguel de Cervantes", "Don Quixote" });
	//std::cout << tree << std::endl;
	tree.insert({ "Herman Melville", "Moby Dick" });
	//std::cout << tree << std::endl;
	tree.insert({ "Leo Tolstoy", "War and Peace" });
	//std::cout << tree << std::endl;
	tree.insert({ "William Shakespeare", "Hamlet" });
	//std::cout << tree << std::endl;
	tree.insert({ "Homer", "The Odyssey" });

	//std::cout << tree << std::endl;

	//std::cout << std::endl;

	//tree.remove({ "Herman Melville", "Moby Dick" });

	//std::cout << tree << std::endl;
	tree.insert({ "Dante Alighieri", "The Divine Comedy" });

	/*std::cout << tree << std::endl;

	for (int i = 0; i < tree.size(); ++i) {
		std::cout << tree[i] << std::endl;
	}*/

	std::cout << tree[0] << std::endl;
}
