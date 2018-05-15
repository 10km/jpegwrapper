/*
 * CImgWrapper.h
 *
 *  Created on: 2016年1月18日
 *      Author: guyadong
 */

#ifndef FACEIMAGE_CIMGWRAPPER_CIMGWRAPPER_H_
#define FACEIMAGE_CIMGWRAPPER_CIMGWRAPPER_H_
// 为CImg加入jpeg图像格式支持
#ifndef cimg_use_jpeg
#define cimg_use_jpeg
#endif
#include <vector>
#include <functional>
#include <memory>
#ifdef _MSC_VER
// 关闭编译CImg.h时产生的警告
#pragma  warning( push ) 
#pragma  warning( disable: 4267 4319 )
#endif
#include "CImg.h"
#ifdef _MSC_VER
#pragma  warning(  pop  ) 
#endif
// 去掉max,min宏定义，否则会影响类似std::numeric_limits<size_t>::max()这样的函数调用
#if defined( _cimg_redefine_min)
#undef min
#pragma message( "undefined 'min' in CImgWrapper.h")
#endif
#if defined( _cimg_redefine_max)
#undef max
#pragma message("undefined 'max' in CImgWrapper.h")
#endif


#include "jpeglib.h"
#include "jpeg_mem.h"
#include "j2k_mem.h"
#include "assert_macros.h"
#ifdef __GNUC__
// 关闭 using  _Base::_Base; 这行代码产生的警告
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winherited-variadic-ctor"
#endif
namespace cimg_library {
template<typename T>
class CImgWrapper:public CImg<T> {
public:
	using   _Base =CImg<T>;
	using  _Base::_Base; // 继承基类构造函数

	virtual ~CImgWrapper()=default;
	const CImgWrapper<T>& save_mem_jpeg(
				const mem_finish_output_fun& finishe_output,
				const unsigned int quality = 100,
				const jpeg_custom_output_fun &custom= jpeg_custom_output_default) const {
		struct  jpeg_compress_cimg:public jpeg_compress_interface {
			// 行缓冲区
			CImg<typename CImg<T>::ucharT> line_buffer;
			const CImgWrapper<T>& cimg_obj;
			jpeg_compress_cimg(const CImgWrapper<T>& cimg_obj):cimg_obj(cimg_obj){}
			virtual void start_output(jpeg_compress_struct&cinfo) {
				uint8_t dimbuf = 0;
				J_COLOR_SPACE colortype = JCS_RGB;
				switch (cimg_obj._spectrum) {
				case 1:
					dimbuf = 1;
					colortype = JCS_GRAYSCALE;
					break;
				case 2:
					dimbuf = 3;
					colortype = JCS_RGB;
					break;
				case 3:
					dimbuf = 3;
					colortype = JCS_RGB;
					break;
				default:
					dimbuf = 4;
					colortype = JCS_CMYK;
					break;
				}
				cinfo.image_width = cimg_obj._width;
				cinfo.image_height = cimg_obj._height;
				cinfo.input_components = dimbuf;
				cinfo.in_color_space = colortype;
				line_buffer=CImg<typename CImg<T>::ucharT>(cinfo.image_width*cinfo.input_components);
				buffer=std::vector<JSAMPROW>(1);
				buffer[0] =(JSAMPROW) line_buffer._data;
			}
			virtual void get_pixel_rows(JDIMENSION line){
				unsigned char* ptrd = line_buffer._data;
				// Fill pixel buffer
				switch (cimg_obj._spectrum) {
					case 1: {
						// Greyscale images
						const T* ptr_g =cimg_obj.data(0, line);
						for (unsigned int b = 0; b < cimg_obj._width; b++)
							*(ptrd++) = (unsigned char) ((*(ptr_g++)));
					}
						break;
					case 2: {
						// RG images
						const T *ptr_r = cimg_obj.data(0, line, 0, 0),
								*ptr_g = cimg_obj.data(0, line, 0, 1);
						for (unsigned int b = 0; b < cimg_obj._width; ++b) {
							*(ptrd++) = (unsigned char) ((*(ptr_r++)));
							*(ptrd++) = (unsigned char) ((*(ptr_g++)));
							*(ptrd++) = 0;
						}
					}
						break;
					case 3: {
						// RGB images
						const T *ptr_r = cimg_obj.data(0, line, 0, 0),
								*ptr_g = cimg_obj.data(0, line, 0, 1),
								*ptr_b = cimg_obj.data(0, line, 0, 2);
						for (unsigned int b = 0; b < cimg_obj._width; ++b) {
							*(ptrd++) = (unsigned char) ((*(ptr_r++)));
							*(ptrd++) = (unsigned char) ((*(ptr_g++)));
							*(ptrd++) = (unsigned char) ((*(ptr_b++)));
						}
					}
						break;
					default: {
						// CMYK images
						const T *ptr_r = cimg_obj.data(0, line, 0, 0),
								*ptr_g = cimg_obj.data(0, line, 0, 1),
								*ptr_b = cimg_obj.data(0, line, 0, 2),
								*ptr_a = cimg_obj.data(0, line, 0, 3);
						for (unsigned int b = 0; b < cimg_obj._width; ++b) {
							*(ptrd++) = (unsigned char) ((*(ptr_r++)));
							*(ptrd++) = (unsigned char) ((*(ptr_g++)));
							*(ptrd++) = (unsigned char) ((*(ptr_b++)));
							*(ptrd++) = (unsigned char) ((*(ptr_a++)));
						}
					}
				}
			}
		} compress_cimg_instance(*this);
		compress_cimg_instance.custom_output=std::move(custom);

		save_jpeg_mem(compress_cimg_instance,finishe_output,quality);
		return *this;
	}

