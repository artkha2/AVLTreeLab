#include <utility>  // provides swap
#include <sstream>  // string stream
#include <iostream>

using namespace std;

class AVLTree {
    private:

        // NODE CLASS
        class Node {
            public:
                int element;
                int height;
                Node* parent;
                Node* left{nullptr};
                Node* right{nullptr};
        
                Node(int e, Node* p = nullptr, int h = 0) : element{e}, parent{p}, height{h} {}

        };

        // MEMBER VARIABLE
        Node* rt{nullptr};

        //REBALANCING HELPERS
        int height(Node* node){
            return node == nullptr ? 0 : node->height;
        }

        int balance_factor(Node* node){
            return node == nullptr ? 0 : (height(node->left) - height(node->right));
        }

        void recompute_height(Node* p){
            p->height = 1 + max(height(p->left), height(p->right));
        }

        Node* right_rotate(Node* y){
            Node* x  = y->left;
            Node* T2 = x->right;

            x->right = y;
            y->left = T2;

            recompute_height(y);
            recompute_height(x);

            return x; // new root
        }

        Node* left_rotate(Node* x){
            Node* y = x->right;
            Node* T2 = y->left;

            y->left = x;
            x->right = T2;

            recompute_height(x);
            recompute_height(y);

            return y; // new root
        }

        // INSERTION AND DELETION HELPERS
        Node* insert_helper(Node* node, const int& k){
            if(node == nullptr) return new Node{k};

            if (k < node->element){
                node->left = insert_helper(node->left, k);
            }
            else if(k > node->element){
                node->right = insert_helper(node->right, k);
            }
            else{
                return node;
            }
            
            recompute_height(node);

            int balance = balance_factor(node);

            // Left Left Case
            if (balance > 1 && k < node->left->element)
                return right_rotate(node);

            // Right Right Case
            if (balance < -1 && k > node->right->element)
                return left_rotate(node);

            // Left Right Case
            if (balance > 1 && k > node->left->element){
                node->left = left_rotate(node->left);
                return right_rotate(node);
            }

            // Right Left Case
            if (balance < -1 && k < node->right->element){
                node->right = right_rotate(node->right);
                return left_rotate(node);
            }

            return node;
                  
        }

        // function to find the node with the minimum key value
        Node* min_value_node(Node* node){
            Node* current = node;
            while (current->left != nullptr)
                current = current->left;
            return current;
        }

        Node* remove_helper(Node* root, const int& k){
            if (root == nullptr)
                return root;

            if (k < root->element)
                root->left = remove_helper(root->left, k);
            else if (k > root->element)
                root->right = remove_helper(root->right, k);
            else{
                // Node with only one child or no child
                if ((root->left == nullptr) || (root->right == nullptr)){
                    Node* temp = root->left ? root->left : root->right;
                    if (temp == nullptr){
                        temp = root;
                        root = nullptr;
                    }
                    else
                        *root = *temp;
                    delete temp;
                }
                else {
                    Node* temp = min_value_node(root->right);  // inorder successor
                    root->element = temp->element;
                    root->right = remove_helper(root->right, temp->element);
                }
            }

            if (root == nullptr)
                return root;

            // Update height of the current node
            recompute_height(root);

            // Get the balance factor of this node
            int balance = balance_factor(root);

            // If this node becomes unbalanced, then there are 4 cases

            // Left Left Case
            if (balance > 1 && balance_factor(root->left) >= 0)
                return right_rotate(root);

            // Left Right Case
            if (balance > 1 && balance_factor(root->left) < 0) {
                root->left = left_rotate(root->left);
                return right_rotate(root);
            }

            // Right Right Case
            if (balance < -1 && balance_factor(root->right) <= 0)
                return left_rotate(root);

            // Right Left Case
            if (balance < -1 && balance_factor(root->right) > 0) {
                root->right = right_rotate(root->right);
                return left_rotate(root);
            }

            return root;
        }


        // SERIALIZATION AND DESERIALIZATION HELPERS
        // serialize using pre-order
        void serialize_helper(Node* node, ostringstream& out) const {
            if (!node){
                out << "#,";  // Print # for nullptr
                return;  // reached nullptr - exit recursion
            }
            out << node->element << ",";           // Visit root, print it, separate by comma
            serialize_helper(node->left, out);     // Traverse left subtree
            serialize_helper(node->right, out);    // Traverse right subtree
        }

        Node* deserialize_helper(istringstream& in, Node* parent = nullptr) {
            string val;
            if (!(getline(in, val, ',')) || val == "#") return nullptr;  // read until comma

            int num = stoi(val);  // convert to integer
            Node* node = new Node(num, parent=parent);
            node->left = deserialize_helper(in, node);   // Construct left subtree first
            node->right = deserialize_helper(in, node);  // Then construct right subtree
            return node;
        }
    

    public:

        AVLTree() {}  // empty tree

        Node* root() const { return rt; }

        // Function to insert a key into the AVL tree
        void insert(int key) { rt = insert_helper(rt, key); }

        // Function to remove a key from the AVL tree
        void remove(int key) { rt = remove_helper(rt, key); }

        string serialize() const {
            ostringstream out;
            serialize_helper(rt, out);
            string result = out.str();
            if(!result.empty()) result.pop_back();  // remove trailign comma
            return result;
        }

        AVLTree deserialize(const string& data) {
            istringstream in(data);
            AVLTree new_tree;
            new_tree.rt = deserialize_helper(in);
            return new_tree;
        }

        
    // RULE OF 5 SUPPORT
    private:
        void tear_down(Node* nd) {
            if (nd != nullptr) {
                tear_down(nd->left);
                tear_down(nd->right);
                delete nd;
            }
        }

        // Create cloned structure of model and return pointer to the new structure
        static Node* clone(Node* model) {
            if (model == nullptr) return nullptr;        // trivial clone
            Node* new_root{new Node(model->element)};
            new_root->left = clone(model->left);
            if (new_root->left) new_root->left->parent = new_root;
            new_root->right = clone(model->right);
            if (new_root->right) new_root->right->parent = new_root;
            return new_root;
        }
        
    public:
        // destructor
        ~AVLTree() { tear_down(rt); }

        // copy constructor
        AVLTree(const AVLTree& other) : rt{clone(other.rt)} {}

        // copy assignment
        AVLTree& operator=(const AVLTree& other) {
            if (this != &other) {        // bypass self-assignment
                tear_down(rt);
                rt = clone(other.rt);
            }
            return *this;
        }

        // move constructor
        AVLTree(AVLTree&& other) : rt{other.rt} {
            // reset other to empty
            other.rt = nullptr;
        }

        // move assignment
        AVLTree& operator=(AVLTree&& other) {
            if (this != &other) {        // bypass self-assignment
                swap(rt, other.rt);      // old structure will be destroyed by other
            }
            return *this;
        }
    };


    int main(){
        AVLTree tree;
        
        // Inserts four nodes into the tree
        cout << "Populating the tree..." << endl;
        tree.insert(3);
        tree.insert(4);
        tree.insert(5);
        tree.insert(6);
        tree.insert(2);
        
        cout << "Serialized tree: ";
        cout << tree.serialize() << endl;
        
        tree.remove(6);
        
        cout << "Serialized tree with 6 removed: ";
        string serialized_tree = tree.serialize();
        cout << serialized_tree << endl;

        cout << "Deserializing the serialized tree..." << endl;
        AVLTree new_tree = tree.deserialize(serialized_tree);
        
        new_tree.insert(7);

        cout << "Final serialized tree after inserting 7: " << new_tree.serialize() << endl;
    }