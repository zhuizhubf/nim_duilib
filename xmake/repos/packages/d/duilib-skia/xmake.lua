-- duilib-skia：nim_duilib 专用的 Skia 本地包
--
-- 说明：
--   1. 与 build/build_duilib_all_in_one.* 使用同一个 Skia 提交和同一个补丁包（skia_compile）；
--   2. 使用 GN + Ninja 编译，编译参数与现有脚本保持一致；
--   3. 只编译当前配置（平台/架构/Debug/Release），编译完成后安装到 xmake 的包目录，可重复使用。

package("duilib-skia")
    set_homepage("https://skia.org/")
    set_description("Skia（nim_duilib 使用的快照，含 skia_compile 补丁，使用 GN 自动编译）")
    set_license("BSD-3-Clause")

    -- Skia 源码快照（与 build_duilib_all_in_one 脚本中的提交一致）
    -- 说明：URL 必须以 .zip 结尾，xmake 才会自动识别并解压源码包
    add_urls("https://github.com/google/skia/archive/34aa71b8bee4648a442b7125680232d803374f19.zip")
    add_versions("2026.2.10", "efe474e84b4ef1b697b2dfdda1f9905c06c676c94a32c72f7d44420023454b5a")

    -- nim_duilib 使用的补丁包（来自 rhett-lee/skia_compile 仓库）
    add_resources("2026.2.10", "skia_patch",
                  "https://github.com/rhett-lee/skia_compile/raw/main/skia.2026-02-10.src.zip",
                  "09124c01076e38297252737df63ff92605604e311ffa032bc1fe04f279d5bbf9")

    add_configs("clang", {description = "使用 LLVM/Clang 编译 Skia（默认使用 MSVC）", default = false, type = "boolean"})
    add_configs("clang_dir", {description = "使用 clang 编译 Skia 时，clang 的安装目录（默认 C:/LLVM）", default = "C:/LLVM", type = "string"})
    add_configs("runtime", {description = "编译 Skia 使用的 MSVC 运行库（MT/MD）", default = "MT", type = "string"})

    -- 类 Unix 平台使用 xmake 提供的 gn/ninja/python（与脚本要求系统安装 gn/ninja/python3 等价）
    if is_plat("linux", "macosx") then
        add_deps("gn", "ninja", "python", {kind = "binary"})
    elseif is_plat("windows") then
        -- Windows 下 gn.exe/ninja.exe 由补丁包提供，但 GN 需要 python
        add_deps("python", {kind = "binary"})
    end

    -- 安装后的目录结构：skia/（打补丁后的源码树）、lib/（编译出的静态库）
    add_includedirs("skia")
    add_linkdirs("lib")
    add_links("svg", "skshaper", "skottie", "sksg", "jsonreader", "skia")

    if is_plat("windows") then
        add_syslinks("gdi32", "user32", "opengl32")
    elseif is_plat("macosx") then
        add_frameworks("CoreFoundation", "CoreGraphics", "CoreText", "CoreServices")
    elseif is_plat("linux", "freebsd") then
        add_syslinks("pthread", "dl")
    end

    on_install("windows", "linux", "macosx", "freebsd", function (package)
        -- 递归合并目录：os.cp 对目录是"替换"，这里需要的是"覆盖同名文件、保留其他文件"
        local function merge_copy(srcdir, dstdir)
            os.mkdir(dstdir)
            for _, entry in ipairs(os.filedirs(path.join(srcdir, "*"))) do
                local dstpath = path.join(dstdir, path.filename(entry))
                if os.isdir(entry) then
                    merge_copy(entry, dstpath)
                else
                    os.cp(entry, dstpath)
                end
            end
        end

        -- 1. 叠加 skia_compile 的补丁文件（覆盖 Skia 源码树中的相应文件）
        local patchdir = package:resourcedir("skia_patch")
        assert(patchdir and os.isdir(patchdir), "无法获取 Skia 补丁包(skia_patch)")
        merge_copy(patchdir, os.curdir())

        -- xmake 解压源码时会跳过以 "." 开头的文件，而 GN 需要源码根目录的 .gn 文件
        if not os.isfile(path.join(os.curdir(), ".gn")) then
            local gntemplate = path.join(os.scriptdir(), "skia.gn")
            assert(os.isfile(gntemplate), "缺少 .gn 模板文件: " .. gntemplate)
            os.cp(gntemplate, path.join(os.curdir(), ".gn"))
        end

        -- 2. 确定目标 CPU 和编译模式
        local cpu = package:arch()
        if cpu == "x86_64" or cpu == "amd64" then
            cpu = "x64"
        elseif cpu == "i386" or cpu == "i686" then
            cpu = "x86"
        elseif cpu == "aarch64" then
            cpu = "arm64"
        end
        local gn = "gn"
        local ninja = "ninja"
        local cflags = {"-DSK_DISABLE_LEGACY_PNG_WRITEBUFFER"}
        local ldflags = {}
        local args = {}
        local args_common = {}
        local outdir = nil
        local buildtype = package:is_debug() and "debug" or "release"

        if is_host("windows") then
            -- Windows：使用补丁包中自带的 gn.exe / ninja.exe
            gn = path.join(os.curdir(), "bin", "gn.exe")
            ninja = path.join(os.curdir(), "bin", "ninja.exe")
            assert(os.isfile(gn), "未找到 gn.exe: " .. gn)
            assert(os.isfile(ninja), "未找到 ninja.exe: " .. ninja)
            if package:config("clang") then
                -- 可选：使用 LLVM/Clang 编译（与项目原有的 build_duilib_all_in_one 脚本一致）
                local clangdir = package:config("clang_dir")
                if not os.isfile(path.join(clangdir, "bin", "clang.exe")) then
                    raise("未找到 %s，请先安装 LLVM/Clang（例如安装到 C:/LLVM），" ..
                          "或者通过 xmake f --skia_clang_dir=<clang目录> 指定正确的目录",
                          path.join(clangdir, "bin", "clang.exe"))
                end
                table.insert(args, 'cc="clang"')
                table.insert(args, 'cxx="clang++"')
                table.insert(args, 'clang_win="' .. clangdir .. '"')
                outdir = "out/llvm." .. cpu .. "." .. buildtype
            else
                -- 默认：使用 MSVC（cl.exe）编译，不需要安装 LLVM；
                -- win_vc / win_toolchain_version / win_sdk_version 由 GN 自动检测（gn/find_msvc.py）
                --
                -- 显式指定 cc/cxx 为 cl：GN 会用 `cc --version` 的结果判断是否 MinGW 编译器
                -- （gn/is_mingw.py），在 MSYS/Git Bash 环境（如 CI）中 PATH 里的 cc 可能是 MinGW 的
                -- gcc，会被误判为 MinGW 并切到 gcc_like 工具链，从而用 g++ 编译 Skia 导致失败；
                -- 指定 cc="cl" 后该判断直接返回否，实际编译器仍由 MSVC 工具链决定。
                table.insert(args, 'cc="cl"')
                table.insert(args, 'cxx="cl"')
                outdir = "out/msvc." .. cpu .. "." .. buildtype
            end
            -- 运行库必须与 duilib/示例保持一致（默认 /MT）
            local runtime = (package:config("runtime") or "MT"):gsub("d$", "")
            table.insert(cflags, "/" .. runtime .. (package:is_debug() and "d" or ""))
        else
            -- 类 Unix：优先使用 clang，没有 clang 时使用 gcc（与脚本一致）
            import("lib.detect.find_tool")
            if find_tool("clang") then
                table.insert(args, 'cc="clang"')
                table.insert(args, 'cxx="clang++"')
                outdir = "out/llvm." .. cpu .. "." .. buildtype
            else
                table.insert(args, 'cc="gcc"')
                table.insert(args, 'cxx="g++"')
                outdir = "out/gcc." .. cpu .. "." .. buildtype
            end
            if package:is_plat("freebsd") then
                -- FreeBSD：与 skia_compile 的 FreeBSD 脚本一致（系统 freetype/fontconfig）
                table.insert(args, 'ar="llvm-ar"')
                table.insert(args_common, "skia_enable_fontmgr_fontconfig=true")
                table.insert(args_common, "skia_use_freetype=true")
                table.insert(ldflags, "-L/usr/local/lib")
                table.insert(cflags, "-I/usr/local/include/freetype2")
                table.insert(cflags, "-I/usr/local/include")
            end
        end

        -- 3. GN 参数（与现有脚本保持一致）
        table.insert(args, 'target_cpu="' .. cpu .. '"')
        table.insert(args, "is_trivial_abi=false")
        table.insert(args, "is_official_build=true")
        table.insert(args, "skia_use_libwebp_encode=false")
        table.insert(args, "skia_use_libwebp_decode=false")
        table.insert(args, "skia_use_libpng_encode=false")
        table.insert(args, "skia_use_libpng_decode=false")
        table.insert(args, "skia_use_zlib=false")
        table.insert(args, "skia_use_libjpeg_turbo_encode=false")
        table.insert(args, "skia_use_libjpeg_turbo_decode=false")
        table.insert(args, "skia_enable_fontmgr_win_gdi=false")
        table.insert(args, "skia_use_icu=false")
        table.insert(args, "skia_use_expat=true")
        table.insert(args, "skia_use_system_expat=false")
        table.insert(args, "skia_use_xps=false")
        table.insert(args, "skia_enable_pdf=false")
        table.insert(args, "skia_use_wuffs=false")
        table.insert(args, "skia_enable_svg=true")
        table.insert(args, "is_debug=false")
        for _, item in ipairs(args_common) do
            table.insert(args, item)
        end
        table.insert(args, 'extra_cflags=["' .. table.concat(cflags, '","') .. '"]')
        if #ldflags > 0 then
            table.insert(args, 'extra_ldflags=["' .. table.concat(ldflags, '","') .. '"]')
        end

        -- 4. 编译 Skia
        local genargs = {"gen", outdir, "--args=" .. table.concat(args, " ")}
        local pydir = nil
        -- GN 需要 python：优先使用 xmake 的 python 包，避免误用系统中的 python3 占位程序
        local pythondep = package:dep("python")
        if pythondep then
            local candidates = {
                path.join(pythondep:installdir(), "python.exe"),
                path.join(pythondep:installdir(), "bin", "python.exe"),
                path.join(pythondep:installdir(), "bin", "python3.exe"),
                path.join(pythondep:installdir(), "bin", "python"),
                path.join(pythondep:installdir(), "python")
            }
            for _, candidate in ipairs(candidates) do
                if os.isfile(candidate) then
                    pydir = path.directory(candidate)
                    table.insert(genargs, "--script-executable=" .. candidate)
                    break
                end
            end
        end
        if pydir then
            -- GN 内部的脚本（例如 bin/gn）会调用 python3，需要保证它在 PATH 中可被找到
            os.setenv("PATH", pydir .. (is_host("windows") and ";" or ":") .. (os.getenv("PATH") or ""))
        end
        os.vrunv(gn, genargs)
        -- 只编译需要的静态库（duilib 链接 skia/svg/skshaper/skottie/sksg/jsonreader）
        os.vrunv(ninja, {"-C", outdir, "skia", "svg", "skshaper", "skottie", "sksg", "jsonreader"})

        -- 5. 安装：静态库 + 源码树（供 duilib 编译 Skia 的窗口相关源码）
        local libdir = package:installdir("lib")
        os.mkdir(libdir)
        for _, file in ipairs(os.files(path.join(outdir, "*.lib"))) do
            os.cp(file, libdir)
        end
        for _, file in ipairs(os.files(path.join(outdir, "*.a"))) do
            os.cp(file, libdir)
        end
        -- 源码树：duilib 需要其中的头文件（include/、tools/ 等）来编译 Skia 的窗口相关源码；
        -- 编译产生的 out 目录不需要安装（静态库已单独安装到 lib/）
        local skia_installdir = path.join(package:installdir(), "skia")
        os.mkdir(skia_installdir)
        for _, entry in ipairs(os.filedirs(path.join(os.curdir(), "*"))) do
            if path.filename(entry) ~= "out" then
                os.cp(entry, skia_installdir)
            end
        end
    end)

    -- 说明：这里不做 on_test 检查。xmake 的片段检查默认使用 /MD 编译，
    -- 而本包按工程配置使用 /MT，链接时会因为运行库不一致而失败；
    -- 头文件与库文件的有效性由后续编译 duilib 与示例程序来验证。
