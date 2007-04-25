// copyright 2007 Greg London
// All Rights Reserved

#ifndef ParseNativeRuleCompiler_h
#define ParseNativeRuleCompiler_h

#include <iostream>
#include <string>
#include <map>
#include <stack>
#include <vector>
using namespace std;


#include "ParseNativePreprocessorClass.h"

const char ESCAPE_CHARACTER = '~';

// there are three categories laid out here:
// opcodes, labels, and instructions.
// An OPCODE is the thing that will actually be executed.
// An OPCODE may contain a parser command, such as a 
// quantifier command which starts with a '#'.
// An OPCODE may contain a string, an integer, or 
// some other value that is used to implement the command.
//
// A LABEL is attached to each OPCODE to indicate
// how to interpret the OPCODE. The OPCODE is a union.
// The LABEL indicates how to access the data stored
// in the union.
//
// An INSTRUCTION is a combination of one OPCODE and one LABEL.
//

typedef enum {
	COMMAND, // ':',   '#',   '|',   etc
	LETTER,
	LETTERS,
	CHARLUT,
	INTEGER } labeltype;

struct instructiontype;

union opcodetype {
	char		command;
	char		letter;
	char*		letters;
	bool*		charlut;
	unsigned int	integer;
};

struct instructiontype {
	labeltype	label;
	opcodetype	opcode;
};



class ParseNativeRuleCompiler : public ParseNativePreprocessorClass {

	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////
	public:
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////

		ParseNativeRuleCompiler();

		// note: grammar must always be contained in separate text file.
		int CompileGrammar(char * filename_containing_grammar);

		void DumpRulebook(void);

		void DumpInstruction(instructiontype instruction);
		void DumpRuleInProgress(void);
	
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////
	protected: // Can't be accessed by anyone but the class.
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////


		// an array of arrays.
		// first index is an integer, which is a rule number.
		// named rules are converted to numbers using the 
		// rule_name_to_rule_number_converter hash above.
		// anon rules just get a number, a new entry at top
		// of vector.
		vector<instructiontype*> rule_catalogue;

		// as we compile rules, compile the instructions into this vector.
		// push instructions onto end.
		// if we switch to building a new rule, push a marker that 
		// indicates its a new rule, and keep pushing onto same vector.
		// when we hit an end of rule marker, go back and pop off
		// the top rule, put it into a fixed array, and store it in
		// the rule catalogue.
		vector<instructiontype> rule_in_progress;
	
		// as we compile named rules, get their number,
		// and store their name/number pairing here.
		// when we need to call a rule, we'll need to
		// convert the rule name to a number and then call
		// it by number.
		map<string,int> rule_name_to_rule_number_converter;		
		map<string,int>::iterator rule_iter;

		vector<char*> rule_number_to_rule_name_converter;

		// if a rule needs to declare an anonymous rule inside,
		// then push the number for the anon rule on stack.
		// pop off when done, and can return to previous rule.
		// current rule being compiled is always the top entry
		// on the stack.
		stack<int> stack_of_current_rules_being_compiled;


		int get_numeric_index_for_named_rule(char *rule_name);
		void start_compiling_a_new_anonymous_inline_rule(void);


		stack<char> parenstack;

		void advance_past_whitespace(void);

		void compile_escaped_character(void);

		void add_to_rule_raw(labeltype label, opcodetype opcode);
		void add_to_rule_command(char command);
		void add_to_rule_letter(char letter);
		void add_to_rule_letters(char* letters);
		void add_to_rule_charlut(bool* charlut);
		void add_to_rule_integer(unsigned int integer);
		int  add_to_rule_return(void);

		bool compile_rule_call(void);
		void compile_character_sequence(void);
		void compile_character_class(void);
		void compile_character_class_copy_class(
			bool * userclass,
			bool set_to_this_when_present, // true=>[\d], false=>[^\d]
			const bool * predefined_class);

		void compile_callback_function(void);
		void compile_whitespace_skipping(void);

		void track_start_paren(char command);
		void track_close_paren(void);

		void compile_quantifier(void);
		bool compile_quantifier_get_number_value(int &numericvalue);

		void compile_alternation(void);
		void track_alternate_commas(void);

		void compile_capturing_parens(void);

		void track_end_of_new_rule(void);
		void track_start_of_new_named_rule(void);
		int track_start_of_new_anonymous_rule(void);

};



