#include "stdafx.h"
#include "DFABuilder.h"
#include "NFA.h"
#include<iostream>
#include"conversions.h"
#include"RandGen.h"

using namespace std;

DFABuilder::DFABuilder()
{
}


DFABuilder::~DFABuilder()
{
}

namespace dfabuilder {	
	vector<char> abc = { '0','1'};
	DFA specific_word(string word) { //itt most van function overload, ha nincs fia akkor õ terminál		
		int i = 0;
		int trap = word.size() + 1;
		DFA dfa;
		for (char c : word) {
			dfa.transitions.insert({ i,i + 1,c });
			for (char d : abc) {
				if (d != c) {
					dfa.transitions.insert({ i,trap,d });
				}								
			}			
			i++;
		}
		for (char c : abc) {
			dfa.transitions.insert({ i,trap,c });
			dfa.transitions.insert({ trap,trap,c });
		}		
		dfa.goalStates.insert(i);
		dfa.nStates = i + 2;
		dfa.startState = 0;

		//cout << endl << "word: " << word << endl;
		//dfa.print();
		return dfa;
	}
	DFA allFollowedBy(char letter1, char letter2){
		DFA dfa;
		set<char> abcSet(abc.begin(), abc.end());
		int trap = 2;
		//kezdõ : 0
		dfa.transitions.insert({ 0,1,letter1 });
		set<char> rest0(abcSet);
		rest0.erase(letter1);
		for (char c : rest0) {
			dfa.transitions.insert({ 0,0,c });
		}

		//masodik : 1
		int target = (letter1 == letter2 ? 1 : 0); //has saját maga akkor saját magába megy
		dfa.transitions.insert({ 1,target,letter2 });

		set<char> rest1(abcSet);
		rest1.erase(letter2);
		for (char c : rest1) {
			dfa.transitions.insert({ 1,trap,c });
		}

		//trap : 2
		for (char c : abcSet) {
			dfa.transitions.insert({ trap,trap,c });
		}
		dfa.goalStates.insert(0);
		dfa.nStates = 3;
		dfa.startState = 0;
		return dfa;
	}
	DFA contains(DFA sym_word) { //konstrukció (paraméterekbõl)	
		NFA word = NFA(sym_word);
		
		auto putsigmacsillag = [](NFA &dfa, int q) {
			for (char c : abc) {
				dfa.transitions.insert({ q,q,c });
			}
		};
		putsigmacsillag(word, 0);
		for (int q : word.goalStates) {
			putsigmacsillag(word, q);
		}
		word = word.determinize().minimize();
		return word;
	}
	DFA startsWith(DFA sym_word) {
		NFA word = NFA(sym_word);		
		auto putsigmacsillag = [](NFA &dfa, int q) {
			for (char c : abc) {
				dfa.transitions.insert({ q,q,c });
			}
		};		
		for (int q : word.goalStates) {
			putsigmacsillag(word, q);
		}
		word = word.determinize().minimize();
		return word;
	}
	DFA endsWith(DFA sym_word) {
		NFA word = NFA(sym_word);		
		auto putsigmacsillag = [](NFA &dfa, int q) {
			for (char c : abc) {
				dfa.transitions.insert({ q,q,c });
			}
		};
		putsigmacsillag(word, 0);		
		word = word.determinize().minimize();
		return word;
	}

	DFA containsNTimes(DFA nTimes, DFA sym_word) {										
		return reachedGoalNTimes(endsWith(sym_word), nTimes);;
	}
	
	DFA star(DFA dfa) {
		for (int st : dfa.goalStates) {
			dfa.transitions.insert({ st,dfa.startState,' ' });
		}
		dfa.goalStates.insert(dfa.startState);
		DFA ret = NFA(dfa).epsilonMent().determinize().minimize();		
		return ret;
	}
	
	DFA NConsecutive(DFA sym_word, DFA nmbr) {
		DFA wStar = star(sym_word);
		return DFA::and(reachedGoalNTimes(wStar,nmbr),wStar);
	}

	DFA and (DFA sw1, DFA sw2) {	
		return DFA::and(sw1, sw2);
	}
	DFA or (DFA sw1, DFA sw2) {
		return DFA::or(sw1, sw2);
	}
	DFA not (DFA sw) {
		return DFA::complement(sw);
	}

