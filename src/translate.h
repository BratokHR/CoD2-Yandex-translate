#pragma once
bool check_valid_key();
std::string check_lang( char *text );
std::string translate( char *text, char *lang_from, char *lang_to );
std::string get_code( std::string response );
std::string get_lang( std::string response );
std::string get_text( std::string response );