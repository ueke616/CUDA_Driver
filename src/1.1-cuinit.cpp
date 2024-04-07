#include <cuda.h>  // CUDA驱动头文件cuda.h
#include <cstdio>
#include <cstring>

int main1(){
    CUresult code=cuInit(0);  //CUresult类型: 用于接受一些可能的错误代码
    if(code != CUresult::CUDA_SUCCESS) {
        const char* err_message = nullptr;
        cuGetErrorString(code, &err_message); // 获取错误代码的字符串描述
        printf("Initialize failed. code=%d, message=%s\n", code, err_message);
        return -1;
    }

    /*
    try to get current cuda driver version
    Graphics Card、CUDA、CUDA Toolkit
        1. GraphicsCard Driver Version, for example: Driver Version: 460.84
        2. CUDA Driver Version, for example: CUDA Version: 11.2
        3. CUDA Toolkit Version: for Instance, when chosing automatic download 10.2、11.2 etc;
            It is different from previous two, CUDA Toolkit's Version need at least CUDA driver Version Program

        each of three have relationship, can refer to https://docs.nvidia.com/cuda/cuda-toolkit-release-notes/index.html
        nvidia-smi show which are GraphicsCard Driver Version and highest CUDA Driver Version it's support.
    */
   int driver_version = 0;
   code = cuDriverGetVersion(&driver_version);  // 获取驱动版本
   printf("CUDA Driver version is %d\n", driver_version);  // 若driver_version为11020指的是11.2 

   // try to get current device infomation
   char device_name[100];  // char array
   CUdevice device = 0;
   code = cuDeviceGetName(device_name, sizeof(device_name), device); // get device name, type, such as Tesla V100-SXM2-32GB // 数组名device_name当作指针
   printf("Device %d name is %s \n", device, device_name);
   return 0;
}