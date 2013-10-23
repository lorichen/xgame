#ifndef __RenderTarget_H__
#define __RenderTarget_H__

class RenderTarget
{
public:
	virtual void 		release();
	virtual const ITexture*	getTexture();
	virtual void 		onAttach();
	virtual void 		onDetach();
	virtual void 		onEndFrame(int layer);
	virtual bool		getTextureData(void  *pData);
	virtual void		getRect(RECT *rc);
public:
	struct RenderState
	{
		Matrix4	m_mtxView,m_mtxWorld,m_mtxProjection,m_mtxModelView;
		bool	m_bDepthWriteEnabled;
		bool	m_bLightingEnabled;
		SceneBlendFactor	m_BlendFactorSource,m_BlendFactorDest;
		bool	m_bDepthCheckEnabled;
		CullingMode	m_cullingMode;
		ColorValue	m_color;
		bool	m_bAlphaCheckEnabled;
		CompareFunction	m_AlphaCompareFunction;
		float	m_AlphaCompareValue;
		FillMode	m_fillMode;
		ColorValue	m_ambientColor;
		ColorValue		m_clearColor;
		CompareFunction	m_DepthBufferFunction;

		RenderState()
		{
			m_bDepthWriteEnabled = true;
			m_bLightingEnabled = false;
			m_bDepthCheckEnabled = false;
			m_cullingMode = CULL_NONE;
			m_color = ColorValue::White;
			m_BlendFactorSource = SBF_ONE;
			m_BlendFactorDest = SBF_ZERO;
			m_mtxView = Matrix4::IDENTITY;
			m_mtxWorld = Matrix4::IDENTITY;
			m_mtxProjection = Matrix4::IDENTITY;
			m_mtxModelView = Matrix4::IDENTITY;
			m_ambientColor = ColorValue(0.2, 0.2, 0.2, 0.1);
			m_bAlphaCheckEnabled = false;
			m_AlphaCompareFunction = CMPF_ALWAYS_PASS;
			m_AlphaCompareValue = 0;
			m_fillMode = FM_SOLID;
			m_clearColor = ColorValue::Black;
			m_DepthBufferFunction = CMPF_LESS;
		}
	};
public:
	HGLRC	rc;
	HDC		dc;
	HWND	m_hWnd;
	RenderState		m_RenderState2D,m_RenderState3D;
	RenderState		m_RenderState;
	bool			m_b2D;

	RenderTarget() : rc(0),dc(0),m_b2D(false),m_hWnd(0){}
	RenderTarget(HDC dc,HGLRC rc,HWND hWnd) : dc(dc),rc(rc),m_b2D(false),m_hWnd(hWnd)
	{
		m_vpLeft = m_vpTop = 0;
		m_vpWidth = m_vpHeight = 0;
	}

	//视口参数不会在切换2D / 3D时候重置，保存下来是方便使用
	int m_vpLeft,m_vpTop,m_vpWidth,m_vpHeight;
};

#endif