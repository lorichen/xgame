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
		virtual ITexture *	getTexture( uint frame) = 0;
		virtual uint		getFrameCount() = 0;
		virtual void		release()=0;
		virtual float		getFrameInterval()=0;
	};
}

#endif