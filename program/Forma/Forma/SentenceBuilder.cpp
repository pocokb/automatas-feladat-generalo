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
	string specific_word(SynthTreeNode *node) { //itt most van function overload, ha nincs fia akkor � termin�l
		string w;
		w.append({ node->symbol });
		node = node->testver; //2.param�terre l�p
		while (node->elsofiu != nullptr) {
			node = node->elsofiu;
			w.append({ node->symbol });
			node = node->testver;
		}
		//megvan a string, most visszat�r�nk egy automat�val ami felismeri azt
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
	string contains(SynthTreeNode *node) { //konstrukci� (param�terekb�l)								
		string mit = specific_word(node->elsofiu);
		//string mit = statement(node->elsofiu);
		if (eatNegatedModifier()) {
			return "nem tartalmaz " + addTargyrag(mit);
		}
		else {
			return "tartalmaz " + addTargyrag(mit);
		}
		
	}
	string startsWith(SynthTreeNode *node) { //konstrukci� (param�terekb�l)								
		string mit = specific_word(node->elsofiu);
		//string mit = statement(node->elsofiu);
		if (eatNegatedModifier()) {
			return  "nem kezd�dik " + addTarsrag(mit);
		}else {
			return  addTarsrag(mit) + " kezd�dik";
		}		
	}
	string endsWith(SynthTreeNode *node) { //konstrukci� (param�terekb�l)								
		string mit = specific_word(node->elsofiu);
		//string mit = statement(node->elsofiu);
		if (eatNegatedModifier()) {
			return "nem v�gz�dik " + addHelyhatarozo(mit);
		}
		else {
			return  addHelyhatarozo(mit) + " v�gz�dik";
		}
	}
	string exact_number(SynthTreeNode *node) { //konstrukci� (param�terekb�l)								
		int nTimes = number(node);		
		return "pontosan " + to_string(nTimes);
	}
	string atMost(SynthTreeNode *node) { //konstrukci� (param�terekb�l)								
		int nTimes = number(node);
		return "legfeljebb " + to_string(nTimes);
	}
	string atLeast(SynthTreeNode *node) { //konstrukci� (param�terekb�l)								
		int nTimes = number(node);
		return "legal�bb " + to_string(nTimes);
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
			return "nem igaz, hogy " + n + " egym�s ut�ni " + addBelso(mi) + " �ll";
		}
		else {
			return n + " egym�s ut�ni " + addBelso(mi) + " �ll";
		}		
	}
	string and(SynthTreeNode *node) {
		string st1 = statement(node->elsofiu);
		string st2 = (	node->testver->symbol == 'E'
						? statement(node->testver)
						: statement(node->testver->elsofiu));	
		return st1 + " �s " + st2;
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
		node = node->testver; //az els�t skippelhetj�k hisz ugyis +1-es �s az els� bet�
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
		return  "p�ros sz�m�";
	}
	string odd_number(SynthTreeNode *node) {		
		return  "p�ratlan sz�m�";
	}
	string divisibleBy_number(SynthTreeNode *node) {
		int nTimes = number(node);		
		return  addTarsrag(to_string(nTimes + 1)) + " oszhat�"; // + 1 kell, hogy 2-rol induljon (HACK)
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
		string ret = string("minden ") + addTargyrag(string() + node->elsofiu->symbol) + " " + node->testver->elsofiu->symbol + string(" k�vet");
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
		string ret = string("b�rmely ") + to_string(everyN) + string(" egym�s ut�ni bet� tartalmaz ") + nTimes + string(" ") + letter + (" bet�t");
		if (eatNegatedModifier()) {
			return "nem igaz, hogy " + ret;
		}
		else {
			return ret;
		}
	}

	string letter(SynthTreeNode *node) {
		return string() + node->symbol + " bet�t";
	}

	string statement(SynthTreeNode *node) { //v�laszt�, (t�bb lehet�s�g)				
		switch (node->symbol) {
		case 'S':
			return specific_word(node->elsofiu); //ilyenkor az alf�ggv�nyek m�r mag�t az els� param�tert kapj�k, aminek a testv�rei a t�bbiek
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
			cout << "Nincs szab�ly megadva a k�vetkez�re: " << node->symbol << endl;
			return string();
		}
	}

	string buildSentence(SynthTreeNode * node) {
		if (node->symbol == 'W') {
			node = node->elsofiu;
		}
		negatedModifier = false;
		//itt a node m�r NEM a W-r�l indul, hanem ami az els�fia
		string abcList = "{";
		for (char c : abc) {
			abcList += c + string(",");
		}
		abcList.pop_back();
		abcList += "}";
		set<char> complex = { 'E','O','T','W' };
		string sentence = string("Adj meg olyan ") + abcList + " abc-j� automat�t amely a k�vetkez�t ismeri fel: sz�, ";
		if (node->symbol == 'T' && complex.count(node->elsofiu->elsofiu->symbol) > 0) { //ha tagad�ssal kezd�dik �s alatta �sszetett, de ilyen most az ujba m�r nincs (ink�bb kivettem)
			sentence += "amelyre nem igaz, hogy ";
			node = node->elsofiu->elsofiu; //attekri a nem-en
		}else {
			//meghatarozza az els� val�di �ll�t�s t�pus�t
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
	//toltad�lokok:	
	string addTargyrag(string szo) {
		switch (szo.back()) {
		case '0': return szo + "-t";
		case '1': return szo + "-et";
		case '2': return szo + "-t";
		case '3': return szo + "-at";
		case '4': return szo + "-et";
		case '5': return szo + "-�t";
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
		case '0': return szo + "-b�l";
		case '1': return szo + "-b�l";
		case '2': return szo + "-b�l";
		case '3': return szo + "-b�l";
		case '4': return szo + "-b�l";
		case '5': return szo + "-b�l";
		case 'a': return szo + "-b�l";
		case 'b': return szo + "-b�l";
		case 'c': return szo + "-b�l";
		default: return "error";
		}
	}
	bool eatNegatedModifier() {
		bool ret = negatedModifier;
		negatedModifier = false;
		return ret;
	}
	
}