# ----- Autogen kernel by Phaeton -----
import numpy as np


tensor3 = np.TensorType('float64', (False,)*3)
a = np.TensorVariable(tensor3)
b = np.TensorVariable(tensor3)
tensor4 = np.TensorType('float64', (False,)*4)
c = np.TensorVariable(tensor4)
tensor10 = np.TensorType('float64', (False,)*10)
d = np.TensorVariable(tensor10)
t0 = np.tensordot(a, b, axes=0)
d = np.tensordot(t0, c, axes=0)
