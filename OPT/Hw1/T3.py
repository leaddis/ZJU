import numpy as np

def rosenbrock_function(x):
    return 100 * (x[1] - x[0]**2)**2 + (1 - x[0])**2

def rosenbrock_gradient(x):
    return np.array([400 * x[0]**3 - 400 * x[0] * x[1] + 2 * x[0] - 2,
                     200 * (x[1] - x[0]**2)])

def rosenbrock_hessian(x):
    return np.array([[1200 * x[0]**2 - 400 * x[1] + 2, -400 * x[0]],
                     [-400 * x[0], 200]])

def beale_function(x):
    return (1.5 - x[0] + x[0] * x[1])**2 + (2.25 - x[0] + x[0] * x[1]**2)**2 + (2.625 - x[0] + x[0] * x[1]**3)**2

def beale_gradient(x):
    return np.array([2.25*(1.33333333333333*x[1] - 1.33333333333333)*(0.666666666666667*x[0]*x[1] - 0.666666666666667*x[0] + 1) + 5.0625*(0.888888888888889*x[1]**2 - 0.888888888888889)*(0.444444444444444*x[0]*x[1]**2 - 0.444444444444444*x[0] + 1) + 6.890625*(0.761904761904762*x[1]**3 - 0.761904761904762)*(0.380952380952381*x[0]*x[1]**3 - 0.380952380952381*x[0] + 1),
                     15.75*x[0]*x[1]**2*(0.380952380952381*x[0]*x[1]**3 - 0.380952380952381*x[0] + 1) + 9.0*x[0]*x[1]*(0.444444444444444*x[0]*x[1]**2 - 0.444444444444444*x[0] + 1) + 3.0*x[0]*(0.666666666666667*x[0]*x[1] - 0.666666666666667*x[0] + 1)])


def beale_hessian(x):
    return np.array([[(2.25*(0.666666666666667*x[1] - 0.666666666666667)*(1.33333333333333*x[1] - 1.33333333333333) + 5.0625*(0.444444444444444*x[1]**2 - 0.444444444444444)*(0.888888888888889*x[1]**2 - 0.888888888888889) + 6.890625*(0.380952380952381*x[1]**3 - 0.380952380952381)*(0.761904761904762*x[1]**3 - 0.761904761904762)),
                      7.875*x[0]*x[1]**2*(0.761904761904762*x[1]**3 - 0.761904761904762) + 4.5*x[0]*x[1]*(0.888888888888889*x[1]**2 - 0.888888888888889) + 2.0*x[0]*x[1] + 1.5*x[0]*(1.33333333333333*x[1] - 1.33333333333333) - 2.0*x[0] + 15.75*x[1]**2*(0.380952380952381*x[0]*x[1]**3 - 0.380952380952381*x[0] + 1) + 9.0*x[1]*(0.444444444444444*x[0]*x[1]**2 - 0.444444444444444*x[0] + 1) + 3.0],
                     [7.875*x[0]*x[1]**2*(0.761904761904762*x[1]**3 - 0.761904761904762) + 4.5*x[0]*x[1]*(0.888888888888889*x[1]**2 - 0.888888888888889) + 2.0*x[0]*x[1] + 1.5*x[0]*(1.33333333333333*x[1] - 1.33333333333333) - 2.0*x[0] + 15.75*x[1]**2*(0.380952380952381*x[0]*x[1]**3 - 0.380952380952381*x[0] + 1) + 9.0*x[1]*(0.444444444444444*x[0]*x[1]**2 - 0.444444444444444*x[0] + 1) + 3.0,
                      18.0*x[0]**2*x[1]**4 + 8.0*x[0]**2*x[1]**2 + 2.0*x[0]**2 + 31.5*x[0]*x[1]*(0.380952380952381*x[0]*x[1]**3 - 0.380952380952381*x[0] + 1) + 9.0*x[0]*(0.444444444444444*x[0]*x[1]**2 - 0.444444444444444*x[0] + 1)]])

def newton_method(x0, gradient_func, hessian_func, function_func, max_iter, tol):
    x = x0
    iter = 0
    
    while iter < max_iter:
        grad = gradient_func(x)
        hess = hessian_func(x)
        if np.linalg.norm(grad) < tol:
            break
        x = x - np.linalg.solve(hess, grad)
        iter += 1
    
    fval = function_func(x)
    return x, fval, iter

def steepest_descent(x0, gradient_func, function_func, max_iter, tol, alpha=0.001):
    x = x0
    iter = 0
    
    while iter < max_iter:
        grad = gradient_func(x)
        if np.linalg.norm(grad) < tol:
            break
        x = x - alpha * grad
        iter += 1
    
    fval = function_func(x)
    return x, fval, iter

# 定义初始点
x0 = np.array([-1.2, 1.0])
max_iter = 100000
tol = 1e-6

# Rosenbrock 函数
x_rosenbrock_sd, fval_rosenbrock_sd, iter_rosenbrock_sd = steepest_descent(x0, rosenbrock_gradient, rosenbrock_function, max_iter, tol)
x_rosenbrock_newton, fval_rosenbrock_newton, iter_rosenbrock_newton = newton_method(x0, rosenbrock_gradient, rosenbrock_hessian, rosenbrock_function, max_iter, tol)

# Beale 函数
x_beale_sd, fval_beale_sd, iter_beale_sd = steepest_descent(x0, beale_gradient, beale_function, max_iter, tol)
x_beale_newton, fval_beale_newton, iter_beale_newton = newton_method(x0, beale_gradient, beale_hessian, beale_function, max_iter, tol)



print('Rosenbrock 函数:')
print(f'最速下降法: 迭代次数 = {iter_rosenbrock_sd}, 最优解 = {x_rosenbrock_sd}, 最优值 = {fval_rosenbrock_sd}')
print(f'牛顿法: 迭代次数 = {iter_rosenbrock_newton}, 最优解 = {x_rosenbrock_newton}, 最优值 = {fval_rosenbrock_newton}')

print('Beale 函数:')
print(f'最速下降法: 迭代次数 = {iter_beale_sd}, 最优解 = {x_beale_sd}, 最优值 = {fval_beale_sd}')
print(f'牛顿法: 迭代次数 = {iter_beale_newton}, 最优解 = {x_beale_newton}, 最优值 = {fval_beale_newton}')