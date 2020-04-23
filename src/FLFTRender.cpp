#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#if !defined(_WIN32)
#include <wchar.h>
#endif 

#if defined(_WIN32) || defined(__linux__)
#include <omp.h>
#endif

#include <ft2build.h>
#include FT_FREETYPE_H

#include "FLFTRender.h"

////////////////////////////////////////////////////////////////////////////////

#define __MIN(a,b) (((a)<(b))?(a):(b))
#define __MAX(a,b) (((a)>(b))?(a):(b))

#define FLFT_DEFAULT_SIZE       12
#define FLFT_DEFAULT_COLOR      0xFFFFFFFF
#define FLFT_PI_F               3.1415926535897932384626433f
#define FLFT_DEFAULT_BOLDR      1.25f
#define FLFT_MAX_BOLDR          10.0f
#define FLFT_MIN_BOLDR          0.05f

////////////////////////////////////////////////////////////////////////////////

FLFTRender::FLFTRender( const char* ttf, long idx )
 :  fface( NULL ),
    fflib( NULL ),
    ffsize( FLFT_DEFAULT_SIZE ),
    fcolor( FLFT_DEFAULT_COLOR ),
    fkerning( false ),
    loaded( false ),
    additionalspaceX( 0 ),
    ttfbuffer( NULL ),
    ttfbufferlen( 0 ),
    flagBold( false ),
    flagItalic( false ),
    flagBoldRatio( FLFT_DEFAULT_BOLDR )
{
    FT_Library libFL = NULL;
    FT_Face face = NULL;

    if ( FT_Init_FreeType( &libFL ) == 0 )
    {
        if ( ttf != NULL )
        {
            if ( access( ttf, 0 ) == 0 )
            {
                if ( FT_New_Face( libFL, ttf, (FT_Long)idx, &face ) == 0 )
                {
                    loaded = true;
                }
            }
        }
    }

    if ( libFL != NULL )
    {
        fflib = (void*)libFL;
    }

    if ( face != NULL )
    {
        fface = (void*)face;
    }
    
    init();
}

FLFTRender::FLFTRender( const unsigned char* ttfbuff, unsigned ttfbuffsz, long idx )
 :  fface( NULL ),
    fflib( NULL ),
    ffsize( FLFT_DEFAULT_SIZE ),
    fcolor( FLFT_DEFAULT_COLOR ),
    fkerning( false ),
    loaded( false ),
    additionalspaceX( 0 ),
    ttfbuffer( NULL ),
    ttfbufferlen( 0 ),
    flagBold( false ),
    flagItalic( false ),
    flagBoldRatio( FLFT_DEFAULT_BOLDR )
{
    FT_Library libFL = NULL;
    FT_Face face = NULL;

    if ( FT_Init_FreeType( &libFL ) == 0 )
    {
        if ( ( ttfbuff != NULL ) && ( ttfbuffsz > 0 ) )
        {
            int retstate = -1;
            ttfbufferlen = ttfbuffsz;
            ttfbuffer = new unsigned char[ ttfbufferlen ];
            
            if ( ttfbuffer != NULL )
            {
                memcpy( ttfbuffer, ttfbuff, ttfbufferlen );
                retstate = FT_New_Memory_Face( libFL, ttfbuffer, ttfbufferlen, \
                                               (FT_Long)idx, &face );
                if ( retstate == 0 )
                {
                    loaded = true;
                }

            }
        }
    }

    if ( libFL != NULL )
    {
        fflib = (void*)libFL;
    }

    if ( face != NULL )
    {
        fface = (void*)face;
    }
    
    init();
}

FLFTRender::~FLFTRender()
{
    FT_Library libFL = (FT_Library)fflib;
    FT_Face face = (FT_Face)fface;

    if ( face != NULL )
    {
        FT_Done_Face( face );
        fface = NULL;
    }

    if ( libFL != NULL )
    {
        FT_Done_FreeType( libFL );
        fflib = NULL;
    }
    
    if ( ttfbuffer != NULL )
    {
        delete[] ttfbuffer;
        ttfbufferlen = 0;
    }
}

