#include <cuda.h>  // CUDA驱动头文件cuda.h
#include <cstdio>  // 因为要使用printf
#include <cstring>

// 很明显，这种代码封装方式，更加的便于使用
//宏定义 #define <宏名>（<参数表>） <宏体>
#define checkDriver(op) __check_cuda_driver3((op), #op, __FILE__, __LINE__)

bool __check_cuda_driver3(CUresult code, const char* op, const char* file, int line){

    if(code != CUresult::CUDA_SUCCESS){
        const char* err_name = nullptr;
        const char* err_message = nullptr;
        cuGetErrorName(code, &err_name);
        cuGetErrorString(code, &err_message);
        printf("%s:%d %s failed. \n code=%s, message=%s\n", file, line, op, err_name, err_message);
        return false;
    }
    return true;
}

int main4(){
    // 检查cuda driver的初始化
    checkDriver(cuInit(0));                 // CUcontext 其实是 struct CUctx_st*(是一个指向结构体CUctx_st的指针)

    // 为设备创建上下文
    CUcontext ctxA = nullptr;
    CUcontext ctxB = nullptr;
    CUdevice device = 0;
    checkDriver(cuCtxCreate(&ctxA, CU_CTX_SCHED_AUTO, device));  // 这一步相当于告知要某一块设备上的某块地方创建 ctxA 管理数据。输入参数 参考 https://www.cs.cmu.edu/afs/cs/academic/class/15668-s11/www/cuda-doc/html/group__CUDA__CTX_g65dc0012348bc84810e2103a40d8e2cf.html
    checkDriver(cuCtxCreate(&ctxB, CU_CTX_SCHED_AUTO, device));  // 参考 1.ctx-stack.jpg
    printf("ctxA = %p\n", ctxA);
    printf("ctxB = %p\n", ctxB);
    /* 
        contexts 栈：
            ctxB -- top <--- current_context
            ctxA 
            ...
     */

    // 获取当前上下文信息
    CUcontext current_context = nullptr;
    checkDriver(cuCtxGetCurrent(&current_context));   // 这个时候current_context 就是上面创建的context
    printf("current_context=%p\n", current_context);

    // 可以使用上下文堆栈对设备管理多个上下文
    // 压入当前context
    checkDriver(cuCtxPushCurrent(ctxA));        // 将这个 ctxA 压入CPU调用thread上。专门用一个thread以栈的方式来管理多个contexts的切换
    checkDriver(cuCtxGetCurrent(&current_context));     // 获取 current_context (即栈顶的context)
    printf("after pushing, current_context = %p\n", current_context);
    /* 
        contexts 栈：
            ctxA -- top <--- current_context
            ctxB
            ...
    */

   // 弹出当前context
   CUcontext popped_ctx = nullptr;
   checkDriver(cuCtxPopCurrent(&popped_ctx));       // 将当前的context pop掉, 并用popped_ctx承接它pop出现的context
   checkDriver(cuCtxGetCurrent(&current_context));  // 获取current_context(栈顶的)
   printf("after poping, popped_ctx = %p\n", popped_ctx);   // 弹出的是ctxA
   printf("after poping, current_context = %p\n", current_context);  // current_context是ctxB

   checkDriver(cuCtxDestroy(ctxA));
   checkDriver(cuCtxDestroy(ctxB));

   // 更推荐使用cuDevicePrimaryCtxRetain获取与设备关联的context
   // 注意这个重点，以后的runtime也是基于此，自动为设备只关联一个context
   checkDriver(cuDevicePrimaryCtxRetain(&ctxA, device));    // 在 device 上指定一个新地址对ctxA进行管理
   printf("ctxA = %p\n", ctxA);
   checkDriver(cuDevicePrimaryCtxRelease(device));
   return 0;
}
