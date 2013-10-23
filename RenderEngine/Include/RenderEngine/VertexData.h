#ifndef __VertexData_H__
#define __VertexData_H__

namespace xs
{
	/*! \addtogroup RenderEngine
	*  ‰÷»æ“˝«Ê
	*  @{
	*/


	/// Vertex element semantics, used to identify the meaning of vertex buffer contents
	enum VertexElementSemantic 
	{
		/// Position, 3 reals per vertex
		VES_POSITION = 1,
		/// Blending weights
		VES_BLEND_WEIGHTS = 2,
		/// Blending indices
		VES_BLEND_INDICES = 3,
		/// Normal, 3 reals per vertex
		VES_NORMAL = 4,
		/// Diffuse colours
		VES_DIFFUSE = 5,
		/// Specular colours
		VES_SPECULAR = 6,
		/// Texture coordinates
		VES_TEXTURE_COORDINATES = 7,
		/// Binormal (Y axis if normal is Z)
		VES_BINORMAL = 8,
		/// Tangent (X axis if normal is Z)
		VES_TANGENT = 9

	};

	/// Vertex element type, used to identify the base types of the vertex contents
	enum VertexElementType
	{
		VET_FLOAT1,
		VET_FLOAT2,
		VET_FLOAT3,
		VET_FLOAT4,
		VET_COLOR,
		VET_SHORT1,
		VET_SHORT2,
		VET_SHORT3,
		VET_SHORT4, 
		VET_UBYTE4
	};

	/** This class declares the usage of a single vertex buffer as a component
	of a complete VertexDeclaration. 
	@remarks
	Several vertex buffers can be used to supply the input geometry for a
	rendering operation, and in each case a vertex buffer can be used in
	different ways for different operations; the buffer itself does not
	define the semantics (position, normal etc), the VertexElement
	class does.
	*/
	class _RenderEngineExport VertexElement
	{
	protected:
		/// The source vertex buffer, as bound to an index using VertexBufferBinding
		ushort mSource;
		/// The offset in the buffer that this element starts at
		uint mOffset;
		/// The type of element
		VertexElementType mType;
		/// The meaning of the element
		VertexElementSemantic mSemantic;
		/// Index of the item, only applicable for some elements like texture coords
		ushort mIndex;
	public:
		/// Constructor, should not be called directly, call VertexDeclaration::addElement
		VertexElement(ushort source, uint offset, VertexElementType theType,
			VertexElementSemantic semantic, ushort index = 0);
		/// Gets the vertex buffer index from where this element draws it's values
		ushort getSource(void ) const { return mSource; }
		/// Gets the offset into the buffer where this element starts
		uint getOffset(void ) const 
		{ 
			return mOffset; 
		}
		/// Gets the data format of this element
		VertexElementType getType(void ) const { return mType; }
		/// Gets the meaning of this element
		VertexElementSemantic getSemantic(void ) const { return mSemantic; }
		/// Gets the index of this element, only applicable for repeating elements
		ushort getIndex(void ) const { return mIndex; }
		/// Gets the size of this element in bytes
		uint getSize(void ) const;
		/// Utility method for helping to calculate offsets
		static uint getTypeSize(VertexElementType etype);
		/// Utility method which returns the count of values in a given type
		static ushort getTypeCount(VertexElementType etype);
		/** Simple converter function which will turn a single-value type into a
		multi-value type based on a parameter. 
		*/
		static VertexElementType multiplyTypeCount(VertexElementType baseType, ushort count);
		/** Simple converter function which will a type into it's single-value
		equivalent - makes switches on type easier. 
		*/
		static VertexElementType getBaseType(VertexElementType multiType);

