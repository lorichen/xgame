#ifndef __Camera_H__
#define __Camera_H__

namespace xs
{
	/*! \addtogroup RenderEngine
	*  ‰÷»æ“˝«Ê
	*  @{
	*/

	/** Worldspace clipping planes.
	*/
	enum FrustumPlane
	{
		FRUSTUM_PLANE_NEAR   = 0,
		FRUSTUM_PLANE_FAR    = 1,
		FRUSTUM_PLANE_LEFT   = 2,
		FRUSTUM_PLANE_RIGHT  = 3,
		FRUSTUM_PLANE_TOP    = 4,
		FRUSTUM_PLANE_BOTTOM = 5,
		FRUSTUM_PLANE_COUNT
	};

	enum IntersectionType
	{
		INTERSECTION_FULL_OUTSIDE,
		INTERSECTION_FULL_INSIDE,
		INTERSECTION_PARTIAL_INSIDE,
	};

	class Viewport;

	class _RenderEngineExport Camera
	{
	protected:
		Vector3			m_vPosition;
		Quaternion		m_qtnOrientation;
		Viewport*		m_pViewport;

		Radian	m_radFOVy;
		float	m_fNearDistance;
		float	m_fFarDistance;

		Matrix4 m_mtxProjection;
		Matrix4 m_mtxView;

		FillMode	m_fillMode;

		bool		m_bNeedUpdate;
		Plane		m_clipPlanes[FRUSTUM_PLANE_COUNT];
		Vector3		m_corners[8];
		Sphere		m_boundingSphere;
		float		m_fSphereDistance;
	public:
		void  calcViewMatrix();
		void  calcProjectionMatrix();
	public:
		void  release();
		/** Standard constructor.
		*/
		Camera(Viewport* pViewport);

		/** Standard destructor.
		*/
		~Camera();

		/** Sets the camera's position.
		*/
		void  setPosition(float x, float y, float z);

		/** Sets the camera's position.
		*/
		void  setPosition(const Vector3& vec);

		/** Retrieves the camera's position.
		*/
		const Vector3& getPosition(void ) const;

		/** Points the camera at a location in worldspace.
		@remarks
		This is a helper method to automatically generate the
		direction vector for the camera, based on it's current position
		and the supplied look-at point.
		@param
		targetPoint A vector specifying the look at point.
		*/
		void  lookAt(const Vector3& vLookat);
		/** Points the camera at a location in worldspace.
		@remarks
		This is a helper method to automatically generate the
		direction vector for the camera, based on it's current position
		and the supplied look-at point.
		@param
		x
		@param
		y
		@param
		z Co-ordinates of the point to look at.
		*/
		void  lookAt(float x,float y,float z);

		void  setDirection(const Vector3& v);

		//void  calculateViewMatrix();
		//void  calculateProjectionMatrix();
		const Matrix4& getViewMatrix();
		const Matrix4& getProjectionMatrix();

		void  setFOVy(Radian fovy);
		void  setNearDistance(float fNearDistance);
		void  setFarDistance(float fFarDistance);

		Radian getFOVy();
		float getNearDistance();
		float getFarDistance();
		float getAspect()const;

		void 	setFillMode(const FillMode& sdl);
		const FillMode&	getFillMode();

		void 			setViewport(Viewport* pViewport);
		Viewport*		getViewport();

		void 			update();
		void 			calcClipPlanes();
		void 			calcCorners();
		void 			calcBoundingSphere();

		void 			setNeedUpdate(bool bUpdate);
	public:
		const Vector3*	getCorners() const;
		/** Retrieves a specified plane of the frustum.
		@remarks
		Gets a reference to one of the planes which make up the frustum frustum, e.g. for clipping purposes.
		*/
		const Plane& getFrustumPlane( ushort plane ) const;

		/** Tests whether the given container is visible in the Frustum.
		@param
		bound Bounding box to be checked
		@param
		culledBy Optional pointer to an int which will be filled by the plane number which culled
		the box if the result was false;
		@returns
		If the box was visible, true is returned.
		@par
		Otherwise, false is returned.
		*/
		bool isVisible(const AABB& bound, FrustumPlane* culledBy = 0) const;

		/** Tests whether the given container is visible in the Frustum.
		@param
		bound Bounding sphere to be checked
		@param
		culledBy Optional pointer to an int which will be filled by the plane number which culled
		the box if the result was false;
		@returns
		If the sphere was visible, true is returned.
		@par
		Otherwise, false is returned.
		*/
		bool isVisible(const Sphere& bound, FrustumPlane* culledBy = 0) const;

		/** Tests whether the given vertex is visible in the Frustum.
		@param
		vert Vertex to be checked
		@param
		culledBy Optional pointer to an int which will be filled by the plane number which culled
		the box if the result was false;
		@returns
		If the box was visible, true is returned.
		@par
		Otherwise, false is returned.
		*/
		bool isVisible(const Vector3& vert, FrustumPlane* culledBy = 0) const;

		/** Tests whether the given vertex is INISDE,OUTSIDE,PARTIAL_INSIDE in the Frustum.
		*/
		IntersectionType intersectAABB(const AABB& bound) const;

		void 	setSphereCullingDistance(float fDistance);
		/***/
		bool isVisibleViaSphere(const AABB& bound) const;
	public:
		Ray getCameraToViewportRay(float x,float y) const;

		Vector3 getDirection(void ) const;
		Vector3 getUp(void ) const;
		Vector3 getRight(void ) const;
	public:

		 /** Rolls the camera anticlockwise, around its local z axis.
        */
        void  roll(const Radian& angle);

        /** Rotates the camera anticlockwise around it's local y axis.
        */
        void  yaw(const Radian& angle);

        /** Pitches the camera up/down anticlockwise around it's local z axis.
        */
        void  pitch(const Radian& angle);

        /** Rotate the camera around an arbitrary axis.
        */
        void  rotate(const Vector3& axis, const Radian& angle);

        /** Rotate the camera around an aritrary axis using a Quarternion.
        */
        void  rotate(const Quaternion& q);

        /** Returns the camera's current orientation.
        */
        const Quaternion& getOrientation(void ) const;

        /** Sets the camera's orientation.
        */
        void  setOrientation(const Quaternion& q);

		/** Moves the camera's position by the vector offset provided along world axes.
        */
        void  move(const Vector3& vec);

        /** Moves the camera's position by the vector offset provided along it's own axes (relative to orientation).
        */
        void  moveRelative(const Vector3& vec);
	};
	/** @} */
}

#endif