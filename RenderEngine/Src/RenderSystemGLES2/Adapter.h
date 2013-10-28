#ifndef __Adapter_H__
#define __Adapter_H__

#ifdef _WIN32

namespace xs{

	/**显示器分辨率管理类
	*/
	class Adapter  
	{
	protected:
		Adapter();
		bool fullScreen(DEVMODE& dm);
		bool toggleBack();
		bool toggleFullScreen( uint nWidth, uint nHeight,uint nColorDepth, uint nFrequency );
		bool testMode(DEVMODE& dm);
	public:
		~Adapter();

		static Adapter *Instance()
		{
			static Adapter a;
			return &a;
		}
	public:
		bool toggleMode(void *hWnd,bool bFullScreen,uint nWidth, uint nHeight, uint nColorDepth, uint nFrequency);
		bool isFullscreen()	{return !m_bWndMode;}
		bool getCurrentMode(void *hWnd, bool &bFullScreen, uint &nWidth, uint & nHeight, uint &nColorDepth, uint & nFrequency);
	private:
		DEVMODE m_OldDevMode;
		bool	m_bWndMode;
	};

}
#endif

#endif
