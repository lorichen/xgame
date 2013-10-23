#include "StdAfx.h"
#include "Camera.h"
#include "Viewport.h"

namespace xs
{
	Camera::Camera(Viewport* pViewport) : m_pViewport(pViewport),m_bNeedUpdate(true)
	{
		ASSERT(pViewport);

		m_radFOVy = Radian(Math::PI / 4.0);
		m_fNearDistance = 1.0;
		m_fFarDistance = 1000.0;
		m_fSphereDistance = m_fFarDistance;

		// Locate at (0,0,0)
        m_vPosition.x = m_vPosition.y = m_vPosition.z = 0;
        // Point down -Z axis
        m_qtnOrientation = Quaternion::IDENTITY;

		m_pViewport->setCamera(this);
	}

	//-----------------------------------------------------------------------
	Camera::~Camera()
	{
	}

	void  Camera::release()
	{
		delete this;
	}

	void  Camera::setPosition(float x, float y, float z)
	{
		m_vPosition.x = x;
		m_vPosition.y = y;
		m_vPosition.z = z;

		m_bNeedUpdate = true;
	}

	void  Camera::setPosition(const Vector3& vec)
	{
		m_vPosition = vec;

		m_bNeedUpdate = true;
	}

	const Vector3& Camera::getPosition(void ) const
	{
		return m_vPosition;
	}

	void  Camera::setDirection(const Vector3& vec)
    {
        // Do nothing if given a zero vector
        if (vec == Vector3::ZERO) return;

        // Remember, camera points down -Z of local axes!
        // Therefore reverse direction of direction vector before determining local Z
        Vector3 zAdjustVec = -vec;
		zAdjustVec.normalize();

		if(1)
        {
			Vector3 mYawFixedAxis = Vector3(0,1,0);
            Vector3 xVec = mYawFixedAxis.crossProduct( zAdjustVec );
            xVec.normalize();

            Vector3 yVec = zAdjustVec.crossProduct( xVec );
            yVec.normalize();

            m_qtnOrientation.FromAxes( xVec, yVec, zAdjustVec );
        }
		else
		{
			// Get axes from current quaternion
			Vector3 axes[3];
			m_qtnOrientation.ToAxes(axes);
			Quaternion rotQuat;
			if((axes[2] + zAdjustVec).squaredLength() <  0.00005f) 
			{
				// Oops, a 180 degree turn (infinite possible rotation axes)
				// Default to yaw i.e. use current UP
				rotQuat.FromAngleAxis(Radian(Math::PI),axes[1]);
			}
			else
			{
				// Derive shortest arc to new direction
				rotQuat = axes[2].getRotationTo(zAdjustVec);

			}
			m_qtnOrientation = rotQuat * m_qtnOrientation;
		}

		m_bNeedUpdate = true;
    }

	void  Camera::lookAt(const Vector3& vLookat)
	{
		setDirection(vLookat - m_vPosition);
	}

	void  Camera::lookAt(float x, float y, float z)
	{
		lookAt(Vector3(x,y,z));
	}

	const Matrix4& Camera::getViewMatrix()
	{
		return m_mtxView;
	}

	const Matrix4& Camera::getProjectionMatrix()
	{
		return m_mtxProjection;
	}

	void  Camera::setFOVy(Radian fovy)
	{
		m_radFOVy = fovy;

		m_bNeedUpdate = true;
	}

	void  Camera::setNearDistance(float fNearDistance)
	{
		m_fNearDistance = fNearDistance;

		m_bNeedUpdate = true;
	}

	void  Camera::setFarDistance(float fFarDistance)
	{
		m_fFarDistance = fFarDistance;

		m_bNeedUpdate = true;
	}

	Radian Camera::getFOVy()
	{
		return m_radFOVy;
	}

	float Camera::getNearDistance()
	{
		return m_fNearDistance;
	}

	float Camera::getFarDistance()
	{
		return m_fFarDistance;
	}

	float Camera::getAspect()const
	{
		return (float)m_pViewport->getWidth() / (float)m_pViewport->getHeight();
	}

	void 	Camera::setFillMode(const FillMode& sdl)
	{
		m_fillMode = sdl;
	}

	const FillMode&	Camera::getFillMode()
	{
		return m_fillMode;
	}

