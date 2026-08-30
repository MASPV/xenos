#include "DlgModules.h"
#include "Lang.h"
#include "Message.hpp"


ModulesDlg::ModulesDlg( blackbone::Process& proc )
    : Dialog( IDD_MODULES )
    , _process( proc )
{
    _messages[WM_INITDIALOG]   = static_cast<Dialog::fnDlgProc>(&ModulesDlg::OnInit);

    _events[IDC_BUTTON_CLOSE]  = static_cast<Dialog::fnDlgProc>(&ModulesDlg::OnCloseBtn);
    _events[IDC_BUTTON_UNLOAD] = static_cast<Dialog::fnDlgProc>(&ModulesDlg::OnUnload);
}

ModulesDlg::~ModulesDlg()
{
}

INT_PTR ModulesDlg::OnInit( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    Dialog::OnInit( hDlg, message, wParam, lParam );

    LVCOLUMNW lvc = { 0 };
    _modList.Attach( GetDlgItem( hDlg, IDC_LIST_MODULES ) );

    ListView_SetExtendedListViewStyle( _modList.hwnd(), LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER );

    //
    // Insert columns
    //
    _modList.AddColumn( lang::Tr( L"Name" ),       100, Name );
    _modList.AddColumn( lang::Tr( L"Image Base" ), 100, ImageBase );
    _modList.AddColumn( lang::Tr( L"Platform" ),   60,  Platform );
    _modList.AddColumn( lang::Tr( L"Load type" ),  80,  LoadType );

    RefreshList();

    lang::LocalizeDialog( _hwnd );

    return TRUE;
}


INT_PTR ModulesDlg::OnCloseBtn( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    return CloseDialog();
}

INT_PTR ModulesDlg::OnUnload( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam )
{
    // Get selection
    auto idx = _modList.selection();
    if (idx == MAXUINT)
        idx = 0;

    if (_process.valid())
    {
        wchar_t* pEnd = nullptr;
        blackbone::module_t modBase = wcstoull( _modList.itemText( idx, ImageBase ).c_str(), &pEnd, 0x10 );
        auto mod = _process.modules().GetModule( modBase );
        auto barrier = _process.core().native()->GetWow64Barrier();

        // Validate module
        if (barrier.type == blackbone::wow_32_32 && mod->type == blackbone::mt_mod64)
        {
            Message::ShowError( hDlg, L"Please use Xenos64.exe to unload 64 bit modules from WOW64 process" );
            return TRUE;
        }

        if (mod != nullptr)
        {
            _process.modules().Unload( mod );
            RefreshList();
        }
        else
            Message::ShowError( hDlg, L"Module not found" );
    }

    return TRUE;
}


/// <summary>
/// Refresh module list
/// </summary>
void ModulesDlg::RefreshList( )
{
    _modList.reset();
    if (!_process.valid())
        return;

    // Found modules
    auto modsLdr = _process.modules().GetAllModules( blackbone::LdrList );
    auto modsSec = _process.modules().GetAllModules( blackbone::Sections );
    auto modsPE = _process.modules().GetAllModules( blackbone::PEHeaders );

    // Known manual modules
    decltype(modsLdr) modsAll;
    auto modsManual = _process.modules().GetManualModules( );

    // Gather all modules
    modsAll.insert( modsLdr.begin(), modsLdr.end() );
    modsAll.insert( modsSec.begin(), modsSec.end() );
    modsAll.insert( modsPE.begin(), modsPE.end() );
    modsAll.insert( modsManual.begin(), modsManual.end() );

    for (auto& mod : modsAll)
    {
        wchar_t address[64];
        std::wstring platfom;
        std::wstring detected;

        wsprintf( address, L"0x%08I64x", mod.second->baseAddress );

        // Module platform
        if (mod.second->type == blackbone::mt_mod32)
            platfom = lang::Tr( L"32 bit" );
        else if (mod.second->type == blackbone::mt_mod64)
            platfom = lang::Tr( L"64 bit" );
        else
            platfom = lang::Tr( L"Unknown" );

        // Mapping type
        if (mod.second->manual == true)
            detected = lang::Tr( L"Manual map" );
        else if (modsLdr.count( mod.first ))
            detected = lang::Tr( L"Normal" );
        else if (modsSec.count( mod.first ))
            detected = lang::Tr( L"Section only" );
        else if (mod.second->name.find( L"Unknown_0x" ) == 0)
            detected = lang::Tr( L"PE header" );
        else
            detected = lang::Tr( L"Unknown" );

        _modList.AddItem( mod.second->name, static_cast<LPARAM>(mod.second->baseAddress), { address, platfom, detected } );
    }
}