all: lamport

lamport: lamport.cpp
	mpic++ -o proj5 -std=c++11 lamport.cpp
