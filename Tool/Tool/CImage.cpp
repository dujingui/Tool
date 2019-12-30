#include "pch.h"
#include "CImage.h"
#include <vector>
#include <iostream>

CImage::CImage(std::string filename)
{ 
	m_strFilename = filename; 

	init(filename.c_str(), &m_ImgData);
}

CImage::CImage(_pic_data& data)
{
	m_ImgData.alpha_flag = data.alpha_flag;
	m_ImgData.bit_depth = data.bit_depth;
	m_ImgData.height = data.height;
	m_ImgData.width = data.width;

	int len = data.alpha_flag ? 4 : 3;
	int bytesize = len * sizeof(ColorType) * data.width * data.height;

	m_ImgData.rgba = new ColorType[bytesize];

	memcpy(m_ImgData.rgba, data.rgba, bytesize);
}

CImage::~CImage()
{
	delete m_ImgData.rgba;
	m_ImgData.rgba = nullptr;
}


bool CImage::isAlpha(int x, int y, int alpha)
{
	bool bRef = false;

	if (!m_ImgData.alpha_flag)
	{
		return false;
	}

	ColorType* pPixel = this->getpixel(x, y);
	bRef = pPixel[3] <= alpha;

	delete pPixel;

	return bRef;
}

bool CImage::isAlpha(CPoint& p, int alpha)
{
	return this->isAlpha(p.x, p.y, alpha);
}

ColorType* CImage::getpixel(int x, int y)
{
	int len = m_ImgData.alpha_flag ? 4 : 3;

	//一个像素占用的字节数
	int pixelsize = len * sizeof(unsigned char);

	//格式rgba
	unsigned char* pixel = new unsigned char[pixelsize];
	memset(pixel, 0, pixelsize);

	//一行像素占用的字节数
	int rowsize = m_ImgData.width * pixelsize;

	for (int i = 0; i < len; i++)
	{
		pixel[i] = m_ImgData.rgba[y * rowsize + x * pixelsize + i];
	}

	return pixel;
}

int CImage::isPNG(FILE **fp, const char *filename) //检查是否png文件
{
	char checkheader[PNG_BYTES_TO_CHECK]; //查询是否png头
	*fp = fopen(filename, "rb");
	if (*fp == NULL) {
		printf("open failed ...1\n");
		return -1;
	}
	if (fread(checkheader, 1, PNG_BYTES_TO_CHECK, *fp) != PNG_BYTES_TO_CHECK) //读取png文件长度错误直接退出
		return 0;
	return png_sig_cmp((png_const_bytep)checkheader, 0, PNG_BYTES_TO_CHECK); //0正确, 非0错误
}

CImage* CImage::cut(int& minX, int& maxX, int& minY, int& maxY)
{
	int w = maxX - minX + 1;
	int h = maxY - minY + 1;

	pic_data data;
	data.alpha_flag = m_ImgData.alpha_flag;
	data.bit_depth = m_ImgData.bit_depth;
	data.width = w;
	data.height = h;

	int len = data.alpha_flag ? 4 : 3;

	//一个像素占用的字节数
	int pixelsize = len * sizeof(unsigned char);

	int pos = 0;
	int rowsize = m_ImgData.width * pixelsize;

	int tempsize = w * pixelsize;

	data.rgba = new ColorType[h * tempsize];

	for (int y = minY; y <= maxY; y++)
	{
		memcpy(data.rgba + pos, m_ImgData.rgba + (y * rowsize + minX * pixelsize), tempsize);
		memset(m_ImgData.rgba + (y * rowsize + minX * pixelsize),0 , tempsize);

		pos += tempsize;
	}

	CImage* pImage = new CImage(data);

	return pImage;
}

