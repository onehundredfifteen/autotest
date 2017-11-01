#include <fstream>
#include <iterator>

#include "rapidxml/rapidxml_print.hpp"
#include "template_gen.h"

bool GenerateXMLTemplate(std::string fpath)
{
	bool res = false;
	rapidxml::xml_document<> doc;
	rapidxml::xml_node<> *autotest, *root, *child, *child2, *child3, 
						 *decl;
	
	decl = doc.allocate_node(rapidxml::node_declaration);
		decl->append_attribute(doc.allocate_attribute("version", "1.0"));
		decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.append_node(decl);
 
	autotest = doc.allocate_node(rapidxml::node_element, "autotest");
		autotest->append_attribute(doc.allocate_attribute("xmlns", "http://onehundredfifteen.com"));
		autotest->append_attribute(doc.allocate_attribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance"));
		autotest->append_attribute(doc.allocate_attribute("xsi:schemaLocation", "http://onehundredfifteen.com http://onehundredfifteen.com/schemas/autotest.xsd"));
	doc.append_node(autotest);
	
	root = doc.allocate_node(rapidxml::node_element, "properties");
	autotest->append_node(root);
	
	child = doc.allocate_node(rapidxml::node_element, "version", "1.1");
	root->append_node(child);
	
	child = doc.allocate_node(rapidxml::node_element, "exe", "my_buggy_app.exe");
	root->append_node(child);
	
	child = doc.allocate_node(rapidxml::node_element, "waitfor", "5000");
	root->append_node(child);
	
	child = doc.allocate_node(rapidxml::node_element, "log", "log_buggy_app.txt");
		child->append_attribute(doc.allocate_attribute("append", "no"));
		child->append_attribute(doc.allocate_attribute("open_finished", "yes"));
		//child->append_attribute(doc.allocate_attribute("save", "failed"));
	root->append_node(child);
		
	//--------- tests
	root = doc.allocate_node(rapidxml::node_element, "test");
	autotest->append_node(root);
	
	child = doc.allocate_node(rapidxml::node_element, "case");
	   child->append_attribute(doc.allocate_attribute("name", "My 1st Test"));
	root->append_node(child);
	
	child2 = doc.allocate_node(rapidxml::node_element, "args", "buggy_app_param_to_run_with=1");
	child->append_node(child2);
	
	child2 = doc.allocate_node(rapidxml::node_element, "result");
	child->append_node(child2);
	
		child3 = doc.allocate_node(rapidxml::node_element, "exitcode", "0");
		child2->append_node(child3);
		
		child3 = doc.allocate_node(rapidxml::node_element, "output", "BUGGYAPP v. 1.0");
			child3->append_attribute(doc.allocate_attribute("match", "equal"));
			child3->append_attribute(doc.allocate_attribute("collapse_ws", "yes"));
		child2->append_node(child3);
		
		child3 = doc.allocate_node(rapidxml::node_element, "min_execution_time", "5");
		child2->append_node(child3);
		
		child3 = doc.allocate_node(rapidxml::node_element, "max_execution_time", "30");
		child2->append_node(child3);
	
	child2 = doc.allocate_node(rapidxml::node_element, "input", "test-input");
	child->append_node(child2);
	
	child2 = doc.allocate_node(rapidxml::node_element, "comment", "my first test");
	child->append_node(child2);
	
	//drugi case
	child = doc.allocate_node(rapidxml::node_element, "case");
	root->append_node(child);
	
	child2 = doc.allocate_node(rapidxml::node_element, "args", "test.com");
	child->append_node(child2);
	
	child2 = doc.allocate_node(rapidxml::node_element, "result");
	child->append_node(child2);
	
		child3 = doc.allocate_node(rapidxml::node_element, "exitcode", "1");
		child2->append_node(child3);
		
		child3 = doc.allocate_node(rapidxml::node_element, "output", "error=404");
			child3->append_attribute(doc.allocate_attribute("match", "like"));
		child2->append_node(child3);
	
	
	child2 = doc.allocate_node(rapidxml::node_element, "comment", "a raise-error test");
	child->append_node(child2);
	
	//3 case
	child = doc.allocate_node(rapidxml::node_element, "case");
		child->append_attribute(doc.allocate_attribute("name", "measure avg time"));
	root->append_node(child);
	
	child2 = doc.allocate_node(rapidxml::node_element, "input", "144/8\n");
	child->append_node(child2);
	
	child2 = doc.allocate_node(rapidxml::node_element, "result");
	child->append_node(child2);
	
		child3 = doc.allocate_node(rapidxml::node_element, "exitcode", "0");
		child2->append_node(child3);
		
		child3 = doc.allocate_node(rapidxml::node_element, "output", "144/8=18");
			child3->append_attribute(doc.allocate_attribute("match", "like"));
		child2->append_node(child3);
	
	child2 = doc.allocate_node(rapidxml::node_element, "repeat", "10");
	child->append_node(child2);
 
   // Convert doc to string if needed
	std::string xml_as_string;
	rapidxml::print(std::back_inserter(xml_as_string), doc);
	 
	// Save to file
	std::ofstream file_stored( fpath.c_str() );
	
	res = file_stored.is_open();
	
	file_stored << doc;
	file_stored.close();
	doc.clear();
	
	return res;
}