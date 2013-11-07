#ifndef __APP_WRAP_H__
#define __APP_WRAP_H__

class AppWrap
{
public:
	static bool init(void* hwnd);
	static void update(int delta_ms);
	static void uninit();
};


#endif