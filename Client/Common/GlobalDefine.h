//////////////////////////////////////////////////////////////////////////
//  该文件定义了客户端使用的一些全局数据结构；

#pragma once

//////////////////////////////////////////////////////////////////////////
//  ProgressBar的初始化参数；
struct CProgressBarInitArgs
{
    //  持续时间；
    ulong m_ulDuration;
    //  是否反转表示；
    bool m_bInverse;

    CProgressBarInitArgs() :
    m_ulDuration(0),
        m_bInverse(false)
    {
    };
};

//  任务奖励；
struct CTaskPrize
{
    //  经验；
    int m_nExperience;
    //  金钱；
    int m_nMoney;
    //  可得物品（不用进行选择）；
    std::string m_strGoodsInfo;
    //  可选物品（多选一）；
    std::string m_strSelectingGoodsInfo;
};
