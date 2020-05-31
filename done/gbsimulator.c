#include "sidlib.h"
#include "gameboy.h"
#include "lcdc.h"
#include "error.h"

#include <stdint.h>

// Key press bits
#define MY_KEY_UP_BIT    0x01
#define MY_KEY_DOWN_BIT  0x02
#define MY_KEY_RIGHT_BIT 0x04
#define MY_KEY_LEFT_BIT  0x08
#define MY_KEY_A_BIT     0x10
#define SCALE 1

// typedef struct gameboy_ gameboy_t;
gameboy_t gameboy;
// ======================================================================
static void set_grey(guchar* pixels, int row, int col, int width, guchar grey)
{
    const size_t i = (size_t) (3 * (row * width + col)); // 3 = RGB
    pixels[i+2] = pixels[i+1] = pixels[i] = grey;
}

// ======================================================================
static void generate_image(guchar* pixels, int height, int width)
{
    // printf("width = %d, height = %d\n", width, height);
    gameboy_run_until(&gameboy, 5000000);
    // printf("No seg fault\n");
    // for (int x = 0; x < width; ++x){
    //     for (int y = 0; y < height; ++y){
    //         uint8_t output = 0;
    //         image_get_pixel(&output, &(gameboy.screen.display), x*SCALE, y*SCALE);
    //         set_grey(pixels, y, x, width, 255 - 85 * output);
    //         // printf("No seg fault, x = %d, y = %d\n", x, y);
    //     }
    // }
}

// ======================================================================
#define do_key(X) \
    do { \
        if (! (psd->key_status & MY_KEY_ ## X ##_BIT)) { \
            psd->key_status |= MY_KEY_ ## X ##_BIT; \
            puts(#X " key pressed"); \
        } \
    } while(0)

static gboolean keypress_handler(guint keyval, gpointer data)
{
    simple_image_displayer_t* const psd = data;
    if (psd == NULL) return FALSE;

    switch(keyval) {
    case GDK_KEY_Up:
        do_key(UP);
        return TRUE;

    case GDK_KEY_Down:
        do_key(DOWN);
        return TRUE;

    case GDK_KEY_Right:
        do_key(RIGHT);
        return TRUE;

    case GDK_KEY_Left:
        do_key(LEFT);
        return TRUE;

    case 'A':
    case 'a':
        do_key(A);
        return TRUE;
    }

    return ds_simple_key_handler(keyval, data);
}
#undef do_key

// ======================================================================
#define do_key(X) \
    do { \
        if (psd->key_status & MY_KEY_ ## X ##_BIT) { \
          psd->key_status &= (unsigned char) ~MY_KEY_ ## X ##_BIT; \
            puts(#X " key released"); \
        } \
    } while(0)

static gboolean keyrelease_handler(guint keyval, gpointer data)
{
    simple_image_displayer_t* const psd = data;
    if (psd == NULL) return FALSE;

    switch(keyval) {
    case GDK_KEY_Up:
        do_key(UP);
        return TRUE;

    case GDK_KEY_Down:
        do_key(DOWN);
        return TRUE;

    case GDK_KEY_Right:
        do_key(RIGHT);
        return TRUE;

    case GDK_KEY_Left:
        do_key(LEFT);
        return TRUE;

    case 'A':
    case 'a':
        do_key(A);
        return TRUE;
    }

    return FALSE;
}
#undef do_key

// ======================================================================
int main(int argc, char *argv[])
{
    if (argc <= 1){
        error("please provide an input file (binary image)");
        return 1;
    }

    memset(&gameboy, 0, sizeof(gameboy_t));

    // M_EXIT_IF_ERR(gameboy_create(&gameboy, argv[1]));   
    M_EXIT_IF_ERR(gameboy_create(&gameboy, argv[1]));

    int x = 0;
    sd_launch(&argc, &argv,
                sd_init(argv[1], (int) LCD_WIDTH * SCALE, (int) LCD_HEIGHT * SCALE, 40,
                        generate_image, keypress_handler, keyrelease_handler));

    x= x*3;
    gameboy_free(&gameboy);
    x = 1+4;
    return 0;
}
