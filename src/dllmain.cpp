#include "dllmain.h"

typedef void( *tServerCommand )( );
tServerCommand pServerCommand;

typedef void( *tEventKey )( );
tEventKey pEventKey;

std::vector<std::string> cmds;

field_t *field = nullptr;
serverInfo_t *si = nullptr;
clientInfo_t *ci = nullptr;
cg_t *cg = nullptr;

void parseText()
{
    char prefix[ 256 ];
    char name[ 256 ];
    char msg[ 256 ];
    char tmp[ 256 ];

    for ( ;; )
    {
        if ( cmds.size() > 0 )
        {
            char *p = (char *)cmds[ 0 ].c_str();
            char *op = &prefix[ 0 ];

            p++; // ih

            while ( *p == '\x15' )
                p++;

            // ^8
            if ( *p == '^' )
            {
                strncpy( op, p, 2 );
                op += 2;
                p += 2;
            }

            while ( *p == '(' )
            {
                char *s = strstr( p, ")" );

                if ( !strstr( s, ": " ) )
                    goto error_parse;

                if ( !s )
                    goto parse_name;

                *op = *p;
                p++;
                op++;

                if ( *p == '\x14' ) // standart chat
                {
                    char *t = &tmp[ 0 ];
                    p++;

                    while ( *p != '\x15' )
                    {
                        *t = *p;
                        p++;
                        t++;
                    }

                    *t = '\0';
                    char *loc_str = localized( tmp );
                    
                    for ( char *l = loc_str; *l; l++, op++ )
                        *op = *l;

                    p++;
                }
                else // custom chat
                {
                    char *s = strstr( p, ")" );

                    if ( !strstr( s, ": " ) )
                        goto error_parse;

                    if ( !s )
                        goto parse_name;

                    while ( p < s )
                    {
                        *op = *p;
                        op++;
                        p++;
                    }
                }

                *op = *p;
                p++;
                op++;
            }

        parse_name:
            *op = '\0';
            char *on = &name[ 0 ];
            char *s = strstr( p, ": ");

            if ( !s )
                goto error_parse;

            while ( p < s )
            {
                *on = *p;
                on++;
                p++;
            }

            //      '^7: '
            // fix --^^---
            if ( *( on - 1 ) == '7' && *( on - 2 ) == '^' )
                on -= 2;

            *on = '\0';
            p += 2; // sizeof( ": " )
            char *om = &msg[ 0 ];

            while ( *p != '\x15' && *p != '\x14' && *p && *p != '\x0D' )
            {
                *om = *p;
                om++;
                p++;
            }

            if ( *p == '\x14' )
            {
                char *t = &tmp[ 0 ];
                p++;

                while ( *p )
                {
                    *t = *p;
                    p++;
                    t++;
                }

                *t = '\0';
                char *loc_str = localized( tmp );

                for ( char *l = loc_str; *l; l++, om++ )
                    *om = *l;

                p++;
                *om = *p;
            }
            else if ( *p )
            {
                p++;

                while ( *p && *p != '\x0D' )
                {
                    *om = *p;
                    om++;
                    p++;
                }
            }

            *om = '\0';

            print_chat( "%s%s^7: %s", prefix, name, msg );
            try_translate( name, msg );

            goto success_parse;
        error_parse:
            print_chat( "^7[^1ErrorParse^7]: %s", cmds[ 0 ].c_str() );

            FILE *f = fopen( "Translator ErrorParse.txt", "a" );
            fprintf( f, "%s\n", cmds[ 0 ].c_str() );
            fclose( f );
        success_parse:
            cmds.erase( cmds.begin() );
        }

        Sleep( 100 );
    }
}

void h_ServerCommand()
{
    int cnt = argc();

    if ( tr_show && tr_show->boolValue && cnt > 1 && ( strcmp( argv( 0 ), "h" ) == 0 || strcmp( argv( 0 ), "i" ) == 0 ) )
    {
        char new_msg[ 256 ];
        sprintf( new_msg, "%s%s", argv( 0 ), argv( 1 ) );

        cmds.push_back( new_msg );
        return;
    }

    pServerCommand();
}

void hEventKey()
{
    static int key;
    static char backup[ 256 ];

    __asm pushad
    __asm mov key, eax

    if ( tr_show && tr_show->iValue )
    {
        if ( GetAsyncKeyState( VK_LCONTROL ) || GetAsyncKeyState( VK_RCONTROL ) )
        {
            if ( ( key == 13 || key == 191 ) && strlen( field->chat.buffer ) > 0 )
            {
                memcpy( backup, field->chat.buffer, 256 );
                translate_message( field->chat.buffer );

                __asm popad
                return;
            }
            else if ( strlen( backup ) > 0 && key == 127 )
            {
                setChatText( backup );

                __asm popad
                return;
            }
        }
        else if ( key == 13 || key == 191 )
            memset( backup, 0, 256 );
    }

    __asm popad
    pEventKey();
}

void mainLoop()
{
    int oldFrame = 999999;

    for ( ;; Sleep( 100 ) )
    {
        if ( !cg->snap )
            continue;

        if ( si->FrameCount < oldFrame )
        {
            cmds.clear();
            players.clear();
        }

        oldFrame = si->FrameCount;
    }
}

void InitDetours()
{
    while ( GetModuleHandleA( "gfx_d3d_mp_x86_s.dll" ) == 0 )
        Sleep( 100 );

    if ( !net_init() )
    {
        PrintInConsole( "^1TranslateError: ^7failed to open CURL" );
        net_close();
        return;
    }

    if ( !check_valid_key() )
        return;

    register_dvars();
    pServerCommand = (tServerCommand)DetourFunction( (PBYTE)offset_servercommand[ version ], (PBYTE)h_ServerCommand );
    pEventKey = (tEventKey)DetourFunction( (PBYTE)offset_eventkey[ version ], (PBYTE)hEventKey );
}

void RemoveDetours()
{
    DetourRemove( (PBYTE)pServerCommand, (PBYTE)h_ServerCommand );
    DetourRemove( (PBYTE)pEventKey, (PBYTE)hEventKey );
    net_close();
}

bool check_version_game()
{
    if ( strcmp( (char*)0x561B1C, "1.0" ) == 0 )
    {
        field = (field_t *)0x005C3700;
        si = (serverInfo_t *)0x0137068C;
        ci = (clientInfo_t *)0x0145A914;
        cg = (cg_t *)0x01379000;
        dvarList = (dvar_t*)0x00B313D0;
        version = 0;
    }
    else if ( strcmp( (char*)0x5A1D54, "1.2" ) == 0 )
    {
        MessageBox( 0, "Invalid version of the game!\nDelete the Translator.asi from the ASI folder.", "Translator", MB_ICONERROR );
        return false;
    }
    else
    {
        field = (field_t *)0x00606E80;
        si = (serverInfo_t *)0x014E570C;
        ci = (clientInfo_t *)0x015CF994;
        cg = (cg_t *)0x014EE080;
        dvarList = (dvar_t*)0x00C5C9D0;
        version = 1;
    }

    return true;
}

BOOL APIENTRY DllMain( HINSTANCE hModule, DWORD fdwReason, LPVOID lpvReserved )
{
    if ( fdwReason == DLL_PROCESS_ATTACH && check_version_game() )
    {
        CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)parseText, 0, 0, 0 );
        CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)InitDetours, 0, 0, 0 );
        CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)mainLoop, 0, 0, 0 );
    }
    else if ( fdwReason == DLL_PROCESS_DETACH )
    {
        RemoveDetours();
    }

    return true;
}