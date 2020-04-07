#include <sys/io.h>
#include <gfx/video.h>

//static int x = 0;
static int y_player1 = 384;
static int y_player2 = 384;

static int button_w = 0;
static int button_s = 0;

static int button_i = 0;
static int button_k = 0;

static int accel = 10;

static int ball_x = 30;
static int ball_y = 30;
static int ball_dx = 10;
static int ball_dy = 10;
static int ball_accel = 20;

void isr0()
{
    if (button_w != 0 || button_s != 0)
    {
        erase_player1(y_player1);

        y_player1 += button_w * accel;
        y_player1 += button_s * accel;

        draw_player1(y_player1);
    }

    if (button_i != 0 || button_k != 0)
    {
        erase_player2(y_player2);

        y_player2 += button_i * accel;
        y_player2 += button_k * accel;

        draw_player2(y_player2);
    }

    // Controle para não apagar a linha central
    if (ball_x > 501 && ball_x < 518)
    {
        erase_ball(502, ball_y);
        erase_ball(517, ball_y);
    } else {
        erase_ball(ball_x, ball_y);
    }

    /* Calcula o eixo x da bola */
    ball_x += ball_dx;

    if (ball_x <= 30)
    {
        ball_x = 30;
        ball_dx += ball_accel;
    }

    if (ball_x >= 1023 - 30)
    {
        ball_x = 1023 - 30;
        ball_dx -= ball_accel;
    }
    
    /* Calcula o eixo y da bola */
    ball_y += ball_dy;

    if (ball_y <= 0)
    {
        ball_y = 30;
        ball_dy += ball_accel;
    }

    if (ball_y >= 758)
    {
        ball_y = 758;
        ball_dy -= ball_accel;
    }

    draw_ball(ball_x, ball_y);
}

void isr1(void)
{
    char keycode;

    keycode = inb(0x60);

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
    }
    else
    {
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
    draw_board();

    draw_player1(y_player1);
    draw_player2(y_player2);

    draw_ball(30, 80);

    while (1)
    {
    }

    return 0;
}
