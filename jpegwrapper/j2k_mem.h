/*
 * j2k_mem.h
 *
 *  Created on: 2016年1月25日
 *      Author: guyadong
 */

#ifndef FACEIMAGE_CIMGWRAPPER_J2K_MEM_H_
#define FACEIMAGE_CIMGWRAPPER_J2K_MEM_H_
#include <vector>
#include <stdexcept>
#include <string>
#include "image_matrix_types.h"

 /* openjpeg编码解码异常类 */
class opj_exception :public std::logic_error {
public:
	// 继承基类构造函数
	using std::logic_error::logic_error;
};
/* stream异常类 */
class opj_stream_exception :public std::logic_error {
public:
	// 继承基类构造函数
	using std::logic_error::logic_error;
};
 /**
 * JPEG2000格式定义,与openjpeg.h定义一致
 */
typedef enum {
	FS_JPEG2K_CODEC_UNKNOWN = -1,	/**< place-holder */
	FS_JPEG2K_CODEC_J2K = 0,		/**< JPEG-2000 codestream : read/write */
	FS_JPEG2K_CODEC_JPT = 1,		/**< JPT-stream (JPEG 2000, JPIP) : read only */
	FS_JPEG2K_CODEC_JP2 = 2,		/**< JP2 file format : read/write */
	FS_JPEG2K_CODEC_JPP = 3,		/**< JPP-stream (JPEG 2000, JPIP) : to be coded */
	FS_JPEG2K_CODEC_JPX = 4		/**< JPX file format (JPEG 2000 Part-2) : to be coded */
} FS_JPEG2K_CODEC_FORMAT;
/* 从j2k_data和size指定的内存数据中解码指定格式(format)的jpeg2000图像
* 返回 fs_image_matrix对象,出错则抛出异常
* 返回解码后的图像矩阵
*/
fs_image_matrix jwp_load_j2k_mem(const void* j2k_data, size_t size, FS_JPEG2K_CODEC_FORMAT format);
// 将图像矩阵保存为format指定的jpeg2000格式，返回压缩后的数据
std::string jwp_save_j2k_mem_as_string(const fs_image_matrix& matrix, const unsigned int quality = 100, FS_JPEG2K_CODEC_FORMAT format = FS_JPEG2K_CODEC_JP2);
// 将图像矩阵保存为format指定的jpeg2000格式，返回压缩后的数据
std::vector<uint8_t> jwp_save_j2k_mem_as_vector(const fs_image_matrix& matrix, const unsigned int quality = 100, FS_JPEG2K_CODEC_FORMAT format = FS_JPEG2K_CODEC_JP2);

#endif /* FACEIMAGE_CIMGWRAPPER_J2K_MEM_H_ */
