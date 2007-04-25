// Copyright 2007 Greg London
// All Rights Reserved


#include <iostream>
using namespace std;

#include "ParseNativeRuleRunner.h"

//int hello_world(ParseNativeRuleRunner &thus){
//	thus.hello_world();
//	return 1;
//}


int main()
{
	ParseNativeRuleRunner infstr;
	//infstr.AppendString("alpha bravo charlie delta");
	//infstr.dump_blocks();

	infstr.CompileGrammar("grammar.txt");

	infstr.DiscardCurrentString();

	infstr.DumpRulebook();


	//int (*callback)(void) = hello_world;
	//int (*callback)(ParseNativeRuleRunner &) = hello_world;

	//infstr.callback_hash["hello_world"] = callback;

	//cout << "filename" << endl;

	infstr.filename("plaintext.txt");

	cout << "RunRule" << endl;

	if(infstr.RunRule("myrule")) {
		cout << "PASSED" << endl;	
	} else {
		cout << "FAILED" << endl;
	}

	return 0;

}
