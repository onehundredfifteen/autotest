#ifndef SCENARIO_H
 #define SCENARIO_H

#include <iostream>
#include <sstream> 
#include "test_process.h"
#include "scenario_metadata.h"



class Scenario
{
	public:
		typedef enum 
		{
			r_ok,
			r_fail,
			r_timeout,
			r_error
		} run_result;
	
		typedef enum 
		{
			om_like,
			om_notlike,
			om_equal,
			om_notequal,
			om_none
		} output_match;
		
		Scenario(){}
		
	    Scenario(std::string path, 
				 std::string out, 
				 std::string in, 
				 int texit_code, 
				 output_match om, 
				 time_int tmin, 
				 time_int tmax, 
				 int waitfor,  
				 ScenarioMetadata &sm);
				 
		virtual ~Scenario();
		
		virtual void PerformTest();
		virtual run_result getTestResult();
		
		virtual void printSummary(std::stringstream &ss);

	private:
	 
		std::string test_path; // path to tested exe + args
		std::string output; //expected output template
		std::string input;  //given input
		int exit_code;      //expected exit code
		
		output_match match;
		time_int execution_min, execution_max;// execution_real; //exe times
		int maxwaitfor; //max wait time fot test
		unsigned repeat;
		
		run_result result; 
		std::string result_output;  //real output
	
	public:
		TestProcess * test;
		ScenarioMetadata metadata;
		
	private:	
		run_result AnalyseResults();
		
		void EscapeChars(std::string &str, bool input);
		void TrimExtraWhiteSpaces(std::string &str);
		
	public:	
		virtual void printBrief(std::ostream &stream);
		
		friend std::ostream &operator<<(std::ostream &stream, Scenario &scenario);
};
 
#endif