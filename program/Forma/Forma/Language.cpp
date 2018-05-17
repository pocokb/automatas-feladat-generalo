#include "stdafx.h"
#include "Language.h"
#include<iostream>
#include <io.h>
#include <fcntl.h>
#include<windows.h>
#include<stack>
#include <algorithm>
#include"RandGen.h"
#include<functional>
using namespace std;


template<class T>
set<T> operator -(set<T> reference, set<T> items_to_remove)
{
	set<T> result;
	std::set_difference(
		reference.begin(), reference.end(),
		items_to_remove.begin(), items_to_remove.end(),
		std::inserter(result, result.end()));
	return result;
}

char* createMultibyteString(wchar_t *s) {
	int bufferSize = WideCharToMultiByte(CP_UTF8, 0, s, -1, NULL, 0, NULL, NULL);
	char* m = new char[bufferSize];
	WideCharToMultiByte(CP_UTF8, 0, s, -1, m, bufferSize, NULL, NULL); //16 bites fix ből utf-8-ra
	return m;
}

Language::Language(string terminating, string nonTerminating, string start, vector<string> rules)
	: terminating(terminating), nonTerminating(nonTerminating), start(start)
{	
	for (string str : rules) { //szabályok kialakítása
		int pos = str.find("-");
		Rule rule = Rule{ str.substr(0,pos), str.substr(pos + 2) };
		this->rules.push_back(rule);		
	}
	/*
	SetConsoleOutputCP(CP_UTF8);
	utfSymbols.leftArrow = createMultibyteString(L"◄"); //new
	utfSymbols.rightArrow = createMultibyteString(L"►"); //new
	utfSymbols.upArrow = createMultibyteString(L"▲"); //new
	utfSymbols.downArrow = createMultibyteString(L"▼"); //new
		*/	
	/*
	for (Rule rule : this->rules) {
		cout << rule.left << "    ->     " << rule.right << endl;
	}*/
}

Language::Language() {};

Language::Language(string terminating, string nonTerminating, string start, initializer_list<string> rules)
	: Language(terminating, nonTerminating, start, vector<string>(rules)) {}

Language::~Language() {/*
	delete[] utfSymbols.leftArrow;
	delete[] utfSymbols.rightArrow;
	delete[] utfSymbols.upArrow;
	delete[] utfSymbols.downArrow;
	*/
}


//Kiir de a speciális karaktereket unicode nyilakra fordítja
void Language::printWord(string str) {			
	for (char c : str) {
		switch (c) {
		/*case 'l': 						
			printf("%s", utfSymbols.leftArrow);
			break;
		case 'r':
			printf("%s", utfSymbols.rightArrow);
			break;*/
		default: 
			cout << c;
		}
	}
	cout << endl;
}

//// PRÓBÁLKOZÁS ÁLTALÁNOS KIFEJTŐRE FÁVAL DE NEM IS ÉRTEM HOGY MI LENNE ÁLTALÁNOSRA A FA SZÓVAL EZT NEM FOLYTATOM...

bool g_printExceed, flagDepthReached;
set<string> votma;

//szeretném ennek megtartani a nyelvi jellegét (formulák helyett), hogy alkalmazható legyen majd általánosra is...
void Language::rGen(string cur , int depth) { //ez a merginfos dolog egyébként contextusfüggöre szerintem nem is helyes...	
	for (Rule rule : rules) {		
		size_t pos = cur.find(rule.left);
		if (pos != string::npos) {
			string newcur(cur);
			newcur.replace(pos, rule.left.length(), rule.right);
			if (votma.count(newcur) > 0) {
				continue;
			}else {
				votma.insert(newcur);								
				if (newcur.find_first_of(this->nonTerminating) != string::npos) {
					if (depth == 0) {
						flagDepthReached = true;
						if (g_printExceed) {
							//printWord(newcur + " , recursion depth exceeded!"); //ezzel nem kell visszatérni resultként mert ebbe még van nemterminális, csak elérte a mélységet!
						}
					}else {
						rGen(newcur, depth - 1);
					}
				}else {					
					//printWord(newcur);
					words.push_back(newcur);
				}				
			}										
			
		}
	}
}

void Language::generateWords(int maxdepth, bool printExceed) {	
	votma.clear();
	words.clear();	
	g_printExceed = printExceed;
	flagDepthReached = false;
	cout << "Started generating words with depth " << maxdepth << ".."<< endl;
	rGen(start,maxdepth);
	if (flagDepthReached) cout << "Depth was reached." << endl;

}

