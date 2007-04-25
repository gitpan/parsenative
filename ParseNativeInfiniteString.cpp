// Copyright 2007 Greg London
// All Rights Reserved

#include "ParseNativeInfiniteString.h"

#include <iostream>
using namespace std;

///////////////////////////////////////////////////////////////////////
ParseNativeInfiniteString::ParseNativeInfiniteString(void) {
///////////////////////////////////////////////////////////////////////
	null_string();
}

///////////////////////////////////////////////////////////////////////
void ParseNativeInfiniteString::null_string(void){
///////////////////////////////////////////////////////////////////////
	_current_block_ptr = generate_new_block();
	_first_block_ptr   = _current_block_ptr;
	_last_block_ptr    = _current_block_ptr;


	_index_to_null_char_in_last_block_string=0;
	_index_to_current_character_in_current_block=0;
}


///////////////////////////////////////////////////////////////////////
ParseNativeLinkedListBlock *ParseNativeInfiniteString::generate_new_block(void){
///////////////////////////////////////////////////////////////////////
	ParseNativeLinkedListBlock *block;

	block = new ParseNativeLinkedListBlock;

	// set all the elements in array to 0
	for(unsigned int i=0; i<BLOCKSIZE; i++) {
		block->textarr[i] = 0;
		block->sourceinfo[i]=NULL;
	}

	block->next = NULL;
	block->prev = NULL;

	return block;
}


///////////////////////////////////////////////////////////////////////
void ParseNativeInfiniteString::garbage_collect_one_block(ParseNativeLinkedListBlock *block){
///////////////////////////////////////////////////////////////////////
	char *srcinfo;
	for(unsigned int i=0; i<BLOCKSIZE; i++) {
		block->textarr[i] = 0;
		srcinfo = block->sourceinfo[i];
		if(srcinfo != NULL) {
			delete srcinfo;
		}
		block->sourceinfo[i]=NULL;
	}


}

///////////////////////////////////////////////////////////////////////
void ParseNativeInfiniteString::garbage_collect_block_and_all_predecessors(ParseNativeLinkedListBlock *block) {
///////////////////////////////////////////////////////////////////////
	ParseNativeLinkedListBlock *curr_block, *next_block;
	curr_block = _first_block_ptr;

	while( (curr_block != block) && (curr_block != NULL) ){
		garbage_collect_one_block(curr_block);

		next_block = curr_block->next;
		garbage_collect_one_block(curr_block);
		curr_block = next_block;
	}

	if(curr_block != NULL) {
		garbage_collect_one_block(curr_block);
	}
}
///////////////////////////////////////////////////////////////////////
void ParseNativeInfiniteString::garbage_collect_all_blocks(void){
///////////////////////////////////////////////////////////////////////
	garbage_collect_block_and_all_predecessors(_last_block_ptr);
}

///////////////////////////////////////////////////////////////////////
void ParseNativeInfiniteString::DiscardCurrentString(void){
///////////////////////////////////////////////////////////////////////
	garbage_collect_all_blocks();
	null_string();
}

///////////////////////////////////////////////////////////////////////
// This is a "virtual" function.
///////////////////////////////////////////////////////////////////////
// virtual destructor because AutoAppendText is a virtual method.
// derivative class may use AutoAppendText to get more text from a 
// file. Destructor would then have to make sure file was closed, etc.
// AutoAppendText in this class just returns false, so destructor
// doesn't need to perform any cleanup.
///////////////////////////////////////////////////////////////////////
ParseNativeInfiniteString::~ParseNativeInfiniteString(void) {
///////////////////////////////////////////////////////////////////////
	garbage_collect_all_blocks();
}

///////////////////////////////////////////////////////////////////////
// preprocessor should call this anytime it is about to add text to string
// that comes from a different source, line number, etc.
///////////////////////////////////////////////////////////////////////
void ParseNativeInfiniteString::RecordSourceInfoForNextAppendedCharacter(void) {
///////////////////////////////////////////////////////////////////////

	char *temp = SourceInfoAtAppendPoint();

	char *srcinfo=NULL;

	if(temp != NULL) {
		int len = strlen(temp);
		srcinfo = new char [len+1];
		strcpy(srcinfo,temp);
	}
	 
	// if there was a string there already, garbage collect it, then put new string there.
	if(_last_block_ptr->sourceinfo[_index_to_null_char_in_last_block_string] != NULL) {
		delete _last_block_ptr->sourceinfo[_index_to_null_char_in_last_block_string];
	}

	_last_block_ptr->sourceinfo[_index_to_null_char_in_last_block_string]=srcinfo;
}

