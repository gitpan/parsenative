// Copyright 2007 Greg London
// All Rights Reserved

#include "ParseNativeRuleCompiler.h"

#include <iostream>
#include <stdio.h>
using namespace std;

// 0-255, plus 256=> \n
const unsigned int CHARACTER_CLASS_DEPTH = 257;



ParseNativeRuleCompiler::ParseNativeRuleCompiler() {
	// rule '0' is undefined.
	rule_catalogue.push_back(NULL);
	rule_number_to_rule_name_converter.push_back(NULL);


	/*
	char *nameptr = new char [100];

	int i;

	strcpy(nameptr,"bongo"); i = rule_name_to_rule_number_converter[nameptr];
	cout << nameptr<<" is "<<i<<endl;
	strcpy(nameptr,"logo"); i = rule_name_to_rule_number_converter[nameptr];
	cout << nameptr<<" is "<<i<<endl;
	strcpy(nameptr,"bongo"); i = rule_name_to_rule_number_converter[nameptr];
	cout << nameptr<<" is "<<i<<endl;

	strcpy(nameptr,"bongo"); rule_name_to_rule_number_converter[nameptr]=42;
	strcpy(nameptr,"logo"); rule_name_to_rule_number_converter[nameptr]=91;

	strcpy(nameptr,"bongo"); i = rule_name_to_rule_number_converter[nameptr];
	cout << nameptr<<" is "<<i<<endl;
	strcpy(nameptr,"logo"); i = rule_name_to_rule_number_converter[nameptr];
	cout << nameptr<<" is "<<i<<endl;
	*/
	
}



/***************************************************************

The infinite string parser uses a rather different approach
to parsing. The "grammar" is a string that the parser takes
and formats into something that can be executed as if it
were a series of instructions.

Think of it as assembly language in the form of a character array.

The allowed grammar is defined here. Note that it looks more 
like a grammar than a regular expression, although it could
be used to implement regular expression searching as well.

Some fundamental differences:

	Implied \G Anchor at the start of all rules:
		All tokens imply a \G anchor at the start.
		This means that by default, the rule must 
		match starting from the current location
		in the string being parsed.

		If you want to look anywhere in the string
		for a match, you must explicitely state it
		with some other command.

	Whitespace Ignored:
		The grammar is whitespace independent.
		If you want a literal space, use ' '.

		The text being parsed is whitespace independent
		as well. Tokens in the grammar may be separated
		by whitespace in the text and still match.

		If you are looking for 'ab', then don't 
		put 'a' 'b' in your grammar, because that 
		will match 'a b' and even 'a (huge comment) b'.

		If you want to control whitespace skipping,
		use the '.' command

The basic command set:

	'	(single quote) 

		examples: 	'hello' 'world'

		Indicates the start of a literal sequence.
		Sequence will continue until the second 
		single quote is encountered. If not on the
		same line, then parser will report a grammar
		error.

		if you want a long sequence, you can break it up
		into smaller pieces in literal quotes and turn
		off whitespace skipping with the ! command.

	[	(left square bracket)

		examples:	[aeiou] [0-9a-f]

		Indicates the start of a character class.
		Character classes are defined the same as perl.


	~	(tilde)

		examples:	~w ~W ~d ~D ~s ~S

		indicates a shortcut character class.
		Note this is same as '\', but safer from misinterpretation.


	#	(the number symbol)

		examples:	#{*}(...) #{+}(...) #{?}(...) 
				#{3}(...) #{5,}(...) #{9,12}(...)
				#{3,11}g(...) #{*}t(...) 

		Indicates the start of a Quantifier specification.
		Note that quantifiers are specified before the 
		thing being quantified, not after.

		The '#' is followed by some form of quantification,
		and the modes match the same as perl:

			*	0 or more
			+	1 or more
			?	0 or 1
			{3}	exactly 3
			{5,}	5 or more
			{9,12}	9 to 12, inclusive

		numbers in curly braces default to decimal.
		To specify a base, use a leading 0 to indicate octal
		and a leading 0x to indicate hex 
		

		The last piece of the quantifier can include 'g' or 't'
		to indicate greedy or thrifty matching.

			#{*}g(...)
			#{+}t(...)
			#{9,12}t(...)

		Quantifiers default to being greedy.

		After the quantifier, the thing being quantified
		is contained within parenthesis.

	|	(vertical pipe)

		examples:	|( ... , ... , ... , ... )

		Indicates the start of an ALTERNATION specification.
		Note that the separator is a comma (,) rather than 
		a vertical pipe (|) as used in perl regexps.
		The comma is used to indicate that the seperator is 
		lowest precedence in evaluation.

	.	(period)

		examples: 	'hello' .! ',' .* 'world'

		Whitespace skipping control.

		This controls skipping whitespace in the text being
		parsed, not the grammar itself. Whitespace in the
		grammar is always skipped.


	%	(percent sign)
	
		examples:	%{id}( ... )
				%{name}( #{+}(~w) )

		Indicates the start of a CAPTURING parenthesis.
		The curly braces contains a name that will be used
		to retrieve whatever was captured.

		(mneumonic: the name will be the key into a hash.
			and '%' is a hash sigil in perl)

	<	(the "less than" symbol)

		examples:	<name>

		Indicates a function callback. Function will be called
		any time the text parser reaches this point in grammar,
		even if the rules turn out to fail later on.

		Note that at the end of a rule or subrule,
		(either upon successful complete of the rule
		 or when the rule exits due to failure)
		a rule-specific callback will be called. 

		This <callback> support is provided to give the
		user extra functionality within a rule.

		Also note that the <name> token can only contain
		the ~w characters between '<' and '>'.


	:	( colon )

		examples:	: identifier := [a-zA-Z] Q+(~w) ;

		Indicates the start of a named rule declaration.
		rules can be referenced in the grammar by barewords.
		For example, after the above declaration, another 
		rule could reference the identifier rule like this:

				: expression := identifier [+-*\/] identifier ;

		Note that the rule declaration starts with ':'
		followed by the name of the rule, followed by ':=',
		followed by the rule definition, followed by ';'.

		It is a little more verbose, but complicated grammars
		can use a little verbosity.

		


***************************************************************/



