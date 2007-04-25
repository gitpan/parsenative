// copyright 2007 Greg London
// All Rights Reserved

#ifndef ParseNativeInfiniteString_h
#define ParseNativeInfiniteString_h

#include <stack>
using namespace std;


// how many characters of text we can hold in a single linked list block
const unsigned int BLOCKSIZE = 9;


struct ParseNativeLinkedListBlock {
	// array[BLOCKSIZE] will create an array from [0] to [BLOCKSIZE-1].
	// All code that checks for last index will need to see if index
	// equals BLOCKSIZE-1
	
	// this is the actual block of text
	char textarr [BLOCKSIZE]; 

	// every character could be from a completely different file.
	// (file may #include another file, which #include's another, etc)
	// this array keeps track of who belongs to what.
	char *sourceinfo [BLOCKSIZE];

	ParseNativeLinkedListBlock *next;
	ParseNativeLinkedListBlock *prev;
};

struct ParseNativeMarkerType {
	ParseNativeLinkedListBlock *blockptr;
	unsigned int index;
};

class ParseNativeInfiniteString
{
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////
	public:
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////
		// constructor/destructor
		///////////////////////////////////////////////////////
		ParseNativeInfiniteString(void);
		virtual ~ParseNativeInfiniteString(void);

		///////////////////////////////////////////////////////
		// dump all the contents of this object to stdout
		///////////////////////////////////////////////////////
		virtual void dump_blocks(void);

		///////////////////////////////////////////////////////
		// user manually appends text to end of object
		///////////////////////////////////////////////////////
		virtual void AppendCharacter(char userchar);
		virtual void AppendString(char *userstr);

		// preprocessor will return a string that indicates the current file info at append point
		virtual char * SourceInfoAtAppendPoint();

		// infinite string will return the source info at or before the current marker.
		virtual char * SourceInfoAtMarkerPoint();

		virtual void RecordSourceInfoForNextAppendedCharacter(void);


		///////////////////////////////////////////////////////
		// this method will be called when we hit the end of the
		// string and there is no more text in object to parse.
		// calling this method will cause more text to be 
		// appended to end of infinite string if it can be done.
		// If it can, return true. If it can't return false.
		// manual behavior is for user to use "AppendString"
		// to put in entire string, and AutoAppendText returns false.
		// derived classes will override the AutoAppendText
		// method and insert their own functionality that will
		// get some new text and use AppendString to insert it.
		///////////////////////////////////////////////////////
		virtual bool AutoAppendText(void);

		///////////////////////////////////////////////////////
		///////////////////////////////////////////////////////
		// the "marker" in the infinite string is managed internally.
		// users simply call the following methods to manage 
		// the marker rather than getting direct control of it
		///////////////////////////////////////////////////////
		///////////////////////////////////////////////////////

		///////////////////////////////////////////////////////
		// return the character we are currently pointing to
		///////////////////////////////////////////////////////
		virtual char CurrentCharacter(void);

		///////////////////////////////////////////////////////
		// increment pointer so it is pointing to next character.
		///////////////////////////////////////////////////////
		virtual bool IncrementMarker(void);

		///////////////////////////////////////////////////////
		// use save/restore/discard when you want to try parsing something
		// and it may fail. save, try to parse, then restore if failed.
		// or discard if passed.
		///////////////////////////////////////////////////////
		virtual void SaveCurrentMarkerPosition(void);

		///////////////////////////////////////////////////////
		// whatever you attempted to match, failed. Go back to previous pointer.
		///////////////////////////////////////////////////////
		virtual void RestoreMostRecentlySavedMarkerPosition(void);

		///////////////////////////////////////////////////////
		// whatever you attempted to match, passed. Don't need to keep pointer.
		///////////////////////////////////////////////////////
		virtual void DiscardMostRecentlySavedMarkerPosition(void);

		// return a marker to the current position.
		// Once you have a marker, you can pass it into NumifyMarker, 
		// which returns a numeric value that can be used by comparison 
		// operators to compare two markers and determine which marker 
		// is ahead of which.
		// You can also pass two markers into StringifyMarkers, which 
		// returns the string between two markers.
		// DistanceBetweenMarkers returns the number of characters between
		// two given markers and sorts the markers so the first parameter
		// will be point to the first character in string block.
		virtual ParseNativeMarkerType GetCurrentMarkerPosition(void);
		void SetCurrentMarkerPosition(ParseNativeMarkerType marker);
		virtual void DumpMarker(ParseNativeMarkerType marker);

		virtual unsigned int NumifyMarker(ParseNativeMarkerType marker);
		virtual char*	StringifyMarkers(ParseNativeMarkerType marker1,ParseNativeMarkerType marker2);
		virtual unsigned int DistanceBetweenMarkers
			(ParseNativeMarkerType &marker1,ParseNativeMarkerType &marker2);

		///////////////////////////////////////////////////////
		// this will delete the linked list that points to the
		// current string being processed and starts a new empty string.
		///////////////////////////////////////////////////////
		virtual void DiscardCurrentString(void);

	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////
	private: // Can't be accessed by anyone but the class.
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////
		
		ParseNativeLinkedListBlock * _first_block_ptr;
		ParseNativeLinkedListBlock * _current_block_ptr;
		ParseNativeLinkedListBlock * _last_block_ptr;

		// always contains the index into the _last_block string array
		// that is the end of the entire string. if appending new text,
		// append it here.
		unsigned int _index_to_null_char_in_last_block_string;


		// this is the index into the _current_block that
		// contains the character we are currently processing
		unsigned int _index_to_current_character_in_current_block;

		// this stack is used to save/restore/discard the 
		// current location that we are parsing.
		stack<ParseNativeMarkerType> _marker_stack;


		// create a dynamically allocated block that can be tied into linked list.
		ParseNativeLinkedListBlock *generate_new_block(void);

		// garbage collect one particular block, a block and all its predecessors, or all blocks.
		void garbage_collect_one_block(ParseNativeLinkedListBlock *block);
		void garbage_collect_block_and_all_predecessors(ParseNativeLinkedListBlock *block);
		void garbage_collect_all_blocks(void);

		// start with a new empty string. assumes any existing 
		// string was properly garbage collected 
		virtual void null_string(void);
};

#endif
