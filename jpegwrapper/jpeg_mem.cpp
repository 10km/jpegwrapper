/*
 * jpeg_mem.cpp
 *
 *  Created on: 2016年1月19日
 *      Author: guyadong
 */
#include <algorithm>
#include <type_traits>
#include <iostream>
#include <cstring>
#include "jpeg_mem.h"
#include "raii.h"
jpeg_custom_output_fun jpeg_custom_output_default=[](j_common_ptr){};
/* 自定义jpeg图像压缩/解压缩过程中错误退出函数 */
METHODDEF(void) jpeg_mem_error_exit (j_common_ptr cinfo) {
	// 调用 format_message 生成错误信息
	char err_msg[JMSG_LENGTH_MAX];
	(*cinfo->err->format_message) (cinfo,err_msg);
	// 抛出c++异常
	throw jpeg_mem_exception(err_msg);
}
/* 将图像数据输出为jpeg格式的内存数据块,调用传入的finishe_output回调函数来处理压缩后的内存图像数据
 * 图像信息及获取图像行数据的方式都由compress_instance定义
 * custom用于设置图像输出参数
 * 出错抛出 jpeg_mem_exception
 */
void save_jpeg_mem(jpeg_compress_interface& compress_instance,
									const mem_finish_output_fun& finish_output,
									const unsigned int quality
									){
	// 输出图像数据缓冲区
	unsigned char* outBuffer = nullptr;
	// 输出图像数据缓冲区长度(压缩后的图像大小)
	unsigned long bufferSize = 0;
	bool jpeg_compress_finished=false;
	// 定义一个压缩对象
	jpeg_compress_struct cinfo;
	//用于错误信息
	jpeg_error_mgr jerr;
	// 错误输出绑定到压缩对象
	cinfo.err = jpeg_std_error(&jerr);
	// 设置自定义的错误处理函数
	jerr.error_exit = jpeg_mem_error_exit;
	// RAII对象在函数结束时释放资源
	gdface::raii buffer_guard([&](){
		jpeg_finish_compress(&cinfo);
		jpeg_destroy_compress(&cinfo);
		// 压缩正常结束则调用finish_output
		if(jpeg_compress_finished)
			finish_output(outBuffer, bufferSize);
		if (nullptr != outBuffer)
			free(outBuffer);
	});
	// 初始化压缩对象
	jpeg_create_compress(&cinfo);
	jpeg_mem_dest(&cinfo, &outBuffer, &bufferSize); // 设置内存输出缓冲区
	compress_instance.start_output(cinfo);
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality<100?quality:100, true);
	compress_instance.custom_output((j_common_ptr)(&cinfo)); // 执行自定义参数设置函数
	jpeg_start_compress(&cinfo, true);
	while (cinfo.next_scanline < cinfo.image_height) {
		compress_instance.get_pixel_rows(cinfo.next_scanline);
		jpeg_write_scanlines(&cinfo, compress_instance.buffer.data(), (JDIMENSION)compress_instance.buffer.size());
	}
	jpeg_compress_finished=true;
}

void save_jpeg_mem(const fs_image_matrix &matrix,
									const mem_finish_output_fun& finish_output,
									const unsigned int quality,
									const jpeg_custom_output_fun &custom
									){
	jpeg_compress_default default_compress_instance(matrix);
	default_compress_instance.custom_output= custom;
	save_jpeg_mem(default_compress_instance,	finish_output);
}
void save_jpeg_gray_mem(const fs_image_matrix &matrix,
									const mem_finish_output_fun& finish_output,
									const unsigned int quality
									){
	static auto custom_output_gray=[](j_common_ptr cinfo) {
			jpeg_set_colorspace((j_compress_ptr)cinfo, JCS_GRAYSCALE);
		};
	save_jpeg_mem(matrix,finish_output,quality,custom_output_gray);
}
/* 将jpeg格式的内存数据块jpeg_data解压缩 
 * 图像行数据存储的方式都由decompress_instance定义
 * 如果数据为空或读取数据出错抛出 jpeg_mem_exception
 */
