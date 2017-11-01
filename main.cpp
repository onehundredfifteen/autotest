#include <sstream>
#include <fstream>
#include <iomanip> 
#include <limits>

#include "tee_stream.h"
#include "scenario.h"
#include "test_xml_reader.h"
#include "template_gen.h"


std::ofstream log_file;

int  Go(const char *scenario_file);
bool OpenLog(const char * log_path, const char * test_path);
void CloseLog();


int main(int argc, char* argv [])
{
	std::cout << "Autotest v. 1.1 (" << __DATE__ << ")\nTesting tool for console applications.\n"
			  << "(c) by onehundredfifteen 2015-2017\n" << std::endl;
	
	if(argc > 1)
	{
		if(strcmp(argv[1], "help") == 0)
		{
			std::cout << "\nUsage:\nautotest [\"filepath.xml\"] - run test cases defined in file \"filepath.xml\".\n" 
					  << "autotest template_gen - generate a test cases file template.\n\n"
					  << "All settings are defined in test cases *.xml files.";
		}
		else if(strcmp(argv[1], "template_gen") == 0)
		{
			if(GenerateXMLTemplate("template.xml"))
			{
				std::cout << "Saved template to \"template.xml\".";
			}
			else std::cout << "!> Cannot open file \"template.xml\" to write.";
		}
		else
		{
			return Go(argv[1]);
		}
	}
	else
	{
		std::string pfile;
		char c;
		
		std::cout << "type 'template' as path to generate a template instead.\n" << std::endl;
			
		while(true)
		{
			std::cout << "Specify the path to the xml file with test cases: ";
			
			std::getline (std::cin, pfile);
			
			if(pfile == "template")
			{
				if(GenerateXMLTemplate("template.xml"))
				{
					std::cout << "Saved template to \"template.xml\".";
				}
				else std::cout << "!> Cannot open file \"template.xml\" to write.";
			}
			else
			{
				if( Go(pfile.c_str()) ){
					std::cout << "!> Cannot run file \"" << pfile << "\".";
				}
			}
				
			std::cout << "\nDo you want to run another set of tests? [y/n]: ";
			std::cin >> c;
				
			if(c != 'y' && c != 'Y')
				break;
					
			std::cin.ignore( std::numeric_limits<int>::max() , '\n'); 
			std::cout << std::endl;
		}
	}
	
	return 0;		
}

int Go(const char *scenario_file)
{
	Scenario *scenario;
	
	std::stringstream log_stream;
	char * log_path;
	unsigned cnt = 0, completed = 0, ok = 0;
	
	Scenario::run_result test_result;
	
	//read file
	TestReader reader(scenario_file);
	if(!reader.isValid())
		return 1;
	
	//open log
	if(reader.getLogFile())
	{
		if(OpenLog(reader.getLogFile(), scenario_file) == false)
			return 2;
	}
	
	teestream tee_stream(std::cout, log_file);
	
    //run scenarios
	while(reader.hasNextScenario())
	{
		tee_stream << "Running test " << std::setw(2) << ++cnt;
		
		scenario = reader.getNextScenario();
		scenario->PerformTest();
		test_result = scenario->getTestResult();
		
		scenario->metadata.test_number = cnt;
	
		if(test_result != Scenario::r_error)
		{
			if(test_result == Scenario::r_ok)
				++ok;
			else{
				scenario->printSummary(log_stream);
			}
			
			++completed;
		}
	
		tee_stream << *scenario << std::endl;
		
		delete scenario;
	}
	
	//podsumowanie
	if(completed == cnt)
		tee_stream << std::string(15, '*') << "\nAll " << cnt << " tests completed. ";
	else 
	    tee_stream << std::string(15, '*') << "\n" << completed << "/" << cnt << " tests completed. ";
	
	if(completed > ok)
		tee_stream << (completed - ok) << " failed (" << std::setprecision(3) << ((float)(completed - ok))* 100.0 / (float)completed << std::fixed << "%)" << std::endl;
	else if(completed)	//> 0 && completed == ok
		tee_stream << "100% tests passed." << std::endl;
	
	
	log_file << log_stream.str() << std::flush;
	CloseLog();
	
	if(reader.opLogOpenFin())
	{
		ShellExecute(0, 0, reader.getLogFile(), 0, 0, SW_SHOW);
	}
	
	return 0;
}


bool OpenLog(const char * log_path, const char * test_path)
{
	#define DTTMFMT "%Y-%m-%d %H:%M:%S "
	#define DTTMSZ 21
    
	char buff[DTTMSZ];
    time_t t = time(0);
	
    strftime (buff, DTTMSZ, DTTMFMT, localtime (&t));
	
    log_file.open(log_path, std::ofstream::app); 
	if(log_file.fail())
	{
		std::cerr << "!> Cannot open log file \"" << log_path << "\"!" << std::endl;
		return false;
	}

	log_file << std::string(15, '=') << " BEGIN " << std::string(15, '=') << std::endl;
    log_file << "AUTOTEST LOG " << buff << "\nTEST CASE FILE: " << test_path << std::endl;
	log_file << std::string(37, '=') << std::endl;
	
	return true;
}

void CloseLog()
{
	if(log_file.is_open())
	{
		log_file << "end" << std::endl;
		log_file.close();
	}
}
