/*
Acknowledgement:
https://codeforces.com/contest/702/submission/71958162
*/
#include<bits/stdc++.h>
using namespace std;
#define rand() (rand() * rand() + rand() + 1)
    
struct Node {
    int id, left, right;
    double key;
    int prior, lazy, ans_lazy;
    int ans;
    Node(){
        id = left = right = key = prior = lazy = ans = ans_lazy = 0;
    }
} node[220000];
    
int ind = 1;
    
int new_node() {
    return ind++;
}
    
namespace Treap{
    int root = 0;

    void push(int curr){
        if (!curr) 
            return;

        int l = node[curr].left;
        int r = node[curr].right;

        if (l) {
            node[l].lazy += node[curr].lazy;
            node[l].ans_lazy += node[curr].ans_lazy;
        }
        if (r) {
            node[r].lazy += node[curr].lazy;
            node[r].ans_lazy += node[curr].ans_lazy;
        }

        node[curr].key -= node[curr].lazy;
        node[curr].ans += node[curr].ans_lazy;

        node[curr].lazy = 0;
        node[curr].ans_lazy = 0;
    }

    void split(int curr, double v, int& l, int& r){
        push(curr);

        if (!curr) 
            l = r = 0;
        else if (node[curr].key < v){
            split(node[curr].right, v, node[curr].right, r);
            l = curr;
        }
        else {
            split(node[curr].left, v, l, node[curr].left);
            r = curr;
        }
    }

    void merge(int& curr, int l, int r){
        push(l);
        push(r);

        if (!l || !r) 
            curr = max(l, r);
        else if (node[l].prior > node[r].prior){
            merge(node[l].right, node[l].right, r);
            curr = l;
        }
        else {
            merge(node[r].left, l, node[r].left);
            curr = r;
        }
    }


    void insert(int& curr, int id){
        push(curr);
        if (!curr) 
            curr = id;
        else if (node[id].prior >= node[curr].prior){
            split(curr, node[id].key, node[id].left, node[id].right);
            curr = id;
        }
        else {
            if (node[id].key < node[curr].key)
                insert(node[curr].left, id);
            else 
                insert(node[curr].right, id);
        }   
    }

    void unite(int& l, int r){
        push(r);
        if(!r)
            return;

        unite(l, node[r].left);
        unite(l, node[r].right);

        node[r].right = node[r].left = 0;
        insert(l, r);
    }

    void add(int id, double key){
        int j = new_node();

        node[j].id = id;
        node[j].key = key + 0.000001 * (id + 1);
        node[j].prior = rand();

        insert(root, j);

    }
    
    void update(int v){
        int l, r;
        split(root, v, l, r);

        if (r) {
            node[r].lazy += v;
            node[r].ans_lazy++;
        }

        int tl, tr;
        split(r, v, tl, tr);
        unite(l, tl);
        root = l;
        merge(root, root, tr);
    }
}
    
    
int32_t main(){
        ios_base::sync_with_stdio(false);
        cin.tie(NULL);
        int n;
        cin >> n;
        vector<pair<int,int>> arr;
        for(int i = 0; i < n; i++){
            int x, y;
            cin >> x >> y;
            arr.push_back({x, y});
        }

        sort(arr.begin(), arr.end(), [&](pair<int,int>& a, pair<int,int>& b) {
            return a.second > b.second || (a.second == b.second && a.first < b.first);
        });
    
        int m;
        cin >> m;
        for(int i = 0; i < m; i++){
            int x;
            cin >> x;
            Treap::add(i, x);
        }
    
        for(int i = 0; i < n; i++){
            Treap::update(arr[i].first);
        }
    
        vector<int> ans(m);
        for(int i=1; i<ind; i++){
            ans[node[i].id] = node[i].ans;
        }
        for(auto x : ans) 
            cout << x << " ";
}