void load_jpeg_mem(const uint8_t *jpeg_data,size_t size,
		 jpeg_decompress_interface &decompress_instance) {
	if(nullptr==jpeg_data||0==size)
		throw jpeg_mem_exception("empty image data");
	// 定义一个压缩对象 
	jpeg_decompress_struct  dinfo;
	//用于错误信息 
	jpeg_error_mgr jerr;
	// 错误输出绑定到压缩对象 
	dinfo.err = jpeg_std_error(&jerr);
	// 设置自定义的错误处理函数 
	jerr.error_exit = jpeg_mem_error_exit;
	// RAII对象在函数结束时释放资源
	gdface::raii buffer_guard([&](){
		jpeg_finish_decompress(&dinfo);
		jpeg_destroy_decompress(&dinfo);
	});
	// 初始化压缩对象
	jpeg_create_decompress(&dinfo);
	jpeg_mem_src(&dinfo, (uint8_t*)jpeg_data, (unsigned long)size); // 设置内存输出缓冲区
	(void) jpeg_read_header(&dinfo, true);
	decompress_instance.custom_output((j_common_ptr)&dinfo); // 执行自定义参数设置函数
	(void) jpeg_start_decompress(&dinfo);
	// 输出通道数必须是1/3/4
	if (dinfo.output_components != 1 && dinfo.output_components != 3 && dinfo.output_components != 4) {
		throw jpeg_mem_exception(
			"load_jpeg_mem(): Failed to load JPEG data cause by output_components error");
	}
	decompress_instance.start_output(dinfo);
	JDIMENSION num_scanlines;
	JDIMENSION expectd_lines;
	while (dinfo.output_scanline  < dinfo.output_height) {
		num_scanlines = jpeg_read_scanlines(&dinfo, decompress_instance.buffer.data(),
				(JDIMENSION)decompress_instance.buffer.size());
		expectd_lines=std::min((dinfo.output_height-dinfo.output_scanline),(JDIMENSION)decompress_instance.buffer.size());
		// 如果取到的行数小于预期的行数，则图像数据不完整抛出异常
        if (num_scanlines<expectd_lines)
        	throw jpeg_mem_exception("load_jpeg_mem(): Incomplete data");
		decompress_instance.put_pixel_rows(num_scanlines);
	}
}

void load_jpeg_mem(const std::vector<uint8_t> &jpeg_data,
		 jpeg_decompress_interface &decompress_instance){
	load_jpeg_mem(jpeg_data.data(),jpeg_data.size(),decompress_instance);
}

fs_image_matrix load_jpeg_mem(const uint8_t *jpeg_data,size_t size,	const jpeg_custom_output_fun &custom) {
	jpeg_decompress_default default_decompress_instance;
	default_decompress_instance.custom_output = custom;
	load_jpeg_mem(jpeg_data,size,default_decompress_instance);
	return std::move(default_decompress_instance.matrix);
}

fs_image_matrix load_jpeg_mem(const std::vector<uint8_t> &jpeg_data,const jpeg_custom_output_fun &custom){
	return load_jpeg_mem(jpeg_data.data(),jpeg_data.size(),custom);
}
fs_image_matrix load_jpeg_gray_mem(const uint8_t *jpeg_data,size_t size) {
	static auto custom_output_gray=[](j_common_ptr dinfo) {
			((j_decompress_ptr)dinfo)->out_color_space = JCS_GRAYSCALE;
		};
	return load_jpeg_mem(jpeg_data,size,custom_output_gray);
}
fs_image_matrix load_jpeg_gray_mem(const std::vector<uint8_t> &jpeg_data){
	return load_jpeg_gray_mem(jpeg_data.data(),jpeg_data.size());
}

