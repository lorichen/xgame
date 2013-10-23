#ifndef __HelperFuncs_H__
#define __HelperFuncs_H__

namespace xs
{
	/*! \addtogroup RenderEngine
	*  ‰÷»æ“˝«Ê
	*  @{
	*/

	inline Matrix4 calcBillboardMatrix(const Matrix4& mtx,const Vector3& vPos,BillboardType billboardType)
	{
		Matrix4 mbb;
		Matrix4 mtxModelView = mtx;
		mtxModelView = mtxModelView.inverse();
		Vector3 camera = mtxModelView * Vector3(0,0,0);
		Vector3 look = (camera - vPos);
		look.normalize();
		Vector3 up = ((mtxModelView * Vector3(0,1,0)) - camera);
		up.normalize();
		Vector3 right = up.crossProduct(look);
		right.normalize();
		up = look.crossProduct(right);
		up.normalize();
		// calculate the billboard matrix
		mbb[0][1] = right.x;
		mbb[1][1] = right.y;
		mbb[2][1] = right.z;
		mbb[0][2] = up.x;
		mbb[1][2] = up.y;
		mbb[2][2] = up.z;
		mbb[0][0] = look.x;
		mbb[1][0] = look.y;
		mbb[2][0] = look.z;
		mbb[0][3] = 0;
		mbb[1][3] = 0;
		mbb[2][3] = 0;
		mbb[3][0] = 0;
		mbb[3][1] = 0;
		mbb[3][2] = 0;
		mbb[3][3] = 1;

		return mbb;
	}
	/** @} */
}

#endif