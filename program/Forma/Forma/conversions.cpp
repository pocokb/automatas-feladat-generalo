#include "stdafx.h"
#include "conversions.h"
#include<iostream>
#include<stack>
#include <algorithm>
#include "NFA.h"

Language DFAToRegLang(DFA m)
{
	Language lang;
	set<char> terminals;
	set<char> nonterminals;
	for (DFA::Transition t : m.transitions) {
		string rightHand = (t.on == ' ' ? string() : string{ t.on }) + (char)('A' + t.to);
		Language::Rule rule{ string{ (char)('A' + t.from)}, rightHand };
		lang.rules.push_back(rule);
		terminals.insert(t.on);
		nonterminals.insert((char)('A' + t.from));
		nonterminals.insert((char)('A' + t.to));
	}
	for (char c : terminals) {
		lang.terminating.append({ c });
	}
	for (char c : nonterminals) {
		lang.nonTerminating.append({ c });
	}
	for (int st : m.goalStates) {
		Language::Rule rule{ string{ (char)('A' + st) }, "" };
		lang.rules.push_back(rule);
	}
	lang.start = "A";
	return lang;
}

DFA RegLangToDFA(Language rl) {
	DFA dfa;
	set<char> states;
	map<char, int> nonTermToState;
	//nonTermToState[rl.start[0]] = 0;
	for (size_t i = 0; i < rl.nonTerminating.size(); i++) {
		char c = rl.nonTerminating[i];
		//if()
		nonTermToState[c] = i;
	}
	for (Language::Rule r : rl.rules) {
		if (r.right == "") { //végállapot
			dfa.goalStates.insert(nonTermToState[r.left[0]]);
		}else { //átmenet
			dfa.transitions.insert(DFA::Transition{ nonTermToState[r.left[0]], nonTermToState[r.right[1]], r.right[0] });
		}
	}
	dfa.startState = nonTermToState[rl.start[0]];
	dfa.nStates = nonTermToState.size();

	return dfa;
}

DFA foldDFAonDFA(DFA frame, DFA entity){	
	//az egyszerûség kedvéért módosítjuk entity-t ugy hogy biztosan legyen egyetlen végállapota
	NFA mEntity(entity);
	mEntity.nStates++;
	for (int st : entity.goalStates) {
		mEntity.transitions.insert({ st,mEntity.nStates - 1,' ' });
	}
	mEntity.goalStates.insert(mEntity.nStates - 1);
	
	//lecopyzzük frame.nstates -szer az mEntity-t
	int totalStates = 0;
	set<DFA::Transition> newTransitions;
	for (int i = 0; i < frame.nStates; i++) { 
		for (DFA::Transition t : mEntity.transitions) {
			t.from += totalStates;
			t.to += totalStates;
			newTransitions.insert(t);
		}
		totalStates += mEntity.nStates;
	}

	//összekötögetjük a frame-el
	size_t i = 0;
	for (DFA::Transition t : frame.transitions) {
		DFA::Transition t1 = {t.from + totalStates, i*mEntity.nStates, ' '}; //igazából tökmind1, hogy melyik csak menjen sorban!
		DFA::Transition t2 = { (i+1)*mEntity.nStates - 1, t.to + totalStates, ' ' };
		newTransitions.insert({ t1,t2 });
		i++;
	}
	set<int> newGoalStates;
	for (int st : frame.goalStates) {
		newGoalStates.insert(st + totalStates);
	}	
	
	totalStates += frame.nStates;

	NFA ret;
	ret.nStates = totalStates;
	ret.transitions = newTransitions;
	ret.startState = 0;
	ret.goalStates = newGoalStates;
	
	return ret.toDFA();
}

struct tmpCut {
	string newStr, remainder;
};

tmpCut cutOnSecondNonTerm(string str) {	
	int pos = 0;
	bool firstPassed = false;
	tmpCut ret{str,""};
	for (char c : str) {
		if (isupper(c)) {
			if (firstPassed) {				
				ret.newStr = str.substr(0, pos + 1);
				ret.remainder = str.substr(pos, str.size() - (pos + 1));
				break;
			}else {
				firstPassed = true;
			}
		}
		pos++;
	}
	return ret;
}
/*
Language collapeseCfToReg(Language cfl) {
	stack<char> verem;
	map<char, set<string>> rulemap;
	for (Language::Rule r : cfl.rules) {
		rulemap[r.left[0]].insert(r.right);
	}
	verem.push(cfl.start[0]);
	while (1) {
		char cur = verem.top;
		for (string str : rulemap[cur]) {			
			
		}
	}

}*/