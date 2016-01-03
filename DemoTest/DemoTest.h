#ifndef DEMOTEST_H
#define DEMOTEST_H

typedef struct
{
	char *testName;

	char *demoName;

	char *iWad;

	int pWadCount;
	char **pWads;

	char *logFile;
} Test;

typedef struct
{
	int testCount;
	Test *test;
} TestLibrary;

typedef struct
{
	char *name;
	char *path;
} IWad;

typedef struct
{
	char *executable;

	char *demoSwitch;
	char *logSwitch;

	int iwadCount;
	IWad **iwads;

	char *additionaloptions;
} Target;

void assert(bool value, char *message);
#endif