	DFA numberWithRelation(int n, string relation) {
		DFA ret;
		for (int i = 0; i < (n + 1); i++) {			
			ret.transitions.insert({ i,i + 1,'e' });
			ret.transitions.insert({ i,i,'z' });
		}
		ret.transitions.insert({ n + 1,n + 1,'e' });
		ret.transitions.insert({ n + 1,n + 1,'z' });
		ret.startState = 0;
		ret.nStates = n + 2;		
		if (relation == "exact") {
			ret.goalStates = { n };
		}else if (relation == "atLeast") {
			ret.goalStates = {n, n + 1 };
		}else if (relation == "atMost") {
			for (int i = 0; i <= n; i++) {
				ret.goalStates.insert(i);
			}
		}else {
			cerr << "Error in numberWithRelation" << endl;
		}
		
		return ret;
	}
	
	//e = elõre , h = hátra, z = marad
	DFA divisibleBy(int n) {
		DFA ret;		
		for (int i = 1; i < n; i++) {
			ret.transitions.insert({ i - 1, i, 'e' });
			ret.transitions.insert({ i, i - 1, 'h' });
			ret.transitions.insert({ i - 1, i - 1, 'z' });
		}
		ret.transitions.insert({ n - 1, 0, 'e' });
		ret.transitions.insert({ 0, n - 1, 'h' });
		ret.transitions.insert({ n - 1, n - 1, 'z' });
		ret.startState = 0;
		ret.nStates = n;
		ret.goalStates = { 0 };
		return ret;
	}

	DFA even() {
		return divisibleBy(2);
	}

	DFA odd() {
		return not(divisibleBy(2));
	}

	vector<int> counter() {
		RandGen rg;
		vector<int> ret(abc.size());
		ret[0] = 1; //az elsõ mindig +1, hogy jól nézzen ki
		if (abc.size() > 1) { //ha 1 elemû az abc (ugyse lesz 1 elemû), akkor ez safety arra az esetre
			size_t rIndex = rg.randInt(abc.size() - 1) + 1;
			int choices[] = { -1,1 };
			ret[rIndex] = choices[rg.randInt(2)]; //kell, hogy legyen még1, ami -1 vagy +1, hogy legyen értelme annak, hogy aritmetikai kifejezés
			for (size_t i = 1; i < abc.size(); i++) { //ezután a többibõl (ha van) még random választgatunk de itt már nem muszaj
				if (i != rIndex) { //nyilvan ahol már választottunk ott nem szabad
					ret[i] = rg.randInt(3) - 1;
				}
			}			
		}
		return ret;
	}
	
	DFA reachedGoalNTimes(DFA dfa, DFA nTimes) {		
		struct CombinedState {
			int dfa, nTimes;
			bool operator<(const CombinedState& other) const {
				if (dfa == other.dfa) {
					return nTimes < other.nTimes;
				}else {
					return dfa < other.dfa;
				}

			}
		};
		DFA ret;
		ret.startState = 0;

		map<CombinedState, int> combinedStateMap;
		auto dfaTransMap = dfa.getTransitionsAsMap();
		auto nTimesTransMap = nTimes.getTransitionsAsMap();
		
		int index = 0;
		vector<CombinedState> work;
		CombinedState start = { 0,0 };
		combinedStateMap[start] = index++;		
		work.push_back(start);

		while (!work.empty()) {
			CombinedState curState = work.back();
			work.pop_back();

			//átmenetek és rekurzió
			for (char c : abc) {				
				//a dfa-ban egyszerûen megnézzük, hogy az adott transition hova visz
				int dfaNextState = *dfaTransMap[{curState.dfa, c}].begin();
				//az nTimesban akkor lesz átmenet, ha a dfa-ban célállapot a következõ
				int nTimesNextState = (dfa.goalStates.count(dfaNextState) > 0 ? *nTimesTransMap[{curState.nTimes,'e'}].begin() : curState.nTimes);
				CombinedState nextCombinedState = { dfaNextState,nTimesNextState };
				int nextCombinedStateIndex;
				
				if (combinedStateMap.count(nextCombinedState) > 0) { //ha már voltunk ebben az állapotban
					nextCombinedStateIndex = combinedStateMap[nextCombinedState];
				}else { //ha még nem
					nextCombinedStateIndex = index++;
					combinedStateMap[nextCombinedState] = nextCombinedStateIndex;
					work.push_back(nextCombinedState);
				}
				//az átmenetek viszont mindenképp kellenek
				ret.transitions.insert({ combinedStateMap[curState], nextCombinedStateIndex, c});
			}			

			//ha a számlálóban célállapot akkor a teljesben is az lesz
			if (nTimes.goalStates.count(curState.nTimes) > 0) {
				ret.goalStates.insert(combinedStateMap[curState]);
			}
		}

		ret.nStates = combinedStateMap.size();
		return ret;
	}