	void 	Camera::setViewport(Viewport* pViewport)
	{
		m_pViewport = pViewport;

		m_bNeedUpdate = true;
	}

	Viewport*	Camera::getViewport()
	{
		return m_pViewport;
	}

	void  Camera::calcViewMatrix()
	{
		// ----------------------
        // Update the view matrix
        // ----------------------

         // View matrix is:
         //
         //  [ Lx  Uy  Dz  Tx  ]
         //  [ Lx  Uy  Dz  Ty  ]
         //  [ Lx  Uy  Dz  Tz  ]
         //  [ 0   0   0   1   ]
         //
         // Where T = -(Transposed(Rot) * Pos)

        // This is most efficiently done using 3x3 Matrices

        // Get orientation from quaternion

		Matrix3 rot;
		const Quaternion& orientation = m_qtnOrientation;
		const Vector3& position = m_vPosition;
		orientation.ToRotationMatrix(rot);

		// Make the translation relative to new axes
		Matrix3 rotT = rot.Transpose();
		Vector3 trans = -rotT * position;

		// Make final matrix
		m_mtxView = Matrix4::IDENTITY;
		m_mtxView = rotT; // fills upper 3x3
		m_mtxView[0][3] = trans.x;
		m_mtxView[1][3] = trans.y;
		m_mtxView[2][3] = trans.z;
	}

	void  Camera::calcProjectionMatrix()
	{
		m_mtxProjection = Matrix4::perspectiveProjection(m_radFOVy,getAspect(),m_fNearDistance,m_fFarDistance);
	}

	void  Camera::calcClipPlanes()
	{
		Matrix4 combo = m_mtxProjection * m_mtxView;
		m_clipPlanes[FRUSTUM_PLANE_LEFT].normal.x = combo[3][0] + combo[0][0];
		m_clipPlanes[FRUSTUM_PLANE_LEFT].normal.y = combo[3][1] + combo[0][1];
		m_clipPlanes[FRUSTUM_PLANE_LEFT].normal.z = combo[3][2] + combo[0][2];
		m_clipPlanes[FRUSTUM_PLANE_LEFT].d = combo[3][3] + combo[0][3];

		m_clipPlanes[FRUSTUM_PLANE_RIGHT].normal.x = combo[3][0] - combo[0][0];
		m_clipPlanes[FRUSTUM_PLANE_RIGHT].normal.y = combo[3][1] - combo[0][1];
		m_clipPlanes[FRUSTUM_PLANE_RIGHT].normal.z = combo[3][2] - combo[0][2];
		m_clipPlanes[FRUSTUM_PLANE_RIGHT].d = combo[3][3] - combo[0][3];

		m_clipPlanes[FRUSTUM_PLANE_TOP].normal.x = combo[3][0] - combo[1][0];
		m_clipPlanes[FRUSTUM_PLANE_TOP].normal.y = combo[3][1] - combo[1][1];
		m_clipPlanes[FRUSTUM_PLANE_TOP].normal.z = combo[3][2] - combo[1][2];
		m_clipPlanes[FRUSTUM_PLANE_TOP].d = combo[3][3] - combo[1][3];

		m_clipPlanes[FRUSTUM_PLANE_BOTTOM].normal.x = combo[3][0] + combo[1][0];
		m_clipPlanes[FRUSTUM_PLANE_BOTTOM].normal.y = combo[3][1] + combo[1][1];
		m_clipPlanes[FRUSTUM_PLANE_BOTTOM].normal.z = combo[3][2] + combo[1][2];
		m_clipPlanes[FRUSTUM_PLANE_BOTTOM].d = combo[3][3] + combo[1][3];

		m_clipPlanes[FRUSTUM_PLANE_NEAR].normal.x = combo[3][0] + combo[2][0];
		m_clipPlanes[FRUSTUM_PLANE_NEAR].normal.y = combo[3][1] + combo[2][1];
		m_clipPlanes[FRUSTUM_PLANE_NEAR].normal.z = combo[3][2] + combo[2][2];
		m_clipPlanes[FRUSTUM_PLANE_NEAR].normal.normalize();
		m_clipPlanes[FRUSTUM_PLANE_NEAR].d = 
			-(m_clipPlanes[FRUSTUM_PLANE_NEAR].normal.dotProduct(m_vPosition) + m_fNearDistance);

		m_clipPlanes[FRUSTUM_PLANE_FAR].normal.x = combo[3][0] - combo[2][0];
		m_clipPlanes[FRUSTUM_PLANE_FAR].normal.y = combo[3][1] - combo[2][1];
		m_clipPlanes[FRUSTUM_PLANE_FAR].normal.z = combo[3][2] - combo[2][2];
		m_clipPlanes[FRUSTUM_PLANE_FAR].normal.normalize();
		m_clipPlanes[FRUSTUM_PLANE_FAR].d = 
			-(m_clipPlanes[FRUSTUM_PLANE_FAR].normal.dotProduct(m_vPosition) - m_fFarDistance);

		// Renormalise any normals which were not unit length (not near & far)
		for(int i = 2; i < 6;i++ ) 
		{
			float length = m_clipPlanes[i].normal.normalize();
			m_clipPlanes[i].d /= length;
		}
	}

