// Copyright 2007 Greg London
// All Rights Reserved



#include "ParseNativePreprocessorClass.h"


#include <iostream>
#include <fstream>
#include <string>
using namespace std;

///////////////////////////////////////////////////////////////////////
void ParseNativePreprocessorClass::filename(char * file_name){
///////////////////////////////////////////////////////////////////////
	cout << "ParseNativePreprocessorClass::filename This should really be updated at some point" << endl;

	_myfile = fopen(file_name, "r");
	if (_myfile==NULL) {
		cout << "ERROR: could not open file '" ;
		cout << file_name << "' for read" << endl;
		exit(1);
	}

	RecordSourceInfoForNextAppendedCharacter();
	AutoAppendText();
}

///////////////////////////////////////////////////////////////////////
bool ParseNativePreprocessorClass::AutoAppendText(void) {
///////////////////////////////////////////////////////////////////////
	char *retval;
	char line[256];


	if(feof(_myfile)) {
		return false;
	}

	retval = fgets(line,256,_myfile);

	if (retval==0) {
		if(_myfile!=NULL) {
		//	fclose(_myfile);
		}
		return false;
	}

	//cout <<endl << "line is '" << line << "'" << endl;
	ParseNativeInfiniteString::AppendString(line);

	return true;
}


char * ParseNativePreprocessorClass::SourceInfoAtAppendPoint(){
	return "some file";
}

