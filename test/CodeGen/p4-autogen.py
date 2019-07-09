# ----- Autogen kernel by TensorIR -----
import numpy as np

a = np.random.rand()
b = np.random.rand()
c = np.random.rand()
d = np.random.rand()
t0 = a + b
d = t0
t1 = a - b
d = t1
t2 = a * b
d = t2
t3 = a / b
d = t3
t4 = a + b
t5 = c - d
t6 = t4 * t5
t7 = t6 + a
t8 = c * d
t9 = t7 + t8
d = t9