bool FLFTRender::FontLoaded()
{
    return loaded;
}

unsigned FLFTRender::FontSize()
{
    return ffsize;
}

void FLFTRender::FontSize( unsigned newsz )
{
    if ( newsz > 8 )
        ffsize = newsz;

    FT_Face face = (FT_Face)fface;

    if ( face != NULL )
    {
        FT_Set_Pixel_Sizes( face, 0, newsz );
    }
}

void FLFTRender::FontColor( unsigned rgba )
{
    fcolor = rgba;
}

unsigned FLFTRender::FontColor()
{
    return fcolor;
}

void FLFTRender::Bold( bool onoff )
{
    flagBold = onoff;
}

bool FLFTRender::Bold()
{
    return flagBold;
}

void FLFTRender::BoldRatio( float r )
{
    if ( ( r >= FLFT_MIN_BOLDR ) && ( r <= FLFT_MAX_BOLDR ) )
    {
        flagBoldRatio = r;
    }
}

float FLFTRender::BoldRatio()
{
    return flagBoldRatio;
}

float FLFTRender::DefaultBoldRatio()
{
    return FLFT_DEFAULT_BOLDR;
}

void FLFTRender::Italic( bool onoff )
{
    flagItalic = onoff;
}

bool FLFTRender::Italic()
{
    return flagItalic;
}

void FLFTRender::AdditionalSpace( long av )
{
    if ( additionalspaceX != av )
        additionalspaceX = av;
}

long FLFTRender::AdditionalSpace()
{
    return additionalspaceX;
}

unsigned FLFTRender::Faces()
{
    if ( fface != NULL )
    {
        FT_Face face = (FT_Face)fface;

        if ( face->num_faces > 0 )
            return face->num_faces;
    }
    
    return 0;
}

unsigned FLFTRender::Glyphs()
{
    if ( fface != NULL )
    {
        FT_Face face = (FT_Face)fface;
        
        if ( face->num_glyphs > 0 )
            return face->num_glyphs;
    }
    
    return 0;
}

unsigned FLFTRender::Charmaps()
{
    if ( fface != NULL )
    {
        FT_Face face = (FT_Face)fface;
        
        if ( face->num_charmaps > 0 )
            return face->num_charmaps;
    }
    
    return 0;    
}

const char* FLFTRender::FamilyName()
{
    if ( fface != NULL )
    {
        FT_Face face = (FT_Face)fface;
        
        return face->family_name;
    }
    
    return NULL;
}

const char* FLFTRender::StyleName()
{
    if ( fface != NULL )
    {
        FT_Face face = (FT_Face)fface;
        
        return face->style_name;
    }
    
    return NULL;
}

bool FLFTRender::MeasureText( const char* text, Rect &rect )
{
    if ( text == NULL )
        return false;

    bool retb =  false;
    unsigned tlen = strlen( text ) + 1;
    wchar_t* wcsbuff = new wchar_t[tlen];
    if( wcsbuff != NULL )
    {
        memset( wcsbuff, 0, tlen );
        mbstowcs( wcsbuff, text, tlen );
        retb = MeasureText( wcsbuff, rect );
        delete[] wcsbuff;
    }

    return retb;    
}

