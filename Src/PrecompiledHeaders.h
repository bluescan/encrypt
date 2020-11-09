//
// PrecompiledHeaders.h
//
// Include headers that are used frequently but changed infrequently.
// Solely for reducing compile time.
//
// Copyright (c) 2000 Tristan Grimmer.
// Please read the accompanying licence file.
//

#pragma once


// Exclude rarely-used stuff from Windows headers.
#define VC_EXTRALEAN


#ifndef WINVER										// Allow use of features specific to Windows 98 or later.
#define WINVER 0x0501								// Change this to the appropriate value to target Windows 2000 or later.
#endif


// Here are the Windows MFC includes.
#include <afx.h>
#include <afxwin.h>									// MFC core and standard components.
#include <afxext.h>									// MFC extensions.
#include <afxdtctl.h>								// MFC support for Internet Explorer 4 Common Controls.
#include <afxcmn.h>									// MFC support for Windows Common Controls.
#include <afxrich.h>								// MFC rich edit.
#include <afxcview.h>								// MFC tree view.
#include <afxadv.h>									// MFC Advanced.  Needed for MRU file list implementation.
