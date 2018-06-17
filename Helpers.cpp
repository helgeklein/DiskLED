#include "stdafx.h"
#include "Helpers.h"


//////////////////////////////////////////////////////////////////////
//
//		ReadINIValue:
//!	Read the value of a key in a specified section of an INI file
//
//////////////////////////////////////////////////////////////////////

DWORD ReadINIValue (CString sINIFile, CString sSection, CString sKey, CString* sValue)
{
	// Check parameters
	if (! sValue)
	{
		return ERROR_INVALID_PARAMETER;
	}

	// Empty the output string
	sValue->Empty ();

	//
	// Check the INI file for existence
	//
	if (! IsFileExistent (sINIFile))
	{
		return ERROR_FILE_NOT_FOUND;
	}

	//
	// Read the value from the config file
	//
	DWORD		nBuffer	= 4096;
	TCHAR*	pBuffer	= NULL;
	DWORD		nCopied	= 0;
	do
	{
		// Allocate a buffer
		pBuffer	= (TCHAR*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof (TCHAR) * nBuffer);
		if (pBuffer == NULL)
		{
			return ERROR_OUTOFMEMORY;
		}

		// Try to read the value in the section specified by sName
		nCopied	= GetPrivateProfileString (sSection, sKey, NULL, pBuffer, nBuffer, sINIFile);

		if (nCopied == 0)
		{
			HeapFree(GetProcessHeap(), 0, pBuffer);
			pBuffer	= NULL;
			return ERROR_NOT_FOUND;
		}
		else if (nCopied == nBuffer - 2)
		{
			// The buffer is too small
			nBuffer	+=	4096;
			HeapFree(GetProcessHeap(), 0, pBuffer);
			pBuffer	= NULL;
			continue;
		}
		else if (nCopied > nBuffer - 2)
		{
			// This should not happen
			HeapFree(GetProcessHeap(), 0, pBuffer);
			pBuffer	= NULL;
			return ERROR_NOT_FOUND;
		}
		else
		{
			break;
		}
	} while (1);

	// Copy the returned string into the out parameter
	sValue->Format (TEXT ("%s"), pBuffer);

	HeapFree(GetProcessHeap(), 0, pBuffer);
	pBuffer	= NULL;

	return ERROR_SUCCESS;
}


//////////////////////////////////////////////////////////////////////
//
//		IsFileExistent:
//!	Checks whether file exists (and is accessible)
//
//////////////////////////////////////////////////////////////////////

BOOL IsFileExistent (CString sPath)
{
	BOOL	bExists	= FALSE;
	DWORD	nAttrs	= 0;
	
	DWORD	nError	= GetFileAttributesAPIWrapper (sPath, &nAttrs);
	if (nError == ERROR_SUCCESS)
	{
		if (nAttrs & FILE_ATTRIBUTE_DIRECTORY)
		{
			bExists	= FALSE;
		}
		else
		{
			bExists	= TRUE;
		}
	}

	return bExists;
}


//////////////////////////////////////////////////////////////////////
//
//		GetFileAttributesAPIWrapper:
//!	Wrapper for the API function GetFileAttributes
//
//////////////////////////////////////////////////////////////////////

DWORD GetFileAttributesAPIWrapper (CString sPath, DWORD* nAttributes)
{
	DWORD nError		= ERROR_INVALID_FUNCTION;

	// Check parameters
	if (! nAttributes)
	{
		return ERROR_INVALID_PARAMETER;
	}

	// Call GetFileAttributes
	*nAttributes	= GetFileAttributes (sPath);
	if (*nAttributes == INVALID_FILE_ATTRIBUTES)
	{
		nError	= GetLastError ();
	}
	else
	{
		nError	= ERROR_SUCCESS;
	}

	return nError;
}


//////////////////////////////////////////////////////////////////////
//
//		WriteINIValue:
//!		Writes the value of a key in a specified section of an INI file
//
//////////////////////////////////////////////////////////////////////

DWORD WriteINIValue (const CString &sINIFile, const CString &sSection, const CString &sKey, const CString &sValue)
{
	DWORD nError;
	
	//
	// Write the value to the config file
	//
	nError	= WritePrivateProfileString (sSection, sKey, sValue, sINIFile);
	if (nError == 0)
	{
		nError = GetLastError();
		return nError;
	}

	return ERROR_SUCCESS;
}

DWORD WriteINIValue (const CString &sINIFile, const CString &sSection, const CString &sKey, const int &nValue)
{
	CString	sValue;
	sValue.Format (TEXT ("%d"), nValue);

	return WriteINIValue (sINIFile, sSection, sKey, sValue);
}

DWORD WriteINIValue (const CString &sINIFile, const CString &sSection, const CString &sKey, const double &nValue)
{
	CString	sValue;
	sValue.Format (TEXT ("%f"), nValue);

	return WriteINIValue (sINIFile, sSection, sKey, sValue);
}


//////////////////////////////////////////////////////////////////////
//
// Split: Called by various CSetACL functions. Emulation of Perl's split function.
//
//////////////////////////////////////////////////////////////////////

BOOL Split (CString sDelimiter, CString sInput, CStringArray* asOutput)
{
	try
	{
		// Delete contents of the output array
		asOutput->RemoveAll ();

		// Remove leading and trailing whitespace
		sInput.TrimLeft ();
		sInput.TrimRight ();

		// Find each substring and add it to the output array
		int i = 0, j;
		while ((j = sInput.Find (sDelimiter, i)) != -1)
		{
			asOutput->Add (sInput.Mid (i, j - i));
			i = j + 1;
		}

		// Is there an element left at the end?
		if (sInput.GetLength () > i)
		{
			asOutput->Add (sInput.Mid (i, sInput.GetLength () - i));
		}

		return TRUE;
	}
	catch (CMemoryException* exc)
	{
		exc->Delete ();

		return FALSE;
	}
}


//////////////////////////////////////////////////////////////////////
//
//	Select the item in a combo box list that contains a given string
//
//////////////////////////////////////////////////////////////////////

/*void SelectComboBoxString (CComboBox& oBox, CString& sItem)
{
	for (int nIndex	= 0; nIndex < oBox.GetCount (); nIndex++)
	{
		oBox.GetItemData (nIndex);
	}
}*/