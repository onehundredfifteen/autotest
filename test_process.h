#ifndef TEST_PROCESS_H
 #define TEST_PROCESS_H
 
#include <windows.h>
#include <string>

class TestProcess
{
	public:
	    TestProcess(std::string cmd, std::string input);
		TestProcess() : TestProcess("cmd", "echo no argument passed") {};
		
		unsigned getError() const;
		void GetRunResult(std::string &, int&);
		
	private:
		HANDLE hChildStd_IN_Rd;
		HANDLE hChildStd_IN_Wr;
		HANDLE hChildStd_OUT_Rd;
		HANDLE hChildStd_OUT_Wr;
		
		PROCESS_INFORMATION processInformation;
        STARTUPINFO startupInfo;
        SECURITY_ATTRIBUTES securityAttributes;		

		unsigned _error; // liczba >0 oznacza jkiś błąd
		
		std::string result_output;
		int result_exit_code;
		
		std::string test_input;
		std::string cmd_arg;
		
		void InitPipes();
		void RunTest();
		void ReadOutputFromPipe();
		void WriteInputToPipe();
		
		void AddEnterToInput();
};
 
#endif