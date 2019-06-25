# ----- Autogen kernel by TensorIR -----
import numpy as np

a = np.random.rand(12, 34, 17)
b = np.random.rand(12, 34, 17)
c = np.random.rand(34, 17, 28, 42)
d = np.random.rand(17, 34, 28, 42)
t2 = np.tensordot(a, b, axes=[[0], [0]])
t4 = np.tensordot(t2, c, axes=[[0, 3], [0, 1]])
d = t4
