# ----- Autogen kernel by Phaeton -----
import numpy as np

a = np.random.rand(12, 24, 11)
b = np.random.rand(12, 24, 11)
c = np.random.rand(24, 11, 22, 64)
d = np.random.rand(11, 24, 22, 64)
t0 = np.tensordot(b, c, axes=[[2], [1]])
t1 = np.tensordot(a, t0, axes=[[1, 0], [2, 0]])
d = t1
