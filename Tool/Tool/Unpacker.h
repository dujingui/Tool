#pragma once
#include "CImage.h"
#include <vector>
#include "FileUtil.h"
#include "Tool.h"

int direction[8][2] = { 0, -1, 1, -1, 1, 0, 1, 1, 0, 1, -1, 1, -1, 0, -1, -1 };

void find(CImage* pImg, CPoint& p, ColorType** v)
{
	for (int i = 0; i < 8; i++)
	{
		CPoint np = p + CPoint(direction[i][0], direction[i][1]);

		if (np.x < 0)
			continue;
		if (np.x >= pImg->width())
			continue;
		if (np.y < 0)
			continue;
		if (np.y >= pImg->height())
			continue;
		if (v[np.x][np.y] > 0)
			continue;
		if (pImg->isAlpha(np))
			continue;

		v[np.x][np.y] = 1;
		find(pImg, np, v);
	}
}


void startQie(CImage* pImg, CPoint& p, int& minX, int& maxX, int& minY, int& maxY)
{
	ColorType** v = new ColorType*[pImg->width()];
	for (int i = 0; i < pImg->width(); i++)
	{
		ColorType* t = new ColorType[pImg->height()];
		memset(t, 0, pImg->height() * sizeof(ColorType));
		v[i] = t;
	}

	find(pImg, p, v);

	for (int i = 0; i < pImg->width(); i++)
	{
		for (int j = 0; j < pImg->height(); j++)
		{
			if (v[i][j] <= 0)
				continue;
			if (i < minX)
				minX = i;
			if (i > maxX)
				maxX = i;
			if (j < minY)
				minY = j;
			if (j > maxY)
				maxY = j;
		}
	}

	for (int i = 0; i < pImg->width(); i++)
	{
		delete[] v[i];
		v[i] = nullptr;
	}

	delete[] v;
}


void unpacker(int alpha) {

	FileUtil* pFileUtil = new FileUtil();
	if (pFileUtil->SetInitDir("origin") == false)
	{
		std::cout << "origin 文件夹不存在!" << endl;
	}

	vector<std::string> files = pFileUtil->BeginBrowseFilenames("*.png");

	if (pFileUtil->SetInitDir("unpacket") == false)
	{
		std::cout << "unpacket 文件夹不存在!" << endl;
	}

	for (int ff = 0; ff < files.size(); ff++)
	{
		CImage* pImg = new CImage(files[ff]);
		if (!pImg)
		{
			std::cout << "create CImage fail by %s" << files[ff] << endl;
			continue;
		}

		int sum = 1;

		for (int i = 0; i < pImg->width(); i++)
		{
			for (int j = 0; j < pImg->height(); j++)
			{
				if (pImg->isAlpha(i, j, alpha))
				{
					continue;
				}
				CPoint p(i, j);
				int minX = 9999, minY = 9999, maxX = 0, maxY = 0;
				startQie(pImg, p, minX, maxX, minY, maxY);

				if (maxX == 0)
				{
					continue;
				}

				std::vector<std::string> filenames;
				std::vector<std::string> temp;
				SplitString(files[ff], filenames, "\\");
				SplitString(filenames[filenames.size() - 1], temp, ".");

				if (pFileUtil->CreateDir(temp[0].c_str()) == OFR_CREATE_DIR_ERROR)
				{
					continue;
				}

				std::string filename = "";
				filename.append(temp[0]);
				filename.append("/file_");
				filename.append(to_string(sum));
				filename.append(".png");
				sum++;

				pFileUtil->GetFullPath(filename);

				CImage* pNewImg = pImg->cut(minX, maxX, minY, maxY);
				if (pNewImg->saveFile(filename.c_str()) == 0)
				{
					std::cout << filename << " success！" << endl;
				}
				else
				{
					std::cout << filename << " fail！" << endl;
				}

				delete pNewImg;
				pNewImg = nullptr;
			}
		}

		delete pImg;
		pImg = nullptr;
	}

	//pImg->print();
	//return;
}
