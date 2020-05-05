#include <sys/io.h>
#include <gfx/video.h>
#include <sys/usart.h>

#define SERIAL_PORT 0x3F8

static int mode = 0; // 1 = cliente && 0 = servidor

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

static int last_ball_x = 512;
static int last_ball_y = 30;

static int points_player1 = 0;
static int points_player2 = 0;

volatile static int estado_rescepcao = 0;
volatile static int tmp_p1_posicao = 0;
volatile static int tmp_p1_pontuacao = 0;
volatile static int tmp_p2_posicao = 0;
volatile static int tmp_p2_pontuacao = 0;
volatile static int tmp_ball_x = 0;
volatile static int tmp_ball_y = 0;

static int tmp_y_player1 = 0;
static int tmp_y_player2 = 0;
static int tmp_points_player1 = 0;
static int tmp_points_player2 = 0;
static int last_tmp_ball_x = 0;
static int last_tmp_ball_y = 0;

int musica[] = {};
int musica_idx = 0;

long int tones[] = {523,587,659,698,783,880,987};

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
    if (!mode) {
        if (button_w != 0 || button_s != 0) {
            y_player1 += button_w * accel;
            y_player1 += button_s * accel;

            // Controle player 1
            if (y_player1 > (668))
                y_player1 = 668;
            if (y_player1 < 0)
                y_player1 = 0;
        }

        if (button_i != 0 || button_k != 0) {
            y_player2 += button_i * accel;
            y_player2 += button_k * accel;

            // Controle player 2
            if (y_player2 > 668)
                y_player2 = 668;
            if (y_player2 < 0)
                y_player2 = 0;
        }
    }

    if (!mode) {
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

        if (ball_x <= 30) {
            if (ball_x >= 20) {
                if ((ball_y >= y_player1) && (ball_y <= y_player1 + 100)) {
                    ball_x = 30;
                    ball_dx += ball_accel;
                }
            }

            if (ball_x <= 0) {
                // Ponto para o jogador 1
                points_player2++;
                reiniciaJogo();
            }
        }

        if (ball_x >= 1023 - 30) {
            if (ball_x <= 1023 - 20) {
                if ((ball_y >= y_player2) && (ball_y <= y_player2 + 100)) {
                    ball_x = 1023 - 30;
                    ball_dx -= ball_accel;
                }
            }

            if (ball_x >= 1023) {
                // Ponto para o jogador 1
                points_player1++;
                reiniciaJogo();
            }
        }
    } else {
        /* seta os dados */
        y_player1 = tmp_y_player1;
        y_player2 = tmp_y_player2;

        points_player1 = tmp_points_player1;
        points_player2 = tmp_points_player2;

        ball_x = last_tmp_ball_x;
        ball_y = last_tmp_ball_y;

        exibePontuacao();
    }

    erase_player1();
    erase_player2();
    // Controle para não apagar a linha central
    if (last_ball_x > 501 && last_ball_x < 518) {
        erase_ball(502, last_ball_y);
        erase_ball(517, last_ball_y);
    } else {
        erase_ball(last_ball_x, last_ball_y);
    }

    draw_player1(y_player1);
    draw_player2(y_player2);
    draw_ball(ball_x, ball_y);

    last_ball_x = ball_x;
    last_ball_y = ball_y;

    if (!mode) {
        unsigned char buff[] = {0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

        buff[1] = y_player1 >> 8;
        buff[2] = y_player1;
        buff[3] = points_player1;
        buff[4] = y_player2 >> 8;
        buff[5] = y_player2;
        buff[6] = points_player2;
        buff[7] = ball_x >> 8;
        buff[8] = ball_x;
        buff[9] = ball_y >> 8;
        buff[10] = ball_y;

        for (int i = 0; i < 11; i++) {
            usart_write(SERIAL_PORT, buff[i]);
        }
    }
}

void isr1(void)
{
    char keycode;
    unsigned char buff[] = {0xFF, 0x00};

    keycode = inb(0x60);

    if (keycode & 0x80)
    {
        keycode &= ~0x80;

        if (!mode)
        {
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
        } else {
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
    } else {

        if (!mode)
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
        } else {
            /* Precionou o I */
            if (keycode == 23) {
                buff[1] = 0x01;
            }

            /* Precionou o K */
            if (keycode == 37) {
                buff[1] = 0x02;
            }
        }

        if (keycode == 2) {
            mode = 1;
        }

    }

    if (mode)
    {
        usart_write(SERIAL_PORT, buff[0]);
        usart_write(SERIAL_PORT, buff[1]);
    }
}

void isr4(void)
{
   unsigned char dado = inb(SERIAL_PORT);

    if (estado_rescepcao == 0) {
        if (dado == 0xFF) {
            estado_rescepcao = 1;
        }
    } else {
        if (mode) {
            if (estado_rescepcao == 1) {
                tmp_p1_posicao = dado << 8;
            } else if (estado_rescepcao == 2) {
                tmp_p1_posicao |= dado;
            } else if (estado_rescepcao == 3) {
                tmp_p1_pontuacao = dado;
            } else if (estado_rescepcao == 4) {
                tmp_p2_posicao = dado << 8;
            } else if (estado_rescepcao == 5) {
                tmp_p2_posicao |= dado;
            } else if (estado_rescepcao == 6) {
                tmp_p2_pontuacao = dado;
            } else if (estado_rescepcao == 7) {
                tmp_ball_x = dado << 8;
            } else if (estado_rescepcao == 8) {
                tmp_ball_x |= dado;
            } else if (estado_rescepcao == 9) {
                tmp_ball_y = dado << 8;
            } else if (estado_rescepcao == 10) {
                tmp_ball_y |= dado;
            }

            if (estado_rescepcao < 10) {
                estado_rescepcao++;
            } else {
                estado_rescepcao = 0;

                /* seta os dados */
                tmp_y_player1 = tmp_p1_posicao;
                tmp_y_player2 = tmp_p2_posicao;

                tmp_points_player1 = tmp_p1_pontuacao;
                tmp_points_player2 = tmp_p2_pontuacao;

                last_tmp_ball_x = tmp_ball_x;
                last_tmp_ball_y = tmp_ball_y;
            }


        } else {
            if (estado_rescepcao == 1) {
                if (dado == 0x01)
                {
                    button_i = -1;
                } else if (dado == 0x02) {
                    button_k = 1;
                } else {
                    button_i = 0;
                    button_k = 0;
                }

                estado_rescepcao = 0;
            }
        }

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
    }

    return 0;
}
