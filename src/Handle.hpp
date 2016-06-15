#ifndef _HANDLE_HPP_
#define _HANDLE_HPP_

#include "MLOpen.h"
#include <vector>
#include <cstdio>
#include <cstring>

// TODO: Should be here and not in MLOpen.h
#if MLOpen_BACKEND_OPENCL
//#include <CL/cl.h>
//typedef cl_command_queue mlopenStream_t;

#include <CL/cl.hpp>

#elif MLOpen_BACKEND_HIP
//#include <hip_runtime.h>
//typedef hipStream_t mlopenStream_t;

#endif // OpenCL or HIP

struct mlopenContext {
	
	mlopenContext() {};
	mlopenContext(mlopenStream_t stream);
	~mlopenContext() {};

	template <typename Stream>
	mlopenStatus_t CreateDefaultStream() {printf("in default template\n");}
	mlopenStatus_t SetStream(mlopenStream_t stream);
	mlopenStatus_t GetStream(mlopenStream_t *stream) const;
	
	std::vector<mlopenStream_t> _streams;
};

#if MLOpen_BACKEND_OPENCL
template<>
mlopenStatus_t mlopenContext::CreateDefaultStream<cl_command_queue>();
#elif MLOpen_BACKEND_HIP
template<>
mlopenStatus_t mlopenContext::CreateDefaultStream<hipStream_t>();
#endif // HIP vs OpenCL

#endif // _HANDLE_HPP_
