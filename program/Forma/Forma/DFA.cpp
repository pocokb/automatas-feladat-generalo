#include "stdafx.h"
#include "DFA.h"
#include "NFA.h"
#include <algorithm>
#include <iterator>
#include <iostream>
#include "Language.h"
#include "conversions.h"
using namespace std;

DFA::DFA(){
}

DFA::DFA(int startState, int nStates, initializer_list<Transition> transitions, initializer_list<int> goalStates)
	: startState(startState), nStates(nStates), transitions(transitions), goalStates(goalStates){
}
DFA::DFA(int startState, int nStates, set<Transition> transitions, set<int> goalStates)
	: startState(startState), nStates(nStates), transitions(transitions), goalStates(goalStates) {
}


DFA::~DFA(){
}



DFA DFA::minimize(){
	//elérhetetlen állapotok elhagyása	
	/*Language lang = DFAToRegLang(*this);
	lang.felesElhagy();
	*this = RegLangToDFA(lang);*/
	this->feleselhagy();
	//kongrugens halmazok keresése
	set<char> abc = getABC(); //ezek a helperek ebbe a formába kellenek
	auto transMap = this->getTransitionsAsMap();
	vector<int> qToH(nStates); //inicializális végállapotsággal
	for (int i = 0; i < nStates; i++) {
		qToH[i] = (goalStates.count(i) == 1 ? 1 : 0);
	}
	
	map<vector<int>, int> kindToNewH;
	do {
		//szétvágjuk ha bármely elem különbözõ halamzba megy mint a másik
		kindToNewH.clear();
		size_t index = 0;
		vector<int> qToHUj;
		for (int q = 0; q < nStates; q++) {
			
			vector<int> kind = { qToH[q] };
			for (char c : abc) {
						
				kind.push_back(qToH[*(transMap[{q, c}].begin())]);	//itt a debugger hibát dob, ha van olyan abc betû amire nincs meghatározva a kimenõ él(ez ugye tilos lenne dfa-ban)
				
			}
			if (kindToNewH.count(kind) != 1) { //ha még nem létezik ez az uj fajta akkor létrehozzuk
				
				kindToNewH[kind] = index;
				
				index++;
			} //különben nem kell semmi
			qToHUj.push_back(kindToNewH[kind]);
		}
		//frissitése inputnak
		if (qToH == qToHUj)
			break;
		else
			qToH = qToHUj;
		
	} while (1);
	
	
	//uj DFA létrehozása az eredményekbõl
	transitions.clear();
	for (auto const& elem : kindToNewH) {
		vector<int> kind = elem.first;
		size_t i = 0;
		for (char c : abc) {
			transitions.insert({ kind[0],kind[1 + i], c });
			i++;
		}
	}
	
	//célállapotok:
	set<int> goalStatesNew;
	for (int q = 0; q < nStates; q++) {
		if (goalStates.count(q) == 1) {
			goalStatesNew.insert(qToH[q]);
		}
	}
	goalStates = goalStatesNew;
	nStates = kindToNewH.size();
	return *this;
}

DFA DFA::nothing(){
	return DFA(0, 2, { 
		{0,1,'0'},
		{0,1,'1'},
		{1,1,'0'},
		{1,1,'1'}
	}, {0});
}

DFA DFA::complement(DFA m) {
	m.feleselhagy();
	DFA newM(m);
	set<int> states;
	//cout << "Komplementer elotti dfa" << endl;
	//newM.print();
	for (int i = 0; i < m.nStates; i++) {
		states.insert(i);
	}
	newM.goalStates.clear();
	set_difference(states.begin(), states.end(), m.goalStates.begin(), m.goalStates.end(),
		inserter(newM.goalStates, newM.goalStates.end()));
	//cout << "Komplementer utani dfa" << endl;
	//newM.print();
	//cout << newM.minimize().goalStates.size() << endl;
	return newM;
}

DFA DFA:: and (DFA m1, DFA m2){ //bug: nStates és legnagyobb q között problema, gyors fix: minimize() a végén!
	DFA dfa;	
	m1.feleselhagy();
	//cout << "m1 feleselhagy" << endl;
	//m1.print();
	m2.feleselhagy();
	//cout << "m2 feleselhagy" << endl;
	//m2.print();
	
	map<char, int> terminalIndexes;
	int i = 0;
	for (char c : m1.getABC()) {
		terminalIndexes[c] = i;
		i++;
	}	
	auto transVec = [&terminalIndexes](DFA dfa) {
		vector<vector<int>> ret(dfa.nStates, vector<int>(terminalIndexes.size()));		
		for (auto t : dfa.transitions) {			
			ret[t.from][terminalIndexes[t.on]] = t.to;
		}
		
		return ret;
	};
	
	auto tv1 = transVec(m1);
	auto tv2 = transVec(m2);

	dfa.nStates = m1.nStates * m2.nStates;
	
	//descartes
	for (int i = 0; i < m1.nStates; i++) {
		for (int j = 0; j < m2.nStates; j++) {
			for (char c : m1.getABC()) {
				dfa.transitions.insert(
					Transition{ i*m2.nStates + j,
								tv1[i][terminalIndexes[c]] * m2.nStates + tv2[j][terminalIndexes[c]],
								c});
			}
			//az és-elést a végállapotok határozzák meg
			if (m1.goalStates.count(i) == 1 && m2.goalStates.count(j) == 1) {
				dfa.goalStates.insert(i*m2.nStates + j);
			}
		}
	}
	
	//kezdõállapot:
	dfa.startState = m1.startState * m1.nStates + m2.startState;
	//dfa.minimize();
	//cout << "vege feleselhagy elott" << endl;
	//dfa.print();
	dfa.feleselhagy();
	return dfa;
}

