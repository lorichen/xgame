
/*! \addtogroup SceneManager
*  场景管理
*  @{
*/
#ifndef __Tile_H__
#define __Tile_H__

#include "IEntityView.h"

typedef VariableLengthArray<DWORD,EntityView*>		LayerList;

#define TILE_MEM_TEST

#ifdef TILE_MEM_TEST
#define TILE_CHECK check();
#else
#define TILE_CHECK
#endif

class Tile
{
public:
	Tile() : m_dwFlag(0),m_pLayerList(0)
	{
	}
	~Tile()
	{
		close();
	}
	enum 
	{
		//逻辑对象占位
		tGroundBlock_Logic = 0x10000,						// 逻辑对象的行走占位
		tFlyBlock_Logic = 0x20000,							// 逻辑对象的飞行占位
		tSpreadBlock_Logic = 0x40000,						// 逻辑对象的漫延占位
		tValid = 0x0001,
		tGroundBlock_Surface = 0x0010,
		tGroundBlock_Entity =    0x2000,
		tGroundBlock = (tGroundBlock_Surface | tGroundBlock_Entity | tGroundBlock_Logic),
		tFlyBlock_Surface = 0x0020,
		tFlyBlock_Entity =    0x8000,
		tFlyBlock = (tFlyBlock_Surface | tFlyBlock_Entity | tFlyBlock_Logic),
		tSpreadBlock_Surface = 0x0040,
		tSpreadBlock_Entity =    0x4000,
		tSpreadBlock = (tSpreadBlock_Surface | tSpreadBlock_Entity | tSpreadBlock_Logic),
		tOccupant_Surface = (tGroundBlock_Surface | tFlyBlock_Surface | tSpreadBlock_Surface),
		tOccupant_Entity = (tGroundBlock_Entity | tFlyBlock_Entity | tSpreadBlock_Entity),
		tOccupant = tOccupant_Surface | tOccupant_Entity,
		tIsMapEntity = 0x2,
		tIsLayerList =  0xFFFFFFFD,
		fMostTileValue = tValid,
		fMostTileValueFlag = 0x04,


	};

	DWORD isBlock()const
	{
		return m_dwFlag & tGroundBlock;
	}
	DWORD isNotBlock()const
	{
		return !(m_dwFlag & tGroundBlock);
	}

	DWORD isFlyBlock()const
	{
		return m_dwFlag & tFlyBlock;
	}

	DWORD isSpreadBlock()const
	{
		return m_dwFlag & tSpreadBlock;
	}

	DWORD isGroundBlock_Surface()const
	{
		return m_dwFlag & tGroundBlock_Surface;
	}

	void set_GroundBlock_Surface(DWORD dwFlag = tGroundBlock_Surface)
	{
		TILE_CHECK
		m_dwFlag |= dwFlag & tGroundBlock_Surface;
		TILE_CHECK
	}

	void clear_GroundBlock_Surface()
	{
		TILE_CHECK
		m_dwFlag &= ~tGroundBlock_Surface;
		TILE_CHECK
	}

	DWORD isGroundBlock_Entity()const
	{
		return m_dwFlag & tGroundBlock_Entity;
	}

	void set_GroundBlock_Entity(DWORD dwFlag = tGroundBlock_Entity)
	{
		TILE_CHECK
		m_dwFlag |= dwFlag & tOccupant_Entity;
		TILE_CHECK
	}

	void clear_GroundBlock_Entity()
	{
		TILE_CHECK
		m_dwFlag &= ~tGroundBlock_Entity;
		TILE_CHECK
	}

	DWORD isFlyBlock_Surface()const
	{
		return m_dwFlag & tFlyBlock_Surface;
	}

	void set_FlyBlock_Surface(DWORD dwFlag = tFlyBlock_Surface)
	{
		TILE_CHECK
		m_dwFlag |= dwFlag & tFlyBlock_Surface;
		TILE_CHECK
	}

	void clear_FlyBlock_Surface()
	{
		TILE_CHECK
		m_dwFlag &= ~tFlyBlock_Surface;
		TILE_CHECK
	}

