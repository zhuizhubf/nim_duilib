--[[
    duilib-scintilla：nim_duilib 的 DUI 原生 Scintilla 扩展库

    该扩展不修改 nim_duilib 核心库，使用核心仓库已经构建好的静态库和头文件。
    典型用法：

        # 1. 先在 nim_duilib 根目录构建核心库
        xmake f -c -m release
        xmake

        # 2. 再在扩展目录构建
        cd extensions/scintilla
        xmake f -c --duilib_dir=../.. --duilib_render=skia --duilib_runtime=MT
        xmake
]]

set_project("duilib-scintilla")
set_version("0.1.0")
set_xmakever("2.9.0")
set_languages("c++17")

add_rules("mode.debug", "mode.release")

local EXT_ROOT = os.scriptdir()

option("duilib_dir")
    set_default("")
    set_showmenu(true)
    set_description("nim_duilib 根目录（包含 src/、lib/、third_party/）")
option_end()

option("duilib_render")
    set_default("skia")
    set_showmenu(true)
    set_values("skia", "gdi", "both")
    set_description("与核心库一致的渲染后端")
option_end()

option("duilib_runtime")
    set_default("MT")
    set_showmenu(true)
    set_values("MT", "MD")
    set_description("与核心库一致的 MSVC 运行库")
option_end()

option("duilib_sdl")
    set_default(false)
    set_showmenu(true)
    set_description("核心库在 Windows 上是否使用 SDL3")
option_end()

option("examples")
    set_default(true)
    set_showmenu(true)
    set_description("编译 ScintillaDemo 示例")
option_end()

local function ext_duilib_dir()
    local dir = get_config("duilib_dir")
    if (dir == nil) or (dir == "") then
        dir = path.join(EXT_ROOT, "..", "..")
    end
    return path.absolute(dir, EXT_ROOT)
end

local DUILIB_DIR = ext_duilib_dir()
local DUILIB_SRC = path.join(DUILIB_DIR, "src")
local DUILIB_LIB = path.join(DUILIB_DIR, "lib")
local SCINTILLA = path.join(EXT_ROOT, "third_party", "scintilla")
local LEXILLA = path.join(EXT_ROOT, "third_party", "lexilla")

local function ext_render_mode()
    local mode = get_config("duilib_render") or "skia"
    return mode
end

local function ext_skia_enabled()
    local mode = ext_render_mode()
    return (mode == "skia") or (mode == "both")
end

local function ext_gdi_enabled()
    local mode = ext_render_mode()
    return is_plat("windows") and ((mode == "gdi") or (mode == "both"))
end

local function ext_sdl_enabled()
    if is_plat("windows") then
        return get_config("duilib_sdl") == true
    end
    return true
end

local function ext_check_core()
    if not os.isdir(DUILIB_SRC) then
        raise("找不到 nim_duilib 源码目录: " .. DUILIB_SRC ..
              "\n请通过 --duilib_dir=<nim_duilib根目录> 指定正确路径。")
    end
    if not os.isdir(DUILIB_LIB) then
        raise("找不到 nim_duilib 构建产物目录: " .. DUILIB_LIB ..
              "\n请先在 nim_duilib 根目录执行 xmake 构建核心库。")
    end
    if not os.isdir(SCINTILLA) then
        raise("找不到 Scintilla 源码: " .. SCINTILLA)
    end
    if not os.isdir(LEXILLA) then
        raise("找不到 Lexilla 源码: " .. LEXILLA)
    end
end

ext_check_core()

add_repositories("duilib-repo " .. path.join(DUILIB_DIR, "xmake", "repos"))

if ext_skia_enabled() then
    add_requires("duilib-skia", {configs = {
        clang = false,
        clang_dir = "C:/LLVM",
        runtime = get_config("duilib_runtime") or "MT"
    }})
end

if ext_sdl_enabled() then
    add_requires("libsdl3", {configs = {shared = false}})
end

