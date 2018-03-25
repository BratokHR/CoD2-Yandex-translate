#pragma once
#define PrintInConsole ( ( void( * )( char *, ... ) )offset_printinconsole[ version ] )

typedef struct
{
    int		cursor;
    int		widthChars1;
    int		widthChars2;
    int		pad[ 3 ];
    char	buffer[ 256 ];
} mfield_t;

typedef struct
{
    mfield_t	console;
    int			pad[ 2 ];
    mfield_t	chat;
} field_t;

typedef struct
{
    int	 Width;
    int	 Height;
    int	 pad1;
    int	 CountCommands;
    int	 FrameCount;
    int	 pad2;
    char GameType[ 32 ];
    char ServerName[ 256 ];
    int	 MaxPlayers;
    char MapName[ 256 ];
} serverInfo_t;

typedef struct
{
    std::string name;
    std::string last_lang;
    int lang;
    int count_translate;
} player_t;

typedef struct
{
    int  pad1[ 2 ];
    int  clientNum;
    char name[ 32 ];
    int  pad5[ 291 ];
} clientInfo_t;

typedef struct
{
    int  clientFrame;
    int  clientNum;
    int  padding[ 7 ];
    int *snap;
} cg_t;

void try_translate( char *name, char *msg );
void translate_message( char *msg );
char *argv( int n );
int argc();
void print_chat( const char *msg, ... );
char *localized( char *text );
void replace_color( char *msg, char *ret );
void setChatText( char *text );

extern std::vector<player_t> players;
extern field_t *field;
extern serverInfo_t *si;
extern clientInfo_t *ci;
extern cg_t *cg;
extern dvar_t *dvarList;

extern int version;
extern DWORD offset_printinconsole[];
extern DWORD offset_printchat[];
extern DWORD offset_argv[];
extern DWORD offset_argc[];
extern DWORD offset_localized[];
extern DWORD offset_servercommand[];
extern DWORD offset_eventkey[];
extern DWORD offset_registerenum[];
extern DWORD offset_cmdconsole[];