#include <sys/io.h>
#include <gfx/video.h>
#include <sys/usart.h>

#define SERIAL_PORT 0x3F8

static int mode = 0; // 0 = cliente && 1 = servidor

static int y_player1 = 384;
static int y_player2 = 384;

static int button_w = 0;
static int button_s = 0;

static int button_i = 0;
static int button_k = 0;

static int accel = 10;

static int ball_x = 512;
static int ball_y = 30;
static int ball_dx = 10;
static int ball_dy = 10;
static int ball_accel = 20;

static int points_player1 = 0;
static int points_player2 = 0;

int musica[] = {};
int musica_idx = 0;

long int tones[] = {523,587,659,698,783,880,987};

static int count_sound = 1E6;

void reiniciaJogo(void)
{
    ball_x = 512;
    ball_y = 30;
    ball_dx = 10;
    ball_dy = 10;

    exibePontuacao();
}

void exibePontuacao(void)
{
    int inicio_1 = 200;
    int inicio_2 = 700;
    int i;

    for (i = 0; i < points_player1; i++)
    {
        inicio_1 += 5;
        draw_square(inicio_1, 30, 2, 20, 0xFFFFFF);
    }

    for (i = 0; i < points_player2; i++)
    {
        inicio_2 += 5;
        draw_square(inicio_2, 30, 2, 20, 0xFFFFFF);
    }
}

void play_sound(long int freq)
{
    long int div;
    unsigned char tmp;

    div = 1103180 / freq;
    outb(0x43, 0xB6);
    outb(0x42, div);
    outb(0x42, div >> 8);

    tmp = inb(0x61);
    if ((tmp & 0x03) != 0x03)
    {
        outb(0x61, tmp | 0x03);
    }
}

void stop_sound(void)
{
    unsigned char tmp;
    tmp = inb(0x61);
    outb(0x61, tmp & 0xFC);
}

void isr0()
{
    //long int tone = tones[musica[musica_idx]];

    if (button_w != 0 || button_s != 0)
    {
        erase_player1(y_player1);

        y_player1 += button_w * accel;
        y_player1 += button_s * accel;

        // Controle player 1
        if (y_player1 > (668))
            y_player1 = 668;
        if (y_player1 < 0)
            y_player1 = 0;

        draw_player1(y_player1);
    }

    if (button_i != 0 || button_k != 0)
    {
        erase_player2(y_player2);

        y_player2 += button_i * accel;
        y_player2 += button_k * accel;

        // Controle player 2
        if (y_player2 > 668)
            y_player2 = 668;
        if (y_player2 < 0)
            y_player2 = 0;

        draw_player2(y_player2);
    }

    // Controle para não apagar a linha central
    if (ball_x > 501 && ball_x < 518)
    {
        erase_ball(502, ball_y);
        erase_ball(517, ball_y);
    } else {
        erase_ball(ball_x, ball_y)/* condition */;
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

    /* Calcula o eixo x da bola */
    ball_x += ball_dx;

    if (ball_x <= 30)
    {
        if (ball_x >= 20)
        {
            count_sound = 0;

            if ((ball_y >= y_player1) && (ball_y <= y_player1 + 100))
            {
                ball_x = 30;
                ball_dx += ball_accel;
            }
        }

        if (ball_x <= 0)
        {
            count_sound = 0;

            // Ponto para o jogador 2
            points_player2++;
            reiniciaJogo();
        }
    }

    if (ball_x >= 1023 - 30)
    {
        count_sound = 0;

        if (ball_x <= 1023 - 20)
        {
            if ((ball_y >= y_player2) && (ball_y <= y_player2 + 100))
            {
                ball_x = 1023 - 30;
                ball_dx -= ball_accel;
            }
        }

        if (ball_x >= 1023)
        {
            count_sound = 0; //zera contador para acionar o som

            // Ponto para o jogador 1
            points_player1++;
            reiniciaJogo();
        }
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

    if (mode)
    {
         usart_write(SERIAL_PORT,0xff);
         usart_write(SERIAL_PORT,0x00);
    }


}

void isr4(void)
{
   unsigned char dado = inb(SERIAL_PORT);

   if (dado == 0xFF)
   {

   }
}

int main(void)
{
    draw_board();

    draw_player1(y_player1);
    draw_player2(y_player2);

    usart_init(SERIAL_PORT);
    //usart_puts("abc");

    while (1)
    {

        if(count_sound < 1E4)
        {
            play_sound(tones[4]);
        }
        else
        {
            stop_sound();
        }

        count_sound++;
    }

    return 0;
}
