#include <iomanip>
#include <numeric>
#include <algorithm>
#include "repeated_scenario.h"

RepeatedScenario::RepeatedScenario(Scenario * scenario, int trepeat)
{
	 this->scenarios = nullptr;
	 repeat = trepeat;
	 
	 //can be zero
	 if(repeat < 0 || repeat > 1024)
		 repeat = 1;
	 
	 if(repeat)
	 {
		 //clone scenario to repeat
		 this->scenarios = new Scenario[repeat];
		 
		 for(int i = 0; i < repeat; ++i)
			 scenarios[i] = *scenario;
	 }
}

RepeatedScenario::~RepeatedScenario()
{
	if(this->scenarios)
		delete [] this->scenarios;
}

void RepeatedScenario::PerformTest()
{
	for(int i = 0; i < repeat; ++i)
	{
		scenarios[i].PerformTest();
	
		times.push_back( scenarios[i].test->getTime() );
	}
}

Scenario::run_result RepeatedScenario::getTestResult() 
{
	for(int i = 0; i < repeat; ++i)
	{
		if(scenarios[i].getTestResult() != Scenario::r_ok)
			return scenarios[i].getTestResult();
	}

	return Scenario::r_ok;
}


void RepeatedScenario::printSummary(std::stringstream &ss)
{
	for(int i = 0; i < repeat; ++i)
	{
		ss << "\n\tREPEATING " << i+1 << "/" << repeat ;
		scenarios[i].printSummary(ss);
	}
}

void RepeatedScenario::printBrief(std::ostream &stream)
{
	float avgtime;
	
	stream << std::setw(16) << (this->scenarios[0].metadata.name.empty() ? this->scenarios[0].metadata.name : std::string("\""+this->scenarios[0].metadata.name+"\"")) << ": ";
	
	if(repeat == 0)
	{
		stream << "ignoring... [repeat=0]";
	}
	else{
		avgtime  = (float)std::accumulate(this->times.begin(), this->times.end(), 0) / (float)this->times.size();
		
		stream << "min " << *std::min_element(this->times.begin(), this->times.end())
				<< " max " << *std::max_element(this->times.begin(), this->times.end())
				<< " avg " << std::setprecision(3) << avgtime;	
	}
	
	for(int i = 0; i < repeat; ++i)
	{
		stream << "\n" << std::setw(15) << "." << i+1;
		scenarios[i].printBrief(stream);
	}
}