const bool character_class_for_w [] = {
	false, 	//   0 
	false, 	//   1 
	false, 	//   2 
	false, 	//   3 
	false, 	//   4 
	false, 	//   5 
	false, 	//   6 
	false, 	//   7 BELL
	false, 	//   8 
	false, 	//   9 TAB
	false, 	//  10 LINEFEED
	false, 	//  11 
	false, 	//  12 
	false, 	//  13 CARRIAGERETURN
	false, 	//  14 
	false, 	//  15 
	false, 	//  16 
	false, 	//  17 
	false, 	//  18 
	false, 	//  19 
	false, 	//  20 
	false, 	//  21 
	false, 	//  22 
	false, 	//  23 
	false, 	//  24 
	false, 	//  25 
	false, 	//  26 
	false, 	//  27 
	false, 	//  28 
	false, 	//  29 
	false, 	//  30 
	false, 	//  31 
	false, 	//  32 SPACE
	false, 	//  33  ! 
	false, 	//  34  " 
	false, 	//  35  # 
	false, 	//  36  $ 
	false, 	//  37  % 
	false, 	//  38  & 
	false, 	//  39  ' 
	false, 	//  40  ( 
	false, 	//  41  ) 
	false, 	//  42  * 
	false, 	//  43  + 
	false, 	//  44  , 
	false, 	//  45  - 
	false, 	//  46  . 
	false, 	//  47  / 
	true, 	//  48  0 
	true, 	//  49  1 
	true, 	//  50  2 
	true, 	//  51  3 
	true, 	//  52  4 
	true, 	//  53  5 
	true, 	//  54  6 
	true, 	//  55  7 
	true, 	//  56  8 
	true, 	//  57  9 
	false, 	//  58  : 
	false, 	//  59  ; 
	false, 	//  60  < 
	false, 	//  61  = 
	false, 	//  62  > 
	false, 	//  63  ? 
	false, 	//  64  @ 
	true, 	//  65  A 
	true, 	//  66  B 
	true, 	//  67  C 
	true, 	//  68  D 
	true, 	//  69  E 
	true, 	//  70  F 
	true, 	//  71  G 
	true, 	//  72  H 
	true, 	//  73  I 
	true, 	//  74  J 
	true, 	//  75  K 
	true, 	//  76  L 
	true, 	//  77  M 
	true, 	//  78  N 
	true, 	//  79  O 
	true, 	//  80  P 
	true, 	//  81  Q 
	true, 	//  82  R 
	true, 	//  83  S 
	true, 	//  84  T 
	true, 	//  85  U 
	true, 	//  86  V 
	true, 	//  87  W 
	true, 	//  88  X 
	true, 	//  89  Y 
	true, 	//  90  Z 
	false, 	//  91  [ 
	false, 	//  92   
	false, 	//  93  ] 
	false, 	//  94  ^ 
	true, 	//  95  _ 
	false, 	//  96  ` 
	true, 	//  97  a 
	true, 	//  98  b 
	true, 	//  99  c 
	true, 	// 100  d 
	true, 	// 101  e 
	true, 	// 102  f 
	true, 	// 103  g 
	true, 	// 104  h 
	true, 	// 105  i 
	true, 	// 106  j 
	true, 	// 107  k 
	true, 	// 108  l 
	true, 	// 109  m 
	true, 	// 110  n 
	true, 	// 111  o 
	true, 	// 112  p 
	true, 	// 113  q 
	true, 	// 114  r 
	true, 	// 115  s 
	true, 	// 116  t 
	true, 	// 117  u 
	true, 	// 118  v 
	true, 	// 119  w 
	true, 	// 120  x 
	true, 	// 121  y 
	true, 	// 122  z 
	false, 	// 123  { 
	false, 	// 124  | 
	false, 	// 125  } 
	false, 	// 126  ~ 
	false, 	// 127 
	false, 	// 128 
	false, 	// 129 
	false, 	// 130 
	false, 	// 131 
	false, 	// 132 
	false, 	// 133 
	false, 	// 134 
	false, 	// 135 
	false, 	// 136 
	false, 	// 137 
	false, 	// 138 
	false, 	// 139 
	false, 	// 140 
	false, 	// 141 
	false, 	// 142 
	false, 	// 143 
	false, 	// 144 
	false, 	// 145 
	false, 	// 146 
	false, 	// 147 
	false, 	// 148 
	false, 	// 149 
	false, 	// 150 
	false, 	// 151 
	false, 	// 152 
	false, 	// 153 
	false, 	// 154 
	false, 	// 155 
	false, 	// 156 
	false, 	// 157 
	false, 	// 158 
	false, 	// 159 
	false, 	// 160 
	false, 	// 161 
	false, 	// 162 
	false, 	// 163 
	false, 	// 164 
	false, 	// 165 
	false, 	// 166 
	false, 	// 167 
	false, 	// 168 
	false, 	// 169 
	false, 	// 170 
	false, 	// 171 
	false, 	// 172 
	false, 	// 173 
	false, 	// 174 
	false, 	// 175 
	false, 	// 176 
	false, 	// 177 
	false, 	// 178 
	false, 	// 179 
	false, 	// 180 
	false, 	// 181 
	false, 	// 182 
	false, 	// 183 
	false, 	// 184 
	false, 	// 185 
	false, 	// 186 
	false, 	// 187 
	false, 	// 188 
	false, 	// 189 
	false, 	// 190 
	false, 	// 191 
	false, 	// 192 
	false, 	// 193 
	false, 	// 194 
	false, 	// 195 
	false, 	// 196 
	false, 	// 197 
	false, 	// 198 
	false, 	// 199 
	false, 	// 200 
	false, 	// 201 
	false, 	// 202 
	false, 	// 203 
	false, 	// 204 
	false, 	// 205 
	false, 	// 206 
	false, 	// 207 
	false, 	// 208 
	false, 	// 209 
	false, 	// 210 
	false, 	// 211 
	false, 	// 212 
	false, 	// 213 
	false, 	// 214 
	false, 	// 215 
	false, 	// 216 
	false, 	// 217 
	false, 	// 218 
	false, 	// 219 
	false, 	// 220 
	false, 	// 221 
	false, 	// 222 
	false, 	// 223 
	false, 	// 224 
	false, 	// 225 
	false, 	// 226 
	false, 	// 227 
	false, 	// 228 
	false, 	// 229 
	false, 	// 230 
	false, 	// 231 
	false, 	// 232 
	false, 	// 233 
	false, 	// 234 
	false, 	// 235 
	false, 	// 236 
	false, 	// 237 
	false, 	// 238 
	false, 	// 239 
	false, 	// 240 
	false, 	// 241 
	false, 	// 242 
	false, 	// 243 
	false, 	// 244 
	false, 	// 245 
	false, 	// 246 
	false, 	// 247 
	false, 	// 248 
	false, 	// 249 
	false, 	// 250 
	false, 	// 251 
	false, 	// 252 
	false, 	// 253 
	false, 	// 254 
	false, 	// 255 
	false	// 256 (special case for \n, which may be two characters) 
};

