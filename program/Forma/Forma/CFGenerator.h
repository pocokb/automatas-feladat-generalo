#pragma once
#include"DFA.h"
#include<string>

struct Result {
	string task;
	DFA solution;
};

vector<Result> CFGenerator();