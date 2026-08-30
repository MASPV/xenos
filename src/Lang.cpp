#include "Lang.h"

#include <CommCtrl.h>

namespace
{
    struct LangPair
    {
        const wchar_t* en;
        const wchar_t* zh;
    };

    // English -> Chinese translation table
    const LangPair g_table[] =
    {
        // Menu
        { L"&Profiles",           L"&配置" },
        { L"&Load\tCtrl+O",       L"&加载\tCtrl+O" },
        { L"&Save As\tCtrl+S",    L"&另存为\tCtrl+S" },
        { L"&Tools",              L"&工具" },
        { L"E&ject modules",      L"卸载模块(&E)" },
        { L"&Protect self",       L"&自我保护" },
        { L"&Settings",           L"&设置" },
        { L"&Language",           L"&语言" },
        { L"&English",            L"&英文" },
        { L"Chinese",             L"中文" },

        // Main dialog
        { L"Process",             L"进程" },
        { L"Process selection",   L"进程选择" },
        { L"Filter",              L"筛选" },
        { L"Filter help",         L"筛选帮助" },
        { L"Type a process name or PID number in the filter field, then open the dropdown to see matching results.", L"在进程选择框的过滤字段中键入 进程名 或 PID 数字，打开下拉框查看匹配结果。" },
        { L"Type",                L"类型" },
        { L"Existing",            L"现有进程" },
        { L"New",                 L"新进程" },
        { L"Manual launch",       L"手动启动" },
        { L"Select",              L"选择" },
        { L"Images",              L"映像" },
        { L"Add",                 L"添加" },
        { L"Remove",              L"移除" },
        { L"Clear",               L"清空" },
        { L"Inject",              L"注入" },
        { L"Advanced",            L"高级" },
        { L"Name",                L"名称" },
        { L"Architecture",        L"架构" },
        { L"Default profile",     L"默认配置" },
        { L"Idle",                L"空闲" },
        { L"Injecting...",        L"注入中..." },

        // Settings dialog
        { L"Advanced settings",   L"高级设置" },
        { L"OK",                  L"确定" },
        { L"Cancel",              L"取消" },
        { L"Manual map options",  L"手动映射选项" },
        { L"Add loader reference",L"添加加载器引用" },
        { L"Manually resolve imports", L"手动解析导入" },
        { L"Erase PE headers",    L"擦除PE头" },
        { L"No exception support",L"无异常支持" },
        { L"Conceal memory",      L"隐藏内存" },
        { L"Native injection options", L"原生注入选项" },
        { L"Unlink module",       L"解除模块链接" },
        { L"Ignore TLS",          L"忽略TLS" },
        { L"Init routine",        L"初始化例程" },
        { L"Init argument",       L"初始化参数" },
        { L"Command line",        L"命令行" },
        { L"Injection type",      L"注入类型" },
        { L"Initialization routine", L"初始化例程" },
        { L"Process startup command line", L"进程启动命令行" },
        { L"General options",     L"常规选项" },
        { L"Close after injection", L"注入后关闭" },
        { L"Inject delay",        L"注入延迟" },
        { L"Inject interval",     L"注入间隔" },
        { L"ms",                  L"毫秒" },
        { L"Use existing thread", L"使用现有线程" },
        { L"Escalate handle acess", L"提升句柄权限" },
        { L"Skip count",          L"跳过数量" },
        { L"Process waiting options", L"进程等待选项" },
        { L"Inject indefinitely", L"无限注入" },
        { L"Native inject",       L"原生注入" },
        { L"Manual map",          L"手动映射" },
        { L"Kernel (CreateThread)", L"内核 (CreateThread)" },
        { L"Kernel (APC)",        L"内核 (APC)" },
        { L"Kernel (Manual map)", L"内核 (手动映射)" },

        // Modules dialog
        { L"Modules",             L"模块" },
        { L"Unload",              L"卸载" },
        { L"Close window",        L"关闭窗口" },
        { L"Image Base",          L"映像基址" },
        { L"Platform",            L"平台" },
        { L"Load type",           L"加载类型" },
        { L"32 bit",              L"32位" },
        { L"64 bit",              L"64位" },
        { L"Unknown",             L"未知" },
        { L"Normal",              L"正常" },
        { L"Section only",        L"仅节区" },
        { L"PE header",           L"PE头" },

        // Wait dialog
        { L"Waiting...",          L"等待中..." },
        { L"Awaiting",            L"正在等待" },
        { L"launch...",           L"启动..." },
    };
}

namespace lang
{
    static Lang g_lang = Lang::English;

