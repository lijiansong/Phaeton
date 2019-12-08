# Phaeton

[![License: WTFPL](https://camo.githubusercontent.com/e611a050b726fe279c2e4ca11b8186efd400b8d4/68747470733a2f2f696d672e736869656c64732e696f2f62616467652f4c6963656e73652d575446504c2d627269676874677265656e2e737667)](http://www.wtfpl.net/about/)

A toy domain specific language for deep learning tensor expressions.

# Building

```
$ git clone git@github.com:lijiansong/Phaeton.git # or git clone https://github.com/lijiansong/Phaeton.git
$ cd Phaeton
$ mkdir build && cd build
$ cmake -DCMAKE_CXX_COMPILER=$(which g++) -DCMAKE_C_COMPILER=$(which gcc) .. && make -j
```

# Related Projects

- taco: <https://github.com/tensor-compiler/taco>
- lift: <https://github.com/lift-project/lift>
- mlir: <https://github.com/tensorflow/mlir>
- halide: <https://github.com/halide/Halide>
- futhark: <https://github.com/diku-dk/futhark>
- tc: <https://github.com/facebookresearch/TensorComprehensions>
- glow: <https://github.com/pytorch/glow>
- tvm: <https://github.com/dmlc/tvm>

Note: Partial design concept of Phaeton comes from [clang](https://github.com/llvm-mirror/clang) and [mlir](https://github.com/tensorflow/mlir).

# Postscript

I only contribute to this project in my part-time, so this repo will be updated irregularly.

If you are interested in becoming a contributor, or you have any questions or suggestions, feel free to open an [issue](https://github.com/lijiansong/Phaeton/issues) or drop me a message.
