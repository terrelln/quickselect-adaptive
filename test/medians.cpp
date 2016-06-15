#include "../quickselect.hpp"
#include <iostream>
#include <vector>

using namespace std;

std::vector<int> random_vector(int size) {
  std::vector<int> vec;
  vec.reserve(size);

  for (int i = 0; i < size; ++i) {
    vec.push_back(rand() % 100);
  }
  return vec;
}

bool check3(const std::vector<int> &v) { return v[0] <= v[1] && v[1] <= v[2]; }

bool check4l(const std::vector<int> &v) {
  return v[0] <= v[1] && v[1] <= v[2] && v[1] <= v[3];
}

bool check4u(const std::vector<int> &v) {
  return v[0] <= v[1] && v[1] <= v[2] && v[2] <= v[3];
}

int main() {
  for (int i = 0; i < 10000; ++i) {
    auto v = random_vector(3);
    ten::internal::median3(v.begin(), v.begin() + 1, v.begin() + 2,
                           less<int>{});
    if (!check3(v)) {
      cout << "bad3" << endl;
      for (const auto elt : v)
        cout << elt << ' ';
      cout << endl;
    }

    v = random_vector(4);
    ten::internal::lower_median4(v.begin(), v.begin() + 1, v.begin() + 2,
                                 v.begin() + 3, less<int>{});
    if (!check4l(v)) {
      cout << "bad4l\n";
      for (const auto elt : v)
        cout << elt << ' ';
      cout << endl;
    }

    v = random_vector(4);
    ten::internal::sort4(v.begin(), v.begin() + 1, v.begin() + 2, v.begin() + 3,
                         less<int>{});
    if (!check4u(v)) {
      cout << "bad4u\n";
      for (const auto elt : v)
        cout << elt << ' ';
      cout << endl;
    }
  }
}
