#pragma once
#include"DFA.h"
#include<set>

class NFA : public DFA{
public:	
	NFA();
	NFA(int startState, int nStates, initializer_list<Transition> transitions, initializer_list<int> goalStates);
	//NFA(int startState, int nStates, set<Transition> transitions, set<int> goalStates);
	NFA(DFA dfa);
	~NFA();

	DFA determinize();
	NFA epsilonMent();
	DFA toDFA();
};

