#include <limits>
#include <iostream>
#include <chrono>
#include "test_process.h"

#define BUFSIZE 2048


TestProcess::TestProcess(std::string cmd, std::string input, int max_waitfor)
{
	internal_error = 0;
	test_input = input;
	cmd_arg = cmd;
	timeout = false;
	result_time = 0;
	
	if(max_waitfor == 0)
		waitfor = std::numeric_limits<int>::max();
	else
		waitfor = max_waitfor;
	
	InitPipes();
	
	if(internal_error)
		return;

	auto cl_start = std::chrono::system_clock::now();
	
	RunTest(); 
	result_time = (std::chrono::duration_cast< std::chrono::milliseconds > (std::chrono::system_clock::now() - cl_start)).count();
}

void TestProcess::InitPipes()
{
  	hChildStd_IN_Rd = NULL;
	hChildStd_IN_Wr = NULL;
	hChildStd_OUT_Rd = NULL;
	hChildStd_OUT_Wr = NULL;
	
	ZeroMemory( &securityAttributes, sizeof(SECURITY_ATTRIBUTES) );
	securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES); 
    securityAttributes.bInheritHandle = TRUE; 
    securityAttributes.lpSecurityDescriptor = NULL;
	
	if ( !CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &securityAttributes, 0) && internal_error == 0 ) 
      internal_error = -1;

    if ( ! SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) && internal_error == 0 )
      internal_error = -2;

    if (! CreatePipe(&hChildStd_IN_Rd, &hChildStd_IN_Wr, &securityAttributes, 0) && internal_error == 0) 
      internal_error = -3;

    if ( ! SetHandleInformation(hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0) && internal_error == 0)
      internal_error = -4;	
}

void TestProcess::RunTest()
{
    DWORD ecode, tecode;
	
	ZeroMemory( &processInformation, sizeof(PROCESS_INFORMATION) );
    ZeroMemory( &startupInfo, sizeof(STARTUPINFO) );
	
	startupInfo.cb = sizeof(startupInfo);
	startupInfo.hStdError = hChildStd_OUT_Wr;
    startupInfo.hStdOutput = hChildStd_OUT_Wr;
    startupInfo.hStdInput = hChildStd_IN_Rd;
    startupInfo.dwFlags    = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    startupInfo.wShowWindow = SW_HIDE; // execute hide 
  
    // Create the process
    BOOL result = CreateProcess(NULL, const_cast<char *>(cmd_arg.c_str()), 
                                NULL, NULL, TRUE, 
                                NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, 
                                NULL, NULL, &startupInfo, &processInformation);

   if (!result)
   { 
      internal_error = GetLastError();
   }
   else
   {
      if(! CloseHandle( hChildStd_OUT_Wr) || ! CloseHandle( hChildStd_IN_Rd) )
		internal_error = -5;
	
	  RunGuardThread();	
	  WriteInputToPipe();	
	  ReadOutputFromPipe();

	  // Successfully created the process.  Wait for it to finish.
      //WaitForSingleObject( processInformation.hProcess, INFINITE );
	  
      //czekamy na guarda
	  if(WaitForSingleObject( Guard, INFINITE ) == WAIT_OBJECT_0)
	  {
         if(GetExitCodeThread( Guard, &tecode))
         {
			  timeout = tecode;
         }
	  }
	  
	  if(!timeout)
	  {
		  result = GetExitCodeProcess(processInformation.hProcess, &ecode);
	      if(result)
			  result_exit_code = static_cast<int>(ecode);
	  }
 
      // Close the handles.
	  CloseHandle( Guard );
      CloseHandle( processInformation.hProcess );
      CloseHandle( processInformation.hThread );
	  CloseHandle( hChildStd_OUT_Rd);
 
      if (!result)
      {
         // Could not get exit code.
         internal_error = -6;
      }
   }
}

void TestProcess::ReadOutputFromPipe() 
{ 
   DWORD dwRead; 
   CHAR chBuf[BUFSIZE]; 
   BOOL bSuccess = FALSE;

   while(true) 
   { 
	  bSuccess = ReadFile( hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
     
	  if( ! bSuccess || dwRead == 0) 
		break; 
	
	  result_output += std::string(chBuf, dwRead);
   } 
} 

void TestProcess::WriteInputToPipe() 
{ 
   DWORD dwLen, dwWritten; 
   CHAR chBuf[BUFSIZE];
   BOOL bSuccess = FALSE;
   
   if(test_input.empty() == false)
   {
	   AddEnterToInput();
	   
	   strcpy(chBuf, test_input.c_str());
	   dwLen = strlen(chBuf);
	 
	   while(true) 
	   {      
		  bSuccess = WriteFile(hChildStd_IN_Wr, chBuf, dwLen, &dwWritten, NULL);
		  if ( ! bSuccess || dwWritten == 0)
			break; 
	   } 
	 
	   // Close the pipe handle so the child process stops reading. 
	 
	   if ( ! CloseHandle(hChildStd_IN_Wr) ) 
		  internal_error = -8;
   }
} 


void TestProcess::AddEnterToInput()
{
	if(test_input.back() != '\n')
		test_input.push_back('\n');
}

//thread guard func
void TestProcess::RunGuardThread()
{
       DWORD ThreadID; 
	   Guard = CreateThread(NULL, 0, GuardThreadStart, (void*) this, 0, &ThreadID);
}

DWORD WINAPI TestProcess::GuardThreadStart( LPVOID lpParam ) 
{
	 TestProcess* This = (TestProcess*) lpParam;
     return This->GuardThreadMonitor();
} 

DWORD TestProcess::GuardThreadMonitor(void)
{
	if( WaitForSingleObject( processInformation.hProcess, waitfor ) == WAIT_TIMEOUT) 
	{
			//if the process is executing too long, kill it
			TerminateProcess(processInformation.hProcess, 0);
			
			#if (_WIN32_WINNT >= 0x0600)
				//and cancel pending io in pipes
				CancelIoEx(hChildStd_OUT_Rd, NULL);
				CancelIoEx(hChildStd_IN_Wr, NULL);
			
			#endif
			
			return 1;
    }
	
    return 0; 
}

//getters

short TestProcess::getError() const
{
	return internal_error;
}

std::string TestProcess::getOutputString() const
{
	return result_output;
}
time_int TestProcess::getTime() const
{
	return result_time;
}
int TestProcess::getExitCode() const
{
	return result_exit_code;
}
bool TestProcess::isTimeout() const
{
	return timeout;
}
 

