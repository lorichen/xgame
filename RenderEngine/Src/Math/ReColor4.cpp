#include "ReMathPch.h"
#include "ReVector3.h"
#include "ReVector4.h"
#include "ReColor3.h"
#include "ReColor4.h"

namespace xs 
{

    Color4 Color4::Black = Color4(0.0,0.0,0.0);
    Color4 Color4::White = Color4(1.0,1.0,1.0);
    Color4 Color4::Red = Color4(1.0,0.0,0.0);
    Color4 Color4::Green = Color4(0.0,1.0,0.0);
    Color4 Color4::Blue = Color4(0.0,0.0,1.0);

    //---------------------------------------------------------------------
    RGBA Color4::getAsRGBA(void) const
    {
        uchar val8;
        uint val32 = 0;

        // Convert to 32bit pattern
        // (RGBA = 8888)

        // Red
        val8 = static_cast<uchar>(r * 255);
        val32 = val8 << 24;

        // Green
        val8 = static_cast<uchar>(g * 255);
        val32 += val8 << 16;

        // Blue
        val8 = static_cast<uchar>(b * 255);
        val32 += val8 << 8;

        // Alpha
        val8 = static_cast<uchar>(a * 255);
        val32 += val8;

        return val32;
    }
    //---------------------------------------------------------------------
    ARGB Color4::getAsARGB(void) const
    {
        uchar val8;
        uint val32 = 0;

        // Convert to 32bit pattern
        // (ARGB = 8888)

        // Alpha
        val8 = static_cast<uchar>(a * 255);
        val32 = val8 << 24;

        // Red
        val8 = static_cast<uchar>(r * 255);
        val32 += val8 << 16;

        // Green
        val8 = static_cast<uchar>(g * 255);
        val32 += val8 << 8;

        // Blue
        val8 = static_cast<uchar>(b * 255);
        val32 += val8;


        return val32;
    }
    //---------------------------------------------------------------------
    ABGR Color4::getAsABGR(void) const
    {
        uchar val8;
        uint val32 = 0;

        // Convert to 32bit pattern
        // (ABRG = 8888)

        // Alpha
        val8 = static_cast<uchar>(a * 255);
        val32 = val8 << 24;

        // Blue
        val8 = static_cast<uchar>(b * 255);
        val32 += val8 << 16;

        // Green
        val8 = static_cast<uchar>(g * 255);
        val32 += val8 << 8;

        // Red
        val8 = static_cast<uchar>(r * 255);
        val32 += val8;


        return val32;
    }
    //---------------------------------------------------------------------
    void Color4::setAsRGBA(const RGBA val)
    {
        uint val32 = val;

        // Convert from 32bit pattern
        // (RGBA = 8888)

        // Red
        r = static_cast<uchar>(val32 >> 24) / 255.0f;

        // Green
        g = static_cast<uchar>(val32 >> 16) / 255.0f;

        // Blue
        b = static_cast<uchar>(val32 >> 8) / 255.0f;

        // Alpha
        a = static_cast<uchar>(val32) / 255.0f;
    }
    //---------------------------------------------------------------------
    void Color4::setAsARGB(const ARGB val)
    {
        uint val32 = val;

        // Convert from 32bit pattern
        // (ARGB = 8888)

        // Alpha
        a = static_cast<uchar>(val32 >> 24) / 255.0f;

        // Red
        r = static_cast<uchar>(val32 >> 16) / 255.0f;

        // Green
        g = static_cast<uchar>(val32 >> 8) / 255.0f;

        // Blue
        b = static_cast<uchar>(val32) / 255.0f;
    }
    //---------------------------------------------------------------------
    void Color4::setAsABGR(const ABGR val)
    {
        uint val32 = val;

        // Convert from 32bit pattern
        // (ABGR = 8888)

        // Alpha
        a = static_cast<uchar>(val32 >> 24) / 255.0f;

        // Blue
        b = static_cast<uchar>(val32 >> 16) / 255.0f;

        // Green
        g = static_cast<uchar>(val32 >> 8) / 255.0f;

        // Red
        r = static_cast<uchar>(val32) / 255.0f;
    }
    //---------------------------------------------------------------------
    bool Color4::operator==(const Color4& rhs) const
    {
        return (r == rhs.r &&
            g == rhs.g &&
            b == rhs.b &&
            a == rhs.a);
    }
    //---------------------------------------------------------------------
    bool Color4::operator!=(const Color4& rhs) const
    {
        return !(*this == rhs);
	}

	const Color4& Color4::operator=( const Color3& v)
	{
		memcpy(val,&v,sizeof(Color3));
		a = 1;

		return *this;
	}


}

