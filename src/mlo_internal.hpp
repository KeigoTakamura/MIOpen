/**********************************************************************
Copyright (c)2016 Advanced Micro Devices, Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or
 other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************/


#ifndef MLO_INTERNAL_H_
#define MLO_INTERNAL_H_

//Header Files
#ifndef NOMINMAX
#define NOMINMAX // stupid windows.h confused with min() macros in std namespace
#endif

#define _USE_MATH_DEFINES
#ifdef __APPLE__
 #include <mach/mach_time.h>  // for mach_absolute_time() and friends
 #include <OpenCL/opencl.h>
#else
 #include <CL/opencl.h>
#endif

#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <map>
#include <string>
#include <limits>
#include <algorithm>    // std::find  and std::min std::maxx

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <ctime>
#include <cmath>
#include <time.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <vector>
#include <numeric>
#include <cstdint>


#ifdef WIN32
 #include <io.h>
 #include <windows.h>
 #include <BaseTsd.h>
 #include <direct.h>
 #define snprintf _snprintf
 #define vsnprintf _vsnprintf
 #define strcasecmp _stricmp
 #define strncasecmp _strnicmp
 //#ifndef getcwd
 // #define getcwd _getcwd
 //#endif
 typedef unsigned int uint;

 #ifndef getcwd
   #define getcwd _getcwd
#endif

#else  // !WIN32 so Linux and APPLE
 #include <limits.h>
 #include <unistd.h>
 #include <stdbool.h>
 #include <sys/time.h>
 #include <sys/resource.h>
 typedef  long long int __int64;
 #ifndef fopen_s
   #define fopen_s(file, fileName, mode)                                          \
   ((*(file)) = fopen((fileName), (mode))) == NULL
 #endif

#endif




 /* Include CLBLAS header. It automatically includes needed OpenCL header,
 ** so we can drop out explicit inclusion of cl.h header.
 */

 //#ifndef WITH_CLBLAS
//#define WITH_CLBLAS
 //#endif

#ifdef WITH_CLBLAS
#include <clBLAS.h>
#endif


class mlo_construct_direct2D {
public:

	mlo_construct_direct2D(int dir)
	{
		_direction = dir;
		_bias_sz = 0;
		_pad0 = 1;
		_pad1 = 1;
		_kernel_size0 = 3;
		_kernel_size1 = 3;
		_kernel_stride0 = 1;
		_kernel_stride1 = 1;
	}

	int mloConstructDirect2D(void);
	bool mloGetConfig(void);
	int mloSearchDirect2D(void);
	inline void getConfigParameters(
		int & grp_tile0,
		int & grp_tile1,
		int & in_tile0,
		int & in_tile1,
		int & out_pix_tile0,
		int & out_pix_tile1,
		int & n_out_pix_tiles,
		int & n_in_data_tiles,
		int & n_stacks
		)
	{

		grp_tile0 = _grp_tile0;
		grp_tile1 = _grp_tile1;
		in_tile0 = _in_tile0;
		in_tile1 = _in_tile1;
		out_pix_tile0 = _out_pix_tile0;
		out_pix_tile1 = _out_pix_tile1;
		n_out_pix_tiles = _n_out_pix_tiles;
		n_in_data_tiles = _n_in_data_tiles;
		n_stacks = _n_stacks;
	}

	inline void setConfigParameters(
		int grp_tile0,
		int grp_tile1,
		int in_tile0,
		int in_tile1,
		int out_pix_tile0,
		int out_pix_tile1,
		int n_out_pix_tiles,
		int n_in_data_tiles,
		int n_stacks
		)
	{

		_grp_tile0 = grp_tile0;
		_grp_tile1 = grp_tile1;
		_in_tile0 = in_tile0;
		_in_tile1 = in_tile1;
		_out_pix_tile0 = out_pix_tile0;
		_out_pix_tile1 = out_pix_tile1;
		_n_out_pix_tiles = n_out_pix_tiles;
		_n_in_data_tiles = n_in_data_tiles;
		_n_stacks = n_stacks;
	}

	inline std::string getKernelFile(void) const
	{
		return(_kernel_file);
	}
	inline std::string getKernelName(void) const
	{
		return(_kernel_name);
	}
	inline const std::string & getCompilerOptions(void) const
	{
		return(_comp_options);
	}


	inline void setTimerIter(int n_timer_iter)
	{
		_n_timer_iter = n_timer_iter;
	}
	inline void setStream(void * stream)
	{
		_stream = stream;
	}
	inline void setKernelPath(const std::string & kernel_path)
	{
		_kernel_path = kernel_path;
	}


	inline void setWeightsSz(size_t weights_sz)
	{
		_weights_sz = weights_sz; // bytes
	}
	inline void setBiasSz(size_t bias_sz)
	{
		_bias_sz = bias_sz; // bytes
	}

	inline void setKernelDescr(int dim, int size, int pad, int stride = 1)
	{
		switch (dim)
		{
		case 0:
			_pad0 = pad;
			_kernel_size0 = size;
			_kernel_stride0 = stride;
			break;
		case 1:
			_pad1 = pad;
			_kernel_size1 = size;
			_kernel_stride1 = stride;
			break;
		}
	}




	inline void setBatchSize(int batch_sz)
	{
		_batch_sz = batch_sz;
	}

