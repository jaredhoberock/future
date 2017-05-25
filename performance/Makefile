all: experimental_make_ready_future make_ready_future

CXX = clang

experimental_make_ready_future: experimental_make_ready_future.cpp
	$(CXX) -std=c++14 -I.. -lstdc++ -pthread -O3 experimental_make_ready_future.cpp -o experimental_make_ready_future

make_ready_future: make_ready_future.cpp
	$(CXX) -std=c++14 -I.. -lstdc++ -pthread -O3 make_ready_future.cpp -o make_ready_future

clean:
	rm -f experimental_make_ready_future make_ready_future

