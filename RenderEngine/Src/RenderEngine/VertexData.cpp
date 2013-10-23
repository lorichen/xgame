#include "StdAfx.h"
#include "VertexData.h"

namespace xs
{

	//-----------------------------------------------------------------------------
	VertexElement::VertexElement(ushort source, uint offset, 
		VertexElementType theType, VertexElementSemantic semantic, ushort index)
		: mSource(source), mOffset(offset), mType(theType), 
		mSemantic(semantic), mIndex(index)
	{
	}
	//-----------------------------------------------------------------------------
	uint VertexElement::getSize(void ) const
	{
		return getTypeSize(mType);
	}
	//-----------------------------------------------------------------------------
	uint VertexElement::getTypeSize(VertexElementType etype)
	{
		switch(etype)
		{
		case VET_COLOR:
			return sizeof(RGBA);
		case VET_FLOAT1:
			return sizeof(float);
		case VET_FLOAT2:
			return sizeof(float)*2;
		case VET_FLOAT3:
			return sizeof(float)*3;
		case VET_FLOAT4:
			return sizeof(float)*4;
		case VET_SHORT1:
			return sizeof(short);
		case VET_SHORT2:
			return sizeof(short)*2;
		case VET_SHORT3:
			return sizeof(short)*3;
		case VET_SHORT4:
			return sizeof(short)*4;
		case VET_UBYTE4:
			return sizeof(unsigned char)*4;
		}
		return 0;
	}
	//-----------------------------------------------------------------------------
	ushort VertexElement::getTypeCount(VertexElementType etype)
	{
		switch (etype)
		{
		case VET_COLOR:
			return 1;
		case VET_FLOAT1:
			return 1;
		case VET_FLOAT2:
			return 2;
		case VET_FLOAT3:
			return 3;
		case VET_FLOAT4:
			return 4;
		case VET_SHORT1:
			return 1;
		case VET_SHORT2:
			return 2;
		case VET_SHORT3:
			return 3;
		case VET_SHORT4:
			return 4;
		case VET_UBYTE4:
			return 4;
		}
		throw("Invalid type - VertexElement::getTypeCount");
	}
	//-----------------------------------------------------------------------------
	VertexElementType VertexElement::multiplyTypeCount(VertexElementType baseType, 
		ushort count)
	{
		switch (baseType)
		{
		case VET_FLOAT1:
			switch(count)
			{
			case 1:
				return VET_FLOAT1;
			case 2:
				return VET_FLOAT2;
			case 3:
				return VET_FLOAT3;
			case 4:
				return VET_FLOAT4;
			default:
				break;
			}
			break;
		case VET_SHORT1:
			switch(count)
			{
			case 1:
				return VET_SHORT1;
			case 2:
				return VET_SHORT2;
			case 3:
				return VET_SHORT3;
			case 4:
				return VET_SHORT4;
			default:
				break;
			}
			break;
		default:
			break;
		}
		throw("Invalid base type - VertexElement::multiplyTypeCount");
	}
	//-----------------------------------------------------------------------------
	VertexElementType VertexElement::getBaseType(VertexElementType multiType)
	{
		switch (multiType)
		{
		case VET_FLOAT1:
		case VET_FLOAT2:
		case VET_FLOAT3:
		case VET_FLOAT4:
			return VET_FLOAT1;
		case VET_COLOR:
			return VET_COLOR;
		case VET_SHORT1:
		case VET_SHORT2:
		case VET_SHORT3:
		case VET_SHORT4:
			return VET_SHORT1;
		case VET_UBYTE4:
			return VET_UBYTE4;
		};
		// To keep compiler happy
		return VET_FLOAT1;
	}
	//-----------------------------------------------------------------------------
	VertexDeclaration::VertexDeclaration()
	{
	}
	//-----------------------------------------------------------------------------
	VertexDeclaration::~VertexDeclaration()
	{
	}
	//-----------------------------------------------------------------------------
	const VertexDeclaration::VertexElementList& VertexDeclaration::getElements(void ) const
	{
		return mElementList;
	}
	//-----------------------------------------------------------------------------
	const VertexElement& VertexDeclaration::addElement(ushort source, 
		uint offset, VertexElementType theType,
		VertexElementSemantic semantic, ushort index)
	{
		mElementList.push_back(
			VertexElement(source, offset, theType, semantic, index)
			);
		return mElementList.back();
	}
	//-----------------------------------------------------------------------------
	const VertexElement& VertexDeclaration::insertElement(ushort atPosition,
		ushort source, uint offset, VertexElementType theType,
		VertexElementSemantic semantic, ushort index)
	{
		if (atPosition >= mElementList.size())
		{
			return addElement(source, offset, theType, semantic, index);
		}

		VertexElementList::iterator i = mElementList.begin();
		for (ushort n = 0; n < atPosition; ++n)
			++i;

		i = mElementList.insert(i, 
			VertexElement(source, offset, theType, semantic, index));
		return *i;

	}
	//-----------------------------------------------------------------------------
	const VertexElement* VertexDeclaration::getElement(ushort index)
	{
		assert(index < mElementList.size() && "Index out of bounds");

		VertexElementList::iterator i = mElementList.begin();
		for (ushort n = 0; n < index; ++n)
			++i;

		return &(*i);

	}
	//-----------------------------------------------------------------------------
	void  VertexDeclaration::removeElement(ushort elem_index)
	{
		assert(elem_index < mElementList.size() && "Index out of bounds");
		VertexElementList::iterator i = mElementList.begin();
		for (ushort n = 0; n < elem_index; ++n)
			++i;
		mElementList.erase(i);
	}
	//-----------------------------------------------------------------------------
	void  VertexDeclaration::removeElement(VertexElementSemantic semantic, ushort index)
	{
		VertexElementList::iterator ei, eiend;
		eiend = mElementList.end();
		for (ei = mElementList.begin(); ei != eiend; ++ei)
		{
			if (ei->getSemantic() == semantic && ei->getIndex() == index)
			{
				mElementList.erase(ei);
				break;
			}
		}
	}
	//-----------------------------------------------------------------------------
	void  VertexDeclaration::modifyElement(ushort elem_index, 
		ushort source, uint offset, VertexElementType theType,
		VertexElementSemantic semantic, ushort index)
	{
		assert(elem_index < mElementList.size() && "Index out of bounds");
		VertexElementList::iterator i = mElementList.begin();
		std::advance(i, elem_index);
		(*i) = VertexElement(source, offset, theType, semantic, index);
	}
	//-----------------------------------------------------------------------------
	const VertexElement* VertexDeclaration::findElementBySemantic(
		VertexElementSemantic sem, ushort index)
	{
		VertexElementList::const_iterator ei, eiend;
		eiend = mElementList.end();
		for (ei = mElementList.begin(); ei != eiend; ++ei)
		{
			if (ei->getSemantic() == sem && ei->getIndex() == index)
			{
				return &(*ei);
			}
		}

		return NULL;


	}
	//-----------------------------------------------------------------------------
	VertexDeclaration::VertexElementList VertexDeclaration::findElementsBySource(
		ushort source)
	{
		VertexElementList retList;
		VertexElementList::const_iterator ei, eiend;
		eiend = mElementList.end();
		for (ei = mElementList.begin(); ei != eiend; ++ei)
		{
			if (ei->getSource() == source)
			{
				retList.push_back(*ei);
			}
		}
		return retList;

	}

