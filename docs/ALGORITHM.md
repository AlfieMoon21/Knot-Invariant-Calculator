# Algorithm Reference

Mathematical background and algorithm details for the Kauffman bracket computation.

---

## DT Notation

Dowker–Thistlethwaite (DT) notation represents a knot diagram as a list of integers.

Walk along the knot and number each strand visit 1, 2, 3, ... up to 2n. Each crossing is visited twice: once on an odd strand, once on an even strand. The DT list records the even strand number for each odd visit, in order.

The **sign** of the DT value encodes the crossing type:
- Positive: positive (right-handed) crossing
- Negative: negative (left-handed) crossing

**Trefoil example `{4, 6, 2}`:**

| i | dt[i] | sign | first_visit (2i) | second_visit (\|val\|−1) |
|---|-------|------|-----------------|--------------------------|
| 0 | 4     | +1   | 0               | 3                        |
| 1 | 6     | +1   | 2               | 5                        |
| 2 | 2     | +1   | 4               | 1                        |

`first_visit` and `second_visit` are the 0-indexed arc positions where the crossing appears in the sequence `0..2n-1`. These are the "arriving" arc endpoints used by the smoothing algorithm.

**Figure-eight example `{6, -8, 2, -4}`:**

| i | dt[i] | sign | first_visit | second_visit |
|---|-------|------|-------------|--------------|
| 0 | 6     | +1   | 0           | 5            |
| 1 | -8    | -1   | 2           | 7            |
| 2 | 2     | +1   | 4           | 1            |
| 3 | -4    | -1   | 6           | 3            |

DT notation encodes a specific diagram, not the knot type; two diagrams of the same knot can have different DT notations. The Jones polynomial is diagram-independent, so it corrects for this.

---

## Laurent Polynomials

The Kauffman bracket and Jones polynomial are **Laurent polynomials**: polynomials where powers can be negative, e.g. `A³ + 2A⁻¹ − 1`.

Stored as `std::map<int, int>` mapping power to coefficient. Only non-zero terms are stored; a term is erased when its coefficient reaches zero.

Key operations:
- `add_term(p, c)`: adds `c·Aᵖ`, erases if result is zero
- `operator+`: term-by-term addition
- `operator*`: polynomial multiplication (add exponents, multiply coefficients)
- `multiply_by_A(k)`: shifts all powers by k; much faster than full multiplication

---

## The Kauffman Bracket

The bracket is a recursive state-sum over all ways to "smooth" the crossings.

### Skein relation

At each crossing, the recursion branches into two smoothings:

```
<K> = A · <A-smoothing> + A⁻¹ · <B-smoothing>
```

### Base case

When all crossings are smoothed, the diagram is a collection of disjoint circles. If there are k circles:

```
<k circles> = (−A² − A⁻²)^(k−1)
```

One circle gives `(−A²−A⁻²)⁰ = 1`; two circles give `−A²−A⁻²`; and so on.

### Recursion outline

```
compute(uf, remaining, n, positions, signs):
    if remaining == 0:
        count circles via uf
        return (−A² − A⁻²)^(circles−1)

    if (canonical(uf), remaining) in cache:
        return cache[...]

    c = lowest set bit in remaining

    uf_A = copy of uf; apply A-smoothing at crossing c
    uf_B = copy of uf; apply B-smoothing at crossing c

    result = A · compute(uf_A, remaining ∖ {c}, ...)
           + A⁻¹ · compute(uf_B, remaining ∖ {c}, ...)

    cache[(canonical(uf), remaining)] = result
    return result
```

The union-find is passed **by value** so each branch gets an independent copy.

---

## Arc Endpoints and Smoothing

A knot with n crossings has 2n arc segments. Each crossing involves four arc endpoints:

```
... -> before_p -> [crossing] -> p -> ...
... -> before_q -> [crossing] -> q -> ...
```

`p` and `q` are the arcs arriving at the crossing (stored in `crossing_positions`). `before_p` and `before_q` are the arcs departing on the other strand; in the cyclic sequence they are at positions `(p−1+2n) % 2n` and `(q−1+2n) % 2n`.

**Smoothing reconnects these four endpoints into two pairs:**

```cpp
bool cross_connect = (type == 'A') == (sign > 0);

if (cross_connect) {
    unite(before_p, q);   // cross-connect
    unite(before_q, p);
} else {
    unite(before_p, before_q);   // parallel-connect
    unite(p, q);
}
```

The logic of `(type == 'A') == (sign > 0)`:

