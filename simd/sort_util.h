#pragma once

#include "utils.h"
#include "common.h"

namespace avx2{
  // Regular
  template <typename InType, typename RegType>
  void SortBlock64(InType *&arr, size_t offset);
  template <typename InType, typename RegType>
  void SortBlock16(InType *&arr, size_t offset);

  // Masked
  template <typename InType, typename RegType>
  void MaskedSortBlock4x8(InType *&arr, size_t offset);
  template <typename InType, typename RegType>
  void MaskedSortBlock2x4(InType *&arr, size_t offset);

};
