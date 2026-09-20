// 属性名派发基准：对比历史顺序比较链 与 顶层一次哈希后 O(1) 定位
// 数据来源：仓库的 xmake/scripts/attribute_defs.lua（control 域真实属性名）
//
// 用法（仓库根目录）：xmake -P tools/bench_attribute_dispatch && xmake run -P tools/bench_attribute_dispatch
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <map>
#include <regex>
#include <string>
#include <vector>

static uint32_t Fnv1a(const std::string &s)
{
    uint32_t h = 2166136261u;
    for (unsigned char c : s) {
        h ^= (uint32_t) c;
        h *= 16777619u;
    }
    return h;
}

static std::vector<std::string> LoadNames(const std::string &file, const std::string &domain)
{
    std::ifstream in(file);
    if (!in) {
        return {};
    }
    const std::string text((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    const std::regex block("\\r?\\n\\s+" + domain + " = \\{([^}]*)\\}");
    std::smatch match;
    std::vector<std::string> names;
    if (std::regex_search(text, match, block)) {
        const std::regex item("\"([^\"]+)\"");
        for (std::sregex_iterator it(match[1].first, match[1].second, item), end; it != end; ++it) {
            names.push_back((*it)[1].str());
        }
    }
    return names;
}

int main(int argc, char **argv)
{
    const std::string dataFile = (argc > 1) ? argv[1] : "xmake/scripts/attribute_defs.lua";
    const std::vector<std::string> names = LoadNames(dataFile, "control");
    if (names.empty()) {
        printf("未读取到属性名（请在仓库根目录运行，或把 xmake/scripts/attribute_defs.lua 路径作为参数传入）\n");
        return 1;
    }
    const std::vector<std::string> hashTableNames = names;
    std::vector<uint32_t> ids;
    ids.reserve(names.size());
    for (const std::string &n : names) {
        ids.push_back(Fnv1a(n));
    }
    // O(1) 定位表（开放寻址），模拟 switch 的跳表开销
    const size_t tableSize = names.size() * 2 + 16;
    std::vector<int32_t> table(tableSize, -1);
    for (size_t i = 0; i < ids.size(); ++i) {
        size_t slot = ids[i] % tableSize;
        while (table[slot] != -1) {
            slot = (slot + 1) % tableSize;
        }
        table[slot] = (int32_t) i;
    }

    volatile size_t sink = 0;
    const int repeat = 20000;

    auto ifChain = [&](const std::string &key) -> size_t {
        size_t hit = 0;
        for (const std::string &n : hashTableNames) {
            if (key == n) {
                hit = 1;
                break;
            }
        }
        return hit;
    };
    auto hashLookup = [&](const std::string &key) -> size_t {
        const uint32_t h = Fnv1a(key);
        size_t slot = h % tableSize;
        while (table[slot] != -1 && ids[(size_t) table[slot]] != h) {
            slot = (slot + 1) % tableSize;
        }
        return (table[slot] != -1) ? 1u : 0u;
    };
    auto bench = [&](const std::string &key, auto &&fn) {
        double best = 1e18;
        for (int round = 0; round < 3; ++round) {
            auto t0 = std::chrono::steady_clock::now();
            for (int r = 0; r < repeat; ++r) {
                sink += fn(key);
            }
            auto t1 = std::chrono::steady_clock::now();
            const double ns = std::chrono::duration<double, std::nano>(t1 - t0).count() / repeat;
            if (ns < best) {
                best = ns;
            }
        }
        return best;
    };

    // std::map 查找：模拟旧版 createControlMap（类名 -> 创建函数）的派发开销
    std::map<std::string, int> orderedMap;
    for (size_t i = 0; i < names.size(); ++i) {
        orderedMap[names[i]] = (int) i;
    }
    auto mapLookup = [&](const std::string &key) -> size_t {
        return (orderedMap.find(key) != orderedMap.end()) ? 1u : 0u;
    };

    const std::string &worstName = names.back();
    const std::string &typicalName = names[names.size() / 2];
    printf("control 域属性名数量: %zu（顺序比较链的平均长度约为其一半）\n", names.size());
    printf("%-36s %14s\n", "查询方式", "ns/op");
    printf("%-36s %14.1f\n", "顺序比较链（查最后一个名字）", bench(worstName, ifChain));
    printf("%-36s %14.1f\n", "顺序比较链（查中间名字）", bench(typicalName, ifChain));
    printf("%-36s %14.1f\n", "一次哈希 + O(1) 定位（查最后一个名字）", bench(worstName, hashLookup));
    printf("%-36s %14.1f\n", "一次哈希 + O(1) 定位（查中间名字）", bench(typicalName, hashLookup));
    printf("%-36s %14.1f\n", "std::map 查找（查平局的中间名字）", bench(typicalName, mapLookup));
    printf(
        "\n注：正式代码用编译期哈希 + switch(密集枚举)，实测比历史 if 链快 30~50 "
        "倍（见设计文档附录）。\n");
    (void) sink;
    return 0;
}