///////////////////////////////////////////////////////////////////////
// preprocessor will return a string that indicates the current file info at append point
///////////////////////////////////////////////////////////////////////
char * ParseNativeInfiniteString::SourceInfoAtAppendPoint(void) {
///////////////////////////////////////////////////////////////////////
	return NULL; // no source info for this class, but extended class might know something
}

///////////////////////////////////////////////////////////////////////
// infinite string will return the source info at or before the current marker.
///////////////////////////////////////////////////////////////////////
char * ParseNativeInfiniteString::SourceInfoAtMarkerPoint(void) {
///////////////////////////////////////////////////////////////////////
	for(int index=_index_to_current_character_in_current_block; index>=0; index--) {
		if(_current_block_ptr->sourceinfo[index] != NULL) {
			return _current_block_ptr->sourceinfo[index];
		}
	}
	return "ParseNativeInfiniteString::SourceInfoAtMarkerPoint. No source information is available\n";
}


///////////////////////////////////////////////////////////////////////
// use this to dump out all the blocks in the object/instance
///////////////////////////////////////////////////////////////////////
void ParseNativeInfiniteString::dump_blocks(void) {
///////////////////////////////////////////////////////////////////////
	cout << endl << "ParseNativeInfiniteString::dump_blocks"<<endl << "----------" << endl; 
	
	ParseNativeLinkedListBlock * curr_block = _first_block_ptr;

	int blockcounter=0;

	char number [20];
	char * ptr;
	char letter;
	while(1) {
		cout << "BLOCK # "<<blockcounter++<<endl;
		ptr = curr_block->textarr;
		for(unsigned int i=0; i<BLOCKSIZE; i++) {
			letter = ptr[i];
			sprintf(number, "%8d", i);
			cout << number << " : ";
			if((letter >= '!') and (letter <= '~')) {
				cout << letter ;
			} else {
				cout << '.';
			}
			cout << " ";

			char *info = curr_block->sourceinfo[i];
			if(info != NULL) {
				cout << info;
			}

			cout << endl;
		}
		cout << endl;
		if(curr_block->next==NULL) { 
			cout << "----------" << endl << endl;
			return;
		}
		curr_block = curr_block->next;
	}
	
}




///////////////////////////////////////////////////////////////////////
void ParseNativeInfiniteString::AppendCharacter(char userchar) {
///////////////////////////////////////////////////////////////////////
	// last_block always points to last element in linked list.
	// _index_to_null_char_in_last_block_string always contains the
	// index that points to the first 0 (null) char in the string
	// (i.e. the first place to append a new character to)
	// So, can always put the incoming character here.
	_last_block_ptr->textarr[_index_to_null_char_in_last_block_string]=userchar;

	// now we need to increment the index intelligently
	// and if we're at the last index for this block,
	// we need to create a new block and tie it into linked list.

	// last index into array is [BLOCKSIZE-1]
	// increment the index, and see if it is equal to BLOCKSIZE or not.
	_index_to_null_char_in_last_block_string++;
	
	// if we are pointing to last character in block
	if(_index_to_null_char_in_last_block_string == BLOCKSIZE) {

		// create a new block and tie it into linked list.
		ParseNativeLinkedListBlock * new_block;

		new_block = generate_new_block();

		new_block->prev = _last_block_ptr;
		_last_block_ptr->next = new_block;

		// now set the current block pointer to point to latest block
		_last_block_ptr = new_block;

		// and indicate that index 0 is empty
		_index_to_null_char_in_last_block_string=0;

		// want first character to contain location information
		// in case previous blocks get garbage collected.
		RecordSourceInfoForNextAppendedCharacter();
	}
}


///////////////////////////////////////////////////////////////////////
void ParseNativeInfiniteString::AppendString(char *userstr) {
///////////////////////////////////////////////////////////////////////
	unsigned int i=0;
	while(userstr[i]) {
		AppendCharacter(userstr[i]);
		i++;
	}
}