//cf kifejtő

//variations helper
vector<SynthTreeNode*> allTreeNodes;
vector<ResultWord> allVariations;
vector<ResultWord> curVariation;
vector<vector<ResultWord>> inputCombinations;
string curWord;

void getAllVariation(int n, int d) {	
	
	if (inputCombinations[d].size() == 0) { //nincs terminális az adott ágon
		//cout << "nincs terminális ezen az ágon" << endl;
		return;
	}
	for (ResultWord s : inputCombinations[d]) {
		
		curVariation[d] = s;
		
		if (d == n - 1) {			//ez így jó csak a memóriakezelés még szar			
			ResultWord w;
			SynthTreeNode *last = nullptr;
			for (int i = 0; i < curVariation.size(); i++) {
				
				ResultWord u = curVariation[i];
				
				w.word += u.word;				
				SynthTreeNode *newNode = new SynthTreeNode{curWord[i], u.synthTree, nullptr }; //másolni kell a sub szintaktikus fát! vagy nem? lehet reuseolni?
				
				allTreeNodes.push_back(newNode);
				if (last != nullptr){
					//cout << "testver set" << endl; //ha már nem első, akkor az előzőnek a testvére
					last->testver = newNode;
				}else {
					w.synthTree = newNode; //ha az első akkor ő lesz a synthtreeje a resultwordnek
				}
				
				//cout << w.word << endl;
				last = newNode;
			}
			allVariations.push_back(w);
		}else {			
			getAllVariation(n, d + 1);
		}
		
	}
}
vector<ResultWord> getAllVariations(vector<vector<ResultWord>> combinations, string cur) { //variációk 0 és n-1 között
	allVariations.clear();
	curWord = cur;
	inputCombinations = combinations;
	curVariation = vector<ResultWord>(combinations.size());
	/*
	for (int i = 0; i < n; i++) {
		curVariation[i] = 0;
	}*/
	getAllVariation(combinations.size(), 0);
	return allVariations;
}
//
vector<SynthTreeNode> synthTree;
//vector<ResultWord> results;

vector<ResultWord> Language::rCfGen(string cur, int depth) {
	if (depth <= 0) { //elértük a mélységet, nemjó
		flagDepthReached = true;
		return vector<ResultWord>(); //üresen térünk vissza
	}else if (cur == "") { //epsilon szabály , ezt a fák rosszul kezelik de nem baj, mert nem is szabad hogy legyen benne epsilon szabály
		return vector<ResultWord>({ ResultWord{ string(""), nullptr } });
	}else{ //különben a szokásos rekurzív ásás
		vector<vector<ResultWord>> wordCombinations;
		//vector<SynthTreeNode> treeNodes;
		
		for (int i = 0; i < cur.length(); i++) {
			char c = cur[i];			
			if (this->nonTerminating.find_first_of(c) != string::npos) { //ha a karakter nemterminális akkor csinálunk rá egy vektort ami tartalmazza a lehetséges kifejtéseit				
				vector<ResultWord> subResultsForNonTerm;
				for (Rule rule : rules) { //nézzük a szabályokat melyikkel lehet sorra
					if (rule.left[0] == c) { //bingo, helyettesíthető! (és itt kihasználjuk hogy cf) //sőt kell hogy a bal oldali szabály ne legyen üres különben bug											 												
						vector<ResultWord> subResult = rCfGen(rule.right, depth - 1);						
						subResultsForNonTerm.insert(subResultsForNonTerm.end(), subResult.begin(), subResult.end());												
					}
				}
				wordCombinations.push_back(subResultsForNonTerm);
			}else{ //ha terminális, akkor csak egy egyelemű vectort a formalitás miatt
				string str;
				str += c;
				vector<ResultWord> termVector({ ResultWord{str, nullptr} }); // "" + c csak javában értelmes c-ben horribilis hiba... a pointerhez adsz hozzá...
				//cout << "term: " << c << endl;
				wordCombinations.push_back(termVector);
			}
		}
		//most megvannak a kombinációk, már csak ki kell fejteni a lehetséges szavakat:
		
		return getAllVariations(wordCombinations, cur);
	}		
}

