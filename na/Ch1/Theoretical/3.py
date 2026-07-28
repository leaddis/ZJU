def newton_method(p, dp, x0, max_iterations=4):
    """
    Perform Newton's method to find a root of the polynomial p(x).
    
    Parameters:
    p : function
        The polynomial function p(x).
    dp : function
        The derivative of the polynomial function p'(x).
    x0 : float
        The initial guess for the root.
    max_iterations : int
        The maximum number of iterations to perform.
        
    Returns:
    list
        A list of approximations for the root after each iteration.
    """
    x_n = x0
    results = [x_n]  # Store the initial guess
    for _ in range(max_iterations):
        p_xn = p(x_n)
        dp_xn = dp(x_n)
        if dp_xn == 0:
            print("Derivative is zero. No solution found.")
            break
        x_n = x_n - p_xn / dp_xn
        results.append(x_n)
    return results

# Define the polynomial and its derivative
def p(x):
    return 4*x**3 - 2*x**2 + 3

def dp(x):
    return 12*x**2 - 4*x

# Initial guess
x0 = -1

# Perform the Newton's method
iterations = newton_method(p, dp, x0)

# Print the results
for i, x in enumerate(iterations, 1):
    print(f"Iteration {i}: x = {x:.6f}")