//
// @file IDEAWinDialog.h
//
// The IDEAWin dialog box thats used for encrypting or decrypting.
//
// Copyright (c) 2000 Tristan Grimmer.
// Please read the accompanying licence file.
//

#pragma once


class CIdeaWinDlg : public CDialog
{
public:
	CIdeaWinDlg(CWnd* parent = 0);
	~CIdeaWinDlg();

protected:
	// Message map functions.
	virtual int OnInitDialog();
	void OnPaint();
	void OnDecrypt();
	void OnEncrypt();

private:
	DECLARE_MESSAGE_MAP()
};