///////////////////////////////////////////////////////////////////////
// This is a virtual function.
///////////////////////////////////////////////////////////////////////
// this method will be called by IncrementMarker if we hit end of string.
// derivative classes will override this method and add their own logic to 
// get next chunk of string and call AppendString.
// if AutoAppendText added more string, return true.
// else return false to indicate no more text to process.
// In this class, user manually adds text with AppendString.
///////////////////////////////////////////////////////////////////////
bool ParseNativeInfiniteString::AutoAppendText(void) {
///////////////////////////////////////////////////////////////////////
	return false;
}

///////////////////////////////////////////////////////////////////////
// return the character we are currently pointing to
///////////////////////////////////////////////////////////////////////
char ParseNativeInfiniteString::CurrentCharacter(void) {
///////////////////////////////////////////////////////////////////////
	if(_current_block_ptr->textarr[_index_to_current_character_in_current_block]==0) {
		AutoAppendText();
	}
	return  _current_block_ptr->textarr[_index_to_current_character_in_current_block];
}


///////////////////////////////////////////////////////////////////////
// increment pointer so it is pointing to next character.
///////////////////////////////////////////////////////////////////////
bool ParseNativeInfiniteString::IncrementMarker(void) {
///////////////////////////////////////////////////////////////////////

	// if we're pointing to a NULL character, we're at end of string
	// and we can't increment anything. see if we can AutoAppendText
	// and if we can't, we're at the true end of string.
	if(_current_block_ptr->textarr[_index_to_current_character_in_current_block]==0){
		if(AutoAppendText()==false) {
			return false;
		}
	}

	// increment the index, then see if it's too far
	_index_to_current_character_in_current_block++;

	// if we went past valid indexes, we've gone too far.
	if(_index_to_current_character_in_current_block==BLOCKSIZE) {

		// if there is another element in linked list
		if(_current_block_ptr->next != NULL) {
			// go to that element
			_current_block_ptr = _current_block_ptr->next;
			_index_to_current_character_in_current_block=0;

			// see if we can garbage collect previous block:
			// if we've parsed into the next block in linked list
			// AND if there are no _marker_stack, then user doesn't
			// want to save previous block. delete it.
			// NOTE: the only way you can backtrack is by saving a 
			// marker to the stack and restoring it. otherwise,
			// you plow ahead one character at a time.

		 	if(_marker_stack.empty()) {
				garbage_collect_block_and_all_predecessors(_current_block_ptr->prev);

				_current_block_ptr->prev = NULL;
				_first_block_ptr   = _current_block_ptr;		
			}
		} else {
			// else we are at end of linked list, can't increment anything
			_index_to_current_character_in_current_block--;
		}
	}


	return true;
}



///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
ParseNativeMarkerType ParseNativeInfiniteString::GetCurrentMarkerPosition(void) {
///////////////////////////////////////////////////////////////////////
	ParseNativeMarkerType save;

	save.blockptr = _current_block_ptr;
	save.index = _index_to_current_character_in_current_block;
	return save;
}


///////////////////////////////////////////////////////////////////////
void ParseNativeInfiniteString::SetCurrentMarkerPosition(ParseNativeMarkerType marker) {
///////////////////////////////////////////////////////////////////////
	_current_block_ptr = marker.blockptr;
	_index_to_current_character_in_current_block = marker.index;
}


///////////////////////////////////////////////////////////////////////
void ParseNativeInfiniteString::DumpMarker(ParseNativeMarkerType marker){
///////////////////////////////////////////////////////////////////////
	cout << endl << "dumping contents of marker" << endl;
	int index = marker.index;
	cout << "index = " << index << endl;
	for (unsigned int i=0; i<BLOCKSIZE; i++) {
		char letter = marker.blockptr->textarr[i];
		cout << letter;
	}
	cout << endl << endl;
}

