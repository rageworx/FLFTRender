#ifndef __FLFTRender_H__
#define __FLFTRender_H__

/*******************************************************************************
*  libfreetype2 toolkit for FLTK RGB uinmage
*  =========================================
*  (C)2020, Raphael Kim
*******************************************************************************/

#include <FL/Fl.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_RGB_Image.H>

// -----------------------------------------------------------------------------
// Version : 0.1.0.0
#define  FLFTRENDER_VERSION         0x00010000

// -----------------------------------------------------------------------------

class FLFTRender
{
    public:
        FLFTRender( const char* ttf = NULL );
        ~FLFTRender();

    public:
        bool        FontLoaded();
        void        FontSize( unsigned newsz );
        unsigned    FontSize();
        void        FontColor( unsigned rgba );
        unsigned    FontColor();

    public:
        bool        RenderText( Fl_RGB_Image* &target, unsigned x, unsigned y, const char* text );
        bool        RenderText( Fl_RGB_Image* &target, unsigned x, unsigned y, const wchar_t* text );

    protected:
        void        col2rgbaf( float &r, float &g, float &b, float &a );

    protected:
        void*           fface;
        void*           fflib;
        unsigned        ffsize;
        unsigned        fcolor;
        bool            loaded;
};

#endif // __FLFTRender_H__
