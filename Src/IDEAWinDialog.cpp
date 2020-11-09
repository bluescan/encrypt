//
// IDEAWinDialog.cpp
//
// The IDEAWin dialog box thats used for encrypting or decrypting.
//
// Copyright (c) 2000 Tristan Grimmer.
// Please read the accompanying licence file.
//

#include "PrecompiledHeaders.h"
#include "IDEAWin.h"
#include "IDEAWinDialog.h"
#include "../IDEAWin/resource.h"
#include "IDEA.h"


BEGIN_MESSAGE_MAP(CIdeaWinDlg, CDialog)
	ON_WM_PAINT()
	ON_BN_CLICKED(ID_DECRYPT, OnDecrypt)
	ON_BN_CLICKED(ID_ENCRYPT, OnEncrypt)
END_MESSAGE_MAP()


CIdeaWinDlg::CIdeaWinDlg(CWnd* parent) :
	CDialog(IDD_DIALOG_IDEA, parent)
{
}


CIdeaWinDlg::~CIdeaWinDlg()
{
}


int CIdeaWinDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set default button to decrypt.
	SetDefID(ID_DECRYPT);

	CEdit* editKey = (CEdit*)GetDlgItem(IDC_EDIT_PASS_PHRASE);
	editKey->SetWindowText("");

	return 1;
}


void CIdeaWinDlg::OnDecrypt()
{
	printf("Decrypt pressed.\n");

	// Get the pass phrase
	CEdit* editPass = (CEdit*)GetDlgItem(IDC_EDIT_PASS_PHRASE);
	CString pass;
	editPass->GetWindowText(pass);
	
	if (pass.IsEmpty())
	{
		char msg[256];
		sprintf(msg, "Enter the correct pass phrase first!");
		MessageBox(msg);
		return;
	}

	// OK, we can do the decryption
	bool ok = DoDecrypt( pass.GetString() );
	if (ok)
	{
		char msg[256];
		sprintf(msg, "Decrypted copy of file successfully created.");
		MessageBox(msg);
		EndDialog(IDOK);
	}
	else
	{
		char msg[256];
		sprintf(msg, "%s\nDecrypted copy of file could not be created.\n\nYou will have to start over.", GetErrorMessage());
		MessageBox(msg);
		EndDialog(IDOK);
	}
}


void CIdeaWinDlg::OnEncrypt()
{
	printf("Encrypt pressed.\n");

	// Get the pass phrase
	CEdit* editPass = (CEdit*)GetDlgItem(IDC_EDIT_PASS_PHRASE);
	CString pass;
	editPass->GetWindowText(pass);
	
	if (pass.IsEmpty())
	{
		char msg[256];
		sprintf(msg, "Choose a pass phrase first!");
		MessageBox(msg);
		return;
	}

	// Bring up a file dialog.
	static char filter[] = "All Files (*.*)|*.*||";

	CFileDialog fileDialog
	(
		1, "set", 0,
		OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_ENABLESIZING,
		filter, 0
	);

	if (fileDialog.DoModal() == IDOK)
	{
		// OK, we can do the encryption.
		bool ok = DoEncrypt( fileDialog.GetPathName().GetString(), pass.GetString() );
		if (ok)
		{
			char msg[256];
			sprintf(msg, "Encrypted copy of file successfully created.");
			MessageBox(msg);
			EndDialog(IDOK);
		}
		else
		{
			char msg[256];
			sprintf(msg, "Encrypted copy of file could not be created.\n%s", GetErrorMessage());
			MessageBox(msg);
		}
	}
}


// If you add a minimize button to your dialog, you will need the code below
// to draw the icon.  For MFC applications using the document/view model,
// this is automatically done for you by the framework.
void CIdeaWinDlg::OnPaint() 
{
	if (IsIconic())
	{
	}
	else
	{
		CDialog::OnPaint();
	}
}
