#include <iostream>
#include <cstring>
#include "test_xml_reader.h"

TestReader::TestReader(std::string xml_path)
{
	char * xml_content;
	
	rapidxml::xml_document <> xml_doc;
	
	
	op_log_append = false;
	op_log_open = false;
	xmlValid = false;
	waitfor = 0;
	
	//otwarcie pliku
	std::ifstream in(xml_path.c_str());
	if(!in)
	{
		//p.what();
		std::cout << "!> Cannot open file \"" << xml_path << "\"." << std::endl; 
		return;
	}
	
	std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
	
	xml_content = const_cast<char *>(contents.c_str());
	
	//parsowanie
	try
	{
		xml_doc.parse<0>( xml_content );
	}
	catch( rapidxml::parse_error p )
	{
		std::cout << "!> XML Parse Error: " << p.what() << std::endl;
		return;
	}
	
	//odczyt opcji
	read_node = xml_doc.first_node( XML_NODENAME_ROOT )->first_node( XML_NODENAME_PROPERTIES ); 

	if(ReadSettings())
	{
		this->read_node = read_node->next_sibling()->first_node( XML_NODENAME_TEST_CASE ); //tests->case
	
		if(this->read_node == NULL)
		{
			std::cout << "!> File \"" << xml_path << "\" is corrupted." << std::endl;
		}
		else if(program_path.empty())
		{
			std::cout << "!> Not all required data are specified." << std::endl;
		}
		else 
		{
			ReadScenarios();
			if(!hasNextScenario())
			{
				std::cout << "!> No test scenarios found." << std::endl;
			}
			else
				xmlValid = true;
		}
	
	}
	xml_doc.clear();
}


bool TestReader::ReadSettings() 
{
	rapidxml::xml_attribute <> *node_attr;
	rapidxml::xml_node <> *properties;
	//opcje - ustawienia
	for(properties = read_node->first_node(); properties; properties = properties->next_sibling() ) 
	{
		if(strcmp(properties->name(),  XML_OP_VERSION) == 0
		&& strcmp(properties->value(), XML_VERSION) != 0)
		{
			std::cout << "!> Version " << properties->value() << " is not supported." << std::endl;
			return false;
		}
		else if(strcmp(properties->name(), XML_OP_EXE) == 0)
		{
			program_path = properties->value();
		}
		else if(strcmp(properties->name(), XML_OP_WAITFOR) == 0)
		{
			waitfor =  atoi( properties->value() ) ;
		}
		else if(strcmp(properties->name(), XML_OP_LOG) == 0)
		{
			log_path = properties->value();
				
			//artybuty
			for(node_attr = properties->first_attribute(); node_attr; node_attr = node_attr->next_attribute() )
			{
				if(strcmp(node_attr->name(), XML_OP_LOGAPPEND) == 0)
				{
					op_log_append =(strcmp(node_attr->value(), XML_OP_YES) == 0);
				}
				else if(strcmp(node_attr->name(), XML_OP_OPENLOG) == 0)
				{
					op_log_open = (strcmp(node_attr->value(), XML_OP_YES) == 0);
				}
				else if(strcmp(node_attr->name(), XML_OP_SAVETOLOG) == 0)
				{
					if( strcmp(node_attr->value(), "all") == 0 )
						op_save_mode = slmAll;
					else if( strcmp(node_attr->value(), "notpassed") == 0 )
						op_save_mode = slmNotpassed;
					else 
					    op_save_mode = slmFailed;
				}
			}
		}
	}
	
	return true;
}

