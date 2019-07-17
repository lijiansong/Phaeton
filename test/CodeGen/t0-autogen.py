# ----- Autogen kernel by Phaeton -----
import numpy as np


t0 = np.TensorType('float64', (False,)*2)
a = np.TensorVariable(t0)
b = np.TensorVariable(t0)
b = a + b
b = a - b
b = a * b
b = a / b
t4 = b + a
t3 = t4 + a
t2 = a + t3
t6 = a * b
t5 = a - t6
t1 = t2 * t5
b = t1 * b
t7 = theano_function([a, b], b)
