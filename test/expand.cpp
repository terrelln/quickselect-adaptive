#include "../quickselect.hpp"
#include <cstdlib>
#include <iostream>
#include <vector>

std::vector<int> random_vector(int size) {
  std::vector<int> vec;
  vec.reserve(size);

  for (int i = 0; i < size; ++i) {
    vec.push_back(rand() % 1000);
  }
  return vec;
}

bool check(const std::vector<int> &vec, int p) {
  for (int i = 0; i < p; ++i) {
    if (vec[i] > vec[p]) {
      std::cout << "idx: " << i << std::endl;
      return false;
    }
  }
  for (unsigned i = p + 1; i < vec.size(); ++i) {
    if (vec[i] < vec[p]) {
      std::cout << "idx: " << i << std::endl;
      return false;
    }
  }
  return true;
}

int main() {
  for (int i = 0; i < 100; ++i) {
    for (int p = 0; p < i; ++p) {
      for (int a = 0; a <= p; ++a) {
        for (int b = p + 1; b < i; ++b) {
          auto v = random_vector(i);
          std::sort(v.begin() + a, v.begin() + b);
          while (v[p] == v[p + 1] && p + 1 < b)
            ++p;
          int pv = v[p];
          assert(a <= p && p < b && b < (int)v.size());
          auto p1 = ten::internal::expand_partition(
              v.begin(), v.end(), v.begin() + a, v.begin() + p, v.begin() + b,
              std::less<int>{});
          if (!check(v, p1 - v.begin())) {
            std::cout << "bad a p b: " << a << ' ' << p << ' ' << b
                      << std::endl;
            for (auto elt : v)
              std::cout << elt << ' ';
            std::cout << std::endl;
          }
          if (pv != *p1)
            std::cout << "bad2" << std::endl;
        }
      }
    }
  }
}