///////////////////////////////////////////////////////////////////////
unsigned int ParseNativeInfiniteString::NumifyMarker(ParseNativeMarkerType marker){
///////////////////////////////////////////////////////////////////////
	ParseNativeLinkedListBlock *blockptr = _first_block_ptr;

	unsigned int count=0;

	while(blockptr != marker.blockptr) {
		count += BLOCKSIZE;

		if(blockptr->next == NULL) {
			cout << "ERROR: tried to numify a marker that does not exist anymore" << endl;
			return 0;
		}

		blockptr = blockptr->next;
	}
	
	count += marker.index;

	return count;
}

///////////////////////////////////////////////////////////////////////
// figure out the distance between two markers and sort the markers in sequential order
///////////////////////////////////////////////////////////////////////
unsigned int ParseNativeInfiniteString::DistanceBetweenMarkers(
///////////////////////////////////////////////////////////////////////
	ParseNativeMarkerType &marker1,ParseNativeMarkerType &marker2
) {
	ParseNativeMarkerType tempmarker;

	unsigned int numify1 = NumifyMarker(marker1);
	unsigned int numify2 = NumifyMarker(marker2);
	unsigned int retval;

	if(numify1>numify2) {
		retval = numify1 - numify2 + 1;

		tempmarker = marker1;
		marker1 = marker2;
		marker2 = tempmarker;

		return retval;
	} else {
		retval = numify2 - numify1 + 1;
		return retval;
	}		
	return 0;
} 


///////////////////////////////////////////////////////////////////////
char*	ParseNativeInfiniteString::StringifyMarkers(
		ParseNativeMarkerType marker1, ParseNativeMarkerType marker2
){
///////////////////////////////////////////////////////////////////////

	unsigned int strlen = DistanceBetweenMarkers(marker1, marker2);

	char *strptr = new char [strlen+1];
	unsigned int nullindex = 0;
	unsigned int iter;

	ParseNativeLinkedListBlock *blockptr = marker1.blockptr;

	while(blockptr != marker2.blockptr) {
		for(iter=0; iter<BLOCKSIZE; iter++) {
			strptr[nullindex++] = blockptr->textarr[iter];
		}
		if(blockptr->next == NULL) {
			cout << "ERROR: StringifyMarkers encountered unexpected end of blocks" << endl;
			strptr[nullindex++]=0;
			return strptr;
		}

		blockptr = blockptr->next;
	}

	for(iter=0; iter<=marker2.index; iter++) {
		strptr[nullindex++] = blockptr->textarr[iter];
	}


	strptr[nullindex++]=0;
	return strptr;
}

///////////////////////////////////////////////////////////////////////
// use save/restore/discard when you want to try parsing something
// and it may fail. save, try to parse, then restore if failed.
// or discard if passed.
//
//struct ParseNativeMarkerType {
//	ParseNativeLinkedListBlock *pointer;
//	unsigned int index;
//};
//
///////////////////////////////////////////////////////////////////////
void ParseNativeInfiniteString::SaveCurrentMarkerPosition(void) {
///////////////////////////////////////////////////////////////////////
	_marker_stack.push(GetCurrentMarkerPosition());
}


///////////////////////////////////////////////////////////////////////
// whatever you attempted to match, failed. Go back to previous marker.
///////////////////////////////////////////////////////////////////////
void ParseNativeInfiniteString::RestoreMostRecentlySavedMarkerPosition(void){
///////////////////////////////////////////////////////////////////////
	ParseNativeMarkerType marker;

	if(_marker_stack.empty()) {
		cout << "Error: tried to RestoreMostRecentlySavedMarkerPosition ";
		cout << "before any markers had been saved" << endl;
		exit(1);
	}

	marker = _marker_stack.top();_marker_stack.pop();
	_current_block_ptr = marker.blockptr;
	_index_to_current_character_in_current_block = marker.index;
}


///////////////////////////////////////////////////////////////////////
// whatever you attempted to match, passed. Don't need to keep marker.
///////////////////////////////////////////////////////////////////////
void ParseNativeInfiniteString::DiscardMostRecentlySavedMarkerPosition(void){
///////////////////////////////////////////////////////////////////////
	ParseNativeMarkerType marker;

	if(_marker_stack.empty()) {
		cout << "Error: tried to DiscardMostRecentlySavedMarkerPosition ";
		cout << "before any pointers had been saved" << endl;
		exit(1);
	}
	marker = _marker_stack.top();_marker_stack.pop();
}