		inline bool operator== (const VertexElement& rhs) const
		{
			if (mType != rhs.mType || 
				mIndex != rhs.mIndex ||
				mOffset != rhs.mOffset ||
				mSemantic != rhs.mSemantic ||
				mSource != rhs.mSource)
				return false;
			else
				return true;

		}
		/** Adjusts a pointer to the base of a vertex to point at this element.
		@remarks
		This variant is for void  pointers, passed as a parameter because we can't
		rely on covariant return types.
		@param pBase Pointer to the start of a vertex in this buffer.
		@param pElem Pointer to a pointer which will be set to the start of this element.
		*/
		inline void  baseVertexPointerToElement(void * pBase, void ** pElem) const
		{
			// The only way we can do this is to cast to char* in order to use uchar offset
			// then cast back to void *.
			*pElem = static_cast<void *>(
				static_cast<uchar*>(pBase) + mOffset);
		}
		/** Adjusts a pointer to the base of a vertex to point at this element.
		@remarks
		This variant is for float pointers, passed as a parameter because we can't
		rely on covariant return types.
		@param pBase Pointer to the start of a vertex in this buffer.
		@param pElem Pointer to a pointer which will be set to the start of this element.
		*/
		inline void  baseVertexPointerToElement(void * pBase, float** pElem) const
		{
			// The only way we can do this is to cast to char* in order to use uchar offset
			// then cast back to float*. However we have to go via void * because casting  
			// directly is not allowed
			*pElem = static_cast<float*>(
				static_cast<void *>(
				static_cast<uchar*>(pBase) + mOffset));
		}

		/** Adjusts a pointer to the base of a vertex to point at this element.
		@remarks
		This variant is for RGBA pointers, passed as a parameter because we can't
		rely on covariant return types.
		@param pBase Pointer to the start of a vertex in this buffer.
		@param pElem Pointer to a pointer which will be set to the start of this element.
		*/
		inline void  baseVertexPointerToElement(void * pBase, RGBA** pElem) const
		{
			*pElem = static_cast<RGBA*>(
				static_cast<void *>(
				static_cast<uchar*>(pBase) + mOffset));
		}
		/** Adjusts a pointer to the base of a vertex to point at this element.
		@remarks
		This variant is for char pointers, passed as a parameter because we can't
		rely on covariant return types.
		@param pBase Pointer to the start of a vertex in this buffer.
		@param pElem Pointer to a pointer which will be set to the start of this element.
		*/
		inline void  baseVertexPointerToElement(void * pBase, uchar** pElem) const
		{
			*pElem = static_cast<uchar*>(pBase) + mOffset;
		}

		/** Adjusts a pointer to the base of a vertex to point at this element.
		@remarks
		This variant is for ushort pointers, passed as a parameter because we can't
		rely on covariant return types.
		@param pBase Pointer to the start of a vertex in this buffer.
		@param pElem Pointer to a pointer which will be set to the start of this element.
		*/
		inline void  baseVertexPointerToElement(void * pBase, ushort** pElem) const
		{
			*pElem = static_cast<ushort*>(pBase) + mOffset;
		}


	};

	/** This class declares the format of a set of vertex inputs, which
	can be issued to the rendering API through a RenderOperation. 
	@remarks
	You should be aware that the ordering and structure of the 
	VertexDeclaration can be very important on DirectX with older
	cards,so if you want to maintain maximum compatibility with 
	all render systems and all cards you should be careful to follow these
	rules:<ol>
	<li>VertexElements should be added in the following order, and the order of the
	elements within a shared buffer should be as follows: 
	position, blending weights, normals, diffuse colours, specular colours, 
	texture coordinates (in order, with no gaps)</li>
	<li>You must not have unused gaps in your buffers which are not referenced
	by any VertexElement</li>
	<li>You must not cause the buffer & offset settings of 2 VertexElements to overlap</li>
	</ol>
	Whilst GL and more modern graphics cards in D3D will allow you to defy these rules, 
	sticking to them will ensure that your buffers have the maximum compatibility. 
	@par
	Like the other classes in this functional area, these declarations should be created and
	destroyed using the HardwareBufferManager.
	*/
	class _RenderEngineExport VertexDeclaration
	{
	public:
		/// Defines the list of vertex elements that makes up this declaration
		typedef std::list<VertexElement> VertexElementList;
		/// Sort routine for vertex elements
		static bool vertexElementLess(const VertexElement& e1, const VertexElement& e2);
	protected:
		VertexElementList mElementList;
	public:
		/// Standard constructor, not you should use HardwareBufferManager::createVertexDeclaration
		VertexDeclaration();
		virtual ~VertexDeclaration();

		/** Get the number of elements in the declaration. */
		uint getElementCount(void ) { return mElementList.size(); }
		/** Gets read-only access to the list of vertex elements. */
		const VertexElementList& getElements(void ) const;
		/** Get a single element. */
		const VertexElement* getElement(ushort index);

