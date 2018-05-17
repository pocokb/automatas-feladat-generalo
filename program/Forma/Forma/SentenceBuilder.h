#pragma once
#include"DFA.h"

class SentenceBuilder
{
public:
	SentenceBuilder();
	~SentenceBuilder();
};
namespace sentence{
	string buildSentence(SynthTreeNode *node);
	string addTargyrag(string szo);
	string addTarsrag(string szo);
	string addHelyhatarozo(string szo);
	string addBelso(string szo);
	bool eatNegatedModifier();
	string statement(SynthTreeNode *node);
	string contains(SynthTreeNode * node);
	string endsWith(SynthTreeNode * node);
}