	//-----------------------------------------------------------------------------
	uint VertexDeclaration::getVertexSize(ushort source)
	{
		VertexElementList::const_iterator i, iend;
		iend = mElementList.end();
		uint sz = 0;

		for (i = mElementList.begin(); i != iend; ++i)
		{
			if (i->getSource() == source)
			{
				sz += i->getSize();

			}
		}
		return sz;
	}
	//-----------------------------------------------------------------------------
	VertexDeclaration* VertexDeclaration::clone(void )
	{
		VertexDeclaration* ret = getRenderEngine()->getRenderSystem()->getBufferManager()->createVertexDeclaration();

		VertexElementList::const_iterator i, iend;
		iend = mElementList.end();
		for (i = mElementList.begin(); i != iend; ++i)
		{
			ret->addElement(i->getSource(), i->getOffset(), i->getType(), i->getSemantic(), i->getIndex());
		}
		return ret;
	}
	//-----------------------------------------------------------------------------
	// Sort routine for VertexElement
	bool VertexDeclaration::vertexElementLess(const VertexElement& e1, const VertexElement& e2)
	{
		// Sort by source first
		if (e1.getSource() < e2.getSource())
		{
			return true;
		}
		else if (e1.getSource() == e2.getSource())
		{
			// Use ordering of semantics to sort
			if (e1.getSemantic() < e2.getSemantic())
			{
				return true;
			}
			else if (e1.getSemantic() == e2.getSemantic())
			{
				// Use index to sort
				if (e1.getIndex() < e2.getIndex())
				{
					return true;
				}
			}
		}
		return false;
	}
	void  VertexDeclaration::sort(void )
	{
		mElementList.sort(VertexDeclaration::vertexElementLess);
	}
	//-----------------------------------------------------------------------------
	void  VertexDeclaration::closeGapsInSource(void )
	{
		if (mElementList.empty())
			return;

		// Sort first
		sort();

		VertexElementList::iterator i, iend;
		iend = mElementList.end();
		ushort targetIdx = 0;
		ushort lastIdx = getElement(0)->getSource();
		ushort c = 0;
		for (i = mElementList.begin(); i != iend; ++i, ++c)
		{
			VertexElement& elem = *i;
			if (lastIdx != elem.getSource())
			{
				targetIdx++;
				lastIdx = elem.getSource();
			}
			if (targetIdx != elem.getSource())
			{
				modifyElement(c, targetIdx, elem.getOffset(), elem.getType(), 
					elem.getSemantic(), elem.getIndex());
			}

		}

	}
	//-----------------------------------------------------------------------
	VertexDeclaration* VertexDeclaration::getAutoOrganisedDeclaration(bool animated)
	{
		VertexDeclaration* newDecl = this->clone();
		// Set all sources to the same buffer (for now)
		const VertexDeclaration::VertexElementList& elems = newDecl->getElements();
		VertexDeclaration::VertexElementList::const_iterator i;
		ushort c = 0;
		for (i = elems.begin(); i != elems.end(); ++i, ++c)
		{
			const VertexElement& elem = *i;
			// Set source & offset to 0 for now, before sort
			newDecl->modifyElement(c, 0, 0, elem.getType(), elem.getSemantic(), elem.getIndex());
		}
		newDecl->sort();
		// Now sort out proper buffer assignments and offsets
		uint offset0 = 0;
		uint offset1 = 0;
		c = 0;
		for (i = elems.begin(); i != elems.end(); ++i, ++c)
		{
			const VertexElement& elem = *i;
			if (animated && 
				elem.getSemantic() != VES_POSITION && elem.getSemantic() != VES_NORMAL)
			{
				// Animated meshes have pos & norm in buffer 0, everything else in 1
				newDecl->modifyElement(c, 1, offset1, elem.getType(), elem.getSemantic(), elem.getIndex());
				offset1 += elem.getSize();
			}
			else
			{
				newDecl->modifyElement(c, 0, offset0, elem.getType(), elem.getSemantic(), elem.getIndex());
				offset0 += elem.getSize();
			}
		}

		return newDecl;


	}
	//-----------------------------------------------------------------------------
	ushort VertexDeclaration::getMaxSource(void ) const
	{
		VertexElementList::const_iterator i, iend;
		iend = mElementList.end();
		ushort ret = 0;
		for (i = mElementList.begin(); i != iend; ++i)
		{
			if (i->getSource() > ret)
			{
				ret = i->getSource();
			}

		}
		return ret;
	}
	//-----------------------------------------------------------------------------
	VertexBufferBinding::VertexBufferBinding() : mHighIndex(0)
	{
	}
	//-----------------------------------------------------------------------------
	VertexBufferBinding::~VertexBufferBinding()
	{
		unsetAllBindings();
	}
	//-----------------------------------------------------------------------------
	void  VertexBufferBinding::setBinding(ushort index, IVertexBuffer* buffer)
	{
		// NB will replace any existing buffer ptr at this index, and will thus cause
		// reference count to decrement on that buffer (possibly destroying it)
		mBindingMap[index] = buffer;
		mHighIndex = std::max(mHighIndex, (ushort)(index+1));
	}
	//-----------------------------------------------------------------------------
	void  VertexBufferBinding::unsetBinding(ushort index)
	{
		VertexBufferBindingMap::iterator i = mBindingMap.find(index);
		if (i == mBindingMap.end())
		{
			Error("Cannot find buffer binding for index "<<index);
			return;
		}
		mBindingMap.erase(i);
	}
	//-----------------------------------------------------------------------------
	void  VertexBufferBinding::unsetAllBindings(void )
	{
		mBindingMap.clear();
	}
	//-----------------------------------------------------------------------------
	const VertexBufferBinding::VertexBufferBindingMap& 
		VertexBufferBinding::getBindings(void ) const
	{
		return mBindingMap;
	}
	//-----------------------------------------------------------------------------
	IVertexBuffer* VertexBufferBinding::getBuffer(ushort index)
	{
		VertexBufferBindingMap::iterator i = mBindingMap.find(index);
		if (i == mBindingMap.end())
		{
			return 0;
		}
		return i->second;
	}

