/*
 * jpeg_mem.h
 *
 *  Created on: 2016年1月19日
 *      Author: guyadong
 */

#ifndef FACEIMAGE_CIMGWRAPPER_JPEG_MEM_H_
#define FACEIMAGE_CIMGWRAPPER_JPEG_MEM_H_
#include <stdexcept>
#include <functional>
#include <string>
#include <vector>
#include <memory>
#include "jpeglib.h"
#include "image_matrix_types.h"

/* jpeg图像处理异常类 */
class jpeg_mem_exception :public std::logic_error {
public:
	// 继承基类构造函数
	using std::logic_error::logic_error;
};
/* 处理压缩解压缩后内存数据的回调函数 */
using mem_finish_output_fun=std::function<void(const uint8_t*,unsigned long)>;
/* 定制压缩解压缩参数 */
using jpeg_custom_output_fun=std::function<void(j_common_ptr)>;
extern jpeg_custom_output_fun jpeg_custom_output_default;
/* 图像压缩接口类 */
struct jpeg_compress_interface{
	// 图像缓冲区行指针
	std::vector<JSAMPROW> buffer;
	// 设置自定义输出参数的函数对象
	jpeg_custom_output_fun custom_output=[](j_common_ptr){};
	// 虚函数用于初始化缓冲区，填充压缩图像信息数据
	virtual void start_output(jpeg_compress_struct&cinfo) =0;
	// 虚函数用于从原始图像数据中获取行数据更新缓冲区buffer
	virtual void get_pixel_rows(JDIMENSION num_scanlines)=0;
	virtual ~jpeg_compress_interface()=default;
};
struct jpeg_compress_default:jpeg_compress_interface{
	const fs_image_matrix& matrix;
	uint32_t row_stride;
	// 当前处理的源图像像素行数
	JDIMENSION next_line;
	jpeg_compress_default(const fs_image_matrix& matrix);
	virtual void start_output(jpeg_compress_struct&cinfo);
	virtual void get_pixel_rows(JDIMENSION num_scanlines);
};
/* 图像解压缩接口类 */
struct jpeg_decompress_interface{
	// 图像缓冲区
	std::vector<JSAMPROW> buffer;
	// 设置自定义输出参数的函数对象
	jpeg_custom_output_fun custom_output=[](j_common_ptr){};
	// 虚函数用于初始化内存填充解压缩后图像信息数据
	virtual void start_output(const jpeg_decompress_struct&cinfo)=0;
	// 虚函数用于将解压缩后的数据写入图像内存区
	virtual void put_pixel_rows(JDIMENSION num_scanlines)=0;
	virtual ~jpeg_decompress_interface()=default;
};

/* 默认的图像解压缩接口实现 */
struct jpeg_decompress_default:public jpeg_decompress_interface{
	/* 解压缩后的图像基本信息 */
	fs_image_matrix matrix;
	uint32_t row_stride;
	// 当前处理的目标图像像素行数
	JDIMENSION next_line;
	jpeg_decompress_default(uint8_t align);
	jpeg_decompress_default();

	virtual void start_output(const jpeg_decompress_struct&dinfo);
	virtual void put_pixel_rows(JDIMENSION num_scanlines);
	virtual ~jpeg_decompress_default()=default;
};


void save_jpeg_mem(jpeg_compress_interface& compress_instance,
					const mem_finish_output_fun& finishe_output,
					const unsigned int quality = 100
					);

void save_jpeg_mem(const fs_image_matrix &matrix,
					const mem_finish_output_fun& finish_output,
					const unsigned int quality=100,
					const jpeg_custom_output_fun &custom=jpeg_custom_output_default
					);
std::string save_jpeg_mem_as_string(const fs_image_matrix &matrix, const unsigned int quality = 100);
std::vector<uint8_t> save_jpeg_mem_as_vector(const fs_image_matrix &matrix, const unsigned int quality = 100);

void save_jpeg_gray_mem(const fs_image_matrix &matrix,
						const mem_finish_output_fun& finish_output,
						const unsigned int quality=100
						);
std::string save_jpeg_gray_mem_as_string(const fs_image_matrix &matrix, const unsigned int quality = 100);
std::vector<uint8_t> save_jpeg_gray_mem_as_vector(const fs_image_matrix &matrix, const unsigned int quality = 100);

void load_jpeg_mem(const uint8_t *jpeg_data,size_t size,jpeg_decompress_interface &decompress_instance);
fs_image_matrix load_jpeg_mem(const uint8_t *jpeg_data,size_t size,
		const jpeg_custom_output_fun &custom=jpeg_custom_output_default);
fs_image_matrix load_jpeg_gray_mem(const uint8_t *jpeg_data,size_t size);
fs_image_matrix read_jpeg_header_file(const char *const filename);
fs_image_matrix read_jpeg_header_file(std::FILE *const file);
fs_image_matrix read_jpeg_header_mem(const uint8_t *jpeg_data,size_t size);
fs_image_matrix to_gray_image_matrix(const fs_image_matrix&matrix);

#endif /* FACEIMAGE_CIMGWRAPPER_JPEG_MEM_H_ */