void TestReader::ReadScenarios()
{
	rapidxml::xml_node <> *node_result, *node_scen;
	rapidxml::xml_attribute <> *output_attr;
	
	while(this->read_node)
	{
		NodeData node_data;
		for(node_scen = read_node->first_node(); node_scen; node_scen = node_scen->next_sibling() ) 
		{
			if(strcmp(node_scen->name(), XML_NODENAME_ARGS) == 0 
			|| strcmp(node_scen->name(), XML_NODENAME_INPUT) == 0 )
			{
				node_data[ node_scen->name() ] = node_scen->value();
			}
			else if(strcmp(node_scen->name(), XML_NODENAME_RESULT) == 0)
			{
				//opcje tego węzła
				for(node_result = node_scen->first_node(); node_result; node_result = node_result->next_sibling() )
				{
					if(strcmp(node_result->name(), XML_NODENAME_EXITCODE) == 0 
					|| strcmp(node_result->name(), XML_NODENAME_MIN_EXTIME) == 0
					|| strcmp(node_result->name(), XML_NODENAME_MAX_EXTIME) == 0)
					{
						node_data[ node_result->name() ] = node_result->value();
					}
					else if(strcmp(node_result->name(), XML_NODENAME_OUTPUT) == 0)
					{
						node_data[ XML_NODENAME_OUTPUT ] = node_result->value();
						
						//artybuty
						for(output_attr = node_result->first_attribute(); output_attr; output_attr = output_attr->next_attribute() )
						{
							if(strcmp(output_attr->name(), XML_NODENAME_MATCH) == 0 
							|| strcmp(output_attr->name(), XML_NODENAME_MERGE_WS) == 0)
							{
								node_data[ output_attr->name() ] = output_attr->value();
							}
						}
					//end output					
					}				
				}
				//end - result
			}	
		}
	//all test cases
	    nodes.push_back(node_data); 
	    this->read_node = this->read_node->next_sibling();
	}
}

Scenario * TestReader::getNextScenario()
{
    Scenario::output_match match;
	NodeData data = nodes.front();
	nodes.pop_front();

	if(data[ XML_NODENAME_MATCH ] == "like" || data[ XML_NODENAME_MATCH ].empty())
		match = Scenario::om_like;
	else if(data[ XML_NODENAME_MATCH ] == "notlike")
		match = Scenario::om_notlike;
	else if(data[ XML_NODENAME_MATCH ] == "equal")
		match = Scenario::om_equal;
	else if(data[ XML_NODENAME_MATCH ] == "notequal")
		match = Scenario::om_notequal;
	else 
		match = Scenario::om_none;
		
#ifdef AT_DEBUG_MODE
	std::cout <<" \nar="<< data[ XML_NODENAME_ARGS ] << ", i=" << data[ XML_NODENAME_INPUT ] << ", o=" << data[ XML_NODENAME_OUTPUT ] << ", oo=" << data[ XML_NODENAME_MATCH ] << ", ec=" 
	          << data[ XML_NODENAME_EXITCODE ] << "("<<atoi( data[ XML_NODENAME_EXITCODE ].c_str())<<"), em=" << atoi( data[ XML_NODENAME_MIN_EXTIME ].c_str()) << ", ex=" << atoi( data[ XML_NODENAME_MAX_EXTIME ].c_str()) 
			  <<" ws=" << (data[ XML_NODENAME_MERGE_WS ] == "yes") <<std::endl;
#endif			  
			  
	//atoi dla pustego stringa zwróci 0, to będa domyslne wartości, np exit_code = success
	return new Scenario( (this->program_path+" "+data[ XML_NODENAME_ARGS ]), 
						 data[ XML_NODENAME_OUTPUT ],
						 data[ XML_NODENAME_INPUT ], 
						 atoi( data[ XML_NODENAME_EXITCODE ].c_str()), 
						 match, 
						 atoi( data[ XML_NODENAME_MIN_EXTIME ].c_str()), 
						 atoi( data[ XML_NODENAME_MAX_EXTIME ].c_str()), 
						 this->waitfor, 
						 (data[ XML_NODENAME_MERGE_WS ]) == XML_OP_YES);
}

bool TestReader::hasNextScenario() const
{
	return (nodes.empty() == false);
}

bool TestReader::isValid() const
{
	 return xmlValid;
}

const char * TestReader::getLogFile() const
{
	if(log_path.empty())
		return NULL;
		
	return log_path.c_str();
}
bool TestReader::opLogAppend() const
{
	return op_log_append;
}
bool TestReader::opLogOpenFin() const
{
	return op_log_open;
}
TestReader::savelog_mode TestReader::opLogSaveMode() const
{
	return op_save_mode;
}