bool FLFTRender::MeasureText( const wchar_t* text, Rect &rect )
{
    // check references are existed.
    if ( text == NULL )
    {
        rect.x = 0;
        rect.y = 0;
        rect.w = 0;
        rect.h = 0;
        return false;
    }

    // --------------------------------------------------

    // Casting ...
    FT_Library libFL = (FT_Library)fflib;
    FT_Face face = (FT_Face)fface;
    
    rect.x = 0;
    rect.y = 0;
    rect.w = 0;
    rect.h = 0;

    if ( ( fflib == NULL ) || ( fface == NULL ) )
        return false;
    
    // make buffer enough.
    unsigned    llen = wcslen( text );
    unsigned    s_x = 0;
    unsigned    s_y = ffsize;
    long        m_h = 0;
    long        m_w = 0;
    unsigned    corr_w2 = 0;

    if ( additionalspaceX != 0 )
    {
        if ( (long)s_x + additionalspaceX > 0 )
        {
            s_x += additionalspaceX;
        }
    }

    for( unsigned cnt=0; cnt<llen; cnt++ )
    {
        FT_Matrix tfmat  = { 0x10000, 0, 0, 0x10000 };
        
        if ( ( flagItalic == true ) || ( flagBold == true ) )
        {
            if ( flagBold == true )
            {
                tfmat.xx = (FT_Fixed)( tfmat.xx * flagBoldRatio );
            }
            
            if ( flagItalic == true )
            {
                float shear = tan( ( 90.f - 75.f ) / 180.f * FLFT_PI_F );
                
                tfmat.xy = (FT_Fixed)( shear * 0x10000 );

                if ( corr_w2 == 0 )
                {
                    corr_w2 = ( 0x10000 - tfmat.xy ) >> 16;
                }
            }
        }        
                            
        FT_Set_Transform( face, &tfmat, NULL );
                        
        FT_Error err =  FT_Load_Char( face, text[cnt], FT_LOAD_NO_BITMAP );
        if ( err == 0 )
        {                        
            unsigned t_rows = face->glyph->bitmap.rows;
            unsigned t_cols = face->glyph->bitmap.width;
            unsigned t_pitc = face->glyph->bitmap.pitch;
            long     t_top  = face->glyph->bitmap_top;
            
            long t_h = t_rows + t_top;

            m_h = __MAX( m_h, t_h );
            m_w = __MAX( m_w, s_x + t_cols );
            
#ifdef DEBUG_TTF_REGION_CHARS
            printf( "t_rows = %u, t_cols = %u, t_pitc = %u, t_top = %ld\n",
                    t_rows, t_cols, t_pitc, t_top );
#endif /// of DEBUG_TTF_REGION

            s_x += face->glyph->advance.x >> 6;
            s_y += face->glyph->advance.y >> 6;
            
            if ( additionalspaceX != 0 )
            {
                if ( (long)s_x + additionalspaceX > 0 )
                {
                    s_x += additionalspaceX;
                }
            }

        } /// of if Load char -
        else
        {
#ifdef DEBUG
            printf( "failed to FL_Load_Char(), text = %C ( %u ), return error = %d\n",
                    text[cnt], (unsigned)text[cnt], err );
            printf( "Error : %s\n", FT_Error_String( err ) );
        
            fflush(stdout);        
#endif
            return false;
        }
        
        // Try to get kerning -
        if ( fkerning == true )
        {
            if ( ( llen > 3 ) && ( cnt > 0 ) && ( cnt + 2 < llen ) )
            {
                FT_UInt   idx1 = FT_Get_Char_Index( face, text[ cnt-1 ] );
                FT_UInt   idx2 = FT_Get_Char_Index( face, text[ cnt+1 ] );
                FT_Vector kern = {0};
                
                if ( FT_Get_Kerning( face, idx1, idx2, FT_KERNING_DEFAULT, &kern ) == 0 )
                {
                    s_x += kern.x >> 6;
                    s_y += kern.y >> 6;
                }
            }
        }        
    }
    
#ifdef DEBUG_TTF_REGION
    printf( "s_y = %u, m_h = %u\n", s_y, m_h );
#endif /// of DEBUG_TTF_REGION

    rect.x = 0;
    rect.y = 0;
    rect.w = m_w;
    rect.h = m_h;

    if ( (long)m_h > (long)( ( m_h - s_y ) / 2 ) )
    {
        rect.h -= ( ( m_h - s_y ) / 2 );
    }
    else
    {
        rect.h += ( ( s_y - m_h ) / 2 );
    }
    
    // correct minimal height.
    if ( rect.h < ffsize )
    {
        rect.h = ffsize;
    }
    
    if ( additionalspaceX != 0 )
    {
        rect.w += abs( additionalspaceX ) * 2;
    }

#ifdef DEBUG_TTF_REGION
    printf( "rect width: %u, height: %u\n", rect.w, rect.h );
    fflush( stdout );
#endif /// of DEBUG_TTF_REGION
    
    return true;
}

