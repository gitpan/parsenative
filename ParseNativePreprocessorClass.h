// copyright 2007 Greg London
// All Rights Reserved

#ifndef ParseNativePreprocessorClass_h
#define ParseNativePreprocessorClass_h

#include <stdio.h>
using namespace std;

#include "ParseNativeInfiniteString.h"

class ParseNativePreprocessorClass : public ParseNativeInfiniteString {

	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////
	public:
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////

	//ParseNativePreprocessorClass(void);
	//virtual ~ParseNativePreprocessorClass(void);

	void filename(char * file_name);

	virtual bool AutoAppendText(void);

	virtual char * SourceInfoAtAppendPoint();

	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////
	private: // Can't be accessed by anyone but the class.
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////
		FILE *_myfile;
	
};



#endif
