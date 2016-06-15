#include "../quickselect.hpp"
#include <algorithm>
#include <benchmark/benchmark.h>
#include <unordered_map>

using std::get;

constexpr unsigned min = 1 << 10;
constexpr unsigned limit = 1 << 18;
constexpr unsigned mod = 100;

std::vector<int> random_vector(int size) {
  static std::unordered_map<int, std::vector<int>> vecs;
  if (vecs.count(size))
    return vecs[size];

  auto &vec = vecs[size];
  vec.reserve(size);

  for (int i = 0; i < size; ++i) {
    vec.push_back(rand() % mod);
  }
  return vec;
}

int partition(int size, int idx) {
  static std::unordered_map<int, std::vector<int>> ps;
  while (idx >= (int)ps[size].size())
    ps[size].push_back(rand() % size);
  return ps[size][idx];
}

std::tuple<int, int, int> partition_range(int size, int idx) {
  static std::unordered_map<int, std::vector<std::tuple<int, int, int>>> ps;
  while (idx >= (int)ps[size].size()) {
    int p = rand() % size;
    int a = rand() % (p + 1);
    auto b =
        p + 1 + (size == p + 1 ? 0 : (rand() % (size - (p + 1))));
    ps[size].emplace_back(a, p, b);
  }
  return ps[size][idx];
}

template <typename It, typename Compare>
It hoare_partition_baseline(It begin, It end, It p, Compare) {
  std::swap(*begin, *p);
  p = begin;
  ++begin;
  auto p1 = std::partition(begin, end, [p](int x) { return x <= *p; });
  --p1;
  std::swap(*p, *p1);
  return p1;
}

void BM_hoare(benchmark::State &state) {
  int i = 0;
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto v = random_vector(state.range_x());
    auto p = partition(v.size(), i++);
    state.ResumeTiming();
    auto p1 = ten::internal::hoare_partition(v.begin(), v.end(), v.begin() + p,
                                             std::less<int>{});
    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(p1);
  }
}

void BM_hoare_baseline(benchmark::State &state) {
  int i = 0;
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto v = random_vector(state.range_x());
    auto p = partition(v.size(), i++);
    state.ResumeTiming();
    auto p1 = ten::internal::hoare_partition_baseline(
        v.begin(), v.end(), v.begin() + p, std::less<int>{});
    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(p1);
  }
}

void BM_std(benchmark::State &state) {
  int i = 0;
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto v = random_vector(state.range_x());
    auto p = partition(v.size(), i++);
    state.ResumeTiming();
    auto p1 = hoare_partition_baseline(v.begin(), v.end(), v.begin() + p,
                                       std::less<int>{});
    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(p1);
  }
}

void BM_expand(benchmark::State &state) {
  int i = 0;
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto v = random_vector(state.range_x());
    auto apb = partition_range(v.size(), i++);
    std::sort(v.begin() + get<0>(apb), v.begin() + get<2>(apb));
    state.ResumeTiming();
    auto p1 = ten::internal::expand_partition(
        v.begin(), v.end(), v.begin() + get<0>(apb), v.begin() + get<1>(apb),
        v.begin() + get<2>(apb), std::less<int>{});
    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(p1);
  }
}

void BM_expand_baseline(benchmark::State &state) {
  int i = 0;
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto v = random_vector(state.range_x());
    auto apb = partition_range(v.size(), i++);
    std::sort(v.begin() + get<0>(apb), v.begin() + get<2>(apb));
    state.ResumeTiming();
    auto p1 = ten::internal::expand_partition_baseline(
        v.begin(), v.end(), v.begin() + get<0>(apb), v.begin() + get<1>(apb),
        v.begin() + get<2>(apb), std::less<int>{});
    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(p1);
  }
}

void BM_far_left(benchmark::State &state) {
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto v = random_vector(state.range_x());
    auto p = rand() % (v.size() / 12);
    state.ResumeTiming();
    auto p1 = ten::internal::repeated_step_far_left(
        v.begin(), v.end(), v.begin() + p, false, std::less<int>{});
    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(p1);
  }
}

void BM_far_right(benchmark::State &state) {
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto v = random_vector(state.range_x());
    auto p = (v.size() - 1) - (rand() % (v.size() / 12));
    state.ResumeTiming();
    auto p1 = ten::internal::repeated_step_far_right(
        v.begin(), v.end(), v.begin() + p, false, std::less<int>{});
    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(p1);
  }
}

void BM_left(benchmark::State &state) {
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto v = random_vector(state.range_x());
    // auto p = rand() % (v.size() / 2);
    auto p = rand() % (v.size() / 12);
    state.ResumeTiming();
    auto p1 = ten::internal::repeated_step_left(
        v.begin(), v.end(), v.begin() + p, false, std::less<int>{});
    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(p1);
  }
}

void BM_right(benchmark::State &state) {
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto v = random_vector(state.range_x());
    auto p = v.size() / 2 + (rand() % (v.size() / 2));
    state.ResumeTiming();
    auto p1 = ten::internal::repeated_step_right(
        v.begin(), v.end(), v.begin() + p, false, std::less<int>{});
    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(p1);
  }
}

void BM_adaptive(benchmark::State &state) {
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto v = random_vector(state.range_x());
    auto p = v.size() / 2;
    state.ResumeTiming();
    auto p1 = ten::internal::repeated_step_adaptive(
        v.begin(), v.end(), v.begin() + p, false, std::less<int>{});
    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(p1);
  }
}

void BM_adaptive_baseline(benchmark::State &state) {
  while (state.KeepRunning()) {
    state.PauseTiming();
    auto v = random_vector(state.range_x());
    auto p = v.size() / 2;
    state.ResumeTiming();
    auto p1 = ten::internal::repeated_step_adaptive_baseline(
        v.begin(), v.end(), v.begin() + p, false, std::less<int>{});
    benchmark::DoNotOptimize(v);
    benchmark::DoNotOptimize(p1);
  }
}

BENCHMARK(BM_hoare_baseline)->Range(min, limit);
BENCHMARK(BM_hoare)->Range(min, limit);
BENCHMARK(BM_expand)->Range(min, limit);
BENCHMARK(BM_expand_baseline)->Range(min, limit);
BENCHMARK(BM_far_left)->Range(min, limit);
BENCHMARK(BM_far_right)->Range(min, limit);
BENCHMARK(BM_left)->Range(min, limit);
BENCHMARK(BM_right)->Range(min, limit);
BENCHMARK(BM_adaptive)->Range(min, limit);
BENCHMARK(BM_adaptive_baseline)->Range(min, limit);
BENCHMARK(BM_adaptive)->Range(min, limit);
BENCHMARK(BM_adaptive_baseline)->Range(min, limit);
BENCHMARK_MAIN();
