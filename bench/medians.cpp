#include "../quickselect.hpp"
#include <algorithm>
#include <benchmark/benchmark.h>

constexpr unsigned min = 1 << 10;
constexpr unsigned limit = 1 << 18;

namespace ten {
namespace internal {
template <typename Iterator, typename Compare>
inline void median3_baseline(Iterator first, Iterator second, Iterator third,
                             Compare comp) {
  using ::std::iter_swap;
  using ::std::forward;

  if (comp(*third, *second)) {
    if (comp(*second, *first)) {
      // third < second < first
      iter_swap(first, third);
    } else {
      // third < second, second >= first
      if (comp(*third, *first)) {
        // third < first <= second
        iter_swap(first, third);
        iter_swap(second, third);
      } else {
        // first <= third < second
        iter_swap(second, third);
      }
    }
  } else {
    // third >= second
    if (comp(*second, *first)) {
      iter_swap(first, second);
      if (comp(*third, *second)) {
        iter_swap(second, third);
      }
    }
  }
}

template <typename Iterator>
inline void maybe_iter_swap(Iterator a, Iterator b) {
  if (a != b) {
    ::std::iter_swap(a, b);
  }
}

template <typename Iterator, typename Compare>
inline void lower_median4_helper(Iterator first, Iterator second, Iterator a1,
                                 Iterator b1, Iterator a2, Iterator b2,
                                 Compare comp) {
  using std::iter_swap;
  using std::forward;

  assert(!comp(*b1, *a1));
  assert(!comp(*b2, *a2));

  if (comp(*a1, *a2)) {
    if (comp(*a2, *b1)) {
      maybe_iter_swap(first, a1);
      iter_swap(second, a2);
    } else {
      if (first != a1) {
        iter_swap(a1, b1);
      }
    }
  } else {
    if (comp(*a1, *b2)) {
      maybe_iter_swap(second, a1);
      iter_swap(first, a2);
    } else {
      iter_swap(first, a2);
      iter_swap(second, b2);
    }
  }
}

template <typename Iterator, typename Compare>
inline void lower_median4_baseline(Iterator first, Iterator second,
                                   Iterator third, Iterator fourth,
                                   Compare comp) {
  using ::std::iter_swap;
  using ::std::forward;

  if (comp(*first, *second)) {
    if (comp(*third, *fourth)) {
      lower_median4_helper(first, second, first, second, third, fourth, comp);
    } else {
      lower_median4_helper(first, second, first, second, fourth, third, comp);
    }
  } else {
    if (comp(*third, *fourth)) {
      lower_median4_helper(first, second, second, first, third, fourth, comp);
    } else {
      lower_median4_helper(first, second, second, first, fourth, third, comp);
    }
  }
}

template <typename Iterator, typename Compare>
inline void upper_median4_helper(Iterator third, Iterator fourth, Iterator a1,
                                 Iterator b1, Iterator a2, Iterator b2,
                                 Compare comp) {
  using std::iter_swap;
  using std::forward;

  assert(!comp(*a1, *b1));
  assert(!comp(*a2, *b2));

  if (comp(*a1, *a2)) {
    if (comp(*a1, *b2)) {
      if (a2 != fourth) {
        iter_swap(a2, b2);
      }
    } else {
      maybe_iter_swap(fourth, a2);
      iter_swap(third, a1);
    }
  } else {
    if (comp(*a2, *b1)) {
      iter_swap(fourth, a1);
      iter_swap(third, b1);
    } else {
      maybe_iter_swap(third, a2);
      iter_swap(fourth, a1);
    }
  }
}

template <typename Iterator, typename Compare>
inline void upper_median4_baseline(Iterator first, Iterator second,
                                   Iterator third, Iterator fourth,
                                   Compare comp) {
  using ::std::iter_swap;
  using ::std::forward;

  if (comp(*first, *second)) {
    if (comp(*third, *fourth)) {
      upper_median4_helper(third, fourth, second, first, fourth, third, comp);
    } else {
      upper_median4_helper(third, fourth, second, first, third, fourth, comp);
    }
  } else {
    if (comp(*third, *fourth)) {
      upper_median4_helper(third, fourth, first, second, fourth, third, comp);
    } else {
      upper_median4_helper(third, fourth, first, second, third, fourth, comp);
    }
  }
}

}
}

