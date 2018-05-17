#include "stdafx.h"
#include "Tests.h"
#include "DFA.h"
#include "NFA.h"
#include<iostream>
#include"conversions.h"
#include"CFGenerator.h"
#include"DFABuilder.h"

using namespace std;

void Tests::DFACounter() {
	
	cout << "Test counter" << endl;
	for (size_t i = 0; i < 5; i++) {
		auto arr = dfabuilder::counter();
		for (int a : arr) {
			cout << a << ",";
		}
		cout << endl;
	}
	cout << "Divisible by 3" << endl;	
	DFA div3 = dfabuilder::divisibleBy(3);
	div3.print();
	cout << "Even" << endl;
	DFA even = dfabuilder::even();
	even.print();
	cout << "Odd" << endl;
	DFA odd = dfabuilder::odd();
	odd.print();

	cout << "Test counter equals" << endl;
	cout << "Odd" << endl;
	auto arr = dfabuilder::counter();
	for (int a : arr) {
		cout << a << ",";
	}
	cout << endl;
	DFA oddTestCounter = dfabuilder::counterEqualsNumber(arr, even);
	oddTestCounter.print();
	Language lang = DFAToRegLang(oddTestCounter);
	lang.cfGenerateWords(8);
	lang.printWords();
	//KÉSZ
}
void Tests::DFAComplement(){
	DFA m1(0, 3, {
		{ 0,1,'a' },
		{ 0,2,'b' },
		{ 1,2,'a' },
		{ 1,0,'b' },
		{ 2,2,'a' },
		{ 2,2,'b' }
	}, { 0 });
	m1.print();
	cout << "--------" << endl;
	DFA::complement(m1).print();
}

void Tests::DFAToRegLang() {
	DFA m1(0, 3, {
		{ 0,1,'a' },
		{ 0,2,'b' },
		{ 1,2,'a' },
		{ 1,0,'b' },
		{ 2,2,'a' },
		{ 2,2,'b' }
	}, { 0 });
	m1.print();
	cout << "--------" << endl;
	//DFA::complement(m1).print();
	Language lang = DFAToRegLang(m1);
	lang.printDescription();
	lang.cfGenerateWords(10);
	lang.printWords();
}

void Tests::RegLangToDFA() {
	Language rl( //két egyes sosem szerepel egymás után benne
		"01",
		"ABT",
		"A",
		{
			"A->0A",
			"A->1B",
			"A->",
			"B->0A",
			"B->1T",
			"B->"
		}
	);
	rl.printDescription();
	rl.generateWords();
	rl.printWords();
	cout << "--------------" << endl;
	//
	DFA dfa = RegLangToDFA(rl);
	dfa.print();
}

