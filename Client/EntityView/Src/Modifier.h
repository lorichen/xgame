//==========================================================================
/**
* @file	  : Modifier.h
* @author : 
* created : 2008-5-21   12:03
* purpose : 实体外观修改器
*/
//==========================================================================

#ifndef __MODIFIER_H__
#define __MODIFIER_H__

enum ModifyType
{
	ModifyType_Color,
	ModifyType_Scale,
	ModifyType_Stun,
};

class Modifier
{
	ulong	mKey;
public:
	Modifier(ulong key) : mKey(key) {}
	ulong getKey() const	{ return mKey; }
	virtual ModifyType getType() const = 0;
	virtual void modify(EntityView*	owner) = 0;
	virtual void unmodify(EntityView* owner) = 0;
};

class ModifierColor : public Modifier
{
	ColorValue mOld, mNew;
public:
	ModifierColor(ulong key, const ColorValue& color) : Modifier(key), mNew(color) {}
	virtual ModifyType getType() const			{ return ModifyType_Color; }
	virtual void modify(EntityView*	owner)
	{
		owner->onCommand(EntityCommand_GetColor, (ulong)&mOld);
		owner->onCommand(EntityCommand_SetColor, (ulong)&mNew);
	}
	virtual void unmodify(EntityView* owner)
	{
		owner->onCommand(EntityCommand_SetColor, (ulong)&mOld);
	}
};

class ModifierScale : public Modifier
{
	float mOld, mNew;
public:
	ModifierScale(ulong key, float scale) : Modifier(key), mNew(scale) {}
	virtual ModifyType getType() const			{ return ModifyType_Scale; }
	virtual void modify(EntityView*	owner)
	{
		owner->onCommand(EntityCommand_GetScale, (ulong)&mOld);
		owner->onCommand(EntityCommand_SetScale, (ulong)&mNew);
	}
	virtual void unmodify(EntityView* owner)
	{
		owner->onCommand(EntityCommand_SetScale, (ulong)&mOld);
	}
};

class ModifierStun : public Modifier
{
public:
	ModifierStun(ulong key) : Modifier(key) {}
	virtual ModifyType getType() const			{ return ModifyType_Stun; }
	virtual void modify(EntityView*	owner)
	{
		owner->onCommand(EntityCommand_Stun);
	}
	virtual void unmodify(EntityView* owner)
	{
		owner->onCommand(EntityCommand_Stand);
	}
};

class ModifierList
{
	typedef std::list<Modifier*>	MdfList;
	MdfList	mList;
	EntityView*	mOwner;
public:
	ModifierList() : mOwner(0)			{ }
	~ModifierList()
	{
		unmodify();

		for (MdfList::iterator it=mList.begin(); it!=mList.end(); ++it)
			delete (*it);
		mList.clear();
	}

	void setOwner(EntityView* owner)	{ mOwner = owner; }

	void add(Modifier* modifier)
	{
		if (modifier)
		{
			mList.push_back(modifier);
			modifier->modify(mOwner);
		}
	}

	void remove(ulong key)
	{
		for (MdfList::iterator it=mList.begin(); it!=mList.end();)
		{
			Modifier* obj = *it;
			if (obj->getKey() == key)
			{
				ModifyType type = obj->getType();
				obj->unmodify(mOwner);
				it = mList.erase(it);
				delete obj;

				// 刷新后面的
				for (MdfList::iterator it1=it; it1!=mList.end(); ++it1)
				{
					if ((*it1)->getType() == type)
						(*it1)->modify(mOwner);
				}
			}
			else
				++it;
		}
	}
	
	void modify()
	{
		for (MdfList::iterator it=mList.begin(); it!=mList.end(); ++it)
			(*it)->modify(mOwner);
	}

	void unmodify()
	{
		for (MdfList::reverse_iterator it=mList.rbegin(); it!=mList.rend(); ++it)
			(*it)->unmodify(mOwner);
	}
};

#endif // __MODIFIER_H__