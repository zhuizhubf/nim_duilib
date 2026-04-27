#include "PerformanceUtil.h"
#include "duilib/Utils/StringUtil.h"
#include "duilib/Utils/LogUtil.h"

namespace ui 
{

PerformanceUtil::PerformanceUtil():
    m_nStatIndex(0)
{
}

PerformanceUtil::~PerformanceUtil()
{
    std::vector<TStat> statList;
    for (const auto& iter : m_stat) {
        if (iter.second.totalCount == 0) {
            continue;
        }
        statList.push_back(iter.second);
    }
    //按名称/添加顺序排序
    if (!statList.empty()) {
        std::sort(statList.begin(), statList.end(), [](const TStat& l, const TStat& r) {
            return l.m_name < r.m_name;
            });
    }
    for (const TStat& stat : statList) {
        auto totalMs = stat.totalTimes.count() / 1000;
        int32_t totalMsInt32 = (int32_t)totalMs;
        float totalMsFloat = (float)totalMsInt32 / stat.totalCount;
        DString log = StringUtil::Printf(_T("%s(%d): %d ms, average: %.03f ms, max: %d ms"), 
                                        stat.m_name.c_str(),                 //统计名称
                                        (int32_t)stat.totalCount,    //统计总次数
                                        (int32_t)(stat.totalTimes.count() / 1000), //总耗时(ms)
                                        totalMsFloat, //平均耗时(ms)
                                        (int32_t)(stat.maxTime.count() / 1000) //最大耗时(ms)
                                        );
        LogUtil::OutputLine(log);
    }
}

PerformanceUtil& PerformanceUtil::Instance()
{
    static PerformanceUtil self;
    return self;
}

void PerformanceUtil::BeginStat(const DString& name)
{
    ASSERT(!name.empty());
    size_t nameHash = std::hash<DString>{}(name);
    TStat& stat = m_stat[nameHash];
    stat.m_name = name;
    BeginStat(nameHash);
}

void PerformanceUtil::EndStat(const DString& name)
{
    ASSERT(!name.empty());
    size_t nameHash = std::hash<DString>{}(name);
    EndStat(nameHash);
}

void PerformanceUtil::AddStat(const DString& name)
{
    ASSERT(!name.empty());
    size_t nameHash = std::hash<DString>{}(name);
    TStat& stat = m_stat[nameHash];
    stat.m_name = name;
    stat.nStatIndex = ++m_nStatIndex;
}

void PerformanceUtil::BeginStat(size_t nameHash)
{
    TStat& stat = m_stat[nameHash];
    stat.startTime = std::chrono::steady_clock::now();
    ASSERT(stat.nStartRefCount >= 0);
    stat.nStartRefCount++;
}

void PerformanceUtil::EndStat(size_t nameHash)
{
    std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();;
    TStat& stat = m_stat[nameHash];
    ASSERT(stat.nStartRefCount > 0);
    if (stat.nStartRefCount <= 0) {
        return;
    }
    stat.endTime = endTime;
    stat.totalCount += 1;
    auto thisTime = std::chrono::duration_cast<std::chrono::microseconds>(stat.endTime - stat.startTime);
    stat.totalTimes += thisTime;
    stat.nStartRefCount--;
    stat.maxTime = (std::max)(stat.maxTime, thisTime);
}

}