vector<ResultWord> Language::cfGenerateWords(int maxdepth, bool printExceed) { //generáló csak cfre // memory leaket csinál!
	votma.clear();
	synthTree.clear();
	words.clear();
	//results.clear();
	/*for (SynthTreeNode* node : allTreeNodes) { //memory leak ellen ujragenerálásnál töröl. Ez nem az igazi de majd meglátjuk
		delete node;
	}
	allTreeNodes.clear();*/
	g_printExceed = printExceed;
	flagDepthReached = false;
	//cout << "Started generating words with depth " << maxdepth << ".." << endl;
	vector<ResultWord> results = rCfGen(start, maxdepth);
	//if (flagDepthReached) cout << "Depth was reached." << endl;
	for (ResultWord rw : results) {
		this->words.push_back(rw.word);
	}	
	return results;
}

ResultWord Language::cfRandomGen(int depth){ //memory leaket csinál!
	map<char, vector<string>> rulemapFull = getRuleMap();
	RandGen rg;
	
	set<string> complex({ "O","E"});
	int countComplex = 0;

	function<ResultWord(char,int,int,int,map<char, vector<string>>)> req;
	req = [&rg,&req,&complex](char left, int countComplex, int countChar, int countNum, map<char, vector<string>> rulemap)->ResultWord {
		//if (depth == 0) return{ "",nullptr };

		SynthTreeNode *node = new SynthTreeNode{left,nullptr,nullptr};				
		
		if (countComplex == 2) {
			//ha elértük a mélységet, kivesszük a complex típusokat					
			for (string c : complex) {
				if (std::find(rulemap['W'].begin(), rulemap['W'].end(), c) != rulemap['W'].end()) {
					rulemap['W'].erase(std::remove(rulemap['W'].begin(), rulemap['W'].end(), c));
				}				
			}
			rulemap['O'] = { "WW" };
			rulemap['E'] = { "WW" };
		}
		if (countChar >= 3) {
			rulemap['A'] = { "" };			
		}
		if (countNum >= 2) {			
			rulemap['M'] = { "" };
		}
		int n = rulemap[left].size();
		if (n == 0) { //ha nincs az adott baloldalhoz jobboldali szabály, akkor terminális kell, hogy legyen (a mi szabályrendszerünkben, kivéve ha rosszul adtuk meg)
			//igazából semmi nem történik csak nem kerül feltöltésre az elsőfiu
		}else {			
			string choice = rulemap[left][rg.randInt(n)];
			//cout << n << endl;
			SynthTreeNode *last = nullptr;
			for (char c : choice) {
				if (complex.count(string() + c) > 0) {
					countComplex++;
				}
				if(c == 'A'){
					countChar++;
				}
				if(c == 'M') {
					countNum++;
				}
				auto lentebbi = req(c,countComplex,countChar,countNum,rulemap).synthTree;
				if (last == nullptr) { //első a fiuk közül
					last = lentebbi;
					node->elsofiu = last;
				}
				else { //már testvér
					last->testver = lentebbi;
					last = last->testver;
				}
			}
		}		
		return ResultWord{ "", node };
	};

	return req(start[0],0,0,0,rulemapFull);
}



void Language::printDescription() {
	cout << "Terminating characters: " << terminating << endl;
	cout << "Non-terminating characters: " << nonTerminating << endl;
	cout << "Start string: " << start << endl;
	cout << "Rules: " << endl;
	for (Rule rule : rules) {
		cout << "\t" << rule.left << "->" << rule.right << endl;
	}
}
map<char, vector<string>> Language::getRuleMap(){
	map<char, vector<string>> rulemap;
	for (Rule r : rules) {
		rulemap[r.left[0]].push_back(r.right);
	}	
	return rulemap;
}

void Language::printWords() {
	for (string word : words) {
		cout << word << " ";
	}
	cout << endl;
}
void Language::printWordsMax(int max) {
	size_t c = 0;
	for (string word : words) {
		if (c++ > max) break;
		cout << word << " ";
	}
	cout << endl;
}


Language Language::random(int nTerm, int nNonterm, int nRules, int nLeft, int nRight,int nStart) { //randomnyelv
	RandGen rg;
	string term, nonterm, start;	
	
	for (int i = 0; i < nTerm; i++) {
		term += ('a' + i);
	}
	for (int i = 0; i < nNonterm; i++) {
		nonterm += ('A' + i);
	}
	string possible(term + nonterm);
	for (int i = 0; i < nStart; i++) {
		start += possible[rg.randInt(possible.length())];
	}
	vector<string> rules;
	for (int i = 0; i < nRules; i++) {
		string left, right;
		for (int i = 0; i < nLeft; i++) {
			left += nonterm[rg.randInt(nonterm.length())];
		}
		for (int i = 0; i < nRight; i++) {
			right += possible[rg.randInt(possible.length())];
		}
		
		rules.push_back(left + "->" + right);
	}
	
	return Language(term, nonterm, start, rules);
}

