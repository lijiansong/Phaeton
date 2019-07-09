# ----- Autogen kernel by TensorIR -----
import numpy as np

a = np.random.rand(12, 34, 17)
b = np.random.rand(34, 17, 28, 42)
c = np.random.rand(12, 28, 42)
t0 = np.tensordot(a, b, axes=[[2, 1], [1, 0]])
c = t0
