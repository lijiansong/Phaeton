//===--- Context.h - Execution context of device accelerators ---*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file defines the Context interface.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_VIEWACC_CONTEXT_H
#define PHAETON_VIEWACC_CONTEXT_H

#pragma once

#include <random>

#ifdef USE_CUDA
#include <cublas_v2.h>
#include <cuda_runtime.h>
#include <curand.h>
#ifdef USE_CUDNN
#include <cudnn.h>
#endif
#endif // USE_CUDA

#ifdef USE_DNNL
#include <dnnl.hpp>
#endif // USE_DNNL

#ifdef USE_BANG
#include <cnrt.h>
#endif // USE_BANG

#ifdef USE_CNML
#include <cnml.h>
#endif // USE_CNML

namespace phaeton {

typedef struct Context_t {
  std::mt19937 random_generator;

// TODO: Context for CppCPU, CudaGPU, CUDNN, BangMLU, CNML and etc.
#ifdef USE_DNNL
  dnnl::engine dnnl_engine;
  dnnl::stream dnnl_stream;
#endif // USE_DNNL

#ifdef USE_CUDA
  cublasHandle_t cublas_handle;
  cudaStream_t stream;
  curandGenerator_t curand_generator;
#ifdef USE_CUDNN
  cudnnHandle_t cudnn_handle;
#endif
#endif // USE_CUDA

#ifdef USE_BANG
  cnrtDev_t cnrt_device_handle;
  cnrtQueue_t mlu_stream;
#endif // USE_BANG

#ifdef USE_CNML
  cnrtDev_t cnrt_device_handle;
  cnrtQueue_t mlu_stream;
#endif // USE_CNML

} Context;

} // end namespace phaeton

#endif // PHAETON_VIEWACC_CONTEXT_H