const bool character_class_for_W [] = {
	true, 	//   0 
	true, 	//   1 
	true, 	//   2 
	true, 	//   3 
	true, 	//   4 
	true, 	//   5 
	true, 	//   6 
	true, 	//   7 BELL
	true, 	//   8 
	true, 	//   9 TAB
	true, 	//  10 LINEFEED
	true, 	//  11 
	true, 	//  12 
	true, 	//  13 CARRIAGERETURN
	true, 	//  14 
	true, 	//  15 
	true, 	//  16 
	true, 	//  17 
	true, 	//  18 
	true, 	//  19 
	true, 	//  20 
	true, 	//  21 
	true, 	//  22 
	true, 	//  23 
	true, 	//  24 
	true, 	//  25 
	true, 	//  26 
	true, 	//  27 
	true, 	//  28 
	true, 	//  29 
	true, 	//  30 
	true, 	//  31 
	true, 	//  32 SPACE
	true, 	//  33  ! 
	true, 	//  34  " 
	true, 	//  35  # 
	true, 	//  36  $ 
	true, 	//  37  % 
	true, 	//  38  & 
	true, 	//  39  ' 
	true, 	//  40  ( 
	true, 	//  41  ) 
	true, 	//  42  * 
	true, 	//  43  + 
	true, 	//  44  , 
	true, 	//  45  - 
	true, 	//  46  . 
	true, 	//  47  / 
	false, 	//  48  0 
	false, 	//  49  1 
	false, 	//  50  2 
	false, 	//  51  3 
	false, 	//  52  4 
	false, 	//  53  5 
	false, 	//  54  6 
	false, 	//  55  7 
	false, 	//  56  8 
	false, 	//  57  9 
	true, 	//  58  : 
	true, 	//  59  ; 
	true, 	//  60  < 
	true, 	//  61  = 
	true, 	//  62  > 
	true, 	//  63  ? 
	true, 	//  64  @ 
	false, 	//  65  A 
	false, 	//  66  B 
	false, 	//  67  C 
	false, 	//  68  D 
	false, 	//  69  E 
	false, 	//  70  F 
	false, 	//  71  G 
	false, 	//  72  H 
	false, 	//  73  I 
	false, 	//  74  J 
	false, 	//  75  K 
	false, 	//  76  L 
	false, 	//  77  M 
	false, 	//  78  N 
	false, 	//  79  O 
	false, 	//  80  P 
	false, 	//  81  Q 
	false, 	//  82  R 
	false, 	//  83  S 
	false, 	//  84  T 
	false, 	//  85  U 
	false, 	//  86  V 
	false, 	//  87  W 
	false, 	//  88  X 
	false, 	//  89  Y 
	false, 	//  90  Z 
	true, 	//  91  [ 
	true, 	//  92   
	true, 	//  93  ] 
	true, 	//  94  ^ 
	false, 	//  95  _ 
	true, 	//  96  ` 
	false, 	//  97  a 
	false, 	//  98  b 
	false, 	//  99  c 
	false, 	// 100  d 
	false, 	// 101  e 
	false, 	// 102  f 
	false, 	// 103  g 
	false, 	// 104  h 
	false, 	// 105  i 
	false, 	// 106  j 
	false, 	// 107  k 
	false, 	// 108  l 
	false, 	// 109  m 
	false, 	// 110  n 
	false, 	// 111  o 
	false, 	// 112  p 
	false, 	// 113  q 
	false, 	// 114  r 
	false, 	// 115  s 
	false, 	// 116  t 
	false, 	// 117  u 
	false, 	// 118  v 
	false, 	// 119  w 
	false, 	// 120  x 
	false, 	// 121  y 
	false, 	// 122  z 
	true, 	// 123  { 
	true, 	// 124  | 
	true, 	// 125  } 
	true, 	// 126  ~ 
	true, 	// 127 
	true, 	// 128 
	true, 	// 129 
	true, 	// 130 
	true, 	// 131 
	true, 	// 132 
	true, 	// 133 
	true, 	// 134 
	true, 	// 135 
	true, 	// 136 
	true, 	// 137 
	true, 	// 138 
	true, 	// 139 
	true, 	// 140 
	true, 	// 141 
	true, 	// 142 
	true, 	// 143 
	true, 	// 144 
	true, 	// 145 
	true, 	// 146 
	true, 	// 147 
	true, 	// 148 
	true, 	// 149 
	true, 	// 150 
	true, 	// 151 
	true, 	// 152 
	true, 	// 153 
	true, 	// 154 
	true, 	// 155 
	true, 	// 156 
	true, 	// 157 
	true, 	// 158 
	true, 	// 159 
	true, 	// 160 
	true, 	// 161 
	true, 	// 162 
	true, 	// 163 
	true, 	// 164 
	true, 	// 165 
	true, 	// 166 
	true, 	// 167 
	true, 	// 168 
	true, 	// 169 
	true, 	// 170 
	true, 	// 171 
	true, 	// 172 
	true, 	// 173 
	true, 	// 174 
	true, 	// 175 
	true, 	// 176 
	true, 	// 177 
	true, 	// 178 
	true, 	// 179 
	true, 	// 180 
	true, 	// 181 
	true, 	// 182 
	true, 	// 183 
	true, 	// 184 
	true, 	// 185 
	true, 	// 186 
	true, 	// 187 
	true, 	// 188 
	true, 	// 189 
	true, 	// 190 
	true, 	// 191 
	true, 	// 192 
	true, 	// 193 
	true, 	// 194 
	true, 	// 195 
	true, 	// 196 
	true, 	// 197 
	true, 	// 198 
	true, 	// 199 
	true, 	// 200 
	true, 	// 201 
	true, 	// 202 
	true, 	// 203 
	true, 	// 204 
	true, 	// 205 
	true, 	// 206 
	true, 	// 207 
	true, 	// 208 
	true, 	// 209 
	true, 	// 210 
	true, 	// 211 
	true, 	// 212 
	true, 	// 213 
	true, 	// 214 
	true, 	// 215 
	true, 	// 216 
	true, 	// 217 
	true, 	// 218 
	true, 	// 219 
	true, 	// 220 
	true, 	// 221 
	true, 	// 222 
	true, 	// 223 
	true, 	// 224 
	true, 	// 225 
	true, 	// 226 
	true, 	// 227 
	true, 	// 228 
	true, 	// 229 
	true, 	// 230 
	true, 	// 231 
	true, 	// 232 
	true, 	// 233 
	true, 	// 234 
	true, 	// 235 
	true, 	// 236 
	true, 	// 237 
	true, 	// 238 
	true, 	// 239 
	true, 	// 240 
	true, 	// 241 
	true, 	// 242 
	true, 	// 243 
	true, 	// 244 
	true, 	// 245 
	true, 	// 246 
	true, 	// 247 
	true, 	// 248 
	true, 	// 249 
	true, 	// 250 
	true, 	// 251 
	true, 	// 252 
	true, 	// 253 
	true, 	// 254 
	true, 	// 255 
	true	// 256 (special case for \n, which may be two characters) 
};

