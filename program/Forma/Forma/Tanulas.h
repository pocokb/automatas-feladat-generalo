#pragma once
#include<iostream>
#include<string>
using namespace std;
class Tanulas{
public:
	struct Tapasztalat {
		Tapasztalat *state;
		int input;
		string output;
		friend ostream& operator<<(ostream& os, const Tapasztalat& t);				
	};
	struct Game {
		string interact(int input);
	};
	Tanulas();
	~Tanulas();
};

