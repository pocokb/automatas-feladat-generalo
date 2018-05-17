#include "stdafx.h"
#include "Tanulas.h"

ostream& operator<<(ostream& os, const Tanulas::Tapasztalat& t) {
	os << '(' << *(t.state) << ')' << ':' << t.input << "->" << t.output << endl;
	return os;
}
/*
string Tanulas::Game::interact(int input) {
		
}*/

Tanulas::Tanulas()
{
}


Tanulas::~Tanulas()
{
}
