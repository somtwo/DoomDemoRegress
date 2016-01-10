#include <stdio.h>
#include <stdlib.h>
#include "SuperEasyJson\json.h"
#include "DemoTest.h"

char *ReadFile(FILE *f)
{
	fseek(f, 0L, SEEK_END);
	auto size = ftell(f);
	fseek(f, 0L, SEEK_SET);

	char *buffer = (char *)malloc(sizeof(char) * (size + 1));

	auto read = fread(buffer, sizeof(char), size, f);
	buffer[read] = 0;
	return buffer;
}

json::Value ReadJsonFile(const char *name)
{
	printf("Reading %s\n", name);

	auto file = fopen(name, "r");
	assert(file != (void *)0, "Failed to open json file.");

	auto buffer = ReadFile(file);

	return json::Deserialize(buffer);
}

char *ReadJsonString(json::Object &object, const char *name, const char *missingMessage, const char *typeMessage)
{
	assert(object.HasKey(name), missingMessage);

	auto v = object[name];
	assert(v.GetType() == json::StringVal, typeMessage);

	return strdup(v.ToString().c_str());
}

void ReadTest(json::Value value, Test *test)
{
	assert(value.GetType() == json::ObjectVal, "Test must be an object");

	auto testConfig = value.ToObject();

	test->testName = ReadJsonString(testConfig, "testname", "Test must contain a property called 'testname'", "testname must be of type String");
	test->demoName = ReadJsonString(testConfig, "demoname", "Test must contain a property called 'demoname'", "demoname must be of type String");
	test->iWad     = ReadJsonString(testConfig, "iwad",     "Test must contain a property called 'iwad'",     "iwad must be of type String");
	test->logFile  = ReadJsonString(testConfig, "logfile",  "Test must contain a property called 'logfile'",  "logfile must be of type String");

	// TODO: support pwads
}

TestLibrary *ReadLibrary(char *fileName)
{
	auto json = ReadJsonFile(fileName);

	assert(json.GetType() == json::ArrayVal, "Test library must be an array.");

	auto library = json.ToArray();
	auto result = (TestLibrary *)malloc(sizeof(TestLibrary));
	result->testCount = library.size();

	assert(result->testCount > 0, "Test library contained no tests.");

	result->test = (Test *)malloc(sizeof(Test) * result->testCount);
	for (int i = 0; i < result->testCount; ++i)
	{
		ReadTest(library[i], &result->test[i]);
	}

	return result;
}

Target *ReadTarget(const char *name)
{
	auto json = ReadJsonFile(name);

	assert(json.GetType() == json::ObjectVal, "Target must be an object.");
	auto targetObject = json.ToObject();

	auto target = (Target *)malloc(sizeof(Target));

	target->executable = ReadJsonString(targetObject, "executable", "Target must contain a property called 'executable'", "executable must be of type String");
	target->demoSwitch = ReadJsonString(targetObject, "demoswitch", "Target must contain a property called 'demoswitch'", "demoswitch must be of type String");
	target->logSwitch  = ReadJsonString(targetObject, "logswitch",  "Target must contain a property called 'logswitch'",  "logswitch must be of type String");

	if (targetObject.HasKey("additionaloptions"))
	{
		target->additionaloptions = ReadJsonString(targetObject, "additionaloptions", "How did you get this?", "additionaloptions must be of type String");
	}
	else
		target->additionaloptions = NULL;

	assert(targetObject.HasKey("iwads"), "Target must contain property 'iwads'");

	auto iwads = targetObject["iwads"];
	assert(iwads.GetType() == json::ObjectVal, "iwads must be of type Object");

	auto iwadsObj = iwads.ToObject();
	assert(iwadsObj.size() > 0, "iwads must contain at least one iwad");

	target->iwadCount = iwadsObj.size();
	target->iwads = (IWad **)malloc(sizeof(IWad*) * target->iwadCount);

	int i = 0;
	for (auto it = iwadsObj.begin(); it != iwadsObj.end(); it++)
	{
		assert(it->second.GetType() == json::StringVal, "iwad path must be of type String");

		target->iwads[i] = (IWad *)malloc(sizeof(IWad));
		target->iwads[i]->name = strdup(it->first.c_str());
		target->iwads[i]->path = strdup(it->second.ToString().c_str());
		i++;
	}

	return target;
}