///////////////////////////////////////////////////////////////////////
void ParseNativeRuleCompiler::advance_past_whitespace(void){
///////////////////////////////////////////////////////////////////////

	char single_char;
	bool	over_whitespace=true;

	while( over_whitespace ) {

		single_char = CurrentCharacter();

		if(single_char==0) { return; }

		if(	(single_char == ' ')
		|| 	(single_char == '\t')
		|| 	(single_char == '\n')
		|| 	(single_char == 10)
		|| 	(single_char == 13)
		) {

			IncrementMarker();
		} else {
			over_whitespace = false;
		}
	}

}		

///////////////////////////////////////////////////////////////////////
int ParseNativeRuleCompiler::get_numeric_index_for_named_rule(char *rule_name){
///////////////////////////////////////////////////////////////////////
	cout << "ParseNativeRuleCompiler::get_numeric_index_for_named_rule, rule_name is "<<rule_name<<endl;
	int numeric_index;

	// if it exists (rulenumber not equal to zero)
	if(rule_name_to_rule_number_converter[rule_name]) {
		numeric_index = rule_name_to_rule_number_converter[rule_name];
		cout << "rule "<< rule_name<<" already defined. Overwriting. ";
		cout << "Rulenumber is "<< numeric_index<<endl;
	} else {
		numeric_index = rule_catalogue.size();
		cout << "ParseNativeRuleCompiler::get_numeric_index_for_named_rule "<< rule_name ;
		cout << " rulenumber is "<< numeric_index << endl;
		rule_catalogue.push_back(NULL);
		rule_name_to_rule_number_converter[rule_name]=numeric_index;

		// need to make a dynamically allocated string to store the rulename permanently.
		// pointer coming in is used by rest of code to point to other stuff.
		char *final_rule_name = new char [strlen(rule_name)+1];
		strcpy(final_rule_name, rule_name);

		rule_number_to_rule_name_converter.push_back(NULL);
		rule_number_to_rule_name_converter[numeric_index]=final_rule_name;
	}
	return numeric_index;
}

///////////////////////////////////////////////////////////////////////
void ParseNativeRuleCompiler::start_compiling_a_new_anonymous_inline_rule(void){
///////////////////////////////////////////////////////////////////////
	cout << "ParseNativeRuleCompiler::start_compiling_a_new_anonymous_inline_rule\n";
	int numeric_index = rule_catalogue.size();
	rule_catalogue.push_back(NULL); // allocate the space 
	rule_number_to_rule_name_converter.push_back(NULL);
	rule_number_to_rule_name_converter[numeric_index]="anon";

	// anon rule for next alternate.
	add_to_rule_command(':');
	add_to_rule_integer(numeric_index);

	stack_of_current_rules_being_compiled.push(numeric_index);
}



///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
// add_to_rule_raw explanation:
// as rules are being compiled, they are pushed onto the top of the
// rule_in_progress vector. We may start a named rule, and then need to
// declare an anonymous rule inside of the named rule. the anon rule 
// will be pushed onto the vector on top of the named rule.
// when the anon rule is finished, the anon rule will be popped off
// the rule_in_progress vector and put into the rule_catalogue. 
// The rule_in_progress will receive a '&' (rulenumber) instruction
// where the anon rule was just located.
//
// rule_in_progress over time as we compile :namedrule:= |( 'a', 'b') ;
// :42			// declaring rule. "namedrule" will be stored in slot 42 of catalogue.
// :42 | 		// start of alternation
// :42 | :43		// first alternate is an anon rule
// :42 | :43 'a 	// anon rule looking for literal 'a'
// :42 | :43 'a ret	// end of first anon rule. pop it off and put it in catalogue.
// :42 | &43		// first alternate is now a rule call to rulenumber 43
// :42 | &43 :44	// second alternate is another anonymous rule
// :42 | &43 :44 'b	// anon rule looking for a literal 'b'
// :42 | &43 :44 'b ret	// end of second anon rule. pop it off and put it in catalogue.
// :42 | &43 &44	// second alternate is now a rule call to rulenumber 44
// :42 | &43 &44 )	// end of alternation command sequence.
// :42 | &43 &44 ) ret	// end of namedrule declaration, pop it off and put in catalogue
// (empty)		// rule_in_progress now empty waiting for next rule declaration
//
// as we add to rule, just keep adding to top of rule_in_progress.
// if we add a "return", then we can always pop off the top most rule
// and store it in the catalogue.
//
// the add_to_rule_return will return the rule number of the rule it just catalogued.
// The code that called add_to_rule_return will decide whether to push the rule number
// onto the rule_in_progress or not.
//
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
void ParseNativeRuleCompiler::add_to_rule_raw(labeltype label, opcodetype opcode){
///////////////////////////////////////////////////////////////////////
	instructiontype thingy;
	thingy.label = label;
	thingy.opcode = opcode;

	if(stack_of_current_rules_being_compiled.size()==0) {
		cout << "ERROR: ParseNativeRuleCompiler::add_to_rule_raw cant add to rule if ";
		cout << "stack_of_current_rules_being_compiled is empty \n";
		return;
	}

	rule_in_progress.push_back(thingy);
}



