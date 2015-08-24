#ifndef SCENARIO_H
 #define SCENARIO_H

#include <fstream> 
#include <string>
#include "test_process.h"

typedef unsigned long int time_int;

class Scenario
{
	public:
		typedef enum 
		{
			om_like,
			om_notlike,
			om_equal,
			om_notequal,
			om_none
		} output_match;
		Scenario(){}
	    Scenario(std::string p, std::string o, std::string i, int ex, output_match om, time_int min, time_int max, bool mws);
		void PerformTest();
		
		short getResult() const;
		time_int getDuration() const;
		
		void SaveToLog(std::ofstream &log, unsigned);
		
	private:
		std::string test_path; // sciezka + argumenty
		std::string output; //spdziewne wyjście
		std::string input;  //podane wejście
		int exit_code;      //spodziewany kod zakończenia
		bool merge_ws;      //ciągi i inne białe znaki sa łączone do jednej spacji
		output_match match;
		
		time_int execution_min, execution_max, execution_real; //zakresy wykonania
		
		bool result; //ogólny wynik testu
		short escape_error;//błąd uruchamiania testu
		
		std::string result_output;  //rzeczywiste wejście
		int result_exit_code;       //rzeczywisty kod zakończenia
		
		void AnalyseResults(TestProcess &t);
		void EscapeChars(std::string &str, bool input);
		void TrimExtraWhiteSpaces(std::string &str);
};
 
#endif