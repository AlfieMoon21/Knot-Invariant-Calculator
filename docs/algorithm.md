# The Kauffman Bracket Algorithm

## The Formula
```
⟨diagram with crossing⟩ = A × ⟨A-smoothing⟩ + A^(-1) × ⟨B-smoothing⟩
⟨n circles, no crossings⟩ = (-A² - A^(-2))^(n-1)
```

## What is Smoothing?

**Smoothing** = removing a crossing by reconnecting strands

### Two Types of Smoothing

**A-smoothing:** Connect strands going the "same way"
```
Original:    A-smooth:
    |           ) (
----+----  →    ) (
    |
```

**B-smoothing:** Connect strands going "opposite ways"
```
Original:    B-smooth:
    |           \ /
----+----  →     X
    |           / \
```

## Our Implementation (Current)

### Simplified Version
```cpp
Polynomial kauffman_bracket(int num_crossings) {
    // Base case
    if (num_crossings == 0) {
        return Polynomial(1);  // Just return 1
    }
    
    // Recursive case
    Polynomial A_result = kauffman_bracket(num_crossings - 1).multiply_by_A(1);
    Polynomial B_result = kauffman_bracket(num_crossings - 1).multiply_by_A(-1);
    
    return A_result + B_result;
}
```

### Why It's Simplified

**What we do:** Assume smoothing just removes one crossing

**What actually happens:** Smoothing can also:
- Create extra circles (components)
- Merge circles together
- Change the topology

## The Recursion Tree

For a knot with 3 crossings:
```
                [3 crossings]
               /             \
           A×[2]            A^-1×[2]
          /     \           /       \
      A²×[1] A^0×[1]   A^0×[1]  A^-2×[1]
      /  \    / \       /  \      /   \
    A³ A¹  A¹ A^-1   A¹ A^-1  A^-1 A^-3
```

**Leaf nodes:** 2³ = 8 states
**Total calls:** 2⁴ - 1 = 15 recursive calls

## Current Results

### Test Cases

**Unknot (0 crossings):**
- Result: `1` ✓
- Calls: 1

**1 Crossing:**
- Result: `A + A^-1` ✓
- Calls: 3

**Trefoil (3 crossings):**
- Result: `A³ + 3A + 3A^-1 + A^-3`
- Calls: 15
- ⚠️ Wrong! (missing component tracking)

### Why Trefoil is Wrong

The correct answer should involve terms like `A⁷`, `A⁵`, etc.

Our simplified version gives: `A³ + 3A + 3A^-1 + A^-3`

**The problem:** We're not accounting for when smoothing creates extra circles, which multiply the result by `(-A² - A^-2)`.

## What Needs to Be Fixed

### 1. Track Components

When we smooth, we need to know:
- Did we create an extra circle?
- How many circles do we have now?

### 2. Proper Base Case
```cpp
Polynomial base_case(int num_components) {
    if (num_components == 1) return 1;
    
    // (-A² - A^-2)^(n-1)
    Polynomial delta;
    delta.add_term(2, -1);
    delta.add_term(-2, -1);
    
    return pow(delta, num_components - 1);
}
```

### 3. State Representation

Instead of just `int num_crossings`, we need:
```cpp
struct DiagramState {
    int num_crossings;
    int num_components;
};
```

## Complexity Analysis

**Without memoization:**
- Time: O(2^n) where n = number of crossings
- Space: O(n) for recursion stack

**With memoization:**
- Time: O(unique_states × polynomial_ops)
- Space: O(unique_states)

**Example for 10 crossings:**
- Without memo: 1,024 states
- With memo: ~100-200 unique states (estimated)
- Speedup: 5-10×

## Next Steps

1. Implement proper component tracking
2. Test against known Kauffman bracket values
3. Add memoization
4. Profile performance
5. Optimize for Raspberry Pi
EOF