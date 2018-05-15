#include  <cstring>
#include <stdexcept>
#include "image_matrix_types.h"

uint32_t fs_get_row_stride(const fs_image_matrix_ptr matrix)
{
	if (nullptr == matrix) {
		return -1;
	}
	// align只取低4位
	uint32_t m = (1U << (matrix->align & 0x0f)) - 1;
	return uint32_t((matrix->width + m)&(~m));
}

uint32_t fs_get_matrix_size(const fs_image_matrix_ptr matrix)
{
	if (nullptr == matrix) {
		return -1;
	}
	auto stride = fs_get_row_stride(matrix);
	return stride * matrix->height * matrix->channels;
}

void fs_fill_channels(const fs_image_matrix_ptr matrix, uint8_t * dst_ptr)
{
	if (nullptr == matrix) {
		return ;
	}
	auto row_stride = fs_get_row_stride(matrix);
	if (nullptr == dst_ptr || nullptr == matrix->pixels) {
		return;
	}
	for (uint8_t ch = 0; ch < matrix->channels; ++ch, dst_ptr += matrix->width*matrix->height) {
		auto dst_offset = dst_ptr;
		auto src_ptr = matrix->pixels;
		for (unsigned int y = 0; y < matrix->height; ++y, src_ptr += row_stride*matrix->channels, dst_offset += matrix->width) {
			for (unsigned int x = 0; x<matrix->width; ++x) {
				dst_offset[x] = src_ptr[x*matrix->channels + ch];
			}
		}
	}
}

int fs_matrix_is_NULL(const fs_image_matrix_ptr matrix)
{
	if (nullptr == matrix) {
		return 1;
	}
	return 0 == matrix->width
		&& 0 == matrix->height
		&& 0 == matrix->channels;
}

int fs_make_matrix(fs_image_matrix_ptr matrix, uint32_t with, uint32_t height, uint8_t channels, FS_COLOR_SPACE color_space, uint8_t align, void * pixels)
{
	if (nullptr != matrix) {
		fs_destruct_matrix(matrix);
		matrix->width = with;
		matrix->height = height;
		matrix->channels = channels;
		matrix->color_space = color_space;
		matrix->align = align;
		matrix->shared = nullptr != pixels;
		matrix->pixels = nullptr;
		uint32_t size = fs_get_matrix_size(matrix);
		if (size > 0) {			
			matrix->pixels = (uint8_t*)(matrix->shared ? pixels : malloc(size));
			// 内存分配失败返回
			return nullptr != matrix->pixels;
		}
	}
	return 0;
}

fs_image_matrix_ptr fs_new_matrix(uint32_t width, uint32_t height, uint8_t channels, FS_COLOR_SPACE color_space, uint8_t align, void * pixels) {
	fs_image_matrix_ptr matrix = (fs_image_matrix_ptr)calloc(1, sizeof(fs_image_matrix));
	fs_make_matrix(matrix,width, height, channels, color_space, align, pixels);
	return matrix;
}

fs_image_matrix_ptr fs_new_matrix_s(uint32_t with, uint32_t height, FS_COLOR_SPACE color_space) {
	auto channels = fs_color_depth(color_space);
	if (channels > 0) {
		return fs_new_matrix(with, height, channels, color_space, 0, nullptr);
	}
	return nullptr;
}

void fs_free_matrix(fs_image_matrix_ptr matrix) {
	fs_destruct_matrix(matrix);
	free(matrix);
}

void fs_destruct_matrix(fs_image_matrix_ptr matrix)
{
	if (nullptr != matrix) {
		if (!matrix->shared) {
			free(matrix->pixels);
		}
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

inline _fs_image_matrix::_fs_image_matrix(uint32_t width, uint32_t height, uint8_t channels, FS_COLOR_SPACE color_space, uint8_t align, void * pixels) {
	fs_make_matrix(this, width, height, channels, color_space, align, pixels);
}

inline _fs_image_matrix::_fs_image_matrix(uint32_t width, uint32_t height, FS_COLOR_SPACE color_space) {
	fs_make_matrix(this, width, height, fs_color_depth(color_space), color_space, 0, nullptr);
}

inline _fs_image_matrix::_fs_image_matrix() :width(0), height(0), channels(0), color_space(FSC_UNKNOWN), align(0), shared(0), pixels(nullptr) {
}

_fs_image_matrix::_fs_image_matrix(const _fs_image_matrix & rv){
	auto b = fs_make_matrix(this, rv.width, rv.height, rv.channels, rv.color_space, rv.align, nullptr);
	if (b) {
		if (nullptr != pixels && nullptr != rv.pixels) {
			std::memcpy(pixels, rv.pixels, get_matrix_size());
		}
	}
	else {
		throw std::logic_error("copy constructor function error");
	}
}

inline _fs_image_matrix::_fs_image_matrix(_fs_image_matrix && rv) :width(rv.width), height(rv.height), channels(rv.channels), color_space(rv.color_space), align(rv.align), shared(rv.shared), pixels(rv.pixels) {
	// 原对象不执行释放动作
	rv.shared = 1;
}

inline _fs_image_matrix::~_fs_image_matrix() {
	fs_destruct_matrix(this);
}

inline uint32_t _fs_image_matrix::get_matrix_size() const{
	return fs_get_matrix_size((fs_image_matrix_ptr)this);
}

inline uint32_t _fs_image_matrix::get_row_stride()const {
	return fs_get_row_stride((fs_image_matrix_ptr)this);
}

inline bool _fs_image_matrix::is_NULL()const{
	auto b = fs_matrix_is_NULL((fs_image_matrix_ptr)this);
	return b ? true : false;
}

inline void _fs_image_matrix::fill_channels(uint8_t * dst_ptr) const{
	fs_fill_channels((fs_image_matrix_ptr)this, dst_ptr);
}
