/*
 * j2k_mem.cpp
 *
 *  Created on: 2016年1月26日
 *      Author: guyadong
 */
#include "j2k_mem.h"
#include "assert_macros.h"

/**
 sample error callback expecting a FILE* client object
 */
void error_callback(const char* msg, void* client_data) {
	fprintf(stdout, "[ERROR] %s", msg);
	throw opj_exception(msg);
}

/**
 sample warning callback expecting a FILE* client object
 */
void warning_callback(const char* msg, void* client_data) {
	fprintf(stdout, "[WARNING] %s", msg);
}

/**
 sample debug callback expecting no client object
 */
void info_callback(const char* msg, void* client_data) {
#ifndef NDEBUG
	fprintf(stdout, "[INFO] %s", msg);
#endif
}

opj_stream_t* opj_stream_create_si(opj_stream_interface& stream, OPJ_SIZE_T p_size) {
	opj_stream_t* l_stream = opj_stream_create(p_size, stream.is_read_stream());
	if (l_stream) {
		opj_stream_set_user_data(l_stream, std::addressof(stream), (opj_stream_free_user_data_fn) (opj_stream_interface_close));
		opj_stream_set_user_data_length(l_stream, stream.stream_length());
		opj_stream_set_read_function(l_stream, (opj_stream_read_fn) (opj_stream_interface_read));
		opj_stream_set_write_function(l_stream, (opj_stream_write_fn) (opj_stream_interface_write));
		opj_stream_set_skip_function(l_stream, (opj_stream_skip_fn) (opj_stream_interface_skip));
		opj_stream_set_seek_function(l_stream, (opj_stream_seek_fn) (opj_stream_interface_seek));
		return l_stream;
	}
	throw opj_exception("fail to ceate stream:opj_stream_create");
}

opj_stream_t* opj_stream_create_default_si(opj_stream_interface& stream) {
	return opj_stream_create_si(stream, OPJ_J2K_STREAM_CHUNK_SIZE);
}

void opj_stream_interface_close(opj_stream_interface* stream_instance) {
	stream_instance->close();
}

OPJ_BOOL opj_stream_interface_seek(OPJ_OFF_T p_nb_bytes, opj_stream_interface* stream_instance) {
	return stream_instance->seek(p_nb_bytes);
}

OPJ_OFF_T opj_stream_interface_skip(OPJ_OFF_T p_nb_bytes, opj_stream_interface* stream_instance) {
	return stream_instance->skip(p_nb_bytes);
}

OPJ_SIZE_T opj_stream_interface_write(void* p_buffer, OPJ_SIZE_T p_nb_bytes, opj_stream_interface* stream_instance) {
	return stream_instance->write(p_buffer, p_nb_bytes);
}

OPJ_SIZE_T opj_stream_interface_read(void* p_buffer, OPJ_SIZE_T p_nb_bytes, opj_stream_interface* stream_instance) {
	return stream_instance->read(p_buffer, p_nb_bytes);
}
/* 从image_matrix_param创建 opj_image_t
 * 失败则抛出 opj_exception 异常
 */
opj_image_t* opj_image_create_from_matrix(const image_matrix_param& matrix, opj_cparameters_t* parameters) {
	throw_if_null(parameters);
	auto subsampling_dx = parameters->subsampling_dx;
	auto subsampling_dy = parameters->subsampling_dy;
	auto color_space = jpeglib_to_opj_color_space(matrix.color_space);
	opj_image_cmptparm_t cmptparm[4]; /* maximum of 4 components */
	opj_image_t * image = nullptr;
	/* initialize image components */
	throw_if_msg(matrix.channels>4||0==matrix.channels,"matrix.channels must be 1/2/3/4")
	memset(cmptparm, 0, matrix.channels * sizeof(opj_image_cmptparm_t));
	for (auto i = matrix.channels; i >0; --i) {
		cmptparm[i-1].prec = 8;
		cmptparm[i-1].bpp = 8;
		cmptparm[i-1].sgnd = 0;
		cmptparm[i-1].dx = (OPJ_UINT32) (subsampling_dx);
		cmptparm[i-1].dy = (OPJ_UINT32) (subsampling_dy);
		cmptparm[i-1].w = (OPJ_UINT32) (matrix.width);
		cmptparm[i-1].h = (OPJ_UINT32) (matrix.height);
	}
	/* create the image */
	if (nullptr == (image = opj_image_create((OPJ_UINT32) (matrix.channels), cmptparm, color_space)))
		throw opj_exception("failed to create image: opj_image_create\n");
	/* set image offset and reference grid */
	image->x0 = (OPJ_UINT32) (parameters->image_offset_x0);
	image->y0 = (OPJ_UINT32) (parameters->image_offset_y0);
	image->x1 = image->x0 + (OPJ_UINT32) ((matrix.width - 1)) * (OPJ_UINT32) (subsampling_dx) + 1;
	image->y1 = image->y0 + (OPJ_UINT32) ((matrix.height - 1)) * (OPJ_UINT32) (subsampling_dy) + 1;
	auto index = 0;
	uint8_t*scanline,*pixel;
	decltype(matrix.height) y;
	decltype(matrix.width) x;
	decltype(matrix.channels) ch;
	auto row_stride=get_row_stride(matrix);
	// 将image_matrix_param中按像素连续存储的通道数据依照opj_image_t的格式拆开到不同的comps中
	for (y = 0; y <matrix.height; ++y) {
		scanline = const_cast<uint8_t*>(matrix.pixels.data())+ matrix.channels * row_stride * y;
		for (x = 0; x <matrix.width ; ++x) {
			pixel = scanline+matrix.channels * x;
			for (ch = 0; ch < matrix.channels; ++ch) {
				image->comps[ch].data[index] = (OPJ_INT32)pixel[ch];
			}
			++index;
		}
	}
	return image;
}
/* 从opj_image_t 创建 image_matrix_param
 * 失败则抛出 opj_exception 异常
 */
