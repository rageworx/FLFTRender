#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>


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

////////////////////////////////////////////////////////////////////////////////

FLFTRender::FLFTRender( const char* ttf, long idx )
 :  fface( NULL ),
    fflib( NULL ),
    ffsize( FLFT_DEFAULT_SIZE ),
    fcolor( FLFT_DEFAULT_COLOR ),
    loaded( false ),
    ttfbuffer( NULL ),
    ttfbufferlen( 0 )
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
}

FLFTRender::FLFTRender( const unsigned char* ttfbuff, unsigned ttfbuffsz, long idx )
 :  fface( NULL ),
    fflib( NULL ),
    ffsize( FLFT_DEFAULT_SIZE ),
    fcolor( FLFT_DEFAULT_COLOR ),
    loaded( false )
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
    unsigned llen = wcslen( text );
    unsigned b_w = target->w();
    unsigned b_h = target->h();
    unsigned b_d = target->d();
    unsigned s_x = x;
    unsigned s_y = y + ffsize;
    long     m_h = 0;

    float fcolf[4] = {0.f};
    col2rgbaf( fcolf[0], fcolf[1], fcolf[2], fcolf[3] );

    unsigned char* renderbuffer = (unsigned char*)target->data()[0];

    if( renderbuffer != NULL )
    {
        for( unsigned cnt=0; cnt<llen; cnt++ )
        {
            if ( FT_Load_Char( face, text[cnt], FT_LOAD_RENDER ) == 0 )
            {
                unsigned t_rows = face->glyph->bitmap.rows;
                unsigned t_cols = face->glyph->bitmap.width;
                unsigned t_pitc = face->glyph->bitmap.pitch;
                long     t_top  = face->glyph->bitmap_top;

#ifdef DEBUG_TTF_REGION
                printf( "t_rows = %u, t_cols = %u, t_pitc = %u, t_top = %ld\n",
                        t_rows, t_cols, t_pitc, t_top );
                fflush(stdout);
#endif /// of DEBUG_TTF_REGION
                
                long t_h = t_rows + t_top;

                m_h = __MAX( m_h, t_h );

                #pragma omp parallel for
                for( unsigned row=0; row<t_rows; row++ )
                {
                    for( unsigned col=0; col<t_cols; col++ )
                    {
                        if ( ( ( s_x + col ) < b_w ) && ( ( s_y + row ) < b_h ) )
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
            }
        }
        
#ifdef DEBUG_TTF_REGION
        printf( "y = %u, s_y = %u, m_h = %u\n", y, s_y - y, m_h );
#endif /// of DEBUG_TTF_REGION

        if ( rect != NULL )
        {
            rect->x = x;
            rect->y = y;
            rect->w = s_x - x;
            rect->h = m_h - ( ( m_h - ( s_y - y ) ) / 2 );
#ifdef DEBUG_TTF_REGION
            printf( "rect: %u,%u,%u,%u\n", rect->x, rect->y, rect->w, rect->h );
            fflush( stdout );
#endif /// of DEBUG_TTF_REGION
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

// A static function --
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