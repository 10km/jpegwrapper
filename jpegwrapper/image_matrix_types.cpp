#include "image_matrix_types.h"

/*
* ��ȡ�����ж�����(����)
* */
uint32_t fs_get_row_stride(const fs_image_matrix& matrix) {
	// alignֻȡ��4λ
	uint32_t m = (1U << (matrix.align & 0x0f)) - 1;
	return uint32_t((matrix.width + m)&(~m));
}

// ����ͼ�������������ͼ�������ͼ��������ռ�ڴ��С(�ֽ�),�ý����pixels�Ƿ�ΪNULL�޹�,
// �����ݴ�ֵΪͼ������ڴ�
uint32_t fs_get_matrix_size(const fs_image_matrix& matrix) {
	auto stride = fs_get_row_stride(matrix);
	return stride * matrix.height * matrix.channels;
}

// ��fs_image_matrix�а����������洢��ͼ�����ݸ�Ϊ��ͨ���洢
void fs_fill_channels(const fs_image_matrix& matrix, uint8_t * dst_ptr) {
	auto row_stride = fs_get_row_stride(matrix);
	if (nullptr == dst_ptr || nullptr == matrix.pixels) {
		return;
	}
	for (uint8_t ch = 0; ch < matrix.channels; ++ch, dst_ptr += matrix.width*matrix.height) {
		auto dst_offset = dst_ptr;
		auto src_ptr = matrix.pixels;
		for (unsigned int y = 0; y < matrix.height; ++y, src_ptr += row_stride*matrix.channels, dst_offset += matrix.width) {
			for (unsigned int x = 0; x<matrix.width; ++x) {
				dst_offset[x] = src_ptr[x*matrix.channels + ch];
			}
		}
	}
}

bool fs_matrix_is_NULL(const fs_image_matrix & matrix)
{
	return 0 == matrix.width 
		&& 0 == matrix.height 
		&& 0 == matrix.channels;
}

uint32_t fs_get_row_stride(const fs_image_matrix * matrix)
{
	if (nullptr == matrix) {
		return -1;
	}
	return fs_get_row_stride(*matrix);
}

uint32_t fs_get_matrix_size(const fs_image_matrix * matrix)
{
	if (nullptr == matrix) {
		return -1;
	}
	return fs_get_matrix_size(*matrix);
}

void fs_fill_channels(const fs_image_matrix * matrix, uint8_t * dst_ptr)
{
	if (nullptr == matrix) {
		return ;
	}
	fs_fill_channels(*matrix, dst_ptr);
}

int fs_matrix_is_NULL(const fs_image_matrix * matrix)
{
	if (nullptr == matrix) {
		return 1;
	}
	return fs_matrix_is_NULL(*matrix);
}

void fs_make_matrix(fs_image_matrix_ptr m_ptr, uint32_t with, uint32_t height, uint8_t channels, FS_COLOR_SPACE color_space, uint8_t align, void * pixels)
{
	if (nullptr != m_ptr) {
		m_ptr->width = with;
		m_ptr->height = height;
		m_ptr->channels = channels;
		m_ptr->color_space = color_space;
		m_ptr->align = align;
		uint32_t size = fs_get_matrix_size(m_ptr);
		m_ptr->shared = nullptr != pixels;
		m_ptr->pixels = (uint8_t*)(m_ptr->shared ? pixels : malloc(size));
	}
}

// ����һ��fs_image_matrix�ṹ��
fs_image_matrix_ptr fs_new_matrix(uint32_t width, uint32_t height, uint8_t channels, FS_COLOR_SPACE color_space, uint8_t align, void * pixels) {
	fs_image_matrix_ptr m_ptr = (fs_image_matrix_ptr)calloc(1, sizeof(fs_image_matrix));
	fs_make_matrix(m_ptr,width, height, channels, color_space, align, pixels);
	return m_ptr;
}

fs_image_matrix_ptr fs_new_matrix_s(uint32_t with, uint32_t height, FS_COLOR_SPACE color_space) {
	auto channels = fs_color_depth(color_space);
	if (channels > 0) {
		return fs_new_matrix(with, height, channels, color_space, 0, nullptr);
	}
	return nullptr;
}

void fs_free_matrix(fs_image_matrix_ptr matrix) {
	if (nullptr != matrix) {
		if(! matrix->shared){
			free(matrix->pixels);
		}
		free(matrix);
	}
}

uint8_t fs_color_depth(FS_COLOR_SPACE color_space) {
	switch (color_space) {
	case FSC_GRAYSCALE:
		return 1;
	case FSC_RGB565:
		return 2;
	case FSC_EXT_RGB:
	case FSC_YCbCr:
	case FSC_RGB:
		return 3;
		break;
	case FSC_EXT_RGBA:
	case FSC_EXT_RGBX:
	case FSC_EXT_ARGB:
	case FSC_EXT_XRGB:
	case FSC_EXT_BGRA:
	case FSC_EXT_BGRX:
	case FSC_EXT_ABGR:
	case FSC_EXT_XBGR:
	case FSC_YCCK:
	case FSC_CMYK:
		return 4;
	default:
		return -1;
	}
}

_fs_image_matrix::_fs_image_matrix(uint32_t with, uint32_t height, uint8_t channels, FS_COLOR_SPACE color_space, uint8_t align, void * pixels) {
	fs_make_matrix(this, width, height, channels, color_space, align, pixels);
}

_fs_image_matrix::_fs_image_matrix(uint32_t width, uint32_t height, FS_COLOR_SPACE color_space) {
	fs_make_matrix(this, width, height, fs_color_depth(color_space), color_space, 0, nullptr);
}

_fs_image_matrix::_fs_image_matrix() :width(0), height(0), channels(0), color_space(FSC_UNKNOWN), align(0), shared(0), pixels(nullptr) {
}

_fs_image_matrix::~_fs_image_matrix() {
	if (!shared) {
		free(pixels);
	}
}

uint32_t _fs_image_matrix::get_matrix_size() {
	return fs_get_matrix_size(this);
}

uint32_t _fs_image_matrix::get_row_stride() {
	return fs_get_row_stride(this);
}

bool _fs_image_matrix::is_NULL() {
	auto b = fs_matrix_is_NULL(this);
	return b ? true : false;
}

void _fs_image_matrix::fill_channels(uint8_t * dst_ptr) {
	fs_fill_channels(this, dst_ptr);
}
