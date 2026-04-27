#ifndef UI_UTILS_PERFORMANCE_UTIL_H_
#define UI_UTILS_PERFORMANCE_UTIL_H_

#include "duilib/duilib_defs.h"
#include <string>
#include <unordered_map>
#include <chrono>
#include <algorithm>

namespace ui 
{

/** 代码执行性能分析工具
*/
class DUILIB_API PerformanceUtil
{
public:
    PerformanceUtil();
    ~PerformanceUtil();

    /** 单例对象
    */
    static PerformanceUtil& Instance();

    /** 代码开始执行，开始计时
    * @param [in] name 统计项的名称
    */
    void BeginStat(const DString& name);

    /** 代码结束执行，统计执行性能
    * @param [in] name 统计项的名称
    */
    void EndStat(const DString& name);

public:
    /** 增加一个统计，但不开始统计
    */
    void AddStat(const DString& name);

    /** 代码开始执行，开始计时
    * @param [in] nameHash 统计项的名称的Hash值
    */
    void BeginStat(size_t nameHash);

    /** 代码结束执行，统计执行性能
    * @param [in] nameHash 统计项的名称的Hash值
    */
    void EndStat(size_t nameHash);
    
private:
    /** 记录每项统计的结果
    */
    struct TStat
    {
        //添加的顺序号
        size_t nStatIndex = 0;

        //是否有开始数据
        int32_t nStartRefCount = 0;

        //开始时间
        std::chrono::steady_clock::time_point startTime;

        //结束时间
        std::chrono::steady_clock::time_point endTime;

        //代码执行总时间：微秒(千分之一毫秒)
        std::chrono::microseconds totalTimes = std::chrono::microseconds::zero();

        //统计总次数
        uint32_t totalCount = 0;

        /** 单次最大：：微秒(千分之一毫秒)
        */
        std::chrono::microseconds maxTime = std::chrono::microseconds::zero();

        /** 名称
        */
        DString m_name;
    };

    std::unordered_map<size_t, TStat> m_stat;
    size_t m_nStatIndex; //添加的顺序号
};

class PerformanceStat
{
public:
    explicit PerformanceStat(const DString& statName):
        m_statName(statName),
        m_nameHash(0)
    {
        if (!m_statName.empty()) {
            PerformanceUtil::Instance().AddStat(m_statName);
            m_nameHash = std::hash<DString>{}(m_statName);
            PerformanceUtil::Instance().BeginStat(m_nameHash);
        }        
    }
    ~PerformanceStat()
    {
        if (m_nameHash != 0) {
            PerformanceUtil::Instance().EndStat(m_nameHash);
        }
    }

    //主动结束统计
    void EndStat()
    {
        if (m_nameHash != 0) {
            PerformanceUtil::Instance().EndStat(m_nameHash);
            m_nameHash = 0;
        }
    }

private:
    DString m_statName; //统计项的显示名称
    size_t m_nameHash;  //统计项的显示名称的Hash值
};

class PerformanceStatFast
{
public:
    explicit PerformanceStatFast(size_t nameHash):
        m_nameHash(nameHash)
    {
        if (m_nameHash != 0) {
            PerformanceUtil::Instance().BeginStat(m_nameHash);
        }
    }
    ~PerformanceStatFast()
    {
        if (m_nameHash != 0) {
            PerformanceUtil::Instance().EndStat(m_nameHash);
        }
    }

    //主动结束统计
    void EndStat()
    {
        if (m_nameHash != 0) {
            PerformanceUtil::Instance().EndStat(m_nameHash);
            m_nameHash = 0;
        }
    }

private:
    size_t m_nameHash;  //统计项的显示名称的Hash值
};

}

#endif // UI_UTILS_PERFORMANCE_UTIL_H_
