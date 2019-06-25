# ----- Autogen kernel by TensorIR -----
import numpy as np

a = np.random.rand(4, 5, 6)
b = np.random.rand(4, 5, 6)
c = np.random.rand(1, 2, 3, 4)
d = np.random.rand(4, 5, 6, 4, 5, 6, 1, 2, 3, 4)
t2 = np.tensordot(a, b, axes=0)
t4 = np.tensordot(t2, c, axes=0)
d = t4
