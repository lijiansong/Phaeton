# ----- Autogen kernel by Phaeton -----
import numpy as np


tensor3 = np.TensorType('float64', (False,)*3)
a = np.TensorVariable(tensor3)
tensor4 = np.TensorType('float64', (False,)*4)
b = np.TensorVariable(tensor4)
t2 = np.TensorType('float64', (False,)*3)
c = np.TensorVariable(t2)
c = np.tensordot(a, b, axes=[[2, 1], [1, 0]])
