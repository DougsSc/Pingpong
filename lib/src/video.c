#include <gfx/video.h>

static unsigned long * const vidptr = (unsigned long *)0xFD000000;

void draw_board()
{
    draw_square(0, 0, 1024, 768, 0x000000); //desenha fundo da tela
    
    int y = 0;
    
    while(y <= 1024)
    {

        draw_square(512, y, 5, 5, 0xFFFFFF); //desenha linha central

        y++;
    }
    
}

void draw_ball(int x, int y)
{
    draw_square(x, y, 10, 10, 0xFFFFFF);
}

void draw_player1(int y)
{
    draw_square(20, y, 10, 100, 0xFFFFFF);
}

void draw_player2(int y)
{
    draw_square(1000, y, 10, 100, 0xFFFFFF);
}

void erase_player1(int y)
{
    draw_square(20, y, 10, 100, 0x000000);
}

void erase_player2(int y)
{
    draw_square(1000, y, 10, 100, 0x000000);
}

void draw_square(int x, int y, int w, int h, int c)
{
    long int i, j;

    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++)
            vidptr[y * 1024 + x + i * 1024 + j] = c;
    }
}

void clear_img(int x, int y, int c, void *image)
{
    long int i = 0, j = 0;

    struct image *img = (struct image *) image;

    int w = img->width;
    int h = img->height;
    int bpp = img->bytes_per_pixel;
    int alpha = 3;
    unsigned char *data = img->pixel_data;

    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            if (data[i * w * bpp + j * bpp + alpha])
                vidptr[y * 1024 + i * 1024 + x + j] = c;
        }
    }
}

void draw_img(int x, int y, void *image)
{
    long int i = 0, j = 0;

    struct image *img = (struct image *) image;

    int w = img->width;
    int h = img->height;
    int bpp = img->bytes_per_pixel;
    const unsigned char *data = img->pixel_data;

    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            if (data[i * w * bpp + j * bpp + bpp]) {
                vidptr[y * 1024 + i * 1024 + x + j] =
                    data[i * w * bpp + j * bpp + 0] << 16 |
                    data[i * w * bpp + j * bpp + 1] << 8 |
                    data[i * w * bpp + j * bpp + 2];
            }
        }
    }
}

