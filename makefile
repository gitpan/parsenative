

CC=g++
CFLAGS=-g -Wall -D__USE_FIXED_PROTOTYPES__ -ansi



main: main.cpp ParseNativeInfiniteString.h ParseNativeInfiniteString.cpp ParseNativePreprocessorClass.h ParseNativePreprocessorClass.cpp ParseNativeRuleCompiler.h  ParseNativeRuleCompiler.cpp ParseNativeRuleRunner.h  ParseNativeRuleRunner.cpp
	${CC} ${CFLAGS} -o main main.cpp ParseNativeInfiniteString.cpp ParseNativePreprocessorClass.cpp ParseNativeRuleCompiler.cpp ParseNativeRuleRunner.cpp

clean: 
	rm -f main
