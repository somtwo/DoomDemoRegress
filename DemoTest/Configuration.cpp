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

json::Value ReadJsonFile(char *name)
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

	assert(testConfig.HasKey("demoname"), "Test must contain a property called 'demoname'");
	assert(testConfig.HasKey("iwad"), "Test must contain a property called 'iwad'");
	assert(testConfig.HasKey("logfile"), "Test must contain a property called 'logfile'");

	auto v = testConfig["demoname"];
	assert(v.GetType() == json::StringVal, "Demo name must be of type string");
	test->demoName = strdup(v.ToString().c_str());

	v = testConfig["iwad"];
	assert(v.GetType() == json::StringVal, "iwad must be of type string");
	test->iWad = strdup(v.ToString().c_str());

	v = testConfig["logfile"];
	assert(v.GetType() == json::StringVal, "Log file must be of type string");
	test->logFile = strdup(v.ToString().c_str());
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
	// Hard code some values for now
	auto target = (Target *)malloc(sizeof(Target));
	target->executable = "D:\\Games\\Doom\\Eternity\\eternity.exe";
	target->demoSwitch = "-timedemo";
	target->logSwitch = "-levellog";
	target->additionaloptions = "-nodraw -nosound -nowait";

	target->iwadCount = 1;
	target->iwads = (IWad **)malloc(sizeof(IWad*));
	target->iwads[0] = (IWad *)malloc(sizeof(IWad));
	target->iwads[0]->name = "doom2.wad";
	target->iwads[0]->path = "C:\\Games\\Doom\\doom2.wad";

	return target;
}