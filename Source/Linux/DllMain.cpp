// dllmain.cpp : Defines the entry point for the DLL application..
#include "Index.hpp"

const char service_interp[] __attribute__((section(".interp"))) = "/DiscordCoreAPI_DLL.so.2";

void lib_service(void)
{

    printf("This is a service of the shared library\n");

}

void lib_entry(void)
{
    printf("Entry point of the service library\n");

    _exit(0);
}