| Smoothing | Sign | cross_connect | Connection |
|-----------|------|---------------|------------|
| A         | +1   | true          | cross      |
| B         | +1   | false         | parallel   |
| A         | −1   | false         | parallel   |
| B         | −1   | true          | cross      |

This matches the standard diagrammatic definition of Kauffman bracket smoothings.

**Counting circles at the base case:**

```cpp
std::set<int> roots;
for (int i = 0; i < 2 * n; i++) roots.insert(uf_find(uf, i));
return compute_delta_power((int)roots.size() - 1);
```

---

## Jones Polynomial

The Kauffman bracket `<K>` is invariant under Reidemeister moves II and III, but not under move I (the curl move). Adding a positive curl multiplies the bracket by `−A³`; adding a negative curl by `−A⁻³`. The writhe tracks this error.

**Writhe:** `w = sum of crossing signs` (each +1 or −1).

**Jones polynomial:**

```
V_K(A) = (−A)^{−3w} · <K>
```

Expanding: `(−A)^{−3w} = (−1)^w · A^{−3w}`

```cpp
int sign_factor = (w % 2 == 0) ? 1 : -1;
return raw.multiply_by_A(-3 * w) * sign_poly;
```

**Right-handed trefoil example `{4, 6, 2}`, writhe w = 3:**

```
<K>         = −A⁻¹⁶ + A⁻¹² + A⁻⁴
(−A)^{−9}   = −A⁻⁹
V_K(A)      = A⁻²⁵ − A⁻²¹ − A⁻¹³
```

Under the substitution `A = t^{−1/4}` this gives `V_K(t) = −t⁻⁴ + t⁻³ + t⁻¹`, matching the known Jones polynomial for the trefoil.

---

## The Memoization Cache

### Why naive caching fails

Processing crossings in fixed order (0, 1, 2, ...) creates a perfect binary tree, meaning every root-to-leaf path is unique. A cache keyed on smoothing history gets **zero hits**, because no two branches ever share a history.

```
              [0,1,2]
             /        \
        A:[1,2]      B:[1,2]
       /      \      /      \
   AA:[2]  AB:[2]  BA:[2]  BB:[2]
```

### The topology-based key

Cache key = **(canonical arc connectivity, remaining-crossings bitmask)**

Two different A/B choice sequences can produce the same arc connectivity with the same crossings still to process. When that happens, the bracket of the remaining diagram is identical, so the second arrival skips the entire subtree.

```cpp
struct CacheKey {
    std::vector<int> connectivity; // canonical component IDs per arc endpoint
    int remaining;                 // bitmask: bit i set = crossing i unsmoothed

    bool operator<(const CacheKey& other) const {
        if (remaining != other.remaining) return remaining < other.remaining;
        return connectivity < other.connectivity;
    }
};
```

### Canonical connectivity

Two union-find states with different internal representation may encode the same connectivity. Canonicalization fixes this: scan endpoints 0..2n−1, assign component IDs 0, 1, 2, ... in order of first appearance.

```
Raw roots [7, 7, 2, 2]  =>  canonical [0, 0, 1, 1]
Raw roots [3, 3, 5, 5]  =>  canonical [0, 0, 1, 1]  (cache hit)
```

### Measured cache performance (torus knots T(2,n))

| Knot   | n  | Calls (no cache) | Calls (cached) | Hit rate | Calls saved |
|--------|----|-----------------|----------------|----------|-------------|
| T(2,7) |  7 |             255 |            211 |   10.4%  |      17.3%  |
| T(2,9) |  9 |           1,023 |            791 |   14.7%  |      22.7%  |
| T(2,11)| 11 |           4,095 |          3,017 |   17.9%  |      26.3%  |
| T(2,13)| 13 |          16,383 |         11,675 |   20.2%  |      28.7%  |
| T(2,15)| 15 |          65,535 |         45,663 |   21.8%  |      30.3%  |
| T(2,21)| 21 |       4,194,303 |      2,831,623 |   24.1%  |      32.5%  |

Hit rates grow with n as larger knots provide more opportunity for smoothing paths to converge. However, for torus knots specifically the overhead of canonicalization and `std::map` lookups exceeds the savings; see [`DEVELOPMENT.md`](DEVELOPMENT.md) for the wall-clock analysis.

---

## Complexity

| Mode       | Time                       | Space              |
|------------|----------------------------|--------------------|
| No cache   | O(2ⁿ)                     | O(n) stack depth   |
| With cache | O(unique states × poly ops)| O(unique states)   |

Unique states are bounded by the number of distinct canonical arc connectivities, which grows much slower than 2ⁿ in practice. The cache is most effective for knots with many coincidental intermediate states (e.g. pretzel knots, composite knots).