void Tests::testRegConv() {
	Language rl( //két egyes sosem szerepel egymás után benne
		"01",
		"ABT",
		"A",
		{
			"A->0A",
			"A->1B",
			"A->",
			"B->0A",
			"B->1T",
			"B->"
		}
	);
	rl.printDescription();
	rl.generateWords();
	rl.printWords();
	cout << "--------------" << endl;
	
	DFA dfa = RegLangToDFA(rl);
	dfa.print();
	dfa = DFA::complement(dfa);
	cout << "--------------" << endl;
	dfa.print();
	cout << "--------------" << endl;

	Language lang = DFAToRegLang(dfa);
	lang.printDescription();
	lang.generateWords();
	lang.printWords();

}
void Tests::testMain() {
	/*
	Language lang(
	"lr",
	"AB",
	"A",
	{ "A->lBr",
	"A->rBl",
	"B->lBr",
	"B->rBl",
	"B->BB",
	"B->lr",
	"B->rl",
	"A->lr",
	"A->rl",
	}
	);
	Language L2 = Language::random(2, 3, 5, 1, 2, 2);
	L2.printDescription();
	L2.generateWords(5);
	*/
	/*
	Language koveti(
	"",
	"AB",
	"AB",
	{
	}
	);
	Language _01_any(
	"01",
	"A",
	"A",
	{
	"A->01A",
	"A->"
	}
	);
	Language _01_once(
	"01",
	"",
	"01",
	{
	}
	);
	Language _111_once_or_twice(
	"1",
	"A",
	"111A",
	{
	"A->111",
	"A->"
	}
	);
	Language paros_egymas_melletti(
	"",
	"A",
	"AA",
	{
	"A->AAA",
	"AA->"
	}
	);
	Language n_szer(
	"",
	"ABC",
	"AAAAB",
	{
	"AB->BC",
	}
	);
	Language contains( //követési tulajdonságokkal megadás
	"",
	"AB",
	"ABA",
	{

	}
	);
	Language anything(
	"01",
	"A",
	"A",
	{
	"A->1A",
	"A->0A",
	"A->",
	}
	);
	Language anything_but_01(
	"01",
	"ABC",
	"A",
	{
	"A->1A",
	"A->0B",
	"B->0B",
	"B->1C",// A TRAP a C nem terminál, ki is vehetnénk!
	"C->0C",
	"C->1C",
	"B->",
	"A->",
	}
	);
	//nemterminális -> másik S-e
	typedef Language::Combination Com;
	/*
	Language cl = Language::combine(
	koveti,
	{ &koveti, &_01_any, &_111_once_or_twice },
	{
	Com(koveti, 'B', _01_any),
	Com(koveti, 'A', _111_once_or_twice)
	}
	);*/
	/*
	Language cl = Language::combine(
	&koveti,
	{ &koveti, &_01_once, &_111_once_or_twice, &paros_egymas_melletti },
	{
	Com(koveti, 'B', paros_egymas_melletti),
	Com(koveti, 'A', _111_once_or_twice),
	Com(paros_egymas_melletti, 'A' , _01_once)
	}
	);


	Language cl = Language::combine(
	&paros_egymas_melletti,
	{ &paros_egymas_melletti, &contains, &anything_but_01, &_01_once },
	{
	Com(paros_egymas_melletti, 'A', contains),
	Com(contains, 'A', anything_but_01),
	Com(contains, 'B', _01_once),

	}
	);
	cl.printDescription();
	cl.generateWords(35);*/
	//TODO deduktív kombinator
	Language containsH( //követési tulajdonságokkal megadás
		"",
		"P",
		"szo, amiben van P",
		{

		}
	);
	Language contains( //követési tulajdonságokkal megadás
		"",
		"AP",
		"APA",
		{
			"A->0A",
			"A->1A",
			"A->0",
			"A->1"
		}
	);
	Language _111_once_or_twiceH(
		"",
		"",
		"111 egyszer vagy ketszer",
		{

		}
	);
	Language _111_once_or_twice(
		"1",
		"A",
		"111A",
		{
			"A->111",
			"A->"
		}
	);
	Language paros_egymas_mellettiH(
		"",
		"P",
		"paros szamu egymas melletti P",
		{

		}
	);
	Language paros_egymas_melletti(
		"",
		"P",
		"PP",
		{
			"P->PPP",
			"PP->"
		}
	);
	Language anythingH(
		"",
		"",
		"tetszoleges szo",
		{

		}
	);
	Language anything(
		"01",
		"A",
		"A",
		{
			"A->1A",
			"A->0A",
			"A->",
		}
	);
	Language somethingH(
		"",
		"P",
		"P",
		{}
	);
	Language something(
		"01",
		"P",
		"P",
		{}
	);
	Language endsWithH{
		"",
		"P",
		"P-ra vegzodik",
		{}
	};
	Language endsWithF{
		"01",
		"AP",
		"AP",
		{
			"A->1A",
			"A->0A",
			"A->",
		}
	};
	Language darabH{
		"",
		"PQ",
		"P darab Q",
		{}
	};
	Language darabF{
		"01",
		"PQI",
		"P",
		{
			"I->Q",
		}
	};
	Language szamNH{
		"",
		"PQ",
		"PQ",
		{

		}
	};
	Language szamNF{
		"",
		"PQ",
		"PQ",
		{
		}
	};
	Language szamIH{
		"I",
		"",
		"I",
		{
		}
	};
	Language szamIF{
		"",
		"I",
		"I",
		{
		}
	};
	Language nothingH{
		"",
		"",
		"",
		{
		}
	};
	Language nothingF{
		"",
		"",
		"",
		{
		}
	};
	Language _1H{
		"",
		"",
		"1-es",
		{
		}
	};
	Language _1F{
		"1",
		"",
		"1",
		{
		}
	};
	Language possibilities( //az a baj, hogy most a megnevezési szokásokat tartani kell mindenütt ez nem az igazi
		"whIEO",
		"WCPDN",
		"W", // ez furcsa mert a W-nek igazából csak a tipusossag miatt van értelme, egyik fordításban sincs szerepe
		{
			"C->W",
			//"W->h",
			"W->C",
			//"W->P",
			//"P->W",
			//"W->w",
			"N->IN",
			"N->IO",
			"W->D",
			"D->NW",
			"W->E"
		}

	);
	/////////
	//map<char, auto> meaningH2;
	//meaningH2.insert('C', []() {});
	/////////
	map<char, Language> meaningF;
	meaningF.insert(make_pair('W', something));
	meaningF.insert(make_pair('C', contains));
	meaningF.insert(make_pair('h', _111_once_or_twice));
	meaningF.insert(make_pair('P', paros_egymas_melletti));
	meaningF.insert(make_pair('w', anything));
	meaningF.insert(make_pair('N', szamNF));
	meaningF.insert(make_pair('I', szamIF));
	meaningF.insert(make_pair('D', darabF));
	meaningF.insert(make_pair('E', _1F));
	meaningF.insert(make_pair('O', nothingF));

	map<char, Language> meaningH;
	meaningH.insert(make_pair('W', somethingH));
	meaningH.insert(make_pair('C', containsH));
	meaningH.insert(make_pair('h', _111_once_or_twiceH));
	meaningH.insert(make_pair('P', paros_egymas_mellettiH));
	meaningH.insert(make_pair('w', anythingH));
	meaningH.insert(make_pair('N', szamNH));
	meaningH.insert(make_pair('I', szamIH));
	meaningH.insert(make_pair('D', darabH));
	meaningH.insert(make_pair('E', _1H));
	meaningH.insert(make_pair('O', nothingH));

	possibilities.printDescription();
	vector<ResultWord> results = possibilities.cfGenerateWords(6);
	for (ResultWord r : results) {
		cout << "///////////////" << endl;
		cout << r.word << endl;
		r.synthTree->print();
		//cout << "RESF START" << endl;
		Language resF = r.synthTree->mergeSemantics(meaningF);
		resF.printDescription();
		//cout << "RESH START" << endl;
		Language resH = r.synthTree->mergeSemantics(meaningH);

		resH.printDescription();
		resH.generateWords(30);
		resH.printWords(); //szar a merge, para van...
		cout << "///////////////" << endl;
	}
	//something.printWords();		

	//something.generateWords(6);
	//something.printWords();
	
}


