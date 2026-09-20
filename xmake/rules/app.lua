-- rule("duilib.app")：可执行程序（示例）的平台链接配置
-- ---------------------------------------------------------------------------
-- Windows：窗口子系统 + 嵌入 manifest + 系统库；CEF 示例额外使用 /DELAYLOAD；
-- Linux/FreeBSD：X11 与 Skia 字体后端依赖的系统库（必须排在静态库之后）；
-- macOS：AppKit/Metal 等框架。源码与依赖仍写在各自 target 体内。

local env = DUILIB

rule("duilib.app")
    add_deps("duilib.features")
    on_load(function (target)
        if target:is_plat("windows") then
            -- 系统库：MSVC 链接时不会自动带上 gdi32/oleaut32 等，这里按需显式列出
            target:add("syslinks", "Comctl32", "Imm32", "Opengl32", "User32", "Gdi32", "shlwapi",
                       "Ole32", "OleAut32", "Uuid", "Advapi32", "Shell32", "WinInet", "Ws2_32",
                       "Comdlg32", "Winspool", "Odbc32")
            if env.render_gdi_enabled() then
                target:add("syslinks", "Gdiplus", "Msimg32")
            end
            if env.runtime() == "MT" then
                -- 使用静态运行库(/MT)时，需要显式链接静态 UCRT（Skia 的数学函数依赖它）
                target:add("syslinks", "libucrt")
            end
            if env.sdl_enabled() then
                target:add("syslinks", "Version", "Winmm", "Setupapi")
            end
            if env.is_cef_example(target:name()) and env.cef_enabled() then
                target:add("ldflags", "/DELAYLOAD:libcef.dll", {force = true})
                -- 使用 /DELAYLOAD 时需要延迟加载助手（__delayLoadHelper2）
                target:add("syslinks", "delayimp")
            end
        elseif target:is_plat("linux") then
            -- 系统库放在最后（syslinks）：Skia 静态库引用了 freetype/fontconfig，
            -- 必须排在 Skia 的静态库之后，否则 GNU ld 会报 FT_* 未定义符号
            target:add("syslinks", "X11", "freetype", "fontconfig", "pthread", "dl")
        elseif target:is_plat("macosx") then
            target:add("frameworks", "AppKit", "Foundation", "Metal", "Cocoa", "CoreText",
                       "CoreGraphics", "CoreFoundation", "Accelerate")
            target:add("syslinks", "pthread")
        elseif target:is_plat("freebsd") then
            target:add("syslinks", "X11", "freetype", "fontconfig", "pthread", "dl")
        end
    end)
