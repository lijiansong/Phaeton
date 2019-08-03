# ----- Autogen kernel by Phaeton -----
import numpy as np

a = np.random.rand(12, 34, 17)
b = np.random.rand(34, 17, 4, 8)
c = np.random.rand(12, 4, 8)
t0 = np.tensordot(a, b, axes=[[2, 1], [1, 0]])
c = t0