local function ext_common_settings()
    set_encodings("utf-8")
    if is_plat("windows") then
        add_defines("UNICODE", "_UNICODE", "NOMINMAX", "WIN32_LEAN_AND_MEAN", "_CRT_SECURE_NO_WARNINGS")
        set_runtimes(get_config("duilib_runtime") or "MT")
    end
    add_defines("SCINTILLA_QT=1")
    add_defines("DUILIB_SDL=" .. (ext_sdl_enabled() and "1" or "0"))
    add_defines("DUILIB_CEF=0")
    add_defines("DUILIB_WEBVIEW2=0")
    add_defines("DUILIB_JPEG_TURBO=0")
    add_defines("DUILIB_LIB_PAG=0")
    add_defines("DUILIB_RENDER_SKIA=" .. (ext_skia_enabled() and "1" or "0"))
    add_defines("DUILIB_RENDER_GDI=" .. (ext_gdi_enabled() and "1" or "0"))
    add_defines("DUILIB_RENDER_DEFAULT_GDI=" .. ((ext_render_mode() == "gdi") and "1" or "0"))
    add_defines("DUILIB_IMAGE_SVG_NANOSVG=1")
    add_defines("DUILIB_IMAGE_SVG_SKIA=0")
    add_defines("DUILIB_IMAGE_LOTTIE_SKIA=0")
    add_defines("DUILIB_COMMON_TEXT_LAYOUT=1")
end

local function ext_link_core()
    add_linkdirs(DUILIB_LIB)
    add_links("duilib", "duilib-text", "duilib-zlib", "duilib-png", "duilib-cximage", "duilib-webp")
    if ext_skia_enabled() then
        add_links("duilib-skia-base", "duilib-render-skia",
                  "duilib-image-svg-skia", "duilib-image-lottie-skia")
        add_packages("duilib-skia")
    end
    if ext_gdi_enabled() then
        add_links("duilib-render-gdi")
    end
    if ext_sdl_enabled() then
        add_packages("libsdl3")
    end
    if is_plat("windows") then
        add_syslinks("Comctl32", "Imm32", "Opengl32", "User32", "Gdi32", "shlwapi",
                     "Ole32", "OleAut32", "Uuid", "Advapi32", "Shell32", "WinInet", "Ws2_32",
                     "Comdlg32", "Winspool", "Odbc32")
        if ext_gdi_enabled() then
            add_syslinks("Gdiplus", "Msimg32")
        end
        if not (get_config("duilib_runtime") == "MD") then
            add_syslinks("libucrt")
        end
        if ext_sdl_enabled() then
            add_syslinks("Version", "Winmm", "Setupapi")
        end
    elseif is_plat("linux", "freebsd") then
        add_links("X11", "freetype", "fontconfig")
        add_syslinks("pthread", "dl")
    elseif is_plat("macosx") then
        add_frameworks("AppKit", "Foundation", "Metal", "Cocoa", "CoreText",
                       "CoreGraphics", "CoreFoundation", "Accelerate")
        add_syslinks("pthread")
    end
end

target("duilib-scintilla")
    set_kind("static")
    set_targetdir(path.join(EXT_ROOT, "lib"))
    ext_common_settings()
    add_includedirs(
        path.join(EXT_ROOT, "src"),
        DUILIB_SRC,
        DUILIB_DIR,
        path.join(SCINTILLA, "include"),
        path.join(SCINTILLA, "src"),
        path.join(LEXILLA, "include"),
        path.join(LEXILLA, "lexlib")
    )
    add_files(path.join(EXT_ROOT, "src", "*.cpp"))
    add_files(path.join(SCINTILLA, "src", "*.cxx"))
    add_files(path.join(LEXILLA, "lexlib", "*.cxx"))
    add_files(path.join(LEXILLA, "lexers", "*.cxx"))
    add_files(path.join(LEXILLA, "src", "Lexilla.cxx"))
    ext_link_core()
target_end()

if get_config("examples") then
    target("ScintillaDemo")
        set_kind("binary")
        set_targetdir(path.join(EXT_ROOT, "bin"))
        set_rundir(path.join(DUILIB_DIR, "bin"))
        ext_common_settings()
        add_includedirs(path.join(EXT_ROOT, "src"), DUILIB_SRC, DUILIB_DIR,
                        path.join(SCINTILLA, "include"),
                        path.join(SCINTILLA, "src"),
                        path.join(LEXILLA, "include"),
                        path.join(LEXILLA, "lexlib"),
                        path.join(DUILIB_DIR, "examples", "ScintillaDemo"))
        add_files(path.join(DUILIB_DIR, "examples", "ScintillaDemo", "*.cpp"))
        if is_plat("macosx") then
            add_files(path.join(DUILIB_DIR, "examples", "ScintillaDemo", "*.mm"))
        end
        add_deps("duilib-scintilla")
        if is_plat("windows") then
            local demoRc = path.join(DUILIB_DIR, "examples", "ScintillaDemo", "ScintillaDemo.rc")
            if os.isfile(demoRc) then
                add_files(demoRc)
                add_ldflags("/subsystem:windows", {force = true})
            end
        end
        ext_link_core()
    target_end()
end
