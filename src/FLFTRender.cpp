#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <omp.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "FLFTRender.h"

////////////////////////////////////////////////////////////////////////////////

#define FLFT_DEFAULT_SIZE       12
#define FLFT_DEFAULT_COLOR      0xFFFFFFFF

////////////////////////////////////////////////////////////////////////////////

FLFTRender::FLFTRender( const char* ttf )
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
        if ( ttf != NULL )
        {
            if ( access( ttf, 0 ) == 0 )
            {
                if ( FT_New_Face( libFL, ttf, 0, &face ) == 0 )
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

bool FLFTRender::RenderText( Fl_RGB_Image* &target, unsigned x, unsigned y, const char* text  )
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
        retb = RenderText( target, x, y, wcsbuff );
        delete[] wcsbuff;
    }

    return retb;
}

bool FLFTRender::RenderText( Fl_RGB_Image* &target, unsigned x, unsigned y, const wchar_t* text  )
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
                unsigned t_top  = face->glyph->bitmap_top;

                #pragma omp parallel for
                for( unsigned row=0; row<t_rows; row++ )
                {
                    for( unsigned col=0; col<t_cols; col++ )
                    {
                        unsigned pos = ( ( s_y - t_top )*b_w + s_x + col + row * b_w ) * b_d;

                        if ( ( s_x < b_w ) && ( s_y < b_h ) )
                        {
                            unsigned grpos = t_pitc * row + col;
                            float gdf = ( face->glyph->bitmap.buffer[ grpos ] ) / 255.f;

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
                            }
                        }
                    }
                }

                s_x += face->glyph->advance.x >> 6;
                s_y += face->glyph->advance.y >> 6;
            }
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
