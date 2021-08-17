#include "stdafx.h"

static WCHAR alphabet[] = L"abcdef012345789";

LPWSTR GetKeyPath(HKEY key) {
	static WCHAR buffer[MAX_PATH] = { 0 };
	DWORD size = sizeof(buffer);
	memset(buffer, 0, sizeof(buffer));
	NtQueryKey(key, 3, buffer, size, &size);
	return buffer + 3;
}

BOOL GetKeyValue(HKEY key, LPCWSTR value, LPBYTE buffer, DWORD* size) {
	if (ERROR_SUCCESS == RegQueryValueEx(key, value, 0, 0, buffer, size)) {
		return TRUE;
	}

	return FALSE;
}

VOID OutSpoofUnique(LPWSTR buffer) {
	for (DWORD i = 0; i < wcslen(buffer); ++i) {
		if (iswxdigit(buffer[i])) {
			buffer[i] = alphabet[rand() % wcslen(alphabet)];
		}
	}
}

VOID KeySpoofOutGUID(HKEY key, LPCWSTR value, LPWSTR buffer, DWORD size) {
	if (!GetKeyValue(key, value, (LPBYTE)buffer, &size)) {
		return;
	}

	OutSpoofUnique(buffer);
	RegSetValueEx(key, value, 0, REG_SZ, (PBYTE)buffer, size);
}

VOID KeySpoofUnique(HKEY key, LPCWSTR value) {
	WCHAR buffer[MAX_PATH] = { 0 };
	KeySpoofOutGUID(key, value, buffer, sizeof(buffer));
}

VOID SpoofUnique(HKEY key, LPCWSTR subkey, LPCWSTR value) {
	OpenThen(key, subkey, {
		KeySpoofUnique(key, value);
		});
}

VOID SpoofUniques(HKEY key, LPCWSTR subkey, LPCWSTR value) {
	OpenThen(key, subkey, {
		WCHAR buffer[0xFFF] = { 0 };
		DWORD size = sizeof(buffer);
		if (!GetKeyValue(key, value, (LPBYTE)buffer, &size)) {
			RegCloseKey(key);
			return;
		}

		for (DWORD i = 0; i < size; ++i) {
			if (iswxdigit(buffer[i])) {
				buffer[i] = alphabet[rand() % (wcslen(alphabet) - 1)];
			}
		}

		RegSetValueEx(key, value, 0, REG_MULTI_SZ, (PBYTE)buffer, size);
		});
}

VOID SpoofDWORD(HKEY key, LPCWSTR subkey, LPCWSTR value) {
	OpenThen(key, subkey, {
		DWORD data = rand();
		});
}

VOID SpoofQWORD(HKEY key, LPCWSTR subkey, LPCWSTR value) {
	OpenThen(key, subkey, {
		LARGE_INTEGER data = { 0 };
		data.LowPart = rand();
		data.HighPart = rand();
		});
}

VOID SpoofBinary(HKEY key, LPCWSTR subkey, LPCWSTR value) {
	OpenThen(key, subkey, {
		DWORD size = 0;
		if (ERROR_SUCCESS != RegQueryValueEx(key, value, 0, 0, 0, &size)) {
			RegCloseKey(key);
			return;
		}

		BYTE* buffer = (BYTE*)malloc(size);
		if (!buffer) {
			RegCloseKey(key);
			return;
		}

		for (DWORD i = 0; i < size; ++i) {
			buffer[i] = (BYTE)(rand() % 0x100);
		}

		RegSetValueEx(key, value, 0, REG_BINARY, buffer, size);
		free(buffer);
		});
}

VOID RenameSubkey(HKEY key, LPCWSTR subkey, LPCWSTR name) {
	HKEY k = 0;
	DWORD error = RegCreateKey(key, name, &k);
	if (ERROR_CHILD_MUST_BE_VOLATILE == error) {
		error = RegCreateKeyEx(key, name, 0, 0, REG_OPTION_VOLATILE, KEY_ALL_ACCESS, 0, &k, 0);
	}

	if (ERROR_SUCCESS != error) {
		return;
	}

	RegCloseKey(k);
}