void ParseNativeRuleCompiler::add_to_rule_command(char command){
	opcodetype opcode;
	opcode.command = command;
	add_to_rule_raw(COMMAND,opcode);
}

void ParseNativeRuleCompiler::add_to_rule_letter(char letter){
	opcodetype opcode;
	opcode.letter = letter;
	add_to_rule_raw(LETTER,opcode);
}


void ParseNativeRuleCompiler::add_to_rule_letters(char* letters){
	opcodetype opcode;
	opcode.letters = letters;
	add_to_rule_raw(LETTERS,opcode);
}


void ParseNativeRuleCompiler::add_to_rule_charlut(bool* charlut){
	opcodetype opcode;
	opcode.charlut = charlut;
	add_to_rule_raw(CHARLUT,opcode);
}


void ParseNativeRuleCompiler::add_to_rule_integer(unsigned int integer){
	opcodetype opcode;
	opcode.integer = integer;
	add_to_rule_raw(INTEGER,opcode);
}


int ParseNativeRuleCompiler::add_to_rule_return(void){
	if(stack_of_current_rules_being_compiled.size()==0) {
		cout << "ERROR: ParseNativeRuleCompiler::add_to_rule_return ";
		cout << "called with no rules on stack" << endl;
		IncrementMarker();
		advance_past_whitespace();
		return -1;
	}


	opcodetype opcode;
	opcode.letter = ';';
	add_to_rule_raw(COMMAND,opcode);

	//DumpRuleInProgress();

	// now, pop the entire rule off the rule_in_progress vector
	// and put it in the catalogue.

	instructiontype instruction;

	vector<instructiontype> temprule;
	// keep popping off the rule in prgress and pushing onto temprule
	// until we see a ':' command
	while(
		!( ((rule_in_progress.back()).label == COMMAND) 
		&& ((rule_in_progress.back()).opcode.command == ':')
		 )
		&&  (rule_in_progress.size()>0)
	) {
		instruction=rule_in_progress.back();
		rule_in_progress.pop_back();

		temprule.push_back(instruction);
		//cout << "while loop popping off instruction "; DumpInstruction(instruction);
	}
	
	// pop off the ':' and the rule number
	// because we won't store them in the memory.
	// keep the rulenumber of this rule.
	instruction = rule_in_progress.back(); rule_in_progress.pop_back();
	//cout << "popped off rule in progress instruction "; DumpInstruction(instruction);
	instruction = temprule.back(); temprule.pop_back();
	//cout << "popped off temprule instruction "; DumpInstruction(instruction);
	int rulenumber = instruction.opcode.integer;

	// now create a dynamic array and put the temprule in it
	int size = temprule.size();
	instructiontype *rulestorage = new instructiontype [size];

	int offset = 0;
	while(temprule.size()) {
		instruction = temprule.back(); temprule.pop_back();
		//cout << "offset is "<< offset <<" ";  DumpInstruction(instruction);
		rulestorage[offset++] = instruction;
	}
	
	//cout << "rulenumber is "<<rulenumber<<endl;
	//cout << "rule_catalogue.size() is "<<rule_catalogue.size()<<endl;

	rule_catalogue[rulenumber]=rulestorage;

	// not compiling this rule anymore. pop it off stack.
	stack_of_current_rules_being_compiled.pop();

	return rulenumber;
}


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
// methods to deal with open and close parenthesis
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////
// NOTE THAT IT IS UP TO CALLER TO INCREMENT POINTER AND MOVE PAST WHITESPACE
///////////////////////////////////////////////////////////////////////
void ParseNativeRuleCompiler::track_start_paren(char command){
///////////////////////////////////////////////////////////////////////
	parenstack.push(command);
}


///////////////////////////////////////////////////////////////////////
// close paren method needs to see if the open paren was an alternation
// if it was, then it needs to close the alternation too.
///////////////////////////////////////////////////////////////////////
void ParseNativeRuleCompiler::track_close_paren(void){
///////////////////////////////////////////////////////////////////////
	if(parenstack.size()==0) {
		cout << "ERROR: close paren, without an open paren\n";
		IncrementMarker();
		advance_past_whitespace();
		return;
	}
		

	char most_recent_paren = parenstack.top(); parenstack.pop();

	// if this is a close paren for an alternate, then 
	// need to clean up the last alternate which is an anon rule.
	if(most_recent_paren == '|') {
		int rulenumber = add_to_rule_return(); // pop off this rule and catalogue it 
		add_to_rule_command('&');
		add_to_rule_integer(rulenumber);
		add_to_rule_command(')');

	} else if (most_recent_paren =='%') {
		int rulenumber = add_to_rule_return(); // pop off this rule and catalogue it 
		add_to_rule_command('&');
		add_to_rule_integer(rulenumber);

	} else if (most_recent_paren=='#') {
		// 
		int rulenumber = add_to_rule_return(); // pop off this rule and catalogue it 
		add_to_rule_command('&');
		add_to_rule_integer(rulenumber);

		// now, everything from here to the end of this rule is an anonymous rule.
		// this anonymous rule is the last parameter passed into the quantification command
		start_compiling_a_new_anonymous_inline_rule();

		// when we hit the full return ';', we need to pop this R,
		// close the anon rule, and make it the second rule call 
		// to the quantification command. push a 'R' onto paren stack
		// to keep track of this.
		parenstack.push('R');

	}
	IncrementMarker();
	advance_past_whitespace();

}




