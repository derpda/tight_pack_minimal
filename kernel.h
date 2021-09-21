#pragma once
#include "config.h"


extern "C" void test_kernel(
  const data_t data_in[], weight_t weight_in, result_t result_out[]
);