void Tests::testLancMent() {
	Language test( //az a baj, hogy most a megnevezési szokásokat tartani kell mindenütt ez nem az igazi
		"whIEO",
		"WCPDN",
		"N", // ez furcsa mert a W-nek igazából csak a tipusossag miatt van értelme, egyik fordításban sincs szerepe
		{			
			"C->",
			"C->W",
			"W->h",
			"W->C",
			"W->P",
			"P->W",
			"W->w",
			"N->IN",
			"N->IO",
			"W->D",
			"D->NW",
			"W->E"
		}
	);
	cout << "Original:" << endl;
	test.printDescription();
	test.generateWords(7);
	test.printWords();
	test.lancMent();	
	cout << "Lancment:" << endl;
	test.printDescription();
	test.generateWords(7);
	test.printWords();
	
	test.felesElhagy();
	cout << "Feleselhagy:" << endl;
	test.printDescription();
	test.generateWords(7);
	test.printWords();
}

void Tests::testDFAAnd(){	
	/*
	DFA m1(0, 3, {
		{ 0,1,'a' },
		{ 0,2,'b' },
		{ 1,2,'a' },
		{ 1,0,'b' },
		{ 2,2,'a' },
		{ 2,2,'b' }
	}, { 0 });
	m1.print();
	cout << "--------" << endl;
	DFA and1 = DFA:: and (m1, m1);
	and1.print();
	cout << "--------" << endl;	
	Language lang = DFAToRegLang(and1);
	lang.felesElhagy();
	lang.printDescription();
	lang.cfGenerateWords(10);
	lang.printWords();
	cout << "--------" << endl;
	DFA vissza = RegLangToDFA(lang);
	vissza.print(); */

	DFA nlegalabb2(0, 4, {
		{ 0,0,'1' },
		{ 0,1,'0' },
		{ 1,3,'1' },
		{ 1,2,'0' },
		{ 2,0,'1' },
		{ 2,2,'0' },
		
		{ 3,3,'1' },
		{ 3,3,'0' },
	}, { 0,2 });

	DFA nemszerep110(0, 4, {
		{ 0,1,'1' },
		{ 0,0,'0' },
		{ 1,2,'1' },
		{ 1,0,'0' },
		{ 2,2,'1' },
		{ 2,3,'0' },

		{ 3,3,'1' },
		{ 3,3,'0' },
	}, { 0,1,2 });

	DFA eselt = DFA:: and (nlegalabb2, nemszerep110);
	eselt.print();
	Language lang = DFAToRegLang(eselt);
	lang.felesElhagy();
	lang.printDescription();
	lang.cfGenerateWords(8);
	lang.printWords();
	cout << "--------" << endl;
	Language lang2 = DFAToRegLang(DFA:: or (nlegalabb2, nemszerep110));
	lang2.felesElhagy();
	lang2.printDescription();
	lang2.cfGenerateWords(8);
	lang2.printWords();
}

