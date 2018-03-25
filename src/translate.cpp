#include "dllmain.h"

bool fatal_error = false;

int current_key = 0;
char *keys[ 10 ] = {
    "trnsl.1.1.20171217T110418Z.33ec36494a514c49.d2254de07ac7ec624771e638a1a8d70a3ea019a7",
    "trnsl.1.1.20171217T110342Z.13d333511f02ee5e.a50770090c1c52f9ebe8dac716c336eddf625611",
    "trnsl.1.1.20171217T110346Z.a5e7a697a9436bee.56d76d03611566446f7e174c489c86354eac7d40",
    "trnsl.1.1.20171217T110350Z.4286c860d8b76b27.565a98f2c0c929934701cf163542aa806690fe24",
    "trnsl.1.1.20171217T110353Z.acf7ff5377f18522.f93847b89ba2c376d8a545700f1b32edd4df8e04",
    "trnsl.1.1.20171217T110358Z.cd5c07364ba32196.68830712e0b5a5fc68747571dc03ef2fed8d3c72",
    "trnsl.1.1.20171217T110401Z.1c2c1c15c630a96b.f384321a1a098330f3574828f7b5b023968eaafb",
    "trnsl.1.1.20171217T110405Z.bdac9c3e18e4e29d.694523b4cef21eec54c8394901f81de49d1999e7",
    "trnsl.1.1.20171217T110410Z.c9b7b791aa064199.5895fbd69d01ffb0309f99d10ef5f7d2c0a1d42f",
    "trnsl.1.1.20171217T110414Z.d5bc1fd1f4a1db66.2e7ae91146274d346fe946c0618bbd11fe7b3b1a"
};

bool check_valid_key()
{
    char url[ 256 ];
    char post[ 256 ] = "text=Hello";
    std::string res;
    std::string code;

    sprintf( url, "https://translate.yandex.net/api/v1.5/tr.json/detect?hint=en&key=%s", keys[ current_key ] );

    res = net_send_post( url, post );
    code = get_code( res );

    if ( code == "200" )
        return true;

    else if ( ( code == "401" || code == "402" || code == "404" ) && current_key < 9 )
    {
        current_key++;
        return check_valid_key();
    }

    fatal_error = true;
    return false;
}

std::string check_lang( char *text )
{
    char url[ 256 ];
    std::string res;
    std::string code;

    if ( fatal_error )
        return "ru";

    sprintf( url, "https://translate.yandex.net/api/v1.5/tr.json/detect?hint=en&key=%s", keys[ current_key ] );;

    res = net_send_post( url, text );
    code = get_code( res );

    if ( code == "200" )
        return get_lang( res ).c_str();
    else if ( check_valid_key() )
        return check_lang( text );

    return "ru";
}

std::string translate( char *text, char *lang_from, char *lang_to )
{
    char url[ 256 ];
    std::string res;
    std::string code;

    sprintf( url, "https://translate.yandex.net/api/v1.5/tr.json/translate?key=%s&lang=%s-%s", keys[ current_key ], lang_from, lang_to );

    res = net_send_post( url, text );
    code = get_code( res );

    if ( code == "200" )
        return get_text( res );
    else if ( check_valid_key() )
        return translate( text, lang_from, lang_to );

    return text;
}

std::string get_code( std::string response )
{
    size_t c = response.find( "code" );
    c += 6; // sizeof( "code\":" )

    return response.substr( c, 3 );
}

std::string get_lang( std::string response )
{
    size_t c = response.find( "lang" );
    c += 7; // sizeof( "lang\":\"" )

    char *p = (char *)response.c_str();
    p += c;

    size_t s = 0;

    while ( *p != '"' )
    {
        p++;
        s++;
    }

    if ( s == 0 )
        return "ru";

    return response.substr( c, s );
}

std::string get_text( std::string response )
{
    size_t c = response.find( "text" );
    c += 8; // sizeof( "text\":[\"" )

    char *p = (char *)response.c_str();
    p += c;

    size_t s = 0;

    while ( *p != '"' )
    {
        p++;
        s++;
    }

    if ( s == 0 )
        return "ru";

    return response.substr( c, s );
}