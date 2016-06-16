#pragma once
#include "../sorts.hpp"
#include <iterator>
#include <utility>

namespace ten {

template <typename Partition, typename RandomAccessIterator, typename Compare>
void quickselect(Partition &&partition, RandomAccessIterator first,
                 RandomAccessIterator last, RandomAccessIterator k,
                 Compare comp) {
  using namespace internal;
  using ::std::distance;
  using ::std::forward;
  using ::std::advance;
  using ::std::swap;

  assert(first <= k && k < last);

  while (true) {
    const auto len = last - first;
    switch (len) {
    case 0:
    case 1:
      return;
    case 2:
      if (comp(*(--last), *first))
        swap(*first, *last);
      return;
    case 3:
      auto m = first;
      sort3(first, ++m, --last, comp);
      return;
    }
    if (len < 12) {
      selection_sort(first, last, comp);
      return;
    }
    auto p = forward<Partition>(partition)(first, last, k, true, comp);
    if (p == k) {
      return;
    }
    if (p > k) {
      last = p;
    } else {
      first = p + 1;
    }
  }
}

template <typename RandomAccessIterator, typename Compare>
inline RandomAccessIterator
three_medians(RandomAccessIterator first, RandomAccessIterator last,
              RandomAccessIterator, bool, Compare comp) {
  sort3(first, first + std::distance(first, last) / 2, last - 1, comp);
  return hoare_partition(first, last, first + std::distance(first, last) / 2,
                         comp);
}
}
