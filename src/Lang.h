#pragma once

#include <string>
#include <Windows.h>

namespace lang
{
    enum class Lang
    {
        English = 0,
        Chinese
    };

    /// <summary>
    /// Set current language and persist it
    /// </summary>
    void SetLang( Lang lang );

    /// <summary>
    /// Get current language
    /// </summary>
    Lang GetLang();

    /// <summary>
    /// Translate UI string to current language (idempotent, both directions)
    /// </summary>
    std::wstring Tr( const std::wstring& text );

    /// <summary>
    /// Localize a dialog: caption, buttons, statics, list columns, status bar
    /// </summary>
    void LocalizeDialog( HWND hDlg );

    /// <summary>
    /// Localize a menu recursively
    /// </summary>
    void LocalizeMenu( HMENU hMenu );

    /// <summary>
    /// Load persisted language from registry
    /// </summary>
    void Load();
}