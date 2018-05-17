#include "stdafx.h"
#include "SentenceBuilder.h"


SentenceBuilder::SentenceBuilder()
{
}


SentenceBuilder::~SentenceBuilder()
{
}

namespace sentence {
	bool negatedModifier;

	vector<char> abc = { '0','1'};
	string specific_word(SynthTreeNode *node) { //itt most van function overload, ha nincs fia akkor õ terminál
		string w;
		w.append({ node->symbol });
		node = node->testver; //2.paraméterre lép
		while (node->elsofiu != nullptr) {
			node = node->elsofiu;
			w.append({ node->symbol });
			node = node->testver;
		}
		//megvan a string, most visszatérünk egy automatával ami felismeri azt
		return w;
	}

	int number(SynthTreeNode *node) {
		int a = 0;
		while (node->elsofiu != nullptr) {
			node = node->elsofiu->testver;
			a++;
		}
		//cout << a << endl;
		return a;		
	}	
	string contains(SynthTreeNode *node) { //konstrukció (paraméterekbõl)								
		string mit = specific_word(node->elsofiu);
		//string mit = statement(node->elsofiu);
		if (eatNegatedModifier()) {
			return "nem tartalmaz " + addTargyrag(mit);
		}
		else {
			return "tartalmaz " + addTargyrag(mit);
		}
		
	}
	string startsWith(SynthTreeNode *node) { //konstrukció (paraméterekbõl)								
		string mit = specific_word(node->elsofiu);
		//string mit = statement(node->elsofiu);
		if (eatNegatedModifier()) {
			return  "nem kezdõdik " + addTarsrag(mit);
		}else {
			return  addTarsrag(mit) + " kezdõdik";
		}		
	}
	string endsWith(SynthTreeNode *node) { //konstrukció (paraméterekbõl)								
		string mit = specific_word(node->elsofiu);
		//string mit = statement(node->elsofiu);
		if (eatNegatedModifier()) {
			return "nem végzõdik " + addHelyhatarozo(mit);
		}
		else {
			return  addHelyhatarozo(mit) + " végzõdik";
		}
	}
	string exact_number(SynthTreeNode *node) { //konstrukció (paraméterekbõl)								
		int nTimes = number(node);		
		return "pontosan " + to_string(nTimes);
	}
	string atMost(SynthTreeNode *node) { //konstrukció (paraméterekbõl)								
		int nTimes = number(node);
		return "legfeljebb " + to_string(nTimes);
	}
	string atLeast(SynthTreeNode *node) { //konstrukció (paraméterekbõl)								
		int nTimes = number(node);
		return "legalább " + to_string(nTimes);
	}
	string containsNTimes(SynthTreeNode *node) {
		string n = statement(node->elsofiu);
		string mit = specific_word(node->testver->elsofiu);
		//string mit = statement(node->testver->elsofiu);
		if (eatNegatedModifier()) {
			return "nem igaz, hogy " + n + " " + addTargyrag(mit) + " tartalmaz";
		}
		else {
			return "tartalmaz " + n + " " + addTargyrag(mit);
		}		
	}
	string NConsecutive(SynthTreeNode *node) {
		string n = statement(node->elsofiu);
		string mi = specific_word(node->testver->elsofiu);
		//string mit = statement(node->testver->elsofiu);
		if (eatNegatedModifier()) {
			return "nem igaz, hogy " + n + " egymás utáni " + addBelso(mi) + " áll";
		}
		else {
			return n + " egymás utáni " + addBelso(mi) + " áll";
		}		
	}
	string and(SynthTreeNode *node) {
		string st1 = statement(node->elsofiu);
		string st2 = (	node->testver->symbol == 'E'
						? statement(node->testver)
						: statement(node->testver->elsofiu));	
		return st1 + " és " + st2;
	}
	string or(SynthTreeNode *node) {
		string st1 = statement(node->elsofiu);
		string st2 = (	node->testver->symbol == 'O'
						? statement(node->testver)
						: statement(node->testver->elsofiu));
		return st1 + " vagy " + st2;
	}
	string not(SynthTreeNode *node) {
		string st = statement(node);
		return st;
	}
	string counter(SynthTreeNode *node) {
		string ret = string("|W|") + abc[0];
		size_t i = 1;
		node = node->testver; //az elsõt skippelhetjük hisz ugyis +1-es és az elsõ betû
		while (node != nullptr) {
			switch (node->elsofiu->symbol) {
			case 'e': ret += (string(" + |W|") + abc[i]); break;
			case 'h': ret += (string(" - |W|") + abc[i]); break;
			case 'z': break;
			default: cout << "error" << node->elsofiu->symbol << endl; break;
			}			
			node = node->testver;
			i++;
		}		
		return ret;
	}
	string even_number(SynthTreeNode *node) {		
		return  "páros számú";
	}
	string odd_number(SynthTreeNode *node) {		
		return  "páratlan számú";
	}
	string divisibleBy_number(SynthTreeNode *node) {
		int nTimes = number(node);		
		return  addTarsrag(to_string(nTimes + 1)) + " oszható"; // + 1 kell, hogy 2-rol induljon (HACK)
	}
	string counterEqualsNumber(SynthTreeNode *node) {
		if (eatNegatedModifier()) {
			return "nem igaz, hogy " + counter(node->elsofiu) + " " + statement(node->testver->elsofiu);
		}
		else {
			return counter(node->elsofiu) + " " + statement(node->testver->elsofiu);
		}
		
	}
	string allFollowedBy(SynthTreeNode *node) {
		string ret = string("minden ") + addTargyrag(string() + node->elsofiu->symbol) + " " + node->testver->elsofiu->symbol + string(" követ");
		if (eatNegatedModifier()) {
			return "nem igaz, hogy " + ret;
		}
		else {
			return ret;
		}
	}

