#include "pch.h"
#include "FileUtil.h" 

FileUtil::FileUtil()
{
	//�õ�ǰĿ¼��ʼ��m_szInitDir 
	_getcwd(m_szInitDir, _MAX_PATH);

	//���Ŀ¼�����һ����ĸ����'\',����������һ��'\' 
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

	////�Ȱ�dirת��Ϊ����·�� 
	//if (_fullpath(m_szCurDir, dir, _MAX_PATH) == NULL)
	//	return false;

	strcat(m_szCurDir, dir);

	//�ж�Ŀ¼�Ƿ���� 
	if (_chdir(m_szCurDir) != 0)
		return false;

	//���Ŀ¼�����һ����ĸ����'\',����������һ��'\' 
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
		// ���� 0 ��ʾ�����ɹ���-1 ��ʾʧ��
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

	//���Ȳ���dir�з���Ҫ����ļ� 
	long hFile;
	_finddata_t fileinfo;
	if ((hFile = _findfirst(filespec, &fileinfo)) != -1)
	{
		do
		{
			//����ǲ���Ŀ¼ 
			//�������,����д��� 
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
	//����dir�е���Ŀ¼ 
	//��Ϊ�ڴ���dir�е��ļ�ʱ���������ProcessFile�п��ܸı��� 
	//��ǰĿ¼����˻�Ҫ�������õ�ǰĿ¼Ϊdir�� 
	//ִ�й�_findfirst�󣬿���ϵͳ��¼���������Ϣ����˸ı�Ŀ¼ 
	//��_findnextû��Ӱ�졣 
	_chdir(dir);
	if ((hFile = _findfirst("*.*", &fileinfo)) != -1)
	{
		do
		{
			//����ǲ���Ŀ¼ 
			//�����,�ټ���ǲ��� . �� ..  
			//�������,���е��� 
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

	//���Ȳ���dir�з���Ҫ����ļ� 
	long hFile;
	_finddata_t fileinfo;
	if ((hFile = _findfirst(filespec, &fileinfo)) != -1)
	{
		do
		{
			//����ǲ���Ŀ¼ 
			//�������,����д��� 
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
	//����dir�е���Ŀ¼ 
	//��Ϊ�ڴ���dir�е��ļ�ʱ���������ProcessFile�п��ܸı��� 
	//��ǰĿ¼����˻�Ҫ�������õ�ǰĿ¼Ϊdir�� 
	//ִ�й�_findfirst�󣬿���ϵͳ��¼���������Ϣ����˸ı�Ŀ¼ 
	//��_findnextû��Ӱ�졣 
	_chdir(dir);
	if ((hFile = _findfirst("*.*", &fileinfo)) != -1)
	{
		do
		{
			//����ǲ���Ŀ¼ 
			//�����,�ټ���ǲ��� . �� ..  
			//�������,���е��� 
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