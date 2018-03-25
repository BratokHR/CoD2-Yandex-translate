#include "dllmain.h"

int version = 1;
DWORD offset_printinconsole[] = { 0x00430540, 0x00431EE0 };
DWORD offset_printchat[] =      { 0x004CABB0, 0x004D0E10 };
DWORD offset_argv[] =           { 0x00A6C480, 0x00B17A80 };
DWORD offset_argc[] =           { 0x00A6EE80, 0x00B1A480 };
DWORD offset_localized[] =      { 0x00533500, 0x00539770 };
DWORD offset_servercommand[] =  { 0x004CB900, 0x004D1B80 };
DWORD offset_eventkey[] =       { 0x0040A3C0, 0x0040ADE0 };
DWORD offset_registerenum[] =   { 0x00436950, 0x004383A0 };
DWORD offset_cmdconsole[] =     { 0x0041FD10, 0x004214C0 };

std::vector<player_t> players;

char *quickmessages[ 35 ]{
    "Отступаем!",
    "Вижу врага!",
    "Враг уничтожен!",
    "Я на позиции.",
    "За мной!",
    "Вперед!",
    "Назад!",
    "Огонь на подавление!",
    "Атаковать с правого фланга!",
    "Атаковать с левого фланга!",
    "Держать эту позицию!",
    "Перегруппироваться!",
    "Держаться вместе!",
    "Отряд, атаковать с правого фланга!",
    "Отряд, атаковать с левого фланга!",
    "Отряд, держать позицию!",
    "Отряд, держать строй!",
    "Отряд, держаться группой!",
    "Я на месте.",
    "Здесь все чисто.",
    "Граната!",
    "Берегись, граната!",
    "Снайпер!",
    "Нужно подкрепление!",
    "Не стрелять!",
    "Так точно!",
    "Никак нет!",
    "Выполняю.",
    "Прошу прощения.",
    "Отличный выстрел!",
    "Почему так долго?",
    "Слишком долго!",
    "Мозги набекрень?",
    "Крыша поехала?",
    "С головой проблемы?"
};

void try_translate( char *name, char *msg )
{
    char without_color[ 256 ];
    int num = -1;

    // its me
    if ( !strcmp( name, ci[ cg->clientNum ].name ) || !tr_lang )
        return;

    // fast message
    for ( int i = 0; i < 35; i++ )
    {
        if ( !strcmp( msg + ( *msg == '^' ? 2 : 0 ), quickmessages[ i ] ) )
            return;
    }

    for ( size_t i = 0; i < players.size(); i++ )
    {
        if ( !strcmp( players[ i ].name.c_str(), name ) )
        {
            num = i;
            break;
        }
    }

    if ( num < 0 )
    {
        player_t new_player;
        new_player.name = name;
        new_player.lang = 0;
        new_player.count_translate = 0;

        num = players.size();
        players.push_back( new_player );
    }
    
    // check lang
    /*if ( players[ num ].count_translate < 6 )
    {
        lang = check_lang( msg );

        players[ num ].last_lang = lang;

        if ( lang == "ru" )
            players[ num ].lang--;
        else
            players[ num ].lang++;
    }
    players[ num ].count_translate++;*/

    players[ num ].last_lang = check_lang( msg );
    players[ num ].lang++;
    players[ num ].count_translate++;

    if ( players[ num ].lang > 0 && players[ num ].last_lang != lang_list[ tr_lang->iValue ] )
    {
        replace_color( msg, without_color );

        std::string res = translate( without_color, (char *)players[ num ].last_lang.c_str(), (char *)lang_list[ tr_lang->iValue ] );

        if ( res.length() > 0 && strcmp( res.c_str(), msg ) != 0 )
            print_chat( "^7[^3Yandex %s^7]%s^7: %s", players[ num ].last_lang.c_str(), name, res.c_str() );
    }
}

void translate_message( char *msg )
{
    std::string lang_to = "en";
    std::string lang_from = "en";
    std::string final_text = "";
    size_t end = strlen( msg );

    if ( strstr( msg, " " ) )
    {
        char *p = msg + strlen( msg );

        while ( *p != ' ' )
            p--;

        p++;

        if ( *p )
        {
            for ( size_t i = 0; lang_list[ i ]; i++ )
            {
                if ( !strcmp( lang_list[ i ], p ) )
                {
                    lang_to = std::string( p );
                    end = p - msg - 1;

                    break;
                }
            }
        }
    }

    final_text = std::string( msg ).substr( 0, end );
    lang_from = check_lang( (char *)final_text.c_str() );

    if ( lang_from == lang_to )
        return;

    std::string res = translate( (char *)final_text.c_str(), (char *)lang_from.c_str(), (char *)lang_to.c_str() );
    setChatText( (char *)res.c_str() );
}

char *argv( int n )
{
    return *(char **)( offset_argv[ version ] + n * 0x4 );
}

int argc()
{
    return *(int *)offset_argc[ version ];
}

void print_chat( const char *msg, ... )
{
    va_list argptr;
    char text[ 256 ];
    DWORD d_sayline = offset_printchat[ version ];

    va_start( argptr, msg );
    _vsnprintf_s<256>( text, 256, msg, argptr );
    va_end( argptr );

    PrintInConsole( "%s\n", text );

    __asm
    {
        lea ESI, text
        mov ECX, ESI
        call d_sayline
    }
}

char *localized( char *text )
{
    DWORD d_func = offset_localized[ version ];

    __asm
    {
        mov EAX, text
        call d_func
    }
}

void replace_color( char *msg, char *ret )
{
    for ( char *c = msg; *c; c++ )
    {
        if ( *c == '^' && *( c + 1 ) >= '0' && *( c + 1 ) <= '9' )
        {
            c++;
            continue;
        }

        *ret = *c;
        ret++;
    }

    *ret = '\0';
}

void setChatText( char *text )
{
    memcpy( field->chat.buffer, text, 256 );
    field->chat.widthChars1 = 0;
    field->chat.widthChars2 = strlen( text );
    field->chat.cursor = strlen( text );
}