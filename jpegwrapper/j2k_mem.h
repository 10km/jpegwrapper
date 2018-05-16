/*
 * j2k_mem.h
 *
 *  Created on: 2016年1月25日
 *      Author: guyadong
 */

#ifndef FACEIMAGE_CIMGWRAPPER_J2K_MEM_H_
#define FACEIMAGE_CIMGWRAPPER_J2K_MEM_H_
#include <vector>
#include <string>
#include "image_matrix_types.h"
/* 从j2k_data和size指定的内存数据中解码指定格式(format)的jpeg2000图像
* 返回 fs_image_matrix对象,出错则抛出异常
* 返回解码后的图像矩阵
*/
fs_image_matrix jwp_load_j2k_mem(const uint8_t* j2k_data, size_t size, JWP_OPJ_CODEC_FORMAT format);
// 将图像矩阵保存为format指定的jpeg2000格式，返回压缩后的数据
std::string jwp_save_j2k_mem_as_string(const fs_image_matrix& matrix, const unsigned int quality = 100, JWP_OPJ_CODEC_FORMAT format = JWP_OPJ_CODEC_JP2);
// 将图像矩阵保存为format指定的jpeg2000格式，返回压缩后的数据
std::vector<uint8_t> jwp_save_j2k_mem_as_vector(const fs_image_matrix& matrix, const unsigned int quality = 100, JWP_OPJ_CODEC_FORMAT format = JWP_OPJ_CODEC_JP2);

#endif /* FACEIMAGE_CIMGWRAPPER_J2K_MEM_H_ */
