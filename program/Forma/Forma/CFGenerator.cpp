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

/* Ez a r�gi genRules, indirekelte gener�l�sban bels� szab�lyokat bev�ve m�dositani kell egy kicsit!
Language genRules( //az a baj, hogy most a megnevez�si szok�sokat tartani kell minden�tt ez nem az igazi
	"01I",
	"WCSANKE", //ide mindig be kell irni k�l�nben termin�l r� �s nem lesz els�fia �s lefagy
	"W", // ez furcsa mert a W-nek igaz�b�l csak a tipusossag miatt van �rtelme, egyik ford�t�sban sincs szerepe
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
Language genRulesEtalon( //az a baj, hogy most a megnevez�si szok�sokat tartani kell minden�tt ez nem az igazi
	"01I",
	"WCSANKEM", //ide mindig be kell irni k�l�nben termin�l r� �s nem lesz els�fia �s lefagy
	"�", // ez furcsa mert a W-nek igaz�b�l csak a tipusossag miatt van �rtelme, egyik ford�t�sban sincs szerepe
	{
		"�->W",		
		"�->O",
		"�->E",		
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
		"Y->Q", //Y = szimbolikus sz�m , Q = pontosan N, R = legfeljebb N, X = p�ros sz�mu
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
		"&->e", //seg�dek P-hez, +1
		"&->h", // -1
		"&->z", // 0
		//"P->" : sz�ml�l� minden bet�re DINAMIKUSAN BET�LTVE
		"U->BB",
		//"B->" : bet�k DINAMIKUSAN BET�LTVE
		//"S->0A", : fix sz� DINAMIKUSAN
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

	Language genRules(genRulesEtalon); //m�solat k�sz�l, mert dinamikusan is lesz load!
	//dinamikusan bet�lt�nk p�r dolgot
	vector<char> abc = { '0','1'};
	//P bet�lt�se
	string Phez = "e";
	for (size_t i = 1; i < abc.size(); i++) {
		Phez += "&";
	}
	genRules.rules.push_back(Language::Rule{ string("P"), Phez });
	
	for (char c: abc) {
		//B bet�lt�se
		genRules.rules.push_back(Language::Rule{ string("B"), string() + c });
		//S,A bet�lt�se
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
		if (dfa.nStates < 2) { //ha trivi�lis eset pl �res sz�, vagy simga*, akkor ink�bb adunk ujat
			nOfTasks++;
			continue;
		}
		cout << "Feladat: ";
		string sentence = sentence::buildSentence(r.synthTree->elsofiu); //teh�t m�r NEM a �-r�l indul hanem az alatta lev�r�l!!		
		cout << sentence << endl;
		
		cout << "Felismert szavak p�ld�ul:" << endl;
		Language lang = DFAToRegLang(dfa);
		lang.cfGenerateWords(8);
		lang.printWordsMax(50);
		
		cout << "Egy megold�s:" << endl;		
		dfa.print();				
		cout << endl;
	}
	
	if(!my_DEBUG) std::cout.rdbuf(coutbuf);
	out.close();
	return vector<Result>();
}
