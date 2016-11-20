#include <fstream>
#include <iostream>
#include <iomanip> 
#include <limits>

#include "scenario.h"
#include "test_xml_reader.h"
#include "template_gen.h"


int  Go(const char *f);
void OpenLog(std::ofstream &log, const char * log_path, const char * test_path, bool append);
void CloseLog(std::ofstream &log);


int main(int argc, char* argv [])
{
	std::cout << "Autotest v. 1.1 (" << __DATE__ << ")\nTesting Tool for console applications.\n"
			  << "(c) by onehundredfifteen 2015\n" << std::endl;
	
	if(argc > 1)
	{
		if(strcmp(argv[1], "help") == 0)
		{
			std::cout << "\nUsage:\nautotest [\"filepath.xml\"] - run test cases defined in file \"filepath.xml\".\n" 
					  << "autotest - and type filepath directly to input.\n"
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
			
		while(true)
		{
			std::cout << "Enter path to xml file with test cases: ";
			std::getline (std::cin, pfile);
				
			Go(pfile.c_str());
				
			std::cout << "Do you want to run other set of tests? [y/n]: ";
			std::cin >> c;
				
			if(c != 'y' && c != 'Y')
				break;
					
			std::cin.ignore( std::numeric_limits<int>::max() , '\n'); 
			std::cout << std::endl;
		}
	}
	
	return 0;		
}

int Go(const char *f)
{
	Scenario *scenario;
	std::ofstream log;
	unsigned cnt = 0, completed = 0, ok = 0;
	short test_result;
	
	//read file
	TestReader reader(f);
	if(!reader.isValid())
		return 1;
	
	//open log
	if(reader.getLogFile())
	{
		OpenLog(log, reader.getLogFile(), f, reader.opLogAppend());
		if(log.fail())
			return 2;
	}
	
    //run scenarios
	while(reader.hasNextScenario())
	{
		std::cout <<"> Running Test #"<< ++cnt << "...\t";
		
		scenario = reader.getNextScenario();
		scenario->PerformTest();
		
		test_result = scenario->getResult();
		if(test_result >= 0)
		{
			if(test_result == 10)
			{
				std::cout << "OK (in " << scenario->getDuration() << " ms)";	
				if(reader.opLogSaveMode() == TestReader::slmAll)
					scenario->SaveToLog(log, cnt);
				
				++ok;
			}
			else if(test_result == 1)
			{
				std::cout << "TIMEOUT (in " << scenario->getDuration() << " ms)";
				scenario->SaveToLog(log, cnt);
			}
			else
			{
				std::cout << "FAIL (in " << scenario->getDuration() << " ms)";
				scenario->SaveToLog(log, cnt);
			}
			++completed;
		}
		else
		{
			std::cout << "NOT COMPLETED";
			if(reader.opLogSaveMode() == TestReader::slmNotpassed)
					scenario->SaveToLog(log, cnt);
		}
		std::cout << std::endl;
		
		delete scenario;
	}
	
	//podsumowanie
	if(completed == cnt)
		std::cout << "*****\nAll " << cnt << " tests completed. ";
	else 
	    std::cout << "*****\n" << completed << "/" << cnt << " tests completed. ";
	
	if(completed > ok)
		std::cout << (completed - ok) << " failed (" << std::setprecision(3) << ((float)(completed - ok))* 100.0 / (float)completed << std::fixed << "%)" << std::endl;
	else if(completed)	//> 0 && completed == ok
		std::cout << "100% tests passed." << std::endl;
	
	
	if(log.is_open() && 
	  (completed > ok 
	  || (reader.opLogSaveMode() == TestReader::slmNotpassed && completed < cnt) 
	  ||  reader.opLogSaveMode() == TestReader::slmAll
	  ) )
	{
		if(reader.opLogOpenFin())
		{
			ShellExecute(0, 0, reader.getLogFile(), 0, 0, SW_SHOW);
		}
		else 
			std::cout << "Additional informations are available in log file \"" << reader.getLogFile() <<"\"" << std::endl;
	}
	
	CloseLog(log);
	
	return 0;
}


void OpenLog(std::ofstream &log, const char * log_path, const char * test_path, bool append)
{
	#define DTTMFMT "%Y-%m-%d %H:%M:%S "
	#define DTTMSZ 21
    
	char buff[DTTMSZ];
    time_t t = time(0);
	
    strftime (buff, DTTMSZ, DTTMFMT, localtime (&t));
	
    log.open(log_path, append ? std::ofstream::app : std::ofstream::out); //sztuczka - out jest zawsze załozona
	if(log.fail())
	{
		std::cout << "!> Cannot open log file \"" << log_path << "\"!" << std::endl;
		return;
	}

    log << "AUTOTEST LOG " << buff << "\nTEST CASE FILE: " << test_path << "\n***********" << std::endl;
}

void CloseLog(std::ofstream &log)
{
	if(log.is_open())
	{
		log << "*** END ***" << std::endl;
		log.close();
	}
}