	DFA counterEqualsNumber(vector<int> cntr, DFA nmbr ) {
		DFA ret;
		//csinál egy mapot a könnyebb ciklushoz
		map<char, vector<char>> cMap;
		cMap['e'] = vector<char>();
		cMap['h'] = vector<char>();
		cMap['z'] = vector<char>();
		for (size_t i = 0; i < cntr.size(); i++) {
			if (cntr[i] == 1) {
				cMap['e'].push_back(abc[i]);
			}else if(cntr[i] == -1){
				cMap['h'].push_back(abc[i]);
			}else { //0
				cMap['z'].push_back(abc[i]);
			}
		}
		set<DFA::Transition> newTransitions;
		for (const auto &t : nmbr.transitions) {
			for (char c : cMap[t.on]) {
				DFA::Transition newT(t);				
				newT.on = c;
				newTransitions.insert(newT);
			}
		}
		nmbr.transitions = newTransitions;
		ret = nmbr;
		return ret;
	}
	DFA occuranceEqualsNumber(char letter, DFA nmbr) {
		vector<int> tempCntr;
		for (char c : abc) {
			if (c == letter) {
				tempCntr.push_back(1);
			}else {
				tempCntr.push_back(0);
			}
		}
		
		return counterEqualsNumber(tempCntr, nmbr);
	}
	DFA everyNPropertyNonDet(int n, DFA check) {
		DFA ret;
		vector<string> work; //2 string equals? jo lesz?
		map<string,int> stateMap;
		work.push_back("");
		stateMap[""] = 0;
		ret.startState = 0;		
		ret.goalStates.insert(0);
		ret.nStates = 1;
		int index = 1;
		
		while (!work.empty()) {
			string cur = work.back();
			work.pop_back();
			int curIndex = stateMap[cur];
			
			
			//ha még nincs n hosszú (tehát az n hosszú résszó még nem létezik), vagy ha a feltétel teljesül
			//int noc = std::count(cur.begin(), cur.end(), 'a');
			if (cur.length() < n || check.accepts(cur)) {//ha célállapot
				ret.goalStates.insert(curIndex);
				for (char c : abc) {
					string tmp = (cur + c);
					string target = tmp.substr((tmp.length() > n ? tmp.length() - n : 0), string::npos);
					int targetIndex;
					if (stateMap.count(target) > 0) {
						targetIndex = stateMap[target];
					}
					else {
						targetIndex = index;
						index++;
						stateMap[target] = targetIndex;
						work.push_back(target);
					}
					ret.transitions.insert(DFA::Transition{ curIndex,targetIndex,c });
				}
			}else { //ha nem célállapot akkor csapda! (ez a típus természetébõl fakad... hiszen bármely n-esre igaz kell hogy legyen, tehát ha egyszer nem volt az akkor csá)
				for (char c : abc) {
					ret.transitions.insert(DFA::Transition{ curIndex,curIndex,c });
				}
			}			
			ret.nStates++;
			//az állapot immáron felépült			
		}
		return ret;
	}
}

