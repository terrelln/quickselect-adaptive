#pragma once
#include <algorithm>
#include <type_traits>
#include <utility>

/**
 * TODO: Benchmark more
 * Use less comparisons and swaps.
 * I use more than necessary right now for simpler code.
 */
namespace ten {
template <typename Iterator, typename Compare>
inline void sort3(Iterator first, Iterator second, Iterator third,
                  Compare comp) {
  using ::std::swap;
  using ::std::forward;

  if (comp(*second, *first)) {
    swap(*first, *second);
  }
  if (comp(*third, *first)) {
    swap(*first, *third);
  }
  if (comp(*third, *second)) {
    swap(*second, *third);
  }
}

template <typename Iterator, typename Compare>
inline void lower_median4(Iterator first, Iterator second, Iterator third,
                          Iterator fourth, Compare comp) {
  using ::std::swap;
  using ::std::forward;

  if (comp(*third, *first)) {
    swap(*first, *third);
  }
  if (comp(*fourth, *second)) {
    swap(*second, *fourth);
  }
  if (comp(*second, *first)) {
    swap(*first, *second);
    if (comp(*fourth, *second)) {
      swap(*second, *fourth);
    }
  } else if (comp(*third, *second)) {
    swap(*second, *third);
  }
}

template <typename Iterator, typename Compare>
inline void sort4(Iterator first, Iterator second, Iterator third,
                  Iterator fourth, Compare comp) {
  using ::std::swap;
  using ::std::forward;

  if (comp(*fourth, *second)) {
    swap(*second, *fourth);
  }
  if (comp(*third, *first)) {
    swap(*first, *third);
  }
  if (comp(*fourth, *third)) {
    swap(*third, *fourth);
    if (comp(*third, *first)) {
      swap(*first, *third);
    }
  } else if (comp(*third, *second)) {
    swap(*second, *third);
  }
  if (comp(*second, *first)) {
    swap(*first, *second);
  }
}

template <typename RandomAccessIterator, typename Compare>
inline void selection_sort(RandomAccessIterator first,
                           RandomAccessIterator last, Compare comp) {
  using ::std::min_element;
  using ::std::add_lvalue_reference;
  using ::std::swap;

  auto lm1 = last;
  for (--lm1; first != lm1; ++first) {
    auto it = min_element<RandomAccessIterator,
                          typename add_lvalue_reference<Compare>::type>(
        first, last, comp);
    if (it != first) {
      swap(*first, *it);
    }
  }
}

}
