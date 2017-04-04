//CS485 Systems Programming
//Project 1
//Ethan Gibson
//January 19th, 2017

//File Combiner
//Purpose: Combine files in argument fields using the given spec and print to stdout

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ARGSIZE 1002
#define NSIZE 1000
#define MAXSTDIN 1024
#define BUFFSIZE 1024

//Determines whether file field is a filename or STDIN
int fileType(char *fileName)
{
	//IF filename indicates stdin, return 0
	if(strncmp(fileName,"-",1) == 0)
	{
		return 0;
	}
	//If not stdin, then try to open the file
	else
	{
		FILE *inFile;
		inFile = fopen(fileName, "r");

		//If the file opens, close it and return 1
		if(inFile)
		{
			fclose(inFile);
			return 1;
		}
		//Else, print the error to stdout and return -1
		else
		{
			perror("File Failure");
			return -1;
		}
	}
	//If the program somehow runs outside of this else (it should never)
	//print and error and return -1 to signify failure.
	perror("File Failure");
	return -1;
}

//Gets the N value from the -s and -e specifications
int getN(char *specArg)
{
	//initialize a string and copy the spec into it
	//also get the length of the string
	char numCharstr[NSIZE];
	strcpy(numCharstr, specArg);	
	int specLength = strlen(numCharstr);

	int specIndex;
	int result = 0;

	//for every character after the first two, convert the value
	//into an integer for later use
	for(specIndex = 2; specIndex < specLength; specIndex += 1)
	{	
		result = result * 10 + (numCharstr[specIndex] - '0');
	}
	
	//return the N value from the spec
	return result;
}

//Get the size of the file
int getFileSize(char *fileName)
{
	//initialize a file and seek to the end. Save the position
	//number at the end as the file size.
	FILE *inFile;
	inFile = fopen(fileName, "rb");
	fseek(inFile, 0L, SEEK_END);
	int fileSize = ftell(inFile);

	//rewind the file so it can be used again later. Close it.
	//return the file size.
	rewind(inFile);
	fclose(inFile);
	return fileSize;
}

//Determine which specification was used.
int findSpec(char *spec)
{
	//If -a used, return 0
	if(strncmp(spec,"-a",2) == 0)
	{
		return 0;
	}
	//If -s, return 1
	else if(strncmp(spec, "-s",2) == 0)
	{
		return 1;
	}
	//If -e, return 2
	else if(strncmp(spec, "-e",2) == 0)
	{
		return 2;
	}
	//If the spec field has any other combination of
	//characters as the first two, print the error
	//and return -1.
	else
	{
		perror("Spec Failure");
		return -1;
	}

	//If the program somehow makes it past the if statements,
	//return -1
	return -1;
}

//Take input from stdin and print it
void prntStdIn(char *fileName, int specArgInt, char *specArg)
{
	//Create a buffer to hold the string
	char buffer[BUFFSIZE];
	
	//If the integer value of the specification argument
	if(specArgInt == 0)
	{
		//Initialize a string and the size for the content
		size_t contentSize = 1;
		char *content = malloc(sizeof(char) * BUFFSIZE);
		//While lines are being input...
		while(fgets(buffer, BUFFSIZE, stdin))
		{
			//Add the new string to the buffer, reallocate string to the
			//new size. Combing the new string with the buffer
			contentSize += strlen(buffer);
			content = realloc(content, contentSize);
			strcat(content, buffer);
		}
		//Print the string
		printf("%s",content);
	}
	else
	{
		perror("File Failure");
	}
}

//Read from a file and print the designated amout of bits.
void prntFileIn(char *fileName, int specArgInt, char *specArg)
{
	int i;
	int character;
	
	//Initialize a file and open it	
	FILE *inFile;
	inFile = fopen(fileName, "rb");
	
	//Get the N specification value and the file size
	int N = getN(specArg);
	int fileSize = getFileSize(fileName);

	//If the filesize is less than the given N value,
	//change the integer specification value to 0
	//so the entire file will be printed to prevent overflow to stdout	
	if(fileSize<=N)
	{
		specArgInt = 0;
	}

	//If -a spec, print the whole file 1 character at a time
	if(specArgInt == 0)
	{
		for(i=0; i<fileSize; i = i+1)
		{
			character = fgetc(inFile);
			putchar(character);
		}
		fclose(inFile);
	}
	//If -s spec used, print characters 0 through N-1
	else if(specArgInt == 1)
	{
		for(i=0; i<N; i = i+1)
		{
			character = fgetc(inFile);
			putchar(character);
		}	
		fclose(inFile);	
	}
	//If -e spec used, seek to the N-1 place and print
	//from there until the end
	else if(specArgInt == 2)
	{
		fseek(inFile, (fileSize-N-1), SEEK_SET);
		
		for(i=0; i<N; i=i+1)
		{
			character = fgetc(inFile);
			putchar(character);
		}
		fclose(inFile);
	}
	//else print an error
	else
	{
		perror("File Failure");
	}
}

int main(int argc, char *argv[])
{
	//Initialize variables for use
	int argIndex;	
	int fileTypeInt;

	char spec[ARGSIZE];
	char fileName[ARGSIZE];

	int specInt;

	//Iterate through all of the arguments 2 at a time
	//because in format [SPEC] [FILE]
	for(argIndex = 1; argIndex < argc; argIndex += 2)
	{
		//Copy arguments to string variables
		strcpy(spec,argv[argIndex]);
		strcpy(fileName,argv[argIndex+1]);
	
		//Get the file type and spec
		fileTypeInt = fileType(fileName);
		specInt = findSpec(spec);
	
		//If stdin file name used, use the prntStdnIn function
		//to read input and print it to stdout
		if(fileTypeInt == 0)
		{
			prntStdIn(fileName, specInt, spec);
		}
		//If a file is used, use the prntFileIn function
		//to read the file and print the correct amount
		//of characters
		else if(fileTypeInt == 1)
		{
			prntFileIn(fileName, specInt, spec);
		}
		//Otherwise, print an error
		else
		{
			perror("Filename Error");
		}
	}
		
	
	return 0;
}
