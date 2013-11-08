#pragma once


struct IProgressManager
{
	//初始化游戏进度
	virtual bool StartInitGameProgress( uint progresslen) =0;//开始游戏初始化进度
	virtual void StopInitGameProgress() = 0;//结束游戏初始化进度

	//加载场景进度
	virtual void StartLoadingScenceProgress(uint progresslen ) =0;//开始加载场景进度
	virtual void StopLoadingScenceProgress()=0;//停止加载场景进度
	virtual void AdvanceLoadingScenceProgressLength( uint delta ) =0;//增加进度
	virtual bool IsLoadingScence()=0;//是否正在加载场景
	
	virtual void Release()=0;//释放进度管理器
	virtual bool InitScheme(void)=0;//初始化进度管理器的配置

};

IProgressManager * CreateProgressManager(void);