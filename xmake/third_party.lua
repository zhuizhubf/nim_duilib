-- nim_duilib xmake 构建脚本：第三方库
-- 说明：源码全部取自仓库，编译方式与项目原有构建方式保持一致。

local thirdroot = DUILIB_THIRD_DIR

-- -----------------------------------------------------------------------------
-- zlib
-- -----------------------------------------------------------------------------
target("duilib-zlib")
    set_kind("static")
    set_targetdir(DUILIB_LIB_DIR)
    duilib_target_settings()
    add_files(path.join(thirdroot, "zlib", "*.c"))
    add_includedirs(path.join(thirdroot, "zlib"))
target_end()

-- -----------------------------------------------------------------------------
-- libpng（使用仓库内已经打过 APNG 补丁的源码）
-- -----------------------------------------------------------------------------
target("duilib-png")
    set_kind("static")
    set_targetdir(DUILIB_LIB_DIR)
    duilib_target_settings()
    add_deps("duilib-zlib")
    add_includedirs(path.join(thirdroot, "libpng"), path.join(thirdroot, "zlib"))
    add_files(path.join(thirdroot, "libpng", "*.c"))
    remove_files(path.join(thirdroot, "libpng", "pngtest.c"))
    local arch = duilib_arch_name()
    if arch == "x86" or arch == "x64" then
        -- x86/x64 使用 SSE2 优化（与 libpng 官方配置一致）
        add_files(path.join(thirdroot, "libpng/intel", "*.c"))
        add_defines("PNG_INTEL_SSE_OPT=1")
    elseif arch == "arm64" then
        add_files(path.join(thirdroot, "libpng/arm", "*.c"))
        add_defines("PNG_ARM_NEON_OPT=2")
    end
target_end()

-- -----------------------------------------------------------------------------
-- cximage
-- -----------------------------------------------------------------------------
target("duilib-cximage")
    set_kind("static")
    set_targetdir(DUILIB_LIB_DIR)
    duilib_target_settings()
    set_languages("c++17")
    add_deps("duilib-png", "duilib-zlib")
    add_includedirs(path.join(thirdroot, "cximage"), path.join(thirdroot, "libpng"), path.join(thirdroot, "zlib"), DUILIB_SRC_DIR)
    add_files(
        path.join(thirdroot, "cximage", "ximaenc.cpp"),
        path.join(thirdroot, "cximage", "ximage.cpp"),
        path.join(thirdroot, "cximage", "ximagif.cpp"),
        path.join(thirdroot, "cximage", "ximaico.cpp"),
        path.join(thirdroot, "cximage", "ximainfo.cpp"),
        path.join(thirdroot, "cximage", "ximalpha.cpp"),
        path.join(thirdroot, "cximage", "ximapal.cpp"),
        path.join(thirdroot, "cximage", "ximapng.cpp"),
        path.join(thirdroot, "cximage", "ximath.cpp"),
        path.join(thirdroot, "cximage", "ximatran.cpp"),
        path.join(thirdroot, "cximage", "xmemfile.cpp")
    )
target_end()

-- -----------------------------------------------------------------------------
-- libwebp（只使用解码、解复用与工具代码，源码列表与原构建方式一致）
-- -----------------------------------------------------------------------------
target("duilib-webp")
    set_kind("static")
    set_targetdir(DUILIB_LIB_DIR)
    duilib_target_settings()
    add_includedirs(path.join(thirdroot, "libwebp"))
    add_files(
        path.join(thirdroot, "libwebp", "src/dec", "*.c"),
        path.join(thirdroot, "libwebp", "src/demux", "*.c"),
        path.join(thirdroot, "libwebp", "src/dsp", "*.c"),
        path.join(thirdroot, "libwebp", "src/utils", "*.c")
    )
target_end()

-- -----------------------------------------------------------------------------
-- CEF 封装库（libcef_dll_wrapper，仅 Windows + --cef=y 时编译）
-- -----------------------------------------------------------------------------
if get_config("cef") then
    local cefroot = duilib_cef_sdk_dir()
    local wraproot = path.join(cefroot, "libcef_dll")
    local cefarch = (duilib_arch_name() == "x86") and "Win32" or "x64"

    target(duilib_cef_wrapper_name())
        set_kind("static")
        set_targetdir(DUILIB_LIB_DIR)
        duilib_target_settings()
        set_languages("c++17")
        add_files(
            path.join(wraproot, "*.cc"),
            path.join(wraproot, "base", "*.cc"),
            path.join(wraproot, "cpptoc", "*.cc"),
            path.join(wraproot, "ctocpp", "*.cc"),
            path.join(wraproot, "wrapper", "*.cc")
        )
        add_includedirs(cefroot, path.join(cefroot, "include"), wraproot)
        -- 与 CEF 官方工程中的宏定义保持一致
        add_defines("WRAPPING_CEF_SHARED",
                    "NOMINMAX",
                    "WIN32_LEAN_AND_MEAN",
                    "__STDC_CONSTANT_MACROS",
                    "__STDC_FORMAT_MACROS",
                    "CEF_USE_BOOTSTRAP",
                    "_HAS_EXCEPTIONS=0",
                    "WINVER=0x0A00",
                    "_WIN32_WINNT=0x0A00",
                    "NTDDI_VERSION=NTDDI_WIN10_FE")
        add_linkdirs(path.join(cefroot, "lib", cefarch))
        add_links("libcef")

        on_load(function (target)
            if get_config("plat") ~= "windows" then
                raise("xmake 构建脚本目前只支持在 Windows 平台启用 CEF，其他平台暂不支持（可参见 docs/CEF.md）")
            end
        end)
    target_end()
end