	//-----------------------------------------------------------------------
	VertexData::VertexData()
	{
		IBufferManager *pBufferManager = getRenderEngine()->getRenderSystem()->getBufferManager();
		vertexBufferBinding = pBufferManager->createVertexBufferBinding();
		vertexDeclaration = pBufferManager->createVertexDeclaration();
		vertexCount = 0;
		vertexStart = 0;
	}
	//-----------------------------------------------------------------------
	VertexData::~VertexData()
	{
		IBufferManager *pBufferManager = getRenderEngine()->getRenderSystem()->getBufferManager();
		pBufferManager->destroyVertexBufferBinding(vertexBufferBinding);
		pBufferManager->destroyVertexDeclaration(vertexDeclaration);
	}
	//-----------------------------------------------------------------------
	VertexData* VertexData::clone() const
	{
		VertexData* dest = new VertexData();

		// Copy vertex buffers in turn
		const VertexBufferBinding::VertexBufferBindingMap bindings = 
			this->vertexBufferBinding->getBindings();
		VertexBufferBinding::VertexBufferBindingMap::const_iterator vbi, vbend;
		vbend = bindings.end();
		for (vbi = bindings.begin(); vbi != vbend; ++vbi)
		{
			IVertexBuffer* srcbuf = vbi->second;
			IVertexBuffer* dstBuf;
			// create new buffer with the same settings
			dstBuf = 
				getRenderEngine()->getRenderSystem()->getBufferManager()->createVertexBuffer
				(
					srcbuf->getVertexSize(),
					srcbuf->getNumVertices(),
					srcbuf->getUsage()
				);

			// copy data
			uint sizeInBytes = srcbuf->getNumVertices() * srcbuf->getVertexSize();
			uchar* pData = (uchar*)dstBuf->lock(0,sizeInBytes,BL_DISCARD);
			dstBuf->writeData(0,sizeInBytes,pData);
			dstBuf->unlock();

			// Copy binding
			dest->vertexBufferBinding->setBinding(vbi->first,dstBuf);
		}

		// Basic vertex info
		dest->vertexStart = this->vertexStart;
		dest->vertexCount = this->vertexCount;
		// Copy elements
		const VertexDeclaration::VertexElementList elems = 
			this->vertexDeclaration->getElements();
		VertexDeclaration::VertexElementList::const_iterator ei, eiend;
		eiend = elems.end();
		for (ei = elems.begin(); ei != eiend; ++ei)
		{
			dest->vertexDeclaration->addElement(
				ei->getSource(),
				ei->getOffset(),
				ei->getType(),
				ei->getSemantic(),
				ei->getIndex() );
		}


		return dest;
	}
}
