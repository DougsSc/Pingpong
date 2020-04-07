#include <sys/io.h>
#include <gfx/video.h>

//static int x = 0;
static int y_player1 = 384;
static int y_player2 = 384;

static int button_w = 0;
static int button_s = 0;

static int button_i = 0;
static int button_k = 0;

void isr1(void)
{
    char keycode;

    keycode = inb(0x60);

    //char c = keyboard_map[keycode];

    /*if (keycode >= 0) {
        draw_square(x, y, 10, 10, 0x665566);
        x += 10;
        draw_square(x, y, 10, 10, 0xFFFF00);
    }*/

    if (keycode & 0x80)
    {
        keycode &= ~0x80;
        /* Parou de precionar o W */
        if (keycode == 17)
        {
            button_w = 0;
        }

        /* Parou de precionar o S */
        if (keycode == 31)
        {
            button_s = 0;
        }

        /* Parou de precionar o I */
        if (keycode == 23)
        {
            button_i = 0;
        }

        /* Parou de precionar o K */
        if (keycode == 37)
        {
            button_k = 0;
        }        
        
    } else {
        /* Precionou o W */
        if (keycode == 17)
        {
            button_w = -1;
        }

        /* Precionou o S */
        if (keycode == 31)
        {
            button_s = 1;
        }

        /* Precionou o I */
        if (keycode == 23)
        {
            button_i = -1;
        }

        /* Precionou o K */
        if (keycode == 37)
        {
            button_k = 1;
        }

    }
}

int main(void)
{
    long int i = 0;

    draw_board();

    draw_player1(y_player1);
    draw_player2(y_player2);

    draw_ball(30, 80);

    while (1) {
        for (i = 0; i < 100000; i++);

        if (button_w != 0 || button_s != 0)
        {
            erase_player1(y_player1);
        
            y_player1 += button_w;
            y_player1 += button_s;

            draw_player1(y_player1);
        }

        if (button_i != 0 || button_k != 0)
        {
            erase_player2(y_player2);
        
            y_player2 += button_i;
            y_player2 += button_k;

            draw_player2(y_player2);
        }
        
    }

    return 0;
}

