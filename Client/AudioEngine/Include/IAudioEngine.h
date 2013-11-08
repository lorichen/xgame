/********************************************************************	
*********************************************************************/
/***************************** HISTORY *******************************

*********************************************************************/
#pragma once

/*! \addtogroup AudioEngine
*  音效引擎
*  @{
*/

/// 引擎默认的最大软通道数
#define AUDIOENGINE_DEFAULT_MAXCHANNEL			32

/// 无效的声道索引号
#define INVALID_CHANNELID						-1

/// 声音类型
enum
{
	/// 适合那些需要多次播放的小的声音片段，例如音效。它播放时占用很少的CPU，并且能使用硬件加速
	SOUNDRES_TYPE_SOUND = 1,

	/// 适合那些因为太大而无法载如内存的声音，声音会被流化并装入到一小段由FMOD管理的环形缓冲中。
	/// 这会占用少量CPU和硬盘带宽，具体视文件格式而定。举个例子，实时解码MP3会比播放一个PCM(未
	/// 压缩WAV)文件占用更多的CPU。一个流式的声音只能播放一次，因为一个流只有一个文件句柄和一个环形缓冲
	SOUNDRES_TYPE_STREAM = 2,
};

/// 声道组，也就是多个声道组成一个集合，可以同时调声音大小，停止播放等
/// 此接口不要重载，因为本工程内会进行强制转换
struct IChannelGroup
{
	/** 释放
	@param   
	@param   
	@param   
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual	void			Release(void) = 0;
	
	/** 设置音量大小
	@param   fVolume ：声音大小(0.0 ~ 1.0)
	@param   
	@param   
	@return  成功：TRUE，失败：FALSE
	@note     
	@warning 
	@retval buffer 
	*/
	virtual BOOL			SetVolume(float fVolume) = 0;

	/** 取得音量大小
	@param   fVolume ：返回音量大小
	@param   
	@param   
	@return  成功：TRUE，失败：FALSE
	@note     
	@warning 
	@retval buffer 
	*/
	virtual BOOL			GetVolume(float &fVolume) = 0;

	/** 停止所有声音播放
	@param   
	@param   
	@param   
	@return  成功：TRUE，失败：FALSE
	@note     
	@warning 
	@retval buffer 
	*/
	virtual BOOL			Stop(void) = 0;

	/** 设置静音与否
	@param   nChannel：声道索引号
	@param   bMute ：静音：TRUE,非静音：FALSE
	@param   
	@return  成功：TRUE，失败：FALSE
	@note     
	@warning 
	@retval buffer 
	*/
	virtual BOOL			SetMute(bool bMute) = 0;
};


/// 音效引擎接口
struct IAudioEngine
{	
	/** 释放音效引擎
	 @param   
	 @param   
	 @param   
	 @return  
	 @note     
	 @warning 
	 @retval buffer 
	 */	
	virtual void			Release(void) = 0;

	/** 创建一个声道组
	@param   
	@param   
	@param   
	@return  返回一个声道组，外面要负责释放此内存
	@note     
	@warning 
	@retval buffer 
	*/
	virtual IChannelGroup *	CreateChannelGroup(void) = 0;


	/** 将SID与声音文件对应起来,未来对声音操作时都是以SID为标识，不再以声音文件名
	@param   dwSID ：声音的ID，由上层分配，全局唯一
	@param   pstrFileName ：声音文件名，绝对路径
	@param   dwSoundType ：声音类型，决定载入类型
	@return  成功：TRUE，失败：FALSE
	@note     
	@warning 必须通过此方法将声音资源压入pool，才可以对声音资料进行操作，播放
	@retval buffer 
	*/
	virtual BOOL			PutSoundResToPool(DWORD dwSID, LPCSTR pstrFileName, DWORD dwSoundType = SOUNDRES_TYPE_SOUND) = 0;

	
	/** 播放音效
	 @param   dwSID：音效ID
	 @param   nLoop：循环次数（-1：一直循环，0：播一次，1，2....）
	 @param   fVolume ：音量大小（0.0 ~ 1.0）
	 @param   pChannelGroup ：此声音归属何个声道组
	 @return  返回声道索引号，未来关闭那个声道时，需要提供声道索引号，返回－1表失败
	 @note     
	 @warning 
	 @retval buffer 
	 */
	virtual int				PlaySound(DWORD dwSID, int nLoop, float fVolume, IChannelGroup * pChannelGroup = NULL) = 0;

	/** 根据声道索引号，停止播放
	@param   nChannel ：声道索引号
	@param   
	@param   
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void			StopSound(int nChannel) = 0;

	/** 根据SID，停止播放
	@param   nSID：音效ID
	@param   
	@param   
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void			StopSoundBySID(DWORD dwSID) = 0;

	/** 设置音量大小
	@param   nChannel：声道索引号
	@param   fVolume ：声音大小(0.0 ~ 1.0)
	@param   
	@return  成功：TRUE，失败：FALSE
	@note     
	@warning 
	@retval buffer 
	*/
	virtual BOOL			SetVolume(int nChannel, float fVolume) = 0;

	/** 取得音量大小
	@param   nChannel：声道索引号
	@param   fVolume ：返回音量大小
	@param   
	@return  成功：TRUE，失败：FALSE
	@note     
	@warning 
	@retval buffer 
	*/
	virtual BOOL            GetVolume(int nChannel, float &fVolume) = 0;

	/** 设置静音与否
	@param   nChannel：声道索引号
	@param   bMute ：静音：TRUE,非静音：FALSE
	@param   
	@return  成功：TRUE，失败：FALSE
	@note     
	@warning 
	@retval buffer 
	*/
	virtual BOOL			SetMute(int nChannel, bool bMute) = 0;


	/** 取得音量大小
	@param   nChannel：声道索引号
	@param   bMute ：返回 静音：TRUE,非静音：FALSE
	@param   
	@return  成功：TRUE，失败：FALSE
	@note     
	@warning 
	@retval buffer 
	*/
	virtual BOOL            GetMute(int nChannel, bool &bMute) = 0;

};
/** @} */

///////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(AUDIOENGINE_STATIC_LIB)	/// 静态库版本
	#	pragma comment(lib, MAKE_LIB_NAME(xs_cae))
	extern "C" IAudioEngine * LoadCAE(int nMaxChannel, int nPoolMaxSize);
	#	define	CreateAudioEngineProc	LoadCAE
#else													/// 动态库版本
	typedef IAudioEngine * (RKT_CDECL *procCreate)(int nMaxChannel, int nPoolMaxSize);
	#define	CreateAudioEngineProc	DllApi<procCreate>::load(MAKE_DLL_NAME(xs_cae), "LoadCAE")//（万@@）
#endif