const bool character_class_for_d [] = {
	false, 	//   0 
	false, 	//   1 
	false, 	//   2 
	false, 	//   3 
	false, 	//   4 
	false, 	//   5 
	false, 	//   6 
	false, 	//   7 BELL
	false, 	//   8 
	false, 	//   9 TAB
	false, 	//  10 LINEFEED
	false, 	//  11 
	false, 	//  12 
	false, 	//  13 CARRIAGERETURN
	false, 	//  14 
	false, 	//  15 
	false, 	//  16 
	false, 	//  17 
	false, 	//  18 
	false, 	//  19 
	false, 	//  20 
	false, 	//  21 
	false, 	//  22 
	false, 	//  23 
	false, 	//  24 
	false, 	//  25 
	false, 	//  26 
	false, 	//  27 
	false, 	//  28 
	false, 	//  29 
	false, 	//  30 
	false, 	//  31 
	false, 	//  32 SPACE
	false, 	//  33  ! 
	false, 	//  34  " 
	false, 	//  35  # 
	false, 	//  36  $ 
	false, 	//  37  % 
	false, 	//  38  & 
	false, 	//  39  ' 
	false, 	//  40  ( 
	false, 	//  41  ) 
	false, 	//  42  * 
	false, 	//  43  + 
	false, 	//  44  , 
	false, 	//  45  - 
	false, 	//  46  . 
	false, 	//  47  / 
	true, 	//  48  0 
	true, 	//  49  1 
	true, 	//  50  2 
	true, 	//  51  3 
	true, 	//  52  4 
	true, 	//  53  5 
	true, 	//  54  6 
	true, 	//  55  7 
	true, 	//  56  8 
	true, 	//  57  9 
	false, 	//  58  : 
	false, 	//  59  ; 
	false, 	//  60  < 
	false, 	//  61  = 
	false, 	//  62  > 
	false, 	//  63  ? 
	false, 	//  64  @ 
	false, 	//  65  A 
	false, 	//  66  B 
	false, 	//  67  C 
	false, 	//  68  D 
	false, 	//  69  E 
	false, 	//  70  F 
	false, 	//  71  G 
	false, 	//  72  H 
	false, 	//  73  I 
	false, 	//  74  J 
	false, 	//  75  K 
	false, 	//  76  L 
	false, 	//  77  M 
	false, 	//  78  N 
	false, 	//  79  O 
	false, 	//  80  P 
	false, 	//  81  Q 
	false, 	//  82  R 
	false, 	//  83  S 
	false, 	//  84  T 
	false, 	//  85  U 
	false, 	//  86  V 
	false, 	//  87  W 
	false, 	//  88  X 
	false, 	//  89  Y 
	false, 	//  90  Z 
	false, 	//  91  [ 
	false, 	//  92   
	false, 	//  93  ] 
	false, 	//  94  ^ 
	false, 	//  95  _ 
	false, 	//  96  ` 
	false, 	//  97  a 
	false, 	//  98  b 
	false, 	//  99  c 
	false, 	// 100  d 
	false, 	// 101  e 
	false, 	// 102  f 
	false, 	// 103  g 
	false, 	// 104  h 
	false, 	// 105  i 
	false, 	// 106  j 
	false, 	// 107  k 
	false, 	// 108  l 
	false, 	// 109  m 
	false, 	// 110  n 
	false, 	// 111  o 
	false, 	// 112  p 
	false, 	// 113  q 
	false, 	// 114  r 
	false, 	// 115  s 
	false, 	// 116  t 
	false, 	// 117  u 
	false, 	// 118  v 
	false, 	// 119  w 
	false, 	// 120  x 
	false, 	// 121  y 
	false, 	// 122  z 
	false, 	// 123  { 
	false, 	// 124  | 
	false, 	// 125  } 
	false, 	// 126  ~ 
	false, 	// 127 
	false, 	// 128 
	false, 	// 129 
	false, 	// 130 
	false, 	// 131 
	false, 	// 132 
	false, 	// 133 
	false, 	// 134 
	false, 	// 135 
	false, 	// 136 
	false, 	// 137 
	false, 	// 138 
	false, 	// 139 
	false, 	// 140 
	false, 	// 141 
	false, 	// 142 
	false, 	// 143 
	false, 	// 144 
	false, 	// 145 
	false, 	// 146 
	false, 	// 147 
	false, 	// 148 
	false, 	// 149 
	false, 	// 150 
	false, 	// 151 
	false, 	// 152 
	false, 	// 153 
	false, 	// 154 
	false, 	// 155 
	false, 	// 156 
	false, 	// 157 
	false, 	// 158 
	false, 	// 159 
	false, 	// 160 
	false, 	// 161 
	false, 	// 162 
	false, 	// 163 
	false, 	// 164 
	false, 	// 165 
	false, 	// 166 
	false, 	// 167 
	false, 	// 168 
	false, 	// 169 
	false, 	// 170 
	false, 	// 171 
	false, 	// 172 
	false, 	// 173 
	false, 	// 174 
	false, 	// 175 
	false, 	// 176 
	false, 	// 177 
	false, 	// 178 
	false, 	// 179 
	false, 	// 180 
	false, 	// 181 
	false, 	// 182 
	false, 	// 183 
	false, 	// 184 
	false, 	// 185 
	false, 	// 186 
	false, 	// 187 
	false, 	// 188 
	false, 	// 189 
	false, 	// 190 
	false, 	// 191 
	false, 	// 192 
	false, 	// 193 
	false, 	// 194 
	false, 	// 195 
	false, 	// 196 
	false, 	// 197 
	false, 	// 198 
	false, 	// 199 
	false, 	// 200 
	false, 	// 201 
	false, 	// 202 
	false, 	// 203 
	false, 	// 204 
	false, 	// 205 
	false, 	// 206 
	false, 	// 207 
	false, 	// 208 
	false, 	// 209 
	false, 	// 210 
	false, 	// 211 
	false, 	// 212 
	false, 	// 213 
	false, 	// 214 
	false, 	// 215 
	false, 	// 216 
	false, 	// 217 
	false, 	// 218 
	false, 	// 219 
	false, 	// 220 
	false, 	// 221 
	false, 	// 222 
	false, 	// 223 
	false, 	// 224 
	false, 	// 225 
	false, 	// 226 
	false, 	// 227 
	false, 	// 228 
	false, 	// 229 
	false, 	// 230 
	false, 	// 231 
	false, 	// 232 
	false, 	// 233 
	false, 	// 234 
	false, 	// 235 
	false, 	// 236 
	false, 	// 237 
	false, 	// 238 
	false, 	// 239 
	false, 	// 240 
	false, 	// 241 
	false, 	// 242 
	false, 	// 243 
	false, 	// 244 
	false, 	// 245 
	false, 	// 246 
	false, 	// 247 
	false, 	// 248 
	false, 	// 249 
	false, 	// 250 
	false, 	// 251 
	false, 	// 252 
	false, 	// 253 
	false, 	// 254 
	false, 	// 255 
	false	// 256 (special case for \n, which may be two characters) 
};