	void  Camera::calcCorners()
	{
		// Update worldspace corners
		Matrix4 eyeToWorld = m_mtxView.inverse();
		// Get worldspace frustum corners
		float farDist = m_fFarDistance;
		float y = Math::Tan(m_radFOVy * 0.5);
		float x = getAspect() * y;
		float neary = y * m_fNearDistance;
		float fary = y * farDist;
		float nearx = x * m_fNearDistance;
		float farx = x * farDist;
		// near
		m_corners[0] = eyeToWorld * Vector3( nearx,  neary, -m_fNearDistance);
		m_corners[1] = eyeToWorld * Vector3(-nearx,  neary, -m_fNearDistance);
		m_corners[2] = eyeToWorld * Vector3(-nearx, -neary, -m_fNearDistance);
		m_corners[3] = eyeToWorld * Vector3( nearx, -neary, -m_fNearDistance);
		// far
		m_corners[4] = eyeToWorld * Vector3( farx,  fary, -farDist);
		m_corners[5] = eyeToWorld * Vector3(-farx,  fary, -farDist);
		m_corners[6] = eyeToWorld * Vector3(-farx, -fary, -farDist);
		m_corners[7] = eyeToWorld * Vector3( farx, -fary, -farDist);
	}

	void  Camera::calcBoundingSphere()
	{
		if(Math::RealEqual(m_fNearDistance,m_fFarDistance))
		{
			return;
		}

		float fDelta = m_fSphereDistance / (m_fFarDistance - m_fNearDistance);
		Vector3 v4 = m_vPosition + (m_corners[4] - m_vPosition) * fDelta;
		Vector3 v6 = m_vPosition + (m_corners[6] - m_vPosition) * fDelta;

		Vector3 vCenter = v4.midPoint(v6);
		vCenter = vCenter.midPoint(m_vPosition);

		float fRadius = (vCenter - v4).length();

		m_boundingSphere.setCenter(vCenter);
		m_boundingSphere.setRadius(fRadius);
	}

	void  Camera::update()
	{
		if(m_bNeedUpdate)
		{
			calcViewMatrix();
			calcProjectionMatrix();
			calcClipPlanes();
			calcCorners();
			calcBoundingSphere();

			m_bNeedUpdate = false;
		}
	}
//+		[3]	{x=0.33231947 y=0.40338248 z=31.004391 ...}	const xs::Vector3
//+		[1]	{x=-0.33145511 y=0.42499629 z=31.004391 ...}	const xs::Vector3
//+		[2]	{x=-0.33145124 y=-0.40338248 z=30.995440 ...}	const xs::Vector3
//+		[3]	{x=0.33231947 y=-0.40338248 z=30.995726 ...}	const xs::Vector3
//+		[4]	{x=10.614034 y=13.574219 z=0.20980540 ...}	const xs::Vector3
//+		[5]	{x=-10.586550 y=13.574219 z=0.20064236 ...}	const xs::Vector3
//+		[6]	{x=-10.586427 y=-12.883882 z=-0.085304566 ...}	const xs::Vector3
//+		[7]	{x=10.614158 y=-12.883882 z=-0.076141529 ...}	const xs::Vector3

	//-----------------------------------------------------------------------
	const Vector3*	Camera::getCorners() const
	{
		return m_corners;
	}