	DWORD isFlyBlock_Entity()const
	{
		return m_dwFlag & tFlyBlock_Entity;
	}

	void set_FlyBlock_Entity(DWORD dwFlag = tFlyBlock_Entity)
	{
		TILE_CHECK
		m_dwFlag |= dwFlag & tFlyBlock_Entity;
		TILE_CHECK
	}

	void clear_FlyBlock_Entity()
	{
		TILE_CHECK
		m_dwFlag &= ~tFlyBlock_Entity;
		TILE_CHECK
	}

	DWORD isSpreadBlock_Surface()const
	{
		return m_dwFlag & tSpreadBlock_Surface;
	}

	void set_SpreadBlock_Surface(DWORD dwFlag = tSpreadBlock_Surface)
	{
		TILE_CHECK
		m_dwFlag |= dwFlag & tSpreadBlock_Surface;
		TILE_CHECK
	}

	void clear_SpreadBlock_Surface()
	{
		TILE_CHECK
		m_dwFlag &= ~tSpreadBlock_Surface;
		TILE_CHECK
	}

	DWORD isSpreadBlock_Entity()const
	{
		return m_dwFlag & tSpreadBlock_Entity;
	}

	void set_SpreadBlock_Entity(DWORD dwFlag = tSpreadBlock_Entity)
	{
		TILE_CHECK
		m_dwFlag |= dwFlag & tSpreadBlock_Entity;
		TILE_CHECK
	}

	void clear_SpreadBlock_Entity()
	{
		TILE_CHECK
		m_dwFlag &= ~tSpreadBlock_Entity;
		TILE_CHECK
	}

	DWORD isOccupant_Surface()const
	{
		return m_dwFlag & tOccupant_Surface;
	}
	void set_Occupant_Surface(DWORD dwFlag)
	{
		TILE_CHECK
		m_dwFlag |= dwFlag & tOccupant_Surface;
		TILE_CHECK
	}
	void clear_Occupant_Surface()
	{
		TILE_CHECK
		m_dwFlag &= ~tOccupant_Surface;
		TILE_CHECK
	}

	DWORD isOccupant_Entity()const
	{
		return m_dwFlag & tOccupant_Entity;
	}
	void set_Occupant_Entity(DWORD dwFlag = tOccupant_Entity)
	{
		TILE_CHECK
		m_dwFlag |= dwFlag & tOccupant_Entity;
		TILE_CHECK
	}
	void clear_Occupant_Entity()
	{
		TILE_CHECK
		m_dwFlag &= ~tOccupant_Entity;
		TILE_CHECK
	}

	void set_Occupant_Entity_Byte(BYTE cbFlag)
	{
		TILE_CHECK
		m_dwFlag |= (((DWORD)cbFlag) << 8) & tOccupant_Entity;
		TILE_CHECK
	}
	void clear_Occupant_Entity_Byte(BYTE cbFlag = (BYTE)(tOccupant >> 8))
	{
		TILE_CHECK
		m_dwFlag &= ~(((DWORD)cbFlag) << 8);
		TILE_CHECK
	}


	EntityView* getLayer(int nIndex)
	{
		if(m_dwFlag & tIsMapEntity) // 此时只有一个结点，不理nIndex值
			return m_pMapEntity;
		else
		{
			if (nIndex < m_pLayerList->getCount())
				return (*m_pLayerList)[nIndex];
		}
		return NULL;
	}

	void onUpdateItems()
	{
		TILE_CHECK
		//Clear_Occupant_Item();
		int nCount = getLayerCount();
		for(int i = 0; i < nCount; i++)
			set_Occupant_Entity(getLayer(i)->getOccupantValue());
		TILE_CHECK
	}

	void removeEntityOccupant(DWORD dwFlag)
	{
		TILE_CHECK
		m_dwFlag &= ~dwFlag;
		TILE_CHECK

		// 此处根据函数名称，参照服务器移除站位信息的函数逻辑；移除逻辑只需修改恢复标识即可；无需再次设定
		// 阻挡信息；对于该处修改的可行性；有待后期的验证；（WZH 2010.7.26）
		//int nCount = getLayerCount();
		//for(int i = 0; i < nCount; i++)
		//	set_Occupant_Entity(getLayer(i)->getOccupantValue());
	}