const bool character_class_for_D [] = {
	true, 	//   0 
	true, 	//   1 
	true, 	//   2 
	true, 	//   3 
	true, 	//   4 
	true, 	//   5 
	true, 	//   6 
	true, 	//   7 BELL
	true, 	//   8 
	true, 	//   9 TAB
	true, 	//  10 LINEFEED
	true, 	//  11 
	true, 	//  12 
	true, 	//  13 CARRIAGERETURN
	true, 	//  14 
	true, 	//  15 
	true, 	//  16 
	true, 	//  17 
	true, 	//  18 
	true, 	//  19 
	true, 	//  20 
	true, 	//  21 
	true, 	//  22 
	true, 	//  23 
	true, 	//  24 
	true, 	//  25 
	true, 	//  26 
	true, 	//  27 
	true, 	//  28 
	true, 	//  29 
	true, 	//  30 
	true, 	//  31 
	true, 	//  32 SPACE
	true, 	//  33  ! 
	true, 	//  34  " 
	true, 	//  35  # 
	true, 	//  36  $ 
	true, 	//  37  % 
	true, 	//  38  & 
	true, 	//  39  ' 
	true, 	//  40  ( 
	true, 	//  41  ) 
	true, 	//  42  * 
	true, 	//  43  + 
	true, 	//  44  , 
	true, 	//  45  - 
	true, 	//  46  . 
	true, 	//  47  / 
	false, 	//  48  0 
	false, 	//  49  1 
	false, 	//  50  2 
	false, 	//  51  3 
	false, 	//  52  4 
	false, 	//  53  5 
	false, 	//  54  6 
	false, 	//  55  7 
	false, 	//  56  8 
	false, 	//  57  9 
	true, 	//  58  : 
	true, 	//  59  ; 
	true, 	//  60  < 
	true, 	//  61  = 
	true, 	//  62  > 
	true, 	//  63  ? 
	true, 	//  64  @ 
	true, 	//  65  A 
	true, 	//  66  B 
	true, 	//  67  C 
	true, 	//  68  D 
	true, 	//  69  E 
	true, 	//  70  F 
	true, 	//  71  G 
	true, 	//  72  H 
	true, 	//  73  I 
	true, 	//  74  J 
	true, 	//  75  K 
	true, 	//  76  L 
	true, 	//  77  M 
	true, 	//  78  N 
	true, 	//  79  O 
	true, 	//  80  P 
	true, 	//  81  Q 
	true, 	//  82  R 
	true, 	//  83  S 
	true, 	//  84  T 
	true, 	//  85  U 
	true, 	//  86  V 
	true, 	//  87  W 
	true, 	//  88  X 
	true, 	//  89  Y 
	true, 	//  90  Z 
	true, 	//  91  [ 
	true, 	//  92   
	true, 	//  93  ] 
	true, 	//  94  ^ 
	true, 	//  95  _ 
	true, 	//  96  ` 
	true, 	//  97  a 
	true, 	//  98  b 
	true, 	//  99  c 
	true, 	// 100  d 
	true, 	// 101  e 
	true, 	// 102  f 
	true, 	// 103  g 
	true, 	// 104  h 
	true, 	// 105  i 
	true, 	// 106  j 
	true, 	// 107  k 
	true, 	// 108  l 
	true, 	// 109  m 
	true, 	// 110  n 
	true, 	// 111  o 
	true, 	// 112  p 
	true, 	// 113  q 
	true, 	// 114  r 
	true, 	// 115  s 
	true, 	// 116  t 
	true, 	// 117  u 
	true, 	// 118  v 
	true, 	// 119  w 
	true, 	// 120  x 
	true, 	// 121  y 
	true, 	// 122  z 
	true, 	// 123  { 
	true, 	// 124  | 
	true, 	// 125  } 
	true, 	// 126  ~ 
	true, 	// 127 
	true, 	// 128 
	true, 	// 129 
	true, 	// 130 
	true, 	// 131 
	true, 	// 132 
	true, 	// 133 
	true, 	// 134 
	true, 	// 135 
	true, 	// 136 
	true, 	// 137 
	true, 	// 138 
	true, 	// 139 
	true, 	// 140 
	true, 	// 141 
	true, 	// 142 
	true, 	// 143 
	true, 	// 144 
	true, 	// 145 
	true, 	// 146 
	true, 	// 147 
	true, 	// 148 
	true, 	// 149 
	true, 	// 150 
	true, 	// 151 
	true, 	// 152 
	true, 	// 153 
	true, 	// 154 
	true, 	// 155 
	true, 	// 156 
	true, 	// 157 
	true, 	// 158 
	true, 	// 159 
	true, 	// 160 
	true, 	// 161 
	true, 	// 162 
	true, 	// 163 
	true, 	// 164 
	true, 	// 165 
	true, 	// 166 
	true, 	// 167 
	true, 	// 168 
	true, 	// 169 
	true, 	// 170 
	true, 	// 171 
	true, 	// 172 
	true, 	// 173 
	true, 	// 174 
	true, 	// 175 
	true, 	// 176 
	true, 	// 177 
	true, 	// 178 
	true, 	// 179 
	true, 	// 180 
	true, 	// 181 
	true, 	// 182 
	true, 	// 183 
	true, 	// 184 
	true, 	// 185 
	true, 	// 186 
	true, 	// 187 
	true, 	// 188 
	true, 	// 189 
	true, 	// 190 
	true, 	// 191 
	true, 	// 192 
	true, 	// 193 
	true, 	// 194 
	true, 	// 195 
	true, 	// 196 
	true, 	// 197 
	true, 	// 198 
	true, 	// 199 
	true, 	// 200 
	true, 	// 201 
	true, 	// 202 
	true, 	// 203 
	true, 	// 204 
	true, 	// 205 
	true, 	// 206 
	true, 	// 207 
	true, 	// 208 
	true, 	// 209 
	true, 	// 210 
	true, 	// 211 
	true, 	// 212 
	true, 	// 213 
	true, 	// 214 
	true, 	// 215 
	true, 	// 216 
	true, 	// 217 
	true, 	// 218 
	true, 	// 219 
	true, 	// 220 
	true, 	// 221 
	true, 	// 222 
	true, 	// 223 
	true, 	// 224 
	true, 	// 225 
	true, 	// 226 
	true, 	// 227 
	true, 	// 228 
	true, 	// 229 
	true, 	// 230 
	true, 	// 231 
	true, 	// 232 
	true, 	// 233 
	true, 	// 234 
	true, 	// 235 
	true, 	// 236 
	true, 	// 237 
	true, 	// 238 
	true, 	// 239 
	true, 	// 240 
	true, 	// 241 
	true, 	// 242 
	true, 	// 243 
	true, 	// 244 
	true, 	// 245 
	true, 	// 246 
	true, 	// 247 
	true, 	// 248 
	true, 	// 249 
	true, 	// 250 
	true, 	// 251 
	true, 	// 252 
	true, 	// 253 
	true, 	// 254 
	true, 	// 255 
	true	// 256 (special case for \n, which may be two characters) 
};