	//-----------------------------------------------------------------------
	const Plane& Camera::getFrustumPlane(ushort plane) const
	{
		return m_clipPlanes[plane];
	}

	//-----------------------------------------------------------------------
	bool Camera::isVisible(const AABB& bound, FrustumPlane* culledBy) const
	{
		// Null boxes always invisible
		if (bound.isNull()) return false;

		// Get corners of the box
		const Vector3* pCorners = bound.getAllCorners();


		// For each plane, see if all points are on the negative side
		// If so, object is not visible
		for (int plane = 0; plane < 6; ++plane)
		{
			// Skip far plane if infinite view frustum
			if (m_fFarDistance == Math::inf() && plane == FRUSTUM_PLANE_FAR)
				continue;

			if (m_clipPlanes[plane].getSide(pCorners[0]) == Plane::NEGATIVE_SIDE &&
				m_clipPlanes[plane].getSide(pCorners[1]) == Plane::NEGATIVE_SIDE &&
				m_clipPlanes[plane].getSide(pCorners[2]) == Plane::NEGATIVE_SIDE &&
				m_clipPlanes[plane].getSide(pCorners[3]) == Plane::NEGATIVE_SIDE &&
				m_clipPlanes[plane].getSide(pCorners[4]) == Plane::NEGATIVE_SIDE &&
				m_clipPlanes[plane].getSide(pCorners[5]) == Plane::NEGATIVE_SIDE &&
				m_clipPlanes[plane].getSide(pCorners[6]) == Plane::NEGATIVE_SIDE &&
				m_clipPlanes[plane].getSide(pCorners[7]) == Plane::NEGATIVE_SIDE)
			{
				// ALL corners on negative side therefore out of view
				if (culledBy)
					*culledBy = (FrustumPlane)plane;
				return false;
			}

		}

		return true;
	}

	void 	Camera::setSphereCullingDistance(float fDistance)
	{
		ASSERT(fDistance > 0);
		m_fSphereDistance = fDistance;
	}

	bool Camera::isVisibleViaSphere(const AABB& bound) const
	{
		return m_boundingSphere.intersects(bound);
	}

	//-----------------------------------------------------------------------
	IntersectionType Camera::intersectAABB(const AABB& bound) const
	{
		// Null boxes always invisible
		if (bound.isNull()) return INTERSECTION_FULL_OUTSIDE;

		// Get corners of the box
		const Vector3* pCorners = bound.getAllCorners();

		uint ui32InsideNum = 0;

		// For each plane, see if all points are on the negative side
		// If so, object is not visible
		for (int plane = 0; plane < 6; ++plane)
		{
			// Skip far plane if infinite view frustum
			if (m_fFarDistance == Math::inf() && plane == FRUSTUM_PLANE_FAR)
			{
				ui32InsideNum++;
				continue;
			}

			Plane::Side side[8];
			bool bFullOutside = true;
			bool bFullInside = true;
			for(int corner = 0;corner < 8;corner++)
			{
				side[corner] = m_clipPlanes[plane].getSide(pCorners[corner]);
				bFullOutside &= (side[corner] == Plane::NEGATIVE_SIDE);
				bFullInside &= (side[corner] == Plane::POSITIVE_SIDE);
			}
			if(bFullOutside)
			{
				// ALL corners on negative side therefore out of view
				return INTERSECTION_FULL_OUTSIDE;
			}
			if(bFullInside)
			{
				ui32InsideNum++;
			}
		}

		return ui32InsideNum == 6 ? INTERSECTION_FULL_INSIDE : INTERSECTION_PARTIAL_INSIDE;
	}

	//-----------------------------------------------------------------------
	bool Camera::isVisible(const Vector3& vert, FrustumPlane* culledBy) const
	{
		// For each plane, see if all points are on the negative side
		// If so, object is not visible
		for (int plane = 0; plane < 6; ++plane)
		{
			// Skip far plane if infinite view frustum
			if (m_fFarDistance == Math::inf() && plane == FRUSTUM_PLANE_FAR)
				continue;

			if (m_clipPlanes[plane].getSide(vert) == Plane::NEGATIVE_SIDE)
			{
				// ALL corners on negative side therefore out of view
				if (culledBy)
					*culledBy = (FrustumPlane)plane;
				return false;
			}

		}

		return true;
	}

