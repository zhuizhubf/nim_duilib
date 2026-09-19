-- =============================================================================
-- 属性名 ID 层生成器：读取 attribute_defs.lua，生成 AttributeIds.g.h / .g.cpp
--
-- 用法（仓库根目录）：
--     xmake attribute-gen                    -- 输出到 src/duilib/Utils
--     xmake l attribute_gen.lua <输出目录>    -- 输出到指定目录
--
-- 约定：
--   1. attribute_defs.lua 是唯一手写输入，本文件不含任何属性名数据。
--   2. 每个名字各占一个 ID（含历史拼写），代码里共享处理分支时写两个 case 标签。
--   3. 输出严格按数据表顺序，保证 diff 最小、既有枚举值不跳动。
-- =============================================================================

-- 纯算术 XOR：不依赖 Lua 5.3+ 位运算符，保证 xmake 内建 Lua 各版本行为一致
local g_xorNibble = {}
do
    for a = 0, 15 do
        g_xorNibble[a] = {}
        for b = 0, 15 do
            local r, bit, x, y = 0, 1, a, b
            for _ = 1, 4 do
                if (x % 2) ~= (y % 2) then
                    r = r + bit
                end
                x = math.floor(x / 2)
                y = math.floor(y / 2)
                bit = bit * 2
            end
            g_xorNibble[a][b] = r
        end
    end
end

local function bxor(a, b)
    local result, shift = 0, 1
    while (a > 0) or (b > 0) do
        local na, nb = a % 16, b % 16
        result = result + g_xorNibble[na][nb] * shift
        a = math.floor(a / 16)
        b = math.floor(b / 16)
        shift = shift * 16
    end
    return result
end

-- FNV-1a 32 位；乘法用 2^16 拆分，避免双精度浮点超出 2^53 丢精度
local function fnv1a(name)
    local h = 2166136261
    for i = 1, #name do
        h = bxor(h, name:byte(i))
        local hi = math.floor(h / 65536)
        local lo = h % 65536
        h = ((hi * 16777619) % 65536 * 65536 + lo * 16777619) % 4294967296
    end
    return h
end