bool FLFTRender::RenderText( Fl_RGB_Image* &target, unsigned x, unsigned y, const char* text, Rect* rect )
{
    if ( text == NULL )
        return false;

    bool retb =  false;
    unsigned tlen = strlen( text ) + 1;
    wchar_t* wcsbuff = new wchar_t[tlen];
    if( wcsbuff != NULL )
    {
        memset( wcsbuff, 0, tlen );
        mbstowcs( wcsbuff, text, tlen );
        retb = RenderText( target, x, y, wcsbuff, rect );
        delete[] wcsbuff;
    }

    return retb;
}

bool FLFTRender::RenderText( Fl_RGB_Image* &target, unsigned x, unsigned y, const wchar_t* text, Rect* rect )
{
    // check references are existed.
    if ( ( text == NULL ) || ( target == NULL ) )
        return false;

    // check target image --
    if( target->d() == 2 ) /// unsupported color depth !
        return false;

    if ( ( target->w() == 0 ) || ( target->h() == 0 ) )
        return false;

    if ( ( x > target->w() ) || ( y > target->h() ) )
        return false;

    // --------------------------------------------------

    // Casting ...
    FT_Library libFL = (FT_Library)fflib;
    FT_Face face = (FT_Face)fface;

    if ( ( fflib == NULL ) || ( fface == NULL ) )
        return false;

    // make buffer enough.
    unsigned    llen = wcslen( text );
    unsigned    b_w = target->w();
    unsigned    b_h = target->h();
    unsigned    b_d = target->d();
    unsigned    s_x = x;
    unsigned    s_y = y + ffsize;
    long        m_w = 0;
    long        m_h = 0;

    if ( additionalspaceX != 0 )
    {
        if ( (long)s_x + additionalspaceX > 0 )
        {
            s_x += additionalspaceX;
        }
    }

    float fcolf[4] = {0.f};
    col2rgbaf( fcolf[0], fcolf[1], fcolf[2], fcolf[3] );

    unsigned char* renderbuffer = (unsigned char*)target->data()[0];

    if( renderbuffer != NULL )
    {
        for( unsigned cnt=0; cnt<llen; cnt++ )
        {            
            FT_Matrix tfmat  = { 0x10000, 0, 0, 0x10000 };
            
            if ( ( flagItalic == true ) || ( flagBold == true ) )
            {
                if ( flagBold == true )
                {
                    tfmat.xx = (FT_Fixed)( tfmat.xx * flagBoldRatio );
                }
                
                if ( flagItalic == true )
                {
                    float shear = tan( ( 90.f - 75.f ) / 180.f * FLFT_PI_F );
                    
                    tfmat.xy = (FT_Fixed)( shear * 0x10000 );
                }
            }
                                
            FT_Set_Transform( face, &tfmat, NULL );
            
            FT_Error err = FT_Load_Char( face, text[cnt], FT_LOAD_RENDER );
            if ( err == 0 )
            {           
                unsigned t_rows = face->glyph->bitmap.rows;
                unsigned t_cols = face->glyph->bitmap.width;
                unsigned t_pitc = face->glyph->bitmap.pitch;
                long     t_top  = face->glyph->bitmap_top;

#ifdef DEBUG_TTF_RENDER_REGION
                printf( "t_rows = %u, t_cols = %u, t_pitc = %u, t_top = %ld\n",
                        t_rows, t_cols, t_pitc, t_top );
                fflush(stdout);
#endif /// of DEBUG_TTF_RENDER_REGION
                
                long t_h = t_rows + t_top;

                m_h = __MAX( m_h, t_h );
                m_w = __MAX( m_w, s_x + t_cols );

                #pragma omp parallel for
                for( unsigned row=0; row<t_rows; row++ )
                {
                    for( unsigned col=0; col<t_cols; col++ )
                    {
                        if ( ( ( s_x + col ) < b_w ) && ( ( s_y - t_top + row ) < b_h ) )
                        {
                            unsigned pos   = ( ( s_y - t_top )*b_w + s_x + col + row * b_w ) * b_d;
                            unsigned grpos = t_pitc * row + col;
                            float    gdf   = ( face->glyph->bitmap.buffer[ grpos ] ) / 255.f;

                            switch( b_d )
                            {
                                case 1: /// gray-scaled.
                                {
                                    float dp = (float)renderbuffer[ pos ] / 255.f;

                                    dp += ( gdf * fcolf[3] );
                                    if ( dp > 1.f ) dp = 1.f;

                                    renderbuffer[ pos ] = (unsigned char)( dp * 255.f );
                                }
                                break;

                                case 3: /// RGB.
                                {
                                    float rf = (float)( renderbuffer[ pos + 0 ] ) / 255.f;
                                    float gf = (float)( renderbuffer[ pos + 1 ] ) / 255.f;
                                    float bf = (float)( renderbuffer[ pos + 2 ] ) / 255.f;

                                    rf += ( fcolf[0] * fcolf[3] * gdf );
                                    if ( rf > 1.f ) rf = 1.f;

                                    gf += ( fcolf[1] * fcolf[3] * gdf );
                                    if ( gf > 1.f ) gf = 1.f;

                                    bf += ( fcolf[2] * fcolf[3] * gdf );
                                    if ( bf > 1.f ) bf = 1.f;

                                    renderbuffer[ pos + 0 ] = (unsigned char)(rf * 255.f);
                                    renderbuffer[ pos + 1 ] = (unsigned char)(gf * 255.f);
                                    renderbuffer[ pos + 2 ] = (unsigned char)(bf * 255.f);
                                }
                                break;

                                case 4: /// RGBA
                                {
                                    float rf = (float)( renderbuffer[ pos + 0 ] ) / 255.f;
                                    float gf = (float)( renderbuffer[ pos + 1 ] ) / 255.f;
                                    float bf = (float)( renderbuffer[ pos + 2 ] ) / 255.f;
                                    float af = (float)( renderbuffer[ pos + 3 ] ) / 255.f;

                                    rf += ( fcolf[0] * fcolf[3] * gdf );
                                    if ( rf > 1.f ) rf = 1.f;

                                    gf += ( fcolf[1] * fcolf[3] * gdf );
                                    if ( gf > 1.f ) gf = 1.f;

                                    bf += ( fcolf[2] * fcolf[3] * gdf );
                                    if ( bf > 1.f ) bf = 1.f;
                                    
                                    af += ( fcolf[3] * gdf );
                                    if ( af > 1.f ) af = 1.f;

                                    renderbuffer[ pos + 0 ] = (unsigned char)(rf * 255.f);
                                    renderbuffer[ pos + 1 ] = (unsigned char)(gf * 255.f);
                                    renderbuffer[ pos + 2 ] = (unsigned char)(bf * 255.f);
                                    renderbuffer[ pos + 3 ] = (unsigned char)(af * 255.f);
                                }
                                break;
                            }
                        }
                    }
                }

                s_x += face->glyph->advance.x >> 6;
                s_y += face->glyph->advance.y >> 6;
                
                if ( additionalspaceX != 0 )
                {
                    if ( (long)s_x + additionalspaceX > 0 )
                    {
                        s_x += additionalspaceX;
                    }
                }

            } /// of if Load char -
            else
            {
#ifdef DEBUG
                printf( "failed to FL_Load_Char(), text = %C ( %u ), return error = %d\n",
                        text[cnt], (unsigned)text[cnt], err );
                printf( "Error : %s\n", FT_Error_String( err ) );
            
                fflush(stdout);        
#endif                
            }
            
            // Try to get kerning -
            if ( fkerning == true )
            {
                if ( ( llen > 3 ) && ( cnt > 0 ) && ( cnt + 2 < llen ) )
                {
                    FT_UInt   idx1 = FT_Get_Char_Index( face, text[ cnt-1 ] );
                    FT_UInt   idx2 = FT_Get_Char_Index( face, text[ cnt+1 ] );
                    FT_Vector kern = {0};
                    
                    if ( FT_Get_Kerning( face, idx1, idx2, FT_KERNING_DEFAULT, &kern ) == 0 )
                    {
                        s_x += kern.x >> 6;
                        s_y += kern.y >> 6;
                    }
                }
            }            
        }
        
#ifdef DEBUG_TTF_RENDER_REGION
        printf( "y = %u, s_y = %u, m_h = %u\n", y, s_y - y, m_h );
#endif /// of DEBUG_TTF_RENDER_REGION

        if ( rect != NULL )
        {
            rect->x = x;
            rect->y = y;
            rect->w = m_w - x;
            rect->h = m_h;

            if ( (long)m_h > (long)( ( m_h - ( s_y - y ) ) / 2 ) )
            {
                rect->h -= ( ( m_h - ( s_y - y ) ) / 2 );
            }
            else
            {
                rect->h += ( ( ( s_y - y ) - m_h ) / 2 );
            }
            
            // correct minimal height.
            if ( rect->h < ffsize )
            {
                rect->h = ffsize;
            }
                        
            if ( additionalspaceX != 0 )
            {
                rect->x += additionalspaceX;
                rect->w += abs( additionalspaceX ) * 2;
            }
#ifdef DEBUG_TTF_RENDER_REGION
            printf( "rect: %u,%u,%u,%u\n", rect->x, rect->y, rect->w, rect->h );
            fflush( stdout );
#endif /// of DEBUG_TTF_RENDER_REGION
        }

        return true;
    }

    return false;
}

