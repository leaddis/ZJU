import matplotlib.pyplot as plt
import numpy as np
from sympy import symbols, diff, simplify, lambdify

# 利用节点和牛顿多项式系数计算插值多项式最终系数
def hermite_interpolation_coefficients(x_points, div_diff_coeffs):
    n = len(x_points)
    x = symbols('x')
    interpolating_poly = div_diff_coeffs[0]

    for i in range(1, n):
        term = div_diff_coeffs[i]
        for j in range(i):
            term *= (x - x_points[j])
        interpolating_poly += term

    return interpolating_poly

# 读取多项式信息
filename = 'D.txt'
with open(filename, 'r') as f:
    lines = f.readlines()

coff = [float(s) for s in lines[2].split()]
node = [float(s) for s in lines[10].split()]

# 读取原函数的插值结果
x_values = []
y_values = []
for line in lines[12:]:
    x, y = map(float, line.split())
    x_values.append(x)
    y_values.append(y)

# 计算多项式和导数
poly_expr = hermite_interpolation_coefficients(node, coff)
x_sym = symbols('x')
poly_derivative = diff(poly_expr, x_sym)

# 使用Lambda函数
eval_func_poly = lambdify(x_sym, poly_expr, 'numpy')
eval_func_derivative = lambdify(x_sym, poly_derivative, 'numpy')

# 生成 x 值的范围
x = np.linspace(min(x_values), max(x_values), 500)

# 计算导数值
y2_derivative = np.array([eval_func_derivative(xi) for xi in x], dtype=float)

x_value = 10 
poly_at_x = eval_func_poly(x_value) 
derivative_at_x = eval_func_derivative(x_value) 

derivative_exceeds_81 = any(y2_derivative > 81)

# 输出结果
print(f"Polynomial value at t = {x_value}: {poly_at_x}") 
print(f"Derivative value at t = {x_value}: {derivative_at_x}") 

if derivative_exceeds_81: 
    print("Derivative exceeds 81.") 
else: 
    print("Derivative does not exceed 81.")
    
# 画两个图
plt.figure(figsize=(12, 5))

# 画原函数图
plt.subplot(1, 2, 1)
plt.plot(x_values, y_values, 'b-', label="Polynomial (n=8)")
plt.title('x-t figure')
plt.xlabel("t")
plt.ylabel("x")
plt.legend()

# 画导函数图
plt.subplot(1, 2, 2)
plt.plot(x, y2_derivative, 'r-', label="Derivative")
plt.title('v-t figure')
plt.xlabel("t")
plt.ylabel("v")
plt.legend()

plt.tight_layout()

figurename = "../report/D.png"
plt.savefig(figurename)