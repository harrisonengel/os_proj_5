all: lamport

lamport: lamport.cpp
	mpic++ -o lamport -std=c++11 lamport.cpp
