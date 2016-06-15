#include "quickselect.hpp"
#include <cstdlib>
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

bool check(const std::vector<int> &vec, int p) {
  for (int i = 0; i < p; ++i) {
    if (vec[i] > vec[p]) {
      cout << i << endl;
      return false;
    }
  }
  for (unsigned i = p + 1; i < vec.size(); ++i) {
    if (vec[i] < vec[p]) {
      cout << i << endl;
      return false;
    }
  }
  return true;
}

int main() {
  // fl
  for (int i = 12; i < 1000; ++i) {
    auto v = random_vector(i);
    int k = rand() % (i / 12);
    assert(float(k) / float(v.size()) <= 1.0 / 12.0);
    auto p = ten::internal::repeated_step_far_left(
        v.begin(), v.end(), v.begin() + k, false, less<int>{});
    if (!check(v, p - v.begin())) {
      cout << "bad " << p - v.begin() << ' ' << *p << endl;
      for (auto elt : v)
        cout << elt << ' ';
      cout << endl;
    }
    if (k > p - v.begin())
      cout << k - (p - v.begin()) << endl;
  }
  // fr
  for (int i = 12; i < 1000; ++i) {
    auto v = random_vector(i);
    int k = v.size() - 1 - (rand() % (i / 12));
    assert(float(k) / float(v.size()) >= 1.0 - 1.0 / 12.0);
    auto p = ten::internal::repeated_step_far_left(
        v.begin(), v.end(), v.begin() + k, false, less<int>{});
    if (!check(v, p - v.begin())) {
      cout << "bad " << p - v.begin() << ' ' << *p << endl;
      for (auto elt : v)
        cout << elt << ' ';
      cout << endl;
    }
    if (k < p - v.begin())
      cout << k - (p - v.begin()) << endl;
  }
  // l
  for (int i = 12; i < 100; ++i) {
    auto v = random_vector(i);
    int k = rand() % i;
    auto p = ten::internal::repeated_step_left(
        v.begin(), v.end(), v.begin() + k, false, less<int>{});
    if (!check(v, p - v.begin())) {
      cout << "bad " << p - v.begin() << ' ' << *p << endl;
      for (auto elt : v)
        cout << elt << ' ';
      cout << endl;
    }
  }
  // r
  for (int i = 12; i < 100; ++i) {
    auto v = random_vector(i);
    int k = rand() % i;
    auto p = ten::internal::repeated_step_right(
        v.begin(), v.end(), v.begin() + k, false, less<int>{});
    if (!check(v, p - v.begin())) {
      cout << "bad " << p - v.begin() << ' ' << *p << endl;
      for (auto elt : v)
        cout << elt << ' ';
      cout << endl;
    }
  }
  // a
  for (int i = 12; i < 100; ++i) {
    auto v = random_vector(i);
    int k = rand() % i;
    auto p = ten::internal::repeated_step_adaptive(
        v.begin(), v.end(), v.begin() + k, false, less<int>{});
    if (!check(v, p - v.begin())) {
      cout << "bad " << p - v.begin() << ' ' << *p << endl;
      for (auto elt : v)
        cout << elt << ' ';
      cout << endl;
    }
  }
}
