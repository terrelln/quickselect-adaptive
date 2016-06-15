#pragma once

#include <algorithm>
#include <cassert>
#include <iterator>

/**
 * Macros for altering behavior for benchmarking & testing
 * TODO: Remove eventually
 */
#define MIN
//#define NTH
#define PROPORTION(f) f * distance(first, k) / distance(first, last)
//#define PROPORTION(f) f / 2
//#define PROPORTION_FAR(f) f * distance(first, k) / distance(first, last)
#define PROPORTION_FAR(f) f / 2

namespace ten {

// Helper functions
namespace internal {

template <typename RandomAccessIterator, typename Compare>
inline RandomAccessIterator
expand_partition(RandomAccessIterator first, RandomAccessIterator last,
                 RandomAccessIterator p_first, RandomAccessIterator p,
                 RandomAccessIterator p_lm1, Compare comp);

template <typename RandomAccessIterator, typename Compare>
inline RandomAccessIterator
hoare_partition(RandomAccessIterator first, RandomAccessIterator last,
                RandomAccessIterator p, Compare comp);

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
inline void selection_sort(RandomAccessIterator first,
                           RandomAccessIterator last, Compare comp);

template <typename RandomAccessIterator, typename Compare>
void quickselect_adaptave(RandomAccessIterator first, RandomAccessIterator last,
                          RandomAccessIterator k, Compare comp);
}

template <typename RandomAccessIterator, typename Compare>
void quickselect_adaptave(RandomAccessIterator first, RandomAccessIterator last,
                          RandomAccessIterator k, Compare comp) {
  using ::std::add_lvalue_reference;

  internal::quickselect_adaptave<RandomAccessIterator,
                                 typename add_lvalue_reference<Compare>::type>(
      first, last, k, comp);
}

namespace internal {

template <typename Iterator, typename Compare>
inline void median3(Iterator first, Iterator second, Iterator third,
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

template <typename RandomAccessIterator, typename Compare>
void quickselect_adaptave(RandomAccessIterator first, RandomAccessIterator last,
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
      median3(first, ++m, --last, comp);
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
      median3(first, ++m, --last, comp);
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
RandomAccessIterator hoare_partition(RandomAccessIterator first,
                                     RandomAccessIterator last,
                                     RandomAccessIterator p, Compare comp) {
  using ::std::swap;
  using ::std::forward;

  assert(first <= p && p < last);

  if (first != p) {
    swap(*first, *p);
    p = first;
  }
  ++first;
  while (true) {
    while (true) {
      if (first == last) {
        --first;
        swap(*p, *first);
        return first;
      }
      if (!comp(*first, *p)) {
        break;
      }
      ++first;
    }
    do {
      if (first == --last) {
        --first;
        swap(*p, *first);
        return first;
      }
    } while (!comp(*last, *p));
    swap(*first, *last);
    ++first;
  }
}

enum class GuardsFound { Left, Right, Both };
// TODO: Simplify
template <typename RandomAccessIterator, typename Compare>
inline GuardsFound
find_guards(RandomAccessIterator &first, RandomAccessIterator &lm1,
            RandomAccessIterator &p_first, RandomAccessIterator p,
            RandomAccessIterator &p_lm1, Compare comp) {
  using ::std::swap;
  assert(first < p && p < lm1);
  assert(first <= p_first && p_first <= p && p <= p_lm1 && p_lm1 <= lm1);

  if (p_first != p && p != p_lm1 && comp(*p_first, *p)) {
    assert(comp(*p_first, *p));
    assert(!comp(*p_lm1, *p));
    swap(*p_first, *p_lm1);
    return GuardsFound::Both;
  }

  if (p_first == p) {
    --p_first;
  }

  if (p_lm1 == p) {
    ++p_lm1;
  }

  GuardsFound to_guard = GuardsFound::Both;
  {
    const bool b_first = comp(*p_first, *p);
    const bool b_lm1 = comp(*p_lm1, *p);
    if (!b_first && b_lm1) {
      return GuardsFound::Both;
    } else if (b_first && b_lm1) {
      to_guard = GuardsFound::Left;
    } else if (!b_first && !b_lm1) {
      to_guard = GuardsFound::Right;
    } else {
      swap(*p_first, *p_lm1);
      return GuardsFound::Both;
    }
  }

  if (to_guard == GuardsFound::Left) {
    if (comp(*p_first, *p)) {
      while (first != p_first && comp(*first, *p)) {
        ++first;
      }
      if (first == p_first) {
        return GuardsFound::Right;
      }
      swap(*p_first, *first);
      ++first;
    }
    return GuardsFound::Both;
  }
  assert(to_guard == GuardsFound::Right);
  if (!comp(*p_lm1, *p)) {
    while (lm1 != p_lm1 && !comp(*lm1, *p)) {
      --lm1;
    }
    if (lm1 == p_lm1) {
      return GuardsFound::Left;
    }
    swap(*p_lm1, *lm1);
    --lm1;
  }
  return GuardsFound::Both;
}

// TODO: This function needs simplification, and more optimizations.
// Currently 50% of the time is spent in this function.
template <typename RandomAccessIterator, typename Compare>
inline RandomAccessIterator
expand_partition(RandomAccessIterator first, RandomAccessIterator last,
                 RandomAccessIterator p_first, RandomAccessIterator p,
                 RandomAccessIterator p_last, Compare comp) {
  using ::std::swap;

  auto p_lm1 = p_last;
  --p_lm1;
  auto lm1 = last;
  --lm1;
  assert(first <= p_first && p_first <= p && p <= p_lm1 && p_lm1 <= lm1);

  if (first == lm1) {
    return first;
  }
  if (first == p || p == lm1) {
    return hoare_partition(first, last, p, comp);
  }
  switch (find_guards(first, lm1, p_first, p, p_lm1, comp)) {
  case GuardsFound::Left:
    assert(p_lm1 == lm1);
    return hoare_partition(first, p + 1, p, comp);
  case GuardsFound::Right:
    assert(first == p_first);
    return hoare_partition(p, last, p, comp);
  case GuardsFound::Both:
    break;
  }

  assert(comp(*p_lm1, *p));
  assert(!comp(*p_first, *p));
  assert(first <= p_first && p_first <= p && p <= p_lm1 && p_lm1 <= lm1);

  while (true) {
    while (comp(*first, *p)) {
      ++first;
    }
    while (!comp(*lm1, *p)) {
      --lm1;
    }
    if (first == p_first || lm1 == p_lm1) {
      break;
    }
    swap(*first, *lm1);
    ++first;
    --lm1;
  }
  // At least one side of the partition is done.
  // Finish up the other side.
  if (lm1 != p_lm1) {
    while (!comp(*lm1, *p)) {
      --lm1;
    }
    if (lm1 == p_lm1) {
      swap(*p_first, *p_lm1);
      return p;
    }
    swap(*p, *lm1);
    first = p;
    ++first;
    p = lm1--;
    while (true) {
      if (first == p_lm1) {
        swap(*p_first, *p_lm1);
        swap(*p, *p_lm1);
        return hoare_partition(p_lm1, lm1 + 1, p_lm1, comp);
      }
      while (!comp(*lm1, *p)) {
        --lm1;
      }
      if (lm1 == p_lm1) {
        swap(*p_first, *p_lm1);
        swap(*p, *first);
        return first;
      }
      swap(*first, *lm1);
      ++first;
      --lm1;
    }
  } else if (first != p_first) {
    while (comp(*first, *p)) {
      ++first;
    }
    if (first == p_first) {
      swap(*p_first, *p_lm1);
      return p;
    }
    swap(*p, *first);
    lm1 = p;
    --lm1;
    p = first++;
    while (true) {
      if (lm1 == p_first) {
        swap(*p_first, *p_lm1);
        swap(*p, *p_first);
        return hoare_partition(first, p_first + 1, p_first, comp);
      }
      while (comp(*first, *p)) {
        ++first;
      }
      if (first == p_first) {
        swap(*p_first, *p_lm1);
        swap(*p, *lm1);
        return lm1;
      }
      swap(*first, *lm1);
      ++first;
      --lm1;
    }
  } else {
    swap(*p_first, *p_lm1);
    return p;
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
  quickselect_adaptave(first + f, first + f + f1, m, comp);
#endif
#ifdef MIN
  // If *m is the minimum element, partition into
  // [firt, it + 1) == *m < [it + 1, last)
  const auto it =
      check_min_partition(first, first + f, m, last, sampling, comp);
  if (it != last) {
    return min(it, k);
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
  quickselect_adaptave(first + 2 * (f + f1), first + 2 * f + 3 * f1, m, comp);
#endif
#ifdef MIN
  const auto it =
      check_min_partition(first, first + f, m, last, sampling, comp);
  if (it != last) {
    return min(it, k);
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
    median3(it, it + f1, it + 2 * f1, comp);
  }
  const auto m = first + f + f1 + PROPORTION(f1);
#ifdef NTH
  std::nth_element(first + f + f1, m, first + f + 2 * f1, comp);
#else
  quickselect_adaptave(first + f + f1, first + f + 2 * f1, m, comp);
#endif
#ifdef MIN
  const auto it =
      check_min_partition(first, first + f, m, last, sampling, comp);
  if (it != last) {
    return min(it, k);
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
    median3(it, it + f1, it + 2 * f1, comp);
  }
  const auto m = first + 2 * f + f1 + PROPORTION(f1);
#ifdef NTH
  std::nth_element(first + 2 * f + f1, m, first + 2 * f + 2 * f1, comp);
#else
  quickselect_adaptave(first + 2 * f + f1, first + 2 * (f + f1), m, comp);
#endif
#ifdef MIN
  const auto it =
      check_min_partition(first, first + f, m, last, sampling, comp);
  if (it != last) {
    return min(it, k);
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
      median3(it, it + 3 * f, it + 6 * f, comp);
    }
    for (auto it = first + 9 * f; it != last; ++it) {
      if (comp(*it, *first)) {
        swap(*it, *first);
      }
    }
  }
  for (auto it = first + 3 * f; it != first + 4 * f; ++it) {
    median3(it, it + f, it + 2 * f, comp);
  }
  const auto m = first + 4 * f + PROPORTION(f);
#ifdef NTH
  std::nth_element(first + 4 * f, m, first + 5 * f, comp);
#else
  quickselect_adaptave(first + 4 * f, first + 5 * f, m, comp);
#endif
#ifdef MIN
  const auto it =
      check_min_partition(first, first + 3 * f, m, last, sampling, comp);
  if (it != last) {
    return min(it, k);
  }
#endif
  return expand_partition(first, last, first + 4 * f, m, first + 5 * f, comp);
}

template <typename RandomAccessIterator, typename Compare>
inline RandomAccessIterator
three_medians(RandomAccessIterator first, RandomAccessIterator last,
              RandomAccessIterator, bool, Compare comp) {
  median3(first, first + std::distance(first, last) / 2, last - 1, comp);
  return hoare_partition(first, last, first + std::distance(first, last) / 2,
                         comp);
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
}
