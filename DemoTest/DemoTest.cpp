#include <stdio.h>
#include <stdlib.h>
#include "DemoTest.h"
#include "Configuration.h"

void assert(bool value, char *message)
{
	if (value)
		return;

	printf("Error: %s\n", message);
	getchar();
	exit(-1);
}

char *BuildCommand(Test *test, Target *target, char* outputFile)
{
	char buffer[2048];
	int i;

	for (i = 0; i < target->iwadCount; ++i)
	{
		if (!stricmp(test->iWad, target->iwads[i]->name))
			break;
	}
	if (i == target->iwadCount)
	{
		printf("iwad %s could not be found, skipping test.\n", test->iWad);
		return NULL;
	}

	sprintf(buffer, "\"%s\" -iwad \"%s\" %s %s %s \"%s\" %s", target->executable, target->iwads[i]->path, target->demoSwitch, test->demoName, target->logSwitch, outputFile, target->additionaloptions);
	// TODO: Build actual command
	return strdup(buffer);
}

int CompareResults(char *cannon, char *test)
{
	return 0;
}

void MoveToErrorFolder(char *fileName)
{

}

void DeleteTestFile(char *fileName)
{
	assert(remove(fileName) == 0, "Failed to delete file.");
}

void RunTest(Test *test, Target *target)
{
	char *testOutput = "t_doom2_demo1.txt";

	char *command = BuildCommand(test, target, testOutput);
	if (command == NULL)
		return;

	system(command);

	free(command);

	auto result = CompareResults(test->logFile, testOutput);

	if (result != 0)
	{
		printf("*** Test failure: %s failed on line %i\n", test->demoName, result);
		printf("Moving file %s to error folder.\n", testOutput);
		MoveToErrorFolder(testOutput);
	}
	else
	{
		printf("Success: %s completed successfully.\n", test->demoName);
		DeleteTestFile(testOutput);
	}
}

int main(int argc, char** argv)
{
	printf("--- Reading configs.");

	auto library = ReadLibrary("library.json");
	printf("Test library config loaded. Found %i tests.\n", library->testCount);

	auto target = ReadTarget("target.json");
	printf("Target information loaded. Found %i iwads.\n", target->iwadCount);

	printf("--- Running tests.\n");

	for (int i = 0; i < library->testCount; ++i)
	{
		RunTest(&library->test[i], target);
	}

	getchar();
	return 0;
}

