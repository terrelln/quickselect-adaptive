#include "../quickselect.hpp"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

int main() {
  for (int s = 0; s < 500; ++s) {
    for (int p = 0; p < s; ++p) {
      vector<int> v;
      v.reserve(s);
      for (int j = 0; j < s; ++j) {
        v.push_back(rand() % 100);
      }
      auto v2 = v;
      auto vec = v;
      ten::quickselect_adaptave(v.begin(), v.end(), v.begin() + p,
                                ::std::less<int>{});
      ten::internal::quickselect(
          ten::internal::three_medians<decltype(vec)::iterator, std::less<int>>,
          vec.begin(), vec.end(), vec.begin() + p,
          std::less<int>{});
      std::nth_element(v2.begin(), v2.begin() + p, v2.end(),
                       ::std::less<int>{});
      if (*(v.begin() + p) != *(v2.begin() + p)) {
        cout << "quickselect says: " << *(v.begin() + p)
             << "\tnth_element says: " << *(v2.begin() + p) << endl;
      }
      if (*(vec.begin() + p) != *(v2.begin() + p)) {
        cout << "quickselect medians says: " << *(v.begin() + p)
             << "\tnth_element says: " << *(v2.begin() + p) << endl;
      }
    }
  }
}
