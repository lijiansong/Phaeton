# ----- Autogen kernel by Phaeton -----
import numpy as np

a = np.random.rand(3, 4)
b = np.random.rand(4, 3)
c = np.random.rand(3, 3)
t0 = np.tensordot(a, b, axes=[[1], [0]])
c = t0
