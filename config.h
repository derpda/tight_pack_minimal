#pragma once
#include "ap_fixed.h"
#include "hlslib/xilinx/DataPack.h"


using data_t = ap_fixed<9, 1>;
using data_pack_t = hlslib::DataPack<data_t, 16>;
constexpr unsigned DATA_SIZE = 128/data_pack_t::kWidth;
using weight_t = ap_fixed<18, 4>;
using result_t = ap_fixed<18, 6>;
using result_pack_t = hlslib::DataPack<result_t, 16>;
