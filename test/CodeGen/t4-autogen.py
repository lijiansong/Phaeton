# ----- Autogen kernel by Phaeton -----
import numpy as np


scalar = np.TensorType('float64', (False,)*0)
a = np.TensorVariable(scalar)
b = np.TensorVariable(scalar)
c = np.TensorVariable(scalar)
d = np.TensorVariable(scalar)
d = a + b
d = a - b
d = a * b
d = a / b
t2 = a + b
t3 = c - d
t1 = t2 * t3
t0 = t1 + a
t4 = c * d
d = t0 + t4
