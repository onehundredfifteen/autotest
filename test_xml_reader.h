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
 #define XML_NODENAME_REPEAT    "repeat"
 
 #define XML_NODENAME_ROOT  	 "autotest"
 #define XML_NODENAME_PROPERTIES "properties"
 #define XML_NODENAME_TESTS  	 "tests"
 #define XML_NODENAME_TEST_CASE  "case"
 
 #define XML_NODENAME_DESCRIPTION  "comment"
#endif

#ifndef XML_OPS
 #define XML_OP_VERSION 	"version"
 #define XML_OP_EXE 		"exe" 
 #define XML_OP_WAITFOR     "waitfor"
 #define XML_OP_LOG 		"log"
 #define XML_OP_LOGAPPEND 	"append"
 #define XML_OP_OPENLOG 	"open_finished"
 #define XML_OP_TESTCASE_NAME 	"name"
 
 #define XML_OP_YES  "yes"
 #define XML_OP_NO  "no"
#endif

#ifndef XML_VERSION
 #define XML_VERSION_1_0_OBSOLETE "1.0"
 #define XML_VERSION "1.1"
#endif


typedef std::map<std::string, std::string> NodeData;

class TestReader
{
	public:
		TestReader(std::string xml_path);
		
	    Scenario * getNextScenario();
		bool hasNextScenario() const;
		bool isValid() const;
		
		const char * getLogFile() const;
		bool opLogAppend() const;
		bool opLogOpenFin() const;
		
	private:
		std::list< NodeData > nodes;
		
		std::string program_path; // sciezka do testowanego programu
		std::string log_path; //ściezka do logu
		bool op_log_append;  //opcja - otiweraj plik logu w append mode
		bool op_log_open; //opcja - otwórz log po zakończeniu
		int waitfor; // czas czekania na watek
		
		rapidxml::xml_node <> *read_node;
		
		bool xmlValid;
		
	private:
		Scenario * getScenario(NodeData &data);
		bool ReadSettings();
		void ReadScenarios();
		void NodeDefault(NodeData &data);
};

#endif