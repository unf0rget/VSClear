#include<Windows.h>
#include<shlwapi.h>
#include<conio.h>
#include<string>
#include<vector>



#pragma comment(lib,"shlwapi.lib")
using namespace std;

vector<wstring> CleanArray;

DWORD FilesCount = 0;
ULONGLONG FilesSizeSum = 0;

DWORD CleanCount = 0;
ULONGLONG CleanSizeSum = 0;

wchar_t ExtensionList[][20] = { {L".obj"},{L".tlog"},{L".lastbuildstate"},{L".idb"},{L".pdb"},{L".pch"},{L".res"},{L".ilk"},
{L".exe"},{L".sdf"},{L".ipch"},{L".log"},{L".db"},{L".suo"}};

BOOL ExtensionCompare(const wchar_t* FilePath)
{
	wchar_t* pExtension = PathFindExtension(FilePath);
	if (pExtension == NULL)
		return FALSE;

	for (int i = 0; i < (sizeof(ExtensionList) / 20); i++)
	{
		if (wcsicmp(pExtension, ExtensionList[i]) == 0)
			return TRUE;
	}

	return FALSE;
}

void FindFiles(const wchar_t* Path)
{
	wstring FindPath = Path;
	FindPath += L"\\*";

	WIN32_FIND_DATA FileInfo = { 0 };
	HANDLE File = FindFirstFile(FindPath.c_str(), &FileInfo);

	if (File != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (wcsicmp(L".", FileInfo.cFileName) == 0 || wcsicmp(L"..", FileInfo.cFileName) == 0)
				continue;
			if (FileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				wstring NextPath = Path;
				NextPath += L"\\";
				NextPath += FileInfo.cFileName;
				FindFiles(NextPath.c_str());
			}
			else
			{
				++FilesCount;
				FilesSizeSum += FileInfo.nFileSizeLow;

				if (ExtensionCompare(FileInfo.cFileName))
				{
					CleanCount++;
					CleanSizeSum += FileInfo.nFileSizeLow;
					wstring CleanPath = Path;
					CleanPath += L"\\";
					CleanPath += FileInfo.cFileName;
					CleanArray.push_back(CleanPath);


					wprintf(L"[Add]%16d\t%s\r\n", FileInfo.nFileSizeLow, CleanPath.c_str());
				}

			}
		} while (FindNextFile(File, &FileInfo));
	}
}

void CleanFiles()
{
	for (auto i : CleanArray)
	{
		DeleteFileW(i.c_str());
	}
}

int wmain(int argc,wchar_t* argv[])
{
	for (int i = 1; i < argc; i++)
	{
		wprintf(L"\r\nFind path : %s\r\n\r\n", argv[i]);
		FindFiles(argv[i]);
	}
	wprintf(L"\r\n\r\n");
	wprintf(L"total count : %4d\ttotal size : %16d\r\n", FilesCount, FilesSizeSum);
	wprintf(L"clean count : %4d\tclean size : %16d\r\n\r\n", CleanCount, CleanSizeSum);


	wprintf(L"press any key to clear\r\n");
	system("pause");

	CleanFiles();

	return 0;
}