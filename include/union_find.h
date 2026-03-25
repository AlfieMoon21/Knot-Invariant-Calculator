#ifndef UNION_FIND_H
#define UNION_FIND_H

// NOT USED IN THE CURRENT BUILD.
//
// Early standalone UnionFind written during initial development. Later inlined into
// KauffmanBracket (kauffman.h) as uf_find/uf_unite on a plain std::vector<int>,
// avoiding the cost of constructing and copying a separate object per recursive branch.
//
// Kept to show the evolution of the data structure.

#include <cstddef>
#include <vector>

class UnionFind {
private:
    std::vector<int> parent;
    
public:
    UnionFind(int n) {
        parent.resize(n);
        for (int i = 0; i < n; i++) parent[i] = i;
    }

    // Path-compressing find.
    int find(int x) {
        if (parent[x] != x) parent[x] = find(parent[x]);
        return parent[x];
    }

    void union_sets(int x, int y) {
        int root_x = find(x);
        int root_y = find(y);
        if (root_x != root_y) parent[root_x] = root_y;
    }

    int count_components() {
        int count = 0;
        for (size_t i = 0; i < parent.size(); i++) {
            if (find(i) == (int)i) count++;
        }
        return count;
    }
};

#endif