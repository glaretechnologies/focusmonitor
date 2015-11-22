/*=====================================================================
Focus Monitor
-------------
Copyright Glare Technologies Limited 2015 - 
Code by Nicholas Chapman, nick@indigorenderer.com

Is designed to be used to catch annoying programs that steal Window focus.

Polls every X ms to work out what Window has focus.  Prints out the path to the executable if a different programs gets focus.
=====================================================================*/
#include "Windows.h"
#include <psapi.h> // For access to GetModuleFileNameEx
#include <iostream>
#include <string>


#ifdef _UNICODE
  #define tcout wcout
  #define tcerr wcerr
#else
  #define tcout cout
  #define tcerr cerr
#endif


int main()
{
	std::wstring state;

	while(1)
	{
		HWND focus_handle = GetForegroundWindow();

		if(focus_handle)
		{
			DWORD process_id;
			GetWindowThreadProcessId(focus_handle, &process_id);

			// http://stackoverflow.com/questions/1933113/c-windows-how-to-get-process-path-from-its-pid
			HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, process_id);
			if(processHandle != NULL)
			{
				TCHAR filename[MAX_PATH];
				if(GetModuleFileNameEx(processHandle, NULL, filename, MAX_PATH) == 0)
				{
					if(state != L"Failed to get module filename")
					{
						std::tcout << "Failed to get module filename." << std::endl;
						state = L"Failed to get module filename"; 
					}
				}
				else
				{
					const std::wstring cur_path(filename);

					if(cur_path != state)
					{
						std::tcout << "============== New program has foreground window ============\n" << filename << std::endl;
						state = cur_path;
					}
				}
				CloseHandle(processHandle);
			}
			else
			{
				if(state != L"Failed to open process")
				{
					std::tcout << "Failed to open process." << std::endl;
					state = L"Failed to open process"; 
				}
			}
		}
		else
		{
			if(state != L"No foreground window")
			{
				std::tcout << "No foreground window." << std::endl;
				state = L"No foreground window"; 
			}
		}

		Sleep(50); // Sleep for 50 ms.
	}
}
