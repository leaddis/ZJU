import numpy as np

# Define the function f(x) = 1/2x^T A x + 1/4σ(x^T A x)^2
def f(x, A, sigma):
    return 1/2 * np.dot(x.T, np.dot(A, x)) + 1/4 * sigma * (np.dot(x.T, np.dot(A, x)))**2

def grad_f(x, A, sigma):
    Ax = np.dot(A, x)
    inner_product = np.dot(x.T, Ax)
    first_term = np.dot(A, x)  # Derivative of 1/2 x^T A x is A x
    second_term = sigma * inner_product * Ax  # Derivative of 1/4 σ (x^T A x)^2
    return first_term + second_term

def hessian_f(x, A, sigma):
    Ax = np.dot(A, x)
    inner_product = np.dot(x.T, Ax)
    first_term = A  # Hessian of 1/2 x^T A x is A
    second_term = sigma * (np.outer(Ax, Ax) + inner_product * A)  # Hessian of 1/4 σ (x^T A x)^2
    return first_term + second_term

# Newton's method
def newtons_method(A, x0, sigma, tol=1e-6, max_iter=100000):
    x = x0
    for i in range(max_iter):
        grad = grad_f(x, A, sigma)
        hess = hessian_f(x, A, sigma)
        step = np.linalg.solve(hess, -grad)
        x = x + step
        if np.linalg.norm(grad) < tol:
            break
    return x, i+1  # return the final x and number of iterations

# Newton's method with line search
def newtons_method_line_search(A, x0, sigma, tol=1e-6, max_iter=100, alpha=0.3, beta=0.8):
    x = x0
    for i in range(max_iter):
        grad = grad_f(x, A, sigma)
        hess = hessian_f(x, A, sigma)
        step = np.linalg.solve(hess, -grad)
        
        # Line search to find step size
        t = 1
        while f(x + t * step, A, sigma) > f(x, A, sigma) + alpha * t * np.dot(grad.T, step):
            t *= beta
        
        x = x + t * step
        if np.linalg.norm(grad) < tol:
            break
    return x, i+1  # return the final x and number of iterations

# Define matrix A
A = np.array([[5, 1, 0, 1/2], [1, 4, 1/2, 0], [0, 1/2, 3, 0], [1/2, 0, 0, 2]])

# Define x(0) for two cases
x0_1 = np.array([np.cos(np.radians(70)), np.sin(np.radians(70)), np.cos(np.radians(70)), np.sin(np.radians(70))])
x0_2 = np.array([np.cos(np.radians(50)), np.sin(np.radians(50)), np.cos(np.radians(50)), np.sin(np.radians(50))])

# Run Newton's method for sigma = 1 and sigma = 1e4
sigma_1 = 1
sigma_2 = 1e4

# Results for x0_1
x1_newton, iter1_newton = newtons_method(A, x0_1, sigma_1)
x2_newton, iter2_newton = newtons_method(A, x0_1, sigma_2)

# Results for x0_2
x1_newton_ls, iter1_newton_ls = newtons_method_line_search(A, x0_2, sigma_1)
x2_newton_ls, iter2_newton_ls = newtons_method_line_search(A, x0_2, sigma_2)
print("Newton's method:")
print("sigma = 1")
print(x1_newton, iter1_newton)
print("sigma = 10000")
print(x2_newton, iter2_newton)
print("Newton's method with line search:")
print("sigma = 1")
print(x1_newton_ls, iter1_newton_ls)
print("sigma = 10000")
print(x2_newton_ls, iter2_newton_ls)
