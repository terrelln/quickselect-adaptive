all: quickselect hoare expand medians repeated

quickselect: quickselect.cpp ../quickselect.hpp
	$(CXX) -O3 -std=c++11 quickselect.cpp -o quickselect

hoare: hoare_partition.cpp ../quickselect.hpp
	$(CXX) -O3 -std=c++11 hoare_partition.cpp -o hoare

expand: expand.cpp ../quickselect.hpp
	$(CXX) -O3 -std=c++11 expand.cpp -o expand

medians: medians.cpp ../quickselect.hpp
	$(CXX) -O3 -std=c++11 medians.cpp -o medians

repeated: repeated.cpp ../quickselect.hpp
	$(CXX) -O3 -std=c++11 repeated.cpp -o repeated

.PHONY: clean

clean:
	rm -rf *.dSYM quickselect hoare expand medians repeated