void Tests::testNFAToDFA(){
	cout << "test1:" << endl;
	NFA tart101(0, 4, {
		{ 0,0,'1' },
		{ 0,0,'0' },
		{ 0,1,'1' },
		{ 1,2,'0' },		
		{ 2,3,'1' },
		{ 3,3,'0' },
		{ 3,3,'1' },		
	}, { 3 });
	tart101.print();
	DFA dfa = tart101.determinize();
	dfa.print();
	Language l = DFAToRegLang(dfa);
	l.cfGenerateWords(9);
	l.printWords();
	cout << "///////////////////////////" << endl;
	cout << "test2:" << endl;
}

void Tests::testMinimize(){
	NFA tart101(0, 4, {
		{ 0,0,'1' },
		{ 0,0,'0' },
		{ 0,1,'1' },
		{ 1,2,'0' },
		{ 2,3,'1' },
		{ 3,3,'0' },
		{ 3,3,'1' },
	}, { 3 });
	tart101.print();
	DFA dfa = tart101.determinize();
	dfa.print();
	dfa.minimize();
	dfa.print();
	cout << "///////////////" << endl << "TEST2" << endl;
	DFA proba(0, 8, {
		{ 0,1,'a' },
		{ 0,3,'b' },
		{ 1,0,'a' },
		{ 1,4,'b' },
		{ 2,3,'a' },
		{ 2,5,'b' },
		{ 3,4,'a' },
		{ 3,6,'b' },
		{ 4,3,'a' },
		{ 4,6,'b' },
		{ 5,5,'a' },
		{ 5,5,'b' },
		{ 6,7,'a' },
		{ 6,5,'b' },
		{ 7,7,'a' },
		{ 7,5,'b' },
	}, { 5,6,7 });
	proba.minimize();
	proba.print();
}

void Tests::testCFGen(){
	CFGenerator();
}

void Tests::testEpsilonMent(){
	NFA pelda(0, 6, {
		{ 0,1,'1' },
		{ 0,4,'0' },
		{ 1,2,'1' },
		{ 1,3,' ' },
		{ 2,3,' ' },
		{ 3,5,'0' },
		{ 4,5,'0' },
		{ 4,1,' ' },
		{ 4,2,' ' },
		{ 5,3,' ' },
	}, { 3 });	
	DFA dfa = pelda.toDFA();
	Language lang = DFAToRegLang(dfa);
	lang.cfGenerateWords(10);
	lang.printWords();
}

