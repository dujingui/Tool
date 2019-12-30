#pragma once

#include <io.h> 
#include <stdlib.h> 
#include <direct.h> 
#include <iostream> 
#include <string> 
#include <vector> 

enum OpFileResult
{
	OFR_SUCCESS					= 0,
	OFR_CREATE_DIR_ERROR		= 1,
};

using namespace std;

class FileUtil
{
protected:
	//��ų�ʼĿ¼�ľ���·������'\'��β 
	char m_szInitDir[_MAX_PATH];
	char m_szCurDir[_MAX_PATH];
public:
	//ȱʡ������ 
	FileUtil();

	//���ó�ʼĿ¼Ϊdir���������false����ʾĿ¼������ 
	bool SetInitDir(const char *dir);

	//��ʼ������ʼĿ¼������Ŀ¼����filespecָ�����͵��ļ� 
	//filespec����ʹ��ͨ��� * ?�����ܰ���·���� 
	//�������false����ʾ�������̱��û���ֹ 
	bool BeginBrowse(const char *filespec);
	vector<string> BeginBrowseFilenames(const char *filespec);

	OpFileResult CreateDir(const char* dir);

	void GetFullPath(std::string& path);
protected:
	//����Ŀ¼dir����filespecָ�����ļ� 
	//������Ŀ¼,���õ����ķ��� 
	//�������false,��ʾ��ֹ�����ļ� 
	bool BrowseDir(const char *dir, const char *filespec);
	vector<string> GetDirFilenames(const char *dir, const char *filespec);
	//����BrowseDirÿ�ҵ�һ���ļ�,�͵���ProcessFile 
	//�����ļ�����Ϊ�������ݹ�ȥ 
	//�������false,��ʾ��ֹ�����ļ� 
	//�û����Ը�д�ú���,�����Լ��Ĵ������� 
	virtual bool ProcessFile(const char *filename);

	//����BrowseDirÿ����һ��Ŀ¼,�͵���ProcessDir 
	//�������ڴ�����Ŀ¼������һ��Ŀ¼����Ϊ�������ݹ�ȥ 
	//������ڴ������ǳ�ʼĿ¼,��parentdir=NULL 
	//�û����Ը�д�ú���,�����Լ��Ĵ������� 
	//�����û�����������ͳ����Ŀ¼�ĸ��� 
	virtual void ProcessDir(const char *currentdir, const char *parentdir);
private:
	void InitDir();
};
