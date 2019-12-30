#pragma once

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>

using namespace std;

typedef unsigned char ColorType;

#define PNG_BYTES_TO_CHECK	8
#define HAVE_ALPHA			1
#define NOT_HAVE_ALPHA		0

typedef struct _pic_data pic_data;
struct _pic_data {
	int width, height; 	//长宽
	int bit_depth; 	   	//位深度
	int alpha_flag;		//是否有透明通道

	//存储结构 一行接一行 ABGR
	unsigned char *rgba;//实际rgb数据
};

struct CPoint
{
	int x;
	int y;

	CPoint(int x = 0, int y = 0)
	{
		this->x = x;
		this->y = y;
	}

	bool operator==(const CPoint& p)
	{
		return p.x == x && p.y == y;
	}

	friend CPoint operator+(const CPoint& p1, const CPoint& p2)
	{
		CPoint p;
		p.x = p1.x + p2.x;
		p.y = p1.y + p2.y;
		return p;
	}
};

class CImage
{
public:
	CImage(std::string filename);
	CImage(_pic_data& data);
	CImage(){}
	~CImage();
public:
	ColorType* getpixel(int x, int y);

	bool isAlpha(CPoint& p, int alpha = 0);
	bool isAlpha(int x, int y, int alpha = 0);

	//生成一个新的png图像
	int saveFile(const char *filename, pic_data *out);
	int saveFile(const char *filename);

	//检查是否png文件
	int isPNG(FILE **fp, const char *filename);

	int init(const char *filename, pic_data *out); //取出png文件中的rgb数据

	CImage* cut(int& minX, int& maxX, int& minY, int& maxY);
	//void find(CPoint& p, vector<CPoint*>& vec);

	int width() { return m_ImgData.width; }

	int height() { return m_ImgData.height; }

	void print();
private:
	pic_data m_ImgData;
	std::string m_strFilename;
};