    void SetLang( Lang lang )
    {
        if (g_lang == lang)
            return;

        g_lang = lang;

        HKEY key = nullptr;
        if (RegCreateKeyExW( HKEY_CURRENT_USER, L"Software\\Xenos", 0, nullptr, 0, KEY_WRITE, nullptr, &key, nullptr ) == ERROR_SUCCESS)
        {
            DWORD val = (lang == Lang::Chinese) ? 1 : 0;
            RegSetValueExW( key, L"Language", 0, REG_DWORD, reinterpret_cast<const BYTE*>(&val), sizeof( val ) );
            RegCloseKey( key );
        }
    }

    Lang GetLang()
    {
        return g_lang;
    }

    std::wstring Tr( const std::wstring& text )
    {
        for (auto& p : g_table)
        {
            if (GetLang() == Lang::Chinese)
            {
                if (text == p.en)
                    return p.zh;
            }
            else
            {
                if (text == p.zh)
                    return p.en;
            }
        }

        return text;
    }

    BOOL CALLBACK EnumChildProc( HWND hwnd, LPARAM lParam )
    {
        wchar_t cls[32] = { 0 };
        GetClassNameW( hwnd, cls, 32 );
        std::wstring classname = cls;

        if (classname == L"Button" || classname == L"Static")
        {
            wchar_t buf[512] = { 0 };
            GetWindowTextW( hwnd, buf, 512 );

            std::wstring text = buf;
            std::wstring tr = Tr( text );
            if (tr != text)
                SetWindowTextW( hwnd, tr.c_str() );
        }
        else if (classname == L"SysListView32")
        {
            HWND header = ListView_GetHeader( hwnd );
            int nCol = Header_GetItemCount( header );
            for (int i = 0; i < nCol; i++)
            {
                wchar_t buf[256] = { 0 };
                LVCOLUMNW col = { 0 };
                col.mask = LVCF_TEXT;
                col.pszText = buf;
                col.cchTextMax = 256;
                ListView_GetColumn( hwnd, i, &col );

                std::wstring text = buf;
                std::wstring tr = Tr( text );
                if (tr != text)
                {
                    col.mask = LVCF_TEXT;
                    col.pszText = const_cast<wchar_t*>(tr.c_str());
                    ListView_SetColumn( hwnd, i, &col );
                }
            }
        }
        else if (classname == L"msctls_statusbar32")
        {
            int nParts = (int)SendMessageW( hwnd, SB_GETPARTS, 0, 0 );
            for (int i = 0; i < nParts; i++)
            {
                int len = (int)SendMessageW( hwnd, SB_GETTEXTLENGTH, i, 0 );
                if (len <= 0)
                    continue;

                std::wstring text( len + 1, L'\0' );
                SendMessageW( hwnd, SB_GETTEXT, i, reinterpret_cast<LPARAM>(text.data()) );
                text.resize( len );

                std::wstring tr = Tr( text );
                if (tr != text)
                    SendMessageW( hwnd, SB_SETTEXT, MAKEWORD( i, SBT_NOBORDERS ), reinterpret_cast<LPARAM>(tr.c_str()) );
            }
        }

        return TRUE;
    }

    void LocalizeDialog( HWND hDlg )
    {
        if (!hDlg)
            return;

        // Caption
        wchar_t buf[256] = { 0 };
        GetWindowTextW( hDlg, buf, 256 );
        std::wstring text = buf;
        std::wstring tr = Tr( text );
        if (tr != text)
            SetWindowTextW( hDlg, tr.c_str() );

        EnumChildWindows( hDlg, EnumChildProc, 0 );
    }

    void LocalizeMenu( HMENU hMenu )
    {
        if (!hMenu)
            return;

        int count = GetMenuItemCount( hMenu );
        for (int i = 0; i < count; i++)
        {
            HMENU sub = GetSubMenu( hMenu, i );

            wchar_t buf[256] = { 0 };
            if (GetMenuStringW( hMenu, i, buf, 256, MF_BYPOSITION ) > 0)
            {
                std::wstring text = buf;
                std::wstring tr = Tr( text );
                if (tr != text)
                {
                    if (sub)
                        ModifyMenuW( hMenu, i, MF_BYPOSITION | MF_POPUP, reinterpret_cast<UINT_PTR>(sub), tr.c_str() );
                    else
                        ModifyMenuW( hMenu, i, MF_BYPOSITION | MF_STRING, GetMenuItemID( hMenu, i ), tr.c_str() );
                }
            }

            if (sub)
                LocalizeMenu( sub );
        }
    }

    void Load()
    {
        HKEY key = nullptr;
        if (RegOpenKeyExW( HKEY_CURRENT_USER, L"Software\\Xenos", 0, KEY_READ, &key ) == ERROR_SUCCESS)
        {
            DWORD val = 0;
            DWORD size = sizeof( val );
            if (RegQueryValueExW( key, L"Language", nullptr, nullptr, reinterpret_cast<LPBYTE>(&val), &size ) == ERROR_SUCCESS)
                g_lang = (val == 1) ? Lang::Chinese : Lang::English;

            RegCloseKey( key );
        }
    }
}