int CImage::init(const char *filename, pic_data *out) //取出png文件中的rgb数据
{
	png_structp png_ptr; //png文件句柄
	png_infop	info_ptr;//png图像信息句柄
	int ret;
	FILE *fp;
	if (isPNG(&fp, filename) != 0) {
		printf("file is not png ...\n");
		return -1;
	}
	//printf("launcher[%s] ...\n", PNG_LIBPNG_VER_STRING); //打印当前libpng版本号

	//1: 初始化libpng的数据结构 :png_ptr, info_ptr
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info_ptr = png_create_info_struct(png_ptr);

	//2: 设置错误的返回点
	setjmp(png_jmpbuf(png_ptr));
	rewind(fp); //等价fseek(fp, 0, SEEK_SET);

	//3: 把png结构体和文件流io进行绑定 
	png_init_io(png_ptr, fp);
	//4:读取png文件信息以及强转转换成RGBA:8888数据格式
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0); //读取文件信息
	int channels, color_type;
	channels = png_get_channels(png_ptr, info_ptr); //通道数量
	color_type = png_get_color_type(png_ptr, info_ptr);//颜色类型
	out->bit_depth = png_get_bit_depth(png_ptr, info_ptr);//位深度	
	out->width = png_get_image_width(png_ptr, info_ptr);//宽
	out->height = png_get_image_height(png_ptr, info_ptr);//高

	//if(color_type == PNG_COLOR_TYPE_PALETTE)
	//	png_set_palette_to_rgb(png_ptr);//要求转换索引颜色到RGB
	//if(color_type == PNG_COLOR_TYPE_GRAY && out->bit_depth < 8)
	//	png_set_expand_gray_1_2_4_to_8(png_ptr);//要求位深度强制8bit
	//if(out->bit_depth == 16)
	//	png_set_strip_16(png_ptr);//要求位深度强制8bit
	//if(png_get_valid(png_ptr,info_ptr,PNG_INFO_tRNS))
	//	png_set_tRNS_to_alpha(png_ptr);
	//if(color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	//	png_set_gray_to_rgb(png_ptr);//灰度必须转换成RG
	//printf("channels = %d color_type = %d bit_depth = %d width = %d height = %d ...\n",
	//	channels, color_type, out->bit_depth, out->width, out->height);

	int i, j, k;
	int size, pos = 0;
	int temp;

	//5: 读取实际的rgb数据
	png_bytepp row_pointers; //实际存储rgb数据的buf
	row_pointers = png_get_rows(png_ptr, info_ptr); //也可以分别每一行获取png_get_rowbytes();
	size = out->width * out->height; //申请内存先计算空间
	if (channels == 4 || color_type == PNG_COLOR_TYPE_RGB_ALPHA) { //判断是24位还是32位
		out->alpha_flag = HAVE_ALPHA; //记录是否有透明通道
		size *= (sizeof(unsigned char) * 4); //size = out->width * out->height * channel
		out->rgba = (png_bytep)malloc(size);
		if (NULL == out->rgba) {
			printf("malloc rgba faile ...\n");
			png_destroy_read_struct(&png_ptr, &info_ptr, 0);
			fclose(fp);
			return -1;
		}
		//从row_pointers里读出实际的rgb数据出来
		temp = channels - 1;
		for (i = 0; i < out->height; i++)
			for (j = 0; j < out->width * 4; j += 4)
				for (k = 0; k <= temp; k++)
					out->rgba[pos++] = row_pointers[i][j + k];
	}
	else if (channels == 3 || color_type == PNG_COLOR_TYPE_RGB) { //判断颜色深度是24位还是32位
		out->alpha_flag = NOT_HAVE_ALPHA;
		size *= (sizeof(unsigned char) * 3);
		out->rgba = (png_bytep)malloc(size);
		if (NULL == out->rgba) {
			printf("malloc rgba faile ...\n");
			png_destroy_read_struct(&png_ptr, &info_ptr, 0);
			fclose(fp);
			return -1;
		}
		//从row_pointers里读出实际的rgb数据
		temp = (3 * out->width);
		for (i = 0; i < out->height; i++) {
			for (j = 0; j < temp; j += 3) {
				out->rgba[pos++] = row_pointers[i][j + 0];
				out->rgba[pos++] = row_pointers[i][j + 1];
				out->rgba[pos++] = row_pointers[i][j + 2];
			}
		}
	}
	else return -1;
	//6:销毁内存
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	fclose(fp);
	//此时， 我们的out->rgba里面已经存储有实际的rgb数据了
	//处理完成以后free(out->rgba)
	return 0;
}

int CImage::saveFile(const char *filename)
{
	return saveFile(filename, &m_ImgData);
}