/* 图像数据(输出/输入)接口 */
struct jpeg_io_interface{
	// 虚函数用于初始化图像数据源或目标
	virtual void open(j_common_ptr info)const=0;
	// 虚函数用于关闭数据IO
	virtual void close()const =0;
	virtual ~jpeg_io_interface()=default;
};
template<bool COMPRESS>
struct jpeg_io_mem:public jpeg_io_interface{
	mutable uint8_t *jpeg_data=nullptr;
	mutable unsigned long size=0;
	template<bool C=COMPRESS,typename Enable=typename std::enable_if<!C>::type>
	jpeg_io_mem(const uint8_t *jpeg_data,size_t size):jpeg_data(const_cast<uint8_t *>(jpeg_data)),size((unsigned long)size){}
	template<bool C=COMPRESS,typename Enable=typename std::enable_if<!C>::type>
	jpeg_io_mem(const std::vector<uint8_t> &jpeg_data):jpeg_data(const_cast<uint8_t *>(jpeg_data.data())),size((unsigned long)jpeg_data.size()){}
	virtual void open(j_common_ptr info)const {
		if (COMPRESS)
			jpeg_mem_dest((j_compress_ptr)info,&jpeg_data,&size);
		else {
			if (nullptr == jpeg_data || 0 == size)
				throw jpeg_mem_exception("empty image data");
			jpeg_mem_src((j_decompress_ptr)info, jpeg_data, size);
		}
	}
	virtual void close()const{}
};
template<bool COMPRESS>
struct jpeg_io_file:public jpeg_io_interface{
	jpeg_io_file( const char *const filename):jpeg_io_file(nullptr,filename){}
	jpeg_io_file(std::FILE *const file):jpeg_io_file(file,nullptr){}
	virtual void open(j_common_ptr info)const {
		if(COMPRESS)
			jpeg_stdio_dest((j_compress_ptr)info, file);
		else
			jpeg_stdio_src((j_decompress_ptr)info, file);
	}
	virtual void close()const{
		if(nullptr!=filename){
			fclose(file);
		}
	}
private:
	const char *const filename;
	mutable std::FILE * file;
	jpeg_io_file( std::FILE *const file,const char *const filename):file(file),filename(filename){
				if((nullptr==filename||0==strlen(filename))&&nullptr==file)
					throw std::invalid_argument("the argument filename and file must not all be nullptr (empty)");
				if(nullptr==this->file){
					if(nullptr==filename||0==strlen(filename))
						throw std::invalid_argument("the argument filename and file must not all be nullptr (empty)");
					if(nullptr==(this->file=fopen(filename,COMPRESS?"wb":"rb"))){
						throw std::invalid_argument(std::string("can't open file ").append(filename));
					}
				}
			}
};
/* (不解压缩)读取jpeg格式的内存数据块的基本信息返回image_matrix_pram对象
 * 如果数据为空或读取数据出错抛出 jpeg_mem_exception
 */