	const CImgWrapper<T>& load_mem_jpeg(
			uint8_t *jpeg_data,
			size_t size,
			const jpeg_custom_output_fun &custom= jpeg_custom_output_default){
		// 实现jpeg_decompress_interface 接口
		struct  jpeg_decompress_cimg:public jpeg_decompress_interface {
			// 行缓冲区
			CImg<typename CImg<T>::ucharT> line_buffer;
			// 颜色通道指针
			T *ptr_r=nullptr , *ptr_g=nullptr , *ptr_b=nullptr , *ptr_a=nullptr;
			CImgWrapper<T>& cimg_obj;
			jpeg_decompress_cimg(CImgWrapper<T>& cimg_obj):cimg_obj(cimg_obj){}
			virtual void start_output(const jpeg_decompress_struct&cinfo) {
				line_buffer=CImg<typename CImg<T>::ucharT>(cinfo.output_width*cinfo.output_components);
				cimg_obj.assign(cinfo.output_width,cinfo.output_height,1,cinfo.output_components);
			    ptr_r  = cimg_obj._data,
			    ptr_g = cimg_obj._data + 1UL*cimg_obj._width*cimg_obj._height,
				ptr_b = cimg_obj._data + 2UL*cimg_obj._width*cimg_obj._height,
			    ptr_a = cimg_obj._data + 3UL*cimg_obj._width*cimg_obj._height;
				buffer=std::vector<JSAMPROW>(1);
				buffer[0] =(JSAMPROW) line_buffer._data;
			}
			virtual void put_pixel_rows(JDIMENSION num_scanlines) {
		        const unsigned char *ptrs = line_buffer._data;
		        switch (cimg_obj._spectrum) {
		        case 1 : {
		        	cimg_forX(cimg_obj,x) *(ptr_r++) = (T)*(ptrs++);
		        } break;
		        case 3 : {
		          cimg_forX(cimg_obj,x) {
		            *(ptr_r++) = (T)*(ptrs++);
		            *(ptr_g++) = (T)*(ptrs++);
		            *(ptr_b++) = (T)*(ptrs++);
		          }
		        } break;
		        case 4 : {
		          cimg_forX(cimg_obj,x) {
		            *(ptr_r++) = (T)*(ptrs++);
		            *(ptr_g++) = (T)*(ptrs++);
		            *(ptr_b++) = (T)*(ptrs++);
		            *(ptr_a++) = (T)*(ptrs++);
		          }
		        } break;
		        }
			}
		}jpeg_decompress_cimg_instance(*this);
		jpeg_decompress_cimg_instance.custom_output=custom;
		load_jpeg_mem(jpeg_data,size,jpeg_decompress_cimg_instance);
		return *this;
	}

