#include "pch.h"
#include "FileUtil.h" 

FileUtil::FileUtil()
{
	//用当前目录初始化m_szInitDir 
	_getcwd(m_szInitDir, _MAX_PATH);

	//如果目录的最后一个字母不是'\',则在最后加上一个'\' 
	int len = strlen(m_szInitDir);
	if (m_szInitDir[len - 1] != '\\')
		strcat(m_szInitDir, "\\");

	memcpy(m_szCurDir, m_szInitDir, sizeof(m_szInitDir));
}

void FileUtil::InitDir()
{
}

bool FileUtil::SetInitDir(const char *dir)
{
	memset(m_szCurDir, 0, sizeof(m_szCurDir));
	memcpy(m_szCurDir, m_szInitDir, sizeof(m_szInitDir));

	////先把dir转换为绝对路径 
	//if (_fullpath(m_szCurDir, dir, _MAX_PATH) == NULL)
	//	return false;

	strcat(m_szCurDir, dir);

	//判断目录是否存在 
	if (_chdir(m_szCurDir) != 0)
		return false;

	//如果目录的最后一个字母不是'\',则在最后加上一个'\' 
	int len = strlen(m_szCurDir);
	if (m_szCurDir[len - 1] != '\\')
		strcat(m_szCurDir, "\\");

	return true;
}

void FileUtil::GetFullPath(std::string& path)
{
	path.insert(0, m_szCurDir);
}

OpFileResult FileUtil::CreateDir(const char* dir)
{
	string folderPath = m_szCurDir;

	int len = strlen(m_szCurDir);
	if (folderPath[len - 1] != '\\')
		folderPath.append("\\");
	folderPath.append(dir);

	if (0 != _access(folderPath.c_str(), 0))
	{
		// if this folder not exist, create a new one.
		// 返回 0 表示创建成功，-1 表示失败
		if (_mkdir(folderPath.c_str()) != 0)
		{
			return OpFileResult::OFR_CREATE_DIR_ERROR;
		}
	}
	return OFR_SUCCESS;
}

vector<string> FileUtil::BeginBrowseFilenames(const char *filespec)
{
	ProcessDir(m_szCurDir, NULL);
	return GetDirFilenames(m_szCurDir, filespec);
}

bool FileUtil::BeginBrowse(const char *filespec)
{
	ProcessDir(m_szCurDir, NULL);
	return BrowseDir(m_szCurDir, filespec);
}

bool FileUtil::BrowseDir(const char *dir, const char *filespec)
{
	_chdir(dir);

	//首先查找dir中符合要求的文件 
	long hFile;
	_finddata_t fileinfo;
	if ((hFile = _findfirst(filespec, &fileinfo)) != -1)
	{
		do
		{
			//检查是不是目录 
			//如果不是,则进行处理 
			if (!(fileinfo.attrib & _A_SUBDIR))
			{
				char filename[_MAX_PATH];
				strcpy(filename, dir);
				strcat(filename, fileinfo.name);
				cout << filename << endl;
				if (!ProcessFile(filename))
					return false;
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	//查找dir中的子目录 
	//因为在处理dir中的文件时，派生类的ProcessFile有可能改变了 
	//当前目录，因此还要重新设置当前目录为dir。 
	//执行过_findfirst后，可能系统记录下了相关信息，因此改变目录 
	//对_findnext没有影响。 
	_chdir(dir);
	if ((hFile = _findfirst("*.*", &fileinfo)) != -1)
	{
		do
		{
			//检查是不是目录 
			//如果是,再检查是不是 . 或 ..  
			//如果不是,进行迭代 
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp
				(fileinfo.name, "..") != 0)
				{
					char subdir[_MAX_PATH];
					strcpy(subdir, dir);
					strcat(subdir, fileinfo.name);
					strcat(subdir, "\\");
					ProcessDir(subdir, dir);
					if (!BrowseDir(subdir, filespec))
						return false;
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	return true;
}

vector<string> FileUtil::GetDirFilenames(const char *dir, const char *filespec)
{
	_chdir(dir);
	vector<string>filename_vector;
	filename_vector.clear();

	//首先查找dir中符合要求的文件 
	long hFile;
	_finddata_t fileinfo;
	if ((hFile = _findfirst(filespec, &fileinfo)) != -1)
	{
		do
		{
			//检查是不是目录 
			//如果不是,则进行处理 
			if (!(fileinfo.attrib & _A_SUBDIR))
			{
				char filename[_MAX_PATH];
				strcpy(filename, dir);
				strcat(filename, fileinfo.name);
				filename_vector.push_back(filename);
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	//查找dir中的子目录 
	//因为在处理dir中的文件时，派生类的ProcessFile有可能改变了 
	//当前目录，因此还要重新设置当前目录为dir。 
	//执行过_findfirst后，可能系统记录下了相关信息，因此改变目录 
	//对_findnext没有影响。 
	_chdir(dir);
	if ((hFile = _findfirst("*.*", &fileinfo)) != -1)
	{
		do
		{
			//检查是不是目录 
			//如果是,再检查是不是 . 或 ..  
			//如果不是,进行迭代 
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp
				(fileinfo.name, "..") != 0)
				{
					char subdir[_MAX_PATH];
					strcpy(subdir, dir);
					strcat(subdir, fileinfo.name);
					strcat(subdir, "\\");
					ProcessDir(subdir, dir);
					vector<string>tmp = GetDirFilenames(subdir, filespec);
					for (vector<string>::iterator it = tmp.begin(); it < tmp.end(); it++)
					{
						filename_vector.push_back(*it);
					}
				}
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
	return filename_vector;
}

bool FileUtil::ProcessFile(const char *filename)
{
	return true;
}

void FileUtil::ProcessDir(const char
	*currentdir, const char *parentdir)
{
}