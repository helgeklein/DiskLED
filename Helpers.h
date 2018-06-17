DWORD	ReadINIValue (CString sINIFile, CString sSection, CString sKey, CString* sValue);
DWORD	WriteINIValue (const CString &sINIFile, const CString &sSection, const CString &sKey, const CString &sValue);
DWORD	WriteINIValue (const CString &sINIFile, const CString &sSection, const CString &sKey, const int &nValue);
DWORD	WriteINIValue (const CString &sINIFile, const CString &sSection, const CString &sKey, const double &nValue);
BOOL	IsFileExistent (CString sPath);
DWORD	GetFileAttributesAPIWrapper (CString sPath, DWORD* nAttributes);
BOOL	Split (CString sDelimiter, CString sInput, CStringArray* asOutput);
void	SelectComboBoxString (CComboBox& oBox, CString& sItem);