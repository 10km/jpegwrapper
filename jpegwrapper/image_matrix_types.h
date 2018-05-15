#ifndef COMMON_SOURCE_CPP_IMAGE_MATRIX_TYPES_H
#define COMMON_SOURCE_CPP_IMAGE_MATRIX_TYPES_H
#include <stdint.h>
#include <stdlib.h>
// ɫ�ʶ��壬��jpeglib.h J_COLOR_SPACE����һ��
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
/* ͼ������������ */
typedef struct _fs_image_matrix {
	uint32_t		width;		// ͼ����
	uint32_t		height;		// ͼ��߶�
	uint8_t			channels;	// ͨ����
	FS_COLOR_SPACE  color_space;// ͼ�����ݵ�ɫ�ʿռ��� jpeglib.h ������ͬ
	uint8_t		    align;	    // �ڴ���뷽ʽ 0Ϊ�����룬>0Ϊ��2��n���ݶ���
	uint8_t         shared;     // Ϊtrueʱ,Ϊ�����ڴ棬����Ҫ�ͷ�
	uint8_t*        pixels;     // ͼ������
#ifdef __cplusplus
	_fs_image_matrix(uint32_t with, uint32_t height, uint8_t channels, FS_COLOR_SPACE  color_space, uint8_t align, void* pixels);
	_fs_image_matrix(uint32_t width, uint32_t height, FS_COLOR_SPACE  color_space);
	_fs_image_matrix();
	// ���ƹ��캯��
	_fs_image_matrix(const _fs_image_matrix&rv);
	// �ƶ����캯��
	_fs_image_matrix(_fs_image_matrix&&rv);
	~_fs_image_matrix();
	uint32_t get_matrix_size();
	uint32_t get_row_stride();
	bool is_NULL();
	void fill_channels(uint8_t *dst_ptr);
#endif // __cplusplus
}fs_image_matrix, *fs_image_matrix_ptr;
// fs_image_matrix�ն�����
static const fs_image_matrix FS_NULL_MATRIX ;

#ifdef __cplusplus
uint32_t fs_get_row_stride(const fs_image_matrix& matrix);
uint32_t fs_get_matrix_size(const fs_image_matrix& matrix);
void fs_fill_channels(const fs_image_matrix& matrix, uint8_t *dst_ptr);
// �ж�matrix�Ƿ�Ϊ��,width,height,channels�����ֶ�Ϊ0��Ϊ��
bool fs_matrix_is_NULL(const fs_image_matrix&matrix);
#endif // __cplusplus

/*
* ��ȡ�����ж�����(����)
* */
uint32_t fs_get_row_stride(const fs_image_matrix_ptr matrix);
// ����ͼ�������������ͼ�������ͼ��������ռ�ڴ��С(�ֽ�),�ý����pixels�Ƿ�ΪNULL�޹�,
// �����ݴ�ֵΪͼ������ڴ�
uint32_t fs_get_matrix_size(const fs_image_matrix_ptr matrix);
// ��fs_image_matrix�а����������洢��ͼ�����ݸ�Ϊ��ͨ���洢
void fs_fill_channels(const fs_image_matrix_ptr matrix, uint8_t *dst_ptr);
// �ж�matrix�Ƿ�Ϊ��
int fs_matrix_is_NULL(const fs_image_matrix_ptr matrix);
// ��ָ���Ĳ������һ��fs_image_matrix�ṹ��
// ����ֵ0ʧ�� 1�ɹ�
int fs_make_matrix(fs_image_matrix_ptr matrix,uint32_t with, uint32_t height, uint8_t channels, FS_COLOR_SPACE  color_space, uint8_t align, void* pixels);
// ����һ��fs_image_matrix�ṹ��
fs_image_matrix_ptr fs_new_matrix(uint32_t with, uint32_t height, uint8_t channels, FS_COLOR_SPACE  color_space, uint8_t align, void* pixels);
fs_image_matrix_ptr fs_new_matrix_s(uint32_t with, uint32_t height, FS_COLOR_SPACE  color_space);
// �ͷ�fs_image_matrix�����ͷ�matrixָ��
void fs_free_matrix(fs_image_matrix_ptr matrix);
// ����fs_image_matrix�����ͷŽṹ��ָ���ڴ�,���ͷ�matrixָ��
void fs_destruct_matrix(fs_image_matrix_ptr matrix);
uint8_t fs_color_depth(FS_COLOR_SPACE color_space);
#endif // !COMMON_SOURCE_CPP_IMAGE_MATRIX_TYPES_H

