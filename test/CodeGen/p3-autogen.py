# ----- Autogen kernel by TensorIR -----
import numpy as np

a = np.random.rand(12, 34, 17)
b = np.random.rand(12, 34, 17)
c = np.random.rand(34, 17, 28, 42)
d = np.random.rand(17, 34, 28, 42)
t0 = np.tensordot(b, c, axes=[[2], [1]])
t1 = np.tensordot(a, t0, axes=[[1, 0], [2, 0]])
d = t1