image_matrix_param create_matrix_from_opj_image(opj_image_t* image) {
	throw_if_null(image);
	image_matrix_param matrix;
	throw_if_msg(0 == image->numcomps, "image->numcomps must >0");
	matrix.width = image->comps[0].w;
	matrix.height = image->comps[0].h;
	// 检查参数合法性
	if (image->numcomps > 1)
		for (auto i = image->numcomps - 1; i > 0; --i) {
			throw_except_if_msg(opj_exception,matrix.width != image->comps[i].w || matrix.height != image->comps[i].h||image->comps[i].prec>8||image->comps[i].bpp>8,
					"each components has different size");
		}

	matrix.channels = (uint8_t) (image->numcomps);
	matrix.color_space = opj_to_jpeglib_color_space(image->color_space);
	matrix.align = 0;
	auto row_stride = get_row_stride(matrix);
	// 为image_matrix_param分配图像存储空间，失败则抛出opj_exception
	try{
		matrix.pixels = std::vector<uint8_t>(row_stride * matrix.channels * matrix.height);
	}catch(exception &e){
		throw opj_exception(ERROR_STR(e.what()));
	}catch(...){
		throw opj_exception(ERROR_STR("fail to constructe std::vector"));
	}
	uint32_t index = 0;
	uint8_t* scanline,*pixel;
	decltype(matrix.height) y;
	decltype(matrix.width) x;
	decltype(matrix.channels) ch;
	for ( y = 0; y < matrix.height; ++y ) {
		scanline = matrix.pixels.data()+ matrix.channels * row_stride * y;
		for ( x = 0; x < matrix.height; ++x ) {
			pixel = scanline+matrix.channels * x;
			for (ch = 0; ch < matrix.channels; ++ch) {
				pixel[ch] = (uint8_t) (image->comps[ch].data[index]);
			}
			++index;
		}
	}
	return std::move(matrix);
}