void Tests::testAppend(){
	DFA akarhany1vagy0(0, 4, {
		{ 0,1,'1' },
		{ 0,2,'0' },
		{ 1,1,'1' },
		{ 1,3,'0' },
		{ 2,3,'1' },
		{ 2,2,'0' },
		{ 3,3,'1' },
		{ 3,3,'0' },
	}, { 0,1,2 });
	DFA _1vagy0vagysemmi(0, 3, {
		{ 0,1,'1' },
		{ 0,1,'0' },
		{ 1,2,'1' },
		{ 1,2,'0' },
		{ 2,2,'1' },
		{ 2,2,'0' },		
	}, { 1,0 });
	DFA dfa = DFA::append(akarhany1vagy0, _1vagy0vagysemmi);
	dfa.print();
	Language lang = DFAToRegLang(dfa);
	lang.cfGenerateWords(10);
	lang.printWords();

}

void Tests::testCfRandom(){

}

void Tests::testOrBug(){	
	DFA m = dfabuilder::or(dfabuilder::endsWith(dfabuilder::specific_word("00")), dfabuilder::endsWith(dfabuilder::specific_word("0")) );
	m.minimize().print();
	Language lang = DFAToRegLang(m);
	lang.generateWords(8);
	lang.printWords();
}

void Tests::testfoldDFAonDFA(){
	DFA legalabb3(0, 4, {
		{ 0,1,'e' },
		{ 1,2,'e' },
		{ 2,3,'e' },
		{ 3,3,'e' },		
	}, { 3 });
	DFA paros(0, 2, {
		{ 0,1,'e' },
		{ 1,0,'e' },		
	}, { 0 });
	DFA legfeljebb3(0, 5, {
		{ 0,1,'e' },
		{ 1,2,'e' },
		{ 2,3,'e' },
		{ 3,4,'e' },
		{ 4,4,'e' },
	}, { 0,1,2,3 });
	DFA pontosan3(0, 5, {
		{ 0,1,'e' },
		{ 1,2,'e' },
		{ 2,3,'e' },
		{ 3,4,'e' },
		{ 4,4,'e' },
	}, { 3 });

	DFA _01(0, 4, {
		{ 0,1,'0' },
		{ 1,2,'1' },
		{ 0,3,'1' },
		{ 1,3,'0' },
		{ 2,3,'0' },
		{ 2,3,'1' },
	}, { 2 });
	DFA dfa = foldDFAonDFA(pontosan3, _01);
	
	dfa.print();
	Language lang = DFAToRegLang(dfa);
	lang.cfGenerateWords(12);
	lang.printWords();

}

void Tests::allFollowesBy() {
	DFA dfa = dfabuilder::allFollowedBy('b', 'a');
	dfa.print();
	Language lang = DFAToRegLang(dfa);
	lang.cfGenerateWords();
	lang.printWords();
	//KÉSZ
	DFA dfa2 = dfabuilder::allFollowedBy('a', 'b');
	dfa2.print();
	Language lang2 = DFAToRegLang(dfa2);
	lang2.cfGenerateWords();
	lang2.printWords();
	//
	DFA dfa3 = dfabuilder::endsWith(dfabuilder::specific_word("a"));
	dfa3.print();
	Language lang3 = DFAToRegLang(dfa3);
	lang3.cfGenerateWords();
	lang3.printWords();
	//
	DFA dfa4 = DFA::and(dfa2,dfa3);
	dfa4.print();
	Language lang4 = DFAToRegLang(dfa4);
	lang4.cfGenerateWords();
	lang4.printWords();
	//
	
	DFA dfa5 = DFA:: and (dfa4, dfa);	
	dfa5.print();
	Language lang5 = DFAToRegLang(dfa5);
	lang5.cfGenerateWords();
	lang5.printWords();
}

void Tests::accepts(){
	DFA dfa = dfabuilder::allFollowedBy('a', 'b');
	dfa.print();
	Language lang = DFAToRegLang(dfa);
	lang.cfGenerateWords();
	lang.printWords();
	cout << dfa.accepts("aba") << endl;
	cout << dfa.accepts("abb") << endl;
	cout << dfa.accepts("") << endl;
	cout << dfa.accepts("abab") << endl;
	cout << dfa.accepts("bab") << endl;
	cout << dfa.accepts("bbbaab") << endl;
}