	const CImgWrapper<T>& load_mem_jpeg(
			std::vector<uint8_t> jpeg_data,
			const jpeg_custom_output_fun &custom= jpeg_custom_output_default){
		return load_mem_jpeg(jpeg_data.data(),jpeg_data.size(),custom);
	}
	const CImgWrapper<T>& load_mem_jpeg_gray(uint8_t *jpeg_data,size_t size){
		static auto custom_output_gray=[](j_common_ptr cinfo) {
			((j_decompress_ptr)cinfo)->out_color_space = JCS_GRAYSCALE;
		};
		return load_mem_jpeg(jpeg_data,size,custom_output_gray);
	}
	const CImgWrapper<T>& load_mem_jpeg_gray(std::vector<uint8_t> jpeg_data){
		return load_mem_jpeg_gray(jpeg_data.data(),jpeg_data.size());
	}
	const CImgWrapper<T>& load_mem_j2k(const uint8_t* jpeg_data, size_t size, OPJ_CODEC_FORMAT format){
		throw_if_null(jpeg_data)
		throw_if_msg(0 == size, "jpeg_data is empty")
		opj_stream_mem_input src(jpeg_data, size);
		gdface::raii_var<opj_image_t*> raii_image([&]() {
			return load_j2k(src, format);
		}, [](opj_image_t* image) {
			/* free image data */
			opj_image_destroy(image);
		});
		// 检查参数合法性
		if (raii_image->numcomps > 1)
			for (auto i = raii_image->numcomps - 1; i > 0; --i) {
				throw_except_if_msg(opj_exception,this->_width != raii_image->comps[i].w || this->_height != raii_image->comps[i].h||raii_image->comps[i].prec>8||raii_image->comps[i].bpp>8,
						"each components has different size");
			}
		// 输出通道数必须是1/3/4
		throw_except_if_msg(opj_exception,raii_image->numcomps!=1 && raii_image->numcomps!=3 && raii_image->numcomps!=4,
			"load_mem_j2k():Failed to load JPEG2000 data cause by numcomps error(must be 1/3/4)");
		this->assign(raii_image->comps[0].w,raii_image->comps[0].h,1,raii_image->numcomps);
		for(uint8_t ch=0;ch<this->_spectrum ;++ch){
			T *ptr=this->_data+ ch*this->_width*this->_height;
			for(unsigned int i=0;i<this->_width*this->_height;++i){
				ptr[i]=(T)raii_image->comps[ch].data[i];
			}
		}
		return *this;
	}

