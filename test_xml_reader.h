#ifndef TEST_XML_READER_H
 #define TEST_XML_READER_H
 
#include <map>
#include <list> 
#include "rapidxml/rapidxml.hpp"
#include "scenario.h"

#ifndef XML_NODENAMES
 #define XML_NODENAME_ARGS 		"args"
 #define XML_NODENAME_INPUT 	"input" 
 #define XML_NODENAME_RESULT 	"result"
 #define XML_NODENAME_EXITCODE 	"exitcode"
 #define XML_NODENAME_OUTPUT 	"output"
 #define XML_NODENAME_MATCH 	"match"
 #define XML_NODENAME_MERGE_WS 	"collapse_ws"
 #define XML_NODENAME_MIN_EXTIME "min_execution_time"
 #define XML_NODENAME_MAX_EXTIME "max_execution_time"
 
 
 #define XML_NODENAME_ROOT  	 "autotest"
 #define XML_NODENAME_PROPERTIES "properties"
 #define XML_NODENAME_TESTS  	 "tests"
 #define XML_NODENAME_TEST_CASE  "case"
#endif

#ifndef XML_OPS
 #define XML_OP_VERSION 	"version"
 #define XML_OP_EXE 		"exe" 
 #define XML_OP_WAITFOR     "waitfor"
 #define XML_OP_LOG 		"log"
 #define XML_OP_LOGAPPEND 	"append"
 #define XML_OP_OPENLOG 	"open_finished"
 #define XML_OP_SAVETOLOG 	"save"
 
 #define XML_OP_YES 		"yes"
#endif

#ifndef XML_VERSION
 #define XML_VERSION "1.0"
#endif


typedef std::map<std::string, std::string> NodeData;

class TestReader
{
	public:
		typedef enum{
		  slmAll,
		  slmFailed,
		  slmNotpassed
		} savelog_mode;
	
		TestReader(std::string xml_path);
		
	    Scenario * getNextScenario();
		bool hasNextScenario() const;
		bool isValid() const;
		
		const char * getLogFile() const;
		bool opLogAppend() const;
		bool opLogOpenFin() const;
		savelog_mode opLogSaveMode() const;
		
	private:
		std::list< NodeData > nodes;
		bool ReadSettings();
		void ReadScenarios();
		
		std::string program_path; // sciezka do testowanego programu
		std::string log_path; //ściezka do logu
		bool op_log_append;  //opcja - otiweraj plik logu w append mode
		bool op_log_open; //opcja - otwórz log po zakończeniu
		int waitfor; // czas czekania na watek
		savelog_mode op_save_mode; //co trafia do logów
		
		rapidxml::xml_node <> *read_node;
		
		bool xmlValid;
};

#endif