const bool character_class_for_s [] = {
	false, 	//   0 
	false, 	//   1 
	false, 	//   2 
	false, 	//   3 
	false, 	//   4 
	false, 	//   5 
	false, 	//   6 
	false, 	//   7 BELL
	false, 	//   8 
	true, 	//   9 TAB
	true, 	//  10 LINEFEED
	false, 	//  11 
	true, 	//  12 
	true, 	//  13 CARRIAGERETURN
	false, 	//  14 
	false, 	//  15 
	false, 	//  16 
	false, 	//  17 
	false, 	//  18 
	false, 	//  19 
	false, 	//  20 
	false, 	//  21 
	false, 	//  22 
	false, 	//  23 
	false, 	//  24 
	false, 	//  25 
	false, 	//  26 
	false, 	//  27 
	false, 	//  28 
	false, 	//  29 
	false, 	//  30 
	false, 	//  31 
	true, 	//  32 SPACE
	false, 	//  33  ! 
	false, 	//  34  " 
	false, 	//  35  # 
	false, 	//  36  $ 
	false, 	//  37  % 
	false, 	//  38  & 
	false, 	//  39  ' 
	false, 	//  40  ( 
	false, 	//  41  ) 
	false, 	//  42  * 
	false, 	//  43  + 
	false, 	//  44  , 
	false, 	//  45  - 
	false, 	//  46  . 
	false, 	//  47  / 
	false, 	//  48  0 
	false, 	//  49  1 
	false, 	//  50  2 
	false, 	//  51  3 
	false, 	//  52  4 
	false, 	//  53  5 
	false, 	//  54  6 
	false, 	//  55  7 
	false, 	//  56  8 
	false, 	//  57  9 
	false, 	//  58  : 
	false, 	//  59  ; 
	false, 	//  60  < 
	false, 	//  61  = 
	false, 	//  62  > 
	false, 	//  63  ? 
	false, 	//  64  @ 
	false, 	//  65  A 
	false, 	//  66  B 
	false, 	//  67  C 
	false, 	//  68  D 
	false, 	//  69  E 
	false, 	//  70  F 
	false, 	//  71  G 
	false, 	//  72  H 
	false, 	//  73  I 
	false, 	//  74  J 
	false, 	//  75  K 
	false, 	//  76  L 
	false, 	//  77  M 
	false, 	//  78  N 
	false, 	//  79  O 
	false, 	//  80  P 
	false, 	//  81  Q 
	false, 	//  82  R 
	false, 	//  83  S 
	false, 	//  84  T 
	false, 	//  85  U 
	false, 	//  86  V 
	false, 	//  87  W 
	false, 	//  88  X 
	false, 	//  89  Y 
	false, 	//  90  Z 
	false, 	//  91  [ 
	false, 	//  92   
	false, 	//  93  ] 
	false, 	//  94  ^ 
	false, 	//  95  _ 
	false, 	//  96  ` 
	false, 	//  97  a 
	false, 	//  98  b 
	false, 	//  99  c 
	false, 	// 100  d 
	false, 	// 101  e 
	false, 	// 102  f 
	false, 	// 103  g 
	false, 	// 104  h 
	false, 	// 105  i 
	false, 	// 106  j 
	false, 	// 107  k 
	false, 	// 108  l 
	false, 	// 109  m 
	false, 	// 110  n 
	false, 	// 111  o 
	false, 	// 112  p 
	false, 	// 113  q 
	false, 	// 114  r 
	false, 	// 115  s 
	false, 	// 116  t 
	false, 	// 117  u 
	false, 	// 118  v 
	false, 	// 119  w 
	false, 	// 120  x 
	false, 	// 121  y 
	false, 	// 122  z 
	false, 	// 123  { 
	false, 	// 124  | 
	false, 	// 125  } 
	false, 	// 126  ~ 
	false, 	// 127 
	false, 	// 128 
	false, 	// 129 
	false, 	// 130 
	false, 	// 131 
	false, 	// 132 
	false, 	// 133 
	false, 	// 134 
	false, 	// 135 
	false, 	// 136 
	false, 	// 137 
	false, 	// 138 
	false, 	// 139 
	false, 	// 140 
	false, 	// 141 
	false, 	// 142 
	false, 	// 143 
	false, 	// 144 
	false, 	// 145 
	false, 	// 146 
	false, 	// 147 
	false, 	// 148 
	false, 	// 149 
	false, 	// 150 
	false, 	// 151 
	false, 	// 152 
	false, 	// 153 
	false, 	// 154 
	false, 	// 155 
	false, 	// 156 
	false, 	// 157 
	false, 	// 158 
	false, 	// 159 
	false, 	// 160 
	false, 	// 161 
	false, 	// 162 
	false, 	// 163 
	false, 	// 164 
	false, 	// 165 
	false, 	// 166 
	false, 	// 167 
	false, 	// 168 
	false, 	// 169 
	false, 	// 170 
	false, 	// 171 
	false, 	// 172 
	false, 	// 173 
	false, 	// 174 
	false, 	// 175 
	false, 	// 176 
	false, 	// 177 
	false, 	// 178 
	false, 	// 179 
	false, 	// 180 
	false, 	// 181 
	false, 	// 182 
	false, 	// 183 
	false, 	// 184 
	false, 	// 185 
	false, 	// 186 
	false, 	// 187 
	false, 	// 188 
	false, 	// 189 
	false, 	// 190 
	false, 	// 191 
	false, 	// 192 
	false, 	// 193 
	false, 	// 194 
	false, 	// 195 
	false, 	// 196 
	false, 	// 197 
	false, 	// 198 
	false, 	// 199 
	false, 	// 200 
	false, 	// 201 
	false, 	// 202 
	false, 	// 203 
	false, 	// 204 
	false, 	// 205 
	false, 	// 206 
	false, 	// 207 
	false, 	// 208 
	false, 	// 209 
	false, 	// 210 
	false, 	// 211 
	false, 	// 212 
	false, 	// 213 
	false, 	// 214 
	false, 	// 215 
	false, 	// 216 
	false, 	// 217 
	false, 	// 218 
	false, 	// 219 
	false, 	// 220 
	false, 	// 221 
	false, 	// 222 
	false, 	// 223 
	false, 	// 224 
	false, 	// 225 
	false, 	// 226 
	false, 	// 227 
	false, 	// 228 
	false, 	// 229 
	false, 	// 230 
	false, 	// 231 
	false, 	// 232 
	false, 	// 233 
	false, 	// 234 
	false, 	// 235 
	false, 	// 236 
	false, 	// 237 
	false, 	// 238 
	false, 	// 239 
	false, 	// 240 
	false, 	// 241 
	false, 	// 242 
	false, 	// 243 
	false, 	// 244 
	false, 	// 245 
	false, 	// 246 
	false, 	// 247 
	false, 	// 248 
	false, 	// 249 
	false, 	// 250 
	false, 	// 251 
	false, 	// 252 
	false, 	// 253 
	false, 	// 254 
	false, 	// 255 
	true	// 256 (special case for \n, which may be two characters) 
};