void Tests::everyN() {
	DFA dfa = dfabuilder::everyNPropertyNonDet(3,dfabuilder::occuranceEqualsNumber('b', dfabuilder::numberWithRelation(1,"exact")));
	dfa.print();
	Language lang = DFAToRegLang(dfa);
	lang.cfGenerateWords(8);
	lang.printWords();
	//KÉSZ
}

void Tests::occurance() {
	cout << "a at least 3 times" << endl;
	DFA dfa = dfabuilder::occuranceEqualsNumber('b', dfabuilder::even());
	dfa.print();
	Language lang = DFAToRegLang(dfa);
	lang.cfGenerateWords(7);
	lang.printWords();
	//KÉSZ
}

void Tests::reachedGoalNTimes(){
	cout << "star" << endl;
	DFA star = dfabuilder::star(dfabuilder::specific_word("aa"));
	star.print();
	Language starlang = DFAToRegLang(star);
	starlang.cfGenerateWords(12);
	starlang.printWords();

	cout << "Ntimes" << endl;
	DFA Ntimes = dfabuilder::reachedGoalNTimes(dfabuilder::star(dfabuilder::specific_word("aa")), dfabuilder::numberWithRelation(5, "exact"));
	//Ntimes = Ntimes.minimize();
	Ntimes.print();
	Language NtimesLang = DFAToRegLang(Ntimes);
	NtimesLang.cfGenerateWords(12);
	NtimesLang.printWords();

	cout << "And" << endl;
	DFA dfa = DFA:: and (Ntimes, star); //ez nem ilyen egyszerû!
	dfa = dfa.minimize();
	dfa.print();
	Language lang = DFAToRegLang(dfa);
	lang.cfGenerateWords(12);
	lang.printWords();
}

void Tests::testBugNew() {
	cout << "aab" << endl;
	DFA aab = dfabuilder::endsWith(dfabuilder::specific_word("aab"));
	//aab.print();
	Language aablang = DFAToRegLang(aab);
	aablang.cfGenerateWords(8);
	//aablang.printWords();
	//
	cout << "a" << endl;
	DFA a = dfabuilder::endsWith(dfabuilder::specific_word("a"));
	//a.print();
	Language alang = DFAToRegLang(a);
	alang.cfGenerateWords(8);
	//alang.printWords();
	//
	cout << "b" << endl;
	DFA b = dfabuilder::startsWith(dfabuilder::specific_word("b"));
	//b.print();
	Language blang = DFAToRegLang(b);
	blang.cfGenerateWords(8);
	//blang.printWords();
	//
	cout << "orbelso" << endl;
	DFA orbelso = dfabuilder::or(a,b);
	orbelso.print();
	Language orbelsolang = DFAToRegLang(orbelso);
	orbelsolang.cfGenerateWords(8);
	orbelsolang.printWords();
	//
	cout << "orkulso" << endl;
	DFA orkulso = dfabuilder::or(aab, orbelso);
	orkulso.print();
	Language orkulsoLang = DFAToRegLang(orkulso);
	orkulsoLang.cfGenerateWords(8);
	orkulsoLang.printWords();
}
void Tests::TestEquals() {
	DFA legalabb3(0, 4, {
		{ 0,1,'e' },
		{ 1,2,'e' },
		{ 2,3,'e' },
		{ 3,3,'e' },
	}, { 3 });
	cout << legalabb3.equals(legalabb3) << endl;
	DFA legalabb3mod1(0, 4, {
		{ 0,0,'e' },
		{ 1,2,'e' },
		{ 2,3,'e' },
		{ 3,3,'e' },
	}, { 3 });
	cout << legalabb3mod1.equals(legalabb3) << endl;
	DFA legalabb3mod2(0, 4, {
		{ 0,1,'e' },
		{ 1,2,'e' },
		{ 2,3,'e' },
		{ 3,3,'e' },
	}, { 3,2 });
	cout << legalabb3mod2.equals(legalabb3) << endl;
	DFA legalabb3mod3(0, 5, {
		{ 0,1,'e' },
		{ 1,2,'e' },
		{ 2,3,'e' },
		{ 3,3,'e' },
		{ 4,3,'e' },
	}, { 3 });
	cout << legalabb3mod3.equals(legalabb3) << endl;
}