namespace interpreter{ //policy: minden forwardot a választók végzik, a konstrukció már a paramétereiket kapják szóval pl continsNtimes már NEM K-t kap, hanem NS-t!!
	int number(SynthTreeNode *node) {
		int a = 0;
		while (node->elsofiu != nullptr) {
			node = node->elsofiu->testver;
			a++;
		}
		return a;
	}
	/*DFA exact_number(SynthTreeNode *node) {
		int n = number(node);		
		return dfabuilder::exact_number(n);
	}*/
	vector<int> counter(SynthTreeNode *node) {
		vector<int> ret;
		ret.push_back(1); 
		node = node->testver; //az elsõ csak simán abc[0]
		while (node != nullptr) {			
			switch(node->elsofiu->symbol){
			case 'e':ret.push_back(1); break;
			case 'h':ret.push_back(-1); break;
			case 'z':ret.push_back(0); break;
			default: cout << "error symbol: " << node->elsofiu->symbol << endl; break;
			}
			node = node->testver;
		}
		return ret;
	}
	DFA exact_number(SynthTreeNode *node) {
		int n = number(node);
		return dfabuilder::numberWithRelation(n, "exact");
	}
	DFA atLeast(SynthTreeNode *node) {
		int n = number(node);
		return dfabuilder::numberWithRelation(n, "atLeast");
	}
	DFA atMost(SynthTreeNode *node) {
		int n = number(node);
		return dfabuilder::numberWithRelation(n, "atMost");
	}
	DFA even_number(SynthTreeNode *node) {		
		return dfabuilder::even();
	}
	DFA odd_number(SynthTreeNode *node) {
		return dfabuilder::odd();
	}
	DFA divisibleBy_number(SynthTreeNode *node) {
		int n = number(node) + 1; //hogy 2-rõl induljon (hack)
		return dfabuilder::divisibleBy(n);
	}
	DFA specific_word(SynthTreeNode *node) { //beépített stringbuilderes konstrukció
		string word;
		word.append({ node->symbol });
		node = node->testver; //2.paraméterre lép
		while (node->elsofiu != nullptr) {
			node = node->elsofiu;
			word.append({ node->symbol });
			node = node->testver;
		}
		
		return dfabuilder::specific_word(word);
	}
	DFA contains(SynthTreeNode *node) {
		DFA sym_word = specific_word(node->elsofiu); //most ez a hack, hogy jó legyen
		//DFA sym_word = symbolic_word(node);
		return dfabuilder::contains(sym_word);
	}	
	DFA startsWith(SynthTreeNode *node) {
		DFA sym_word = specific_word(node->elsofiu); //most ez a hack, hogy jó legyen	
		return dfabuilder::startsWith(sym_word);
	}
	DFA endsWith(SynthTreeNode *node) {
		DFA sym_word = specific_word(node->elsofiu); //most ez a hack, hogy jó legyen	
		return dfabuilder::endsWith(sym_word);
	}
	

	DFA containsNTimes(SynthTreeNode *node) { //többparaméteres konstrukció		
		DFA nTimes = symbolic_number(node);
		DFA sym_word = specific_word(node->testver->elsofiu); //most ez a hack, hogy jó legyen
															  //DFA sym_word = symbolic_word(node->testver);
		return dfabuilder::containsNTimes(nTimes, sym_word);
	}
	DFA NConsecutive(SynthTreeNode *node) { //többparaméteres konstrukció		
		DFA nTimes = symbolic_number(node);
		DFA sym_word = specific_word(node->testver->elsofiu); //most ez a hack, hogy jó legyen
															  //DFA sym_word = symbolic_word(node->testver);
		return dfabuilder::NConsecutive(sym_word, nTimes);
	}
	DFA counterEqualsNumber(SynthTreeNode *node) { //többparaméteres konstrukció		
		vector<int> cntr = counter(node->elsofiu);
		DFA nmbr = symbolic_number_finite(node->testver);
		return dfabuilder::counterEqualsNumber(cntr, nmbr);
	}
	DFA and(SynthTreeNode *node) {
		DFA sw1 = symbolic_word(node);
		DFA sw2 = (	node->testver->symbol == 'E' 
					? and(node->testver->elsofiu)
					: symbolic_word(node->testver));		
		
		return dfabuilder::and(sw1,sw2);
	}
	DFA or (SynthTreeNode *node) {
		DFA sw1 = symbolic_word(node);		
		DFA sw2 = (	node->testver->symbol == 'O'
					? or (node->testver->elsofiu)
					: symbolic_word(node->testver));
		
		return dfabuilder::or(sw1, sw2);
	}
	DFA not(SynthTreeNode *node) {
		DFA sw = symbolic_word(node);		
		return dfabuilder::not(sw);
	}
	DFA allFollowedBy(SynthTreeNode *node) {
		DFA ret = dfabuilder::allFollowedBy(node->elsofiu->symbol, node->testver->elsofiu->symbol);
		return ret;
	}
	
	DFA everyNPropertyNonDet(SynthTreeNode *node) { //többparaméteres konstrukció		
		int everyN = number(node);
		DFA NTimes = symbolic_number(node->testver);
		DFA letter = dfabuilder::specific_word(string(1,node->testver->testver->elsofiu->symbol)); 
		DFA letterNTimes = dfabuilder::containsNTimes(NTimes, letter);
		return dfabuilder::everyNPropertyNonDet(everyN, letterNTimes);
	}
	