Language::Combination::Combination(Language &base, char nonTerm, Language &fn) : base(base), fn(fn){
	std::size_t found = base.nonTerminating.find(nonTerm);
	if (found == std::string::npos) {
		cerr << "Error, nonterminating character not found." << endl; //hibakezelés?
		nonTermIndex = 0;
	}else {
		nonTermIndex = found;
	}
}

/*char firstFree(string str, char start, char end) { //SZAR MEGOLDÁS ÉS KORLÁTOZOTT...
	char c = start;
	while (c != end + 1) {
		if (str.find(c) == string::npos)
			return c;
		c++;
	}
}*/
void replaceAllChars(string& str, string oldChars, string newChars) { //feltételezi, hogy str minden karaktere benne van oldcharsban és |oldchars| = |newchars|	
	for (int i = 0; i < str.length(); i++) {
		int pos = oldChars.find(str[i]);
		if (pos != string::npos) {			
			str[i] = newChars[pos];
		}
		
	}
}
Language Language::combine(Language *base, initializer_list<Language*> elements, initializer_list<Combination> combinations) {
	
	string term, nonTerm;
	char free = 'A';
	vector<string> rules;	
	//vector<Language> elements(elementsi);
	for (Language *l : elements) {		
		for (char c : l->terminating) {
			if (term.find(c) == string::npos) term += c;
		}
		string newNames, oldNames;
		oldNames = l->nonTerminating;
		for (char c : l->nonTerminating) {
			nonTerm += free;
			newNames += free;
			free++;
		}	
		replaceAllChars(l->start, oldNames, newNames);			
		
		replaceAllChars(l->nonTerminating, oldNames, newNames);		
		for (Rule& rule: l->rules) {							 //EGYSZERŰBB DE SZARABB MEGOLDÁS!
			string ruleStr(rule.left + "->" + rule.right);
			replaceAllChars(ruleStr, oldNames, newNames);
			rules.push_back(ruleStr);
		}							
	}
	//Jöhetnek az összekötések:
	for (Combination com : combinations) {		
		string left, right;
		left += (com.base.nonTerminating[com.nonTermIndex]);
		right += com.fn.start;		
		rules.push_back(left + "->" + right );
	}

	return Language(term, nonTerm, base->start, rules);
}
Language& Language::merge(Language &subLang, string on) {
	char freeTry = 'A';
	string oldNames, newNames;
	string newNonTerms;
	for (char c : subLang.nonTerminating) {
		if (this->nonTerminating.find(c) != string::npos && string("I").find(c) == string::npos) { //ha nemterminális ütközés van és nincs rajta a tiltólistán
			while (this->nonTerminating.find(freeTry) != string::npos || subLang.nonTerminating.find(freeTry) != string::npos || freeTry == 'I') { //keresünk egy nagybetűt ami egyikbe sem foglalt				
				freeTry++;				
				if (freeTry == 0) break; //failsafe, arra az esetre ha minden char foglalt ne fagyjon le
			}
			oldNames += c;
			newNames += freeTry;
			newNonTerms += freeTry;
			freeTry++; //ide is kell egy + különben a következő helyettesítésnél ugyanazt adja!
		}else {
			newNonTerms += c;
		}
	} // megvannak a megfelelő helyettesítők és az új nemterminálisok
	//elvégezzük a helyettesítéseket a subLangon (az alapnyelv marad!)
	replaceAllChars(subLang.start, oldNames, newNames);
	replaceAllChars(subLang.nonTerminating, oldNames, newNames);
	for (Rule& rule : subLang.rules) {
		replaceAllChars(rule.left, oldNames, newNames);
		replaceAllChars(rule.right, oldNames, newNames);		
	}
	//kész vagyunk a helyettesítésekkel	
	//most hozzá kell adni az új szabályokat az alapnyelvhez
	this->rules.insert(this->rules.end(), subLang.rules.begin(), subLang.rules.end());
	//és az új nemterminálisokat
	this->nonTerminating += newNonTerms;
	//és hozzáadni az összekötő helyettesítést	
	this->rules.push_back(Rule{ on,subLang.start });

	//kész

	return *this;
}

