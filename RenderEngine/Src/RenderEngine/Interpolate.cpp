#include "StdAfx.h"
#include "Interpolate.h"

namespace xs
{
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
}

#if 0

#include <d3dx9.h>
#pragma comment(lib,"d3dx9.lib")

namespace xs
{
	Quaternion QuaternionSquad ( float r, const Quaternion &pQ0, const Quaternion &pQ1, const Quaternion &pQ2, const Quaternion &pQ3 )
	{
		D3DXQUATERNION q1 = D3DXQUATERNION(pQ0.x,pQ0.y,pQ0.z,pQ0.w);
		D3DXQUATERNION q2 = D3DXQUATERNION(pQ1.x,pQ1.y,pQ1.z,pQ1.w);
		D3DXQUATERNION q3 = D3DXQUATERNION(pQ2.x,pQ2.y,pQ2.z,pQ2.w);
		D3DXQUATERNION q4 = D3DXQUATERNION(pQ3.x,pQ3.y,pQ3.z,pQ3.w);

		D3DXQUATERNION a,b,c;
		D3DXQuaternionSquadSetup(&a,&b,&c,&q1,&q2,&q3,&q4);
		D3DXQUATERNION q;
		D3DXQuaternionSquad(&q,&q1,&a,&b,&c,r);

		return Quaternion(q.x,q.y,q.z,q.w);
	}
}
#endif