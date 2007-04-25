// copyright 2007 Greg London
// All Rights Reserved


#ifndef ParseNativeRuleRunner_h
#define ParseNativeRuleRunner_h

#include <stack>
using namespace std;

#include "ParseNativeRuleCompiler.h"

class ParseNativeRuleRunner : public ParseNativeRuleCompiler {

	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////
	public:
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////

		ParseNativeRuleRunner();

		// note: rule must be compiled before it can be run
		int RunRule(char * rule);

		int hello_world(void);

		// callbacks
		//map<string,int(*)()> callback_hash;
		map<string,int(*)(ParseNativeRuleRunner&)> callback_hash;

	protected:
		// every time we call a new rule, push the instruction pointer onto this stack.
		// when we're done with the rule call, we can pop off the stack and resume
		// wherever we left off.
		stack<instructiontype*> instruction_pointer_stack;

	
		// every time we call a new rule, push the rule number onto this stack.
		// when we fail, we have a history of rule inside rule inside rule to get here.
		stack<unsigned int> rule_call_number_stack;


		labeltype ip_label(void);
		opcodetype ip_opcode(void);
		void next_ip();

		int RunRule(unsigned int rulenumber);
		int run_literal_characters(void);
		int run_character_class(void);
		int run_shortcut(void);
		int run_quantifier(void);
		int run_alternation_start(void);
		int run_alternation_comma(void);
		int run_capturing_parens(void);
		int run_callback(void);
		int run_rulecall(void);
		int run_skip_whitespace(void);
		int run_dont_skip_space(void);
		int run_close_paren(void);


		int run_quantifier_greedy
			(unsigned int highcount, 
			unsigned int lowcount, 
			unsigned int quantizationrulenumber, 
			unsigned int postquantrulenumber);
		int run_quantifier_thrifty
			(unsigned int highcount, 
			unsigned int lowcount, 
			unsigned int quantizationrulenumber, 
			unsigned int postquantrulenumber);

};

#endif

