# creating  a basic cosine function 

import numpy as np
import matplotlib.pyplot as plt 

def cosine (x)
    return np.cos(x)

def plot_cosine(): 
    x = np.linspace(-2 * np.pi, 2 * np.pi , 100)
    y = cosine(x)
    plt.plot(x,y)
    plt.tittle('Cosine function')
    plt.xlabel('x')
    plt.ylabel('cos(x)')
    plt.grid()
    plt.show()

if __name__ == '__main__':
    plot_cosine()
    # this code will plto the cosine function 