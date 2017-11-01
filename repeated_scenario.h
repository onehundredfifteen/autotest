#ifndef REPEATED_SCENARIO_H
 #define REPEATED_SCENARIO_H

#include <vector>
#include "scenario.h"

class RepeatedScenario : public Scenario
{
	public:
	    RepeatedScenario(Scenario * scenario, int trepeat);
				 
		virtual ~RepeatedScenario();
		
		virtual void PerformTest();
		virtual run_result getTestResult();
		
		virtual void printSummary(std::stringstream &ss);
		
	private:	
		int repeat;
		Scenario * scenarios;
		std::vector<time_int> times;
		
	private:
		virtual void printBrief(std::ostream &stream);
};
 
#endif