std::vector<int> random_vector(int size) {
  std::vector<int> vec;
  vec.reserve(size);

  for (int i = 0; i < size; ++i) {
    vec.push_back(rand() % 100);
  }
  return vec;
}

void BM_median3(benchmark::State &state) {
  std::vector<std::vector<int>> arrs = {
      {1, 2, 3}, {1, 3, 2}, {2, 1, 3}, {2, 3, 1}, {3, 1, 2},
      {3, 2, 1}, {1, 1, 1}, {1, 2, 2}, {2, 1, 1}, {2, 2, 1},
      {1, 1, 2}, {2, 1, 2}, {1, 2, 1}};
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto tmp = arrs;
    state.ResumeTiming();
    for (auto &arr : tmp) {
      ten::internal::median3(arr.begin(), arr.begin() + 1, arr.begin() + 2,
                             std::less<int>{});
      benchmark::DoNotOptimize(arr.data());
    }
  }
}
void BM_median3_baseline(benchmark::State &state) {
  std::vector<std::vector<int>> arrs = {
      {1, 2, 3}, {1, 3, 2}, {2, 1, 3}, {2, 3, 1}, {3, 1, 2},
      {3, 2, 1}, {1, 1, 1}, {1, 2, 2}, {2, 1, 1}, {2, 2, 1},
      {1, 1, 2}, {2, 1, 2}, {1, 2, 1}};
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto tmp = arrs;
    state.ResumeTiming();
    for (auto &arr : tmp) {
      ten::internal::median3_baseline(arr.begin(), arr.begin() + 1,
                                      arr.begin() + 2, std::less<int>{});
      benchmark::DoNotOptimize(arr.data());
    }
  }
}

void BM_lower_median4(benchmark::State &state) {
  std::vector<std::vector<int>> arrs = {
      {1, 2, 3, 4}, {1, 2, 4, 3}, {1, 3, 2, 4}, {1, 3, 4, 2}, {1, 4, 2, 3},
      {1, 4, 3, 2}, {2, 1, 3, 4}, {2, 1, 4, 3}, {2, 3, 1, 4}, {2, 3, 4, 1},
      {2, 4, 1, 3}, {2, 4, 3, 1}, {3, 1, 2, 4}, {3, 1, 4, 2}, {3, 2, 1, 4},
      {3, 2, 4, 1}, {3, 4, 1, 2}, {3, 4, 2, 1}, {4, 1, 2, 3}, {4, 1, 3, 2},
      {4, 2, 1, 3}, {4, 2, 3, 1}, {4, 3, 1, 2}, {4, 3, 2, 1}};
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto tmp = arrs;
    state.ResumeTiming();
    for (auto &arr : tmp) {
      ten::internal::lower_median4(arr.begin(), arr.begin() + 1,
                                   arr.begin() + 2, arr.begin() + 3,
                                   std::less<int>{});
      benchmark::DoNotOptimize(arr.data());
    }
  }
}