	//-----------------------------------------------------------------------
	bool Camera::isVisible(const Sphere& sphere, FrustumPlane* culledBy) const
	{
		// For each plane, see if sphere is on negative side
		// If so, object is not visible
		for (int plane = 0; plane < 6; ++plane)
		{
			// Skip far plane if infinite view frustum
			if (m_fFarDistance == Math::inf() && plane == FRUSTUM_PLANE_FAR)
				continue;

			// If the distance from sphere center to plane is negative, and 'more negative' 
			// than the radius of the sphere, sphere is outside frustum
			if (m_clipPlanes[plane].getDistance(sphere.getCenter()) < -sphere.getRadius())
			{
				// ALL corners on negative side therefore out of view
				if (culledBy)
					*culledBy = (FrustumPlane)plane;
				return false;
			}

		}

		return true;
	}

	Ray Camera::getCameraToViewportRay(float x,float y) const
	{
		float screenX = x / m_pViewport->getWidth();
		float screenY = y / m_pViewport->getHeight();
        float centeredScreenX = (screenX - 0.5f);
        float centeredScreenY = (0.5f - screenY);

		float normalizedSlope = Math::Tan(m_radFOVy / 2);
        float viewportYToWorldY = normalizedSlope * m_fNearDistance * 2;
        float viewportXToWorldX = viewportYToWorldY * getAspect();

		Vector3 rayDirection, rayOrigin;

		// From camera centre
		rayOrigin = m_vPosition;
		// Point to perspective projected position
		rayDirection.x = centeredScreenX * viewportXToWorldX;
		rayDirection.y = centeredScreenY * viewportYToWorldY;
		rayDirection.z = -m_fNearDistance;
		rayDirection = m_qtnOrientation * rayDirection;
		rayDirection.normalize();

        return Ray(rayOrigin, rayDirection);
    } 

	Vector3 Camera::getDirection(void ) const
    {
        // Direction points down -Z by default
        return m_qtnOrientation * -Vector3::UNIT_Z;
    }

    //-----------------------------------------------------------------------
    Vector3 Camera::getUp(void ) const
    {
        return m_qtnOrientation * Vector3::UNIT_Y;
    }

    //-----------------------------------------------------------------------
    Vector3 Camera::getRight(void ) const
    {
        return m_qtnOrientation * Vector3::UNIT_X;
    }

	void  Camera::roll(const Radian& angle)
	{
        // Rotate around local Z axis
        Vector3 zAxis = m_qtnOrientation * Vector3::UNIT_Z;
        rotate(zAxis,angle);
    }

	void  Camera::yaw(const Radian& angle)
	{
        Vector3 yAxis;
        // Rotate around local Y axis
        yAxis = m_qtnOrientation * Vector3::UNIT_Y;

        rotate(yAxis, angle);
    }

	void  Camera::pitch(const Radian& angle)
	{
        // Rotate around local X axis
        Vector3 xAxis = m_qtnOrientation * Vector3::UNIT_X;
        rotate(xAxis,angle);
    }

	void  Camera::rotate(const Vector3& axis, const Radian& angle)
	{
        Quaternion q;
        q.FromAngleAxis(angle,axis);
        rotate(q);
    }

	void  Camera::rotate(const Quaternion& q)
	{
        // Note the order of the mult, i.e. q comes after
        m_qtnOrientation = q * m_qtnOrientation;

		m_bNeedUpdate = true;
    }

	const Quaternion& Camera::getOrientation(void ) const
	{
        return m_qtnOrientation;
    }

	void  Camera::setOrientation(const Quaternion& q)
	{
        m_qtnOrientation = q;
		m_bNeedUpdate = true;
    }

	void  Camera::move(const Vector3& vec)
    {
        m_vPosition = m_vPosition + vec;

		m_bNeedUpdate = true;
    }

    //-----------------------------------------------------------------------
    void  Camera::moveRelative(const Vector3& vec)
    {
        // Transform the axes of the relative vector by camera's local axes
        Vector3 trans = m_qtnOrientation * vec;

        m_vPosition = m_vPosition + trans;
		m_bNeedUpdate = true;
    }

	void  Camera::setNeedUpdate(bool bUpdate)
	{
		m_bNeedUpdate = bUpdate;
	}
}