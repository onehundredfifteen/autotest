#ifndef TEST_PROCESS_H
 #define TEST_PROCESS_H
 
#include <windows.h>
#include <string>

typedef unsigned long int time_int;

class TestProcess
{
	public:
	    TestProcess(std::string cmd, std::string input, int max_waitfor = 0 );
		//TestProcess() : TestProcess("cmd", "echo no argument passed") {};
		
		short getError() const;
		
		std::string getOutputString() const;
		time_int getTime() const;
		int getExitCode() const;
		bool isTimeout() const;
		
	private:
		HANDLE hChildStd_IN_Rd;
		HANDLE hChildStd_IN_Wr;
		HANDLE hChildStd_OUT_Rd;
		HANDLE hChildStd_OUT_Wr;
		
		HANDLE Guard;
		
		PROCESS_INFORMATION processInformation;
        STARTUPINFO startupInfo;
        SECURITY_ATTRIBUTES securityAttributes;		
		

		short internal_error; 
		
		std::string result_output;
		int result_exit_code;
		time_int result_time;
		
		bool timeout;
		int waitfor; //max miliseconds to wait, else kill
		
		std::string test_input;
		std::string cmd_arg;
		
		
		void RunTest();
		
		void InitPipes();
		void ReadOutputFromPipe();
		void WriteInputToPipe();
		
		void AddEnterToInput();
		
		static DWORD WINAPI GuardThreadStart( LPVOID lpParam );
		DWORD GuardThreadMonitor();
		void RunGuardThread();
};
 
#endif