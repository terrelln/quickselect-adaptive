#pragma once

#include <cassert>
#include <utility>

namespace ten {
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

namespace internal {
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
}

// TODO: This function needs simplification, and more optimizations.
// Currently 50% of the time is spent in this function.
template <typename RandomAccessIterator, typename Compare>
inline RandomAccessIterator
expand_partition(RandomAccessIterator first, RandomAccessIterator last,
                 RandomAccessIterator p_first, RandomAccessIterator p,
                 RandomAccessIterator p_last, Compare comp) {
  using ::std::swap;
  using namespace internal;

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
}
