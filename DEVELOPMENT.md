    # Development Log

    ## Week 2 - Thursday
    - Project proposed: Jones polynomial calculator
    - Initial repository structure created
    - Researching Kauffman bracket algorithm
    - Setting up C++ development environment

    ## Thursday Evening - Week 2
    - Implemented Laurent polynomial class
    - Basic arithmetic operations working (addition, multiplication)
    - Next: Need to represent knots and implement Kauffman bracket

    ### Kauffman Bracket - First Implementation
    ✅ Recursive algorithm structure implemented
    ✅ Polynomial multiplication by A^n working
    ✅ Base case (0 crossings) returns 1
    ✅ Exponential growth confirmed (1 crossing = 3 calls, 3 crossings = 15 calls)

    **Current limitation:** Simplified version doesn't track component changes during smoothing
    - Results are structurally correct but numerically wrong for real knots
    - Need to implement proper component tracking in smoothing operations

    **Test results:**
    - Unknot: 1 ✓
    - 1 crossing: A + A^-1 ✓
    - Trefoil: A^3 + 3A + 3A^-1 + A^-3 (structurally correct, needs component tracking)

    ### Next Session Goals
    - Understand how smoothing changes number of components
    - Implement proper component tracking
    - Add the (-A² - A^-²) factor for extra circles
    - Verify against known Kauffman bracket values


    ### Technical Notes
    - Using std::map for sparse polynomial storage
    - Powers can be negative (Laurent polynomials)

    ### Next Steps
    - Implement basic polynomial class
    - Implement knot representation
    - Get basic algorithm working for trefoil knot
    - Add test cases with known results

    ### Resources
    - KnotInfo database: https://knotinfo.math.indiana.edu/
    - Kauffman bracket algorithm papers


    EOF