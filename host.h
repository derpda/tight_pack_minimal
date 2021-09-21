#pragma once
#include "config.h"

#include "hlslib/xilinx/OpenCL.h"

#include <iomanip>
#include <iostream>
#include <random>
#include <vector>


template<typename T>
using aligned_v = std::vector<T, hlslib::ocl::AlignedAllocator<T, 4096>>;

template <hlslib::ocl::Access access, class T>
hlslib::ocl::Buffer<T, access> make_buffer(
  const std::vector<T, hlslib::ocl::AlignedAllocator<T, 4096>>& vector,
  hlslib::ocl::Context& context,
  hlslib::ocl::MemoryBank bank = hlslib::ocl::MemoryBank::bank0
) {
  return context.MakeBuffer<T, access>(bank, vector.size());
}

aligned_v<data_t> get_random_numbers() {
  std::random_device rd;
  std::mt19937 e2(rd());
  std::uniform_real_distribution<> dist(-1, 1);
  aligned_v<data_t> random_numbers(DATA_SIZE*data_pack_t::kWidth);
  for (data_t& number : random_numbers) number = dist(e2);
  return random_numbers;
}

void check_result(
  const aligned_v<data_t>& data_in,
  weight_t weight,
  const aligned_v<result_t>& result
) {
  assert(data_in.size() == result.size());
  for (size_t i=0; i<data_in.size(); ++i) {
    float calculated = result[i].to_float();
    float correct = (data_in[i] * weight).to_float();
    float abs_err = std::abs(calculated - correct);
    float rel_err = std::abs(abs_err / correct);
    if (rel_err > 0.05 && abs_err > std::pow(10, -6)) {
        std::cout << std::setw(3) << i << " ";
        std::cout << std::setw(20) << correct << " ";
        std::cout << std::setw(20) << calculated << "\n";
    }
  }
}
