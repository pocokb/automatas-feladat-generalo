#pragma once
#include<string>
#include<set>
#include<vector>
#include<map>

using namespace std;

class Language;

struct SynthTreeNode {
	char symbol;
	SynthTreeNode *elsofiu, *testver;
	void print(int depth = 0);
	Language mergeSemantics(map<char, Language> &meaning);
};

struct ResultWord {
	string word;
	SynthTreeNode *synthTree;
};

class Language{
public:	
	struct Rule {
		string left, right;	
	};
	struct UTFSymbols {
		char *leftArrow, *rightArrow, *downArrow, *upArrow;		
	};	
	
	UTFSymbols utfSymbols;

	string terminating, nonTerminating, start;
	vector<Rule> rules;
	vector<string> words;

	Language();
	Language(string terminating, string nonTerminating, string start, vector<string> rules);
	Language(string terminating, string nonTerminating, string start, initializer_list<string> rules);	
	~Language();
	void generateWords(int depth=6, bool printExceed=false);
	vector<ResultWord> cfGenerateWords(int depth = 6, bool printExceed = false);
	ResultWord cfRandomGen(int depth = 6);


	map<char, vector<string>> getRuleMap();
	//vector<ResultWord> generateResultWords(int depth = 6, bool printExceed = false);
	void printWords();
	void printWordsMax(int max);
	void printDescription();
	void lancMent();
	void felesElhagy();

	static Language random(int nTerm, int nNonterm, int nRules, int nLeft, int nRight, int nStart);

	struct Combination {
		Language &base;
		int nonTermIndex;
		Language &fn;
		Combination(Language &base, char nonTerm, Language &fn);
	};	
	static Language combine(Language *base, initializer_list<Language*> elements, initializer_list<Combination> combinations);
	Language& merge(Language& subLang, string on);
	
private:
	void Language::printWord(string str);
	void Language::rGen(string cur, int depth);
	vector<ResultWord> Language::rCfGen(string cur, int depth);
	
};


//vector<vector<int>> getAllVariations(int n);