	const opj_stream_mem_output save_mem_j2k(
					const unsigned int quality = 100,
					OPJ_CODEC_FORMAT format=OPJ_CODEC_JP2
					) const {
		auto parameters = std::make_shared<opj_cparameters_t>();
		/* set encoding parameters to default values */
		opj_set_default_encoder_parameters(parameters.get());
		parameters->cod_format=format;
		parameters->tcp_numlayers=1;
		parameters->tcp_distoratio[0]=(float)(quality>100?100:quality);
		parameters->cp_fixed_quality=1;
		gdface::raii_var<opj_image_t*> raii_image([&]() {
			return create_opj_image(*parameters);
		}, [](opj_image_t*image) {
			/* free image data */
			opj_image_destroy (image);
		});
		opj_stream_mem_output out;
		save_j2k(*raii_image,parameters.get(),out);
		return std::move(out);
	}
	const CImgWrapper<T>& save_mem_j2k(
					const mem_finish_output_fun& finishe_output,
					const unsigned int quality = 100,
					OPJ_CODEC_FORMAT format=OPJ_CODEC_J2K
					) const {
		auto out=save_mem_j2k(quality,format);
		finishe_output(out.stream_data(),out.stream_length());
		return *this;
	}

private:
	/* 创建 opj_image_t
	 * 失败则抛出 opj_exception 异常
	 */
	opj_image_t* create_opj_image(opj_cparameters_t &parameters) const{

		uint8_t dimbuf = 0;
		J_COLOR_SPACE colortype = JCS_RGB;
		switch (this->_spectrum) {
		case 1:
			dimbuf = 1;
			colortype = JCS_GRAYSCALE;
			break;
		case 2:
			dimbuf = 3;
			colortype = JCS_RGB;
			break;
		case 3:
			dimbuf = 3;
			colortype = JCS_RGB;
			break;
		default:
			dimbuf = 4;
			colortype = JCS_CMYK;
			break;
		}

		auto subsampling_dx = parameters.subsampling_dx;
		auto subsampling_dy = parameters.subsampling_dy;
		auto color_space = jpeglib_to_opj_color_space(colortype);
		opj_image_cmptparm_t cmptparm[4]; /* maximum of 4 components */
		opj_image_t * image = nullptr;
		/* initialize image components */
		memset(cmptparm, 0, dimbuf * sizeof(opj_image_cmptparm_t));
		for (auto i = dimbuf; i >0; --i) {
			cmptparm[i-1].prec = 8;
			cmptparm[i-1].bpp = 8;
			cmptparm[i-1].sgnd = 0;
			cmptparm[i-1].dx = (OPJ_UINT32) (subsampling_dx);
			cmptparm[i-1].dy = (OPJ_UINT32) (subsampling_dy);
			cmptparm[i-1].w = (OPJ_UINT32) (this->_width);
			cmptparm[i-1].h = (OPJ_UINT32) (this->_height);
		}
		/* create the image */
		if (nullptr == (image = opj_image_create((OPJ_UINT32) (dimbuf), cmptparm, color_space)))
			throw opj_exception("failed to create image: opj_image_create\n");
		/* set image offset and reference grid */
		image->x0 = (OPJ_UINT32) (parameters.image_offset_x0);
		image->y0 = (OPJ_UINT32) (parameters.image_offset_y0);
		image->x1 = image->x0 + (OPJ_UINT32) ((this->_width - 1)) * (OPJ_UINT32) (subsampling_dx) + 1;
		image->y1 = image->y0 + (OPJ_UINT32) ((this->_height - 1)) * (OPJ_UINT32) (subsampling_dy) + 1;
		// 将CImg中的通道数据依照opj_image_t的格式拆开到不同的comps中
		for(uint8_t ch=0;ch<dimbuf ;++ch){
			auto ptr=this->_data+ ch*this->_width*this->_height;
			for(unsigned int i=0;i<this->_width*this->_height;++i){
				image->comps[ch].data[i]=(OPJ_INT32)ptr[i];
			}
		}
		return image;
	}

};
// fs_image_matrix转换为CImgWrapper
CImgWrapper<uint8_t> toCImg(const fs_image_matrix&img) {	
	std::vector<uint8_t> tmp(img.width*img.height*img.channels);
	img.fill_channels(tmp.data());
	return CImgWrapper<uint8_t>(tmp.data(), img.width, img.height, 1, img.channels, false);
	}
// 显示一个fs_image_matrix对象代表的图像
void image_matrix_display(const fs_image_matrix&img, const std::string& title, const bool display_info, unsigned int *const XYZ = 0,
	const bool exit_on_anykey = false) {
	toCImg(img).display(title.empty() ? nullptr : title.data(), display_info, XYZ, exit_on_anykey);
}
} /* namespace cimg_library */
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#endif /* FACEIMAGE_CIMGWRAPPER_CIMGWRAPPER_H_ */