VOID DeleteKey(HKEY key, LPCWSTR subkey) {
	DWORD s = SHDeleteKey(key, subkey);
	if (ERROR_FILE_NOT_FOUND == s) {
		return;
	}
}

VOID DeleteValue(HKEY key, LPCWSTR subkey, LPCWSTR value) {
	DWORD s = SHDeleteValue(key, subkey, value);
	if (ERROR_FILE_NOT_FOUND == s) {
		return;
	}
}

BOOL AdjustCurrentPrivilege(LPCWSTR privilege) {
	LUID luid = { 0 };
	if (!LookupPrivilegeValue(0, privilege, &luid)) {
		return FALSE;
	}

	TOKEN_PRIVILEGES tp = { 0 };
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	HANDLE token = 0;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &token)) {
		return FALSE;
	}

	if (!AdjustTokenPrivileges(token, FALSE, &tp, sizeof(tp), 0, 0)) {
		CloseHandle(token);
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
		CloseHandle(token);
		return FALSE;
	}

	CloseHandle(token);
	return TRUE;
}

VOID ForceDeleteFile(LPWSTR path) {
	if (!PathFileExists(path)) {
		return;
	}

	PSID all = 0, admin = 0;
	SID_IDENTIFIER_AUTHORITY world = SECURITY_WORLD_SID_AUTHORITY;
	if (!AllocateAndInitializeSid(&world, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &all)) {
		return;
	}

	SID_IDENTIFIER_AUTHORITY auth = SECURITY_NT_AUTHORITY;
	if (!AllocateAndInitializeSid(&auth, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &admin)) {
		FreeSid(all);
		return;
	}

	EXPLICIT_ACCESS access[2] = { 0 };
	access[0].grfAccessPermissions = GENERIC_ALL;
	access[0].grfAccessMode = SET_ACCESS;
	access[0].grfInheritance = NO_INHERITANCE;
	access[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	access[0].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	access[0].Trustee.ptstrName = all;
	access[1].grfAccessPermissions = GENERIC_ALL;
	access[1].grfAccessMode = SET_ACCESS;
	access[1].grfInheritance = NO_INHERITANCE;
	access[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	access[1].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	access[1].Trustee.ptstrName = admin;

	PACL acl = { 0 };
	DWORD error = SetEntriesInAcl(2, access, 0, &acl);
	if (ERROR_SUCCESS != error) {
		FreeSid(all);
		FreeSid(admin);
		return;
	}

	if (ERROR_SUCCESS != (error = SetNamedSecurityInfo((LPWSTR)path, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION, admin, 0, 0, 0))) {
		FreeSid(all);
		FreeSid(admin);
		LocalFree(acl);
		return;
	}

	if (ERROR_SUCCESS != (error = SetNamedSecurityInfo((LPWSTR)path, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, 0, 0, acl, 0))) {
		FreeSid(all);
		FreeSid(admin);
		LocalFree(acl);
		return;
	}

	SetFileAttributes(path, FILE_ATTRIBUTE_NORMAL);

	SHFILEOPSTRUCT op = { 0 };
	op.wFunc = FO_DELETE;
	path[wcslen(path) + 1] = 0;
	op.pFrom = path;
	op.pTo = L"\0";
	op.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
	op.lpszProgressTitle = L"";

	FreeSid(all);
	FreeSid(admin);
	LocalFree(acl);
}

VOID RecursiveDelete(LPWSTR dir, LPWSTR match) {
	WCHAR path[MAX_PATH] = { 0 };
	wsprintf(path, L"%ws\\*", dir);

	WIN32_FIND_DATA fd = { 0 };
	HANDLE f = FindFirstFile(path, &fd);

	do {
		WCHAR sub[MAX_PATH] = { 0 };
		wsprintf(sub, L"%ws\\%ws", dir, fd.cFileName);

		if (wcscmp(fd.cFileName, L".") && wcscmp(fd.cFileName, L"..")) {
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				RecursiveDelete(sub, match);
			}
			else if (StrStr(fd.cFileName, match)) {
				ForceDeleteFile(sub);
			}
		}
	} while (FindNextFile(f, &fd));

	FindClose(f);
}