#pragma once
#include<vector>
#include<set>
#include"Language.h"
#include<iostream>
using namespace std;

class DFA{
public:
	struct Transition {
		int from, to;
		char on;
		bool operator<(const Transition& other) const {			
			return from < other.from || (from == other.from && (on < other.on || (on == other.on && to < other.to)));						
		}
	};

	int nStates;
	int startState;
	set<Transition> transitions;
	set<int> goalStates;
	DFA();
	DFA(int startState, int nStates, initializer_list<Transition> transitions, initializer_list<int> goalStates);
	DFA(int startState, int nStates, set<Transition> transitions, set<int> goalStates);
	~DFA();

	bool accepts(string str);
	set<char> getABC();
	void print();
	//bool addTransitionIfNotExists(Transition t);

	struct hCondition {
		int from;
		char on;
		bool operator<(const hCondition& other) const { 
			if (from == other.from) {
				return on < other.on;
			}else {
				return from < other.from;
			}
			
		}
	};
	map<hCondition, set<int>> getTransitionsAsMap();

	void feleselhagy();

	bool equals(DFA other);

	static DFA sigmaStar(set<char> abc);
	
	DFA minimize();
	static DFA nothing();
	static DFA complement(DFA m);
	static DFA and(DFA m1, DFA m2);
	static DFA or(DFA m1, DFA m2);
	static DFA append(DFA m1, DFA m2);
	//static Language convertToLanguage();

};

