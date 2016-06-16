#pragma once

#include "partitions.hpp"
#include "sorts.hpp"
#include <algorithm>
#include <cassert>
#include <iterator>
#include <type_traits>

/**
 * Macros for altering behavior for benchmarking & testing
 * TODO: Remove eventually
 */
// If MIN is defined, when we select a partition p, check to see if it is the
// minimum element, if it is partition into (== *p, > p).
#define MIN

// If NTH is defined, call `std::nth_element` instead of `quickselect_adaptive`
// in repeated_step_* functions.
//#define NTH

// PROPORTION controls the order statistic we use as a partition in
// `repeated_step_{right,left,adaptive}`.
#define PROPORTION(f) f *distance(first, k) / distance(first, last)
//#define PROPORTION(f) f / 2

// PROPORTION_FAR controls the order statistic we use as a partition in
// `repeated_step_far_*`.
//#define PROPORTION_FAR(f) f * distance(first, k) / distance(first, last)
#define PROPORTION_FAR(f) f / 2

namespace ten {

// Helper functions
namespace internal {

template <typename RandomAccessIterator, typename Compare>
inline RandomAccessIterator
repeated_step_far_left(RandomAccessIterator first, RandomAccessIterator last,
                       RandomAccessIterator k, bool sampling, Compare comp);

template <typename RandomAccessIterator, typename Compare>
inline RandomAccessIterator
repeated_step_left(RandomAccessIterator first, RandomAccessIterator last,
                   RandomAccessIterator k, bool sampling, Compare comp);

template <typename RandomAccessIterator, typename Compare>
inline RandomAccessIterator
repeated_step_far_right(RandomAccessIterator first, RandomAccessIterator last,
                        RandomAccessIterator k, bool sampling, Compare comp);

template <typename RandomAccessIterator, typename Compare>
inline RandomAccessIterator
repeated_step_right(RandomAccessIterator first, RandomAccessIterator last,
                    RandomAccessIterator k, bool sampling, Compare comp);

template <typename RandomAccessIterator, typename Compare>
inline RandomAccessIterator
repeated_step_adaptive(RandomAccessIterator first, RandomAccessIterator last,
                       RandomAccessIterator k, bool sampling, Compare comp);

template <typename RandomAccessIterator, typename Compare>
void quickselect_adaptive(RandomAccessIterator first, RandomAccessIterator last,
                          RandomAccessIterator k, Compare comp);
}

template <typename RandomAccessIterator, typename Compare>
void quickselect_adaptive(RandomAccessIterator first, RandomAccessIterator last,
                          RandomAccessIterator k, Compare comp) {
  using ::std::add_lvalue_reference;

  internal::quickselect_adaptive<RandomAccessIterator,
                                 typename add_lvalue_reference<Compare>::type>(
      first, last, k, comp);
}

namespace internal {

template <typename RandomAccessIterator, typename Compare>
void quickselect_adaptive(RandomAccessIterator first, RandomAccessIterator last,
                          RandomAccessIterator k, Compare comp) {
  using ::std::distance;
  using ::std::forward;
  using ::std::advance;
  using ::std::swap;

  assert(first <= k && k < last);

  bool sampling = true;
  while (true) {
    const auto len = last - first;
    switch (len) {
    case 0:
    case 1:
      return;
    case 2:
      if (comp(*(--last), *first)) {
        swap(*first, *last);
      }
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
    const auto r = float(distance(first, k)) / float(len);
    decltype(first) p;
    if (r <= 7.0 / 16.0) {
      if (r <= 1.0 / 12.0) {
        p = repeated_step_far_left(first, last, k, sampling, comp);
        // TODO: Make this assert true again
        // assert(sampling || k <= p);
        if (sampling && (k > p || distance(p, last) < len / 12)) {
          sampling = false;
        }
      } else {
        p = repeated_step_left(first, last, k, sampling, comp);
        if (sampling &&
            (distance(first, p) < len / 6 || distance(p, last) < len / 4)) {
          sampling = false;
        }
      }
    } else if (r >= 1.0 - 7.0 / 16.0) {
      if (r >= 1.0 - 1.0 / 12.0) {
        p = repeated_step_far_right(first, last, k, sampling, comp);
        assert(sampling || k >= p);
        if (sampling && (k < p || distance(first, p) < len / 9)) {
          sampling = false;
        }
      } else {
        p = repeated_step_right(first, last, k, sampling, comp);
        if (sampling &&
            (distance(first, p) < len / 4 || distance(p, last) < len / 6)) {
          sampling = false;
        }
      }
    } else {
      p = repeated_step_adaptive(first, last, k, sampling, comp);
      if (sampling && (distance(first, p) < 2 * len / 9 ||
                       distance(p, last) < 2 * len / 9)) {
        sampling = false;
      }
    }
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

/**
 * Checks to see if the partition element is the minimum element.
 * Uses the structure of [first, last) provided by repeated_step_*.
 * Fixes the quadratic running time when there are many duplicate elements.
 *
 * Doesn't add a significant performance hit because in the common case it
 * is easy to find a counterexample.
 *
 * Performs worse when sampling is true, could possibly be sped up by looking
 * not at the first part of the array, but the part we already partitioned to
 * find our partition element.
 */
template <typename RandomAccessIterator, typename Compare>
inline RandomAccessIterator
check_min_partition(RandomAccessIterator first, RandomAccessIterator m_last,
                    RandomAccessIterator p, RandomAccessIterator last,
                    bool sampling, Compare comp) {
  using ::std::swap;

  if (sampling) {
    // TODO: Find something more efficient
    m_last = last;
  }
  // [first, m_last) must contain the minimum element
  for (auto it = first; it != m_last; ++it) {
    if (comp(*it, *p)) {
      return last;
    }
  }
  // *p <= [first, m_last) ==> *p is the minimum element
  // Partition into == p and > p
  assert(first != p);
  swap(*p, *first);
  p = first;
  // We now have a guard going down
  auto lm1 = last;
  --lm1;
  while (true) {
    while (comp(*p, *lm1)) {
      --lm1;
    }
    if (lm1 == first) {
      break;
    }
    swap(*(++first), *lm1);
  }
  // Return the last element == p
  return first;
}

template <typename RandomAccessIterator, typename Compare>
inline RandomAccessIterator
repeated_step_far_left(RandomAccessIterator first, RandomAccessIterator last,
                       RandomAccessIterator k, bool sampling, Compare comp) {
  using ::std::distance;
  using ::std::forward;
  using ::std::swap;
  using ::std::min;

  assert(first <= k && k < last);
  assert(distance(first, last) >= 12);

  auto f = distance(first, last) / 4;
  if (!sampling) {
    for (auto it = first; it != first + f; ++it) {
      lower_median4(it, it + f, it + 2 * f, it + 3 * f, comp);
    }
    // Make sure the minimum element is really in [first, first + f).
    for (auto it = first + 4 * f; it != last; ++it) {
      if (comp(*it, *first)) {
        swap(*it, *first);
      }
    }
  }
  auto f1 = f / 3;
  for (auto it = first + f; it != first + f + f1; ++it) {
    if (comp(*(it + f1), *it)) {
      swap(*it, *(it + f1));
    }
    if (comp(*(it + 2 * f1), *it)) {
      swap(*it, *(it + 2 * f1));
    }
  }

  const auto m = first + f + PROPORTION_FAR(f1);
#ifdef NTH
  std::nth_element(first + f, m, first + f + f1, comp);
#else
  quickselect_adaptive(first + f, first + f + f1, m, comp);
#endif
#ifdef MIN
  // We know that quickselect_adaptive partitions [first + f, m) <= m,
  // and that it tries its best to partition into [first + f, m) < m,
  // so check the first element in the range before we check if
  // *m is the minimum element to try to keep the common case that *m
  // is not the minimum as fast as possible.
  if (!comp(*(first + f), *m)) {
    // If *m is the minimum element, partition into
    // [firt, it + 1) == *m < [it + 1, last)
    const auto it =
        check_min_partition(first, first + f, m, last, sampling, comp);
    if (it != last) {
      return min(it, k);
    }
  }
#endif
  return expand_partition(first, last, first + f, m, first + f + f1, comp);
}

template <typename RandomAccessIterator, typename Compare>
inline RandomAccessIterator
repeated_step_far_right(RandomAccessIterator first, RandomAccessIterator last,
                        RandomAccessIterator k, bool sampling, Compare comp) {
  using ::std::distance;
  using ::std::forward;
  using ::std::swap;
  using ::std::min;

  assert(first <= k && k < last);
  assert(distance(first, last) >= 12);

  auto f = distance(first, last) / 4;
  if (!sampling) {
    for (auto it = first; it != first + f; ++it) {
      sort4(it, it + f, it + 2 * f, it + 3 * f, comp);
    }
    for (auto it = first + 4 * f; it != last; ++it) {
      if (comp(*it, *first)) {
        swap(*it, *first);
      }
    }
  }
  auto f1 = f / 3;
  for (auto it = first + 2 * (f + f1); it != first + 2 * f + 3 * f1; ++it) {
    if (comp(*it, *(it - f1))) {
      swap(*it, *(it - f1));
    }
    if (comp(*it, *(it - 2 * f1))) {
      swap(*it, *(it - 2 * f1));
    }
  }
  const auto m = first + 2 * (f + f1) + PROPORTION_FAR(f1);
#ifdef NTH
  std::nth_element(first + 2 * (f + f1), m, first + 2 * f + 3 * f1, comp);
#else
  quickselect_adaptive(first + 2 * (f + f1), first + 2 * f + 3 * f1, m, comp);
#endif
#ifdef MIN
  if (!comp(*(first + 2 * (f + f1)), *m)) {
    const auto it =
        check_min_partition(first, first + f, m, last, sampling, comp);
    if (it != last) {
      return min(it, k);
    }
  }
#endif
  return expand_partition(first, last, first + 2 * (f + f1), m,
                          first + 2 * f + 3 * f1, comp);
}

template <typename RandomAccessIterator, typename Compare>
inline RandomAccessIterator
repeated_step_left(RandomAccessIterator first, RandomAccessIterator last,
                   RandomAccessIterator k, bool sampling, Compare comp) {
  using ::std::distance;
  using ::std::forward;
  using ::std::min;
  using ::std::swap;

  assert(first <= k && k < last);
  assert(distance(first, last) >= 12);

  auto f = distance(first, last) / 4;
  if (!sampling) {
    for (auto it = first; it != first + f; ++it) {
      lower_median4(it, it + f, it + 2 * f, it + 3 * f, comp);
    }
    for (auto it = first + 4 * f; it != last; ++it) {
      if (comp(*it, *first)) {
        swap(*it, *first);
      }
    }
  }
  auto f1 = f / 3;
  for (auto it = first + f; it != first + f + f1; ++it) {
    sort3(it, it + f1, it + 2 * f1, comp);
  }
  const auto m = first + f + f1 + PROPORTION(f1);
#ifdef NTH
  std::nth_element(first + f + f1, m, first + f + 2 * f1, comp);
#else
  quickselect_adaptive(first + f + f1, first + f + 2 * f1, m, comp);
#endif
#ifdef MIN
  if (!comp(*(first + f + f1), *m)) {
    const auto it =
        check_min_partition(first, first + f, m, last, sampling, comp);
    if (it != last) {
      return min(it, k);
    }
  }
#endif
  return expand_partition(first, last, first + f + f1, m, first + f + 2 * f1,
                          comp);
}

template <typename RandomAccessIterator, typename Compare>
inline RandomAccessIterator
repeated_step_right(RandomAccessIterator first, RandomAccessIterator last,
                    RandomAccessIterator k, bool sampling, Compare comp) {
  using ::std::distance;
  using ::std::forward;
  using ::std::min;
  using ::std::swap;

  assert(first <= k && k < last);
  assert(distance(first, last) >= 12);

  const auto f = distance(first, last) / 4;
  if (!sampling) {
    for (auto it = first + f; it != first + 2 * f; ++it) {
      sort4(it - f, it, it + f, it + 2 * f, comp);
    }
    for (auto it = first + 4 * f; it != last; ++it) {
      if (comp(*it, *first)) {
        swap(*it, *first);
      }
    }
  }
  const auto f1 = f / 3;
  for (auto it = first + 2 * f; it != first + 2 * f + f1; ++it) {
    sort3(it, it + f1, it + 2 * f1, comp);
  }
  const auto m = first + 2 * f + f1 + PROPORTION(f1);
#ifdef NTH
  std::nth_element(first + 2 * f + f1, m, first + 2 * f + 2 * f1, comp);
#else
  quickselect_adaptive(first + 2 * f + f1, first + 2 * (f + f1), m, comp);
#endif
#ifdef MIN
  if (!comp(*(first + 2 * f + f1), *m)) {
    const auto it =
        check_min_partition(first, first + f, m, last, sampling, comp);
    if (it != last) {
      return min(it, k);
    }
  }
#endif
  return expand_partition(first, last, first + 2 * f + f1, m,
                          first + 2 * (f + f1), comp);
}

template <typename RandomAccessIterator, typename Compare>
inline RandomAccessIterator
repeated_step_adaptive(RandomAccessIterator first, RandomAccessIterator last,
                       RandomAccessIterator k, bool sampling, Compare comp) {
  using ::std::distance;
  using ::std::forward;
  using ::std::min;
  using ::std::swap;

  assert(first <= k && k < last);
  assert(distance(first, last) >= 12);

  auto f = distance(first, last) / 9;
  if (!sampling) {
    for (auto it = first; it != first + 3 * f; ++it) {
      sort3(it, it + 3 * f, it + 6 * f, comp);
    }
    for (auto it = first + 9 * f; it != last; ++it) {
      if (comp(*it, *first)) {
        swap(*it, *first);
      }
    }
  }
  for (auto it = first + 3 * f; it != first + 4 * f; ++it) {
    sort3(it, it + f, it + 2 * f, comp);
  }
  const auto m = first + 4 * f + PROPORTION(f);
#ifdef NTH
  std::nth_element(first + 4 * f, m, first + 5 * f, comp);
#else
  quickselect_adaptive(first + 4 * f, first + 5 * f, m, comp);
#endif
#ifdef MIN
  if (!comp(*(first + 4 * f), *m)) {
    const auto it =
        check_min_partition(first, first + 3 * f, m, last, sampling, comp);
    if (it != last) {
      return min(it, k);
    }
  }
#endif
  return expand_partition(first, last, first + 4 * f, m, first + 5 * f, comp);
}
}
}
