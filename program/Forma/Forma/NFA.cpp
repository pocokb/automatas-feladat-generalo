#include "stdafx.h"
#include "NFA.h"
#include<map>
#include<stack>
#include<iostream>
#include"conversions.h"
using namespace std;


DFA NFA::determinize() {
	DFA dfa;
	dfa.startState = this->startState;

	stack<set<int>> stack;
	set<set<int>> createdAndScheduled;
	map<set<int>, int> statesToNum;	
	auto transMap = this->getTransitionsAsMap();

	stack.push({this->startState});	
	statesToNum[stack.top()] = 0; //kezdı·llapot a 0
	createdAndScheduled.insert(stack.top());

	dfa.nStates = 1; //ezt haszn·ljuk egyben indexnek is (0-val kezd)
	set<char> abc = this->getABC();	
	while (!stack.empty()) {				
		set<int> top = stack.top();
		stack.pop();		
		for (int st : top) { //nÈzz¸k vÈg·llapot-e						
			if (this->goalStates.count(st)) {				
				//cout << statesToNum[top];
				dfa.goalStates.insert(statesToNum[top]);
			}
		}		
		for (char c : abc) {			
			set<int> toStatesTotal;
			for (int fromState : top) {								
				set<int> toStatesSingle = transMap[{fromState, c}];
				//cout << toStatesSingle.size() << endl;
				toStatesTotal.insert(toStatesSingle.begin(),toStatesSingle.end());
			}						
			if (createdAndScheduled.count(toStatesTotal) != 1) { //lÈtre kell hozni
				statesToNum[toStatesTotal] = dfa.nStates;
				dfa.nStates++;
				stack.push(toStatesTotal);				
				createdAndScheduled.insert(toStatesTotal);
			}
			//behuzzok a nyilat a kÈsz¸lı dfa-ba
			dfa.transitions.insert(DFA::Transition{ statesToNum[top],statesToNum[toStatesTotal],c });			
		}		
	}
	return dfa;
}


NFA NFA::epsilonMent(){
	Language lang = DFAToRegLang(*this);
	//lang.printDescription();
	//lang.cfGenerateWords(15);
	//lang.printWords();
		
	lang.lancMent();

	//lang.printDescription();
	//lang.cfGenerateWords(15);
	//lang.printWords();	
	return NFA(RegLangToDFA(lang));
}

DFA NFA::toDFA(){	
	return this->epsilonMent().determinize().minimize();
}

NFA::NFA(){
}

NFA::NFA(int startState, int nStates, initializer_list<Transition> transitions, initializer_list<int> goalStates)
	: DFA(startState, nStates, transitions, goalStates){
}
/*NFA::NFA(int startState, int nStates, set<Transition> transitions, set<int> goalStates)
	: DFA(startState, nStates, transitions, goalStates) {
}*/

NFA::NFA(DFA dfa) : DFA(dfa.startState, dfa.nStates, dfa.transitions, dfa.goalStates) {			
}

NFA::~NFA()
{
}