		/** Sorts the elements in this list to be compatible with the maximum
		number of rendering APIs / graphics cards.
		@remarks
		Older graphics cards require vertex data to be presented in a more
		rigid way, as defined in the main documentation for this class. As well
		as the ordering being important, where shared source buffers are used, the
		declaration must list all the elements for each source in turn.
		*/
		void  sort(void );

		/** Remove any gaps in the source buffer list used by this declaration.
		@remarks
		This is useful if you've modified a declaration and want to remove
		any gaps in the list of buffers being used. Note, however, that if this
		declaration is already being used with a VertexBufferBinding, you will
		need to alter that too. This method is mainly useful when reorganising
		buffers based on an altered declaration.
		@note
		This will cause the vertex declaration to be re-sorted.
		*/
		void  closeGapsInSource(void );

		/** Generates a new VertexDeclaration for optimal usage based on the current
		vertex declaration, which can be used with VertexData::reorganiseBuffers later 
		if you wish, or simply used as a template.
		@param animated Whether this vertex data is going to be animated; this
		affects the choice of both usage and buffer splits.
		*/
		VertexDeclaration* getAutoOrganisedDeclaration(bool animated);

		/** Gets the indeex of the highest source value referenced by this declaration. */
		ushort getMaxSource(void ) const;



		/** Adds a new VertexElement to this declaration. 
		@remarks
		This method adds a single element (positions, normals etc) to the end of the
		vertex declaration. <b>Please read the information in VertexDeclaration about
		the importance of ordering and structure for compatibility with older D3D drivers</b>.
		@param source The binding index of HardwareVertexBuffer which will provide the source for this element.
		See VertexBufferBindingState for full information.
		@param offset The offset in bytes where this element is located in the buffer
		@param theType The data format of the element (3 Floats, a colour etc)
		@param semantic The meaning of the data (position, normal, diffuse colour etc)
		@param index Optional index for multi-input elements like texture coordinates
		@returns A reference to the VertexElement added.
		*/
		virtual const VertexElement& addElement(ushort source, uint offset, VertexElementType theType,
			VertexElementSemantic semantic, ushort index = 0);
		/** Inserts a new VertexElement at a given position in this declaration. 
		@remarks
		This method adds a single element (positions, normals etc) at a given position in this
		vertex declaration. <b>Please read the information in VertexDeclaration about
		the importance of ordering and structure for compatibility with older D3D drivers</b>.
		@param source The binding index of HardwareVertexBuffer which will provide the source for this element.
		See VertexBufferBindingState for full information.
		@param offset The offset in bytes where this element is located in the buffer
		@param theType The data format of the element (3 Floats, a colour etc)
		@param semantic The meaning of the data (position, normal, diffuse colour etc)
		@param index Optional index for multi-input elements like texture coordinates
		@returns A reference to the VertexElement added.
		*/
		virtual const VertexElement& insertElement(ushort atPosition,
			ushort source, uint offset, VertexElementType theType,
			VertexElementSemantic semantic, ushort index = 0);

		/** Remove the element at the given index from this declaration. */
		virtual void  removeElement(ushort elem_index);

		/** Remove the element with the given semantic and usage index. 
		@remarks
		In this case 'index' means the usage index for repeating elements such
		as texture coordinates. For other elements this will always be 0 and does
		not refer to the index in the vector.
		*/
		virtual void  removeElement(VertexElementSemantic semantic, ushort index = 0);

		/** Modify an element in-place, params as addElement. 
		@remarks
		<b>Please read the information in VertexDeclaration about
		the importance of ordering and structure for compatibility with older D3D drivers</b>.
		*/
		virtual void  modifyElement(ushort elem_index, ushort source, uint offset, VertexElementType theType,
			VertexElementSemantic semantic, ushort index = 0);

		/** Finds a VertexElement with the given semantic, and index if there is more than 
		one element with the same semantic. 
		@remarks
		If the element is not found, this method returns null.
		*/
		virtual const VertexElement* findElementBySemantic(VertexElementSemantic sem, ushort index = 0);
		/** Based on the current elements, gets the size of the vertex for a given buffer source. 
		@param source The buffer binding index for which to get the vertex size.
		*/

		/** Gets a list of elements which use a given source. 
		@remarks
		Note that the list of elements is returned by value therefore is separate from
		the declaration as soon as this method returns. 
		*/
		virtual VertexElementList findElementsBySource(ushort source);