/* 将opj_image_t对象的图像数据压缩成jpeg2000格式输出到opj_stream_interface对象 */
void save_j2k(opj_image_t* image, opj_cparameters_t *parameters ,opj_stream_interface& dest) {
	throw_if_null(image);
	throw_if_null(parameters);
	bool set_comment = false;
	/* Create comment for codestream */
	if (nullptr == parameters->cp_comment) {
		const char comment[] = "Created by OpenJPEG version ";
		const size_t clen = strlen(comment);
		const char* version = opj_version();
		parameters->cp_comment = (char*) (malloc(clen + strlen(version) + 1));
		sprintf(parameters->cp_comment, "%s%s", comment, version);
		set_comment = true;
	}
	opj_codec_t* l_codec = opj_create_compress((CODEC_FORMAT)parameters->cod_format);
	/* catch events using our callbacks and give a local context */
	opj_set_info_handler(l_codec, info_callback, nullptr);
	opj_set_warning_handler(l_codec, warning_callback, nullptr);
	opj_set_error_handler(l_codec, error_callback, nullptr);
	opj_setup_encoder(l_codec, parameters, image);
	opj_stream_t* l_stream = opj_stream_create_default_si(dest);
	gdface::raii guard([&]() {
		/* close and free the byte stream */
		opj_stream_destroy(l_stream);
		/* free remaining compression structures */
		opj_destroy_codec(l_codec);
		if (set_comment)
		free(parameters->cp_comment);
	});
	/* encode the image */
	if (!opj_start_compress(l_codec, image, l_stream))
		throw opj_exception("failed to encode image: opj_start_compress");

	if (!opj_encode(l_codec, l_stream))
		throw opj_exception("failed to encode image: opj_encode");

	if (!opj_end_compress(l_codec, l_stream))
		throw opj_exception("failed to encode image: opj_end_compress");
}
// 将一个image_matrix_param数据压缩成jpeg2000格式输出到opj_stream_interface对象
// 默认压缩质量100
// 默认压缩格式为OPJ_CODEC_JP2
void save_j2k(const image_matrix_param& matrix, opj_stream_interface& dest, const unsigned int quality, OPJ_CODEC_FORMAT format) {
	opj_cparameters_t parameters;
	/* set encoding parameters to default values */
	opj_set_default_encoder_parameters(&parameters);
	parameters.tcp_numlayers=1;
	parameters.tcp_distoratio[0]=(float)(quality>100?100:quality);
	parameters.cp_fixed_quality=1;
	/*parameters.tcp_numlayers=1;
	parameters.tcp_rates[0]=5;
	//parameters.tcp_rates[1]=40;
	//parameters.tcp_rates[2]=10;
	 parameters.cp_disto_alloc = 1;*/
	/*switch(format){
	case OPJ_CODEC_JP2:
		parameters.cod_format=1;
		break;
	case OPJ_CODEC_JPT:
		parameters.cod_format=2;
		break;
	default:
		parameters.cod_format=OPJ_CODEC_J2K;
	}*/
	parameters.cod_format=format;
	gdface::raii_var<opj_image_t*> raii_image([&]() {
		return opj_image_create_from_matrix(matrix, &parameters);
	}, [](opj_image_t*image) {
		/* free image data */
		opj_image_destroy (image);
	});
	save_j2k(*raii_image, &parameters,dest);
}
// 将一个image_matrix_param数据压缩成jpeg2000格式输出到内存流对象(opj_stream_mem_output)
// 默认压缩质量100
// 默认压缩格式为OPJ_CODEC_JP2
opj_stream_mem_output save_j2k_mem(const image_matrix_param& matrix, const unsigned int quality, OPJ_CODEC_FORMAT format) {
	opj_stream_mem_output dest;
	save_j2k(matrix, dest,quality, format);
	return std::move(dest);
}

OPJ_COLOR_SPACE jpeglib_to_opj_color_space(int color_space) {
	switch (color_space) {
	case JCS_RGB:
		return OPJ_CLRSPC_SRGB;
	case JCS_GRAYSCALE:
		return OPJ_CLRSPC_GRAY;
	case JCS_CMYK:
		return OPJ_CLRSPC_CMYK;
	case JCS_YCbCr:
		return OPJ_CLRSPC_SYCC;
	case JCS_YCCK:
		return OPJ_CLRSPC_EYCC;
	default:
		throw opj_exception("fail to convert J_COLOR_SPACE to OPJ_COLOR_SPACE");
	}
}

J_COLOR_SPACE opj_to_jpeglib_color_space(int color_space) {
	switch (color_space) {
	case OPJ_CLRSPC_SRGB:
		return JCS_RGB;
	case OPJ_CLRSPC_GRAY:
		return JCS_GRAYSCALE;
	case OPJ_CLRSPC_CMYK:
		return JCS_CMYK;
	case OPJ_CLRSPC_SYCC:
		return JCS_YCbCr;
	case OPJ_CLRSPC_EYCC:
		return JCS_YCCK;
	default:
		throw opj_exception("fail to convert OPJ_COLOR_SPACE to J_COLOR_SPACE");
	}
}
/* 从流对象(opj_stream_t)中根据parameters提供的解码参数解码jpeg2000图像
 * 返回 opj_image_t,出错则抛出opj_exception异常
 */
