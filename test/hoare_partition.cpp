#include "../quickselect.hpp"
#include <cstdlib>
#include <iostream>
#include <vector>

std::vector<int> random_vector(int size) {
  std::vector<int> vec;
  vec.reserve(size);

  for (int i = 0; i < size; ++i) {
    vec.push_back(rand() % 100);
  }
  return vec;
}

bool check(const std::vector<int> &vec, int p) {
  for (int i = 0; i < p; ++i) {
    if (vec[i] > vec[p])
      return false;
  }
  for (unsigned i = p + 1; i < vec.size(); ++i) {
    if (vec[i] < vec[p])
      return false;
  }
  return true;
}

int main() {
  for (int i = 0; i < 1000; ++i) {
    for (int p = 0; p < i; ++p) {
      auto v = random_vector(i);
      auto pv = v[p];
      auto p1 = ten::internal::hoare_partition(v.begin(), v.end(),
                                               v.begin() + p, std::less<int>{});
      if (!check(v, p1 - v.begin())) {
        std::cout << "bad" << std::endl;
      }
      if (pv != *p1)
        std::cout << "bad2" << std::endl;
    }
  }
}
