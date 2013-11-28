#ifndef __APP_WRAP_H__
#define __APP_WRAP_H__

class AppWrap
{
public:
	static void setViewSize(unsigned int width,unsigned int height);
	static bool init(void* hwnd);
	static void update(int tick,int delta_ms);
	static void uninit();

	//simple input
	static void move(float dx,float dy);
};


#endif