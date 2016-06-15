# Quickselect Adaptive

C++ implementation of the algorithm described in [Fast Deterministic Selection](http://arxiv.org/abs/1606.00484) by Andrei Alexandrescu.
There is only one significant difference between my algorithm and the algorithm described in the paper (that I'm aware of).
The algorithm as described (or at least as I initially implemented it) doesn't handle repeated elements well, it degrades to quadratic behavior.

## Fixing the quadratic behavior

First, I normally partition the range `[first, last)` into elements less than the partition, and elements greater than or equal to the partition.
The exception is that in the `repeated_step_*` functions, I don't check to make sure that elements pre-partitioned by the call to `quickselect_adaptive` and to the left of the partition are strictly less than the partition, they could be equal.

If the partition is the minimum element the algorithm will only chop off one element for each partition, which degrades the performance to quadratic.
My solution was to check to see if the partition chosen is the minimum element in `repeated_step_*`, and if it is partition instead into elements equal to the partition and elements greater than the partition.
The nice thing about this check though, is that when we are doing `repeated_step_*`, after the first median step we know that the minimum element must be in the first 1/3 or 1/4 of the array (depending on the algorithm), so we don't have to check the whole array (unless we are sampling).
It might also make sense to check the first element in the partitioned region, but I don't currently do that.

# Goals

* I would like to improve the performance of `expand_partition`, currently about 50% of the running time is spent in that function, so a 10-20% improvement would be huge.
* Be as fast or faster than libcxx's `std::nth_element` in all or almost all cases.  It is already faster for finding the median in some of my benchmarks (they could be flawed, I don't have much experience benchmarking).
* Add to libcxx.
