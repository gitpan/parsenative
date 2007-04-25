
README.txt file for 
ParseNativeInfiniteString.cpp
ParseNativePreprocessorClass.cpp
ParseNativeRuleCompiler.cpp
ParseNativeRuleRunner.cpp

Copyright 2007 Greg London
All Rights Reserved.

(probably to be released under GNU-GPL at some point, or something,
once it's not quite such an embarrasing piece of code)

ParseNativeInfiniteString
=========================
The Base Class in the hierarchy is ParseNativeInfiniteString.
It contains the methods needed to interact with a finite segment
of an infinitely long string (or as long as your harddrive can 
hold.)

Methods include AppendString and AppendCharacter, which take
a string or character and append it to the string in memory
being processed. A linked list is used to keep a running
tally of the text being processed at any moment.

The class maintains a "Marker", which is a struct of information
that keeps track of where in the linked list the text is currently
being processed.

The method CurrentCharacter() returns the character at the current
marker location. The method IncrementMarker() increments the marker
to the next character in the string.

If CurrentCharacter is at the end of the string, then it calls
the method AutoAppendText(), which in the base class does nothing,
but in an extended class may read more text from a file and 
then call AppendString to put it into the linked list in memory.

As the Marker is incremented, and processing is finished on a 
piece of the linked list, older blocks no longer needed are 
deleted from memory. The marker can only move forward one character
at a time. 

The class also provides other methods for marker control.
SaveCurrentMarkerPosition(), RestoreMostRecentlySavedMarkerPosition(),
and DiscardMostRecentlySavedMarkerPosition() are the current
methods used for parsing different possibilities. When a grammar
hits a point where different possibile interpretations must
be tried before matching, calling SaveCurrentMarkerPosition()
will mark the location internally, so that the string isn't
garbage collected beyond that point. If a parsing alternate is
tried and fails, the RestoreMostRecentlySavedMarkerPosition()
method will restore the previous marker and all the grammer
to attempt a different match from the same location.

If a match occurs, the saved marker may be discarded, and elements
in the linked list that are no longer needed can be garbage
collected.

How the "marker" system is saved and restored may change in teh
future, as it is showing some hiccups in the derived classes.



ParseNativePreprocessorClass
============================

The next class in the hierarchy is ParseNativePreprocessorClass.
This class is a stub that shows how one might attach a file to
the InfiniteString, so that as AutoAppendText() is called, text
is read from the file and appended to the string in memory.

At some point, this should handle a filelist on the command line
and process define statements and include directives.



ParseNativeRuleCompiler
=======================

This class takes a grammar and compiles it into a sort of "byte code"
for later execution. This class's main method is CompileGrammar(),
which takes the name of a file containing a grammar.

The grammar consists of rules which are compiled into a "rule book".
The contents of the rulebook can be seen after the rules have been
compiled by calling DumpRulebook().

The rule compiler uses a grammar format that is conducive to compiling.

A simple example of a grammar might look like this:


:fruit:= |('apple','banana');

:name:= 'bob';

:myrule:= #{1,2}t(fruit) name;



Once compiled, calling DumpRulebook will display the following rules:

---------------------------
ParseNativeRuleCompiler::DumpRulebook
---------------------------
rulenumber=1  rulename='fruit'
rulenumber=2  rulename='anon'
rulenumber=3  rulename='anon'
rulenumber=4  rulename='name'
rulenumber=5  rulename='myrule'
rulenumber=6  rulename='anon'
rulenumber=7  rulename='anon'



rulenumber=1  rulename='fruit'
0 : COMMAND: |  : ALTERNATION START
1 : COMMAND: &  : RULECALL
2 : INTEGER: 2
3 : COMMAND: &  : RULECALL
4 : INTEGER: 3
5 : COMMAND: )  : CLOSE PAREN
6 : COMMAND ';' (return)
rulenumber=2  rulename='anon'
0 : COMMAND: '  : LITERAL CHARACTERS
1 : LETTERS: apple
2 : COMMAND ';' (return)
rulenumber=3  rulename='anon'
0 : COMMAND: '  : LITERAL CHARACTERS
1 : LETTERS: banana
2 : COMMAND ';' (return)
rulenumber=4  rulename='name'
0 : COMMAND: '  : LITERAL CHARACTERS
1 : LETTERS: bob
2 : COMMAND ';' (return)
rulenumber=5  rulename='myrule'
0 : COMMAND: #  : QUANTIFIER
1 : INTEGER: 1
2 : INTEGER: 2
3 : LETTER: t
4 : COMMAND: &  : RULECALL
5 : INTEGER: 6
6 : COMMAND: &  : RULECALL
7 : INTEGER: 7
8 : COMMAND ';' (return)
rulenumber=6  rulename='anon'
0 : COMMAND: &  : RULECALL
1 : INTEGER: 1
2 : COMMAND ';' (return)
rulenumber=7  rulename='anon'
0 : COMMAND: &  : RULECALL
1 : INTEGER: 4
2 : COMMAND ';' (return)
---------------------------


The complete list of grammar syntax is as follows:

'	(single quote) Literal text match, i.e. 'hello'
[	(left square bracket) Character Class, i.e. [aeiou] 
~	(tilde) Shortcut and escape character. i.e. ~w or ~n
#	(the number symbol) Quantifier rule.

		#{*}(...) #{+}(...) #{?}(...) 
		#{3}(...) #{5,}(...) #{9,12}(...)
		#{3,11}g(...) #{*}t(...) 

		The {} wraps the quantity, *, +, ? 
		follow standard perl rules, as does
		{3}, {5,}, and {9,1}

		The 'g' and 't' indicate greedy and thrifty.
		The thing being quantified is in parenthesis.


|	(vertical pipe)	Alternation. i.e.   |( ... , ... , ... , ... )
		where ... is replaced by other grammar bits.

%	(percent sign) Capturing parens, i.e. %{id}( ... )

<	(the "less than" symbol) Callback, i.e. <mycallback>

:	( colon ) Rule Definition. i.e.   : identifier := [a-zA-Z] Q+(~w) ;


A rule may be called inside another rule by simply listing the
name of the rule to be called. i.e. see the "curse" rule called below:
		:mccoy:= curse ',' 'Jim'; 
		:curse:= 'dammit';

	Note that a rulecall in the RuleBook is preceded with a '&'
	followed by the rule number to call. 

	Note that as rules are compiled, they are accumulated 
	in the rulebook. The rulebook keeps track of all the 
	rulenames and stores them by number (simply to avoid 
	a hash lookup per call and do an array index instead). 
	So, when looking at the output of DumpRulebook, you'll
	see calls to rulenumbers, not rulenames, but when you
	dump out the rulebook, it will list each rule in numeric
	order, followed by its name.

All the rule compiler class does is compile the rule into the
rulebook. The rule runner class actually executes them.

The grammar may be a little odd, but it is extremely easy to compile,
and a grammar could be built up so that this parser could read in
any other format of grammar and convert it.

Note also that quantifiers and alternations cause anonymous
rules to be compiled, rules with a rulenumber, but no rulename,
to be compiled into the rulebook. This will help execute the
rules later on.



ParseNativeRuleRunner
=====================

The main method in ParseNativeRuleRunner is RunRule().

Once all the rules have been compiled into the rulebook,
you can execute one of them by name.

Currently, crude versions of everything are implemented
except for the capturing parnthesis.

Currently, callbacks are handled by placing a pointer to 
a code reference into a hash called callback_hash.
Whenever the callback in the grammar is encounted, the
rule runner gets the pointer from the hash and calls it.




