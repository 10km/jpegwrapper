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
#include <cstdio>
#include <string>
#include <vector>
#include <memory>
#include "image_matrix_types.h"
 /* jpeg图像处理异常类 */
class jpeg_mem_exception :public std::logic_error {
public:
	// 继承基类构造函数
	using std::logic_error::logic_error;
};
std::string jwp_save_jpeg_mem_as_string(const fs_image_matrix &matrix, const unsigned int quality = 100);
std::vector<uint8_t> jwp_save_jpeg_mem_as_vector(const fs_image_matrix &matrix, const unsigned int quality = 100);

std::string jwp_save_jpeg_gray_mem_as_string(const fs_image_matrix &matrix, const unsigned int quality = 100);
std::vector<uint8_t> jwp_save_jpeg_gray_mem_as_vector(const fs_image_matrix &matrix, const unsigned int quality = 100);

// 对jpeg格式的图像数据(jpeg_data)解码,返回图像矩阵
fs_image_matrix jwp_load_jpeg_mem(const void *jpeg_data, size_t size);
// 对jpeg格式的图像数据(jpeg_data)解码并转为灰度图像,返回灰度图像矩阵
fs_image_matrix jwp_load_jpeg_gray_mem(const void *jpeg_data,size_t size);
// 读取jpeg格式头信息
fs_image_matrix jwp_read_jpeg_header_file(const char *const filename);
fs_image_matrix jwp_read_jpeg_header_file(std::FILE *const file);
fs_image_matrix jwp_read_jpeg_header_mem(const void *jpeg_data,size_t size);
#endif /* FACEIMAGE_CIMGWRAPPER_JPEG_MEM_H_ */
