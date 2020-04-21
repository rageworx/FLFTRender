#ifndef __FLFTRender_H__
#define __FLFTRender_H__

/*******************************************************************************
*
*  libfreetype2 toolkit for FLTK RGB inmage
*  ========================================
*  (C)Copyrighted 2020, Raphael Kim
*  [ source at ] https://github.com/rageworx/FLFTRender
*
*******************************************************************************/

#include <FL/Fl.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_RGB_Image.H>
#ifndef _WIN32
#include <wchar.h>
#endif /// of _WIN32

// -----------------------------------------------------------------------------
// Version : 0.1.9.25 [ build, 25 ]
#define     FLFTRENDER_VERSION_S        "0.1.9.25"
#define     FLFTRENDER_VERSION          0x00010919
#define     FLFTRENDER_VERSION_EX       0x00000019

// -----------------------------------------------------------------------------

class FLFTRender
{
    public:
        typedef struct _Rect
        {
            unsigned x;
            unsigned y;
            unsigned w;
            unsigned h;
        }Rect;
        
    public:
        FLFTRender( const char* ttf = NULL, long idx = 0 );
        FLFTRender( const unsigned char* ttfbuff = NULL, \
                    unsigned ttfbuffsz = 0, long idx = 0 );
        ~FLFTRender();

    public:
        bool        FontLoaded();
        void        FontSize( unsigned newsz );
        unsigned    FontSize();
        void        FontColor( unsigned rgba );
        unsigned    FontColor();
        void        Bold( bool onoff );
        bool        Bold();
        void        BoldRatio( float r );
        float       BoldRatio();
        float       DefaultBoldRatio();
        void        Italic( bool onoff );
        bool        Italic();
        void        AdditionalSpace( long av );
        long        AdditionalSpace();
        
    public:
        unsigned    Faces();
        unsigned    Glyphs();
        unsigned    Charmaps();
        const char* FamilyName();
        const char* StyleName();

    public:
        bool        MeasureText( const char* text, Rect &rect );
        bool        MeasureText( const wchar_t* text, Rect &rect );
        bool        RenderText( Fl_RGB_Image* &target, unsigned x, unsigned y, \
                                const char* text, \
                                Rect* rect = NULL );
        bool        RenderText( Fl_RGB_Image* &target, unsigned x, unsigned y, \
                                const wchar_t* text, \
                                Rect* rect = NULL );

    public:
        /*
        ** A sataic function to load TTF font from Windows base file systems,
        ** or aisan, or special charactor file name with wide charactor.
        */
        static bool Loader( const wchar_t* ttfpath, long idx, \
                            FLFTRender* &flftr );
                                        
    protected:
        void        col2rgbaf( float &r, float &g, float &b, float &a );
        void        init();

    protected:
        void*           fface;
        void*           fflib;
        unsigned        ffsize;
        unsigned        fcolor;
        bool            fkerning;
        bool            loaded;
        long            additionalspaceX;
        bool            flagBold;
        bool            flagItalic;
        float           flagBoldRatio;
        
    private:
        unsigned char*  ttfbuffer;
        unsigned        ttfbufferlen;
};

#endif // __FLFTRender_H__