void SynthTreeNode::print(int depth) {
	
	SynthTreeNode *node = this;
	while (node != nullptr) {
		for (int i = 0; i < depth; i++) {
			cout << "\t";
		}
		cout << node->symbol << endl;
		if (node->elsofiu != nullptr) {
			node->elsofiu->print(depth + 1);
		}
		node = node->testver;	
	}
}

Language SynthTreeNode::mergeSemantics(map<char, Language> &meaning) {	//hát ez head recursion de most ez van
	SynthTreeNode *node = this;

	if (meaning.count(node->symbol) == 0) {
		cerr << "Error! No meaning found for symbol. Result is corrupt!" << endl;
	}
	Language mainSema = meaning[node->symbol];
	char sym = node->symbol;
	//cout << "+++" << sym << endl;
	int i = 0;	
	node = node->elsofiu;
	while (node != nullptr) {			
		Language subSema = node->mergeSemantics(meaning); //postorder
		//mainSema.printDescription();
		mainSema.merge(subSema, {(char)(i + 'P')});
		//cout << node->symbol << endl;
		//cout << "lang after merge:" << endl;
		//mainSema.printDescription();
		node = node->testver;
		i++;
		//cout << "testver i: " << i << endl;
	}	
	//cout << "---" << sym << endl;
	return mainSema;
}



void Language::lancMent() {
	struct nemterm{
		set<char> elerhetok;
		bool visited = false, finished = false;
		set<string> rightHands;
	};
	map<char,nemterm> nemtermek;	

	for (Rule r : this->rules) {
		if (r.left.length() == 1) { //ha cf szabály
			if (r.right.length() == 1 && this->terminating.find(r.right[0]) == string::npos ) { //ha láncszabály
				nemtermek[r.left[0]].elerhetok.insert(r.right[0]);			
			}else { //ha rendes szabály
				nemtermek[r.left[0]].rightHands.insert(r.right);
			}			
		}		
	}
	
	stack<char> verem;	
	for (char c : nonTerminating) {
		verem.push(c);
	}	
	while (verem.size() > 0) {		
		char cur = verem.top();
		//cout << "verem bejaras: " << cur << endl;
		if (nemtermek[cur].visited == true) { //ha már visszafele jövünk és minden rápakolt föl lett dolgozva	
			//cout << "visszafele" << endl;
			if (nemtermek[cur].finished != true) {
				for (char c : nemtermek[cur].elerhetok) {
					nemtermek[cur].rightHands.insert(nemtermek[c].rightHands.begin(), nemtermek[c].rightHands.end());
					//cout << "righthand hozzaadva!" << endl;
				}
				nemtermek[cur].finished = true;
			}						
			verem.pop();			
		}else {	//különben még nem lettek bejárva a gyerekek, szoval ezt kell tenni		
			
			for (char c : nemtermek[cur].elerhetok) {
				verem.push(c);
				//cout << c << " pushed" << endl;
			}
			nemtermek[cur].visited = true; //rápakoljuk a követelményeket és megjlelöljük, hogyha fölülröl ujra visszaérünk akkor már kész
		}
	}
	this->rules.clear();	
	for (const auto& nt : nemtermek) {
		for (string str : nt.second.rightHands) {
			this->rules.push_back(Rule{ string{ nt.first }, str });
		}		
	}
}

void Language::felesElhagy(){
	set<char> elerhetok, ujak, temp;	
	size_t sizeprev = -1;
	ujak.insert(this->start[0]);
	elerhetok.insert(this->start[0]);
	while (sizeprev != elerhetok.size()) {
		for (Rule r : this->rules) {
			if (ujak.count(r.left[0]) == 1) {
				for (char c : r.right) {					
					temp.insert(c);					
				}

			}
		}
		sizeprev = elerhetok.size();		
		elerhetok.insert(temp.begin(), temp.end());
		ujak = temp;
	}	
	vector<Rule> newrules;
	for (Rule r : this->rules) {
		if (elerhetok.count(r.left[0]) == 1) {
			newrules.push_back(r);
		}
	}
	this->rules = newrules;
	string newnonterms, newterms;
	for (char c : this->nonTerminating) {
		if (elerhetok.count(c) == 1) {
			newnonterms += c;
		}
	}
	for (char c : this->terminating) {
		if (elerhetok.count(c) == 1) {
			newterms += c;
		}
	}
	this->terminating = newterms;
	this->nonTerminating = newnonterms;
}

//todo: cfcollapse, convertReglangToDFA

