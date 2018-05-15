#ifndef COMMON_SOURCE_CPP_IMAGE_MATRIX_TYPES_H
#define COMMON_SOURCE_CPP_IMAGE_MATRIX_TYPES_H
#include <stdint.h>
#include <stdlib.h>
// 色彩定义，与jpeglib.h J_COLOR_SPACE定义一致
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
typedef struct _fs_image_matrix {
	uint32_t		width;		// 图像宽度
	uint32_t		height;		// 图像高度
	uint8_t			channels;	// 通道数
	FS_COLOR_SPACE  color_space;// 图像数据的色彩空间与 jpeglib.h 定义相同
	uint8_t		    align;	    // 内存对齐方式 0为不对齐，>0为以2的n次幂对齐
	uint8_t         shared;     // 为true时,为共享内存，不需要释放
	uint8_t*        pixels;     // 图像数据
#ifdef __cplusplus
	_fs_image_matrix(uint32_t with, uint32_t height, uint8_t channels, FS_COLOR_SPACE  color_space, uint8_t align, void* pixels);
	_fs_image_matrix(uint32_t width, uint32_t height, FS_COLOR_SPACE  color_space);
	_fs_image_matrix();
	// 复制构造函数
	_fs_image_matrix(const _fs_image_matrix&rv);
	// 移动构造函数
	_fs_image_matrix(_fs_image_matrix&&rv);
	~_fs_image_matrix();
	uint32_t get_matrix_size();
	uint32_t get_row_stride();
	bool is_NULL();
	void fill_channels(uint8_t *dst_ptr);
#endif // __cplusplus
}fs_image_matrix, *fs_image_matrix_ptr;
// fs_image_matrix空对象常量
static const fs_image_matrix FS_NULL_MATRIX ;

#ifdef __cplusplus
uint32_t fs_get_row_stride(const fs_image_matrix& matrix);
uint32_t fs_get_matrix_size(const fs_image_matrix& matrix);
void fs_fill_channels(const fs_image_matrix& matrix, uint8_t *dst_ptr);
// 判断matrix是否为空,width,height,channels三个字段为0即为空
bool fs_matrix_is_NULL(const fs_image_matrix&matrix);
#endif // __cplusplus

/*
* 获取矩阵行对齐宽度(像素)
* */
uint32_t fs_get_row_stride(const fs_image_matrix_ptr matrix);
// 根据图像基本参数计算图像矩阵中图像数据所占内存大小(字节),该结果与pixels是否为NULL无关,
// 可依据此值为图像分配内存
uint32_t fs_get_matrix_size(const fs_image_matrix_ptr matrix);
// 将fs_image_matrix中按像素连续存储的图像数据改为按通道存储
void fs_fill_channels(const fs_image_matrix_ptr matrix, uint8_t *dst_ptr);
// 判断matrix是否为空
int fs_matrix_is_NULL(const fs_image_matrix_ptr matrix);
// 用指定的参数填充一个fs_image_matrix结构体
// 返回值0失败 1成功
int fs_make_matrix(fs_image_matrix_ptr matrix,uint32_t with, uint32_t height, uint8_t channels, FS_COLOR_SPACE  color_space, uint8_t align, void* pixels);
// 创建一个fs_image_matrix结构体
fs_image_matrix_ptr fs_new_matrix(uint32_t with, uint32_t height, uint8_t channels, FS_COLOR_SPACE  color_space, uint8_t align, void* pixels);
fs_image_matrix_ptr fs_new_matrix_s(uint32_t with, uint32_t height, FS_COLOR_SPACE  color_space);
// 释放fs_image_matrix对象，释放matrix指针
void fs_free_matrix(fs_image_matrix_ptr matrix);
// 析构fs_image_matrix对象，释放结构内指针内存,不释放matrix指针
void fs_destruct_matrix(fs_image_matrix_ptr matrix);
uint8_t fs_color_depth(FS_COLOR_SPACE color_space);
#endif // !COMMON_SOURCE_CPP_IMAGE_MATRIX_TYPES_H