void BM_lower_median4_baseline(benchmark::State &state) {
  std::vector<std::vector<int>> arrs = {
      {1, 2, 3, 4}, {1, 2, 4, 3}, {1, 3, 2, 4}, {1, 3, 4, 2}, {1, 4, 2, 3},
      {1, 4, 3, 2}, {2, 1, 3, 4}, {2, 1, 4, 3}, {2, 3, 1, 4}, {2, 3, 4, 1},
      {2, 4, 1, 3}, {2, 4, 3, 1}, {3, 1, 2, 4}, {3, 1, 4, 2}, {3, 2, 1, 4},
      {3, 2, 4, 1}, {3, 4, 1, 2}, {3, 4, 2, 1}, {4, 1, 2, 3}, {4, 1, 3, 2},
      {4, 2, 1, 3}, {4, 2, 3, 1}, {4, 3, 1, 2}, {4, 3, 2, 1}};
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto tmp = arrs;
    state.ResumeTiming();
    for (auto &arr : tmp) {
      ten::internal::lower_median4_baseline(arr.begin(), arr.begin() + 1,
                                            arr.begin() + 2, arr.begin() + 3,
                                            std::less<int>{});
      benchmark::DoNotOptimize(arr.data());
    }
  }
}

void BM_upper_median4(benchmark::State &state) {
  std::vector<std::vector<int>> arrs = {
      {1, 2, 3, 4}, {1, 2, 4, 3}, {1, 3, 2, 4}, {1, 3, 4, 2}, {1, 4, 2, 3},
      {1, 4, 3, 2}, {2, 1, 3, 4}, {2, 1, 4, 3}, {2, 3, 1, 4}, {2, 3, 4, 1},
      {2, 4, 1, 3}, {2, 4, 3, 1}, {3, 1, 2, 4}, {3, 1, 4, 2}, {3, 2, 1, 4},
      {3, 2, 4, 1}, {3, 4, 1, 2}, {3, 4, 2, 1}, {4, 1, 2, 3}, {4, 1, 3, 2},
      {4, 2, 1, 3}, {4, 2, 3, 1}, {4, 3, 1, 2}, {4, 3, 2, 1}};
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto tmp = arrs;
    state.ResumeTiming();
    for (auto &arr : tmp) {
      ten::internal::upper_median4(arr.begin(), arr.begin() + 1,
                                   arr.begin() + 2, arr.begin() + 3,
                                   std::less<int>{});
      benchmark::DoNotOptimize(arr.begin());
      benchmark::DoNotOptimize(arr.begin() + 1);
      benchmark::DoNotOptimize(arr.begin() + 2);
      benchmark::DoNotOptimize(arr.begin() + 3);
    }
  }
}

void BM_upper_median4_baseline(benchmark::State &state) {
  std::vector<std::vector<int>> arrs = {
      {1, 2, 3, 4}, {1, 2, 4, 3}, {1, 3, 2, 4}, {1, 3, 4, 2}, {1, 4, 2, 3},
      {1, 4, 3, 2}, {2, 1, 3, 4}, {2, 1, 4, 3}, {2, 3, 1, 4}, {2, 3, 4, 1},
      {2, 4, 1, 3}, {2, 4, 3, 1}, {3, 1, 2, 4}, {3, 1, 4, 2}, {3, 2, 1, 4},
      {3, 2, 4, 1}, {3, 4, 1, 2}, {3, 4, 2, 1}, {4, 1, 2, 3}, {4, 1, 3, 2},
      {4, 2, 1, 3}, {4, 2, 3, 1}, {4, 3, 1, 2}, {4, 3, 2, 1}};
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto tmp = arrs;
    state.ResumeTiming();
    for (auto &arr : tmp) {
      ten::internal::upper_median4_baseline(arr.begin(), arr.begin() + 1,
                                            arr.begin() + 2, arr.begin() + 3,
                                            std::less<int>{});
      benchmark::DoNotOptimize(arr.begin());
      benchmark::DoNotOptimize(arr.begin() + 1);
      benchmark::DoNotOptimize(arr.begin() + 2);
      benchmark::DoNotOptimize(arr.begin() + 3);
    }
  }
}
BENCHMARK(BM_median3_baseline);
BENCHMARK(BM_median3);
BENCHMARK(BM_lower_median4_baseline);
BENCHMARK(BM_lower_median4);
BENCHMARK(BM_upper_median4_baseline);
BENCHMARK(BM_upper_median4);
BENCHMARK_MAIN();
