#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

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

void drawLines( Fl_RGB_Image* img, unsigned y, unsigned h )
{
    fl_imgtk::draw_line( img, 
                         10, y, 1270, y,
                         (Fl_Color)0xFF3333F8F );
    fl_imgtk::draw_line( img,
                         10, y + h, 1270, y + h,
                         (Fl_Color)0x33FF338F );
}

void drawRect( Fl_RGB_Image* img, FLFTRender::Rect * r, unsigned col = 0x5555FFAF)
{
    fl_imgtk::draw_rect( img,
                         r->x, r->y, r->w, r->h,
                         col );
}

int main( int argc, char** argv )
{
    if ( access( FNT_N, 0 ) != 0 )
    {
        printf( "ERROR: Font file %s need to run this testing.\n", FNT_N );
        return 0;
    }

    Fl_RGB_Image* imgGrad = NULL;

    char wintitle[80] = {0};
    snprintf( wintitle, 80, "FLFTRender %s testing window", FLFTRENDER_VERSION_S );
    
    // Just make an window ..
    Fl_Window window( 1280, 720, wintitle );
    window.color( 0x00000000 );
    window.begin();
    
        // Create a box for contians testing image.
        Fl_Box boxImage( 0, 0, 1280, 720 );
        boxImage.box( FL_NO_BOX );

        // make a gradation background image and draw text on it.
        imgGrad = fl_imgtk::makegradation_h( window.w(), window.h(),
                                             0x484848FF, 0xAAAAAA3F, 
                                             true );
        if ( imgGrad != NULL )
        {
            unsigned slope = 60;
            unsigned lstart = 10;
            unsigned lend = window.w() - slope - 10;
            unsigned ly = 10;
            unsigned lh = window.h() - 10;
            
            unsigned colshade = 0xF0000000;
            for( unsigned cnt=lstart; cnt<lend; cnt+=10 )
            {
                fl_imgtk::draw_smooth_line_ex ( imgGrad,
                                                cnt, ly, cnt + slope, lh,
                                                2.3f,
                                                colshade&0xFFFFFF00|0x3E );
                colshade = colshade >> 1;
                if ( colshade <= 0x00000FFF )
                    colshade = 0xF0000000;
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
                
                string teststr;
                wstring testwstr;
                
                flftr.AdditionalSpace( 4 );
                
                FLFTRender::Rect mrect = {0};
                FLFTRender::Rect rect = {0};

                teststr = "FLFTRender testing :";
                flftr.FontSize( 75 );
                flftr.FontColor( 0x3366FFFF );
                flftr.Italic( true );
                flftr.MeasureText( teststr.c_str(), mrect );
                mrect.x = 10;
                mrect.y = 10;
                drawRect( imgGrad, &mrect, 0xFF0000AF );
                flftr.RenderText( imgGrad, 10, 10, teststr.c_str(), &rect );
                drawRect( imgGrad, &rect );
                flftr.FontSize( 40 );
                flftr.FontColor( 0xFFFFFF7F );
                flftr.Italic( false );

                unsigned putY = 100;
                
                teststr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ !@#$%^&*()_+";
                drawLines( imgGrad, putY, flftr.FontSize() );
                flftr.MeasureText( teststr.c_str(), mrect );
                mrect.x = 10;
                mrect.y = putY;
                drawRect( imgGrad, &mrect, 0xFF20409F );
                flftr.RenderText( imgGrad, 10, putY, teststr.c_str(), &rect );
                drawRect( imgGrad, &rect );
                putY += 50;

                teststr = "abcdefghijklmnopqrstuvwxyz 1234567890-=";
                drawLines( imgGrad, putY, flftr.FontSize() );
                flftr.MeasureText( teststr.c_str(), mrect );
                mrect.x = 10;
                mrect.y = putY;
                drawRect( imgGrad, &mrect, 0xFF20409F );
                flftr.RenderText( imgGrad, 10, putY, teststr.c_str(), &rect );
                drawRect( imgGrad, &rect );
                putY += 50;

                teststr.clear();
                testwstr = L"가나다라마바사아자차카타파하, 대한민국 한글!";
                drawLines( imgGrad, putY, flftr.FontSize() );
                flftr.MeasureText( testwstr.c_str(), mrect );
                mrect.x = 10;
                mrect.y = putY;
                drawRect( imgGrad, &mrect, 0xFF20409F );
                flftr.RenderText( imgGrad, 10, putY, testwstr.c_str(), &rect );
                drawRect( imgGrad, &rect );
                putY += 50;

                testwstr = L"※☆★○●◎◇◆□■△▲▽▼→←←↑↓↔〓㉠㉡㉢㉣㉤㉥㉦㉧㉨㉩㉪";
                drawLines( imgGrad, putY, flftr.FontSize() );
                flftr.MeasureText( testwstr.c_str(), mrect );
                mrect.x = 10;
                mrect.y = putY;
                drawRect( imgGrad, &mrect, 0xFF20409F );
                flftr.RenderText( imgGrad, 10, putY, testwstr.c_str(), &rect );
                drawRect( imgGrad, &rect );
                putY += 50;

                // alpha depth test ...
                testwstr = L"ΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡΣΤΥΦΧΨΩαβγδεζηθικλμνξοπρστυφχψω";
                flftr.AdditionalSpace( 0 );
                flftr.FontSize( 50 );
                flftr.FontColor( 0x3366995F );
                flftr.MeasureText( testwstr.c_str(), mrect );
                mrect.x = 10;
                mrect.y = putY;
                drawRect( imgGrad, &mrect, 0xFF20409F );
                flftr.RenderText( imgGrad, 10, putY, testwstr.c_str(), &rect );
                drawRect( imgGrad, &rect );
                
                putY += 70;
                // alpha depth over-width test ...
                testwstr = L"ΑΒΓΔΕΖΗΘΙΚΛΜΝΞΟΠΡΣΤΥΦΧΨΩαβγδεζηθικλμνξοπρστυφχψω";
                flftr.AdditionalSpace( 8 );                
                flftr.FontSize( 20 );
                flftr.FontColor( 0x6633995F );
                flftr.MeasureText( testwstr.c_str(), mrect );
                mrect.x = 10;
                mrect.y = putY;
                drawRect( imgGrad, &mrect, 0xFF20409F );
                flftr.RenderText( imgGrad, 10, putY, testwstr.c_str(), &rect );
                drawRect( imgGrad, &rect );

                putY += 30;
                // Plain text
                testwstr = L"Plain text : 일반 텍스트";
                flftr.AdditionalSpace( 0 );
                flftr.Bold( false );
                flftr.Italic( false );
                flftr.FontSize( 20 );
                flftr.FontColor( 0xFFFFFFEF );
                flftr.MeasureText( testwstr.c_str(), mrect );
                mrect.x = 10;
                mrect.y = putY;
                drawRect( imgGrad, &mrect, 0xFF20409F );
                flftr.RenderText( imgGrad, 10, putY, testwstr.c_str(), &rect );
                drawRect( imgGrad, &rect );

                putY += 30;
                // Bold
                testwstr = L"Bold text : 두꺼운 텍스트";
                flftr.AdditionalSpace( 0 );
                flftr.Bold( true );
                flftr.Italic( false );
                flftr.FontSize( 20 );
                flftr.FontColor( 0xFFFFFFEF );
                flftr.MeasureText( testwstr.c_str(), mrect );
                mrect.x = 10;
                mrect.y = putY;
                drawRect( imgGrad, &mrect, 0xFF20409F );
                flftr.RenderText( imgGrad, 10, putY, testwstr.c_str(), &rect );
                drawRect( imgGrad, &rect );
                
                putY += 30;
                // Bold
                testwstr = L"Booooooooold text : 두꺼어어어어어운 텍스트";
                flftr.AdditionalSpace( 0 );
                flftr.Bold( true );
                flftr.BoldRatio( 8.f );
                flftr.Italic( false );
                flftr.FontSize( 20 );
                flftr.FontColor( 0xFFFFFFEF );
                flftr.MeasureText( testwstr.c_str(), mrect );
                mrect.x = 10;
                mrect.y = putY;
                drawRect( imgGrad, &mrect, 0xFF20409F );
                flftr.RenderText( imgGrad, 10, putY, testwstr.c_str(), &rect );
                drawRect( imgGrad, &rect );
                flftr.ResetBoldRatio();

                putY += 30;
                // Thin
                testwstr = L"Thin text : 얇은 텍스트";
                flftr.AdditionalSpace( 0 );
                flftr.WidthRatio( 0.5f );
                flftr.Bold( false );
                flftr.Italic( false );
                flftr.FontSize( 20 );
                flftr.FontColor( 0xFFFFFFEF );
                flftr.MeasureText( testwstr.c_str(), mrect );
                mrect.x = 10;
                mrect.y = putY;
                drawRect( imgGrad, &mrect, 0xFF20409F );
                flftr.RenderText( imgGrad, 10, putY, testwstr.c_str(), &rect );
                drawRect( imgGrad, &rect );
                flftr.ResetWidthRatio();

                putY += 30;
                // Italic
                testwstr = L"Italic text : 이탤릭 텍스트";
                flftr.AdditionalSpace( 0 );
                flftr.Bold( false );
                flftr.Italic( true );
                flftr.FontSize( 20 );
                flftr.FontColor( 0xFFFFFFEF );
                flftr.MeasureText( testwstr.c_str(), mrect );
                mrect.x = 10;
                mrect.y = putY;
                drawRect( imgGrad, &mrect, 0xFF20409F );
                flftr.RenderText( imgGrad, 10, putY, testwstr.c_str(), &rect );
                drawRect( imgGrad, &rect );

                putY += 30;
                // Italic + Bold
                testwstr = L"Italic bold text : 이탤릭 두꺼운 텍스트";
                flftr.AdditionalSpace( 0 );
                flftr.Bold( true );
                flftr.Italic( true );
                flftr.FontSize( 42 );
                flftr.FontColor( 0xFFFFFFEF );
                flftr.MeasureText( testwstr.c_str(), mrect );
                mrect.x = 10;
                mrect.y = putY;
                drawRect( imgGrad, &mrect, 0xFF80A0AF );
                flftr.RenderText( imgGrad, 10, putY, testwstr.c_str(), &rect );
                drawRect( imgGrad, &rect );


                testwstr.clear();
            }

            imgGrad->uncache();

            boxImage.image( imgGrad );
        }
    window.end();
    window.show();

    int reti = Fl::run();

    if ( imgGrad != NULL )    
    {
        fl_imgtk::discard_user_rgb_image( imgGrad );
    }

    return reti;
}
