#ifndef __IVIDEOOBJECT_H__
#define __IVIDEOOBJECT_H__

namespace xs
{

	enum  enumVideoObjectType
	{
		EVOT_AVI,	//avi视频对象
		EVOT_Unknown,//未知对象
	};

	struct IVideoObject
	{
		virtual ITexture *	getTexture( uint frame) = NULL;
		virtual uint		getFrameCount() = NULL;
		virtual void		release()=NULL;
		virtual float		getFrameInterval()=NULL;
	};
}

#endif