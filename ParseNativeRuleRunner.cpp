// Copyright 2007 Greg London
// All Rights Reserved



#include "ParseNativeRuleRunner.h"

///////////////////////////////////////////////////////////////////////
ParseNativeRuleRunner::ParseNativeRuleRunner(void){
///////////////////////////////////////////////////////////////////////

	//callback_hash["hello_world"] = ::hello_world;
}



///////////////////////////////////////////////////////////////////////
labeltype ParseNativeRuleRunner::ip_label(void){
///////////////////////////////////////////////////////////////////////
	instructiontype *ip = instruction_pointer_stack.top();
	return (*ip).label;
}


///////////////////////////////////////////////////////////////////////
opcodetype ParseNativeRuleRunner::ip_opcode(void){
///////////////////////////////////////////////////////////////////////
	instructiontype *ip = instruction_pointer_stack.top();
	return (*ip).opcode;
}

///////////////////////////////////////////////////////////////////////
void	ParseNativeRuleRunner::next_ip(){
///////////////////////////////////////////////////////////////////////
	instructiontype *ip = instruction_pointer_stack.top();
	instruction_pointer_stack.pop();
	ip++;
	instruction_pointer_stack.push(ip);
}

///////////////////////////////////////////////////////////////////////
int ParseNativeRuleRunner::RunRule(char * rule_name){
///////////////////////////////////////////////////////////////////////
cout << "running rule "<<rule_name;
	unsigned int rulenumber = rule_name_to_rule_number_converter[rule_name];
cout << " rulenumber is "<<rulenumber<<endl;
	int retval = RunRule(rulenumber);
cout << "retval is "<<retval<<endl;
	return retval;
}

///////////////////////////////////////////////////////////////////////
int ParseNativeRuleRunner::RunRule(unsigned int rulenumber){
///////////////////////////////////////////////////////////////////////
	instructiontype *instructionptr = rule_catalogue[rulenumber];

	if(instructionptr == NULL) {
		cout << "ERROR: No rule defined for '" << rulenumber << "'" << endl;
		return 0;
	}

	instruction_pointer_stack.push(instructionptr);
	rule_call_number_stack.push(rulenumber);

	int retval; 

	labeltype	label  ;
	opcodetype	opcode ;

	while( (ip_label() != COMMAND) || ((ip_opcode()).command != ';') ){

		label = ip_label();
		opcode = ip_opcode();

		if(label != COMMAND) {
			cout << "ERROR: expected opcode in rule "<<rulenumber<<endl;
		}

		advance_past_whitespace();

		//cout << "opcode.command is "<< opcode.command<<endl;

		switch(opcode.command) {
			case '\'': retval = run_literal_characters()	; break;
			case '[' : retval = run_character_class()	; break;
			case '~' : retval = run_shortcut()		; break;
			case '#' : retval = run_quantifier()		; break;
			case '|' : retval = run_alternation_start()	; break;
			case '%' : retval = run_capturing_parens()	; break;
			case '<' : retval = run_callback()		; break;
			case '&' : retval = run_rulecall()		; break;
			case '.' : retval = run_skip_whitespace()	; break;
			//case ')' : retval = run_close_paren()		; break;
			default  : cout << "unknown" << endl; return 0	; break;
		}
		if(retval==0) {
			instruction_pointer_stack.pop();
			rule_call_number_stack.pop();
			return 0;
			}
	}

	// we've hit the COMMAND ';' (i.e. a "return" command),
	// so the rule was able to complete successfully.
	// return a 1 to reflect success.
	instruction_pointer_stack.pop();
	rule_call_number_stack.pop();
	return 1;
}


