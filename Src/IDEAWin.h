//
// IDEAWin.h
//
// The MFC IDEA application.
//
// Copyright (c) 2000 Tristan Grimmer.
// Please read the accompanying licence file.
//

#pragma once


// The main IDEAWin app.
class CIdeaWinApp : public CWinApp
{
public:
	CIdeaWinApp();
	virtual ~CIdeaWinApp();
	virtual int InitInstance();

private:
	void CreateOutputConsole();
	void DestroyOutputConsole();
	DECLARE_MESSAGE_MAP()
};