		/** Gets the vertex size defined by this declaration for a given source. */
		virtual uint getVertexSize(ushort source);

		/** Clones this declaration. */
		virtual VertexDeclaration* clone(void );

		inline bool operator== (const VertexDeclaration& rhs) const
		{
			if (mElementList.size() != rhs.mElementList.size())
				return false;

			VertexElementList::const_iterator i, iend, rhsi, rhsiend;
			iend = mElementList.end();
			rhsiend = rhs.mElementList.end();
			rhsi = rhs.mElementList.begin();
			for (i = mElementList.begin(); i != iend && rhsi != rhsiend; ++i, ++rhsi)
			{
				if ( !(*i == *rhsi) )
					return false;
			}

			return true;
		}
		inline bool operator!= (const VertexDeclaration& rhs) const
		{
			return !(*this == rhs);
		}

	};

	/** Records the state of all the vertex buffer bindings required to provide a vertex declaration
	with the input data it needs for the vertex elements.
	@remarks
	Why do we have this binding list rather than just have VertexElement referring to the
	vertex buffers direct? Well, in the underlying APIs, binding the vertex buffers to an
	index (or 'stream') is the way that vertex data is linked, so this structure better
	reflects the realities of that. In addition, by separating the vertex declaration from
	the list of vertex buffer bindings, it becomes possible to reuse bindings between declarations
	and vice versa, giving opportunities to reduce the state changes required to perform rendering.
	@par
	Like the other classes in this functional area, these binding maps should be created and
	destroyed using the HardwareBufferManager.
	*/
	class _RenderEngineExport VertexBufferBinding
	{
	public:
		/// Defines the vertex buffer bindings used as source for vertex declarations
		typedef std::map<ushort, IVertexBuffer*> VertexBufferBindingMap;
	protected:
		VertexBufferBindingMap mBindingMap;
		mutable ushort mHighIndex;
	public:
		/// Constructor, should not be called direct, use HardwareBufferManager::createVertexBufferBinding
		VertexBufferBinding();
		virtual ~VertexBufferBinding();

		/** Set a binding, associating a vertex buffer with a given index. 
		@remarks
		If the index is already associated with a vertex buffer, 
		the association will be replaced. This may cause the old buffer
		to be destroyed if nothing else is referring to it.
		You should assign bindings from 0 and not leave gaps, although you can
		bind them in any order.
		*/
		virtual void  setBinding(ushort index, IVertexBuffer* buffer);
		/** Removes an existing binding. */
		virtual void  unsetBinding(ushort index);

		/** Removes all the bindings. */
		virtual void  unsetAllBindings(void );

		/// Gets a read-only version of the buffer bindings
		virtual const VertexBufferBindingMap& getBindings(void ) const;

		/// Gets the buffer bound to the given source index
		virtual IVertexBuffer* getBuffer(ushort index);

		virtual uint getBufferCount(void ) const { return mBindingMap.size(); }

		/** Gets the highest index which has already been set, plus 1.
		@remarks
		This is to assist in binding the vertex buffers such that there are
		not gaps in the list.
		*/
		virtual ushort getNextIndex(void ) const { return mHighIndex++; }

	};

	/** Summary class collecting together vertex source information. */
	class _RenderEngineExport VertexData
	{
	private:
		/// Protected copy constructor, to prevent misuse
		VertexData(const VertexData& rhs); /* do nothing, should not use */
		/// Protected operator=, to prevent misuse
		VertexData& operator=(const VertexData& rhs); /* do not use */
	public:
		VertexData();
		~VertexData();

		/** Declaration of the vertex to be used in this operation. 
		@remarks Note that this is created for you on construction.
		*/
		VertexDeclaration* vertexDeclaration;
		/** The vertex buffer bindings to be used. 
		@remarks Note that this is created for you on construction.
		*/
		VertexBufferBinding* vertexBufferBinding;
		/// The base vertex index to start from
		uint vertexStart;
		/// The number of vertices used in this operation
		uint vertexCount;

		/** Clones this vertex data, potentially including replicating any vertex buffers.
		@remarks The caller is expected to delete the returned pointer when ready
		*/
		VertexData* clone() const;

	};
	/** @} */

}
#endif

