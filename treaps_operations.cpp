/*
Acknowledgement:
https://www.tutorialspoint.com/cplusplus-program-to-implement-treap
https://stackoverflow.com/questions/36802354/print-binary-tree-in-a-pretty-way-using-c
*/

#include <bits/stdc++.h>
using namespace std;

class Treap {
    struct TreapNode {
        int data;
        int priority;
        TreapNode *l, *r;
        TreapNode(int d) {
            this->data = d;
            this->priority = rand() % 100;
            this->l= this->r = nullptr;
        }
        ~TreapNode() { 
            delete l; delete r; 
        }
        int max_depth() const {
            const int left_depth = l ? l->max_depth() : 0;
            const int right_depth = r ? r->max_depth() : 0;
            return (left_depth > right_depth ? left_depth : right_depth) + 1;
        }
    } *root;

    public:
    Treap() : root(nullptr) {}

    ~Treap() { 
        delete root; 
    }

    bool search(int k) {
        return searchKey(root, k);
    }

    bool searchKey(TreapNode* root, int k) {
        if (root == nullptr)
            return false;
        else if (root->data == k)
            return true;
        else if (k < root->data)
            return searchKey(root->l, k);
        else
            return searchKey(root->r, k);
    }

    void rotateRight(TreapNode* &root) {
        TreapNode* L = root->l;
        TreapNode* Y = root->l->r;
        L->r = root;
        root->l= Y;
        root = L;
    }

    void rotateLeft(TreapNode* &root) {
        TreapNode* R = root->r;
        TreapNode* X = root->r->l;
        R->l = root;
        root->r= X;
        root = R;
    }

    void insert(int k) {
        insertNode(root, k);
    }

    void insertNode(TreapNode* &root, int k) {
        if (root == nullptr) {
            root = new TreapNode(k);
            return;
        }
        if (k < root->data) {
            insertNode(root->l, k);
            if (root->l != nullptr && root->l->priority > root->priority)
                rotateRight(root);
        } else {
            insertNode(root->r, k);
            if (root->r!= nullptr && root->r->priority > root->priority)
                rotateLeft(root);
        }
    }

    void deleteNd(int k) {
        deleteNode(root, k);
    }

    void deleteNode(TreapNode* &root, int k) {
        if (root == nullptr)
            return;
        if (k < root->data)
            deleteNode(root->l, k);
        else if (k > root->data)
            deleteNode(root->r, k);
        else {
            if (root->l ==nullptr && root->r == nullptr) {
                free(root);
                root = nullptr;
            }
            else if (root->l && root->r) {
                if (root->l->priority < root->r->priority) {
                    rotateLeft(root);
                    deleteNode(root->l, k);
                } else {
                    rotateRight(root);
                    deleteNode(root->r, k);
                }
            }
            else {
                TreapNode* child = (root->l) ? root->l : root->r;
                TreapNode* curr = root;
                root = child;
                free(curr);
                curr = nullptr;
            }
        }
    }

    void buildTreap(vector<int> & set) {
        for (int s : set) {
            insert(s);
        }
    }

    void split(TreapNode* &t, int k, TreapNode* &X, TreapNode* &Y) {
        if (!t)
            X = Y = NULL;
        else if (k < t->data) {
            split(t->l, k, X, t->l);
            Y = t;
        }
        else {
            split(t->r, k, t->r, Y);
            X = t;
        }
    }

    void join(TreapNode* &t, TreapNode* &l, TreapNode* &r) {
        if (!l || !r)
            t = l ? l : r;
        else if (l->priority > r->priority)
            join(l->r, l->r, r),  t = l;
        else
            join(r->l, l, r->l),  t = r;
    }

    int get_max_depth() const { 
        return root ? root->max_depth() : 0; 
    }

    struct cell_display {
        string valstr;
        bool present;
        cell_display() : present(false) {}
        cell_display(std::string valstr) : valstr(valstr), present(true) {}
    };

    using display_rows = vector<vector<cell_display>>;
    display_rows get_row_display() const {
        vector<TreapNode*> traversal_stack;
        vector<std::vector<TreapNode*>> rows;
        if(!root) return display_rows();

        TreapNode *p = root;
        const int max_depth = root->max_depth();
        rows.resize(max_depth);
        int depth = 0;
        for(;;) {
            if(depth == max_depth-1) {
                rows[depth].push_back(p);
                if(depth == 0) break;
                --depth;
                continue;
            }

            if(traversal_stack.size() == depth) {
                rows[depth].push_back(p);
                traversal_stack.push_back(p);
                if(p) p = p->l;
                ++depth;
                continue;
            }

            if(rows[depth+1].size() % 2) {
                p = traversal_stack.back();
                if(p) p = p->r;
                ++depth;
                continue;
            }

            if(depth == 0) break;

            traversal_stack.pop_back();
            p = traversal_stack.back();
            --depth;
        }

        display_rows rows_disp;
        std::stringstream ss;
        for(const auto& row : rows) {
            rows_disp.emplace_back();
            for(TreapNode* pn : row) {
                if(pn) {
                    ss << "k:" << pn->data << " p:" << pn->priority;
                    rows_disp.back().push_back(cell_display(ss.str()));
                    ss = std::stringstream();
                } 
                else {
                    rows_disp.back().push_back(cell_display());
                }   
            }   
        }
        return rows_disp;
    }