void FLFTRender::col2rgbaf( float &r, float &g, float &b, float &a )
{
    r = (float)( ( fcolor & 0XFF000000 ) >> 24 ) / 255.f;
    g = (float)( ( fcolor & 0x00FF0000 ) >> 16 ) / 255.f;
    b = (float)( ( fcolor & 0x0000FF00 ) >>  8 ) / 255.f;
    a = (float)( fcolor & 0x000000FF ) / 255.f;
}

void FLFTRender::init()
{
    if ( ( fflib != NULL ) && ( fface != NULL ) )
    {
        FT_Library libFL = (FT_Library)fflib;
        FT_Face face = (FT_Face)fface;
        
        // detect kerning flag existed ?
        fkerning = FT_HAS_KERNING( face );
        
        // bug_fix: set default font size
        FT_Set_Pixel_Sizes( face, 0, ffsize );
    }
}

// A static function for Load TTF from filesystem (Windows)
bool FLFTRender::Loader( const wchar_t* ttfpath, long idx, FLFTRender* &flftr )
{
    if ( _waccess( ttfpath, 0 ) == 0 )
    {
        FILE* fp = _wfopen( ttfpath, L"rb" );
        if ( fp != NULL )
        {
            bool retb = false;
            
            fseek( fp, 0L, SEEK_END );
            unsigned fsz = ftell( fp );
            rewind( fp );
            
            if ( fsz > 0 )
            {
                unsigned char* buff = new unsigned char[fsz];
                if ( buff != NULL )
                {
                    fread( buff, 1, fsz, fp );
                    flftr = new FLFTRender( buff, fsz, idx );
                    if ( flftr != NULL )
                    {
                        if ( flftr->FontLoaded() == true )
                        {
                            retb = true;
                        }
                    }
                }
            }

            fclose( fp );
            
            return retb;
        }
    }
    
    return false;
}
