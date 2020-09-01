//===--- Device.h - Abstraction for different accelerators ------*- C++ -*-===//
//
//                     The Phaeton Compiler Infrastructure
//
//===----------------------------------------------------------------------===//
//
//  This file defines the Device interface.
//  Refer to the design of Apache Singa.
//
//===----------------------------------------------------------------------===//

#ifndef PHAETON_VIEWACC_H
#define PHAETON_VIEWACC_H

#pragma once

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <vector>

namespace phaeton {

/// DeviceType - Target device specifier.
enum DeviceType {
  DEVICE_TYPE_UNKNOWN,
  DEVICE_TYPE_CPP_CPU,
  DEVICE_TYPE_CUDA_GPU,
  DEVICE_TYPE_BANG_MLU,
  DEVICE_TYPE_CCE_NPU,
  DEVICE_TYPE_OPENCL_DEV,

  DEVICE_TYPE_DEFAULT,
  DEVICE_TYPE_COUNT
};

enum LangType {
  LANG_CPP,
  LANG_CUDA,
  LANG_OPENCL,
  LANG_BANG,
  LANG_CCE,
  LANG_TYPE_COUNT
};

/// Allocate memory and execute Tensor operations.
class Device {
public:
  // Device() = default;
  virtual ~Device();
  /// Constructor with device ID, num of executors (e.g., cuda streams),
  /// max mem size to use (in MB)
  Device(int id, int num_executors);
  Device(DeviceType dt, int id, int num_executors);

  virtual void SetRandSeed(unsigned seed) = 0;

  /// Return the size (bytes) of memory in use
  /// TODO(wangwei) override this function for all devices.
  virtual size_t GetAllocatedMem() { return 0u; }

  /// Copy data within or across devices.
  virtual void CopyDataToFrom(Block *dst, Block *src, size_t nBytes,
                              CopyDirection direction, int dst_offset,
                              int src_offset);

  void CopyDataFromHostPtr(Block *dst, const void *src, size_t nBytes,
                           size_t dst_offset = 0);
  /// Submit the operation to the device, which may execute it right now or
  /// delay it depending on the scheduler.
  void Exec(std::function<void(Context *)> &&fn, OpType type,
            const std::vector<Block *> read_blocks,
            const std::vector<Block *> write_blocks,
            bool use_rand_generator = false);

  // Wait for one event.
  // void WaitFor();

  /// wait for all operations submitted to this device.
  virtual void Sync();

  int id() const { return id_; }

  /// Return the programming language for this device.
  LangType lang() const { return lang_; }

  Context *context(int k) { return &ctx_; }

  bool graph_enabled() const { return graph_enabled_; }

  virtual std::shared_ptr<Device> host() const { return host_; }

protected:
  /// Execute one operation on one executor.
  virtual void DoExec(std::function<void(Context *)> &&fn, int executor) = 0;

  virtual void CopyToFrom(void *dst, const void *src, size_t nBytes,
                          CopyDirection direction, Context *ctx) = 0;

  /// Allocate device memory.
  virtual void *Malloc(int size) = 0;

  /// Free device memory.
  virtual void Free(void *ptr) = 0;

private:
  Device(){};

  // Note: class Graph is defined in file: include/singa/core/scheduler.h
protected:
  friend class CodeGen;

  int DeviceId = 0;
  /// The computational graph
  CodeGen *CG = nullptr;
  /// Programming language type, could be kCpp, kCuda, kOpencl
  LangType lang_;
  /// The host device
  std::shared_ptr<Device> host_;

  Context ctx_;

  /// Device type, could be DT_CppCPU, DT_CudaGPU, DT_SwapCudaGPU,
  /// DT_OpenclDevice.
  DeviceType device_type_;
};

} // end namespace phaeton

#endif // PHAETON_AST_STMT_H
