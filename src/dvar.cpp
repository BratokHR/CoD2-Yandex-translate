#include "dllmain.h"

tDvar_Registere Dvar_Registere;

enum_type lang_list[] = { "en", "fr", "de", "it", "es", "en", "ru", "pl", "ko", "en", "ja", "zh", "th", "en", "cs", "hu", "tr", "el", NULL };
enum_type country_list[] = { "English", "French", "German", "Italian", "Spanish", "British", "Russian", "Polish", "Korean", "Taiwanese", "Japanese", "Chinese", "Thai", "Leet", "Czech", "Hungarian", "Turkish", "Greek", NULL };

dvar_t *tr_lang;
dvar_t *tr_show;
dvar_t *dvarList = nullptr;

void register_dvars()
{
    tr_show = Dvar_FindDvar( "translator" );
    tr_lang = Dvar_FindDvar( "translator_lang" );

    if ( !tr_show )
    {
        CmdConsole( "dvar_bool translator 1" );
        tr_show = Dvar_FindDvar( "translator" );
    }

    int lang_num = 0;
    dvar_t *loc_language = Dvar_FindDvar( "loc_language" );

    if ( loc_language )
        lang_num = loc_language->iValue;

    if ( !tr_lang )
    {
        tr_lang = RegisterEnum( "translator_lang", country_list, lang_num, 0 );
    }
}

dvar_t * Dvar_FindDvar( const char * name )
{
    for ( int i = 0; i < 0x500; i++ )
    {
        if ( dvarList[ i ].name )
        {
            if ( _stricmp( dvarList[ i ].name, name ) == 0 )
                return &( dvarList[ i ] );
        }
    }

    return NULL;
}