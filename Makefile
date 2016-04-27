all: proj_5_main

proj_5_main: proj_5_main.cpp
	mpic++ -o proj5 -std=c++11 proj_5_main.cpp
