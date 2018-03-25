#include "dllmain.h"

#define CURL_STATICLIB
#include "curl\curl.h"
#pragma comment ( lib, "curl/libcurl_a.lib" )

size_t writeFunction( void *ptr, size_t size, size_t nmemb, std::string* data )
{
    data->append( (char *)ptr, size * nmemb );
    return size * nmemb;
}

std::string decode_text( std::string text )
{
    std::string result = "";

    for ( size_t i = 0; i < text.length(); i++ )
    {
        char c = text[ i ];
        bool cyr = false;

        if ( (DWORD)c == 0xFFFFFFD0 || (DWORD)c == 0xFFFFFFD1 )
        {
            // Exceptions
            //if ( (DWORD)c == 0xFFFFFF81 ) // ¨
            //    c = 0xFFFFFFC5;
            //if ( (DWORD)c == 0xFFFFFF91 ) // ¸
            //    c = 0xFFFFFFB8;

            char n = text[ i + 1 ];

            if ( (DWORD)c == 0xFFFFFFD0 )
                n += 0x30;
            else if ( (DWORD)c == 0xFFFFFFD1 )
                n += 0x70;

            i++;
            c = n;
        }

        result += c;
    }

    return result;
}

std::string encode_text( std::string text )
{
    std::string result = "";
    char hex[ 16 ];

    for ( size_t i = 0; i < text.size(); i++ )
    {
        char c = text[ i ];

        if ( ( c >= 0xFFFFFFF0 && c <= 0xFFFFFFFF ) || c == 0xFFFFFFB8 )
        {
            result += "%D1%";
            sprintf( hex, "%2X", ( c - 0x70 ) & 0xFF );
        }
        else if ( c == 0xFFFFFFA8 || ( c >= 0xFFFFFFC0 && c <= 0xFFFFFFEF ) )
        {
            result += "%D0%";
            sprintf( hex, "%2X", ( c - 0x30 ) & 0xFF );
        }
        else
        {
            result += "%";
            sprintf( hex, "%2X", c & 0xFF );
        }

        result += hex;
    }

    return result;
}

CURL *curl;

bool net_init()
{
    curl_global_init( CURL_GLOBAL_ALL );
    curl = curl_easy_init();

    if ( curl )
        return true;

    return false;
}

void net_close()
{
    if ( curl )
        curl_easy_cleanup( curl );

    curl_global_cleanup();
}

std::string net_send_post( char *header, char *post )
{
    // http://www.cyberforum.ru/cpp-networks/thread2210264.html

    char snd_post[ 1024 ];
    sprintf( snd_post, "text=%s", (char *)encode_text( std::string( post ) ).c_str() );

    curl_easy_setopt( curl, CURLOPT_URL, header );
    curl_easy_setopt( curl, CURLOPT_POSTFIELDS, snd_post );

    std::string response_string;
    std::string header_string;
    curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, writeFunction );
    curl_easy_setopt( curl, CURLOPT_WRITEDATA, &response_string );
    curl_easy_setopt( curl, CURLOPT_HEADERDATA, &header_string );

    if ( curl_easy_perform( curl ) != CURLE_OK )
    {
        PrintInConsole( "^1TranslateError^7: curl_easy_perform() failed.\n" );
        return "";
    }

    std::string dec_text = decode_text( response_string );
    return dec_text;
}