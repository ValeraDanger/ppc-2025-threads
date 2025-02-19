#include "seq/petrov_o_vertical_image_filtration/include/ops_seq.hpp"

#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

bool petrov_o_vertical_image_filtration_seq::TaskSequential::PreProcessingImpl() {
  unsigned int input_size = task_data->inputs_count[0];
  auto *in_ptr = reinterpret_cast<int *>(task_data->inputs[0]);
  input_ = std::vector<int>(in_ptr, in_ptr + input_size);

  unsigned int output_size = task_data->outputs_count[0];
  output_ = std::vector<int>(output_size, 0);

  rc_size_ = static_cast<size_t>(std::sqrt(input_size));
  return true;
}

bool petrov_o_vertical_image_filtration_seq::TaskSequential::ValidationImpl() {
  size_t dim = static_cast<size_t>(std::sqrt(task_data->inputs_count[0]));
  return task_data->outputs_count[0] == (dim - 2) * (dim - 2);  // No pooling
}

bool petrov_o_vertical_image_filtration_seq::TaskSequential::RunImpl() {
  // Apply Gaussian filter
  for (int i = 1; i < rc_size_ - 1; ++i) {
    for (int j = 1; j < rc_size_ - 1; ++j) {
      float sum = 0.0f;
      for (int ki = -1; ki <= 1; ++ki) {
        for (int kj = -1; kj <= 1; ++kj) {
          sum += input_[(i + ki) * rc_size_ + (j + kj)] * gaussian_kernel_[(ki + 1) * 3 + (kj + 1)];
        }
      }
      output_[(i - 1) * (rc_size_ - 2) + (j - 1)] = static_cast<int>(sum);
    }
  }
  return true;
}

bool petrov_o_vertical_image_filtration_seq::TaskSequential::PostProcessingImpl() {
  for (size_t i = 0; i < output_.size(); i++) {
    reinterpret_cast<int *>(task_data->outputs[0])[i] = output_[i];
  }
  return true;
}
