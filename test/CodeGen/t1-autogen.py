# ----- Autogen kernel by Phaeton -----
import numpy as np

a = np.random.rand(4, 5, 6)
b = np.random.rand(4, 5, 6)
c = np.random.rand(1, 2, 3, 4)
d = np.random.rand(4, 5, 6, 4, 5, 6, 1, 2, 3, 4)
t0 = np.tensordot(b, c, axes=0)
t1 = np.tensordot(a, t0, axes=0)
d = t1
