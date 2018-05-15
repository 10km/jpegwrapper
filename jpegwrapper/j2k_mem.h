/*
 * j2k_mem.h
 *
 *  Created on: 2016年1月25日
 *      Author: guyadong
 */

#ifndef FACEIMAGE_CIMGWRAPPER_J2K_MEM_H_
#define FACEIMAGE_CIMGWRAPPER_J2K_MEM_H_
#include <vector>
#include <iostream>
#include <algorithm>
#include <utility>
#include <cstring>
#include "openjpeg.h"
#include "raii.h"
#include "jpeg_mem.h"
using namespace std;

#define DEFAULT_MEM_STREAM_INIT_SIZE (1024*16)
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
	virtual OPJ_BOOL seek (OPJ_OFF_T p_nb_bytes)const{
		if(p_nb_bytes>=0){
			cursor=start+p_nb_bytes;
			return OPJ_TRUE;
		}
		return OPJ_FALSE;
	}
	virtual OPJ_OFF_T skip (OPJ_OFF_T p_nb_bytes)const{
		// 这个函数设计是有问题的,当p_nb_bytes为-1时返回会产生歧义，
		// 但openjpeg中opj_skip_from_file就是这么写的
		// opj_stream_skip_fn定义的返回也是bool
		// 所以也只能按其接口要求这么定义
		auto nc=cursor+p_nb_bytes;
		if(nc>=start){
			cursor=nc;
			return p_nb_bytes;
		}
		return (OPJ_OFF_T)-1;
	}
	virtual OPJ_UINT64 stream_length()const{
		return (OPJ_UINT64)(last-start);
	}
	virtual void close(){}
	virtual ~opj_stream_mem_abstract()=default;
};
/**
memory output stream
*/
class opj_stream_mem_output:public opj_stream_mem_abstract,private vector<uint8_t>{
	/** pointer to the end of the vector */
	uint8_t *end;
	using base=vector<uint8_t>;
public:
	opj_stream_mem_output():opj_stream_mem_output(DEFAULT_MEM_STREAM_INIT_SIZE){}
	opj_stream_mem_output(size_t init_capacity):base(init_capacity){
		start=stream_data();
		end=stream_data()+size();
		cursor=stream_data();
		last=stream_data();
	}
	virtual OPJ_SIZE_T read (void * p_buffer, OPJ_SIZE_T p_nb_bytes)const{
		// output stream不能读取
		return 0;
	}
	virtual OPJ_SIZE_T write (void * p_buffer, OPJ_SIZE_T p_nb_bytes){
		auto left=(OPJ_SIZE_T)(end-cursor);
		if(p_nb_bytes>left){
			// 容量不足时先扩充(至少扩充1倍)
			auto off_cur=cursor-start;
			auto off_last=last-start;
			try{
				base::resize(base::size()+max(p_nb_bytes-left,(OPJ_SIZE_T)base::size()));
			}catch(...){
				// 处理resize失败抛出的异常
#ifndef NDEBUG
				cerr<<"exception on call vector::resize"<<endl;
#endif
				return 0;
			}
			start=stream_data();
			end=start+base::size();
			last=start+off_last;
			cursor=start+off_cur;
		}
		memcpy(cursor,p_buffer,p_nb_bytes);
		auto old_cursor=cursor;
		cursor+=p_nb_bytes;
		if(cursor>last){
			if(old_cursor>last){
				memset(last,0,old_cursor-last);
			}
			last=cursor;
		}
		return p_nb_bytes;
	}
	virtual uint8_t* stream_data()const{
		return const_cast<uint8_t*>(base::data());
	}
	virtual OPJ_BOOL is_read_stream()const{return 0;}
};

/**
memory input stream(内存输入流)
实现opj_stream_interface中的read,write,stream_data,is_read_stream
*/
class opj_stream_mem_input:public opj_stream_mem_abstract{
	const uint8_t* _data;
	const size_t size;
public:
	opj_stream_mem_input(const void * data,size_t size):_data(reinterpret_cast<const uint8_t*>(data)),size(size){
		if(nullptr==data)
			throw opj_stream_exception("input data is null");
		start=const_cast<uint8_t*>(_data);
		cursor=start;
		last=start+size;
	}
	virtual OPJ_SIZE_T read (void * p_buffer, OPJ_SIZE_T p_nb_bytes)const{
		if(last>cursor){
			auto len=min((OPJ_SIZE_T)(last-cursor),p_nb_bytes);
			if(len){
				memcpy(p_buffer,cursor,len);
				cursor+=len;
				return len;
			}
		}
		return (OPJ_SIZE_T)-1;
	}
	virtual OPJ_SIZE_T write (void * p_buffer, OPJ_SIZE_T p_nb_bytes){
		// input stream不能写入
		return 0;
	}
	virtual uint8_t* stream_data()const{
		return const_cast<uint8_t*>(_data);
	}
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
J_COLOR_SPACE opj_to_jpeglib_color_space(int color_space);
opj_image_t* opj_image_create_from_matrix(const fs_image_matrix& matrix, opj_cparameters_t* parameters);
fs_image_matrix create_matrix_from_opj_image(opj_image_t* image);
void save_j2k(opj_image_t* image, opj_cparameters_t *parameters ,opj_stream_interface& dest);
void save_j2k(const fs_image_matrix& matrix, opj_stream_interface& dest, const unsigned int quality=100,OPJ_CODEC_FORMAT format = OPJ_CODEC_JP2);
opj_stream_mem_output save_j2k_mem(const fs_image_matrix& matrix, const unsigned int quality=100,OPJ_CODEC_FORMAT format = OPJ_CODEC_JP2);
opj_image_t* load_j2k(opj_stream_t* l_stream,opj_dparameters_t& parameters);
opj_image_t* load_j2k(opj_stream_interface& src, opj_dparameters_t& parameters);
opj_image_t* load_j2k(opj_stream_interface& src, OPJ_CODEC_FORMAT format);
fs_image_matrix load_j2k_mem(const uint8_t* jpeg_data, size_t size, OPJ_CODEC_FORMAT format);
fs_image_matrix load_j2k_mem(const std::vector<uint8_t>&jpeg_data, OPJ_CODEC_FORMAT format);
#endif /* FACEIMAGE_CIMGWRAPPER_J2K_MEM_H_ */
