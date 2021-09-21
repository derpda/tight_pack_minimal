#include "kernel.h"

#include "config.h"


void read_data(
  const data_t data_in[], data_pack_t data[DATA_SIZE]
) {
  read_data_i:for (unsigned i=0; i<DATA_SIZE; ++i) {
    read_data_j:for (unsigned j=0; j<data_pack_t::kWidth; ++j) {
      #pragma HLS pipeline II=1
      #pragma HLS loop_flatten
      data[i][j] = data_in[i*data_pack_t::kWidth+j];
    }
  }
}

void calculate_result(
  const data_pack_t data[DATA_SIZE],
  const weight_t weight,
  result_pack_t result[DATA_SIZE]
) {
  calculate_result_i:for (unsigned i=0; i<DATA_SIZE; ++i) {
    #pragma HLS pipeline II=1
    calculate_result_j:for (unsigned j=0; j<data_pack_t::kWidth; ++j) {
      #pragma HLS unroll
      result[i][j] = data[i][j] * weight;
    }
  }
}

void write_result(
  const result_pack_t result[DATA_SIZE],
  result_t result_out[]
) {
  write_result_i:for (unsigned i=0; i<DATA_SIZE; ++i) {
    write_result_j:for (unsigned j=0; j<data_pack_t::kWidth; ++j) {
      #pragma HLS pipeline II=1
      #pragma HLS loop_flatten
      result_out[i*data_pack_t::kWidth+j] = result[i][j];
    }
  }
}

extern "C" void test_kernel(
  const data_t data_in[], weight_t weight_in, result_t result_out[]
) {
  #pragma HLS INTERFACE m_axi port=data_in offset=slave
  #pragma HLS INTERFACE m_axi port=weight_in offset=slave
  #pragma HLS INTERFACE m_axi port=result_out offset=slave
  #pragma HLS INTERFACE s_axilite port=data_in bundle=control
  #pragma HLS INTERFACE s_axilite port=weight_in bundle=control
  #pragma HLS INTERFACE s_axilite port=result_out bundle=control
  #pragma HLS INTERFACE s_axilite port=return bundle=control

  data_pack_t data[DATA_SIZE];
  read_data(data_in, data);

  result_pack_t result[DATA_SIZE];
  calculate_result(data, weight_in, result);

  write_result(result, result_out);
}
