#include"stdafx.h"
#include"CFGenerator.h"
#include"DFABuilder.h"
#include"SentenceBuilder.h"
#include"conversions.h"
#include<stack>
#include<utility>
#include<iostream>
#include <fstream>

using namespace std;

/* Ez a régi genRules, indirekelte generálásban belsõ szabályokat bevíve módositani kell egy kicsit!
Language genRules( //az a baj, hogy most a megnevezési szokásokat tartani kell mindenütt ez nem az igazi
	"01I",
	"WCSANKE", //ide mindig be kell irni különben terminál rá és nem lesz elsõfia és lefagy
	"W", // ez furcsa mert a W-nek igazából csak a tipusossag miatt van értelme, egyik fordításban sincs szerepe
	{		
		"W->S",
		"W->E",
		//"W->C",
		"W->K",
		"S->0A",
		"S->1A",
		"A->0A",
		"A->1A",		
		"A->",
		"N->IN",
		"N->",
		"C->W",
		"K->NW",
		"E->WW"
	}
);
*/
Language genRulesEtalon( //az a baj, hogy most a megnevezési szokásokat tartani kell mindenütt ez nem az igazi
	"01I",
	"WCSANKEM", //ide mindig be kell irni különben terminál rá és nem lesz elsõfia és lefagy
	"÷", // ez furcsa mert a W-nek igazából csak a tipusossag miatt van értelme, egyik fordításban sincs szerepe
	{
		"÷->W",		
		"÷->O",
		"÷->E",		
		"W->C",
		"W->K",
		"W->T",
		"W->Z",
		"W->V",
		"W->L",
		"W->U",
		"W->F",		
		"W->D",
		//		
		"T->C",
		"T->K",		
		"T->Z",
		"T->V",
		"T->L",
		"T->U",
		"T->F",
		"T->D",
		//
		"N->IM",
		"M->IM",
		"M->",
		"C->S",
		"K->YS",
		"F->YS",
		"D->NYB",
		"E->WW",
		"E->WE",
		"O->WW",
		"O->WO",		
		"Z->S",
		"V->S",
		"Y->Q", //Y = szimbolikus szám , Q = pontosan N, R = legfeljebb N, X = páros számu
		"Y->R",
		"Y->$",
		"Y->X",
		"Y->|",
		"Y->H",
		"Q->N",
		"R->N",
		"$->N",
		"X->",
		"|->",		
		"H->N",
		"L->PJ",
		"J->X", //finite number
		"J->|",
		"J->H",
		"&->e", //segédek P-hez, +1
		"&->h", // -1
		"&->z", // 0
		//"P->" : számláló minden betûre DINAMIKUSAN BETÖLTVE
		"U->BB",
		//"B->" : betûk DINAMIKUSAN BETÖLTVE
		//"S->0A", : fix szó DINAMIKUSAN
		//"S->1A",
		//"A->0A",
		//"A->1A",
		"A->",
	}
);
vector<Result> CFGenerator(){
	bool my_DEBUG = false;
	std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf	
	std::ofstream out("out.txt");;
	if (!my_DEBUG) {		
		std::cout.rdbuf(out.rdbuf());
	}

	Language genRules(genRulesEtalon); //másolat készül, mert dinamikusan is lesz load!
	//dinamikusan betöltünk pár dolgot
	vector<char> abc = { '0','1'};
	//P betöltése
	string Phez = "e";
	for (size_t i = 1; i < abc.size(); i++) {
		Phez += "&";
	}
	genRules.rules.push_back(Language::Rule{ string("P"), Phez });
	
	for (char c: abc) {
		//B betöltése
		genRules.rules.push_back(Language::Rule{ string("B"), string() + c });
		//S,A betöltése
		genRules.rules.push_back(Language::Rule{ string("S"), c + string("A")});
		genRules.rules.push_back(Language::Rule{ string("A"), c + string("A") });
	}			
	if(my_DEBUG) genRules.printDescription();
	size_t nOfTasks = 10;
	while(nOfTasks--) {
		ResultWord r = genRules.cfRandomGen(8);
		if (my_DEBUG) {
			cout << r.word << endl;
			r.synthTree->print();
		}
		DFA dfa = buildDFA(r.synthTree);
		dfa.feleselhagy();
		dfa.minimize();
		if (dfa.nStates < 2) { //ha triviális eset pl üres szó, vagy simga*, akkor inkább adunk ujat
			nOfTasks++;
			continue;
		}
		cout << "Feladat: ";
		string sentence = sentence::buildSentence(r.synthTree->elsofiu); //tehát már NEM a ÷-rõl indul hanem az alatta levõrõl!!		
		cout << sentence << endl;
		
		cout << "Felismert szavak például:" << endl;
		Language lang = DFAToRegLang(dfa);
		lang.cfGenerateWords(8);
		lang.printWordsMax(50);
		
		cout << "Egy megoldás:" << endl;		
		dfa.print();				
		cout << endl;
	}
	
	if(!my_DEBUG) std::cout.rdbuf(coutbuf);
	out.close();
	return vector<Result>();
}
