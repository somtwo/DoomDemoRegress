#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "SuperEasyJson\json.h"
#include "DemoTest.h"
#include <stdio.h>

char *ReadFile(FILE *f);

json::Value ReadJsonFile(char *name);

void ReadTest(json::Value value, Test *test);

TestLibrary *ReadLibrary(char *fileName);

Target *ReadTarget(const char *name);

#endif