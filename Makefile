all: lamport vector

lamport: lamport.cpp
	mpic++ -o lamport -std=c++11 lamport.cpp

vector: vector.cpp
	mpic++ -o vector -std=c++11 vector.cpp