///////////////////////////////////////////////////////////////////////
void ParseNativeRuleCompiler::track_alternate_commas(void) {
///////////////////////////////////////////////////////////////////////
	char most_recent_paren = parenstack.top();

	if(most_recent_paren != '|') {
		cout << "ERROR: alternatation comma, without alternation ('|') \n";
		return;
	}

	// comma indicates the previous anon rule is finished.
	// add a return, which will get the anon rule off the rule in progress
	// and cataloge it.
	int rulenumber = add_to_rule_return();

	// now take the rulenumber of that last anon rule alternate,
	// and put it in rule in progress as a rulecall.
	// alternate looks like 
	//	| &43 &44 &45 )
	add_to_rule_command('&');
	add_to_rule_integer(rulenumber);


	// start a new anon rule for the next alternation.
	start_compiling_a_new_anonymous_inline_rule();

	IncrementMarker();
	advance_past_whitespace();

}



///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
// methods for compiling specific symbols.
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////
int ParseNativeRuleCompiler::CompileGrammar(char * filename_containing_grammar) {
///////////////////////////////////////////////////////////////////////

	char single_char;

	filename(filename_containing_grammar);

	/*********************************************
	SaveCurrentMarkerPosition();
	while( single_char = CurrentCharacter() ) {
		cout << single_char << endl;
		IncrementMarker();
	}
	dump_blocks();
	return 0;
	*********************************************/


	advance_past_whitespace();

	while( single_char = CurrentCharacter() ) {
		
		cout << "single_char is "<< single_char << endl;
		//cout << "decimal is "<<(int)single_char<<endl;
		switch(single_char) {

		/////////////////////////////////////////////////////
		case '\'': // literal character sequence
			compile_character_sequence();
		break;

		/////////////////////////////////////////////////////
		case ESCAPE_CHARACTER: 
			compile_escaped_character();
		break;	// break for case ESCAPE_CHARACTER:

		/////////////////////////////////////////////////////
		case '[': // user defined character class
			compile_character_class();
		break;

		/////////////////////////////////////////////////////
		case '#':
			compile_quantifier();
		break;

		/////////////////////////////////////////////////////
		case '|':
			compile_alternation();
		break;

		/////////////////////////////////////////////////////
		case ',':	// alternates are separated by commas
			track_alternate_commas();
		break;
	
		/////////////////////////////////////////////////////
		case '%':
			compile_capturing_parens();
		break;

		/////////////////////////////////////////////////////
		case '<':
			compile_callback_function();
		break;

		/////////////////////////////////////////////////////
		case '.':	
			compile_whitespace_skipping();
		break;

		/////////////////////////////////////////////////////
		case ')':	// close paren from one of the above commands
			track_close_paren();
		break;

		/////////////////////////////////////////////////////
		case ':':	
			track_start_of_new_named_rule();
		break;
	
		/////////////////////////////////////////////////////
		case ';':	
			track_end_of_new_rule();
		break;
	
		/////////////////////////////////////////////////////
		default:
			// if it isn't a rule call, then error.
			if(!compile_rule_call()) {
				cout << "not sure what to do with '";
				cout << single_char << "'. Skipping it." << endl;
				IncrementMarker();
				advance_past_whitespace();
			}

		break;
		} // end switch(single_char) {


	}
	if(parenstack.size()!=0) {
		cout << "ERROR: Open paren did not have corresponding close paren" << endl;
	}


	//show_rule();

	return 1;

}

///////////////////////////////////////////////////////////////////////
// see if we can compile this as a rule call.
// rules must start with a letter or an underscore.
// if not a rule, then return false.
// if letter/underscore, compile it as a rule and return true.
///////////////////////////////////////////////////////////////////////
bool ParseNativeRuleCompiler::compile_rule_call(void){
///////////////////////////////////////////////////////////////////////
	char rulename[1000];
	int i;

	char single_char = CurrentCharacter();
	if(!(
		((single_char>='A')&&(single_char<='Z'))
	||	((single_char>='a')&&(single_char<='z'))
	||	 (single_char=='_')
	)) { 
		return false;
	}

	i=0;
	rulename[i++] = single_char;
	IncrementMarker();

	while(character_class_for_w[(int)CurrentCharacter()]) {
		rulename[i++] = CurrentCharacter();
		IncrementMarker();
	}

	rulename[i++]=0;

	add_to_rule_command('&');

	int rulenumber = get_numeric_index_for_named_rule(rulename);

	add_to_rule_integer(rulenumber);

	advance_past_whitespace();

	return true;
}


///////////////////////////////////////////////////////////////////////
void ParseNativeRuleCompiler::compile_whitespace_skipping(void){
///////////////////////////////////////////////////////////////////////

	add_to_rule_command('.');

	IncrementMarker();

	add_to_rule_command(CurrentCharacter());

	IncrementMarker();
	advance_past_whitespace();

}