	string everyNPropertyNonDet(SynthTreeNode *node) {
		int everyN = number(node);
		string nTimes = statement(node->testver->elsofiu);
		char letter = node->testver->testver->elsofiu->symbol;
		string ret = string("bármely ") + to_string(everyN) + string(" egymás utáni betû tartalmaz ") + nTimes + string(" ") + letter + (" betût");
		if (eatNegatedModifier()) {
			return "nem igaz, hogy " + ret;
		}
		else {
			return ret;
		}
	}

	string letter(SynthTreeNode *node) {
		return string() + node->symbol + " betût";
	}

	string statement(SynthTreeNode *node) { //választó, (több lehetõség)				
		switch (node->symbol) {
		case 'S':
			return specific_word(node->elsofiu); //ilyenkor az alfüggvények már magát az elsõ paramétert kapják, aminek a testvérei a többiek
		case 'C':
			return contains(node->elsofiu);
		case 'K':
			return containsNTimes(node->elsofiu);
		case 'F':
			return NConsecutive(node->elsofiu);
		case 'E':
			return and(node->elsofiu); 
		case 'O':
			return or(node->elsofiu); break;
		case 'T':
			negatedModifier = true;
			return not(node->elsofiu); break;
		case 'Z':
			return startsWith(node->elsofiu); break;
		case 'V':
			return endsWith(node->elsofiu); break;
		case 'Q':
			return exact_number(node->elsofiu); break;
		case 'R':
			return atMost(node->elsofiu); break;			
		case '$':
			return atLeast(node->elsofiu); break;			
		case 'L':			
			return counterEqualsNumber(node->elsofiu); break;
		case 'X':
			return even_number(node->elsofiu); break;
		case '|':
			return odd_number(node->elsofiu); break;
		case 'H':
			return divisibleBy_number(node->elsofiu); break;
		case 'U':
			return allFollowedBy(node->elsofiu); break;
		case 'D':
			return everyNPropertyNonDet(node->elsofiu); break;
		case 'B':
			return letter(node->elsofiu); break;			
		default:
			cout << "Nincs szabály megadva a következõre: " << node->symbol << endl;
			return string();
		}
	}

