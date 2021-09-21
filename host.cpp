#include "config.h"
#include "host.h"
#include "kernel.h"

#include "hlslib/xilinx/Utility.h"
#include "hlslib/xilinx/OpenCL.h"
using hlslib::ocl::Access;


int main() {
  hlslib::ocl::Context context;

  aligned_v<data_t> data = get_random_numbers();
  auto data_D = make_buffer<Access::read>(data, context);
  weight_t weight = 1.52802;
  aligned_v<result_t> result(DATA_SIZE*result_pack_t::kWidth);
  auto result_D = make_buffer<Access::write>(result, context);

  auto program = context.MakeProgram(std::string("hw.xclbin"));
  auto kernel = program.MakeKernel("test_kernel", data_D, weight, result_D);

  data_D.CopyFromHost(data.cbegin());
  kernel.ExecuteTask();
  result_D.CopyToHost(result.begin());
  check_result(data, weight, result);
}
