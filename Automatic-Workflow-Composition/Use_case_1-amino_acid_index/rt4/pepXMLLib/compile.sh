gcc -c -O3 IOFunctions.c
gcc -c -O3 StringFunctions.c
gcc -c -O3 XMLFunctions.c
gcc -c -O3 pepXMLDelegates.c
gcc -c -O3 PeptideProphetDelegate.c
gcc -c pepXMLParser.c -ggdb
gcc -c -O3 pepXMLReader.c
gcc -c -O3 pepXMLWriteFunctions.c
gcc -c -O3 pepXMLWriter.c
ar rcs libpepXML.a *.o
rm *.o