	void setOneLayer(EntityView* pItem)
	{
		TILE_CHECK
        if (pItem == NULL)
        {
            ASSERT(false);
            return;
        }

        m_pMapEntity = pItem;
        m_dwFlag |= tIsMapEntity;
		TILE_CHECK
	}

	void setNoLayer()
	{
		TILE_CHECK
		m_pMapEntity = NULL;
		m_dwFlag &= tIsLayerList;
		TILE_CHECK
	}

	int getLayerCount() const
	{
		if(m_pMapEntity == 0)
			return 0;
		else if(m_dwFlag & tIsMapEntity)
			return 1;
		else 
			return m_pLayerList->getCount();
	}

	bool isMapEntityValid()
	{
		return (m_dwFlag & tIsMapEntity) != 0;
	}

	EntityView* getMapEntity()
	{
		return m_pMapEntity;
	}

	LayerList* getLayerList()const
	{
		return m_pLayerList;
	}

	LayerList** getLayerListPointer()
	{
		return &m_pLayerList;
	}

	bool isValid()const
	{
		return (bool) (m_dwFlag & tValid);
	}

	bool isMostTileValue()
	{
        //  如果Tile中只存在笼子类型的实体，则返回true，因为笼子类型的实体是用于生成配置文件而放置的（比如怪物
        //  生成文件）；
        BYTE nItemCount = getLayerCount();
        if (nItemCount > 0)
        {        
            for (int nItemIndex = 0; nItemIndex < nItemCount; nItemIndex++)
            {
                EntityView* pEntity = getLayer(nItemIndex);
                if (pEntity == NULL)
                {
                    ASSERT(false);
                    continue;
                }
                //  只要存在一个非笼子类型的实体就返回false；
                BYTE nType = (BYTE)pEntity->getEntityType();
                if (nType != typeCage)
                {
                    return false;
                }
            }

            return true;
        }

        return m_dwFlag == fMostTileValue;
	}

	void setMostValue()
	{
		TILE_CHECK
		m_dwFlag = fMostTileValue;
		TILE_CHECK
	}

	bool hasFlag(DWORD dwFlag) const
	{
		return (m_dwFlag & dwFlag) == dwFlag;
	}

	DWORD getFlag() const
	{
		return m_dwFlag;
	}

	void setFlag(DWORD dwFlag)
	{
		TILE_CHECK
		m_dwFlag = dwFlag;
		TILE_CHECK
	}

	void create()
	{
		m_dwFlag = tValid | 0;
		m_pLayerList = 0;
	}

	void close()
	{
		TILE_CHECK
		if ( isValid() && !isMapEntityValid())
		{
			safeRelease(m_pLayerList);
		}
		m_dwFlag = 0;
		m_pLayerList = 0;
	}

	bool serialize(bool bWrite,xs::MemoryStream* pStream,bool bClearOccupantWhenRead)
	{
		struct MAP_ARCHIVE_TILE_HEADER
		{
			DWORD dwFlag;
		};
		MAP_ARCHIVE_TILE_HEADER mt;

		if(bWrite)
		{
			MAP_ARCHIVE_TILE_HEADER mt;
			mt.dwFlag = m_dwFlag;
			pStream->write((char*)&mt,sizeof(mt));
		}
		else
		{
			pStream->read((char*)&mt,sizeof(mt));
			m_dwFlag = mt.dwFlag;
			if(bClearOccupantWhenRead)
				m_dwFlag |= mt.dwFlag & (~Tile::tOccupant_Entity);

		}
		return true;
	}
private:
	void check()
	{
		if (0 == m_dwFlag && NULL != m_pLayerList)
		{
			__asm{int 3};
		}
	}
protected:
	DWORD		m_dwFlag;
	union
	{
		LayerList*	m_pLayerList;
		EntityView*	m_pMapEntity;
	};
};

#endif

/** @} */