fs_image_matrix read_jpeg_header(const jpeg_io_interface &src) {
	// 定义一个压缩对象
	jpeg_decompress_struct  dinfo;
	// 用于错误信息
	jpeg_error_mgr jerr;
	// 错误输出绑定到压缩对象
	dinfo.err = jpeg_std_error(&jerr);
	// 设置自定义的错误处理函数
	jerr.error_exit = jpeg_mem_error_exit;
	// RAII对象在函数结束时释放资源
	gdface::raii buffer_guard([&](){
		src.close();
		jpeg_destroy_decompress(&dinfo);
	});
	// 初始化压缩对象
	jpeg_create_decompress(&dinfo);
	src.open((j_common_ptr)&dinfo);
	(void) jpeg_read_header(&dinfo, true);
	fs_image_matrix matrix;
	// 填充图像基本信息结构
	matrix.width=dinfo.image_width;
	matrix.height=dinfo.image_height;
	matrix.color_space=(FS_COLOR_SPACE)dinfo.jpeg_color_space;
	matrix.channels=dinfo.num_components;
	//std::cout<<matrix.width<<"x"<<matrix.height<<"x"<<(uint32_t)matrix.channels<<" color="<<matrix.color_space<<std::endl;
	return std::move(matrix);
}
fs_image_matrix read_jpeg_header_file(const char *const filename) {
	return read_jpeg_header(jpeg_io_file<false>(filename));
}
fs_image_matrix read_jpeg_header_file(std::FILE *const file) {
	return read_jpeg_header(jpeg_io_file<false>(file));
}
fs_image_matrix read_jpeg_header_mem(const uint8_t *jpeg_data,size_t size) {
	return read_jpeg_header(jpeg_io_mem<false>(jpeg_data,size));
}
fs_image_matrix read_jpeg_header_mem(const std::vector<uint8_t> &jpeg_data) {
	return read_jpeg_header(jpeg_io_mem<false>(jpeg_data));
}
uint8_t depth(J_COLOR_SPACE color_space){
    switch(color_space){
    case JCS_GRAYSCALE:
    	return 1;
    case JCS_RGB565:
    	return 2;
    case JCS_EXT_RGB:
    case JCS_YCbCr:
    case JCS_RGB:
    	return 3;
    	break;
    case JCS_EXT_RGBA:
    case JCS_EXT_RGBX:
    case JCS_EXT_ARGB:
    case JCS_EXT_XRGB:
    case JCS_EXT_BGRA:
    case JCS_EXT_BGRX:
    case JCS_EXT_ABGR:
    case JCS_EXT_XBGR:
    case JCS_YCCK:
    case JCS_CMYK:
    	return 4;
    default:
    	throw jpeg_mem_exception("unsupported color space");
    }
}
template<J_COLOR_SPACE COLOR_SPACE>
typename std::enable_if<COLOR_SPACE==JCS_RGB||COLOR_SPACE==JCS_EXT_RGBA||COLOR_SPACE==JCS_EXT_RGB||COLOR_SPACE==JCS_EXT_RGBX,uint8_t>::type
gray_value(const uint8_t *ptr){
	// red=ptr[0] green=ptr[1] blud=ptr[2]
	return uint8_t( (ptr[0]*299+ptr[1]*587+ptr[2]*114+500) / 1000);
}
template<J_COLOR_SPACE COLOR_SPACE>
typename std::enable_if<COLOR_SPACE==JCS_EXT_ARGB||COLOR_SPACE==JCS_EXT_XRGB,uint8_t>::type
gray_value(const uint8_t *ptr){
	// red=ptr[1] green=ptr[2] blud=ptr[3]
	return uint8_t( (ptr[1]*299+ptr[2]*587+ptr[3]*114+500) / 1000);
}

template<J_COLOR_SPACE COLOR_SPACE>
typename std::enable_if<COLOR_SPACE==JCS_EXT_BGRA||COLOR_SPACE==JCS_EXT_BGR||COLOR_SPACE==JCS_EXT_BGRX,uint8_t>::type
gray_value(const uint8_t *ptr){
	// red=ptr[2] green=ptr[1] blud=ptr[0]
	return uint8_t( (ptr[2]*299+ptr[1]*587+ptr[0]*114+500) / 1000);
}
template<J_COLOR_SPACE COLOR_SPACE>
typename std::enable_if<COLOR_SPACE==JCS_EXT_ABGR||COLOR_SPACE==JCS_EXT_XBGR,uint8_t>::type
gray_value(const uint8_t *ptr){
	// red=ptr[3] green=ptr[2] blud=ptr[1]
	return uint8_t( (ptr[3]*299+ptr[2]*587+ptr[1]*114+500) / 1000);
}

