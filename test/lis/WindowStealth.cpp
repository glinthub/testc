// WindowStealth.cpp : Defines the entry point for the application.
//

#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <tlhelp32.h>

FILE * f;
time_t t;

BOOL CALLBACK EnumWindowsProc(HWND hwnd,LPARAM hDlg);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	//DialogBox(hInstance, (LPCTSTR)IDD_DIALOG1, NULL, (DLGPROC)WindowStealth);
	//HotKeyMon();
	int ExitApp = 0; // used to signal the monitor key thread to exit
	char line[256] = {0};
	f = fopen("user.txt", "a");
	t = time(NULL);

	strcat(line, ctime(&t));
	line[strlen(line) - 1] = '\0';
	strcat(line, ", Instance initiated. ####################\n");
	fputs(line, f);

	/**/
	while(ExitApp != 1) {
        if (GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(VK_SHIFT) && GetAsyncKeyState(VK_F12)) // if PAUSE is down or was down since the last call
			ExitApp = 1;
		else {
			#if 0
			// PROCESSENTRY32结构对象
			PROCESSENTRY32 pe;
			// 创建快照句柄
			HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			// 先搜索系统中第一个进程的信息
			Process32First(hSnapshot, &pe);
			// 下面对系统中的所有进程进行枚举，并保存其信息
			DWORD pid = 0;
			do{
				/**/
				if(strstr(pe.szExeFile, "NOTEPAD")) {
					pid = pe.th32ProcessID;
				}
				/**/
				fputs(pe.szExeFile, f);
				fputs("\n", f);
			} while (Process32Next(hSnapshot, &pe));
			// 关闭快照句柄
			CloseHandle(hSnapshot); 

			/**/
			if(pid) {
				HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
				TerminateProcess(hProcess,0); 
			}
			/**/
			#endif

			EnumWindows(EnumWindowsProc, NULL);

			Sleep(3000);
			fflush(f);
		}
	}
	/**/

	line[0] = '\0';
	t = time(NULL);
	strcat(line, ctime(&t));
	line[strlen(line) - 1] = '\0';
	strcat(line, ", Instance exited. ####################\n");
	fputs(line, f);
	fclose(f); 

	return 0;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd,LPARAM hDlg)
{

	char szbuff[256];
	char line[256] = {0};
	DWORD pid;
		
	GetWindowText(hwnd, szbuff, sizeof(szbuff));

	// some windows do not have a title, so we ignore the windows that doesn't have one	
	if(strlen(szbuff) <= 0)
		return TRUE;

	if(strstr(szbuff, "README")) {
		HANDLE hProcess;
		GetWindowThreadProcessId(hwnd, &pid);
		hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
		TerminateProcess(hProcess,0);

		t = time(NULL);
		strcat(line, ctime(&t));
		line[strlen(line) - 1] = '\0';
		strcat(line, ", target captured. [");
		strcat(line, szbuff);
		strcat(line, "]\n");
		fputs(line, f);
	}
	
	/*
	if	(IsWindowVisible(hwnd) > 0 )
		SendMessage(hListBox,LB_ADDSTRING,0,(LPARAM)szbuff);	
	*/
	
	return TRUE;
}

