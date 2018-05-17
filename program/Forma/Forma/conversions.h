#pragma once
#include"DFA.h"
#include"language.h"

Language DFAToRegLang(DFA m);
DFA RegLangToDFA(Language rl);
DFA foldDFAonDFA(DFA frame, DFA entity);