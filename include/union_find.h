#ifndef UNION_FIND_H
#define UNION_FIND_H
#include <cstddef>
#include <vector>

class UnionFind {
private:
    std::vector<int> parent;
    
public:
    // Constructor - n elements, each in its own set
    UnionFind(int n) {
        parent.resize(n);
        for (int i = 0; i < n; i++) {
            parent[i] = i; 
        }
    }
    
    // Find the root of x's set
    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }
    
    // Merge sets containing x and y
    void union_sets(int x, int y) {
        int root_x = find(x);
        int root_y = find(y);
        
        if (root_x != root_y) {
            parent[root_x] = root_y;
        }
    }
    
    // Count how many separate sets exist
    int count_components() {
        int count = 0;
        for (size_t i = 0; i < parent.size(); i++) {
            if (find(i) == i) {  
                count++;
            }
        }
        return count;
    }
};

#endif