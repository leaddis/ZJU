import dionysus as d
import matplotlib.pyplot as plt
import numpy as np
from scipy.spatial.distance import pdist

def read_points(filename):
    points = []
    with open(filename, 'r') as f:
        for line in f:
            coords = list(map(float, line.strip().split()))
            points.append(coords)
    return np.array(points)

def plot_persistence_diagram(dgm, dim, max_death=None, annotate=False):
    """绘制指定维度的持久图，并可选择标注点"""
    births = []
    deaths = []
    persistence = []  # 存储持续时间
    points = []       # 存储点对象
    
    for pt in dgm:
        if pt.death == float('inf'):
            continue
        birth = pt.birth
        death = pt.death
        births.append(birth)
        deaths.append(death)
        persistence.append(death - birth)
        points.append(pt)
    
    plt.scatter(births, deaths, s=30, alpha=0.7)
    plt.title(f'H{dim} Persistence Diagram')
    plt.xlabel('Birth time')
    plt.ylabel('Death time')
    if max_death is not None:
        plt.plot([0, max_death], [0, max_death], 'k--')
    
    # 添加点标注
    if annotate:
        for i, (birth, death, pers) in enumerate(zip(births, deaths, persistence)):
            # 标注坐标
            plt.annotate(f"({birth:.2f}, {death:.2f})", 
                         (birth, death),
                         xytext=(5, 5),
                         textcoords='offset points',
                         fontsize=8)
            
            # 标注持续时间
            plt.annotate(f"Δ={pers:.2f}", 
                         (birth, death),
                         xytext=(5, -15),
                         textcoords='offset points',
                         fontsize=8,
                         color='red')
            
            # 添加点编号
            plt.annotate(f"#{i}", 
                         (birth, death),
                         xytext=(-15, 5),
                         textcoords='offset points',
                         fontsize=9,
                         weight='bold')

def main():
    points = read_points('cloud.txt')
    
    # 计算点云直径
    diameter = np.max(pdist(points))
    print(f"点云直径: {diameter:.4f}")
    
    # 设置阈值 - 确保能捕捉完整环结构
    max_distance = diameter * 1.3
    max_dim = 2
    
    # 生成Rips复形
    simplices = d.fill_rips(points, max_dim, max_distance)
    
    # 构建过滤复合体
    f = d.Filtration(simplices)
    f.sort()
    
    # 计算持续同调
    p = d.homology_persistence(f)
    dgms = d.init_diagrams(p, f)
    
    # 打印H1点信息
    print("\nH1持久点信息:")
    print("编号 | 出生时间 | 死亡时间 | 持续时间")
    print("-" * 40)
    for i, pt in enumerate(dgms[1]):
        if pt.death == float('inf'):
            continue
        print(f"{i:2d}  | {pt.birth:.4f} | {pt.death:.4f} | {pt.death - pt.birth:.4f}")
    
    # 绘图
    plt.figure(figsize=(14, 6))
    
    # H0图
    plt.subplot(121)
    plot_persistence_diagram(dgms[0], 0)
    
    # H1图 - 添加标注
    plt.subplot(122)
    if dgms[1]:
        finite_deaths = [pt.death for pt in dgms[1] if pt.death != float('inf')]
        max_death = max(finite_deaths) if finite_deaths else 0
    else:
        max_death = 0
    plot_persistence_diagram(dgms[1], 1, max_death, annotate=True)
    
    plt.tight_layout()
    plt.savefig('persistence_diagrams_annotated.png', dpi=300)
    plt.show()

if __name__ == '__main__':
    main()