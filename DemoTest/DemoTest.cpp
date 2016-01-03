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

	sprintf(buffer, "\"%s\" -iwad %s %s %s %s %s %s", target->executable, target->iwads[i]->path, target->demoSwitch, test->demoName, target->logSwitch, outputFile, target->additionaloptions);
	return strdup(buffer);
}

int CompareResults(char *cannon, char *test)
{
	char cbuffer[1024];
	char tbuffer[1024];

	FILE *cannonFile = fopen(cannon, "r");
	FILE *testFile = fopen(test, "r");

	bool success = true;
	int lineNumber = 1;

	while (1)
	{
		char *cline = fgets(cbuffer, 1024, cannonFile);
		char *tline = fgets(tbuffer, 1024, testFile);

		if (cline == NULL || tline == NULL)
		{
			if (cline != tline)
				success = false;

			break;
		}

		if (stricmp(cline, tline) != 0)
		{
			success = false;
			break;
		}

		lineNumber++;
	}

	fclose(cannonFile);
	fclose(testFile);

	return success ? 0 : lineNumber;
}

void MoveToErrorFolder(char *fileName)
{

}

void DeleteTestFile(char *fileName)
{
	assert(remove(fileName) == 0, "Failed to delete file.");
}

bool RunTest(Test *test, Target *target)
{
	char buffer[1024];
	sprintf(buffer, "t_%s.txt", test->testName);

	char *testOutput = buffer;

	char *command = BuildCommand(test, target, testOutput);
	if (command == NULL)
		return false;

	auto result = system(command);

	if (result != 0)
	{
		printf("Command failed to execute:\n%s\n", command);
		free(command);
		return false;
	}

	free(command);

	result = CompareResults(test->logFile, testOutput);

	if (result != 0)
	{
		printf("*** Test failure: %s failed on line %i\n", test->demoName, result);
		printf("Moving file %s to error folder.\n", testOutput);
		MoveToErrorFolder(testOutput);
		return false;
	}
	else
	{
		printf("Success: %s completed successfully.\n", test->demoName);
		DeleteTestFile(testOutput);
		return true;
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

	bool success = true;
	for (int i = 0; i < library->testCount; ++i)
	{
		success = success && RunTest(&library->test[i], target);
	}

	printf("Test run was %s.\n", success ? "successful" : "not successful");
	printf("--- Tests complete.\n");

	getchar();
	return 0;
}

