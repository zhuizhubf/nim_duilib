#ifndef UI_UTILS_PERFORMANCE_UTIL_H_
#define UI_UTILS_PERFORMANCE_UTIL_H_

#include "duilib/duilib_config.h"
#include <string>
#include <unordered_map>
#include <chrono>
#include <algorithm>

/** 功能开关（给调用方使用）: 1表示开启性能统计日志，0表示关闭性能统计日志
*/
#define DUILIB_PERFORMANCE_STAT_ENABLED 1

namespace ui 
{
/** 代码执行性能分析工具
*/
class DUILIB_API PerformanceUtil
{
public:
    explicit PerformanceUtil(const DString& statName);
    ~PerformanceUtil();

    //主动结束统计
    void EndStat();

private:
    DString m_statName; //统计项的显示名称
    size_t m_nameHash;  //统计项的显示名称的Hash值
};

/** 代码执行性能分析工具（快速版，避免使用字符串的hash函数）
*/
class DUILIB_API PerformanceUtilFast
{
public:
    explicit PerformanceUtilFast(size_t nameHash);
    ~PerformanceUtilFast();

    //主动结束统计
    void EndStat();

private:
    size_t m_nameHash;  //统计项的显示名称的Hash值
};

/** 代码执行性能分析工具（实现）
*/
class DUILIB_API PerformanceUtilHelper
{
public:
    PerformanceUtilHelper();
    ~PerformanceUtilHelper();

    /** 单例对象
    */
    static PerformanceUtilHelper& Instance();

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

}

#endif // UI_UTILS_PERFORMANCE_UTIL_H_
