import sympy as sp

# Define variables
x1, x2 = sp.symbols('x1 x2')

# Define the function
f = (1.5 - x1 + x1 * x2)**2 + (2.25 - x1 + x1 * x2**2)**2 + (2.625 - x1 + x1 * x2**3)**2

# Compute the gradient
grad_f = [sp.diff(f, var) for var in (x1, x2)]

# Compute the Hessian matrix
hessian_f = sp.hessian(f, (x1, x2))

print ("梯度：")
print (grad_f)

print ("hessian:")
print (hessian_f)
