#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_RGB_Image.H>
#include <fl_imgtk.h>
#include <FLFTRender.h>

using namespace std;

#define FNT_N   "D2CodingBold-Ver1.3.2-20180524.ttf"

int main( int argc, char** argv )
{
    Fl_Window window( 1280, 720, "Testing Window" );
    window.begin();
    
        Fl_Box boxImage( 0, 0, 1280, 720 );
        boxImage.box( FL_NO_BOX );

        Fl_RGB_Image* imgGradation = fl_imgtk::makegradation_h( 1280, 720, 0xAAAAAAFF, 0x333333FF, true );
        if ( imgGradation != NULL )
        {
            // Write something on here.
            FLFTRender flftr( FNT_N );
            if ( flftr.FontLoaded() == true )
            {
                flftr.FontSize( 40 );
                flftr.FontColor( 0xFFFFFF7F );

                unsigned putY = 100;
                fl_imgtk::draw_line( imgGradation, 
                                     10, putY, 1270, putY, 
                                     0xFF3333AF );
                fl_imgtk::draw_line( imgGradation,
                                     10, putY + 40, 1270, putY + 40,
                                     0x33FF33AF );
                flftr.RenderText( imgGradation, 10, putY,
                                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ !@#$%^&*()_+" );
                putY += 50;

                fl_imgtk::draw_line( imgGradation, 
                                     10, putY, 1270, putY, 
                                     0xFF3333AF );
                fl_imgtk::draw_line( imgGradation,
                                     10, putY + 40, 1270, putY + 40,
                                     0x33FF33AF );
                flftr.RenderText( imgGradation, 10, putY,
                                  "abcdefghijklmnopqrstuvwxyz 1234567890-=" );
                putY += 50;

                fl_imgtk::draw_line( imgGradation, 
                                     10, putY, 1270, putY, 
                                     0xFF3333AF );
                fl_imgtk::draw_line( imgGradation,
                                     10, putY + 40, 1270, putY + 40,
                                     0x33FF33AF );
                flftr.RenderText( imgGradation, 10, putY,
                                  L"가나다라마바사아자차카타파하, 대한민국 한글!" );
                putY += 50;


            }

            imgGradation->uncache();

            boxImage.image( imgGradation );
        }
    window.end();
    window.show();

    return Fl::run();

    return 0;
}
