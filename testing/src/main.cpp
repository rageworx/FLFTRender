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

// D2CodingBold-Ver1.3.2
#define FNT_N   "D2CodingBold-Ver1.3.2-20180524.ttf"

// Arial Narrow
//#define FNT_N   "ARIALN.TTF"

void drawLines( Fl_RGB_Image* img, unsigned y )
{
    fl_imgtk::draw_line( img, 
                         10, y, 1270, y,
                         0xFF3333AF );
    fl_imgtk::draw_line( img,
                         10, y + 40, 1270, y + 40,
                         0x33FF33AF );
}

void drawRect( Fl_RGB_Image* img, FLFTRender::Rect * r )
{
    fl_imgtk::draw_rect( img,
                         r->x, r->y, r->w, r->h,
                         0x3333FF5F );
}

int main( int argc, char** argv )
{
    if ( access( FNT_N, 0 ) != 0 )
    {
        printf( "ERROR: Font file %s need to run this testing.\n", FNT_N );
        return 0;
    }

    Fl_RGB_Image* imgGradation = NULL;

    // Just make an window ..
    Fl_Window window( 1280, 720, "Testing Window" );
    window.begin();
    
        // Create a box for contians testing image.
        Fl_Box boxImage( 0, 0, 1280, 720 );
        boxImage.box( FL_NO_BOX );

        // make a gradation background image and draw text on it.
        imgGradation = fl_imgtk::makegradation_h( 1280, 720, 
                                                  0xAAAAAAFF, 0x333333FF, 
                                                  true );
        if ( imgGradation != NULL )
        {
            for( unsigned cnt=10; cnt<1200; cnt+=10 )
            {
                fl_imgtk::draw_smooth_line( imgGradation,
                                            cnt, 10, cnt + 60, 710,
                                            0xEE88336F );
            }


            // Write something on here.
            FLFTRender flftr( FNT_N );
            if ( flftr.FontLoaded() == true )
            {
                printf( "Font Info : \n" );
                printf( " - [%s %s] has %u face(s), %u glyphs and %u charmaps.\n", 
                        flftr.FamilyName(), flftr.StyleName(),
                        flftr.Faces(), flftr.Glyphs(), flftr.Charmaps() );
                fflush( stdout );
                
                flftr.AdditionalSpace( 4 );
                
                FLFTRender::Rect rect = {0};

                flftr.FontSize( 75 );
                flftr.FontColor( 0x3366FF3F );
                flftr.RenderText( imgGradation, 10, 10,
                                  "FLFTRender testing :",
                                  &rect );
                drawRect( imgGradation, &rect );
                flftr.FontSize( 40 );
                flftr.FontColor( 0xFFFFFF7F );


                unsigned putY = 100;
                drawLines( imgGradation, putY );
                flftr.RenderText( imgGradation, 10, putY,
                                  "ABCDEFGHIJKLMNOPQRSTUVWXYZ !@#$%^&*()_+",
                                  &rect );
                drawRect( imgGradation, &rect );
                putY += 50;

                drawLines( imgGradation, putY );
                flftr.RenderText( imgGradation, 10, putY,
                                  "abcdefghijklmnopqrstuvwxyz 1234567890-=",
                                  &rect );
                drawRect( imgGradation, &rect );
                putY += 50;

                drawLines( imgGradation, putY );
                flftr.RenderText( imgGradation, 10, putY,
                                  L"가나다라마바사아자차카타파하, 대한민국 한글!",
                                  &rect );
                drawRect( imgGradation, &rect );
                putY += 50;

                drawLines( imgGradation, putY );
                flftr.RenderText( imgGradation, 10, putY,
                                  L"※☆★○●◎◇◆□■△▲▽▼→←←↑↓↔〓㉠㉡㉢㉣㉤㉥㉦㉧㉨㉩㉪",
                                  &rect );
                drawRect( imgGradation, &rect );
                putY += 50;

                drawLines( imgGradation, putY );

                putY += 100;
                // alpha depth test ...
                
                flftr.AdditionalSpace( 0 );
                flftr.FontSize( 50 );
                flftr.FontColor( 0x3366995F );
                flftr.RenderText( imgGradation, 10, putY,
                                  L"ΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡΣΤΥΦΧΨΩαβγδεζηθικλμνξοπρστυφχψω",
                                  &rect );
                drawRect( imgGradation, &rect );
            }

            imgGradation->uncache();

            boxImage.image( imgGradation );
        }
    window.end();
    window.show();

    int reti = Fl::run();

    if ( imgGradation != NULL )    
    {
        fl_imgtk::discard_user_rgb_image( imgGradation );
    }

    return reti;
}
