-- nim_duilib xmake 构建脚本：第三方库
-- ---------------------------------------------------------------------------
-- 源码全部取自仓库，编译方式与项目原有构建方式保持一致；
-- 源文件逐个列出（不使用通配符）：升级第三方源码时需要同步修改本文件。

local env = DUILIB
local paths = env.paths
local thirdroot = paths.third_dir

-- check_cincludes 等编译期探测接口（只有 zlib 的头文件探测用到，就近引入）
includes("@builtin/check")

-- zlib ----------------------------------------------------------------------
-- 与 xmake 官方 zlib 包（xmake-repo/packages/z/zlib）保持一致：不编译示例程序
-- （example.c / minigzip.c 里有 main 函数）
target("duilib-zlib")
    set_kind("static")
    set_targetdir(paths.lib_dir)
    add_rules("duilib.config")
    add_files(
        path.join(thirdroot, "zlib", "adler32.c"),
        path.join(thirdroot, "zlib", "compress.c"),
        path.join(thirdroot, "zlib", "crc32.c"),
        path.join(thirdroot, "zlib", "deflate.c"),
        path.join(thirdroot, "zlib", "gzclose.c"),
        path.join(thirdroot, "zlib", "gzlib.c"),
        path.join(thirdroot, "zlib", "gzread.c"),
        path.join(thirdroot, "zlib", "gzwrite.c"),
        path.join(thirdroot, "zlib", "infback.c"),
        path.join(thirdroot, "zlib", "inffast.c"),
        path.join(thirdroot, "zlib", "inflate.c"),
        path.join(thirdroot, "zlib", "inftrees.c"),
        path.join(thirdroot, "zlib", "trees.c"),
        path.join(thirdroot, "zlib", "uncompr.c"),
        path.join(thirdroot, "zlib", "zutil.c")
    )
    add_includedirs(path.join(thirdroot, "zlib"))
    -- zconf.h 依据这些宏决定是否包含 <unistd.h>/<sys/types.h>/<stdint.h>/<stddef.h>
    -- （gzread.c 等需要 read/close）。官方包是用 check_cincludes 检测后再定义，这里保持一致，
    -- 避免硬编码平台假设：macOS（clang，C99 起隐式函数声明为错误）缺少该宏时会报 read/close 未声明。
    check_cincludes("Z_HAVE_UNISTD_H", "unistd.h")
    check_cincludes("HAVE_SYS_TYPES_H", "sys/types.h")
    check_cincludes("HAVE_STDINT_H", "stdint.h")
    check_cincludes("HAVE_STDDEF_H", "stddef.h")
    if is_plat("windows") then
        add_defines("_CRT_SECURE_NO_DEPRECATE", "_CRT_NONSTDC_NO_DEPRECATE")
    else
        -- 与官方包一致：启用大文件接口（64 位文件偏移）
        add_defines("_LARGEFILE64_SOURCE=1")
    end

-- libpng（使用仓库内已经打过 APNG 补丁的源码）---------------------------------
target("duilib-png")
    set_kind("static")
    set_targetdir(paths.lib_dir)
    add_rules("duilib.config")
    add_deps("duilib-zlib")
    add_includedirs(path.join(thirdroot, "libpng"), path.join(thirdroot, "zlib"))
    -- 不编译自带的测试程序（pngtest.c）
    add_files(
        path.join(thirdroot, "libpng", "example.c"),
        path.join(thirdroot, "libpng", "png.c"),
        path.join(thirdroot, "libpng", "pngerror.c"),
        path.join(thirdroot, "libpng", "pngget.c"),
        path.join(thirdroot, "libpng", "pngmem.c"),
        path.join(thirdroot, "libpng", "pngpread.c"),
        path.join(thirdroot, "libpng", "pngread.c"),
        path.join(thirdroot, "libpng", "pngrio.c"),
        path.join(thirdroot, "libpng", "pngrtran.c"),
        path.join(thirdroot, "libpng", "pngrutil.c"),
        path.join(thirdroot, "libpng", "pngset.c"),
        path.join(thirdroot, "libpng", "pngtrans.c"),
        path.join(thirdroot, "libpng", "pngwio.c"),
        path.join(thirdroot, "libpng", "pngwrite.c"),
        path.join(thirdroot, "libpng", "pngwtran.c"),
        path.join(thirdroot, "libpng", "pngwutil.c")
    )
    local arch = env.arch_name()
    if arch == "x86" or arch == "x64" then
        -- x86/x64 使用 SSE2 优化（与 libpng 官方配置一致）
        add_files(
            path.join(thirdroot, "libpng", "intel", "filter_sse2_intrinsics.c"),
            path.join(thirdroot, "libpng", "intel", "intel_init.c")
        )
        add_defines("PNG_INTEL_SSE_OPT=1")
    elseif arch == "arm64" then
        add_files(
            path.join(thirdroot, "libpng", "arm", "arm_init.c"),
            path.join(thirdroot, "libpng", "arm", "filter_neon_intrinsics.c"),
            path.join(thirdroot, "libpng", "arm", "palette_neon_intrinsics.c")
        )
        add_defines("PNG_ARM_NEON_OPT=2")
    end

