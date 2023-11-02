#include <iostream>
#include <dlfcn.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include "loadso.h"

//prefix和posfix是汇编的转义，目的将需要调用的新地址放入寄存器rax，然后跳转
//为什么使用rax，不影响栈
const char prefix[] = { '\x48', '\xb8' };  //MOV new_func %rax
const char postfix[] = { '\xff', '\xe0' };  //JMP %rax

void* loadSymbolAddr(const char *path, const char *symbol)
{
    void *handler = dlopen(path, RTLD_NOW);
    char *err = dlerror();
    if (handler == NULL || err != NULL)
    {
		std::cerr << (path ? path : "test") << " dlopen failed!" << err << std::endl;
		exit(-1);
    }

	void* func = dlsym(handler, symbol);
	err = dlerror();
	if (err != NULL)
	{
		std::cerr << (path ? path : "test") << " dlsym failed!" << err << std::endl;
		exit(-1);
	}
    return func;
}

void HotfixFuncByAddr(void *oldFunc, void *newFunc)
{
	//得到机器PAGE_SIZE
	size_t pageSize = getpagesize();

	//执行长度mov+函数地址+jmp
	size_t instructionLen = sizeof(prefix) + sizeof(void *) + sizeof(postfix);

	//man mprotect ref:addr must be aligned to a page boundary
	char *alignAddr = (char *)oldFunc - ((unsigned long long)oldFunc % pageSize);

	//开启代码可写权限
	int ret = mprotect(alignAddr, (char *)oldFunc - alignAddr + instructionLen, PROT_READ | PROT_WRITE | PROT_EXEC);
	if (ret != 0)
	{
		std::cerr << "mprotect write failed!" << ret << std::endl;
		exit(-1);
	}

	//将跳转指令写入原函数开头
	//覆盖并打乱了原来函数的内容
	memcpy((char *)oldFunc, prefix, sizeof(prefix));
	memcpy((char *)oldFunc + sizeof(prefix), &newFunc, sizeof(void *));
	memcpy((char *)oldFunc + sizeof(prefix) + sizeof(void *), postfix, sizeof(postfix));

	//关闭代码可写权限
	ret = mprotect(alignAddr, (char *)oldFunc - alignAddr + instructionLen, PROT_READ | PROT_EXEC);
	if (ret != 0)
	{
		std::cerr << "mprotect read failed!" << ret << std::endl;
		exit(-1);
	}
}

void signalUserHander(int signum)
{
	std::cout << "Recv Signal User" << std::endl;
	void *libFuncAddr = loadSymbolAddr("./libhotfix.so", "_ZN9TestClass4DecrEi");
	if (!libFuncAddr)
	{
		std::cerr << "libFuncAddr is null!" << std::endl;
		return;
	}

	void *mainFuncAddr = loadSymbolAddr(NULL, "_ZN9TestClass4DecrEi");
	if (!mainFuncAddr)
	{
		std::cerr << "mainFuncAddr is null!" << std::endl;
		return;
	}

	std::cout << "libfuncaddr:" << libFuncAddr << "|mainFuncAddr:" << mainFuncAddr << std::endl;
	HotfixFuncByAddr(mainFuncAddr, libFuncAddr);
};
