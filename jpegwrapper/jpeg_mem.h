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
#include <memory>
#include <vector>
#include "jpeglib.h"
//#include "image_matrix_types.h"
typedef enum {
	FSC_UNKNOWN,            /* error/unspecified */
	FSC_GRAYSCALE,          /* monochrome */
	FSC_RGB,                /* red/green/blue as specified by the RGB_RED,
							RGB_GREEN, RGB_BLUE, and RGB_PIXELSIZE macros */
	FSC_YCbCr,              /* Y/Cb/Cr (also known as YUV) */
	FSC_CMYK,               /* C/M/Y/K */
	FSC_YCCK,               /* Y/Cb/Cr/K */
	FSC_EXT_RGB,            /* red/green/blue */
	FSC_EXT_RGBX,           /* red/green/blue/x */
	FSC_EXT_BGR,            /* blue/green/red */
	FSC_EXT_BGRX,           /* blue/green/red/x */
	FSC_EXT_XBGR,           /* x/blue/green/red */
	FSC_EXT_XRGB,           /* x/red/green/blue */
							/* When out_color_space it set to FSC_EXT_RGBX, FSC_EXT_BGRX, FSC_EXT_XBGR,
							or FSC_EXT_XRGB during decompression, the X byte is undefined, and in
							order to ensure the best performance, libjpeg-turbo can set that byte to
							whatever value it wishes.  Use the following colorspace constants to
							ensure that the X byte is set to 0xFF, so that it can be interpreted as an
							opaque alpha channel. */
	FSC_EXT_RGBA,           /* red/green/blue/alpha */
	FSC_EXT_BGRA,           /* blue/green/red/alpha */
	FSC_EXT_ABGR,           /* alpha/blue/green/red */
	FSC_EXT_ARGB,           /* alpha/red/green/blue */
	FSC_RGB565              /* 5-bit red/6-bit green/5-bit blue */
} FS_COLOR_SPACE;
/* 图像矩阵基本参数 */
typedef struct _fs_image_matrix{
        uint32_t		width;					// 图像宽度
        uint32_t		height;					// 图像高度
        uint8_t		channels;				// 通道数
		FS_COLOR_SPACE color_space; // 图像数据的色彩空间
        uint8_t		align;	// 内存对齐方式 0为不对齐，>0为以2的n次幂对齐
        std::vector <uint8_t> pixels; // 图像数据
}fs_image_matrix,*fs_image_matrix_ptr;
/*
 * 获取矩阵行对齐宽度(像素)
 * */
inline uint32_t fs_get_row_stride(const fs_image_matrix&matrix){
	// align只取低4位
	uint32_t m=(1U << (matrix.align&0x0f))-1;
	return uint32_t((matrix.width+m)&(~m));
}
// 将fs_image_matrix中按像素连续存储的图像数据改为按通道存储
inline void fs_fill_channels(const fs_image_matrix&img,uint8_t *dst_ptr) {
	auto row_stride = fs_get_row_stride(img);
	if (nullptr == dst_ptr) {
		return;
	}
	for (uint8_t ch = 0; ch < img.channels; ++ch, dst_ptr += img.width*img.height) {
		auto dst_offset = dst_ptr;
		auto src_ptr = img.pixels.data();
		for (unsigned int y = 0; y < img.height; ++y, src_ptr += row_stride*img.channels, dst_offset += img.width) {
			for (unsigned int x = 0; x<img.width; ++x) {
				dst_offset[x] = src_ptr[x*img.channels + ch];
			}
		}
	}
}
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
	jpeg_compress_default(const fs_image_matrix& matrix):matrix(matrix),next_line(0){
		row_stride=fs_get_row_stride(matrix);
	}
	virtual void start_output( jpeg_compress_struct&cinfo){
		cinfo.image_width = matrix.width;
		cinfo.image_height = matrix.height;
		cinfo.input_components = matrix.channels;
		cinfo.in_color_space = (J_COLOR_SPACE)matrix.color_space;
		// buffer只保存一行像素的源数据指针
		buffer=std::vector<JSAMPROW>(1);
		next_line=0;
	}
	virtual void get_pixel_rows(JDIMENSION num_scanlines){
		// buffer指向当前行像素数据的地址
		buffer[0]=const_cast<JSAMPROW>(matrix.pixels.data())+(next_line++)*row_stride*matrix.channels;
	}
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
	jpeg_decompress_default(uint8_t align):next_line(0),row_stride(0){
		matrix.align=align;
	}
	jpeg_decompress_default():jpeg_decompress_default(0){}

	virtual void start_output(const jpeg_decompress_struct&dinfo){
		// 填充图像基本信息结构
		matrix.width=dinfo.output_width;
		matrix.height=dinfo.output_height;
		matrix.color_space=(FS_COLOR_SPACE)dinfo.out_color_space;
		matrix.channels=dinfo.output_components;
		row_stride=fs_get_row_stride(matrix);
		// 分配像素数据存储区
		matrix.pixels=std::vector<uint8_t>(row_stride*matrix.height*matrix.channels);
		// buffer只保存一行像素的目标数据指针
		buffer=std::vector<JSAMPROW>(1);
		next_line=0;
		// 初始化buffer指向第一像素存储地址
		buffer[next_line]=matrix.pixels.data();
	}
	virtual void put_pixel_rows(JDIMENSION num_scanlines){
		// buffer指向下一行要像素存储地址
		buffer[0]=matrix.pixels.data()+(++next_line)*row_stride*matrix.channels;
	}
	virtual ~jpeg_decompress_default()=default;
};

/* jpeg图像处理异常类 */
class jpeg_mem_exception:public std::logic_error{
public:
	// 继承基类构造函数
	using std::logic_error::logic_error;
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
void save_jpeg_gray_mem(const fs_image_matrix &matrix,
									const mem_finish_output_fun& finish_output,
									const unsigned int quality=100
									);
void load_jpeg_mem(const uint8_t *jpeg_data,size_t size,
		 jpeg_decompress_interface &decompress_instance);
void load_jpeg_mem(const std::vector<uint8_t> &jpeg_data,
		 jpeg_decompress_interface &decompress_instance);
fs_image_matrix load_jpeg_mem(const uint8_t *jpeg_data,size_t size,
		const jpeg_custom_output_fun &custom=jpeg_custom_output_default);
fs_image_matrix load_jpeg_mem(const std::vector<uint8_t> &jpeg_data,
		const jpeg_custom_output_fun &custom=jpeg_custom_output_default);
fs_image_matrix load_jpeg_gray_mem(const uint8_t *jpeg_data,size_t size);
fs_image_matrix load_jpeg_gray_mem(const std::vector<uint8_t> &jpeg_data);
fs_image_matrix read_jpeg_header_file(const char *const filename);
fs_image_matrix read_jpeg_header_file(std::FILE *const file);
fs_image_matrix read_jpeg_header_mem(const uint8_t *jpeg_data,size_t size);
fs_image_matrix read_jpeg_header_mem(const std::vector<uint8_t> &jpeg_data);
fs_image_matrix to_gray_image_matrix(const fs_image_matrix&matrix);

#endif /* FACEIMAGE_CIMGWRAPPER_JPEG_MEM_H_ */
