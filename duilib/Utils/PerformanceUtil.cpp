#include "PerformanceUtil.h"
#include "duilib/Utils/StringUtil.h"
#include "duilib/Utils/LogUtil.h"

namespace ui 
{
PerformanceUtil::PerformanceUtil(const DString& statName) :
    m_statName(statName),
    m_nameHash(0)
{
    if (!m_statName.empty()) {
        PerformanceUtilHelper::Instance().AddStat(m_statName);
        m_nameHash = std::hash<DString>{}(m_statName);
        PerformanceUtilHelper::Instance().BeginStat(m_nameHash);
    }
}
PerformanceUtil::~PerformanceUtil()
{
    if (m_nameHash != 0) {
        PerformanceUtilHelper::Instance().EndStat(m_nameHash);
    }
}

void PerformanceUtil::EndStat()
{
    if (m_nameHash != 0) {
        PerformanceUtilHelper::Instance().EndStat(m_nameHash);
        m_nameHash = 0;
    }
}

////////////////////////////////////////////////////////////////////

PerformanceUtilFast::PerformanceUtilFast(size_t nameHash) :
    m_nameHash(nameHash)
{
    if (m_nameHash != 0) {
        PerformanceUtilHelper::Instance().BeginStat(m_nameHash);
    }
}
PerformanceUtilFast::~PerformanceUtilFast()
{
    if (m_nameHash != 0) {
        PerformanceUtilHelper::Instance().EndStat(m_nameHash);
    }
}

void PerformanceUtilFast::EndStat()
{
    if (m_nameHash != 0) {
        PerformanceUtilHelper::Instance().EndStat(m_nameHash);
        m_nameHash = 0;
    }
}

////////////////////////////////////////////////////////////////////

PerformanceUtilHelper::PerformanceUtilHelper():
    m_nStatIndex(0)
{
}

PerformanceUtilHelper::~PerformanceUtilHelper()
{
#if DUILIB_PERFORMANCE_STAT_ENABLED
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
#endif //  DUILIB_PERFORMANCE_STAT_ENABLED
}

PerformanceUtilHelper& PerformanceUtilHelper::Instance()
{
    static PerformanceUtilHelper self;
    return self;
}

void PerformanceUtilHelper::BeginStat(const DString& name)
{
    ASSERT(!name.empty());
    size_t nameHash = std::hash<DString>{}(name);
    TStat& stat = m_stat[nameHash];
    stat.m_name = name;
    BeginStat(nameHash);
}

void PerformanceUtilHelper::EndStat(const DString& name)
{
    ASSERT(!name.empty());
    size_t nameHash = std::hash<DString>{}(name);
    EndStat(nameHash);
}

void PerformanceUtilHelper::AddStat(const DString& name)
{
    ASSERT(!name.empty());
    size_t nameHash = std::hash<DString>{}(name);
    TStat& stat = m_stat[nameHash];
    stat.m_name = name;
    stat.nStatIndex = ++m_nStatIndex;
}

void PerformanceUtilHelper::BeginStat(size_t nameHash)
{
    TStat& stat = m_stat[nameHash];
    stat.startTime = std::chrono::steady_clock::now();
    ASSERT(stat.nStartRefCount >= 0);
    stat.nStartRefCount++;
}

void PerformanceUtilHelper::EndStat(size_t nameHash)
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
