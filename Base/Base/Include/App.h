/**
* Created by pk 2008.01.07
*/

#ifndef RKT_APP_H
#define RKT_APP_H

#include "Common.h"

namespace xs {

class CApplication
{
public:
	virtual int run() = 0;
};

/** 拥有消息循环的Win32应用程序 */
class RKT_EXPORT CAppWin32 : public CApplication
{
	// 构造（析构）
public:
	CAppWin32()							{ }
	virtual ~CAppWin32()				{ }

	// 方法
public:
	virtual bool onCreate()				{ return true; }
	virtual int run();
	/// 当用于游戏时，可以将返回值改为1，这样当没有windows消息时，将循环调用onIdle
	virtual int onIdle(int idleCount)	{ return 0; }
	virtual void onClose()				{ }
};


/// 判断某个按键是否按下
RKT_API bool isKeyPressed(uint virtualKeyCode);


/** 消息分发器 */
class RKT_EXPORT CMessageDispatcher
{
	// 构造（析构）
public:
	CMessageDispatcher()			{ }
	virtual ~CMessageDispatcher()	{ }

	// 方法
public:
	/** 处理（分发）感兴趣的消息(回调机制)，WM_DESTROY 消息被系统内置，外界没法获得
	@param hWnd 消息源的窗口句柄
	@param message 消息码
	@param wParam 参数1
	@param lParam 参数2
	@return 如果处理了消息，不想让windows调用默认处理，请返回true，否则返回false
	*/
	virtual bool onMessage(void* hWnd, uint message, uint wParam, uint lParam)	{ return false; }
};

/** 带窗口的应用程序（仅仅适合游戏应用，只有单个窗口） */
class RKT_EXPORT CAppWindow : public CAppWin32
{
private:
	CMessageDispatcher* m_msgDispatcher;	/// 消息分发器
	void*	m_hWnd;							/// 窗口句柄

	// 构造（析构）
public:
	CAppWindow() : m_msgDispatcher(0), m_hWnd(0) { }
	CAppWindow(CMessageDispatcher* msgDispatcher) : m_hWnd(0), m_msgDispatcher(msgDispatcher) { }
	virtual ~CAppWindow()	{ }

	// 方法
public:
	bool createWindow(ulong w, ulong h, ulong xPos = 0, ulong yPos = 0, bool centerWnd = true);
	void setWindowTitle(const char* title);
	void showWindow(bool show);

	// 属性
public:
	void setMessageDispatcher(CMessageDispatcher* msgDispatcher)	{ m_msgDispatcher = msgDispatcher; }
	CMessageDispatcher* getMessageDispatcher() const				{ return m_msgDispatcher; }
	void* getHWND() const											{ return m_hWnd; }
	virtual void onSize(int dx, int dy)								{ };
	virtual bool onCreate()											{ return m_hWnd != NULL; }
};

/*
class RKT_EXPORT CApplicationScript : public CApplication
{
};*/


} // namespace xs


#endif // RKT_APP_H