/*******************************************************************
** 文件名:	CommandFactory.h 
** 版  权:	(C)  2008 - All Rights Reserved
** 
** 日  期:	02/18/2008
** 版  本:	1.0
** 描  述:	
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __COMMAND_FACTORY_H__
#define __COMMAND_FACTORY_H__

#include "Common.h"
#include "Command.h"
#include "Singleton.h"
#include <map>

namespace xs
{
	class CommandFactory : public ICommandFactory,public Singleton<CommandFactory>
	{
	public:
		typedef std::map<std::string,ICommand *> COMMAND_MAP;

		/**
		@name         : 注册一个命令
		@brief        :
		@param        :
		@return       :
		*/
		virtual void RegisterCommand(ICommand * cmd);

		/**
		@name         : 根据输入的命令行解释出命令对象
		@return       : 如果不存在的命令，或者命令格式不对则返回0
		*/
		virtual ICommand * CreateCommand( const char * cmd );

		COMMAND_MAP & GetCommandList() { return m_CommandList; };

		CommandFactory();

		virtual ~CommandFactory();

	private:
		COMMAND_MAP   m_CommandList;
	};

	/**
	@name : 查询其他命令的格式
	@brief: 语法: help [command]
	*/
	class Command_Help : public Command
	{
	public:
		virtual bool Execute();

		virtual Command * Clone() { return new Command_Help; }

		Command_Help();
	};
};

#endif//__COMMAND_FACTORY_H__