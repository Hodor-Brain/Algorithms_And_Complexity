#include <algorithm>
#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <limits>

struct Library {
	std::string Writer;
	std::string Book;

	Library(std::string Writer, std::string Book)
		:Writer(Writer),
		Book(Book) {};

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

#define INFINITY std::numeric_limits<double>::max();

using probabilities = std::vector<double>;
using expectationTable = std::vector<std::vector<double>>;
using subtreeProbabilitiesTable = std::vector<std::vector<double>>;
using rootTable = std::vector<std::vector<int>>;

template <typename T>
struct treeNode {
	T data;
	treeNode* left, * right, * parent;

	treeNode(T data, treeNode* parent = nullptr, treeNode* left = nullptr, treeNode* right = nullptr)
		:data(data),
		parent(parent),
		left(left),
		right(right) {}
};

template <typename T>
class OptimalBST {
private:
	treeNode<T>* root = nullptr;

	//Optimal BST algorithm after Kormen
	auto constructOptimalBST(const probabilities& p, const probabilities& q, int n) {
		rootTable root(n);
		expectationTable e(n + 1);
		subtreeProbabilitiesTable w(n + 1);

		for (size_t i = 0; i < n; i++)
			root[i].resize(n);

		for (size_t i = 0; i < n + 1; i++) {
			e[i].resize(n + 1);
			w[i].resize(n + 1);
		}

		for (size_t i = 0; i < n + 1; i++) {
			e[i][i] = q[i];
			w[i][i] = q[i];
		}

		for (size_t l = 1; l <= n; l++) {
			for (size_t i = 1; i <= n - l + 1; i++) {
				int j = i + l - 1;

				e[i - 1][j] = INFINITY;
				w[i - 1][j] = w[i - 1][j - 1] + p[j] + q[j];

				for (size_t r = i; r <= j; r++) {
					double t = e[i - 1][r - 1] + e[r][j] + w[i - 1][j];

					if (t < e[i - 1][j]) {
						e[i - 1][j] = t;
						root[i - 1][j - 1] = r - 1;
					}
				}
			}
		}

		return std::make_pair(e, root);
	}

	void getGraphInfo(treeNode<T>* x, std::string& text) {
		if (x != nullptr) {
			if (x->left != nullptr)
				text += "\"" + x->data.Book + "\"" + " -> " + "\"" + x->left->data.Book + "\";\n";
			if (x->right != nullptr)
				text += "\"" + x->data.Book + "\"" + " -> " + "\"" + x->right->data.Book + "\";\n";

			getGraphInfo(x->left, text);
			getGraphInfo(x->right, text);
		}
	}

	//build the tree structure itself
	treeNode<T>* buildOptimalBST(const std::vector<T>& data, const rootTable& root, size_t i = 0, size_t j = 0, treeNode<T> * lastPtr = nullptr) {
		if (i == j) {
			return new treeNode<T>(data[root[i][j]], lastPtr);
		}
		else if (i > j)
			return nullptr;

		size_t currentIndex = root[i][j];

		if (j - i == root.size() - 1) {
			this->root = new treeNode<T>(data[currentIndex]);
			lastPtr = this->root;
		}
		else {
			lastPtr = new treeNode<T>(data[currentIndex], lastPtr);
		}

		lastPtr->left = buildOptimalBST(data, root, i, currentIndex - 1, lastPtr);
		lastPtr->right = buildOptimalBST(data, root, currentIndex + 1, j, lastPtr);

		return lastPtr;
	}

	void print2DUtil(treeNode<T>* root, int space);

public:
	OptimalBST(std::vector<T> data, const probabilities p, const probabilities q, int n) {

		auto [e, rootTable] = constructOptimalBST(p, q, n);

		buildOptimalBST(data, rootTable, 0, n - 1);
	}

	std::string getWebGraphviz(std::string graphName = "G") {
		std::string graphText = "digraph " + graphName + " {\n";

		getGraphInfo(root, graphText);

		return graphText += "}";
	}

	void print2D();
};

template<typename T>
void OptimalBST<T>::print2D() {
	this->print2DUtil(root, 0);
}

template<typename T>
void OptimalBST<T>::print2DUtil(treeNode<T>* root, int space) {
	if (root == NULL)
		return;

	space += 15;

	print2DUtil(root->right, space);

	std::cout << std::endl;
	for (int i = 15; i < space; i++)
		std::cout << " ";
	std::cout << root->data << "\n";

	print2DUtil(root->left, space);
}


int main() {
	std::vector<Library> p;
	p.push_back({ "Miguel de Cervantes", "Don Quixote" });
	p.push_back({ "William Shakespeare", "Hamlet" });
	p.push_back({ "Homer", "Illiada" });
	p.push_back({ "Herman Melville", "Moby Dick" });
	p.push_back({ "Homer", "The Odyssey" });
	p.push_back({ "James Joyce", "Ulysses" });
	p.push_back({ "Leo Tolstoy", "War and Peace" });

	OptimalBST<Library> tree(p, { 0, 1, 1, 2, 3, 5, 8, 13 }, { 0, 0, 0, 0, 0, 0, 0, 0 }, p.size());

	std::cout << tree.getWebGraphviz();
}