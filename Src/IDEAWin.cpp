//
// IDEAWin.cpp
//
// The MFC IDEA application.
//
// Copyright (c) 2000 Tristan Grimmer.
// Please read the accompanying licence file.
//

#include "PrecompiledHeaders.h"
#include "IDEAWin.h"
#include "IDEAWinDialog.h"
#include "../IDEAWin/resource.h"


BEGIN_MESSAGE_MAP(CIdeaWinApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


CIdeaWinApp cIdeaWinApp;


CIdeaWinApp::CIdeaWinApp()
{
	#ifdef IDEA_DEBUG
		CreateOutputConsole();
	#endif
}


CIdeaWinApp::~CIdeaWinApp()
{
	#ifdef IDEA_DEBUG
		DestroyOutputConsole();
	#endif
}


int CIdeaWinApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();
	CWinApp::InitInstance();
	AfxEnableControlContainer();

	CIdeaWinDlg dlg;
	m_pMainWnd = &dlg;
	LRESULT response = dlg.DoModal();

	if (response == IDOK)
	{
	}

	// Since the dialog has been closed, return False so that we exit the
	// application, rather than start the application's message pump.
	return 0;
}


void CIdeaWinApp::CreateOutputConsole()
{
	AllocConsole();

	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screenInfo;
	GetConsoleScreenBufferInfo(console, &screenInfo);

	COORD size =
	{
		screenInfo.dwSize.X,
		screenInfo.dwSize.Y + 100
	};

	SetConsoleScreenBufferSize(console, size);
	freopen("CON", "w", stdout);
	printf("The Simple IDEA Debugging Console.\n");
}


void CIdeaWinApp::DestroyOutputConsole()
{
	FreeConsole();
}
