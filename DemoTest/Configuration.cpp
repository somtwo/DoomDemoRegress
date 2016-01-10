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

void ReadTest(json::Value value, Test *test)
{
	assert(value.GetType() == json::ObjectVal, "Test must be an object");

	auto testConfig = value.ToObject();

	assert(testConfig.HasKey("testname"), "Test must contain a property called 'testname'");
	assert(testConfig.HasKey("demoname"), "Test must contain a property called 'demoname'");
	assert(testConfig.HasKey("iwad"), "Test must contain a property called 'iwad'");
	assert(testConfig.HasKey("logfile"), "Test must contain a property called 'logfile'");

	auto v = testConfig["testname"];
	assert(v.GetType() == json::StringVal, "Test name must be of type string");
	test->testName = strdup(v.ToString().c_str());

	v = testConfig["demoname"];
	assert(v.GetType() == json::StringVal, "Demo name must be of type string");
	test->demoName = strdup(v.ToString().c_str());

	v = testConfig["iwad"];
	assert(v.GetType() == json::StringVal, "iwad must be of type string");
	test->iWad = strdup(v.ToString().c_str());

	v = testConfig["logfile"];
	assert(v.GetType() == json::StringVal, "Log file must be of type string");
	test->logFile = strdup(v.ToString().c_str());

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

	assert(targetObject.HasKey("executable"), "Target must contian property 'executable'");
	assert(targetObject.HasKey("demoswitch"), "Target must contian property 'demoswitch'");
	assert(targetObject.HasKey("logswitch"), "Target must contian property 'logswitch'");
	assert(targetObject.HasKey("iwads"), "Target must contain property 'iwads'");

	auto target = (Target *)malloc(sizeof(Target));

	auto exe = targetObject["executable"];
	assert(exe.GetType() == json::StringVal, "executable must be of type String");

	auto demoswitch = targetObject["demoswitch"];
	assert(demoswitch.GetType() == json::StringVal, "demoswitch must be of type String");

	auto logswitch = targetObject["logswitch"];
	assert(logswitch.GetType() == json::StringVal, "logswitch must be of type String");

	target->executable = strdup(exe.ToString().c_str());
	target->demoSwitch = strdup(demoswitch.ToString().c_str());
	target->logSwitch = strdup(logswitch.ToString().c_str());

	if (targetObject.HasKey("additionaloptions"))
	{
		auto options = targetObject["additionaloptions"];
		assert(options.GetType() == json::StringVal, "additionaloptions must be of type String");
		target->additionaloptions = strdup(options.ToString().c_str());
	}
	else
		target->additionaloptions = NULL;

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