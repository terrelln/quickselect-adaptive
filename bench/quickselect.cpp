#include "../quickselect.hpp"
#include <algorithm>
#include <benchmark/benchmark.h>
#include <tuple>
#include <unordered_map>

constexpr unsigned min = 1 << 10;
constexpr unsigned limit = 1 << 18;
constexpr unsigned mod = 100000;

namespace std {
template <> struct hash<std::pair<int, unsigned>> {
  size_t operator()(const std::pair<int, unsigned> &p) const {
    return hash<int>{}(p.first) ^ hash<unsigned>{}(p.second);
  };
};
}

std::vector<int> random_vector(int size, unsigned mod) {
  static std::unordered_map<std::pair<int, unsigned>, std::vector<int>> vecs;
  auto tup = std::make_pair(size, mod);
  if (vecs.count(tup))
    return vecs[tup];

  auto &vec = vecs[tup];
  vec.reserve(size);

  for (int i = 0; i < size; ++i) {
    vec.push_back(rand() % mod);
  }
  return vec;
}

void BM_quickselect_repeat(benchmark::State &state) {
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto vec = random_vector(state.range_x(), 100);
    state.ResumeTiming();
    ten::quickselect_adaptave(vec.begin(), vec.end(),
                              vec.begin() + vec.size() / 2, std::less<int>{});
    benchmark::DoNotOptimize(vec.data());
  }
}

void BM_quickselect_repeat_baseline(benchmark::State &state) {
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto vec = random_vector(state.range_x(), 100);
    state.ResumeTiming();
    std::nth_element(vec.begin(), vec.begin() + vec.size() / 2, vec.end(),
                     std::less<int>{});
    benchmark::DoNotOptimize(vec.data());
  }
}
void BM_quickselect(benchmark::State &state) {
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto vec = random_vector(state.range_x(), mod);
    state.ResumeTiming();
    ten::quickselect_adaptave(vec.begin(), vec.end(),
                              vec.begin() + vec.size() / 2, std::less<int>{});
    benchmark::DoNotOptimize(vec.data());
  }
}

void BM_quickselect_baseline(benchmark::State &state) {
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto vec = random_vector(state.range_x(), mod);
    state.ResumeTiming();
    std::nth_element(vec.begin(), vec.begin() + vec.size() / 2, vec.end(),
                     std::less<int>{});
    benchmark::DoNotOptimize(vec.data());
  }
}

void BM_quickselect_median3(benchmark::State &state) {
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto vec = random_vector(state.range_x(), mod);
    state.ResumeTiming();
    ten::internal::quickselect(
        ten::internal::three_medians<decltype(vec)::iterator, std::less<int>>,
        vec.begin(), vec.end(), vec.begin() + vec.size() / 2, std::less<int>{});
    benchmark::DoNotOptimize(vec.data());
  }
}

// BENCHMARK(BM_quickselect_repeat)->Range(min, limit);
// BENCHMARK(BM_quickselect_repeat_baseline)->Range(min, limit);
BENCHMARK(BM_quickselect)->Range(min, limit);
BENCHMARK(BM_quickselect_baseline)->Range(min, limit);
// BENCHMARK(BM_quickselect_median3)->Range(min, limit);
BENCHMARK_MAIN();