	inline void setOutputDescr(int height,
							int width,
							int depth,
							int stride,
							int channel_stride,
							int batch_stride,
							size_t size,
							const std::string & layout,
							const std::string & data_type
							)
	{
		if (_direction)
		{

			_out_width = width;
			_out_height = height;
			_n_outputs = depth;
			_out_batch_stride = batch_stride;
			_out_channel_stride = channel_stride;
			_out_stride = stride;
			_top_sz = size;
			_out_layout = layout;
			_out_data_type = data_type;
		}
		else
		{
			_in_width = width;
			_in_height = height;
			_n_inputs = depth;
			_in_batch_stride = batch_stride;
			_in_channel_stride = channel_stride;
			_in_stride = stride;
			_bot_sz = size;
			_in_layout = layout;
			_in_data_type = data_type;
			_tens_layout = layout;
			_tens_data_format = data_type;

		}
	}

	inline void setInputDescr(int height,
							int width,
							int depth,
							int stride,
							int channel_stride,
							int batch_stride,
							size_t size,
							const std::string & layout,
							const std::string & data_type
							)
	{
		if (_direction)
		{

			_in_width = width;
			_in_height = height;
			_n_inputs = depth;
			_in_batch_stride = batch_stride;
			_in_channel_stride = channel_stride;
			_in_stride = stride;
			_bot_sz = size;
			_in_layout = layout;
			_in_data_type = data_type;
			_tens_layout = layout;
			_tens_data_format = data_type;

		}
		else
		{
			_out_width = width;
			_out_height = height;
			_n_outputs = depth;
			_out_batch_stride = batch_stride;
			_out_channel_stride = channel_stride;
			_out_stride = stride;
			_top_sz = size;
			_out_layout = layout;
			_out_data_type = data_type;
		}
	}

	inline void doBias(int do_bias)
	{
		_bias = do_bias;
	}

	inline void doSearch(bool do_search)
	{
		_search = do_search;
	}

	inline bool doSearch(void) const
	{
		return(_search);
	}

	inline void saveSearchRequest(bool save_req)
	{
		_save_srch_req = save_req;
	}

	inline void setGeneralCompOptions(const std::string & options)
	{
		_gen_comp_options = options;
	}
	inline const std::string & getGeneralCompOptions(void) const
	{
		return(_gen_comp_options);
	}

	inline const std::vector<size_t> & getLocalWkSize(void) const
	{
		return(_l_wk);
	}
	inline const std::vector<size_t> & getGlobalWkSize(void) const
	{
		return(_g_wk);
	}

protected:

	int mloConstructDirect2DFwd(void);
	int mloConstructDirect2DFwdGen(void);

	int mloSetConf(const std::string & conf_val);
	int mloBuildConf_Key(std::string & conf_key);
	int mloBuildConf_Key(std::string & conf_key) const;
	int mloSelectDefaultConfig(std::string & conf_val);
	int mloAddConfigReq(cl_device_id dev,
		const std::string & conf_key
		) const;
	int mloRemoveConfigReq(
		cl_device_id dev,
		const std::string & conf_key
		) const;
	int mloReadConfigDB(
		cl_device_id dev,
		std::map<std::string, std::string> & conf_db
		) const;
	int mloWriteConfigDB(
		cl_device_id dev,
		const std::map<std::string, std::string> & conf_db
		) const;
	int mloAddConfig(
		cl_device_id dev,
		std::string & conf_key,
		std::string & conf_val
		) const;
	bool mloSearchConfigInDB(
		cl_device_id dev,
		std::string & conf_key,
		std::string & conf_val
		) const;

	int mloMeasuredLoop(
		cl_context ctxt,
		cl_device_id dev,
		cl_command_queue profile_q,
		cl_mem bot_ocl_buf,
		cl_mem top_ocl_buf,
		cl_mem wei_ocl_buf,
		cl_mem bias_ocl_buf,
		double &processing_time
		);

protected:

	int _direction;
	int _pad0;
	int _pad1;
	int _kernel_size0;
	int _kernel_size1;
	int _kernel_stride0;
	int _kernel_stride1;
	int _n_outputs;
	int _n_inputs;
	int _batch_sz;
	int _out_width;
	int _out_height;
	int _out_batch_stride;
	int _out_channel_stride;
	int _out_stride;
	std::string _out_layout;
	std::string _out_data_type;
	int _in_width;
	int _in_height;
	int _in_batch_stride;
	int _in_channel_stride;
	int _in_stride;
	std::string _in_layout;
	std::string _in_data_type;
	// FIX IT
//	int _weights_height;
//	int _weights_stride;
	std::string _weights_layout;
	std::string _weight_data_type;
	// 
	std::string _tens_layout;
	std::string _tens_data_format;

	int _in_tile0;     // size of in-tile in local memory
	int _in_tile1;     // size of in-tile in local memory
	int _grp_tile0;   // total number ALUs per group
	int _grp_tile1;   // total number ALUs per group
	int _out_pix_tile0; // # of generated pixels per output per wk-item  (ALU)
	int _out_pix_tile1; // # of generated pixels per output per wk-item  (ALU)
	int _n_out_pix_tiles;  // # output pixel tiles per wk-item (ALU)
	int _n_in_data_tiles; // # of blocks of different inputs in LDS
	int _n_stacks; // # of diff stacks (part of batch).
	int _bias;     // bias calculated inside conv (forward)
	std::string _comp_options;
	std::string _kernel_file;
	std::string _kernel_name;
	std::vector<size_t> _l_wk;
	std::vector<size_t> _g_wk;

	bool _gen; // genral case 
	int _n_timer_iter;
	int _quiet;
	bool _search;
	bool _save_srch_req;
	std::string _gen_comp_options;
	std::string _kernel_path;
	// local memory size per group
	size_t _dev_local_mem_sz;
	// wave size
	int _hw_wave_sz;
	// cl_queue
	void * _stream;
	size_t _bot_sz; // bytes
	size_t _top_sz; // bytes
	size_t _weights_sz; // bytes
	size_t _bias_sz; // bytes


};




#endif