	string buildSentence(SynthTreeNode * node) {
		if (node->symbol == 'W') {
			node = node->elsofiu;
		}
		negatedModifier = false;
		//itt a node már NEM a W-rõl indul, hanem ami az elsõfia
		string abcList = "{";
		for (char c : abc) {
			abcList += c + string(",");
		}
		abcList.pop_back();
		abcList += "}";
		set<char> complex = { 'E','O','T','W' };
		string sentence = string("Adj meg olyan ") + abcList + " abc-jû automatát amely a következõt ismeri fel: szó, ";
		if (node->symbol == 'T' && complex.count(node->elsofiu->elsofiu->symbol) > 0) { //ha tagadással kezdõdik és alatta összetett, de ilyen most az ujba már nincs (inkább kivettem)
			sentence += "amelyre nem igaz, hogy ";
			node = node->elsofiu->elsofiu; //attekri a nem-en
		}else {
			//meghatarozza az elsõ valódi állítás típusát
			auto tempNode = node;
			bool firstIsNegated = false;
			while (complex.count(tempNode->symbol) > 0) {
				if (tempNode->symbol == 'T') firstIsNegated = true;
				tempNode = tempNode->elsofiu;
			}
			char realFirst = tempNode->symbol;			
			switch (realFirst) {
			case 'U':
			case 'L':
			case 'D':
				if (firstIsNegated) {
					sentence += "amelyre ";
				}else {
					sentence += "amelyben ";
				}								
				break;
			case 'F':
			case 'K':
				if (firstIsNegated) {
					sentence += "amelyre ";
				}
				else {
					sentence += "ami ";
				}
				break;
			default: sentence += "ami "; break;
			}			
		}
		return  sentence + statement(node) + "!";
	}
	//toltadélokok:	
	string addTargyrag(string szo) {
		switch (szo.back()) {
		case '0': return szo + "-t";
		case '1': return szo + "-et";
		case '2': return szo + "-t";
		case '3': return szo + "-at";
		case '4': return szo + "-et";
		case '5': return szo + "-öt";
		case 'a': return szo + "-t";
		case 'b': return szo + "-t";
		case 'c': return szo + "-t";
		default: return "error";
		}
	}
	string addTarsrag(string szo) {
		switch (szo.back()) {
		case '0': return szo + "-val";			
		case '1': return szo + "-gyel";
		case '2': return szo + "-vel";
		case '3': return szo + "-mal";
		case '4': return szo + "-gyel";
		case '5': return szo + "-tel";
		case 'a': return szo + "-val";
		case 'b': return szo + "-vel";
		case 'c': return szo + "-vel";
		default: return "error";
		}
	}
	string addHelyhatarozo(string szo) {
		switch (szo.back()) {
		case '0': return szo + "-ra";
		case '1': return szo + "-re";
		case '2': return szo + "-re";
		case '3': return szo + "-ra";
		case '4': return szo + "-re";
		case '5': return szo + "-re";
		case 'a': return szo + "-ra";
		case 'b': return szo + "-re";
		case 'c': return szo + "-re";
		default: return "error";
		}
	}
	string addBelso(string szo) {
		switch (szo.back()) {
		case '0': return szo + "-ból";
		case '1': return szo + "-bõl";
		case '2': return szo + "-bõl";
		case '3': return szo + "-ból";
		case '4': return szo + "-bõl";
		case '5': return szo + "-bõl";
		case 'a': return szo + "-ból";
		case 'b': return szo + "-bõl";
		case 'c': return szo + "-bõl";
		default: return "error";
		}
	}
	bool eatNegatedModifier() {
		bool ret = negatedModifier;
		negatedModifier = false;
		return ret;
	}
	
}