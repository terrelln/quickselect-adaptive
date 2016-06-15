partitions: partitions.cpp ../quickselect.hpp
	$(CXX) -O3 -pg -std=c++11 -L/usr/local/lib -lbenchmark -o partitions partitions.cpp

medians: medians.cpp ../quickselect.hpp
	$(CXX) -DNDEBUG -O3 -pg -std=c++11 -L/usr/local/lib -lbenchmark -o medians medians.cpp

quickselect: quickselect.cpp ../quickselect.hpp
	$(CXX) -O3 -DNDEBUG -g3 -pg -std=c++11 -L/usr/local/lib -lbenchmark -o quickselect quickselect.cpp

.PHONY: clean
clean:
	rm -rf *.dSYM partitions medians quickselect
