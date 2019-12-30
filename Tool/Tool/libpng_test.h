//#pragma once
//
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <png.h>
//
//
//int check_is_png(FILE **fp, const char *filename) //����Ƿ�png�ļ�
//{
//	char checkheader[PNG_BYTES_TO_CHECK]; //��ѯ�Ƿ�pngͷ
//	*fp = fopen(filename, "rb");
//	if (*fp == NULL) {
//		printf("open failed ...1\n");
//		return -1;
//	}
//	if (fread(checkheader, 1, PNG_BYTES_TO_CHECK, *fp) != PNG_BYTES_TO_CHECK) //��ȡpng�ļ����ȴ���ֱ���˳�
//		return 0;
//	return png_sig_cmp((png_const_bytep)checkheader, 0, PNG_BYTES_TO_CHECK); //0��ȷ, ��0����
//}
//
//int decode_png(const char *filename, pic_data *out) //ȡ��png�ļ��е�rgb����
//{
//	png_structp png_ptr; //png�ļ����
//	png_infop	info_ptr;//pngͼ����Ϣ���
//	int ret;
//	FILE *fp;
//	if (check_is_png(&fp, filename) != 0) {
//		printf("file is not png ...\n");
//		return -1;
//	}
//	printf("launcher[%s] ...\n", PNG_LIBPNG_VER_STRING); //��ӡ��ǰlibpng�汾��
//
//	//1: ��ʼ��libpng�����ݽṹ :png_ptr, info_ptr
//	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
//	info_ptr = png_create_info_struct(png_ptr);
//
//	//2: ���ô���ķ��ص�
//	setjmp(png_jmpbuf(png_ptr));
//	rewind(fp); //�ȼ�fseek(fp, 0, SEEK_SET);
//
//	//3: ��png�ṹ����ļ���io���а� 
//	png_init_io(png_ptr, fp);
//	//4:��ȡpng�ļ���Ϣ�Լ�ǿתת����RGBA:8888���ݸ�ʽ
//	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0); //��ȡ�ļ���Ϣ
//	int channels, color_type;
//	channels = png_get_channels(png_ptr, info_ptr); //ͨ������
//	color_type = png_get_color_type(png_ptr, info_ptr);//��ɫ����
//	out->bit_depth = png_get_bit_depth(png_ptr, info_ptr);//λ���	
//	out->width = png_get_image_width(png_ptr, info_ptr);//��
//	out->height = png_get_image_height(png_ptr, info_ptr);//��
//
//	//if(color_type == PNG_COLOR_TYPE_PALETTE)
//	//	png_set_palette_to_rgb(png_ptr);//Ҫ��ת��������ɫ��RGB
//	//if(color_type == PNG_COLOR_TYPE_GRAY && out->bit_depth < 8)
//	//	png_set_expand_gray_1_2_4_to_8(png_ptr);//Ҫ��λ���ǿ��8bit
//	//if(out->bit_depth == 16)
//	//	png_set_strip_16(png_ptr);//Ҫ��λ���ǿ��8bit
//	//if(png_get_valid(png_ptr,info_ptr,PNG_INFO_tRNS))
//	//	png_set_tRNS_to_alpha(png_ptr);
//	//if(color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
//	//	png_set_gray_to_rgb(png_ptr);//�Ҷȱ���ת����RG
//	printf("channels = %d color_type = %d bit_depth = %d width = %d height = %d ...\n",
//		channels, color_type, out->bit_depth, out->width, out->height);
//
//	int i, j, k;
//	int size, pos = 0;
//	int temp;
//
//	//5: ��ȡʵ�ʵ�rgb����
//	png_bytepp row_pointers; //ʵ�ʴ洢rgb���ݵ�buf
//	row_pointers = png_get_rows(png_ptr, info_ptr); //Ҳ���Էֱ�ÿһ�л�ȡpng_get_rowbytes();
//	size = out->width * out->height; //�����ڴ��ȼ���ռ�
//	if (channels == 4 || color_type == PNG_COLOR_TYPE_RGB_ALPHA) { //�ж���24λ����32λ
//		out->alpha_flag = HAVE_ALPHA; //��¼�Ƿ���͸��ͨ��
//		size *= (sizeof(unsigned char) * 4); //size = out->width * out->height * channel
//		out->rgba = (png_bytep)malloc(size);
//		if (NULL == out->rgba) {
//			printf("malloc rgba faile ...\n");
//			png_destroy_read_struct(&png_ptr, &info_ptr, 0);
//			fclose(fp);
//			return -1;
//		}
//		//��row_pointers�����ʵ�ʵ�rgb���ݳ���
//		temp = channels - 1;
//		for (i = 0; i < out->height; i++)
//			for (j = 0; j < out->width * 4; j += 4)
//				for (k = temp; k >= 0; k--)
//					out->rgba[pos++] = row_pointers[i][j + k];
//	}
//	else if (channels == 3 || color_type == PNG_COLOR_TYPE_RGB) { //�ж���ɫ�����24λ����32λ
//		out->alpha_flag = NOT_HAVE_ALPHA;
//		size *= (sizeof(unsigned char) * 3);
//		out->rgba = (png_bytep)malloc(size);
//		if (NULL == out->rgba) {
//			printf("malloc rgba faile ...\n");
//			png_destroy_read_struct(&png_ptr, &info_ptr, 0);
//			fclose(fp);
//			return -1;
//		}
//		//��row_pointers�����ʵ�ʵ�rgb����
//		temp = (3 * out->width);
//		for (i = 0; i < out->height; i++) {
//			for (j = 0; j < temp; j += 3) {
//				out->rgba[pos++] = row_pointers[i][j + 2];
//				out->rgba[pos++] = row_pointers[i][j + 1];
//				out->rgba[pos++] = row_pointers[i][j + 0];
//			}
//		}
//	}
//	else return -1;
//	//6:�����ڴ�
//	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
//	fclose(fp);
//	//��ʱ�� ���ǵ�out->rgba�����Ѿ��洢��ʵ�ʵ�rgb������
//	//��������Ժ�free(out->rgba)
//	return 0;
//}
//
//int RotationRight90(unsigned char * src, int srcW, int srcH, int channel) //˳ʱ����ת90��
//{
//	unsigned char * tempSrc = NULL; //��ʱ��buf������¼ԭʼ��ͼ��(δ��ת֮ǰ��ͼ��)
//	int mSize = srcW * srcH * sizeof(char) * channel;
//	int i = 0;
//	int j = 0;
//	int k = 0;
//	int l = 3;
//	int desW = 0;
//	int desH = 0;
//
//	desW = srcH;
//	desH = srcW;
//
//	tempSrc = (unsigned char *)malloc(sizeof(char) * srcW * srcH * channel);
//	memcpy(tempSrc, src, mSize); //����ԭʼͼ����tempbuf
//	for (i = 0; i < desH; i++)
//	{
//		for (j = 0; j < desW; j++)
//		{
//			for (k = 0; k < channel; k++)
//			{
//				src[(i * desW + j) * channel + k] = tempSrc[((srcH - 1 - j) * srcW + i) * channel + k]; //�滻����
//			}
//		}
//	}
//	free(tempSrc);
//	return 0;
//}
//
//int write_png_file(const char *filename, pic_data *out) //����һ���µ�pngͼ��
//{
//	png_structp png_ptr;
//	png_infop 	info_ptr;
//	png_byte color_type;
//	png_bytep * row_pointers;
//	FILE *fp = fopen(filename, "wb");
//	if (NULL == fp) {
//		printf("open failed ...2\n");
//		return -1;
//	}
//	//1: ��ʼ��libpng�ṹ��  
//	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
//	if (!png_ptr) {
//		printf("png_create_write_struct failed ...\n");
//		return -1;
//	}
//	//2: ��ʼ��png_infop�ṹ�� �� 
//	//�˽ṹ�������ͼ��ĸ�����Ϣ��ߴ磬����λ��, ��ɫ���͵ȵ�
//	info_ptr = png_create_info_struct(png_ptr);
//	if (!info_ptr) {
//		printf("png_create_info_struct failed ...\n");
//		return -1;
//	}
//	//3: ���ô��󷵻ص�
//	if (setjmp(png_jmpbuf(png_ptr))) {
//		printf("error during init_io ...\n");
//		return -1;
//	}
//	//4:���ļ�IO��Png�ṹ��
//	png_init_io(png_ptr, fp);
//	if (setjmp(png_jmpbuf(png_ptr))) {
//		printf("error during init_io ...\n");
//		return -1;
//	}
//	if (out->alpha_flag == HAVE_ALPHA) color_type = PNG_COLOR_TYPE_RGB_ALPHA;
//	else color_type = PNG_COLOR_TYPE_RGB;
//	//5�������Լ�д��ͷ����Ϣ��Png�ļ�
//	png_set_IHDR(png_ptr, info_ptr, out->width, out->height, out->bit_depth,
//		color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
//	png_write_info(png_ptr, info_ptr);
//	if (setjmp(png_jmpbuf(png_ptr))) {
//		printf("error during init_io ...\n");
//		return -1;
//	}
//	int channels, temp;
//	int i, j, pos = 0;
//	if (out->alpha_flag == HAVE_ALPHA) {
//		channels = 4;
//		temp = (4 * out->width);
//		printf("have alpha ...\n");
//	}
//	else {
//		channels = 3;
//		temp = (3 * out->width);
//		printf("not have alpha ...\n");
//	}
//	// ˳ʱ����ת90�� �� ��ת����һ��Ҫ��width ��height���� ��Ȼ�õ���ͼ���ǻ���  ��ת���ξ�����ʱ����תһ��
//	//RotationRight90(out->rgba, out->width, out->height, channels);
//	//RotationRight90(out->rgba, out->height, out->width, channels);
//	//RotationRight90(out->rgba, out->width, out->height, channels);
//	row_pointers = (png_bytep*)malloc(out->height * sizeof(png_bytep));
//	for (i = 0; i < out->height; i++) {
//		row_pointers[i] = (png_bytep)malloc(temp * sizeof(unsigned char));
//		for (j = 0; j < temp; j += channels) {
//			if (channels == 4) {
//				row_pointers[i][j + 3] = out->rgba[pos++];
//				row_pointers[i][j + 2] = out->rgba[pos++];
//				row_pointers[i][j + 1] = out->rgba[pos++];
//				row_pointers[i][j + 0] = out->rgba[pos++];
//			}
//			else {
//				row_pointers[i][j + 2] = out->rgba[pos++];
//				row_pointers[i][j + 1] = out->rgba[pos++];
//				row_pointers[i][j + 0] = out->rgba[pos++];
//			}
//		}
//	}
//	//6: д��rgb���ݵ�Png�ļ�
//	png_write_image(png_ptr, (png_bytepp)row_pointers);
//	if (setjmp(png_jmpbuf(png_ptr))) {
//		printf("error during init_io ...\n");
//		return -1;
//	}
//	//7: д��β����Ϣ
//	png_write_end(png_ptr, NULL);
//	//8:�ͷ��ڴ� ,����png�ṹ��
//	for (i = 0; i < out->height; i++)
//		free(row_pointers[i]);
//	free(row_pointers);
//	png_destroy_write_struct(&png_ptr, &info_ptr);
//	fclose(fp);
//	return 0;
//}
//
////int main(int argc, char **argv)
////{
////	pic_data out;
////	if (argc == 3) {
////		decode_png(argv[1], &out);
////		write_png_file(argv[2], &out);
////		free(out.rgba);
////	}
////	else {
////		puts("please input two file, \nargv[1]:source.png argv[2]:dest.png");
////	}
////	return 0;
////}
