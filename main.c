#include <windows.h>
#include <stdio.h>
#include <commctrl.h>
#include <pthread.h>
#include "header.h"



static HWND MainWindow, BT_Play, STATIC_ProgBarPos, STATIC_MoneyStatus, STATIC_forTest, BT_Info, movieProgress;

int devStatus = 1;
int BT_PlayStatus = 0;
int money = 1000;
char fileLine[1000];
char userDataId[1000];
char userDataName[1000];
char userDataMoney[1000];

void UpdateLabel(void);
void WatchingMovie(void);
void SwitchStatus(void);
int callback(void*, int, char**, char**);

pthread_t thread1, thread2;

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) 
{
	switch(Message) 
	{
		case WM_CREATE:
		{
			break;
		}
    	case WM_COMMAND:
    	{
			if(wParam == id_BT_Play)
			{
				SwitchStatus();
				pthread_create(&thread1, NULL, WatchingMovie, NULL);
				break;
			}
			if(wParam == id_BT_Info)
			{
				MessageBox(NULL, userDataName,"info",MB_ICONEXCLAMATION|MB_OK);
				break;
			}
		}
		case WM_HSCROLL:
       		UpdateLabel();
       		break;
		case WM_DESTROY: 
		{
			PostQuitMessage(0);
			break;
		}
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

/* The 'main' function of Win32 GUI programs: this is where execution starts */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
	
	WNDCLASSEX wc; /* A properties struct of our window */
	HWND hwnd; /* A 'HANDLE', hence the H, or a pointer to our window */
	MSG msg; /* A temporary location for all messages */


	/* zero out the struct and set the stuff we want to modify */
	memset(&wc,0,sizeof(wc));
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.lpfnWndProc	 = WndProc; /* This is where we will send messages to */
	wc.hInstance	 = hInstance;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	
	/* White, COLOR_WINDOW is just a #define for a system color, try Ctrl+Clicking it */
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszClassName = "WindowClass";
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION); /* Load a standard icon */
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION); /* use the name "A" to use the project icon */

	if(!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	
	FILE *file;
	file = fopen("User.txt", "r");
	int i=0;
	while((fgets(fileLine, 256, file))!=NULL)
    {
			if(i==1)
			{
				strncpy(userDataId, fileLine, sizeof(fileLine));
			}
			if(i==3)
			{
				strncpy(userDataName, fileLine, sizeof(fileLine));
			}
			if(i==5)
			{
				strncpy(userDataMoney, fileLine, sizeof(fileLine));
			}
			i++;
    }
    i=0;
	fclose(file);
	sscanf(userDataMoney, "%d", &money);

	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE,"WindowClass","Caption",WS_VISIBLE|WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, /* x */
		CW_USEDEFAULT, /* y */
		800, /* width */
		600, /* height */
		NULL,NULL,hInstance,NULL);
	BT_Play = CreateWindowEx(NULL,"BUTTON","Play",WS_VISIBLE|WS_CHILD,
		330, /* x */
		440, /* y */
		100, /* width */
		50, /* height */
		hwnd,(HMENU)id_BT_Play,hInstance,NULL);
	BT_Info = CreateWindowEx(NULL,"BUTTON","Info",WS_VISIBLE|WS_CHILD,
		600, /* x */
		200, /* y */
		100, /* width */
		50, /* height */
		hwnd,(HMENU)id_BT_Info,hInstance,NULL);
	STATIC_ProgBarPos = CreateWindowEx(NULL,"Static", "0", WS_CHILD | WS_VISIBLE, 
        270,
		60,
		30,
		30,
		hwnd, (HMENU)id_STATIC_ProgBarPos, NULL, NULL);
	STATIC_MoneyStatus = CreateWindowEx(NULL,"Static", "0", WS_CHILD | WS_VISIBLE, 
        400,
		100,
		40,
		30,
		hwnd, (HMENU)id_STATIC_MoneyStatus, NULL, NULL);
	movieProgress = CreateWindowW(TRACKBAR_CLASSW, L"Trackbar Control",
        WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS,
        20,
		20,
		500,
		40,
		hwnd, (HMENU) 3, NULL, NULL);
	
    SendMessageW(movieProgress, TBM_SETRANGE,  TRUE, MAKELONG(0, 100)); 
    SendMessageW(movieProgress, TBM_SETPAGESIZE, 0,  10); 
    SendMessageW(movieProgress, TBM_SETTICFREQ, 10, 0); 
    SendMessageW(movieProgress, TBM_SETPOS, TRUE, 0); 
	
	if(devStatus == 1)
	{
		STATIC_forTest = CreateWindowEx(NULL,"Static", "0", WS_CHILD | WS_VISIBLE, 
        1,
		1,
		20,
		20,
		hwnd, (HMENU)id_STATIC_DBStatus, NULL, NULL);
	}

	if(hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}
	while(GetMessage(&msg, NULL, 0, 0) > 0) { /* If no error is received... */
		TranslateMessage(&msg); /* Translate key codes to chars if present */
		DispatchMessage(&msg); /* Send it to WndProc */
	}
	return msg.wParam;
}
void UpdateLabel(void) 
{

    LRESULT pos = SendMessageW(movieProgress, TBM_GETPOS, 0, 0);
    wchar_t buf[4];
    sprintf(buf, "%d", pos);

    SetWindowText(STATIC_ProgBarPos, buf);
}

void WatchingMovie(void)
{
	while(BT_PlayStatus == 1)
	{
		money-=20;
		//movieProgress+=1;
		int pos = SendMessageW(movieProgress, TBM_GETPOS, 0, 0);
		SendMessageW(movieProgress, TBM_SETPOS, TRUE, pos+1);
		pthread_create(&thread2, NULL, UpdateLabel, NULL);
		char tempStr[20];
		sprintf(tempStr, "%d", money);
		SetWindowText(STATIC_MoneyStatus, tempStr);
		sleep(1);
		//SwitchStatus();
	}
}
void SwitchStatus(void)
{
	if(BT_PlayStatus == 1)
	{
		BT_PlayStatus = 0;
		SetWindowText(BT_Play,"Play");
	}
	else
	{
		BT_PlayStatus = 1;
		SetWindowText(BT_Play,"Pause");
	}
}
