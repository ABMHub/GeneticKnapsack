all:
	g++ -fPIC -shared -o knapsack.so knapsack.cpp
	python knapsack.py