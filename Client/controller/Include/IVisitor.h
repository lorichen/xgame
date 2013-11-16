#ifndef _IVISITOR_H__
#define _IVISITOR_H__

struct IEntity;
class CommandManager;
/** Visitor接口定义
*/
struct IItemVisitor
{
	   virtual void VisitBox(IEntity *pEntity) = 0;
	   virtual void VisitHero(IEntity *pEntity) = 0;
	   virtual void VisitMast(IEntity *pEntity) = 0;
	   virtual void VisitMonster(IEntity *pEntity) = 0;
	   virtual void VisitPupet(IEntity *pEntity) = 0;	
	   virtual void VisitPet(IEntity *pEntity) = 0;
};

/** 缺省定义
*/
class ItemDefaultVisitor: public IItemVisitor
{
public:
	ItemDefaultVisitor(){}
public:
	virtual void VisitBox(IEntity *pEntity){}
	virtual void VisitHero(IEntity *pEntity){}
	virtual void VisitMast(IEntity *pEntity){}
	virtual void VisitMonster(IEntity *pEntity){}
	virtual void VisitPupet(IEntity *pEntity){}
	virtual void VisitPet(IEntity *pEntity){}

protected:
	virtual bool IsEnmity(IEntity *pEntity);
};

#endif