    vector<string> row_formatter(const display_rows& rows_disp) const {
        using s_t = string::size_type;

        s_t cell_width = 0;
        for(const auto& row_disp : rows_disp) {
            for(const auto& cd : row_disp) {
                if(cd.present && cd.valstr.length() > cell_width) {
                    cell_width = cd.valstr.length();
                }   
            }   
        }

        if(cell_width % 2 == 0) ++cell_width;

        vector<string> formatted_rows;

        s_t row_count = rows_disp.size();

        s_t row_elem_count = 1 << (row_count-1);

        s_t left_pad = 0;

        for(s_t r=0; r<row_count; ++r) {
            const auto& cd_row = rows_disp[row_count-r-1];
            s_t space = (s_t(1) << r) * (cell_width + 1) / 2 - 1;
            string row;
            for(s_t c=0; c<row_elem_count; ++c) {
                row += string(c ? left_pad*2+1 : left_pad, ' ');
                if(cd_row[c].present) {
                    const string& valstr = cd_row[c].valstr;
                    s_t long_padding = cell_width - valstr.length();
                    s_t short_padding = long_padding / 2;
                    long_padding -= short_padding;
                    row += string(c%2 ? short_padding : long_padding, ' ');
                    row += valstr;
                    row += string(c%2 ? long_padding : short_padding, ' ');
                } else {
                    row += string(cell_width, ' ');
                }
            }
            formatted_rows.push_back(row);

            if(row_elem_count == 1) break;

            s_t left_space  = space + 1;
            s_t right_space = space - 1;
            for(s_t sr=0; sr<space; ++sr) {
                string row;
                for(s_t c=0; c<row_elem_count; ++c) {
                    if(c % 2 == 0) {
                        row += string(c ? left_space*2 + 1 : left_space, ' ');
                        row += cd_row[c].present ? '/' : ' ';
                        row += string(right_space + 1, ' ');
                    } else {
                        row += string(right_space, ' ');
                        row += cd_row[c].present ? '\\' : ' ';
                    }
                }
                formatted_rows.push_back(row);
                ++left_space;
                --right_space;
            }
            left_pad += space + 1;
            row_elem_count /= 2;
        }

        std::reverse(formatted_rows.begin(), formatted_rows.end());

        return formatted_rows;
    }

    static void trim_rows_left(vector<string>& rows) {
        if(!rows.size()) return;
        auto min_space = rows.front().length();
        for(const auto& row : rows) {
            auto i = row.find_first_not_of(' ');
            if(i==string::npos) i = row.length();
            if(i == 0) return;
            if(i < min_space) min_space = i;
        }
        for(auto& row : rows) {
            row.erase(0, min_space);
        }   
    }

    void Dump() const {
        const int d = get_max_depth();

        if(d == 0) {
            cout << " <empty tree>\n";
            return;
        }

        const auto rows_disp = get_row_display();
        auto formatted_rows = row_formatter(rows_disp);
        trim_rows_left(formatted_rows);
        for(const auto& row : formatted_rows) {
            std::cout << ' ' << row << '\n';
        }
    }
};

int main() {
    int numNode, key;
    Treap t;
    vector<int> keys(numNode);
    printf("Please enter total number of nodes:\n");
    scanf("%i", &numNode);

    printf("Please enter the keys in the format x x x ...\n");
    for (int i=0; i<numNode; i++) {
        cin >> key;
        t.insert(key);
    }
    printf("Treap is built!\n");
    t.Dump();

    int numOprtn;
    printf("Please enter the number of operations (e.g insert, delete, etc) you want to execute:\n");
    cin >> numOprtn;

    printf("Instructions:\n");
    printf("For searching a key, enter the letter F followed by the key, e.g F 1\n");
    printf("For inserting a key, enter the letter I followed by the key, e.g I 2\n");
    printf("For deleting a key, enter the letter D followed by the key, e.g D 3\n\n");

    for (int j=0; j<numOprtn; j++) {
        char ch;
        cin >> ch;

        if (ch == 'F') {
            cin >> key;
            printf("Searching node with key %i\n", key);
            if (t.search(key))
                printf("Key %i is found!\n\n", key);
            else
                printf("Key %i is not found!\n\n", key);
        }
        else if (ch == 'I') {
            cin >> key;
            printf("Inserting node with key %i\n", key);
            t.insert(key);
            t.Dump();
            printf("\n");
        }
        else if (ch == 'D') {
            cin >> key;
            printf("Deleting node with key %i\n", key);
            t.deleteNd(key);
            t.Dump();
            printf("\n");
        }
    }

    return 0;
}