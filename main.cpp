#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>

using namespace std;

struct Query {
    int op;
    long long a, b, c;
};

const int MAX_NODES = 80000000;
struct Node {
    int ls, rs;
    int sum;
} tr[MAX_NODES];
int node_cnt = 0;

int insert(int p, int l, int r, int pos) {
    int q = ++node_cnt;
    tr[q] = tr[p];
    tr[q].sum++;
    if (l == r) return q;
    int mid = l + (r - l) / 2;
    if (pos <= mid) tr[q].ls = insert(tr[p].ls, l, mid, pos);
    else tr[q].rs = insert(tr[p].rs, mid + 1, r, pos);
    return q;
}

int erase(int p, int l, int r, int pos) {
    int q = ++node_cnt;
    tr[q] = tr[p];
    tr[q].sum--;
    if (l == r) return q;
    int mid = l + (r - l) / 2;
    if (pos <= mid) tr[q].ls = erase(tr[p].ls, l, mid, pos);
    else tr[q].rs = erase(tr[p].rs, mid + 1, r, pos);
    return q;
}

int query_sum(int p, int l, int r, int ql, int qr) {
    if (!p) return 0;
    if (ql <= l && r <= qr) return tr[p].sum;
    int mid = l + (r - l) / 2;
    int res = 0;
    if (ql <= mid) res += query_sum(tr[p].ls, l, mid, ql, qr);
    if (qr > mid) res += query_sum(tr[p].rs, mid + 1, r, ql, qr);
    return res;
}

bool query_exist(int p, int l, int r, int pos) {
    if (!p) return false;
    if (l == r) return tr[p].sum > 0;
    int mid = l + (r - l) / 2;
    if (pos <= mid) return query_exist(tr[p].ls, l, mid, pos);
    else return query_exist(tr[p].rs, mid + 1, r, pos);
}

int find_pred(int p, int l, int r, int limit) {
    if (!p || tr[p].sum == 0 || limit < l) return -1;
    if (l == r) return l;
    int mid = l + (r - l) / 2;
    int res = -1;
    if (limit > mid) {
        res = find_pred(tr[p].rs, mid + 1, r, limit);
    }
    if (res == -1) {
        res = find_pred(tr[p].ls, l, mid, limit);
    }
    return res;
}

int find_succ(int p, int l, int r, int limit) {
    if (!p || tr[p].sum == 0 || limit > r) return -1;
    if (l == r) return l;
    int mid = l + (r - l) / 2;
    int res = -1;
    if (limit <= mid) {
        res = find_succ(tr[p].ls, l, mid, limit);
    }
    if (res == -1) {
        res = find_succ(tr[p].rs, mid + 1, r, limit);
    }
    return res;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    vector<Query> queries;
    vector<long long> vals;
    int op;
    while (cin >> op) {
        Query q;
        q.op = op;
        if (op == 0 || op == 1 || op == 3) {
            cin >> q.a >> q.b;
            vals.push_back(q.b);
        } else if (op == 2) {
            cin >> q.a;
        } else if (op == 4) {
            cin >> q.a >> q.b >> q.c;
            vals.push_back(q.b);
            vals.push_back(q.c);
        }
        queries.push_back(q);
    }
    
    sort(vals.begin(), vals.end());
    vals.erase(unique(vals.begin(), vals.end()), vals.end());
    int M = vals.size();
    if (M == 0) M = 1;

    auto get_rank = [&](long long v) {
        return lower_bound(vals.begin(), vals.end(), v) - vals.begin();
    };

    unordered_map<long long, int> root;
    long long lst = 0;
    long long it_a = -1;
    long long it_val = -1;
    int valid = 0;

    for (const auto& q : queries) {
        if (q.op == 0) {
            int pos = get_rank(q.b);
            bool exist = query_exist(root[q.a], 0, M - 1, pos);
            if (!exist) {
                root[q.a] = insert(root[q.a], 0, M - 1, pos);
                it_a = q.a;
                it_val = q.b;
                valid = 1;
            }
        } else if (q.op == 1) {
            if (valid && it_a == q.a && it_val == q.b) valid = 0;
            int pos = get_rank(q.b);
            bool exist = query_exist(root[q.a], 0, M - 1, pos);
            if (exist) {
                root[q.a] = erase(root[q.a], 0, M - 1, pos);
            }
        } else if (q.op == 2) {
            lst++;
            root[lst] = root[q.a];
        } else if (q.op == 3) {
            int pos = get_rank(q.b);
            bool exist = query_exist(root[q.a], 0, M - 1, pos);
            if (exist) {
                cout << "true\n";
                it_a = q.a;
                it_val = q.b;
                valid = 1;
            } else {
                cout << "false\n";
            }
        } else if (q.op == 4) {
            int l_pos = lower_bound(vals.begin(), vals.end(), q.b) - vals.begin();
            int r_pos = upper_bound(vals.begin(), vals.end(), q.c) - vals.begin() - 1;
            if (l_pos <= r_pos) {
                cout << query_sum(root[q.a], 0, M - 1, l_pos, r_pos) << "\n";
            } else {
                cout << "0\n";
            }
        } else if (q.op == 5) {
            if (valid) {
                int pos = get_rank(it_val);
                int pred_pos = find_pred(root[it_a], 0, M - 1, pos - 1);
                if (pred_pos != -1) {
                    it_val = vals[pred_pos];
                    cout << it_val << "\n";
                } else {
                    valid = 0;
                    cout << "-1\n";
                }
            } else {
                cout << "-1\n";
            }
        } else if (q.op == 6) {
            if (valid) {
                int pos = get_rank(it_val);
                int succ_pos = find_succ(root[it_a], 0, M - 1, pos + 1);
                if (succ_pos != -1) {
                    it_val = vals[succ_pos];
                    cout << it_val << "\n";
                } else {
                    valid = 0;
                    cout << "-1\n";
                }
            } else {
                cout << "-1\n";
            }
        }
    }
    return 0;
}