const bool character_class_for_S [] = {
	true, 	//   0 
	true, 	//   1 
	true, 	//   2 
	true, 	//   3 
	true, 	//   4 
	true, 	//   5 
	true, 	//   6 
	true, 	//   7 BELL
	true, 	//   8 
	false, 	//   9 TAB
	false, 	//  10 LINEFEED
	true, 	//  11 
	false, 	//  12 
	false, 	//  13 CARRIAGERETURN
	true, 	//  14 
	true, 	//  15 
	true, 	//  16 
	true, 	//  17 
	true, 	//  18 
	true, 	//  19 
	true, 	//  20 
	true, 	//  21 
	true, 	//  22 
	true, 	//  23 
	true, 	//  24 
	true, 	//  25 
	true, 	//  26 
	true, 	//  27 
	true, 	//  28 
	true, 	//  29 
	true, 	//  30 
	true, 	//  31 
	false, 	//  32 SPACE
	true, 	//  33  ! 
	true, 	//  34  " 
	true, 	//  35  # 
	true, 	//  36  $ 
	true, 	//  37  % 
	true, 	//  38  & 
	true, 	//  39  ' 
	true, 	//  40  ( 
	true, 	//  41  ) 
	true, 	//  42  * 
	true, 	//  43  + 
	true, 	//  44  , 
	true, 	//  45  - 
	true, 	//  46  . 
	true, 	//  47  / 
	true, 	//  48  0 
	true, 	//  49  1 
	true, 	//  50  2 
	true, 	//  51  3 
	true, 	//  52  4 
	true, 	//  53  5 
	true, 	//  54  6 
	true, 	//  55  7 
	true, 	//  56  8 
	true, 	//  57  9 
	true, 	//  58  : 
	true, 	//  59  ; 
	true, 	//  60  < 
	true, 	//  61  = 
	true, 	//  62  > 
	true, 	//  63  ? 
	true, 	//  64  @ 
	true, 	//  65  A 
	true, 	//  66  B 
	true, 	//  67  C 
	true, 	//  68  D 
	true, 	//  69  E 
	true, 	//  70  F 
	true, 	//  71  G 
	true, 	//  72  H 
	true, 	//  73  I 
	true, 	//  74  J 
	true, 	//  75  K 
	true, 	//  76  L 
	true, 	//  77  M 
	true, 	//  78  N 
	true, 	//  79  O 
	true, 	//  80  P 
	true, 	//  81  Q 
	true, 	//  82  R 
	true, 	//  83  S 
	true, 	//  84  T 
	true, 	//  85  U 
	true, 	//  86  V 
	true, 	//  87  W 
	true, 	//  88  X 
	true, 	//  89  Y 
	true, 	//  90  Z 
	true, 	//  91  [ 
	true, 	//  92   
	true, 	//  93  ] 
	true, 	//  94  ^ 
	true, 	//  95  _ 
	true, 	//  96  ` 
	true, 	//  97  a 
	true, 	//  98  b 
	true, 	//  99  c 
	true, 	// 100  d 
	true, 	// 101  e 
	true, 	// 102  f 
	true, 	// 103  g 
	true, 	// 104  h 
	true, 	// 105  i 
	true, 	// 106  j 
	true, 	// 107  k 
	true, 	// 108  l 
	true, 	// 109  m 
	true, 	// 110  n 
	true, 	// 111  o 
	true, 	// 112  p 
	true, 	// 113  q 
	true, 	// 114  r 
	true, 	// 115  s 
	true, 	// 116  t 
	true, 	// 117  u 
	true, 	// 118  v 
	true, 	// 119  w 
	true, 	// 120  x 
	true, 	// 121  y 
	true, 	// 122  z 
	true, 	// 123  { 
	true, 	// 124  | 
	true, 	// 125  } 
	true, 	// 126  ~ 
	true, 	// 127 
	true, 	// 128 
	true, 	// 129 
	true, 	// 130 
	true, 	// 131 
	true, 	// 132 
	true, 	// 133 
	true, 	// 134 
	true, 	// 135 
	true, 	// 136 
	true, 	// 137 
	true, 	// 138 
	true, 	// 139 
	true, 	// 140 
	true, 	// 141 
	true, 	// 142 
	true, 	// 143 
	true, 	// 144 
	true, 	// 145 
	true, 	// 146 
	true, 	// 147 
	true, 	// 148 
	true, 	// 149 
	true, 	// 150 
	true, 	// 151 
	true, 	// 152 
	true, 	// 153 
	true, 	// 154 
	true, 	// 155 
	true, 	// 156 
	true, 	// 157 
	true, 	// 158 
	true, 	// 159 
	true, 	// 160 
	true, 	// 161 
	true, 	// 162 
	true, 	// 163 
	true, 	// 164 
	true, 	// 165 
	true, 	// 166 
	true, 	// 167 
	true, 	// 168 
	true, 	// 169 
	true, 	// 170 
	true, 	// 171 
	true, 	// 172 
	true, 	// 173 
	true, 	// 174 
	true, 	// 175 
	true, 	// 176 
	true, 	// 177 
	true, 	// 178 
	true, 	// 179 
	true, 	// 180 
	true, 	// 181 
	true, 	// 182 
	true, 	// 183 
	true, 	// 184 
	true, 	// 185 
	true, 	// 186 
	true, 	// 187 
	true, 	// 188 
	true, 	// 189 
	true, 	// 190 
	true, 	// 191 
	true, 	// 192 
	true, 	// 193 
	true, 	// 194 
	true, 	// 195 
	true, 	// 196 
	true, 	// 197 
	true, 	// 198 
	true, 	// 199 
	true, 	// 200 
	true, 	// 201 
	true, 	// 202 
	true, 	// 203 
	true, 	// 204 
	true, 	// 205 
	true, 	// 206 
	true, 	// 207 
	true, 	// 208 
	true, 	// 209 
	true, 	// 210 
	true, 	// 211 
	true, 	// 212 
	true, 	// 213 
	true, 	// 214 
	true, 	// 215 
	true, 	// 216 
	true, 	// 217 
	true, 	// 218 
	true, 	// 219 
	true, 	// 220 
	true, 	// 221 
	true, 	// 222 
	true, 	// 223 
	true, 	// 224 
	true, 	// 225 
	true, 	// 226 
	true, 	// 227 
	true, 	// 228 
	true, 	// 229 
	true, 	// 230 
	true, 	// 231 
	true, 	// 232 
	true, 	// 233 
	true, 	// 234 
	true, 	// 235 
	true, 	// 236 
	true, 	// 237 
	true, 	// 238 
	true, 	// 239 
	true, 	// 240 
	true, 	// 241 
	true, 	// 242 
	true, 	// 243 
	true, 	// 244 
	true, 	// 245 
	true, 	// 246 
	true, 	// 247 
	true, 	// 248 
	true, 	// 249 
	true, 	// 250 
	true, 	// 251 
	true, 	// 252 
	true, 	// 253 
	true, 	// 254 
	true, 	// 255 
	false	// 256 (special case for \n, which may be two characters) 
};





/******************************************************************************
 The lookup tables above were generated by the following perl code:

use warnings;
use strict;

sub print_truth_table {
	my $regexp_code = shift(@_);
	my $regexp = "\\".$regexp_code;

	print "const bool character_class_for_".$regexp_code." [] = {\n";

	for(my $i=0; $i<256; $i++) {
		print "\t";
		my $char = chr($i);
		if ($char =~ m{$regexp}) {
			print "true";
		} else {
			print "false";
		}
		my $formatted = sprintf("%3d",$i);
		print ", \t// $formatted \n";
	}

	print "\t";
	if("\n" =~ m{$regexp}) {
		print "true";
	} else { 
		print "false";
	}

	print "\t// 256 (special case for \\n, which may be two characters) \n};\n\n";
}

foreach my $char qw(w W d D s S) {
	print_truth_table($char);
}

******************************************************************************/



#endif
