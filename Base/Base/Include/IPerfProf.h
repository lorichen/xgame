#ifndef __I_PerfProf_H__
#define __I_PerfProf_H__

namespace xs
{

#ifndef RKT_PROFILE
	#define _NO_PERF_PROF
#endif

#ifdef _NO_PERF_PROF
	#define	PP_BY_NAME(name)
	#define	PP_BY_ID(name,id)
	#define PP_BY_NAME_START(name)
	#define PP_BY_NAME_STOP()
#else
	class RKT_EXPORT ppSample
	{
	public:
		ppSample(const char * name);
		ppSample(const char * name,int id);
		~ppSample();
	};

#if !defined(RELEASEDEBUG) && !defined(_DEBUG)
	#define	PP_BY_NAME( name )		xs::ppSample __profile(name);
	#define	PP_BY_ID( name,id )		xs::ppSample __profile(name,id);
	#define PP_BY_NAME_START(name)	xs::getProfile()->Start(name);
	#define PP_BY_NAME_STOP()		xs::getProfile()->Stop();
#else
	#define	PP_BY_NAME(name)
	#define	PP_BY_ID(name,id)
	#define PP_BY_NAME_START(name)
	#define PP_BY_NAME_STOP()
#endif
	
	struct IProfileNode
	{
		virtual IProfileNode * GetParent()	= 0;
		virtual IProfileNode * GetSibling()	= 0;
		virtual IProfileNode * GetChild()	= 0;

		virtual IProfileNode * GetSubNode( const char * name ) = 0;

		virtual void				Reset( void )	= 0;
		virtual void				Call( void )	= 0;
		virtual bool				Return( void )	= 0;

		virtual const char *	GetName()		= 0;
		virtual int				GetTotalCalls()	= 0;
		virtual float			GetTotalTime()	= 0;

		virtual float			GetAverageTime() = 0;
		virtual float			GetMaxTime() = 0;
		virtual float			GetMinTime() = 0;
	};

	struct IProfileIterator
	{
		virtual void	First()		= 0;
		virtual void	Next()		= 0;
		virtual bool	IsDone()	= 0;

		virtual void	EnterChild(int index) 					= 0;
		virtual void	EnterLargestChild()						= 0;
		virtual void	EnterParent()							= 0;

		virtual const char *	GetCurrentName()				= 0;
		virtual int				GetCurrentTotalCalls()			= 0;
		virtual float			GetCurrentTotalTime()			= 0;

		virtual const char *	GetCurrentParentName()			= 0;
		virtual int				GetCurrentParentTotalCalls()	= 0;
		virtual float			GetCurrentParentTotalTime()		= 0;
	};

	struct IProfile
	{
		virtual void Start(const char *name)				= 0;
		virtual void Stop()									= 0;

		virtual void	Reset()								= 0;
		virtual void	Profile()							= 0;
		virtual void	ShowUI()							= 0;
		virtual void	HideUI()							= 0;
		virtual void	WriteLog2File(char szFileName[])	= 0;

		virtual void	IncrementFrameCounter()				= 0;
		virtual int		GetFrameCountSinceReset()			= 0;
		virtual float	GetTimeSinceReset()					= 0;

	//	virtual IProfileIterator *	GetIterator()	= 0;
	//	virtual void				ReleaseIterator(IProfileIterator *) = 0;
		virtual IProfileNode *		GetRoot()				= 0;
	};
#endif
	RKT_API IProfile*	getProfile();
}

#endif