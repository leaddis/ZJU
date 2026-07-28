import numpy as np
import matplotlib.pyplot as plt
import csv

def plot_csv(filename, title, outfile):
    x, y = [], []
    with open(filename, 'r') as f:
        reader = csv.reader(f)
        next(reader)
        for row in reader:
            x.append(float(row[0]))
            y.append(float(row[1]))
    plt.figure()
    plt.plot(x, y, linewidth=2)
    plt.title(title)
    plt.grid(True)
    plt.savefig(outfile, dpi=150)
    plt.close()

plot_csv('fig1_0_original.csv', '(t-1)_+', '../pics/2_fig1_0.png')
plot_csv('fig1_1_original.csv', '(t-1)_+', '../pics/2_fig1_1.png')
plot_csv('fig1_2_original.csv', '(t-1)_+', '../pics/2_fig1_2.png')
plot_csv('fig1_3_original.csv', '(t-1)_+', '../pics/2_fig1_3.png')
plot_csv('fig2_1.csv', 'First difference on [t_i,t_{i+1}]', '../pics/2_fig2_1.png')
plot_csv('fig2_2.csv', 'First difference on [t_{i-1},t_i]', '../pics/2_fig2_2.png')
plot_csv('fig2_3.csv', 'First difference on [t_{i},t_{i+1}]', '../pics/2_fig2_3.png')
plot_csv('fig3_1.csv', 'B-spline (n=1)', '../pics/2_fig3_1.png')
plot_csv('fig3_2.csv', 'B-spline (n=1)', '../pics/2_fig3_2.png')
plot_csv('fig4.csv', 'B-spline (n=2)', '../pics/2_fig4.png')
