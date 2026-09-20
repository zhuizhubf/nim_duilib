-- nim_duilib xmake 构建脚本：可选控件扩展（scintilla / cef / webview2）
-- ---------------------------------------------------------------------------
-- 扩展在 extensions/ 目录下，各自拥有源码、第三方依赖与文档；
-- 目标按配置启用，条件块内的 target 必须显式 target_end()；
-- 源文件逐个列出（不使用通配符）：扩展或上游源码升级时同步修改本文件。

local env = DUILIB
local paths = env.paths

-- duilib-scintilla：DUI 原生 Scintilla 编辑器扩展（--enable_scintilla=y）-------
if env.scintilla_enabled() then
    local extroot = paths.scintilla_dir
    local scroot = path.join(extroot, "third_party", "scintilla")
    local lxroot = path.join(extroot, "third_party", "lexilla")

    target("duilib-scintilla")
        set_kind("static")
        set_targetdir(paths.lib_dir)
        add_rules("duilib.features")
        set_languages("c++17")
        add_defines("SCINTILLA_QT=1")
        add_includedirs(paths.src_dir, paths.root)
        add_includedirs(env.scintilla_includedirs())

        -- extensions/scintilla/src
        add_files(
            path.join(extroot, "src", "ScintillaControl.cpp"),
            path.join(extroot, "src", "ScintillaDui.cpp"),
            path.join(extroot, "src", "ScintillaDuiPlatform.cpp"),
            path.join(extroot, "src", "ScintillaDuiPopup.cpp"),
            path.join(extroot, "src", "ScintillaDuiSurface.cpp"),
            path.join(extroot, "src", "ScintillaDuiTextShaper.cpp"),
            path.join(extroot, "src", "ScintillaManager.cpp")
        )

        -- third_party/scintilla/src
        add_files(
            path.join(scroot, "src", "AutoComplete.cxx"),
            path.join(scroot, "src", "CallTip.cxx"),
            path.join(scroot, "src", "CaseConvert.cxx"),
            path.join(scroot, "src", "CaseFolder.cxx"),
            path.join(scroot, "src", "CellBuffer.cxx"),
            path.join(scroot, "src", "ChangeHistory.cxx"),
            path.join(scroot, "src", "CharacterCategoryMap.cxx"),
            path.join(scroot, "src", "CharacterType.cxx"),
            path.join(scroot, "src", "CharClassify.cxx"),
            path.join(scroot, "src", "ContractionState.cxx"),
            path.join(scroot, "src", "DBCS.cxx"),
            path.join(scroot, "src", "Decoration.cxx"),
            path.join(scroot, "src", "Document.cxx"),
            path.join(scroot, "src", "EditModel.cxx"),
            path.join(scroot, "src", "Editor.cxx"),
            path.join(scroot, "src", "EditView.cxx"),
            path.join(scroot, "src", "Geometry.cxx"),
            path.join(scroot, "src", "Indicator.cxx"),
            path.join(scroot, "src", "KeyMap.cxx"),
            path.join(scroot, "src", "LineMarker.cxx"),
            path.join(scroot, "src", "MarginView.cxx"),
            path.join(scroot, "src", "PerLine.cxx"),
            path.join(scroot, "src", "PositionCache.cxx"),
            path.join(scroot, "src", "RESearch.cxx"),
            path.join(scroot, "src", "RunStyles.cxx"),
            path.join(scroot, "src", "ScintillaBase.cxx"),
            path.join(scroot, "src", "Selection.cxx"),
            path.join(scroot, "src", "Style.cxx"),
            path.join(scroot, "src", "UndoHistory.cxx"),
            path.join(scroot, "src", "UniConversion.cxx"),
            path.join(scroot, "src", "UniqueString.cxx"),
            path.join(scroot, "src", "ViewStyle.cxx"),
            path.join(scroot, "src", "XPM.cxx")
        )

        -- third_party/lexilla/lexlib
        add_files(
            path.join(lxroot, "lexlib", "Accessor.cxx"),
            path.join(lxroot, "lexlib", "CharacterCategory.cxx"),
            path.join(lxroot, "lexlib", "CharacterSet.cxx"),
            path.join(lxroot, "lexlib", "DefaultLexer.cxx"),
            path.join(lxroot, "lexlib", "InList.cxx"),
            path.join(lxroot, "lexlib", "LexAccessor.cxx"),
            path.join(lxroot, "lexlib", "LexerBase.cxx"),
            path.join(lxroot, "lexlib", "LexerModule.cxx"),
            path.join(lxroot, "lexlib", "LexerSimple.cxx"),
            path.join(lxroot, "lexlib", "PropSetSimple.cxx"),
            path.join(lxroot, "lexlib", "StyleContext.cxx"),
            path.join(lxroot, "lexlib", "WordList.cxx")
        )

        -- third_party/lexilla/lexers
        add_files(
            path.join(lxroot, "lexers", "LexA68k.cxx"),
            path.join(lxroot, "lexers", "LexAbaqus.cxx"),
            path.join(lxroot, "lexers", "LexAda.cxx"),
            path.join(lxroot, "lexers", "LexAPDL.cxx"),
            path.join(lxroot, "lexers", "LexAsciidoc.cxx"),
            path.join(lxroot, "lexers", "LexAsm.cxx"),
            path.join(lxroot, "lexers", "LexAsn1.cxx"),
            path.join(lxroot, "lexers", "LexASY.cxx"),
            path.join(lxroot, "lexers", "LexAU3.cxx"),
            path.join(lxroot, "lexers", "LexAVE.cxx"),
            path.join(lxroot, "lexers", "LexAVS.cxx"),
            path.join(lxroot, "lexers", "LexBaan.cxx"),
            path.join(lxroot, "lexers", "LexBash.cxx"),
            path.join(lxroot, "lexers", "LexBasic.cxx"),
            path.join(lxroot, "lexers", "LexBatch.cxx"),
            path.join(lxroot, "lexers", "LexBibTeX.cxx"),
            path.join(lxroot, "lexers", "LexBullant.cxx"),
            path.join(lxroot, "lexers", "LexCaml.cxx"),
            path.join(lxroot, "lexers", "LexCIL.cxx"),
            path.join(lxroot, "lexers", "LexCLW.cxx"),
            path.join(lxroot, "lexers", "LexCmake.cxx"),
            path.join(lxroot, "lexers", "LexCOBOL.cxx"),
            path.join(lxroot, "lexers", "LexCoffeeScript.cxx"),
            path.join(lxroot, "lexers", "LexConf.cxx"),
            path.join(lxroot, "lexers", "LexCPP.cxx"),
            path.join(lxroot, "lexers", "LexCrontab.cxx"),
            path.join(lxroot, "lexers", "LexCsound.cxx"),
            path.join(lxroot, "lexers", "LexCSS.cxx"),
            path.join(lxroot, "lexers", "LexD.cxx"),
            path.join(lxroot, "lexers", "LexDart.cxx"),
            path.join(lxroot, "lexers", "LexDataflex.cxx"),
            path.join(lxroot, "lexers", "LexDiff.cxx"),
            path.join(lxroot, "lexers", "LexDMAP.cxx"),
            path.join(lxroot, "lexers", "LexDMIS.cxx"),
            path.join(lxroot, "lexers", "LexECL.cxx"),
            path.join(lxroot, "lexers", "LexEDIFACT.cxx"),
            path.join(lxroot, "lexers", "LexEiffel.cxx"),
            path.join(lxroot, "lexers", "LexErlang.cxx"),
            path.join(lxroot, "lexers", "LexErrorList.cxx"),
            path.join(lxroot, "lexers", "LexEScript.cxx"),
            path.join(lxroot, "lexers", "LexEscSeq.cxx"),
            path.join(lxroot, "lexers", "LexFlagship.cxx"),
            path.join(lxroot, "lexers", "LexForth.cxx"),
            path.join(lxroot, "lexers", "LexFortran.cxx"),
            path.join(lxroot, "lexers", "LexFSharp.cxx"),
            path.join(lxroot, "lexers", "LexGAP.cxx"),
            path.join(lxroot, "lexers", "LexGDScript.cxx"),
            path.join(lxroot, "lexers", "LexGui4Cli.cxx"),
            path.join(lxroot, "lexers", "LexHaskell.cxx"),
            path.join(lxroot, "lexers", "LexHex.cxx"),
            path.join(lxroot, "lexers", "LexHollywood.cxx"),
            path.join(lxroot, "lexers", "LexHTML.cxx"),
            path.join(lxroot, "lexers", "LexIndent.cxx"),
            path.join(lxroot, "lexers", "LexInno.cxx"),
            path.join(lxroot, "lexers", "LexJSON.cxx"),
            path.join(lxroot, "lexers", "LexJulia.cxx"),
            path.join(lxroot, "lexers", "LexKix.cxx"),
            path.join(lxroot, "lexers", "LexKVIrc.cxx"),
            path.join(lxroot, "lexers", "LexLaTeX.cxx"),
            path.join(lxroot, "lexers", "LexLisp.cxx"),
            path.join(lxroot, "lexers", "LexLout.cxx"),
            path.join(lxroot, "lexers", "LexLua.cxx"),
            path.join(lxroot, "lexers", "LexMagik.cxx"),
            path.join(lxroot, "lexers", "LexMake.cxx"),
            path.join(lxroot, "lexers", "LexMarkdown.cxx"),
            path.join(lxroot, "lexers", "LexMatlab.cxx"),
            path.join(lxroot, "lexers", "LexMaxima.cxx"),
            path.join(lxroot, "lexers", "LexMetapost.cxx"),
            path.join(lxroot, "lexers", "LexMMIXAL.cxx"),
            path.join(lxroot, "lexers", "LexModula.cxx"),
            path.join(lxroot, "lexers", "LexMPT.cxx"),
            path.join(lxroot, "lexers", "LexMSSQL.cxx"),
            path.join(lxroot, "lexers", "LexMySQL.cxx"),
            path.join(lxroot, "lexers", "LexNim.cxx"),
            path.join(lxroot, "lexers", "LexNimrod.cxx"),
            path.join(lxroot, "lexers", "LexNix.cxx"),
            path.join(lxroot, "lexers", "LexNsis.cxx"),
            path.join(lxroot, "lexers", "LexNull.cxx"),
            path.join(lxroot, "lexers", "LexOpal.cxx"),
            path.join(lxroot, "lexers", "LexOScript.cxx"),
            path.join(lxroot, "lexers", "LexPascal.cxx"),
            path.join(lxroot, "lexers", "LexPB.cxx"),
            path.join(lxroot, "lexers", "LexPerl.cxx"),
            path.join(lxroot, "lexers", "LexPLM.cxx"),
            path.join(lxroot, "lexers", "LexPO.cxx"),
            path.join(lxroot, "lexers", "LexPOV.cxx"),
            path.join(lxroot, "lexers", "LexPowerPro.cxx"),
            path.join(lxroot, "lexers", "LexPowerShell.cxx"),
            path.join(lxroot, "lexers", "LexProgress.cxx"),
            path.join(lxroot, "lexers", "LexProps.cxx"),
            path.join(lxroot, "lexers", "LexPS.cxx"),
            path.join(lxroot, "lexers", "LexPython.cxx"),
            path.join(lxroot, "lexers", "LexR.cxx"),
            path.join(lxroot, "lexers", "LexRaku.cxx"),
            path.join(lxroot, "lexers", "LexRebol.cxx"),
            path.join(lxroot, "lexers", "LexRegistry.cxx"),
            path.join(lxroot, "lexers", "LexRuby.cxx"),
            path.join(lxroot, "lexers", "LexRust.cxx"),
            path.join(lxroot, "lexers", "LexSAS.cxx"),
            path.join(lxroot, "lexers", "LexScriptol.cxx"),
            path.join(lxroot, "lexers", "LexSINEX.cxx"),
            path.join(lxroot, "lexers", "LexSmalltalk.cxx"),
            path.join(lxroot, "lexers", "LexSML.cxx"),
            path.join(lxroot, "lexers", "LexSorcus.cxx"),
            path.join(lxroot, "lexers", "LexSpecman.cxx"),
            path.join(lxroot, "lexers", "LexSpice.cxx"),
            path.join(lxroot, "lexers", "LexSQL.cxx"),
            path.join(lxroot, "lexers", "LexStata.cxx"),
            path.join(lxroot, "lexers", "LexSTTXT.cxx"),
            path.join(lxroot, "lexers", "LexTACL.cxx"),
            path.join(lxroot, "lexers", "LexTADS3.cxx"),
            path.join(lxroot, "lexers", "LexTAL.cxx"),
            path.join(lxroot, "lexers", "LexTCL.cxx"),
            path.join(lxroot, "lexers", "LexTCMD.cxx"),
            path.join(lxroot, "lexers", "LexTeX.cxx"),
            path.join(lxroot, "lexers", "LexTOML.cxx"),
            path.join(lxroot, "lexers", "LexTroff.cxx"),
            path.join(lxroot, "lexers", "LexTxt2tags.cxx"),
            path.join(lxroot, "lexers", "LexVB.cxx"),
            path.join(lxroot, "lexers", "LexVerilog.cxx"),
            path.join(lxroot, "lexers", "LexVHDL.cxx"),
            path.join(lxroot, "lexers", "LexVisualProlog.cxx"),
            path.join(lxroot, "lexers", "LexX12.cxx"),
            path.join(lxroot, "lexers", "LexYAML.cxx"),
            path.join(lxroot, "lexers", "LexZig.cxx")
        )

        -- third_party/lexilla/src
        add_files(
            path.join(lxroot, "src", "Lexilla.cxx")
        )

        -- ScintillaControl.cpp 在 DUILIB_BUILD_FOR_SDL 下直接调用 SDL3 API（文本输入事件），
        -- 非 Windows 平台始终成立、Windows 上由 --enable_sdl=y 决定，需要显式引用包
        -- 才能拿到 SDL3 的头文件（否则报 SDL3/SDL_events.h 找不到）
        if env.sdl_enabled() then
            add_packages("libsdl3")
        end

        add_deps("duilib", "duilib-text")
    target_end()
