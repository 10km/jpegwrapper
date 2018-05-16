/*
 * testCImg.cpp
 *
 *  Created on: 2016年1月16日
 *      Author: guyadong
 */
//#include "stdio.h"
#include <iostream>
#include <fstream>
#include <string>
#include <iostream>
#include "file_utilits.h"
#include "CImgWrapper.h"
#include "jpeg_mem.h"
#include "j2k_mem.h"
using namespace cimg_library;
//using namespace gdface;
using namespace std;
int main()
{
	try {
		// 定义一个每个颜色 8 位(bit)的 640x400 的彩色图像
		//CImgWrapper<unsigned char> img(640,400,1,3);
		std::wstring jpg_path = L"images/he049.jpg";
		//CImg<unsigned char> image_jpg("images/he049.jpg");
		//std::cout<<"D:/tmp/人像/guyadong-1.jpg"<<std::endl;
		const char *input_jpg_file = "images/he049.jpg";
		const char *output_jpg_file = "images/he049-out.jpg";
		const char *output2_jpg_file = "images/he049-out2.jpg";
		const char *output3_jpg_file = "images/he049-out3.jpg";
		const char *output4_jpg_file = "images/he049-out4.jp2";
		//CImgWrapper<unsigned char> image_jpg(input_jpg_file);
		CImgWrapper<unsigned char> image_jpg;
		std::vector<uint8_t> jpeg_data=gdface::load_binary_file(input_jpg_file);
		auto matrix=read_jpeg_header_mem(jpeg_data.data(), jpeg_data.size());

		//auto matrix=read_jpeg_header_file(input_jpg_file);
		cout<<matrix.width<<"x"<<matrix.height<<"x"<<(uint32_t)matrix.channels<<" color="<<matrix.color_space<<endl;
		image_jpg.load_mem_jpeg(
				jpeg_data,
				[](j_common_ptr cinfo){
					((j_decompress_ptr)cinfo)->out_color_space=JCS_GRAYSCALE;
			});
		auto mat=load_jpeg_mem(jpeg_data.data(), jpeg_data.size(),[](j_common_ptr cinfo) {
			//((j_decompress_ptr)cinfo)->out_color_space = JCS_GRAYSCALE;
			//((j_decompress_ptr)cinfo)->scale_num=1;
			//((j_decompress_ptr)cinfo)->scale_denom=2;
		});
		cimg_library::image_matrix_display(mat, "test display", true);

		//auto mat=load_jpeg_gray_mem(jpeg_data);
		/*save_jpeg_mem(mat,
				[&](const uint8_t *img, unsigned long size) {
					std::ofstream ofs;
					ofs.open(output3_jpg_file, std::ofstream::binary);
					ofs.write((const char*)img, size);
					printf("%s saved,size=%d\n", output3_jpg_file, size);
					ofs.close();
				},100,
				[](j_common_ptr cinfo) {
					//jpeg_set_colorspace((j_compress_ptr)cinfo, JCS_GRAYSCALE);
				});*/
		//save_jpeg_gray_mem(mat,
		//				[&](const uint8_t *img, unsigned long size) {
		//					gdface::save_binary_file(output3_jpg_file,img,size);
		//				});
		auto out_str = save_jpeg_gray_mem_as_string(mat);
		gdface::save_binary_file(output3_jpg_file, out_str.c_str(), out_str.size());

		auto output=save_j2k_mem(mat,45, OPJ_CODEC_JP2);
		gdface::save_binary_file(output4_jpg_file,output.stream_data(),size_t(output.stream_length()));
		//将像素值设为 0（黑色）

		//image_jpg.fill(0);
		// 定义一个紫色
		unsigned char purple[] = { 0,0,255 };

		// 在坐标(100, 100)处画一个紫色的“Hello world”
		image_jpg.draw_text(100,100,"Hello World",purple);
		// 在一个标题为“My first CImg code”的窗口中显示这幅图像
		image_jpg.display("My first CImg code");
		image_jpg.save_jpeg(output2_jpg_file);
		auto mem_out=image_jpg.save_mem_j2k();
		image_jpg.save_mem_jpeg([&](const uint8_t *img, unsigned long size) {
			std::ofstream ofs;
			ofs.open(output_jpg_file, std::ofstream::binary);
			ofs.write((const char*)img, size);
			printf("%s saved,size=%d\n", output_jpg_file, size);
			ofs.close();
		},
		100,
		[](j_common_ptr com_ptr) {
			//jpeg_set_colorspace((j_compress_ptr)com_ptr, JCS_GRAYSCALE);
		});

		auto j2k_data=gdface::load_binary_file(output4_jpg_file);
		image_jpg.load_mem_j2k(j2k_data.data(), j2k_data.size(), FS_JPEG2K_CODEC_JP2);
		image_jpg.draw_text(100,100,"jpeg2000 test",purple);
		image_jpg.display(output4_jpg_file);
		auto j2k_mat = jwp_load_j2k_mem(j2k_data.data(), j2k_data.size(), FS_JPEG2K_CODEC_JP2);
		cimg_library::image_matrix_display(j2k_mat, "test jwp_load_j2k_mem display", true);

	}catch (exception &e){
		cout<<e.what()<<endl;
	}
	//image_jpg.display("image_jpg");
    return 0;
}
