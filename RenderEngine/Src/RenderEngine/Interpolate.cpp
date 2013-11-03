#include "StdAfx.h"
#include "Interpolate.h"

namespace xs
{
#if (TARGET_PLATFORM == PLATFORM_WIN32)
	Quaternion QuaternionSquad ( float r, const Quaternion& c1, const Quaternion& c2, const Quaternion& c3, const Quaternion& c4 )
	{
		Quaternion result;
		typedef Quaternion* (__stdcall *Squad)(Quaternion *pOut,const Quaternion *pQ1,const Quaternion *pA,const Quaternion *pB,const Quaternion *pC,float t);
		typedef void (__stdcall *SquadSetup)(Quaternion *pA,Quaternion *pB,Quaternion *pC,const Quaternion *pQ0,const Quaternion *pQ1,const Quaternion *pQ2,const Quaternion *pQ3);

		static SquadSetup pSquadSetup = 0;
		static Squad pSquad = 0;

		if(pSquad == 0)
		{
			HINSTANCE hInstance = LoadLibrary("d3dx9_36.dll");
			if(hInstance)
			{
				pSquad = (Squad)GetProcAddress(hInstance,"D3DXQuaternionSquad");
				pSquadSetup = (SquadSetup)GetProcAddress(hInstance,"D3DXQuaternionSquadSetup");
			}
		}

		if(pSquad)
		{
			Quaternion a,b,c;
			pSquadSetup(&a,&b,&c,&c1,&c2,&c3,&c4);
			pSquad(&result,&c1,&a,&b,&c,r);
		}

		return result;
	}
#else
    Quaternion QuaternionSquad ( float r, const Quaternion& c1, const Quaternion& c2, const Quaternion& c3, const Quaternion& c4 )
	{
		Quaternion result;
        assert(0);  //have not implement!
        
		return result;
	}
#endif
}

