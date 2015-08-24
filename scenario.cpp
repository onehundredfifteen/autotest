#include <iostream>
#include <algorithm>
#include <chrono>

#include "scenario.h"

Scenario::Scenario(std::string p, std::string o, std::string i, int ex, output_match om, time_int min, time_int max, bool mws)
{
	test_path = p;
	output = o;
	input = i;
	exit_code = ex;
	match = om;
	
	execution_min = min;
	execution_max = max;
	execution_real = 0;
	
	merge_ws = mws;
	
	result = false;
	escape_error = 0;
	
	EscapeChars(input, true);
	EscapeChars(output, false);
}

void Scenario::PerformTest()
{
	auto cl_start = std::chrono::system_clock::now();

	TestProcess test(test_path, input);
	
	execution_real = (std::chrono::duration_cast< std::chrono::milliseconds > (std::chrono::system_clock::now() - cl_start)).count();
	
	if(test.getError())
	{
		std::cout << "!> Error! Escape code = " << test.getError() << "\t";
		escape_error = test.getError();
	}
	else
	{
	    test.GetRunResult(result_output, result_exit_code);
		AnalyseResults(test);
	}
}

short Scenario::getResult() const
{
	if(escape_error != 0)
		return -escape_error;
	
	return result ? 1 : 0;
}

time_int Scenario::getDuration() const
{
	return execution_real;
}

void Scenario::AnalyseResults(TestProcess &t)
{
	bool ex_ok, ro_ok, ti_ok;
	
	if(merge_ws == true)
	{
		TrimExtraWhiteSpaces(output);
		TrimExtraWhiteSpaces(result_output);
	}
	
	//exit-code
	ex_ok = (result_exit_code == exit_code);
	
	//output
	switch(match)
	{
		case om_like:
			ro_ok = (result_output.find(output) != std::string::npos);
			break;
		case om_notlike:
			ro_ok = (result_output.find(output) == std::string::npos);
			break;
		case om_equal:
			ro_ok = (result_output == output);
			break;
		case om_notequal:
			ro_ok = (result_output != output);
			break;
		case om_none:
		default:
		    ro_ok = true;
	}
	
	//timing
	ti_ok = ( execution_min ? execution_real >= execution_min : true ) && ( execution_max ? execution_real <= execution_max : true );
	
	//wynik
	result = ( ex_ok && ro_ok && ti_ok);
}

void Scenario::EscapeChars(std::string &str, bool input)
{
  /*
  Zamienia \n na char('\r\n') , \t, \\
  */
  std::size_t pos = 0;
  
  /* Kłopoty z nowymi liniami.
   * Dwa znaki \n to <br>, łatwy do wpisania znak nowej linii.
   * \n to char(10)
   * jezeli jest input, nalezy zamienić <BR> na \n, które przez strumień zostanie zamieniony na \r\n
   * jeżeli jest output, to alby porównac do result_output rozwijam <BR> do \r\n
   * Reszta znaków normalnie.
  */
  if(input)
  {
	  while ((pos = str.find("\\n", pos)) != std::string::npos)
	  {
			str.replace(pos, 2, "\n");
	  }
  }
  else
  {
      while ((pos = str.find("\\n", pos)) != std::string::npos)
	  {
			str.replace(pos, 2, "\r\n");
	  }
  }
  
  //tab
  pos = 0;
  while ((pos = str.find("\\t", pos)) != std::string::npos)
  {
			str.replace(pos, 2, "\t");
  }
}

void Scenario::TrimExtraWhiteSpaces(std::string &str)
{
	/*
	* Każdy ciąg biąłych znaków jest zamieniany na jedną spację.
	*/
	std::size_t pos = 0;
	std::replace_if (str.begin(), str.end(), [](char c){return std::iscntrl(c);}, ' '); 
   
    while ((pos = str.find("  ", pos)) != std::string::npos) //każde dwie spacje na jedną
    {
			str.replace(pos, 2, " ");
    }
}

void Scenario::SaveToLog(std::ofstream &log, unsigned test_num)
{
	if(!log.is_open())
		return;
	
	log << "> TEST #" << test_num;
		
	if(this->getResult() < 0)
	{
		log << " NOT COMPLETED.\n"
			<< "Process terminated with escape code [" << -this->getResult() << "]" << std::endl;
		return;
	}
	else if(this->getResult() > 0)
	{
		log << " PASSED." << std::endl;
	}
	else 
	{
		log << " FAILED." << std::endl;
	}
		
	log << "Expected exit code = [" << exit_code << "]\n";
	log << "Actual exit code   = [" << result_exit_code << "]\n";
	log << "Expected output = \n[" << output << "]\n";
	log << "Actual output = \n[" << result_output << "]\n";
	
	log << "Output match method = [";
	switch(match)
	{
		case om_like:
			log << "LIKE";
			break;
		case om_notlike:
			log << "NOT-LIKE";
			break;
		case om_equal:
			log << "EQUAL";
			break;
		case om_notequal:
			log << "NOT-EQUAL";
			break;
		case om_none:
		default:
		    log << "NONE-IGNORE";
	}
	
	log << "]\n";
	log << "Whitespace collapse option was [" << (merge_ws ? "ON" : "OFF") << "]";
	
	if(execution_min || execution_max)
	{
		log << "\n"
		    << "Accepted execution time range = <" << execution_min << "," << execution_max << "> miliseconds.\n";
		log << "Actual execution time = [" << execution_real << "] miliseconds.";
	}	
	log << "\n" << std::endl;	
}

/*
	for(int i = 0 ; i < output.size(); i++)
	{
		std::cout << "OUT[" << i << "]=" <<output[i] << " and in digit =" << static_cast<int>(output[i]) << std::endl;
	}
	
	for(int i = 0 ; i < result_output.size(); i++)
	{
		std::cout << "RE[" << i << "]=" <<result_output[i] << " and in digit =" << static_cast<int>(result_output[i]) << std::endl;
	}
*/