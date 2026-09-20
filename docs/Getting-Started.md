# 快速上手（Windows系统，以VS 2022为例）

此示例将引导你快速部署一个基于 nim_duilib 的基本应用，此示例与 `examples` 中的 `basic` 项目相似，可以直接参考 `examples/basic` 的代码（构建方式见 [README](../README.md) 中的“编译（xmake）”章节）。

## 获取项目代码并编译

1. 获取项目代码

```bash
git clone https://github.com/rhett-lee/nim_duilib
```

2. 编译 nim_duilib：进入 `nim_duilib` 目录，执行以下命令（首次配置会自动下载并编译 Skia：默认使用 MSVC，**不需要安装 LLVM**）：

```bash
xmake f -o build/build_temp/xmake -c     # 配置（首次会自动下载并编译 Skia，约 5 分钟）
xmake                                    # 编译：第三方库 + duilib + 全部示例程序
```

3. 编译完成后，示例程序生成在 `bin` 目录中，可直接运行 `bin\basic.exe` 查看效果（也可以执行 `xmake run basic`）。

## 创建基础工程

本项目使用 xmake 构建，新增一个示例程序的步骤如下（可参考 `examples/basic`）：

1. 在 `examples` 目录下新建一个子目录（例如 `examples/MyDuilibApp`），放入程序源码：Windows 入口文件（参考 `examples/basic/main_windows.cpp`）、`MainThread.h/.cpp`、`MainForm.h/.cpp`、`.rc` 资源文件等；

入口函数的形式如下：
```cpp
#include "MainThread.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    MainThread thread;
    thread.Start();
    return 0;
}
```

2. 在 [xmake/env.lua](../xmake/env.lua) 的 `common_examples` 列表中登记新的示例名称；
3. 重新配置并编译：

```bash
xmake f -o build/build_temp/xmake -c
xmake build MyDuilibApp
xmake run MyDuilibApp
```

示例程序的工程配置（头文件路径、系统库、manifest、资源文件、duilib 依赖等）由 [xmake/targets/examples.lua](../xmake/targets/examples.lua) 与 `duilib.app` 规则统一设置，不需要手工配置工程。

## 引入线程库

在创建的项目中增加自定义的线程类MainThread（主线程和一个工作线程）    
创建两个文件（`MainThread.h` 和 `MainThread.cpp`），放在示例的源码目录中（xmake 会自动收录目录下的源码文件），两个文件的内容分别如下：

MainThread.h    
```cpp
#ifndef EXAMPLES_MAIN_THREAD_H_
#define EXAMPLES_MAIN_THREAD_H_

// duilib
#include "duilib/duilib.h"

/** 主线程
*/
class MainThread : public ui::FrameworkThread
{
public:
    MainThread();
    virtual ~MainThread() override;

private:
    /** 运行前初始化，在进入消息循环前调用，如果初始化失败则不进入消息循环
    * @return 初始化成功返回true，初始化失败返回false
    */
    virtual bool OnInit() override;

    /** 退出时清理，在退出消息循环后调用
    */
    virtual void OnCleanup() override;
};

#endif // EXAMPLES_MAIN_THREAD_H_
```

MainThread.cpp    
```cpp
#include "MainThread.h"
#include "MainForm.h"

MainThread::MainThread() :
    FrameworkThread(_T("MainThread"), ui::kThreadUI)
{
}

MainThread::~MainThread()
{
}

bool MainThread::OnInit()
{
    //初始化全局资源, 使用本地文件夹作为资源
    ui::FilePath resourcePath = ui::GlobalManager::GetResourceRootPath(false);
    ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath));

    //在下面加入启动窗口代码
    //
    //创建一个默认带有阴影的居中窗口
    MainForm* window = new MainForm();
    window->CreateWnd(nullptr, ui::WindowCreateParam(_T("MyDuilibApp"), true));
    window->PostQuitMsgWhenClosed(true);
    window->ShowWindow(ui::kSW_SHOW_NORMAL);
    return true;
}

void MainThread::OnCleanup()
{
    ui::GlobalManager::Instance().Shutdown();
}
```

在 wWinMain 实例化主线程对象，并调用执行主线程循环，添加后 wWinMain 函数修改如下：