-- cximage -------------------------------------------------------------------
target("duilib-cximage")
    set_kind("static")
    set_targetdir(paths.lib_dir)
    add_rules("duilib.config")
    set_languages("c++17")
    add_deps("duilib-png", "duilib-zlib")
    add_includedirs(path.join(thirdroot, "cximage"), path.join(thirdroot, "libpng"),
                    path.join(thirdroot, "zlib"), paths.src_dir)
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

-- libwebp（只使用解码、解复用与工具代码，源码列表与原构建方式一致）-------------
target("duilib-webp")
    set_kind("static")
    set_targetdir(paths.lib_dir)
    add_rules("duilib.config")
    add_includedirs(path.join(thirdroot, "libwebp"))
    -- src/dec
    add_files(
        path.join(thirdroot, "libwebp", "src", "dec", "alpha_dec.c"),
        path.join(thirdroot, "libwebp", "src", "dec", "buffer_dec.c"),
        path.join(thirdroot, "libwebp", "src", "dec", "frame_dec.c"),
        path.join(thirdroot, "libwebp", "src", "dec", "idec_dec.c"),
        path.join(thirdroot, "libwebp", "src", "dec", "io_dec.c"),
        path.join(thirdroot, "libwebp", "src", "dec", "quant_dec.c"),
        path.join(thirdroot, "libwebp", "src", "dec", "tree_dec.c"),
        path.join(thirdroot, "libwebp", "src", "dec", "vp8_dec.c"),
        path.join(thirdroot, "libwebp", "src", "dec", "vp8l_dec.c"),
        path.join(thirdroot, "libwebp", "src", "dec", "webp_dec.c")
    )
    -- src/demux
    add_files(
        path.join(thirdroot, "libwebp", "src", "demux", "anim_decode.c"),
        path.join(thirdroot, "libwebp", "src", "demux", "demux.c")
    )
    -- src/dsp（SIMD 分支由各源文件内部的宏自行裁剪）
    add_files(
        path.join(thirdroot, "libwebp", "src", "dsp", "alpha_processing.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "alpha_processing_neon.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "alpha_processing_sse2.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "alpha_processing_sse41.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "cpu.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "dec.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "dec_clip_tables.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "dec_neon.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "dec_sse2.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "dec_sse41.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "filters.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "filters_neon.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "filters_sse2.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "lossless.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "lossless_avx2.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "lossless_neon.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "lossless_sse2.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "lossless_sse41.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "rescaler.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "rescaler_neon.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "rescaler_sse2.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "upsampling.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "upsampling_neon.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "upsampling_sse2.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "upsampling_sse41.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "yuv.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "yuv_neon.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "yuv_sse2.c"),
        path.join(thirdroot, "libwebp", "src", "dsp", "yuv_sse41.c")
    )
    -- src/utils
    add_files(
        path.join(thirdroot, "libwebp", "src", "utils", "bit_reader_utils.c"),
        path.join(thirdroot, "libwebp", "src", "utils", "color_cache_utils.c"),
        path.join(thirdroot, "libwebp", "src", "utils", "filters_utils.c"),
        path.join(thirdroot, "libwebp", "src", "utils", "huffman_utils.c"),
        path.join(thirdroot, "libwebp", "src", "utils", "palette.c"),
        path.join(thirdroot, "libwebp", "src", "utils", "quant_levels_dec_utils.c"),
        path.join(thirdroot, "libwebp", "src", "utils", "random_utils.c"),
        path.join(thirdroot, "libwebp", "src", "utils", "rescaler_utils.c"),
        path.join(thirdroot, "libwebp", "src", "utils", "thread_utils.c"),
        path.join(thirdroot, "libwebp", "src", "utils", "utils.c")
    )

-- CEF 封装库（libcef_dll_wrapper，仅 --with_cef=latest|109 时编译）-------------
-- 源文件按 CEF SDK 版本（libcef_win / libcef_win_109）分别列出，升级 SDK 时同步更新。
if env.cef_enabled() then
    local cefroot = env.cef_sdk_dir()
    local wraproot = path.join(cefroot, "libcef_dll")
    local cefarch = (env.arch_name() == "x86") and "Win32" or "x64"

    target(env.cef_wrapper_name())
        set_kind("static")
        set_targetdir(paths.lib_dir)
        add_rules("duilib.config")
        set_languages("c++17")

        if env.cef_version() == "109" then
            -- libcef_win_109/libcef_dll
            add_files(
                path.join(wraproot, "shutdown_checker.cc"),
                path.join(wraproot, "transfer_util.cc"),
                path.join(wraproot, "base", "cef_atomic_flag.cc"),
                path.join(wraproot, "base", "cef_callback_helpers.cc"),
                path.join(wraproot, "base", "cef_callback_internal.cc"),
                path.join(wraproot, "base", "cef_lock_impl.cc"),
                path.join(wraproot, "base", "cef_lock.cc"),
                path.join(wraproot, "base", "cef_logging.cc"),
                path.join(wraproot, "base", "cef_ref_counted.cc"),
                path.join(wraproot, "base", "cef_thread_checker_impl.cc"),
                path.join(wraproot, "base", "cef_weak_ptr.cc"),
                path.join(wraproot, "cpptoc", "accessibility_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "app_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "audio_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "base_ref_counted_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "base_scoped_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "browser_process_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "client_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "command_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "completion_callback_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "context_menu_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "cookie_access_filter_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "cookie_visitor_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "delete_cookies_callback_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "dev_tools_message_observer_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "dialog_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "display_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "domvisitor_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "download_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "download_image_callback_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "drag_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "end_tracing_callback_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "extension_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "find_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "focus_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "frame_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "jsdialog_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "keyboard_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "life_span_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "load_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "media_observer_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "media_route_create_callback_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "media_sink_device_info_callback_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "menu_model_delegate_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "navigation_entry_visitor_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "pdf_print_callback_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "permission_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "print_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "read_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "render_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "render_process_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "request_context_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "request_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "resolve_callback_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "resource_bundle_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "resource_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "resource_request_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "response_filter_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "run_file_dialog_callback_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "scheme_handler_factory_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "server_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "set_cookie_callback_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "string_visitor_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "task_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "urlrequest_client_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "v8accessor_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "v8array_buffer_release_callback_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "v8handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "v8interceptor_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "write_handler_cpptoc.cc"),
                path.join(wraproot, "ctocpp", "auth_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "before_download_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "binary_value_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "browser_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "browser_host_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "command_line_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "context_menu_params_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "cookie_manager_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "dictionary_value_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "domdocument_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "domnode_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "download_item_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "download_item_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "drag_data_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "extension_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "file_dialog_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "frame_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "get_extension_resource_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "image_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "jsdialog_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "list_value_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "media_access_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "media_route_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "media_router_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "media_sink_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "media_source_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "menu_model_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "navigation_entry_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "permission_prompt_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "post_data_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "post_data_element_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "preference_manager_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "preference_registrar_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "print_dialog_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "print_job_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "print_settings_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "process_message_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "registration_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "request_context_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "request_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "resource_bundle_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "resource_read_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "resource_skip_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "response_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "run_context_menu_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "run_quick_menu_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "scheme_registrar_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "select_client_certificate_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "server_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "shared_memory_region_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "shared_process_message_builder_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "sslinfo_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "sslstatus_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "stream_reader_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "stream_writer_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "task_runner_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "thread_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "urlrequest_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "v8context_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "v8exception_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "v8stack_frame_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "v8stack_trace_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "v8value_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "value_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "waitable_event_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "x509cert_principal_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "x509certificate_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "xml_reader_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "zip_reader_ctocpp.cc"),
                path.join(wraproot, "wrapper", "cef_byte_read_handler.cc"),
                path.join(wraproot, "wrapper", "cef_closure_task.cc"),
                path.join(wraproot, "wrapper", "cef_message_router.cc"),
                path.join(wraproot, "wrapper", "cef_resource_manager.cc"),
                path.join(wraproot, "wrapper", "cef_scoped_temp_dir.cc"),
                path.join(wraproot, "wrapper", "cef_stream_resource_handler.cc"),
                path.join(wraproot, "wrapper", "cef_xml_object.cc"),
                path.join(wraproot, "wrapper", "cef_zip_archive.cc"),
                path.join(wraproot, "wrapper", "libcef_dll_wrapper.cc"),
                path.join(wraproot, "wrapper", "libcef_dll_wrapper2.cc")
            )
        else
            -- libcef_win/libcef_dll
            add_files(
                path.join(wraproot, "shutdown_checker.cc"),
                path.join(wraproot, "transfer_util.cc"),
                path.join(wraproot, "base", "cef_atomic_flag.cc"),
                path.join(wraproot, "base", "cef_callback_helpers.cc"),
                path.join(wraproot, "base", "cef_callback_internal.cc"),
                path.join(wraproot, "base", "cef_dump_without_crashing.cc"),
                path.join(wraproot, "base", "cef_lock_impl.cc"),
                path.join(wraproot, "base", "cef_lock.cc"),
                path.join(wraproot, "base", "cef_logging.cc"),
                path.join(wraproot, "base", "cef_ref_counted.cc"),
                path.join(wraproot, "base", "cef_thread_checker_impl.cc"),
                path.join(wraproot, "base", "cef_weak_ptr.cc"),
                path.join(wraproot, "cpptoc", "accessibility_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "app_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "audio_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "base_ref_counted_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "base_scoped_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "browser_process_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "client_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "command_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "completion_callback_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "context_menu_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "cookie_access_filter_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "cookie_visitor_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "delete_cookies_callback_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "dev_tools_message_observer_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "dialog_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "display_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "domvisitor_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "download_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "download_image_callback_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "drag_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "end_tracing_callback_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "find_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "focus_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "frame_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "jsdialog_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "keyboard_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "life_span_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "load_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "media_observer_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "media_route_create_callback_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "media_sink_device_info_callback_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "menu_model_delegate_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "navigation_entry_visitor_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "pdf_print_callback_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "permission_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "preference_observer_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "print_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "read_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "render_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "render_process_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "request_context_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "request_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "resolve_callback_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "resource_bundle_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "resource_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "resource_request_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "response_filter_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "run_file_dialog_callback_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "scheme_handler_factory_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "server_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "set_cookie_callback_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "setting_observer_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "string_visitor_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "task_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "urlrequest_client_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "v8_accessor_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "v8_array_buffer_release_callback_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "v8_handler_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "v8_interceptor_cpptoc.cc"),
                path.join(wraproot, "cpptoc", "write_handler_cpptoc.cc"),
                path.join(wraproot, "ctocpp", "auth_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "before_download_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "binary_value_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "browser_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "browser_host_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "command_line_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "context_menu_params_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "cookie_manager_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "dictionary_value_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "domdocument_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "domnode_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "download_item_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "download_item_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "drag_data_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "file_dialog_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "frame_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "image_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "jsdialog_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "list_value_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "media_access_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "media_route_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "media_router_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "media_sink_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "media_source_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "menu_model_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "navigation_entry_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "permission_prompt_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "post_data_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "post_data_element_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "preference_manager_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "preference_registrar_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "print_dialog_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "print_job_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "print_settings_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "process_message_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "registration_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "request_context_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "request_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "resource_bundle_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "resource_read_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "resource_skip_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "response_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "run_context_menu_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "run_quick_menu_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "scheme_registrar_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "select_client_certificate_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "server_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "shared_memory_region_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "shared_process_message_builder_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "sslinfo_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "sslstatus_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "stream_reader_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "stream_writer_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "task_manager_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "task_runner_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "thread_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "unresponsive_process_callback_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "urlrequest_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "v8_context_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "v8_exception_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "v8_stack_frame_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "v8_stack_trace_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "v8_value_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "value_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "waitable_event_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "x509_cert_principal_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "x509_certificate_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "xml_reader_ctocpp.cc"),
                path.join(wraproot, "ctocpp", "zip_reader_ctocpp.cc"),
                path.join(wraproot, "wrapper", "cef_byte_read_handler.cc"),
                path.join(wraproot, "wrapper", "cef_certificate_util_win.cc"),
                path.join(wraproot, "wrapper", "cef_closure_task.cc"),
                path.join(wraproot, "wrapper", "cef_message_router.cc"),
                path.join(wraproot, "wrapper", "cef_message_router_utils.cc"),
                path.join(wraproot, "wrapper", "cef_resource_manager.cc"),
                path.join(wraproot, "wrapper", "cef_scoped_library_loader_win.cc"),
                path.join(wraproot, "wrapper", "cef_scoped_temp_dir.cc"),
                path.join(wraproot, "wrapper", "cef_stream_resource_handler.cc"),
                path.join(wraproot, "wrapper", "cef_util_win.cc"),
                path.join(wraproot, "wrapper", "cef_xml_object.cc"),
                path.join(wraproot, "wrapper", "cef_zip_archive.cc"),
                path.join(wraproot, "wrapper", "libcef_dll_wrapper.cc"),
                path.join(wraproot, "wrapper", "libcef_dll_wrapper2.cc")
            )
        end

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
            if not target:is_plat("windows") then
                raise("xmake 构建脚本目前只支持在 Windows 平台启用 CEF，其他平台暂不支持（可参见 docs/CEF.md）")
            end
        end)
    target_end()
end
