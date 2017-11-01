#include <algorithm>
#include <iomanip>
#include "scenario.h"

Scenario::Scenario(std::string path, std::string out, std::string in, int texit_code, 
                   output_match om, time_int tmin, time_int tmax, int waitfor,  
				   ScenarioMetadata &sm)
{
	test = nullptr;
	
	test_path = path;
	output = out;
	input = in;
	exit_code = texit_code;
	match = om;
	
	execution_min = tmin;
	execution_max = tmax;
	
	maxwaitfor = waitfor;
	
	metadata = sm;
	
	if(execution_max > maxwaitfor)
		maxwaitfor = execution_max;
	
	result = r_error;
	
	EscapeChars(input, true);
	EscapeChars(output, false);
}

Scenario::~Scenario()
{
	if(test)
		delete test;
}

void Scenario::PerformTest()
{
	test = new TestProcess(test_path, input, maxwaitfor);
	
	result = AnalyseResults();
}

Scenario::run_result Scenario::getTestResult()
{
	return result;
}

Scenario::run_result Scenario::AnalyseResults()
{
	bool ex_ok, ro_ok, ti_ok;
	
	if(test == nullptr || test->getError())
	{
		return r_error;
	}
	
	result_output = test->getOutputString();
	
	if(metadata.merge_ws == true)
	{
		TrimExtraWhiteSpaces(output);
		TrimExtraWhiteSpaces(result_output);
	}
	
	//exit-code
	ex_ok = (test->getExitCode() == exit_code);
	
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
	ti_ok = ( execution_min ? test->getTime() >= execution_min : true ) && ( execution_max ? test->getTime() <= execution_max : true );
	
	//result
	if( ex_ok && ro_ok && ti_ok && test->isTimeout() == false)
		return r_ok;
    else if(test->isTimeout())
		return r_timeout;
	
	return r_fail;
}

void Scenario::EscapeChars(std::string &str, bool input)
{
  /*
  For input
  Replace "\n" to newline
  Replace \t to TAB
  Replace \{char_code} to single char of given code, where char_code is 3 digit max
  
  for output
  Replace newline to Windows newline 
  */
  const short char_esc_len = 2;
  std::size_t pos = 0, pos2;
  char charnum;
  
  if(input)
  {
	  while ((pos = str.find("\\n", pos)) != std::string::npos)
	  {
			str.replace(pos, 2, "\n");
	  }
	  
	  //tab
	  pos = 0;
	  while ((pos = str.find("\\t", pos)) != std::string::npos)
	  {
				str.replace(pos, 2, "\t");
	  }
	  
	  //chars
	  pos = 0;
	  while ((pos = str.find("\\{", pos)) != std::string::npos)
	  {
				if( (pos2 = str.find("}", pos) ) != std::string::npos )
				{
					if( pos2-(pos+char_esc_len) < 4 && pos2-(pos+char_esc_len) > 0)
					{
						charnum = (char)atoi( str.substr(pos+char_esc_len, pos2-(pos+char_esc_len) ).c_str() );
						str.erase (pos+1, pos2-pos);
						str[pos] = charnum;	
				    }
					else
					{
						str.erase (pos, pos2); 
					}
				}
				else break;
	  }
  }
  else
  {
      while ((pos = str.find("\\n", pos)) != std::string::npos)
	  {
			str.replace(pos, 2, "\r\n");
	  }
  }

}

void Scenario::TrimExtraWhiteSpaces(std::string &str)
{
	/*
	* Merge a series of whitespaces to just single space
	*/
	std::size_t pos = 0;
	std::replace_if (str.begin(), str.end(), [](char c){return std::iscntrl(c);}, ' '); 
   
    while ((pos = str.find("  ", pos)) != std::string::npos)
    {
			str.replace(pos, 2, " ");
    }
}


void Scenario::printSummary(std::stringstream &ss)
{
	ss << "> TEST #" << metadata.test_number << "; " << metadata.name << "\n\t\"" << metadata.comment << "\"\n";
		
	ss << "Arguments = [" << test_path << "]\n";
	ss << "Expected exit code = [" << exit_code << "]\n";
	ss << "Real exit code   = [" << test->getExitCode() << "]\n";
	ss << "Expected output = \n[" << output << "]\n";
	ss << "Real output = \n[" << result_output << "]\n";
	
	ss << "Output match method [";
	switch(match)
	{
		case om_like:
			ss << "LIKE";
			break;
		case om_notlike:
			ss << "NOT-LIKE";
			break;
		case om_equal:
			ss << "EQUAL";
			break;
		case om_notequal:
			ss << "NOT-EQUAL";
			break;
		case om_none:
		default:
		    ss << "NONE-IGNORE";
	}
	
	ss << "]; ";
	ss << "Whitespace merge option [" << (metadata.merge_ws ? "ON" : "OFF") << "]";
	
	if(execution_min || execution_max)
	{
		ss << "\n"
		   << "Accepted execution time range = <" << execution_min << "," << execution_max << "> miliseconds.\n";
		ss << "Real execution time = [" << test->getTime() << "] miliseconds.";
	}	
	
	if(test->isTimeout())
	{
		ss << "\n"
		   << "Process had to be killed because his execution time exceeded " << maxwaitfor << " miliseconds.";
	}
	
	ss << "\n" << std::endl;	
}

void Scenario::printBrief(std::ostream &stream)
{
	stream << std::setw(16) << (this->metadata.name.empty() ? this->metadata.name : std::string("["+this->metadata.name+"]")) << ": ";
	
	if(this->result == Scenario::r_error)
	{
		stream << "NOT COMPLETED. An internal exception occured, error code = " << this->test->getExitCode();
	}
	else
	{
		switch(this->result)
		{
			case Scenario::r_ok: stream << "OK"; break;
			case Scenario::r_timeout: stream << "TIMEOUT"; break;
			case Scenario::r_fail: 
			default:
				stream << "FAILED"; break;
		}
		
	    stream << " (in " << this->test->getTime() << " ms)";
	}
}


std::ostream &operator<<(std::ostream &stream, Scenario &scenario)
{
	scenario.printBrief(stream);
	return stream;
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