#define RGB565_MASK_RED 0xF800
#define RGB565_MASK_GREEN 0x07E0
#define RGB565_MASK_BLUE 0x001F
template<J_COLOR_SPACE COLOR_SPACE>
typename std::enable_if<COLOR_SPACE==JCS_RGB565,uint8_t>::type
gray_value(const uint8_t *ptr){
	// 这个转换在大端系统存在问题
	auto wPixel=uint16_t(*ptr);
	uint8_t R = (wPixel & RGB565_MASK_RED) >> 11; // 取值范围0-31
	uint8_t G = (wPixel & RGB565_MASK_GREEN) >> 5; // 取值范围0-63
	uint8_t B = wPixel & RGB565_MASK_BLUE; // 取值范围0-31
	return  (R*299+G*587+B*114+500) / 1000;
}

template<J_COLOR_SPACE COLOR_SPACE>
typename std::enable_if<COLOR_SPACE==JCS_YCbCr||COLOR_SPACE==JCS_YCCK,uint8_t>::type
gray_value(const uint8_t *ptr){
	return *ptr;
}

template<J_COLOR_SPACE COLOR_SPACE>
void convert(const uint8_t*src_ptr, uint8_t*dst_ptr,size_t size,size_t src_step){
    for (size_t y = 0; y < size; ++y,src_ptr+=src_step,++dst_ptr) {
   		*dst_ptr=gray_value<COLOR_SPACE>(src_ptr);
    }
}
/*
 * 将彩色图像转为灰度图像
*/
fs_image_matrix to_gray_image_matrix(const fs_image_matrix&matrix){
	if (FSC_GRAYSCALE == matrix.color_space){ 
		// 调用复制构造函数
		return matrix;
	}
	auto row_stride=fs_get_row_stride(matrix);
	auto new_size=row_stride*matrix.height;
    fs_image_matrix gray{matrix.width,matrix.height,1,FSC_GRAYSCALE,matrix.align,nullptr};
    auto dimbuf=depth((J_COLOR_SPACE)matrix.color_space);
    auto src_ptr=matrix.pixels;
    auto dst_ptr=gray.pixels;
    switch((J_COLOR_SPACE)matrix.color_space){
    case JCS_RGB:
    	convert<JCS_RGB>(src_ptr,dst_ptr,new_size,dimbuf);
    	break;
    case JCS_EXT_RGBA:
    	convert<JCS_EXT_RGBA>(src_ptr,dst_ptr,new_size,dimbuf);
    	break;
    case JCS_EXT_RGB:
    	convert<JCS_EXT_RGB>(src_ptr,dst_ptr,new_size,dimbuf);
    	break;
    case JCS_EXT_RGBX:
    	convert<JCS_EXT_RGBX>(src_ptr,dst_ptr,new_size,dimbuf);
    	break;
    case JCS_EXT_ARGB:
    	convert<JCS_EXT_ARGB>(src_ptr,dst_ptr,new_size,dimbuf);
    	break;
    case JCS_EXT_XRGB:
    	convert<JCS_EXT_XRGB>(src_ptr,dst_ptr,new_size,dimbuf);
    	break;
    case JCS_EXT_BGRA:
    	convert<JCS_EXT_BGRA>(src_ptr,dst_ptr,new_size,dimbuf);
    	break;
    case JCS_EXT_BGR:
    	convert<JCS_EXT_BGR>(src_ptr,dst_ptr,new_size,dimbuf);
    	break;
    case JCS_EXT_ABGR:
    	convert<JCS_EXT_ABGR>(src_ptr,dst_ptr,new_size,dimbuf);
    	break;
    case JCS_EXT_XBGR:
    	convert<JCS_EXT_XBGR>(src_ptr,dst_ptr,new_size,dimbuf);
    	break;
    case JCS_RGB565:
    	convert<JCS_RGB565>(src_ptr,dst_ptr,new_size,dimbuf);
    	break;
    case JCS_YCbCr:
    	convert<JCS_YCbCr>(src_ptr,dst_ptr,new_size,dimbuf);
    	break;
    case JCS_YCCK:
    	convert<JCS_YCCK>(src_ptr,dst_ptr,new_size,dimbuf);
    	break;
    default:
    	throw jpeg_mem_exception("unsupported color space");
    }
    return gray;
}


