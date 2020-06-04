#include "sidlib.h"
#include "gameboy.h"
#include "lcdc.h"
#include "error.h"

#include <stdint.h>
#include <sys/time.h>

// Key press bits
#define MY_KEY_UP_BIT    0x01
#define MY_KEY_DOWN_BIT  0x02
#define MY_KEY_RIGHT_BIT 0x04
#define MY_KEY_LEFT_BIT  0x08
#define MY_KEY_A_BIT     0x10
#define MY_KEY_B_BIT     0x20
#define MY_KEY_SELECT_BIT   0x40
#define MY_KEY_START_BIT 0x80

#define SCALE 3

// typedef struct gameboy_ gameboy_t;
gameboy_t gameboy;
struct timeval start;
struct timeval paused;

 uint64_t get_time_in_GB_cyles_since(struct timeval* from){
    if (from == NULL){
        return 0;
    } 
    
    struct timeval current={0,0};
    gettimeofday(&current, NULL);
    struct timeval delta= {0,0};
    if (!timercmp(from, &current,>)){
        timersub(&current, from, &delta);
        return delta.tv_sec * GB_CYCLES_PER_S +  (delta.tv_usec * GB_CYCLES_PER_S) / 1000000;
    }

    fprintf(stderr, "current time (sec=%zu, usec=%zu) was not strctly greater than parameter from (sec=%zu, usec=%zu)\n",
    current.tv_sec, current.tv_usec, from->tv_sec, from->tv_usec);
    return 0;
 }  

// ======================================================================
static void set_grey(guchar* pixels, int row, int col, int width, guchar grey)
{
    const size_t i = (size_t) (3 * (row * width + col)); // 3 = RGB
    pixels[i+2] = pixels[i+1] = pixels[i] = grey;
}

// ======================================================================
static void generate_image(guchar* pixels, int height, int width)
{
    
    // gameboy_run_until(&gameboy, get_time_in_GB_cyles_since(&start));
    gameboy_run_until(&gameboy, 5000000);
    gameboy_t g = gameboy;

    for (int x = 0; x < width; ++x){
        for (int y = 0; y < height; ++y){
            uint8_t output = 0;
            image_get_pixel(&output, &(gameboy.screen.display), x/SCALE, y/SCALE);
            set_grey(pixels, y, x, width, 255 - 85 * output);
        }
    }
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
        joypad_key_pressed(&gameboy.pad, UP_KEY);
        return TRUE;

    case GDK_KEY_Down:
        do_key(DOWN);
        joypad_key_pressed(&gameboy.pad, DOWN_KEY);
        return TRUE;

    case GDK_KEY_Right:
        do_key(RIGHT);
        joypad_key_pressed(&gameboy.pad, RIGHT_KEY);
        return TRUE;

    case GDK_KEY_Left:
        do_key(LEFT);
        joypad_key_pressed(&gameboy.pad, LEFT_KEY);
        return TRUE;

    case 'A':
    case 'a':
        do_key(A);
        joypad_key_pressed(&gameboy.pad, A_KEY);
        return TRUE;
    case 'Z':
    case 'z':
        do_key(B);
        joypad_key_pressed(&gameboy.pad, B_KEY);
        return TRUE;
    case 'P':
    case 'p':
        do_key(SELECT);
        joypad_key_pressed(&gameboy.pad, SELECT_KEY);
        return TRUE;
    case 'L':
    case 'l':
        do_key(START);
        joypad_key_pressed(&gameboy.pad, START_KEY);
        return TRUE;
    case GDK_KEY_space:
        if (psd->timeout_id >0){
            gettimeofday(&paused, NULL);
        }else{
            struct timeval current;
            gettimeofday(&current, NULL);
            timersub(&current, &paused, &paused);
            timeradd(&start, &paused, &start);
            timerclear(&paused);
        }
        return ds_simple_key_handler(keyval, data);
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
        joypad_key_released(&gameboy.pad, UP_KEY);
        return TRUE;

    case GDK_KEY_Down:
        do_key(DOWN);
        joypad_key_released(&gameboy.pad, DOWN_KEY);
        return TRUE;

    case GDK_KEY_Right:
        do_key(RIGHT);
        joypad_key_released(&gameboy.pad, RIGHT_KEY);
        return TRUE;

    case GDK_KEY_Left:
        do_key(LEFT);
        joypad_key_released(&gameboy.pad, LEFT_KEY);
        return TRUE;

    case 'A':
    case 'a':
        do_key(A);
        joypad_key_released(&gameboy.pad, A_KEY);
        return TRUE;
    case 'Z':
    case 'z':
        do_key(B);
        joypad_key_released(&gameboy.pad, B_KEY);
        return TRUE;
    case 'P':
    case 'p':
        do_key(SELECT);
        joypad_key_released(&gameboy.pad, SELECT_KEY);
        return TRUE;
    case 'L':
    case 'l':
        do_key(START);
        joypad_key_released(&gameboy.pad, START_KEY);
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
    gettimeofday(&start, NULL);
    timerclear(&paused);
    // M_EXIT_IF_ERR(gameboy_create(&gameboy, argv[1]));   
    M_EXIT_IF_ERR(gameboy_create(&gameboy, argv[1]));

    sd_launch(&argc, &argv,
                sd_init(argv[1], (int) LCD_WIDTH * SCALE, (int) LCD_HEIGHT * SCALE, 40,
                        generate_image, keypress_handler, keyrelease_handler));


    gameboy_free(&gameboy);

    return 0;
}