```cpp
#include "MainThread.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    //创建主线程
    MainThread thread;

    //执行主线程消息循环
    thread.RunMessageLoop();

    //正常退出程序
    return 0;
}
```

## 创建一个简单窗口

创建一个窗口类MainForm，继承 `ui::WindowImplBase` 类，并覆写 `GetSkinFolder` `GetSkinFile` 等方法。

```cpp
//MainForm.h
#ifndef EXAMPLES_MAIN_FORM_H_
#define EXAMPLES_MAIN_FORM_H_

// duilib
#include "duilib/duilib.h"

/** 应用程序的主窗口实现
*/
class MainForm : public ui::WindowImplBase
{
    typedef ui::WindowImplBase BaseClass;
public:
    MainForm();
    virtual ~MainForm() override;

    /**  创建窗口时被调用，由子类实现用以获取窗口皮肤目录
    * @return 子类需实现并返回窗口皮肤目录
    */
    virtual DString GetSkinFolder() override;

    /**  创建窗口时被调用，由子类实现用以获取窗口皮肤 XML 描述文件
    * @return 子类需实现并返回窗口皮肤 XML 描述文件
    *         返回的内容，可以是XML文件内容（以字符'<'为开始的字符串），
    *         或者是文件路径（不是以'<'字符开始的字符串），文件要在GetSkinFolder()路径中能够找到
    */
    virtual DString GetSkinFile() override;

    /** 当窗口创建完成以后调用此函数，供子类中做一些初始化的工作
    */
    virtual void OnInitWindow() override;
};

#endif //EXAMPLES_MAIN_FORM_H_
```

```cpp
//MainForm.cpp
#include "MainForm.h"

MainForm::MainForm()
{
}

MainForm::~MainForm()
{
}

DString MainForm::GetSkinFolder()
{
    return _T("my_duilib_app");
}

DString MainForm::GetSkinFile()
{
    return _T("MyDuilibForm.xml");
}

void MainForm::OnInitWindow()
{
    BaseClass::OnInitWindow();
    //窗口初始化完成，可以进行本Form的初始化

}
```

## 创建窗口描述 XML 文件

在我们创建的窗口类中，指定了窗口描述文件目录是 `my_duilib_app`，指定窗口的描述文件为 `MyDuilibForm.xml`。
接下来在 `bin\resources\themes\default` 目录下创建 `my_duilib_app` 文件夹并新建一个 `MyDuilibForm.xml` 文件，写下如下内容。    
注意事项：XML文件的编码格式是UTF-8。  
```xml
<?xml version="1.0" encoding="UTF-8"?>
<Window size="75%,75%" min_size="240,100"
        caption="0,0,0,36" use_system_caption="false" snap_layout_menu="true" sys_menu="true" sys_menu_rect="0,0,36,36"
        shadow_type="default" shadow_snap="true"
        size_box="4,4,4,4" icon="public/caption/logo.ico">
    <!-- 整个窗口中，所有控件都放在这个VBox容器中 -->
    <VBox bkcolor="bg_window_main">
        <!-- 标题栏区域 -->
        <HBox name="window_title_bar" width="stretch" height="36" bkcolor="bg_titlebar">
            <!-- 标题栏：窗口左上角显示区域 -->
            <Control mouse_enabled="false"/>
            <!-- 标题栏：右侧窗口控制区域，窗口最小化、最大化、还原、关闭按钮 -->
            <HBox margin="0,0,0,0" valign="center" width="auto" height="36">
                <Button class="btn_switch_theme" height="32" width="40" name="btn_window_theme" margin="0,2,0,2"/>
                <Button class="btn_switch_lang" height="32" width="40" name="btn_window_language" margin="0,2,0,2"/>
                <Button class="btn_wnd_fullscreen_11" height="32" width="40" name="btn_window_fullscreen" margin="0,2,0,2"/>
                <Button class="btn_wnd_min_11" height="32" width="40" name="btn_window_min" margin="0,2,0,2"/>
                <Box height="stretch" width="40" margin="0,2,0,2">
                    <Button class="btn_wnd_max_11" height="32" width="stretch" name="btn_window_max"/>
                    <Button class="btn_wnd_restore_11" height="32" width="stretch" name="btn_window_restore" visible="false"/>
                </Box>
                <Button class="btn_wnd_close_11" height="stretch" width="40" name="btn_window_close" margin="0,0,0,2"/>
            </HBox>
        </HBox>
        <!-- 标题栏区域结束 -->

        <!-- 工作区域，除了标题栏外的内容都放在这个大的Box区域 -->
        <Box bkcolor="bg_container">
            <VBox margin="0,0,0,0" valign="center" halign="center">
                <Label name="tooltip" text="这是一个简单的nim_duilib窗口，带有标题栏和常规按钮。" height="100%" width="100%" text_align="hcenter,vcenter"/>
            </VBox>
        </Box>
    </VBox>
</Window>
```