///////////////////////////////////////////////////////////////////////
void ParseNativeRuleCompiler::compile_character_sequence(void){
///////////////////////////////////////////////////////////////////////
	add_to_rule_command('\'');
	IncrementMarker();

	char temp[1000];
	int i=0;
	char *dynamic_pointer;

	while( CurrentCharacter() && (CurrentCharacter() != '\'') && (CurrentCharacter() != '\n')) {
		temp[i++]=CurrentCharacter();
		IncrementMarker();

	}

	if(CurrentCharacter() == '\'') {
		IncrementMarker();
	} else if(CurrentCharacter() == '\n') {
		cout << "ERROR: multiline string not supported" << endl;
		exit(1);
	} else if(CurrentCharacter() == 0) {
		cout << "ERROR: premature end of file" << endl;
		exit(1);
	}

	temp[i++]=0;

	dynamic_pointer = new char [i+1];
	strcpy(dynamic_pointer, temp);

	add_to_rule_letters(dynamic_pointer);

	advance_past_whitespace();

}


/////////////////////////////////////////////////////
//	~w ~W ~d ~D ~s ~S
/////////////////////////////////////////////////////
void ParseNativeRuleCompiler::compile_escaped_character(void) {
	// start out pointing at escape character
	// increment pointer, but do not skip whitespace.
	// if user escapes a space ' ' then they want a literal space.
	IncrementMarker();
	char escaped_char = CurrentCharacter();

	/////////////////////////////////////////////
	switch(escaped_char) {

	// note that for shortcut classes such as `d or \d, 
	// we don't need to store the instruction as a 
	// character class in the executable memory. 
	// the predefined shortcuts have LUT's that are
	// part of this C++ package. Dont need to put them
	// in the executable memory.
	case 'd':
		add_to_rule_command('~');
		add_to_rule_letter('d');
	break;
	case 'D':
		add_to_rule_command('~');
		add_to_rule_letter('D');
	break;
	case 'w':
		add_to_rule_command('~');
		add_to_rule_letter('w');
	break;
	case 'W':
		add_to_rule_command('~');
		add_to_rule_letter('W');
	break;
	case 's':
		add_to_rule_command('~');
		add_to_rule_letter('s');
	break;
	case 'S':
		add_to_rule_command('~');
		add_to_rule_letter('S');
	break;

	// user escaped a non-escape character
	// interpret as a regular literal character.
	default:
		add_to_rule_command('\'');
		add_to_rule_letter(escaped_char);

		cout << "bad meta character '" ;
		cout << escaped_char << "'" << endl;	
		exit (1);
	break;
	};


	IncrementMarker();
	advance_past_whitespace();
}


/////////////////////////////////////////////////////
//		examples:	[aeiou] [0-9a-f]
/////////////////////////////////////////////////////
void ParseNativeRuleCompiler::compile_character_class(void) {
	// character class is something like [a-f0-9].
	// expression pointer should be pointing to first '['.
	cout << "processing character class" << endl;

	// need to keep a copy of character class string for
	// hash table key. will also append it to rule at end of function.

	IncrementMarker(); 
	advance_past_whitespace();

	// keep track of where the character class declaration started
	unsigned int start_of_character_class = 0;

	char single_char;
	bool *userclass = new bool [256];

	bool set_to_this_when_present;
	// look at first character in class.
	// if it's '^', then this is a NEGATIVE CLASS
	// otherwise it's a POSITIVE CLASS.
	if(CurrentCharacter() == '^') {
		set_to_this_when_present = false;
		IncrementMarker(); // skip over '^'
		advance_past_whitespace();
	} else {
		set_to_this_when_present = true;
	}

	// initialize the character class based on 
	// whether it is POSITIVE or NEGATIVE
	for(int i=0; i<256; i++) {
		userclass[i]=!set_to_this_when_present;
	}

	// now check to see if first char is ']'
	// which means ']' is part of class,
	// not the end of the character class definition
	// example: [][] <== this character class is actually the characters '[' and ']'
	if (CurrentCharacter() == ']') {
		userclass[']'] = set_to_this_when_present;
		IncrementMarker(); // skip over ']'
		advance_past_whitespace();
	}

	// now move forward until we hit ']'
	// and put every character in userclass
	while(single_char = CurrentCharacter()) {
		if(single_char == ']') {
			break; 
		}

		// if it's an escaped character, probably something like \w or \s
		if(single_char==ESCAPE_CHARACTER) {
			// increment pointer, but don't skip whitespace.
			// if user escapes a ' ', then put a space in character class
			IncrementMarker(); 
			char escaped_char = CurrentCharacter();

			switch(escaped_char) {
			case 'd':
				compile_character_class_copy_class( userclass,
					set_to_this_when_present, character_class_for_d);
			break;
			case 'D':
				compile_character_class_copy_class( userclass,
					set_to_this_when_present, character_class_for_D);
			break;
			case 'w':
				compile_character_class_copy_class( userclass,
					set_to_this_when_present, character_class_for_w);
			break;
			case 'W':
				compile_character_class_copy_class( userclass,
					set_to_this_when_present, character_class_for_W);
			break;
			case 's':
				compile_character_class_copy_class( userclass,
					set_to_this_when_present, character_class_for_s);
			break;
			case 'S':
				compile_character_class_copy_class( userclass,
					set_to_this_when_present, character_class_for_S);
			break;

			// note that \n may be two characters in a sequence.
			// want to be able to handle that separately.
			case 'n':	
				userclass[256]=set_to_this_when_present;
			break;

			case ' ':	
				userclass[' ']=set_to_this_when_present;
			break;

			// user escaped a non-escapable character.
			// `( or \( is just '('
			default: 
				userclass[escaped_char]=set_to_this_when_present;
			break;
			}

		// not an escape character, then its just a regular character like 'a'
		} else {
			// it's not an escape character, its a normal character.
			// look ahead and see if it's a character range.
			// if its a character range, i.e. [ ... 0-9 ... ]
			// the 0-9 is a range. need to get all the characters
			// in that range of characters
			
			SaveCurrentMarkerPosition();
			IncrementMarker();
			advance_past_whitespace();
			if (CurrentCharacter() == '-') {
				DiscardMostRecentlySavedMarkerPosition();
				IncrementMarker(); // pointing to '9' now
				advance_past_whitespace();
				char last_char = CurrentCharacter();

				char i;
				for(i=single_char; i<=last_char; i++) {
					userclass[i] = set_to_this_when_present;
				}

			// if next char is not '-', then treat this as 
			// a normal, single character in the class. i.e. [ .. a  ..]
			} else {
				userclass[single_char] = set_to_this_when_present;
				RestoreMostRecentlySavedMarkerPosition();
			}
		}

		IncrementMarker();
		advance_past_whitespace();
	}

	// error check and clean up pointer
	single_char = CurrentCharacter();
	if(single_char==']') {
		// this is good.
		IncrementMarker();
		advance_past_whitespace();

	} else { 
		cout<<"Bad character class definition '" << start_of_character_class;
		cout << "'" << endl;
	}
	

	add_to_rule_command('[');
	add_to_rule_charlut(userclass);
}



