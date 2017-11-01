#ifndef SCENARIO_METADATA_H
 #define SCENARIO_METADATA_H

#include <string>


struct ScenarioMetadata
{
	std::string name;
	std::string comment;
	bool merge_ws;
	unsigned test_number;
};
 
#endif