#include "config.h"
#include "host.h"
#include "kernel.h"


int main() {
  aligned_v<data_t> data = get_random_numbers();
  weight_t weight = 1.52802;
  aligned_v<result_t> result(DATA_SIZE*result_pack_t::kWidth);
  test_kernel(data.data(), weight, result.data());
  check_result(data, weight, result);
}