opj_image_t* load_j2k(opj_stream_t* l_stream,opj_dparameters_t& parameters) {
	opj_image_t* image = nullptr; /* Handle to a image */
	opj_codec_t* l_codec = nullptr; /* Handle to a decompressor */
	auto decompress_ok = false;
	gdface::raii guard([&]() {
		/* free remaining compression structures */
		opj_destroy_codec(l_codec);
		if (!decompress_ok) {
			opj_image_destroy(image);
			image = nullptr;
		}
	});
	/* decode the JPEG2000 stream */
	/* ---------------------- */
	switch (parameters.decod_format) {
	case OPJ_CODEC_J2K: /* JPEG-2000 codestream */
	case OPJ_CODEC_JP2: /* JPEG 2000 compressed image data */
	case OPJ_CODEC_JPT: /* JPEG 2000, JPIP */
	{
		/* Get a decoder handle */
		l_codec = opj_create_decompress((OPJ_CODEC_FORMAT) parameters.decod_format);
		break;
	}
	default:
		throw opj_exception(ERROR_STR("invalid decod_format"));
	}
	/* catch events using our callbacks and give a local context */
	opj_set_info_handler(l_codec, info_callback, nullptr);
	opj_set_warning_handler(l_codec, warning_callback, nullptr);
	opj_set_error_handler(l_codec, error_callback, nullptr);
	/* Setup the decoder decoding parameters using user parameters */
	if (!opj_setup_decoder(l_codec, &parameters)) {
		throw opj_exception("ERROR -> opj_compress: failed to setup the decoder");
	}
	/* Read the main header of the codestream and if necessary the JP2 boxes*/
	if (!opj_read_header(l_stream, l_codec, &image)) {
		throw opj_exception("ERROR -> opj_decompress: failed to read the header");
	}
	if (!parameters.nb_tile_to_decode) {
		/* Optional if you want decode the entire image */
		if (!opj_set_decode_area(l_codec, image, (OPJ_INT32) (parameters.DA_x0), (OPJ_INT32) (parameters.DA_y0),
				(OPJ_INT32) (parameters.DA_x1), (OPJ_INT32) (parameters.DA_y1)))
			throw opj_exception("ERROR -> opj_decompress: failed to set the decoded area");

		/* Get the decoded image */
		if (!(opj_decode(l_codec, l_stream, image) && opj_end_decompress(l_codec, l_stream)))
			throw opj_exception("ERROR -> opj_decompress: failed to decode image!");
	} else {
		if (!opj_get_decoded_tile(l_codec, l_stream, image, parameters.tile_index))
			throw opj_exception("ERROR -> opj_decompress: failed to decode tile!");
#ifndef NDEBUG
		printf("tile %d is decoded!\n", parameters.tile_index);
#endif
	}
	decompress_ok = true;
	return image;
}

/* 从流对象(opj_stream_interface)中根据parameters提供的解码参数解码jpeg2000图像
 * 返回 opj_image_t,出错则抛出opj_exception异常
 */
opj_image_t* load_j2k(opj_stream_interface& src, opj_dparameters_t& parameters) {
	gdface::raii_var<opj_stream_t*> raii_stream([&]() {
		return opj_stream_create_default_si(src); /* Stream */
	}, [](opj_stream_t* l_stream) {
		/* free image data */
		opj_stream_destroy(l_stream);
	});
	return load_j2k(*raii_stream,parameters);
}
/* 从流对象(opj_stream_interface)中解码指定格式(format)的jpeg2000图像
 * 返回 opj_image_t,出错则抛出opj_exception异常
 */
opj_image_t* load_j2k(opj_stream_interface& src, OPJ_CODEC_FORMAT format) {
	opj_dparameters_t parameters;
	/* set decoding parameters to default values */
	opj_set_default_decoder_parameters(&parameters);
	parameters.decod_format = format;
	return load_j2k(src, parameters);
}
/* 从jpeg_data和size指定的内存数据中解码指定格式(format)的jpeg2000图像
 * 返回 image_matrix_param对象,出错则抛出opj_exception异常
 */
image_matrix_param load_j2k_mem(const uint8_t* jpeg_data, size_t size, OPJ_CODEC_FORMAT format) {
	throw_if_null(jpeg_data)
	throw_if_msg(0 == size, "jpeg_data is empty")
	opj_stream_mem_input src(jpeg_data, size);
	gdface::raii_var<opj_image_t*> raii_image([&]() {
		return load_j2k(src, format);
	}, [](opj_image_t* image) {
		/* free image data */
		opj_image_destroy(image);
	});
	return create_matrix_from_opj_image(*raii_image);
}
/* 从jpeg_data指定的内存数据中解码指定格式(format)的jpeg2000图像
 * 返回 image_matrix_param对象,出错则抛出opj_exception异常
 */
image_matrix_param load_j2k_mem(const std::vector<uint8_t>&jpeg_data, OPJ_CODEC_FORMAT format){
	return load_j2k_mem(jpeg_data.data(),jpeg_data.size(),format);
}