local function to_identifier(name)
    local parts = {}
    for part in name:gmatch("[^_]+") do
        parts[#parts + 1] = part:sub(1, 1):upper() .. part:sub(2)
    end
    return "k" .. table.concat(parts)
end

-- 展开并校验数据表
local function normalize(defs, domains)
    local result = {}
    for _, domain in ipairs(domains) do
        local list, seenName, seenIdent, seenHash = {}, {}, {}, {}
        for _, raw in ipairs(defs[domain]) do
            -- 条目可以是字符串，也可以是 { name = "...", macro = "..." } 表（ctrl 域用后者携带宏名）
            local name, macro
            if type(raw) == "table" then
                name = raw.name
                macro = raw.macro
            else
                name = raw
            end
            assert(type(name) == "string", domain .. " 域条目缺少 name")
            assert(name:match("^[A-Za-z0-9_]+$"), "非法属性名（仅允许 ASCII 字母数字下划线）: " .. name)
            assert(name ~= "none" or true, "")
            assert(seenName[name] == nil, "同域属性名重复: " .. domain .. "." .. name)
            local ident = to_identifier(name)
            assert(ident ~= "kInvalidId", "属性名与保留标识符冲突（kInvalidId）: " .. domain .. "." .. name)
            if seenIdent[ident] ~= nil then
                local suffix = 2
                while seenIdent[ident .. suffix] ~= nil do
                    suffix = suffix + 1
                end
                ident = ident .. suffix
            end
            local hash = fnv1a(name)
            assert(seenHash[hash] == nil,
                   string.format("同域哈希冲突: %s 的 %s 与 %s 都映射到 0x%08X", domain, name, tostring(seenHash[hash]), hash))
            seenName[name], seenIdent[ident], seenHash[hash] = true, name, name
            list[#list + 1] = { name = name, ident = ident, hash = hash, id = #list, macro = macro }
        end
        result[domain] = list
    end
    return result
end

local function emit_header(domains, data)
    local h = {}
    local function add(line) h[#h + 1] = line end
    add("// 本文件由 tools/attribute_gen.lua 生成，请勿手改。")
    add("// 数据来源：tools/attribute_defs.lua（新增属性请改数据表后运行 xmake attribute-gen）")
    add("")
    add("#ifndef UI_UTILS_ATTRIBUTE_IDS_G_H_")
    add("#define UI_UTILS_ATTRIBUTE_IDS_G_H_")
    add("")
    add("#include \"duilib/duilib_config.h\"")
    add("#include \"duilib/duilib_string.h\"")
    add("")
    add("#include <cstdint>")
    add("")
    add("namespace ui {")
    add("namespace attr {")
    for _, domain in ipairs(domains) do
        add("")
        add("namespace " .. domain .. " {")
        add("enum Id : uint32_t {")
        add("    kInvalidId = 0xFFFFFFFFu,")
        for _, item in ipairs(data[domain]) do
            add(string.format("    %s = %d,", item.ident, item.id))
        end
        add("};")
        add("")
        add("Id IdOf(const DString &strName);")
        add("DString IdToString(Id id);")
        add("} //namespace " .. domain)
    end

    add("} //namespace attr")
    add("} //namespace ui")
    add("")
    add("#endif //UI_UTILS_ATTRIBUTE_IDS_G_H_")
    return h
end

local function emit_source(domains, data)
    local c = {}
    local function add(line) c[#c + 1] = line end
    add("// 本文件由 tools/attribute_gen.lua 生成，请勿手改。")
    add("// 数据来源：tools/attribute_defs.lua（新增属性请改数据表后运行 xmake attribute-gen）")
    add("")
    add("#include \"duilib/Utils/AttributeIds.g.h\"")
    add("#include \"duilib/duilib_config.h\"")
    add("")
    add("#include <unordered_set>")
    add("")
    add("namespace ui {")
    add("namespace attr {")
    add("namespace {")
    add("/** 运行期 FNV-1a 32 位哈希，必须与生成器 fnv1a 的实现完全一致。")
    add("*/")
    add("uint32_t Hash(const DString &strName)")
    add("{")
    add("    uint32_t h = 2166136261u;")
    add("    for (DString::value_type c : strName) {")
    add("        h ^= (uint32_t) c;")
    add("        h *= 16777619u;")
    add("    }")
    add("    return h;")
    add("}")
    add("} //namespace")
    for _, domain in ipairs(domains) do
        add("")
        add("namespace " .. domain .. " {")
        add("Id IdOf(const DString &strName)")
        add("{")
        add("    switch (Hash(strName)) {")
        for _, item in ipairs(data[domain]) do
            add(string.format("    case 0x%08Xu: //%s", item.hash, item.name))
            add("        return Id::" .. item.ident .. ";")
        end
        add("    default:")
        add("        break;")
        add("    }")
        add("    return Id::kInvalidId;")
        add("}")
        add("")
        add("DString IdToString(Id id)")
        add("{")
        add("    switch (id) {")
        for _, item in ipairs(data[domain]) do
            add("    case Id::" .. item.ident .. ":")
            add("        return _T(\"" .. item.name .. "\");")
        end
        add("    default:")
        add("        break;")
        add("    }")
        add("    return DString();")
        add("}")
        add("} //namespace " .. domain)
    end

    add("} //namespace attr")
    add("} //namespace ui")
    return c
end

function main(...)
    local args = { ... }
    import("attribute_defs", { rootdir = os.scriptdir() })
    local defs = attribute_defs.get_defs()

    local domains = {}
    for domain in pairs(defs) do
        domains[#domains + 1] = domain
    end
    table.sort(domains)

    local data = normalize(defs, domains)
    local total = 0
    for _, domain in ipairs(domains) do
        print(string.format("  %-8s names=%d", domain, #data[domain]))
        total = total + #data[domain]
    end

    local outDir = args[1]
    if (outDir == nil) or (outDir == "") then
        outDir = path.join(os.scriptdir(), "src", "duilib", "Utils")
    end
    os.mkdir(outDir)
    io.writefile(path.join(outDir, "AttributeIds.g.h"), table.concat(emit_header(domains, data), "\n") .. "\n")
    io.writefile(path.join(outDir, "AttributeIds.g.cpp"), table.concat(emit_source(domains, data), "\n") .. "\n")

    -- 控件类名宏：与数据表一起生成，保持公开 API（DUI_CTR_*）兼容且不再手写
    local macroLines = {}
    for _, item in ipairs(data["ctrl"] or {}) do
        if (item.macro ~= nil) and (item.macro ~= "") then
            macroLines[#macroLines + 1] = "#define " .. item.macro .. " (_T(\"" .. item.name .. "\"))"
        end
    end
    if #macroLines > 0 then
        local m = {}
        local function madd(line)
            m[#m + 1] = line
        end
        madd("// 本文件由 tools/attribute_gen.lua 生成，请勿手改。")
        madd("// 数据来源：tools/attribute_defs.lua 的 ctrl 域（控件类名宏，保持公开 API 兼容）")
        madd("")
        madd("#ifndef UI_DUILIB_CTRL_DEFS_G_H_")
        madd("#define UI_DUILIB_CTRL_DEFS_G_H_")
        madd("")
        for _, line in ipairs(macroLines) do
            madd(line)
        end
        madd("")
        madd("#endif //UI_DUILIB_CTRL_DEFS_G_H_")
        io.writefile(path.join(outDir, "CtrlDefs.g.h"), table.concat(m, "\n") .. "\n")
        print(string.format("生成完成：控件类名宏 %d 个 -> CtrlDefs.g.h", #macroLines))
    end
    print(string.format("生成完成：names=%d，输出目录 %s", total, outDir))
end
