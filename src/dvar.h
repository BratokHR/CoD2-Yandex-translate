#pragma once
#define RegisterEnum ( ( dvar_t * ( * )( const char * var_name, enum_type enum_values[], int initial, int flags ) )offset_registerenum[ version ] )
#define CmdConsole  ( ( void (*)( char * ) )offset_cmdconsole[ version ] )

typedef struct
{
    const char *	name;
    int				flags;
    //LPVOID		value;
    union
    {
        float		fValue;
        float *		pVectorValue;
        int			iValue;
        ULONG		ulValue;
        char *		pStringValue;
        bool		boolValue;
    };
    LPVOID			value2;
    LPVOID			value3;
    LPVOID			min;
    LPVOID			max;
    int				pad[ 2 ];
} dvar_t;

typedef const char *enum_type;

typedef dvar_t * ( *tDvar_Registere ) ( const char * var_name, enum_type enum_values[], int initial, int flags );

void register_dvars();
dvar_t * Dvar_FindDvar( const char * name );

extern dvar_t *tr_lang;
extern dvar_t *tr_show;
extern enum_type lang_list[];