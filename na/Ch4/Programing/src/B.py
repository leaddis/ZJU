import matplotlib.pyplot as plt
import numpy as np

filename = "B_data.txt"
with open(filename, 'r') as f:
    lines = f.readlines()

x0 = [float(s) for s in lines[0].split()]
x1 = [float(s) for s in lines[1].split()]

xaxis = np.array([-3.5 , 3.5])
yaxis = np.array([0,0])

y0 = np.zeros(len(x0))
y1 = np.zeros(len(x1))
plt.figure(figsize=(8, 2), dpi=150)
plt.plot(xaxis, yaxis, c="black")
plt.scatter(x0, y0, c="red", s=5)
plt.yticks([])
plt.savefig("../report/B1.png")
plt.scatter(x1, y1, c="blue", s=5)
plt.savefig("../report/B2.png")

print("Finish Ploting B.")