/////////////////////////////////////////////////////
void ParseNativeRuleCompiler::compile_character_class_copy_class(
/////////////////////////////////////////////////////
	bool * userclass,
	bool set_to_this_when_present, // true=>[\d], false=>[^\d]
	const bool * predefined_class) {

	unsigned int i;

	for(i=0; i<CHARACTER_CLASS_DEPTH; i++) {

		// set_to_this_when_present: true=>[\d], false=>[^\d]
		// if we have a class like [\d], then look for
		// where \d is true and set userclass true.
		//
		// if we have a class like [^\d], then look for
		// where \d is false, and set userclass true.
		if(predefined_class[i]==set_to_this_when_present) {
			userclass[i]=true;
		}
	}

}



/////////////////////////////////////////////////////
//	example: 	... <callback> ... 
/////////////////////////////////////////////////////
void ParseNativeRuleCompiler::compile_callback_function() {
	char temp[1000];
	int i;
	char single_char;
	char *dynamic_char;

	add_to_rule_command('<');
	IncrementMarker(); 
	single_char = CurrentCharacter();

	i=0;
	if(
		((single_char>='A')&&(single_char<='Z'))
	||	((single_char>='a')&&(single_char<='z'))
	||	 (single_char=='_')
	) {
		temp[i++] = single_char;
		IncrementMarker();

		while(character_class_for_w[(int)CurrentCharacter()]) {
			temp[i++]=CurrentCharacter();
			IncrementMarker();
		}
	}

	temp[i++]=0;

	dynamic_char = new char [i];
	strcpy(dynamic_char, temp);

	add_to_rule_letters(dynamic_char);

	if(CurrentCharacter()!='>') {
		cout << "ERROR: Expecting '>' to end callback" << endl;
	} 

	IncrementMarker();
	advance_past_whitespace();
}


/////////////////////////////////////////////////////
//examples:	#{*}(...) #{+}(...) #{?}(...) 
//		#{3}(...) #{5,}(...) #{9,12}(...)
//		#{3,11}g(...) #{*}t(...) 
/////////////////////////////////////////////////////

void ParseNativeRuleCompiler::compile_quantifier(void){
	track_start_paren('#');

	add_to_rule_command('#');
	IncrementMarker();
	advance_past_whitespace();

	if (CurrentCharacter() != '{') {
		cout << "ERROR: expected open curly brace after quantifier"<< endl;
	}

	IncrementMarker();
	advance_past_whitespace();
	int numericvalue=-1;
	bool added_comma = compile_quantifier_get_number_value(numericvalue);

	if(!added_comma) {
		advance_past_whitespace();
	
		if (CurrentCharacter() == '}') {
			add_to_rule_integer(numericvalue);
		} else if (CurrentCharacter() != ',') {
			cout << "ERROR: expected comma in quantifier"<<endl;
		} else {
			IncrementMarker();
			advance_past_whitespace();
	
			compile_quantifier_get_number_value(numericvalue);	
			if (CurrentCharacter() != '}') {
				cout << "ERROR: expected end curly brace in quantifier"<<endl;
			}
		}
	}
	

	IncrementMarker();
	advance_past_whitespace();

	// thrifty or greedy
	if (CurrentCharacter() == 't') {
		add_to_rule_letter('t');
		IncrementMarker();
		advance_past_whitespace();
	} else if (CurrentCharacter() == 'g') {
		add_to_rule_letter('g');
		IncrementMarker();
		advance_past_whitespace();
	} else {
		// default to 'g'
		add_to_rule_letter('g');
	}
	
	if (CurrentCharacter() != '(') {
		cout << "ERROR: expected open parenthesis in quantifier"<<endl;
	}

	// quantifier command will end with a call to a rulenumber.
	// whatever follows the '(' will be put into an anon rule.
	start_compiling_a_new_anonymous_inline_rule();


	IncrementMarker();
	advance_past_whitespace();

}