///////////////////////////////////////////////////////////////////////
int ParseNativeRuleRunner::run_literal_characters(){
///////////////////////////////////////////////////////////////////////
	next_ip();

	labeltype	label  = ip_label();
	opcodetype	opcode = ip_opcode();

	next_ip();

	char	letter;
	char	*letters;

	if(label == LETTERS) {
		letters = opcode.letters;

		while(*letters) {
			letter = *letters;
			if(CurrentCharacter() != letter) {
				return 0;
			}
			IncrementMarker();
			letters++;
		}

		return 1;
	} else if (label == LETTER) {
		letter = opcode.letter;

		if(CurrentCharacter() != letter) {
			return 0;
		}
		IncrementMarker();
		return 1;
		
	} else {
		cout << "ERROR: bad grammar for literal character" << endl;
		return 0;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////
int ParseNativeRuleRunner::run_character_class(void){
///////////////////////////////////////////////////////////////////////

	next_ip();

	labeltype	label  = ip_label();
	opcodetype	opcode = ip_opcode();

	next_ip();

	if(label != CHARLUT) {
		cout << "ERROR: Bad grammar for character class" << endl;
		return 0;
	}

	bool *charlut = opcode.charlut;

	if(charlut[CurrentCharacter()]) {
		IncrementMarker();
		return 1;
	} else {
		return 0;
	}

	return 0;
}


///////////////////////////////////////////////////////////////////////
int ParseNativeRuleRunner::run_shortcut(void){
///////////////////////////////////////////////////////////////////////

	return 0;
}


///////////////////////////////////////////////////////////////////////
int ParseNativeRuleRunner::run_quantifier(void){
///////////////////////////////////////////////////////////////////////
	next_ip();	// get past the '#'. will be pointing at first number.

	// this is the first number
	if( ip_label() != INTEGER ) {
		cout << "ERROR: quantifier expected integer"<<endl;
		return 0;
	}

	unsigned int lowcount = ip_opcode().integer;

	next_ip();	// move past the first number. will be pointing at second number.

	// this is the second number
	if( ip_label() != INTEGER ) {
		cout << "ERROR: quantifier expected integer"<<endl;
		return 0;
	}

	unsigned int highcount = ip_opcode().integer;

	next_ip(); 	// move past second number, will be pointing at greedy/thrifty


	if( ip_label() != LETTER ) {
		cout << "ERROR: quantifier expected letter"<<endl;
		return 0;
	}

	char greedy_or_thrifty = ip_opcode().letter;

	next_ip(); 	// move past the greedy/thrifty character. will be pointing at rule call

	if( ip_label() != COMMAND ) {
		cout << "ERROR: quantifier expected Command for rule call"<<endl;
		return 0;
	}

	next_ip(); 	// move past the '&'. will be pointing at rule number

	if( ip_label() != INTEGER ) {
		cout << "ERROR: quantifier expected rule number"<<endl;
		return 0;
	}

	unsigned int quantizationrulenumber = ip_opcode().integer;

	next_ip(); 	// move past the '&'. will be pointing at rule number

	if( ip_label() != COMMAND ) {
		cout << "ERROR: quantifier expected Command for rule call"<<endl;
		return 0;
	}

	next_ip(); 	// move past the '&'. will be pointing at rule number

	if( ip_label() != INTEGER ) {
		cout << "ERROR: quantifier expected rule number"<<endl;
		return 0;
	}

	unsigned int postquantrulenumber = ip_opcode().integer;

	next_ip(); 	// move past the rule number. will be pointing at next command in rule

	// have rulenumbers, greedy/thrifty, and high and low count. go do it.
	SaveCurrentMarkerPosition();

	// { lowcount, highcount }

	int retval;

	if(greedy_or_thrifty == 'g') {

		retval = run_quantifier_greedy( 
			highcount, lowcount, quantizationrulenumber, postquantrulenumber );

	} else {

		retval = run_quantifier_thrifty( 
			highcount, lowcount, quantizationrulenumber, postquantrulenumber );

	}

	// if we failed, then go back to where we came from
	if(retval==0) {
		RestoreMostRecentlySavedMarkerPosition();
	} else {
		// else, throw away old marker and start at the current spot.
		DiscardMostRecentlySavedMarkerPosition();
	}

	return retval;
}




///////////////////////////////////////////////////////////////////////
int ParseNativeRuleRunner::run_quantifier_greedy
			(unsigned int highcount, 
			unsigned int lowcount, 
			unsigned int quantizationrulenumber, 
			unsigned int postquantrulenumber){
///////////////////////////////////////////////////////////////////////

	ParseNativeMarkerType marker;
	stack<ParseNativeMarkerType> markerstack;

	unsigned int count = 0;
	bool keeptrying = true;

	while((count<highcount) && keeptrying) {
		count++;
		keeptrying = RunRule(quantizationrulenumber);
		if(keeptrying) {
			markerstack.push(GetCurrentMarkerPosition());
		}
	}
	if(!keeptrying) {
		count--;
	}

	// now, see if we at least got our minimum
	if(count<lowcount){
		return false;
	}
		
	// and then some magic happens
	// call the postquantrulenumber and see if it works.
	bool postquantpassed;
	while(markerstack.size()) {
		marker=markerstack.top();
		markerstack.pop();

		//cout <<"Trying marker \n";
		//DumpMarker(marker);

		postquantpassed = RunRule(postquantrulenumber);

		if(postquantpassed) {
			return true;
		}
	}

	cout << "quantifier rule failed\n";
	return false;
}




///////////////////////////////////////////////////////////////////////
int ParseNativeRuleRunner::run_quantifier_thrifty
			(unsigned int highcount, 
			unsigned int lowcount, 
			unsigned int quantizationrulenumber, 
			unsigned int postquantrulenumber){
///////////////////////////////////////////////////////////////////////
	// more magic for thrifty mode.
	unsigned int count=0;
	while(count<highcount) {
		count++;

		// if quantization rule passed
		if( RunRule(quantizationrulenumber) ) {
			if(count>=lowcount) {
				// if post quant rule passed, 
				if(RunRule(postquantrulenumber)) {
					return true;
				}
			}
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////
int ParseNativeRuleRunner::run_alternation_start(void){
///////////////////////////////////////////////////////////////////////
	next_ip();	// get past the '|'. will be pointing at first rule call alternate

	bool retval=false;

	while( (retval==false) && ((ip_label() != COMMAND) || (ip_opcode().command != ')')) ){
		next_ip();	// get past the '&'
		unsigned int rulenumber = ip_opcode().integer;
		next_ip();
		retval = RunRule(rulenumber);
	}

	// in case an early alternate passed, need to skip the other alternates 
	// move forward till we hit a ')';
	while( (ip_label() != COMMAND) || (ip_opcode().command != ')') ) {
		next_ip();	
	}

	// now, skip the ')'
	next_ip();	

	return retval;
}


///////////////////////////////////////////////////////////////////////
int ParseNativeRuleRunner::run_capturing_parens(void){
///////////////////////////////////////////////////////////////////////

	return 0;
}


///////////////////////////////////////////////////////////////////////
int ParseNativeRuleRunner::run_callback(void){
///////////////////////////////////////////////////////////////////////

	cout << "run callback" << endl;
	next_ip();

	labeltype	label  = ip_label();
	opcodetype	opcode = ip_opcode();

	next_ip();

	char	*letters;

	if(label == LETTERS) {
		letters = opcode.letters;
		int (*callback)(ParseNativeRuleRunner&) = callback_hash[letters];

		int retval = (*callback)(*this);

		return retval;
	}

	return 0;
}


///////////////////////////////////////////////////////////////////////
int ParseNativeRuleRunner::run_rulecall(void){
///////////////////////////////////////////////////////////////////////
	next_ip();

	unsigned int rulenumber = ip_opcode().integer;

	next_ip();

	//cout << "ParseNativeRuleRunner::run_rulecall is calling rule number "<<rulenumber<<endl;
	bool retval = RunRule(rulenumber);

	return retval;
}


///////////////////////////////////////////////////////////////////////
int ParseNativeRuleRunner::run_skip_whitespace(void){
///////////////////////////////////////////////////////////////////////

	return 0;
}


///////////////////////////////////////////////////////////////////////
int ParseNativeRuleRunner::hello_world(void){
///////////////////////////////////////////////////////////////////////
	cout << "HELLO WORLD THIS IS THE METHOD CALL" << endl;
	return 0;
}
