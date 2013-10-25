

#include "stdafx.h"
#include <Foundation/NSThread.h>

unsigned int GetCurrentThreadId()
{
    NSThread* thread = [NSThread currentThread];
    return (unsigned int)thread;
}