///////////////////////////////////////////////////////////////////////
bool ParseNativeRuleCompiler::compile_quantifier_get_number_value(int &numericvalue){
///////////////////////////////////////////////////////////////////////
	char number[100];
	int i;
	int numeric;

	numericvalue=0;

	switch (CurrentCharacter()) {
	case '*':
		//add_to_rule("{0,...}");
		add_to_rule_integer(0);
		add_to_rule_integer(0); // a zero in second position means unlimited
		IncrementMarker();
		advance_past_whitespace();
		return true;	// added two numbers, don't need to look for another.
	break;
	case '+':
		//add_to_rule("{1,...}");
		add_to_rule_integer(1);
		add_to_rule_integer(0); // a zero in second position means unlimited
		IncrementMarker();
		advance_past_whitespace();
		return true;	// added two numbers, don't need to look for another.
	break;

	case '?':
		//add_to_rule("{0,1}");
		add_to_rule_integer(0);
		add_to_rule_integer(1);
		IncrementMarker();
		advance_past_whitespace();
		return true;	// added two numbers, don't need to look for another.
	break;
	default:
		if( 
			   ((CurrentCharacter()>='0') && (CurrentCharacter()<='9'))
			|| ((CurrentCharacter()>='a') && (CurrentCharacter()<='f'))
			|| ((CurrentCharacter()>='A') && (CurrentCharacter()<='F'))
			|| ((CurrentCharacter()=='x'))
		) {
			number[0]=0;
			i = 0;
			while((CurrentCharacter()>='0') && (CurrentCharacter()<='9') ) {
				number[i]=CurrentCharacter();
				IncrementMarker();
				i++;
			}
			number[i]=0;

			// number is a string such as '31353' (decimal), '0123123' (octal), or '0x7282fb' (hex)
			// convert it to an integer
			numeric = atoi(number);
			numericvalue=numeric;

			add_to_rule_integer(numeric);

			advance_past_whitespace();

			return false; // only added one number to rule.			
		} else {
			cout << "ERROR: expected numeric value in quantifier";
			cout << "'" << CurrentCharacter() << "'";
			cout << endl;
			IncrementMarker();
			advance_past_whitespace();
			return false;
		}
	break;


	};

}


/////////////////////////////////////////////////////
//	%	(percent sign)
//	
//		examples:	%{name}( ... )
/////////////////////////////////////////////////////
void ParseNativeRuleCompiler::compile_capturing_parens(void){
	char temp[1000];
	int i;
	char *dynamic_char;

	track_start_paren('%');

	add_to_rule_command('%');
	IncrementMarker();
	advance_past_whitespace();

	if (CurrentCharacter() != '{') {
		cout << "ERROR: expected open parenthesis after capturing parenthesis"<< endl;
	}

	IncrementMarker();
	advance_past_whitespace();

	i=0;

	while(character_class_for_w[(int)CurrentCharacter()]) {
		temp[i++] = CurrentCharacter();
		IncrementMarker();
	}

	temp[i++]=0;

	dynamic_char=new char [i+1];
	strcpy(dynamic_char, temp);

	add_to_rule_letters(dynamic_char);

	advance_past_whitespace();

	if (CurrentCharacter() != '}') {
		cout << "ERROR: expected close curly brace after capturing parenthesis"<< endl;
	}

	IncrementMarker();
	advance_past_whitespace();

	if (CurrentCharacter() != '(') {
		cout << "ERROR: expected open parenthesis after capturing parenthesis"<< endl;
	}

	// capturing parens command will end with a call to a rulenumber.
	// whatever follows the '(' will be put into an anon rule.
	start_compiling_a_new_anonymous_inline_rule();

	IncrementMarker();
	advance_past_whitespace();

}


/////////////////////////////////////////////////////
// |	(vertical pipe)
//
//	examples:	|( ... , ... , ... , ... )
/////////////////////////////////////////////////////
void ParseNativeRuleCompiler::compile_alternation(void){
	track_start_paren('|');

	IncrementMarker();
	advance_past_whitespace();

	if (CurrentCharacter() != '(') {
		cout << "ERROR: expected open parenthesis after alternation"<< endl;
	}

	add_to_rule_command('|');

	// will want the command for alternation, followed by a series of anon rule numbers.
	start_compiling_a_new_anonymous_inline_rule();

	IncrementMarker();
	advance_past_whitespace();

}


///////////////////////////////////////////////////////////////////////
// : rulename := rule definition ;
//
// : identifier := [a-zA-Z] Q+(~w) ;
///////////////////////////////////////////////////////////////////////
void ParseNativeRuleCompiler::track_start_of_new_named_rule(void){
///////////////////////////////////////////////////////////////////////

	cout << "track_start_of_new_rule" << endl;

	IncrementMarker();
	advance_past_whitespace();

	char rulename[1000];
	int i;

	if(
		   ((CurrentCharacter()>='a') && (CurrentCharacter()<='z'))
		|| ((CurrentCharacter()>='A') && (CurrentCharacter()<='Z'))
		||  (CurrentCharacter()=='_')
	) {
		
	} else {
		cout << "ERROR: rule name must start with a letter or underscore" << endl;
		return;
	}

	i=0;

	while(character_class_for_w[(int)CurrentCharacter()]) {
		rulename[i++] = CurrentCharacter();
		IncrementMarker();
	}

	rulename[i++]=0;

	cout << "rulename is '" << rulename << "'\n";

	int rulenumber = get_numeric_index_for_named_rule(rulename);
	stack_of_current_rules_being_compiled.push(rulenumber);

	// in case we're a rule inside a rule, or something, push on 
	// the command for ':'. When we hit the end of rule instruction,
	// then we can pop this entire rule off and put it in catalogue.
	add_to_rule_command(':');
	add_to_rule_integer(rulenumber);

	advance_past_whitespace();

	if(CurrentCharacter()!=':') {
		cout << "ERROR: expect rule name to be followed by := " << endl;
		return;
	}

	IncrementMarker();
	advance_past_whitespace();

	if(CurrentCharacter()!='=') {
		cout << "ERROR: expect rule name to be followed by := " << endl;
		return;
	}

	IncrementMarker();
	advance_past_whitespace();
}

