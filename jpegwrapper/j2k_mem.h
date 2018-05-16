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
 /**
 * 编译格式定义,与openjpeg.h定义一致
 */
typedef enum {
	JWP_OPJ_CODEC_UNKNOWN = -1,	/**< place-holder */
	JWP_OPJ_CODEC_J2K = 0,		/**< JPEG-2000 codestream : read/write */
	JWP_OPJ_CODEC_JPT = 1,		/**< JPT-stream (JPEG 2000, JPIP) : read only */
	JWP_OPJ_CODEC_JP2 = 2,		/**< JP2 file format : read/write */
	JWP_OPJ_CODEC_JPP = 3,		/**< JPP-stream (JPEG 2000, JPIP) : to be coded */
	JWP_OPJ_CODEC_JPX = 4		/**< JPX file format (JPEG 2000 Part-2) : to be coded */
} JWP_OPJ_CODEC_FORMAT;

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