int CImage::saveFile(const char *filename, pic_data *out) //生成一个新的png图像
{
	png_structp png_ptr;
	png_infop 	info_ptr;
	png_byte color_type;
	png_bytep * row_pointers;
	FILE *fp = fopen(filename, "wb");
	if (NULL == fp) {
		printf("open failed ...2\n");
		return -1;
	}
	//1: 初始化libpng结构体  
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if (!png_ptr) {
		printf("png_create_write_struct failed ...\n");
		return -1;
	}
	//2: 初始化png_infop结构体 ， 
	//此结构体包含了图像的各种信息如尺寸，像素位深, 颜色类型等等
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		printf("png_create_info_struct failed ...\n");
		return -1;
	}
	//3: 设置错误返回点
	if (setjmp(png_jmpbuf(png_ptr))) {
		printf("error during init_io ...\n");
		return -1;
	}
	//4:绑定文件IO到Png结构体
	png_init_io(png_ptr, fp);
	if (setjmp(png_jmpbuf(png_ptr))) {
		printf("error during init_io ...\n");
		return -1;
	}
	if (out->alpha_flag == HAVE_ALPHA) color_type = PNG_COLOR_TYPE_RGB_ALPHA;
	else color_type = PNG_COLOR_TYPE_RGB;
	//5：设置以及写入头部信息到Png文件
	png_set_IHDR(png_ptr, info_ptr, out->width, out->height, out->bit_depth,
		color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr, info_ptr);
	if (setjmp(png_jmpbuf(png_ptr))) {
		printf("error during init_io ...\n");
		return -1;
	}
	int channels, temp;
	int i, j, pos = 0;
	if (out->alpha_flag == HAVE_ALPHA) {
		channels = 4;
		temp = (4 * out->width);
	}
	else {
		channels = 3;
		temp = (3 * out->width);
	}
	// 顺时针旋转90度 ， 旋转完了一定要把width 和height调换 不然得到的图像是花的  旋转三次就是逆时针旋转一次
	//RotationRight90(out->rgba, out->width, out->height, channels);
	//RotationRight90(out->rgba, out->height, out->width, channels);
	//RotationRight90(out->rgba, out->width, out->height, channels);
	row_pointers = (png_bytep*)malloc(out->height * sizeof(png_bytep));
	for (i = 0; i < out->height; i++) {
		row_pointers[i] = (png_bytep)malloc(temp * sizeof(unsigned char));
		for (j = 0; j < temp; j += channels) {
			if (channels == 4) {
				row_pointers[i][j + 0] = out->rgba[pos++];
				row_pointers[i][j + 1] = out->rgba[pos++];
				row_pointers[i][j + 2] = out->rgba[pos++];
				row_pointers[i][j + 3] = out->rgba[pos++];
			}
			else {
				row_pointers[i][j + 0] = out->rgba[pos++];
				row_pointers[i][j + 1] = out->rgba[pos++];
				row_pointers[i][j + 2] = out->rgba[pos++];
			}
		}
	}
	//6: 写入rgb数据到Png文件
	png_write_image(png_ptr, (png_bytepp)row_pointers);
	if (setjmp(png_jmpbuf(png_ptr))) {
		printf("error during init_io ...\n");
		return -1;
	}
	//7: 写入尾部信息
	png_write_end(png_ptr, NULL);
	//8:释放内存 ,销毁png结构体
	for (i = 0; i < out->height; i++)
		free(row_pointers[i]);
	free(row_pointers);
	png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(fp);
	return 0;
}

void CImage::print()
{
	std::cout << "width = " << m_ImgData.width << ",height = " << m_ImgData.height << ",depth = " << m_ImgData.bit_depth << ",alpth = " << m_ImgData.alpha_flag << endl;

	int len = m_ImgData.alpha_flag ? 4 : 3;

	//一个像素占用的字节数
	int pixelsize = len * sizeof(unsigned char);

	int rowsize = m_ImgData.width * pixelsize;


	for (int i = 0; i < m_ImgData.height; i++)
	{
		for (int j = 0; j < m_ImgData.width; j++)
		{
			ColorType* pixel = this->getpixel(i, j);

			/*std::cout << (int)(pixel[0]);
			std::cout << "-";
			std::cout << (int)(pixel[1]);
			std::cout << "-";
			std::cout << (int)(pixel[2]);

			if (len > 3)
			{
				std::cout << "-";
				std::cout << (int)(pixel[3]);
				std::cout << " ";
			}*/
			if ( ((pixel[3]) == 0))
			{
				cout << i;
				cout <<" ";
			}

			delete pixel;
		}
		//std::cout << endl;
	}
}