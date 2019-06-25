# ----- Autogen kernel by TensorIR -----
import numpy as np

a = np.random.rand(3, 3)
b = np.random.rand(3, 3)
t2 = np.tensordot(a, b, axes=[[0], [0]])
b = t2