	DFA symbolic_word(SynthTreeNode *node) { //választó 	
		//minden forwardot a választok végzik, tehát ez pl 'W'-t fog kapni, ezért még forwardolnia kell egyet, majd mégegyet		
		node = node->elsofiu;	
		DFA ret;
		switch(node->symbol){
		case 'C':
			ret = contains(node->elsofiu); break;
		case 'K':
			ret = containsNTimes(node->elsofiu); break;
		case 'S':
			ret = specific_word(node->elsofiu); break;
		case 'E':
			ret = and(node->elsofiu); break;
		case 'O':
			ret = or(node->elsofiu); break;
		case 'T':
			ret = not(node); break;
		case 'Z':
			ret = startsWith(node->elsofiu); break;
		case 'V':
			ret = endsWith(node->elsofiu); break;
		case 'L':
			ret = counterEqualsNumber(node->elsofiu); break;
		case 'U':
			ret = allFollowedBy(node->elsofiu); break;
		case 'F':
			ret = NConsecutive(node->elsofiu); break;
		case 'D':
			ret = everyNPropertyNonDet(node->elsofiu); break;
			
		default:
			cout << "Nincs szabaly megadva a kovetkezore: " << node->symbol << endl;
			return DFA();
		}
		ret.feleselhagy();
		return ret;
	}

	DFA symbolic_number_finite(SynthTreeNode *node) { //választó 	
											   //minden forwardot a választok végzik, tehát ez pl 'W'-t fog kapni, ezért még forwardolnia kell egyet, majd mégegyet		
		node = node->elsofiu;
		DFA ret;
		switch (node->symbol) {
		case 'H':
			ret = divisibleBy_number(node->elsofiu); break;
		case 'X':
			ret = even_number(node->elsofiu); break;
		case '|':
			ret = odd_number(node->elsofiu); break;		
		default:
			cout << "Nincs szabaly megadva a kovetkezore: " << node->symbol << endl;
			return DFA();
		}
		return ret.minimize();
	}
	DFA symbolic_number(SynthTreeNode *node) { //választó 	
											   //minden forwardot a választok végzik, tehát ez pl 'W'-t fog kapni, ezért még forwardolnia kell egyet, majd mégegyet		
		node = node->elsofiu;
		DFA ret;
		switch (node->symbol) {
		case 'Q':			
			ret = exact_number(node->elsofiu); break;
		case 'H':
			ret = divisibleBy_number(node->elsofiu); break;
		case 'X':
			ret = even_number(node->elsofiu); break;
		case '|':
			ret = odd_number(node->elsofiu); break;		
		case 'R':
			ret = atMost(node->elsofiu); break;
		case '$':
			ret = atLeast(node->elsofiu); break;			
		default:
			cout << "Nincs szabaly megadva a kovetkezore: " << node->symbol << endl;
			return DFA();
		}		
		return ret.minimize();
	}
}

DFA buildDFA(SynthTreeNode * node){
	if (node->elsofiu->symbol == 'W') {
		node = node->elsofiu; //betekeri ha csak W
	}
	return interpreter::symbolic_word(node);
}

/*
DFA reachedGoalNTimes(DFA dfa, DFA nTimes) {
	//goalstatebe menõ átmenetek->g+betû
	dfa.print();
	vector<DFA::Transition> goalTrans;
	set<char> goalChars;
	vector<char> GCB;
	set<DFA::Transition> newTrans;
	for (DFA::Transition t : dfa.transitions) {
		//ha célállapotba megy
		if (dfa.goalStates.count(t.to) > 0) {
			goalChars.insert(t.on);
			GCB.push_back(t.on);
			t.on = goalTrans.size(); //most jo //hekk
			goalTrans.push_back(t);

			//cout << ".." << t.on << endl;
			newTrans.insert(t);
		}
		else {
			newTrans.insert(t);
		}
	}
	dfa.transitions = newTrans;
	dfa.print();

	//e={ez a halmaz}
	//z=Sigma*
	nTimes.print();
	set<DFA::Transition> NNewTrans;
	for (DFA::Transition t : nTimes.transitions) {
		if (t.on == 'e') {
			for (size_t i = 0; i < goalTrans.size(); i++) { //nagy hekk ez
				t.on = i;
				NNewTrans.insert(t);
			}
		}
		else if (t.on == 'z') {
			for (char c : abc) {
				if (goalChars.count(c) == 0) {
					t.on = c;
					NNewTrans.insert(t);
				}
			}
		}
	}
	nTimes.transitions = NNewTrans;
	nTimes.print();
	//and
	DFA result = DFA:: and (dfa, nTimes);
	result.minimize();
	//visszaalakítás
	set<DFA::Transition> resultNewTrans;
	for (DFA::Transition t : result.transitions) {
		if (t.on >= 0 && t.on < goalTrans.size()) { //micsoda hekk

													//t.on = GCB[t.on];
		}
		resultNewTrans.insert(t);
	}
	result.transitions = resultNewTrans;
	return result;
}*/