///////////////////////////////////////////////////////////////////////
void ParseNativeRuleCompiler::track_end_of_new_rule(void){
///////////////////////////////////////////////////////////////////////
	// this is a hard return, indicated by a ';' in grammar.
	// anon rules may cause add_to_rule_return to get called,
	// but only ';' calls this method.
	//
	// need to look at parenstack.
	// It may contain a number of 'R' symbols.
	// for each 'R' symbol, call add_to_rule_return(),
	// get the return value (rule number), 
	// then add a call to that rule into the rule in progress.
	// repeat for each 'R' in stack.
	//
	// then add a return for the main rule ';'

	while(parenstack.size()) {
		char letter = parenstack.top();parenstack.pop();
		if(letter != 'R') {
			cout << "ERROR: end of rule ';' found leftover in parenstack '"<<letter<<"'\n";
			return;
		}
		int rulenumber = add_to_rule_return();

		add_to_rule_command('&');
		add_to_rule_integer(rulenumber);		
	}

	// now add a return for the entire rule (named rule)
	add_to_rule_return();

	IncrementMarker();
	advance_past_whitespace();
}


///////////////////////////////////////////////////////////////////////
void ParseNativeRuleCompiler::DumpRuleInProgress(void){
///////////////////////////////////////////////////////////////////////
	instructiontype instruction;

	for(unsigned int i=0; i<rule_in_progress.size(); i++) {
		instruction = rule_in_progress[i];
		cout << "rule_in_progress["<<i<<"]=>";DumpInstruction(instruction);
	}
}


///////////////////////////////////////////////////////////////////////
void ParseNativeRuleCompiler::DumpInstruction(instructiontype instruction){
///////////////////////////////////////////////////////////////////////
	labeltype	label;
	opcodetype	opcode;

	label = instruction.label;
	opcode = instruction.opcode;

	switch(label) {

	case COMMAND:
		cout << "COMMAND: " << opcode.command << "\t: ";
		switch(opcode.command) {
			case '\'': cout << "LITERAL CHARACTERS"	; break;
			case '[' : cout << "CHARACTER CLASS"	; break;
			case '~' : cout << "SHORTCUT"		; break;
			case '#' : cout << "QUANTIFIER"		; break;
			case '|' : cout << "ALTERNATION START"	; break;
			case ',' : cout << "ALTERNATION COMMA"	; break;
			case '%' : cout << "CAPTURING PARENS"	; break;
			case '<' : cout << "CALLBACK"		; break;
			case '&' : cout << "RULECALL"		; break;
			case ':' : cout << "RULE DECLARATION"	; break;
			case '.' : cout << "WHITE SPACE"	; break;
			case ')' : cout << "CLOSE PAREN"	; break;
			case ';' : cout << "RETURN"		; break;
			default  : cout << "unknown"		; break;
		}
		cout << endl;
	break;
	case LETTER:
		cout << "LETTER: " << opcode.letter << endl;
	break;
	case LETTERS:
		cout << "LETTERS: " << opcode.letters << endl;
	break;
	case CHARLUT:
		cout << "CHARLUT: " ;
		for(char ltr='!'; ltr<='~'; ltr++) {
			if(opcode.charlut[ltr]) {
				cout << ltr ;
			}
		}
		cout << endl;
	break;
	case INTEGER:
		cout << "INTEGER: " << opcode.integer << endl;
	break;
	default:
		cout << "Unknown label type" << endl;
	break;
	}
}

///////////////////////////////////////////////////////////////////////
void ParseNativeRuleCompiler::DumpRulebook(void){
///////////////////////////////////////////////////////////////////////

	cout << endl << "---------------------------" << endl;
	cout << "ParseNativeRuleCompiler::DumpRulebook" << endl;
	cout << "---------------------------" << endl;

	unsigned int rulenumber;
	char* rulename;
	instructiontype *instructionptr;

	labeltype	label;
	opcodetype	opcode;

	unsigned int i;
	for ( rulenumber=1; rulenumber<rule_number_to_rule_name_converter.size(); rulenumber++ ) {
		rulename = rule_number_to_rule_name_converter[rulenumber];
		cout << "rulenumber="<<rulenumber<<"  rulename='";
		if(rulename != NULL) {
			cout<<rulename;
		} else {
			cout << "NULL";
		}

		cout <<"'"<<endl;

	}

	cout << endl << endl<< endl;

	for ( rulenumber=1; rulenumber<rule_catalogue.size(); rulenumber++ ) {
		instructionptr = rule_catalogue[rulenumber];
		rulename = rule_number_to_rule_name_converter[rulenumber];
		cout << "rulenumber="<<rulenumber<<"  rulename='";
		if(rulename != NULL) {
			cout<<rulename;
		} else {
			cout << "NULL";
		}
		cout <<"'"<<endl;

		if(rulename == NULL) {
			//next;
		}

		i=0;
		while( (rulename!=NULL) 
			&& (	((*instructionptr).opcode.command != ';') 
				|| ((*instructionptr).label != COMMAND)) 
			   )
		{

			label = (*instructionptr).label;
			opcode = (*instructionptr).opcode;

			cout << i++ << " : ";

			DumpInstruction(*instructionptr);
			instructionptr++;
		}
		cout << i <<  " : COMMAND ';' (return)" << endl;
	}
	cout << "---------------------------" << endl;
}