end

-- duilib-cef：CEF 控件模块（--with_cef=latest|109，仅 Windows 支持）-----------
if env.cef_enabled() and not is_plat("freebsd") then
    target("duilib-cef")
        set_kind("static")
        set_targetdir(paths.lib_dir)
        add_rules("duilib.features")
        set_languages("c++17")
        add_deps("duilib")

        -- extensions/cef/src/cef
        add_files(
            path.join(paths.cef_dir, "src", "cef", "CefControl.cpp"),
            path.join(paths.cef_dir, "src", "cef", "CefControlNative.cpp"),
            path.join(paths.cef_dir, "src", "cef", "CefControlOffScreen.cpp"),
            path.join(paths.cef_dir, "src", "cef", "CefManager.cpp"),
            path.join(paths.cef_dir, "src", "cef", "CefManager_Linux.cpp"),
            path.join(paths.cef_dir, "src", "cef", "CefManager_MacOS.cpp"),
            path.join(paths.cef_dir, "src", "cef", "CefManager_Windows.cpp"),
            path.join(paths.cef_dir, "src", "cef", "CefWindowUtils_Linux.cpp"),
            path.join(paths.cef_dir, "src", "cef", "CefWindowUtils_Windows.cpp")
        )
        if is_plat("macosx") then
            add_files(
                path.join(paths.cef_dir, "src", "cef", "CefWindowUtils_MacOS.mm")
            )
        end

        -- extensions/cef/src/cef/internal
        add_files(
            path.join(paths.cef_dir, "src", "cef", "internal", "CefBrowserHandler.cpp"),
            path.join(paths.cef_dir, "src", "cef", "internal", "CefClientApp.cpp"),
            path.join(paths.cef_dir, "src", "cef", "internal", "CefClientAppBrowser.cpp"),
            path.join(paths.cef_dir, "src", "cef", "internal", "CefClientAppRender.cpp"),
            path.join(paths.cef_dir, "src", "cef", "internal", "CefJSBridge.cpp"),
            path.join(paths.cef_dir, "src", "cef", "internal", "CefJsHandler.cpp"),
            path.join(paths.cef_dir, "src", "cef", "internal", "CefMemoryBlock.cpp"),
            path.join(paths.cef_dir, "src", "cef", "internal", "CefRegisteredFunctions.cpp")
        )

        -- extensions/cef/src/cef/internal/Windows
        if is_plat("windows") then
            add_files(
                path.join(paths.cef_dir, "src", "cef", "internal", "Windows", "CefOsrDropTarget.cpp"),
                path.join(paths.cef_dir, "src", "cef", "internal", "Windows", "bytes_write_handler.cc"),
                path.join(paths.cef_dir, "src", "cef", "internal", "Windows", "osr_dragdrop_win.cc"),
                path.join(paths.cef_dir, "src", "cef", "internal", "Windows", "osr_ime_handler_win.cc"),
                path.join(paths.cef_dir, "src", "cef", "internal", "Windows", "util_win.cc")
            )
        end

        add_includedirs(paths.src_dir, paths.root)
        add_includedirs(env.cef_includedirs())
    target_end()
end

-- duilib-webview2：WebView2 控件模块（Windows，默认启用）----------------------
if env.webview2_enabled() then
    target("duilib-webview2")
        set_kind("static")
        set_targetdir(paths.lib_dir)
        add_rules("duilib.features")
        add_deps("duilib")
        add_files(
            path.join(paths.webview2_dir, "src", "webview2", "WebView2Control.cpp"),
            path.join(paths.webview2_dir, "src", "webview2", "WebView2ControlImpl.cpp"),
            path.join(paths.webview2_dir, "src", "webview2", "WebView2EnvironmentOptions.cpp"),
            path.join(paths.webview2_dir, "src", "webview2", "WebView2Manager.cpp")
        )
        add_includedirs(paths.src_dir, paths.root)
        add_includedirs(env.webview2_includedirs())
        add_linkdirs(path.join(env.webview2_sdk_dir(), "build/native", env.arch_name()))
        add_links("WebView2LoaderStatic")
        add_syslinks("advapi32", "ole32", "shell32", "version", "wininet")
    target_end()
end
