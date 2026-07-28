import matplotlib.pyplot as plt
import numpy as np
import math

def condf(x):
    if x != 0:
        return (x * math.exp(-x) / (1 - math.exp(-x)))
    else:
        return 1

def condA(x):
    if x != 0:
        return ((5 - 3 * math.exp(-x)) / (x * math.exp(-x)))
    else:
        return 1000

figurename = ["condf", "condA"]
for s in figurename:
    x = np.linspace(0, 1, 300)
    plt.figure()
    y = []
    for t in x:
        if s == "condf" :
            yt = condf(t)
        else :
            yt = condA(t)
        y.append(yt)
    plt.plot(x, y, label = s)
    plt.xlabel("x")
    plt.ylabel(s)
    plt.title(s)
    plt.legend()
    figname = s + ".png"
    plt.savefig(figname)

print("Finish ploting.")