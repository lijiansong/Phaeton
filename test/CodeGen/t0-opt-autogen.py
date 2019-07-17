# ----- Autogen kernel by Phaeton -----
import numpy as np

a = np.random.rand(3, 3)
b = np.random.rand(3, 3)
t0 = a + b
b = t0
t1 = a - b
b = t1
t2 = a * b
b = t2
t3 = a / b
b = t3
t4 = b + a
t5 = t4 + a
t6 = a + t5
t7 = a * b
t8 = a - t7
t9 = t6 * t8
t10 = t9 * b
b = t10