DFA DFA:: or (DFA m1, DFA m2){	
	DFA tmp = and (complement(m1), complement(m2));	
	tmp = complement(tmp);	
	return tmp;
}

DFA DFA::append(DFA m1, DFA m2){
	NFA ret;	
	for (auto& t : m2.transitions) {		
		ret.transitions.insert({ t.from + m1.nStates, t.to + m1.nStates, t.on });
	}
	for (auto& q : m1.goalStates) {
		ret.transitions.insert({q,m2.startState + m1.nStates, ' '});
	}
	ret.transitions.insert(m1.transitions.begin(), m1.transitions.end());

	ret.nStates = m1.nStates + m2.nStates;
	ret.startState = m1.startState;
	for (auto& q : m2.goalStates) {
		ret.goalStates.insert(q + m1.nStates);
	}
	return ret.toDFA();
}


bool DFA::accepts(string str){
	int activeState = startState;
	map<hCondition, set<int>> transMap = this->getTransitionsAsMap();
	for (char c : str) {
		activeState = *(transMap[hCondition{ activeState,c }].begin()); //állapot elõre megy
	}
	if (goalStates.count(activeState) > 0) { //ha célállapot
		return true;
	}else {
		return false;
	}
	
}

set<char> DFA::getABC(){
	set<char> ret;
	for (auto t : transitions) {
		ret.insert(t.on);
	}
	return ret;
}

void DFA::print() {
	cout << "Number of states = " << nStates << endl;
	cout << "Start state = q" << startState << endl;
	cout << "State transitions:" << endl;	
	for (Transition t : transitions) {		
		cout << "q" << t.from << " --(" << t.on << ")--> q" << t.to << endl;
	}
	cout << "Goal states: " << endl;
	bool first = true;
	for (int st : goalStates) {
		if (!first) cout << ',';
		else first = false;
		cout << 'q' << st;
	}
	cout << endl;
}

map<DFA::hCondition, set<int>> DFA::getTransitionsAsMap(){
	map<hCondition, set<int>> ret;
	for (DFA::Transition t : this->transitions) {		
		ret[hCondition{ t.from, t.on }].insert(t.to);
	}
	return ret;
}

void DFA::feleselhagy() {	
	vector<int> work; //2 string equals? jo lesz?	
	work.push_back(this->startState);	
	set<int> found;
	found.insert(this->startState);
	int index = 0;
	map<int, int> oldToNew;
	

	auto transMap = this->getTransitionsAsMap();
		
	while (!work.empty()) {
		int curState = work.back();		
		work.pop_back();
		oldToNew[curState] = index++;

		for (char c : this->getABC()) {
			int to = *(transMap[{curState, c}].begin());			
			if (found.count(to) == 0) { //ha még nem láttuk, megjelöljük és betesszük workbe
				work.push_back(to);
				found.insert(to);

			}			
		}
	}
	//itt már csak azok vannak foundba akiket elértünk a kezdõbõl
	//ezért azoka transitionöket tartjuk meg
	set<DFA::Transition> newTrans;
	for (auto t : this->transitions) {
		if (found.count(t.from) > 0) {
			newTrans.insert({oldToNew[t.from],oldToNew[t.to],t.on});
		}
	}
	this->transitions = newTrans;
	this->nStates = found.size();

	set<int> newGoals;
	for (int st : this->goalStates) {
		if (found.count(st) > 0) {
			newGoals.insert(oldToNew[st]);
		}
	}
	this->goalStates = newGoals;	
	//cout << "before" << endl;
	//this->print();
}

bool DFA::equals(DFA other) {
	
	
	
	DFA first = DFA::and(*this, DFA::complement(other));
	first.feleselhagy();
	
	DFA second = DFA:: and (DFA::complement(*this), other);
	second.feleselhagy();
	
	return (first.goalStates.size() == 0) && (second.goalStates.size() == 0);
}

/*DFA DFA::sigmaStar(set<char> abc) {

}*/