#pragma once
#include"DFA.h"
#include"language.h"

class DFABuilder{
public:
	
	DFABuilder();

	~DFABuilder();
};

DFA buildDFA(SynthTreeNode *node);

namespace dfabuilder {
	//DFA exact_number(int n);
	DFA numberWithRelation(int n, string relation);
	DFA divisibleBy(int n);
	DFA even();
	DFA odd();
	vector<int>	counter();
	DFA counterEqualsNumber(vector<int> cntr, DFA nmbr);
	DFA occuranceEqualsNumber(char letter, DFA nmbr);
	DFA everyNPropertyNonDet(int n, DFA check);
	DFA or (DFA sw1, DFA sw2);
	DFA startsWith(DFA sym_word);
	DFA endsWith(DFA sym_word);
	DFA star(DFA dfa);
	DFA reachedGoalNTimes(DFA dfa, DFA nTimes);
	DFA specific_word(string word);
	DFA allFollowedBy(char letter1, char letter2);
}
namespace interpreter{
	DFA symbolic_word(SynthTreeNode *node);
	DFA symbolic_number_finite(SynthTreeNode * node);
	DFA symbolic_number(SynthTreeNode *node);
	DFA startsWith(SynthTreeNode * node);
}
