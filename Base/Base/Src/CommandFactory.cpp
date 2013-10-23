/*******************************************************************
** 文件名:	CommandFactory.cpp 
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

#include "stdafx.h"
#include "CommandFactory.h"

#define new RKT_NEW

using namespace xs;

CommandFactory::CommandFactory()
{
	RegisterCommand(new Command_Help);
}

CommandFactory::~CommandFactory()
{
	COMMAND_MAP::iterator it = m_CommandList.begin();
	for ( ;it!=m_CommandList.end();++it )
	{
		ICommand * cmd = it->second;
		cmd->Release();
	}
}

void CommandFactory::RegisterCommand(ICommand * cmd)
{
	m_CommandList[cmd->GetCommandName()] = cmd;
}

ICommand * CommandFactory::CreateCommand(const char * cmd )
{
	std::string cmd_line = cmd;
	std::string cmd_name;

	size_t pos = cmd_line.find(' ');
	if ( pos<cmd_line.length())
		cmd_name = cmd_line.substr(0,pos);
	else
		cmd_name = cmd_line;

	COMMAND_MAP::iterator it = m_CommandList.find(cmd_name);
	if ( it==m_CommandList.end())
		return 0;

	return it->second->Clone();
}

//////////////////////////////////////////////////////////////////////////
Command_Help::Command_Help() : Command("help","帮助命令")
{
	DeclareStrParam("","",true,"cmd","命令名称.");
}

bool Command_Help::Execute()
{
	std::string cmd_name;

	GetStrParam("unnamed0",cmd_name);

	if ( cmd_name.empty() )
	{
		CommandFactory::COMMAND_MAP::iterator it= CommandFactory::getInstance().GetCommandList().begin();
		for ( ;it!=CommandFactory::getInstance().GetCommandList().end();++it )
		{
			Command * pCmd = (Command*)it->second;
			m_Output += pCmd->GetCommandName();
			m_Output += "		";
			m_Output += pCmd->GetCommandDesc();
			m_Output += "\r\n";
		}
		return true;
	}else
	{
		CommandFactory::COMMAND_MAP::iterator it= CommandFactory::getInstance().GetCommandList().find(cmd_name.c_str());
		if ( it==CommandFactory::getInstance().GetCommandList().end() )
		{
			m_Output = "The command not exist!";
			return false;
		}

		Command * pCmd = (Command*)it->second;
		m_Output = pCmd->GetCommandDescDetail();
		return true;
	}
}

API_EXPORT ICommandFactory * RKT_CDECL GetCommandFactory()
{
	return &CommandFactory::getInstance();
}