## 显示窗口

在主线程的 `MainThread::OnInit` 方法中，创建窗口并居中显示，创建窗口前先引入窗口的头文件，修改后的代码如下：    
（首先在文件中包含头文件：`#include "MainForm.h"`）

```cpp
bool MainThread::OnInit()
{
    //初始化全局资源, 使用本地文件夹作为资源
    ui::FilePath resourcePath = ui::GlobalManager::GetResourceRootPath(false);
    ui::GlobalManager::Instance().Startup(ui::LocalFilesResParam(resourcePath));

    //在下面加入启动窗口代码
    //
    //创建一个默认带有阴影的居中窗口
    MainForm* window = new MainForm();
    window->CreateWnd(nullptr, ui::WindowCreateParam(_T("MyDuilibApp"), true));
    window->PostQuitMsgWhenClosed(true);
    window->ShowWindow(ui::kSW_SHOW_NORMAL);
    return true;
}
```

这样一个简单的带有最小化、最大化、还原和关闭按钮、全屏按钮，具有阴影效果和一行文字提示的窗口就创建出来了，你可以编译运行以下代码看一看窗口效果。
   
## 在程序中使用libCEF
可以参考相关的文档[CEF.md](CEF.md)

## 关于工程配置（xmake）
项目的编译配置位于仓库根目录的 `xmake.lua` 和 `xmake` 目录；Skia 由 `xmake/repos` 中的本地包自动下载并编译。

仓库根目录只保留 `xmake.lua` 入口，其余脚本按职责分目录：
- `xmake/options.lua`：全部命令行选项（`--enable_*`、`--with_*`、`--render_backend` 等）；
- `xmake/env.lua`：公共路径与配置判定（描述域全局表 `DUILIB`）；
- `xmake/rules/`：项目规则（`duilib.config`、`duilib.features`、`duilib.app`、`duilib.skia`、`duilib.log`）；
- `xmake/targets/`：目标定义（`third_party.lua`、`duilib.lua`、`render.lua`、`image.lua`、`extensions.lua`、`examples.lua`、`bench.lua`）；每个目标的 `add_files` 都逐个列出具体源文件（不使用通配符），新增/删除源文件时在此登记；
- `xmake/tasks/register.lua`：注册 `xmake format`、`xmake format-check`、`xmake attribute-gen`、`xmake attribute-check` 四个任务；
- `xmake/scripts/`：任务脚本（`format_apply.lua`、`format_check.lua`）与属性名登记表（`attribute_defs.lua`、`attribute_gen.lua`、`attribute_check.lua`）。
    
## 如何设置项目中使用的源代码文件编码为UTF-8格式
1. 在项目根目录创建一个格式配置文件，文件名为：.editorconfig
2. 文件内容如下：
```
# Visual Studio generated .editorconfig file with C++ settings.
root = true

[*.{c,c++,cc,cpp,cppm,cxx,h,h++,hh,hpp,hxx,inl,ipp,ixx,tlh,tli}]

# Visual C++ Formatting settings

end_of_line = crlf               # 行尾格式，可选值为 lf（Unix 风格）、cr（Mac 风格）或 crlf（Windows 风格）
charset = utf-8                  # 文件编码字符集为 UTF-8（可选值为 utf-8、utf-8-bom、latin1 等）
trim_trailing_whitespace = true  # 删除文件末尾空格
insert_final_newline = true      # 末尾插入新行
indent_style = space             # 以空格代替 tab
indent_size = 4                  # 代替 tab 的空格数量
tab_width = 4                    # 制表符的宽度
```
3. 该方法适用于Visual Studio 2022。
