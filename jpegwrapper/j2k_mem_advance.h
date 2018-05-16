/*
 * j2k_mem.h
 *
 *  Created on: 2016年1月25日
 *      Author: guyadong
 */

#ifndef FACEIMAGE_CIMGWRAPPER_J2K_MEM_ADVANCE_H_
#define FACEIMAGE_CIMGWRAPPER_J2K_MEM_ADVANCE_H_
#include <vector>
#include <iostream>
#include <algorithm>
#include <utility>
#include <cstring>
#include <string>
#include "openjpeg.h"
#include "image_matrix_types.h"
/* openjpeg编码解码异常类 */
class opj_exception:public std::logic_error{
public:
	// 继承基类构造函数
	using std::logic_error::logic_error;
};

/* stream异常类 */
class opj_stream_exception:public std::logic_error{
public:
	// 继承基类构造函数
	using std::logic_error::logic_error;
};

/* 流(stream)接口 */
struct opj_stream_interface{
	// 从stream中读取指定长度的数据,对应opj_read_from_file
	virtual OPJ_SIZE_T read (void * p_buffer, OPJ_SIZE_T p_nb_bytes)const=0;
	// 向stream中写入指定长度的数据,对应opj_write_from_file
	virtual OPJ_SIZE_T write (void * p_buffer, OPJ_SIZE_T p_nb_bytes)=0;
	// 	以stream起始位置为参照设置stream的游标(position indicator)到指定的位置，对应opj_seek_from_file
	virtual OPJ_BOOL seek (OPJ_OFF_T p_nb_bytes)const=0;
	// 	以stream当前位置为参照设置stream的游标(position indicator)到指定的位置，对应opj_skip_from_file
	virtual OPJ_OFF_T skip (OPJ_OFF_T p_nb_bytes)const=0;
	// 返回流的长度
	virtual OPJ_UINT64 stream_length()const=0;
	// 返回流内存数据地址
	virtual uint8_t* stream_data()const=0;
	// 关闭流，对应fclose()
	virtual void close()=0;
	// 为TRUE时stream对象为read only ，否则为只写write only。
	virtual OPJ_BOOL is_read_stream()const=0;
	virtual ~opj_stream_interface()=default;
};
/**
abstract memory stream
*/
class opj_stream_mem_abstract:public opj_stream_interface{
protected:
	/** pointer to the start of the stream */
	mutable uint8_t *start;
	/** pointer to the end of the stream */
	mutable uint8_t *last;
	/** pointer to the current position */
	mutable uint8_t *cursor;
public:
	virtual OPJ_SIZE_T write (void * p_buffer, OPJ_SIZE_T p_nb_bytes)=0;
	virtual uint8_t* stream_data()const=0;
	virtual OPJ_BOOL is_read_stream()const=0;
	virtual OPJ_SIZE_T read (void * p_buffer, OPJ_SIZE_T p_nb_bytes)const=0;
	virtual OPJ_BOOL seek(OPJ_OFF_T p_nb_bytes)const;
	virtual OPJ_OFF_T skip(OPJ_OFF_T p_nb_bytes)const;
	virtual OPJ_UINT64 stream_length()const;
	virtual void close() {}
	virtual ~opj_stream_mem_abstract()=default;
};
/**
memory output stream
*/
class opj_stream_mem_output:public opj_stream_mem_abstract,private std::vector<uint8_t>{
	/** pointer to the end of the vector */
	uint8_t *end;
	using base=std::vector<uint8_t>;
public:
	opj_stream_mem_output();
	opj_stream_mem_output(size_t init_capacity);
	virtual OPJ_SIZE_T read(void * p_buffer, OPJ_SIZE_T p_nb_bytes)const;
	virtual OPJ_SIZE_T write(void * p_buffer, OPJ_SIZE_T p_nb_bytes);
	virtual uint8_t* stream_data()const;
	virtual OPJ_BOOL is_read_stream()const;
	virtual void close();
	// 将输出的压缩数据封装为string返回
	std::string as_string();
	// 将输出的压缩数据封装为vector返回
	std::vector<uint8_t> as_vector();
};

/**
memory input stream(内存输入流)
实现opj_stream_interface中的read,write,stream_data,is_read_stream
*/
class opj_stream_mem_input:public opj_stream_mem_abstract{
	const uint8_t* _data;
	const size_t size;
public:
	opj_stream_mem_input(const void * data, size_t size);
	virtual OPJ_SIZE_T read(void * p_buffer, OPJ_SIZE_T p_nb_bytes)const;
	virtual OPJ_SIZE_T write(void * p_buffer, OPJ_SIZE_T p_nb_bytes);
	virtual uint8_t* stream_data()const;
	virtual OPJ_BOOL is_read_stream()const{return 1;}
};

OPJ_SIZE_T opj_stream_interface_read(void* p_buffer, OPJ_SIZE_T p_nb_bytes, opj_stream_interface* stream_instance);
OPJ_SIZE_T opj_stream_interface_write(void* p_buffer, OPJ_SIZE_T p_nb_bytes, opj_stream_interface* stream_instance);

OPJ_OFF_T opj_stream_interface_skip(OPJ_OFF_T p_nb_bytes, opj_stream_interface* stream_instance);
OPJ_BOOL opj_stream_interface_seek(OPJ_OFF_T p_nb_bytes, opj_stream_interface* stream_instance);
void opj_stream_interface_close(opj_stream_interface* stream_instance);

opj_stream_t* opj_stream_create_si(opj_stream_interface& stream, OPJ_SIZE_T p_size);
opj_stream_t* opj_stream_create_default_si(opj_stream_interface& stream);

OPJ_COLOR_SPACE jpeglib_to_opj_color_space(int color_space);
FS_COLOR_SPACE opj_to_jpeglib_color_space(int color_space);
opj_image_t* opj_image_create_from_matrix(const fs_image_matrix& matrix, opj_cparameters_t* parameters);
fs_image_matrix create_matrix_from_opj_image(opj_image_t* image);
void save_j2k(opj_image_t* image, opj_cparameters_t *parameters ,opj_stream_interface& dest);
void save_j2k(const fs_image_matrix& matrix, opj_stream_interface& dest, const unsigned int quality=100,OPJ_CODEC_FORMAT format = OPJ_CODEC_JP2);
opj_image_t* load_j2k(opj_stream_t* l_stream,opj_dparameters_t& parameters);
opj_image_t* load_j2k(opj_stream_interface& src, opj_dparameters_t& parameters);
opj_image_t* load_j2k(opj_stream_interface& src, OPJ_CODEC_FORMAT format);
// 将图像矩阵保存为format指定的jpeg2000格式，返回内存输出流对象
opj_stream_mem_output save_j2k_mem(const fs_image_matrix& matrix, const unsigned int quality = 100, OPJ_CODEC_FORMAT format = OPJ_CODEC_JP2);

#endif /* FACEIMAGE_CIMGWRAPPER_J2K_MEM_ADVANCE_H_ */
