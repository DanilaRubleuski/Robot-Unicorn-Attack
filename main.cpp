#define _USE_MATH_DEFINES

#include<math.h>

#include<stdio.h>

#include<string.h>

#include <cstdlib>

#include <time.h>

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"

#include"./SDL2-2.0.10/include/SDL_main.h"

}
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 480
#define STANDART SCREEN_HEIGHT - 45
#define MAX_BACKGROUNDS 100
#define MAX_BLOCKS 100
#define MAX_PLATFORMS 100
#define MAX_STALAKS 100
#define G_platforms 0.07
#define G_backs 0.07
#define GRAVITY_PLATFORMS 0.007
#define GRAVITY_BACKS 0.007
#define MAX_HEART 4
#define MAX_FAIRIES 100
#define MOVING_AWAY_VALUE 10000000
#define FAIRY_POINTS 10
#define STAR_POINTS 100
#define MAX_DASH_TIME 500
#define NOT_DASH_COORDINATE_VALUE 300
#define DASH_COORDINATE_VALUE 600
#define BACK_ITERATION_VALUE 2000
#define ARROWS_Y 50
#define ARROWS_X 2000
#define START_PIXELS 6
#define START_SPEED -6
#define OBJECTS_ITERATION_LENGTH 600
#define FALL_DOWN_DEPTH -200
struct gameObject {
    double x;
    double y;
    double x_speed;
    double y_speed;
    int width;
    int heigth;
    SDL_Surface* image;
};
// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface* screen, int x, int y,
    const char* text,
    SDL_Surface* charset) {
    int px, py, c;
    SDL_Rect s, d;
    s.w = 8;
    s.h = 8;
    d.w = 8;
    d.h = 8;
    while (*text) {
        c = *text & 255;
        px = (c % 16) * 8;
        py = (c / 16) * 8;
        s.x = px;
        s.y = py;
        d.x = x;
        d.y = y;
        SDL_BlitSurface(charset, &s, screen, &d);
        x += 8;
        text++;
    };
};
// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y) {
    SDL_Rect dest;
    dest.x = x - sprite->w / 2;
    dest.y = y - sprite->h / 2;
    dest.w = sprite->w;
    dest.h = sprite->h;
    SDL_BlitSurface(sprite, NULL, screen, &dest);
};
// draw a single pixel
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
    int bpp = surface->format->BytesPerPixel;
    Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
    *(Uint32*)p = color;
};
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color) {
    for (int i = 0; i < l; i++) {
        DrawPixel(screen, x, y, color);
        x += dx;
        y += dy;
    };
};
// draw a rectangle of size l by k
void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k,
    Uint32 outlineColor, Uint32 fillColor) {
    int i;
    DrawLine(screen, x, y, k, 0, 1, outlineColor);
    DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
    DrawLine(screen, x, y, l, 1, 0, outlineColor);
    DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
    for (i = y + 1; i < y + k - 1; i++)
        DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
};
void jump(bool* isJumping, gameObject* objects, int length) { 
    for (int i = 0; i < length; i++) {
        if (*isJumping) {
            objects[i].y_speed += G_platforms; 
        }
    }
}
void jumpFall(bool* isJumping, double* Platform_point, double* y_speed) { 
    if (*isJumping) {
        *y_speed += G_backs; 
    }
}
void checkDashTime(int* dash_time1, bool* dash, int* dash_counter) { 
    int dash_time2 = SDL_GetTicks();
    if (dash_time2 - *dash_time1 > MAX_DASH_TIME || *dash_counter > 1) {
        *dash = false;
        *dash_time1 = 0; 
    }
}
int collision(gameObject object, gameObject player) {

    if (object.y - object.heigth / 2 < player.y + player.heigth / 2 &&
        object.x - object.width / 2 < player.x + player.width / 2 &&
        object.y + object.heigth / 2 > player.y - player.heigth / 2 &&
        object.x + object.width / 2 > player.x - player.width / 2) {
        return 1;
    }
    else {
        return 0;
    }
}
void drawFigure(gameObject* objects, int length, bool* dash, double* delta, bool* self_playing) {
    if (*self_playing == false) {
        for (int i = 0; i < length; i++) {
            
            objects[i].y -= NOT_DASH_COORDINATE_VALUE * objects[i].y_speed * (*delta);
            if (!*dash) {
                objects[i].x -= NOT_DASH_COORDINATE_VALUE * objects[i].x_speed * (*delta);
            }
            else {
                objects[i].x -= DASH_COORDINATE_VALUE * objects[i].x_speed * (*delta);
                objects[i].y_speed = 0;
            }
        }
    }
}
void drawBackground(gameObject* object, bool* dash, double* delta, bool* auto_playing) {
    if (*auto_playing == false) {
        object->y -= NOT_DASH_COORDINATE_VALUE * object->y_speed * (*delta);
        if (!*dash) {
            object->x -= NOT_DASH_COORDINATE_VALUE * object->x_speed * (*delta);
        }
        else {
            object->x -= DASH_COORDINATE_VALUE * object->x_speed * (*delta);
            object->y_speed = 0;
        }
    }

}
void arrowsUp(gameObject* objects, int length, double* delta) {
    for (int i = 0; i < length; i++) {
        objects[i].y += ARROWS_Y * objects[i].y_speed * (*delta);
        objects[i].y += START_PIXELS;
    }
}
void arrowsDown(gameObject* objects, int length, double* delta) {
    for (int i = 0; i < length; i++) {
        objects[i].y -= ARROWS_Y * objects[i].y_speed * (*delta);
    }
}
void arrowsLeft(gameObject* objects, int length, double* delta) {
    for (int i = 0; i < length; i++) {
        objects[i].x += ARROWS_X * objects[i].x_speed * (*delta);
    }
}
void arrowsRight(gameObject* objects, int length, double* delta) {
    for (int i = 0; i < length; i++) {
        objects[i].x -= ARROWS_X * objects[i].x_speed * (*delta);
    }
}
void cycleBackground(gameObject* object) {
    if (object->x < 0) {
        object->x = BACK_ITERATION_VALUE;
    }
}
void cycleObjects(gameObject* objects, int length) {
    gameObject object = objects[0];
    if (object.x + object.width < 0) {

        for (int i = 1; i < length; i++) {
            objects[i - 1] = objects[i];

        }
        object.x = objects[length - 1].x + OBJECTS_ITERATION_LENGTH;
        objects[length - 1] = object;

    }

}
void endGame(int* quit, int* games, bool* game_end, int* fairy_counter, int* star_counter) {
    *quit = 1;
    *game_end = true;
    *fairy_counter = 0;
    *star_counter = 0;
}
void fallout(gameObject* object, gameObject* player, bool* game_end, int* quit, int* games, int* fairy_counter, int* star_counter) {
    if (object->y <= FALL_DOWN_DEPTH) {
        endGame(quit, games, game_end, fairy_counter, star_counter);
        *game_end = true;
    }
}
void removeHeart(gameObject object[MAX_HEART], bool* game_end, SDL_Surface* screen,int* games) {
    if (*game_end) {
        (*games)--;
    }
}
void stopFalling(gameObject* object) {
    object->y_speed = 0;
}
void stayAlive(gameObject objects[MAX_PLATFORMS], gameObject objects2[MAX_PLATFORMS], bool* isPlatform, bool* isJumping, bool* isFalling,
    bool* Two, int* counter, gameObject* back) {
    for (int j = 0; j < MAX_PLATFORMS; j++) {
        *isPlatform = true;
        *isJumping = false;
        *isFalling = false;
        *Two = false;
        *counter = 0;
        stopFalling(&objects[j]);
        stopFalling(&objects2[j]);
        back->y_speed = 0;
    }
}
int stalaktiteCollision(gameObject object, gameObject player) {
    if (collision(object, player)) {
        if (object.y + object.heigth / 2 > player.y + player.heigth / 2 &&
            object.x - object.width / 2 < player.x - player.width / 2) 
        {
            return 1;
        }
        else {
            return -1;
        }
    }
    return 0;
}
void fallBackground(gameObject* object, bool* isPlatform, bool* isFalling) {
    *isPlatform = false;
    *isFalling = true;
    object->y_speed += GRAVITY_BACKS;
}
void drawPlatforms(gameObject* objects, int platforms_counter, SDL_Surface* screen) {
    for (int i = 0; i < platforms_counter; i++) 
    {
        DrawSurface(screen, objects[i].image,
            objects[i].x,
            objects[i].y);
    }
}
void drawStars(gameObject objects[MAX_BLOCKS], int stars_counter, SDL_Surface* screen) {
    for (int i = 0; i < stars_counter; i++) {
        DrawSurface(screen, objects[i].image,
            objects[i].x,
            objects[i].y);
    }
}
void drawStalaktites(gameObject objects[MAX_STALAKS], int stalaks_counter, SDL_Surface* screen)
{   
    for (int i = 0; i < stalaks_counter; i++) {
        DrawSurface(screen, objects[i].image,
            objects[i].x,
            objects[i].y);
    }
}
void drawFairies(gameObject objects[MAX_FAIRIES], int fairies_counter, SDL_Surface* screen) {
    for (int i = 0; i < fairies_counter; i++) {
        DrawSurface(screen, objects[i].image,
            objects[i].x,
            objects[i].y);
    }
}
void startSpeed(gameObject* objects, int length) 
{  
    for (int i = 0; i < length; i++) {
        objects[i].y_speed = START_SPEED;
        objects[i].y += START_PIXELS;
    }
}

#ifdef __cplusplus
extern "C"
#endif

int main(int argc, char** argv) {
    srand(time(0));

    int games = 4; 
    int t1, t2, quit, frames, rc;
    double delta, time1, worldTime, fpsTimer, fps, distance;
    SDL_Event event;
    SDL_Surface* screen, * charset;
    SDL_Surface* unicorn;
    SDL_Surface* star;
    SDL_Surface* heart;
    SDL_Surface* fairy;
    SDL_Surface* background;
    SDL_Surface* stalaktite;
    SDL_Surface* platform;
    SDL_Texture* scrtex;
    SDL_Window* window;
    SDL_Renderer* renderer;

    while (games) {
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
            printf("SDL_Init error: %s\n", SDL_GetError());
            return 1;
        }
        rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &
            window, &renderer);
        if (rc != 0) {
            SDL_Quit();

            printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
            return 1;
        };

        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
        SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        SDL_SetWindowTitle(window, "Robot Unicorn Attack");

        screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
            0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

        scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            SCREEN_WIDTH, SCREEN_HEIGHT);

        SDL_ShowCursor(SDL_DISABLE);

        charset = SDL_LoadBMP("./cs8x8.bmp");
        if (charset == NULL) {
            printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
            SDL_FreeSurface(screen);
            SDL_DestroyTexture(scrtex);
            SDL_DestroyWindow(window);
            SDL_DestroyRenderer(renderer);
            SDL_Quit();

            return 1;
        };
        SDL_SetColorKey(charset, true, 0x000000);
        unicorn = SDL_LoadBMP("./Unicorn3.bmp");
        if (unicorn == NULL) {
            printf("SDL_LoadBMP(Unicorn3.bmp) error: %s\n", SDL_GetError());
            SDL_FreeSurface(charset);
            SDL_FreeSurface(screen);
            SDL_DestroyTexture(scrtex);
            SDL_DestroyWindow(window);
            SDL_DestroyRenderer(renderer);
            SDL_Quit();

            return 1;
        };
        star = SDL_LoadBMP("./star.bmp"); 
        if (star == NULL) {
            printf("SDL_LoadBMP(star.bmp) error: %s\n", SDL_GetError());
            SDL_FreeSurface(charset);
            SDL_FreeSurface(screen);
            SDL_DestroyTexture(scrtex);
            SDL_DestroyWindow(window);
            SDL_DestroyRenderer(renderer);
            SDL_Quit();

            return 1;
        };
        background = SDL_LoadBMP("./back.bmp"); 
        if (background == NULL) {
            printf("SDL_LoadBMP(back.bmp) error: %s\n", SDL_GetError());
            SDL_FreeSurface(charset);
            SDL_FreeSurface(screen);
            SDL_DestroyTexture(scrtex);
            SDL_DestroyWindow(window);
            SDL_DestroyRenderer(renderer);
            SDL_Quit();

            return 1;
        };
        platform = SDL_LoadBMP("./Platform.bmp");
        if (platform == NULL) {
            printf("SDL_LoadBMP(Platform.bmp) error: %s\n", SDL_GetError());
            SDL_FreeSurface(charset);
            SDL_FreeSurface(screen);
            SDL_DestroyTexture(scrtex);
            SDL_DestroyWindow(window);
            SDL_DestroyRenderer(renderer);
            SDL_Quit();

            return 1;
        };
        stalaktite = SDL_LoadBMP("./stalak.bmp");
        if (stalaktite == NULL) {
            printf("SDL_LoadBMP(stalak.bmp) error: %s\n", SDL_GetError());
            SDL_FreeSurface(charset);
            SDL_FreeSurface(screen);
            SDL_DestroyTexture(scrtex);
            SDL_DestroyWindow(window);
            SDL_DestroyRenderer(renderer);
            SDL_Quit();

            return 1;
        };
        heart = SDL_LoadBMP("./heart.bmp");
        if (heart == NULL) {
            printf("SDL_LoadBMP(heart.bmp) error: %s\n", SDL_GetError());
            SDL_FreeSurface(charset);
            SDL_FreeSurface(screen);
            SDL_DestroyTexture(scrtex);
            SDL_DestroyWindow(window);
            SDL_DestroyRenderer(renderer);
            SDL_Quit();

            return 1;
        };
        fairy = SDL_LoadBMP("./fairy.bmp");
        if (fairy == NULL) {
            printf("SDL_LoadBMP(fairy.bmp) error: %s\n", SDL_GetError());
            SDL_FreeSurface(charset);
            SDL_FreeSurface(screen);
            SDL_DestroyTexture(scrtex);
            SDL_DestroyWindow(window);
            SDL_DestroyRenderer(renderer);
            SDL_Quit();

            return 1;
        };
        char text[128];
        int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
        int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
        int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
        int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);

        t1 = SDL_GetTicks();

        bool isJumping = false;
        bool isFalling = false;

        int star_flag = 0;
        int star_counter = 0;
        int fairy_flag = 0;
        int fairy_counter = 0;

        int jump_counter = 0;
        bool Two = false; 
        bool self_playing = false;
        bool dash = false;
        int dash_time1; 
        int dash_counter = 0;

        int platforms_count = 0;
        int stalaks_count = 0;
        int stars_count = 0;
        int fairies_count = 0;

        frames = 0;
        fpsTimer = 0;
        fps = 0;
        quit = 0;
        worldTime = 0;
        distance = 0;

        bool isPlatform = false;
        bool game_end = false;

        gameObject platforms[MAX_PLATFORMS*10] = {};
        gameObject stalaktites[MAX_STALAKS] = {};
        gameObject stars[MAX_PLATFORMS] = {};
        gameObject fairies[MAX_PLATFORMS] = {};
        for (int i = 0; i < MAX_PLATFORMS; i++) {

            int random_value = rand() % 100;

            gameObject Platform;
            Platform.image = platform;
            Platform.width = 700;
            Platform.heigth = 60;
            Platform.x = i * 900 + Platform.width / 2;
            Platform.y = SCREEN_HEIGHT - Platform.heigth;
            Platform.x_speed = 1;
            Platform.y_speed = 0;
            platforms[platforms_count] = Platform;
            platforms_count++;

            gameObject Platform2;
            Platform2.image = platform;
            Platform2.width = 700;
            Platform2.heigth = 60;
            Platform2.x = i * 900 + Platform2.width / 4;
            Platform2.y = SCREEN_HEIGHT - Platform2.heigth - 600;
            Platform2.x_speed = 1;
            Platform2.y_speed = 0;
            platforms[platforms_count] = Platform2;
            platforms_count++;

            if (random_value > 61 && random_value<100)
            {
                gameObject Star;
                Star.image = star;
                Star.width = 60;
                Star.heigth = 60;
                Star.x = (2 * i) * 900 + Platform.width - 5 * Star.width;
                Star.y = SCREEN_HEIGHT - Platform.heigth - Star.heigth;
                Star.x_speed = 1;
                Star.y_speed = 0;
                stars[stars_count] = Star;
                stars_count++;
            }

            if (random_value > 31 && random_value <60)
            {   
                gameObject Fairy;
                Fairy.image = fairy;
                Fairy.width = 60;
                Fairy.heigth = 60;
                Fairy.x = (2 * i + 1) * 900 + Platform.width - 5 * Fairy.width;
                Fairy.y = SCREEN_HEIGHT - Platform.heigth - Fairy.heigth;
                Fairy.x_speed = 1;
                Fairy.y_speed = 0;

                fairies[fairies_count] = Fairy;
                fairies_count++;
            }

            if (random_value > 0 && random_value < 30) 
            {  
                gameObject Stalaktite;
                Stalaktite.image = stalaktite;
                Stalaktite.width = 200;
                Stalaktite.heigth = 60;
                Stalaktite.x = (3 * i + 1) * 900 + Platform.width - Stalaktite.width / 2;
                Stalaktite.y = SCREEN_HEIGHT - Platform.heigth - Stalaktite.heigth;
                Stalaktite.x_speed = 1;
                Stalaktite.y_speed = 0;
                stalaktites[stalaks_count] = Stalaktite;

                stalaks_count++;
            }
        }
        //declairing the hearts object, and all of the hearts parametres
        gameObject hearts[MAX_HEART] = {};
        for (int i = 0; i < MAX_HEART; i++) {
            gameObject Heart;
            Heart.image = heart;
            Heart.width = 60;
            Heart.heigth = 60;
            Heart.x = i * Heart.width + Heart.width / 2;
            Heart.x_speed = 0;
            Heart.y_speed = 0;
            Heart.y = 30;
            hearts[i] = Heart;
        }
        //declairing the unicorn object, and all of the unicorn parametres
        gameObject Unicorn;
        Unicorn.image = unicorn;
        Unicorn.x = 45;
        Unicorn.x_speed = 0;
        Unicorn.width = 90;
        Unicorn.heigth = 90;
        Unicorn.y_speed = 0;
        Unicorn.y = SCREEN_HEIGHT - 300;

        //declairing the background object, and all of the background parametres
        gameObject back;
        back.image = background;
        back.x = 2000;
        back.y = 480;
        back.x_speed = 1;
        back.y_speed = 0;

        while (!quit) {
            
            checkDashTime(&dash_time1, &dash, &dash_counter);
            t2 = SDL_GetTicks();
            delta = (t2 - t1) * 0.001;
            t1 = t2;
            worldTime += delta;

            distance += back.x_speed * delta;

            SDL_FillRect(screen, NULL, czarny);
            drawBackground(&back, &dash, &delta, &self_playing);
            if (dash)
            {
                for (int i = 0; i < MAX_PLATFORMS; i++) {

                    stopFalling(&stalaktites[i]);
                    stopFalling(&stars[i]);
                    stopFalling(&fairies[i]);
                }
                for (int i = 0; i < platforms_count; i++) {

                    stopFalling(&platforms[i]);
                }
            }
            DrawSurface(screen, back.image,
                back.x,
                back.y);

            cycleBackground(&back);

            cycleObjects(platforms, platforms_count);
            cycleObjects(stars, stars_count);
            cycleObjects(stalaktites, stalaks_count);
            cycleObjects(fairies, fairies_count);

            if (!isPlatform && isFalling) {
                fallBackground(&back, &isPlatform, &isFalling);
            }
            jumpFall(&isJumping, &back.y, &back.y_speed);

            DrawSurface(screen, Unicorn.image,
                Unicorn.x,
                Unicorn.y);

            for (int i = 0; i < games; i++) {
                DrawSurface(screen, hearts[i].image,
                    hearts[i].x,
                    hearts[i].y);
            }

            drawFigure(platforms, platforms_count, &dash, &delta, &self_playing);
            if (dash) {
                back.y_speed = 0;
            }
            drawPlatforms(platforms, platforms_count, screen);
            jump(&isJumping, platforms, platforms_count);

            drawFigure(stalaktites, stalaks_count, &dash, &delta, &self_playing);
            drawStalaktites(stalaktites, stalaks_count, screen);
            jump(&isJumping, stalaktites, stalaks_count);

            drawFigure(stars, stars_count, &dash, &delta, &self_playing);
            drawStars(stars, stars_count, screen);
            jump(&isJumping, stars, stars_count);

            drawFigure(fairies, fairies_count, &dash, &delta, &self_playing);
            drawFairies(fairies, fairies_count, screen);
            jump(&isJumping, fairies, fairies_count);

            int stalak_flag = 0;
            for (int i = 0; i < stalaks_count; i++) {
                if (stalaktiteCollision(stalaktites[i], Unicorn) == 1) 
                {   
                    for (int j = 0; j < stalaks_count; j++) {
                        isPlatform = true;
                        isJumping = false;
                        isFalling = false;
                        dash_counter = 0;
                        Two = false;
                        jump_counter = 0;
                        stopFalling(&fairies[j]);
                        stopFalling(&stalaktites[j]);
                        stopFalling(&stars[j]);
                        back.y_speed = 0;
                    }
                    for (int j = 0; j < platforms_count; j++) {
                        isPlatform = true;
                        isJumping = false;
                        isFalling = false;
                        dash_counter = 0;
                        Two = false;
                        jump_counter = 0;
                        stopFalling(&platforms[j]);
                        back.y_speed = 0;
                    }
                    stalak_flag = 1;
                    break;
                }
                else if (stalaktiteCollision(stalaktites[i], Unicorn) == -1) 
                {   
                    endGame(&quit, &games, &game_end, &fairy_counter, &star_counter);
                    break;
                }
                else {
                    
                    isPlatform = false;
                    isFalling = true;
                }

            }
            if (!stalak_flag) {
                
                for (int i = 0; i < platforms_count; i++) {
                    if (stalaktiteCollision(platforms[i], Unicorn) == 1) 
                    {   
                        for (int j = 0; j < stalaks_count; j++) {
                            isPlatform = true;
                            isJumping = false;
                            isFalling = false;
                            dash_counter = 0;
                            Two = false;
                            jump_counter = 0;
                            stopFalling(&stalaktites[j]);
                            stopFalling(&stars[j]);
                            stopFalling(&fairies[j]);
                            back.y_speed = 0;
                        }
                        for (int j = 0; j < platforms_count; j++) {
                            isPlatform = true;
                            isJumping = false;
                            isFalling = false;
                            dash_counter = 0;
                            Two = false;
                            jump_counter = 0;
                            stopFalling(&platforms[j]);
                            back.y_speed = 0;
                        }
                        break;
                    }
                    else if (stalaktiteCollision(platforms[i], Unicorn) == -1) 
                    {   
                        endGame(&quit, &games, &game_end, &fairy_counter, &star_counter);
                        break;
                    }
                    else {
                        isPlatform = false;
                        isFalling = true;
                    }

                }
            }
            for (int i = 0; i < stars_count; i++) {
                if (collision(stars[i], Unicorn) == 1 && !dash)
                {
                    endGame(&quit, &games, &game_end, &fairy_counter, &star_counter);
                    star_flag = -1;
                    break;
                }
                else if (collision(stars[i], Unicorn) == 1 && dash && star_flag != 1) {
                    star_flag = 1;
                    star_counter++;//increasing the quantity of kolizied stars
                    distance += STAR_POINTS * star_counter;//add the collected points to entire amount of  points
                    stars[i].x = MOVING_AWAY_VALUE;//after kolizion move the star away
                    break;
                }
                else if (collision(stars[i], Unicorn) == 0 && stars[i].x < Unicorn.x && star_flag != 1 && stars[i].x > 0) 
                {   //if the player left one of the stars
                    for (int i = 2; i <= star_counter; i++) {
                        distance -= STAR_POINTS * i;//substract the points
                    }
                    star_counter = 0;//start the count over
                    break;
                }
                else if (!dash) {
                    star_flag = 0;//if there is no dash, and player is already left the star, make the flag 0
                }
            }
            //this cycle is responsible for kolizion  of Fairies with Unicorn
            for (int i = 0; i < fairies_count; i++) {
                if (collision(fairies[i], Unicorn) == 1 && fairy_flag != 1) {
                    fairy_flag = 1;
                    fairy_counter++;//increasing the quantity of kolizied fairies
                    distance += FAIRY_POINTS * fairy_counter;//add the collected points to entire amount of  points
                    fairies[i].x = MOVING_AWAY_VALUE;//after kolizion move the fairy away
                    break;
                }
                else if (collision(fairies[i], Unicorn) == 0 && fairies[i].x < Unicorn.x && fairy_flag != 1 && fairies[i].x > 0)
                {//if the player left one of the fairies
                    for (int i = 2; i <= fairy_counter; i++) {
                        distance -= FAIRY_POINTS * i;//substract the points
                    }
                    fairy_counter = 0;//start the count over
                    break;
                }
                else {
                    fairy_flag = 0;//if player is already left the star, make the flag 0
                }
            }
            //this condition is about Unicorn falling down
            if (isFalling == true && isPlatform == false) {
               //if the player felt down off the platform or stalaktite
                isJumping = true;
                for (int i = 0; i < MAX_PLATFORMS; i++) {
                    //adding the gravity for stalaktites, stars and fairies
                    stalaktites[i].y_speed += GRAVITY_PLATFORMS;
                    stars[i].y_speed += GRAVITY_PLATFORMS;
                    fairies[i].y_speed += GRAVITY_PLATFORMS;
                }
                for (int j = 0; j < platforms_count; j++) {
                    //adding the gravity for platforms
                    platforms[j].y_speed += GRAVITY_PLATFORMS;
                }
            }
            // checking for player felt under the level
            fallout(&back, &Unicorn, &game_end, &quit, &games, &fairy_counter, &star_counter);

            fpsTimer += delta;
            if (fpsTimer > 0.5) {
                fps = frames * 2;
                frames = 0;
                fpsTimer -= 0.5;
            };
            // tekst informacyjny / info text
            DrawRectangle(screen, 245, 4, 510, 36, czerwony, niebieski);
            sprintf(text, "Czas trwania = %.1lf s  %.0lf klatek / s", worldTime, fps);
            DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);
            //Buttons
            sprintf(text, "Esc - wyjscie, n - restart, z - jump, x - zryw");
            DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 26, text, charset);
            //Points amount
            DrawRectangle(screen, SCREEN_WIDTH - 570, 50, 140, 30, czerwony, niebieski);
            sprintf(text, "Points= %.0lf", distance);
            DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 60, text, charset);

            SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
            SDL_RenderCopy(renderer, scrtex, NULL, NULL);
            SDL_RenderPresent(renderer);

            removeHeart(hearts, &game_end, screen, &games);//removeing players heart points if he dies

            // obs³uga zdarzeñ (o ile jakieœ zasz³y) / handling of events (if there were any)
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        quit = 1;
                        games = 0;//making heart points(quantity of games) equal 0
                    }
                    else if (event.key.keysym.sym == SDLK_RIGHT && self_playing == true) {
                        //moving all of the objects right by the right arrow
                        arrowsRight(platforms, platforms_count, &delta);
                        arrowsRight(stars, stars_count, &delta);
                        arrowsRight(stalaktites, stalaks_count, &delta);
                        arrowsRight(fairies, fairies_count, &delta);
                        back.x -= 2000 * back.x_speed * delta;
                    }
                    else if (event.key.keysym.sym == SDLK_LEFT && self_playing == true) {
                        //moving all of the objects left by the left arrow
                        arrowsLeft(platforms, platforms_count, &delta);
                        arrowsLeft(stars, stars_count, &delta);
                        arrowsLeft(stalaktites, stalaks_count, &delta);
                        arrowsLeft(fairies, fairies_count, &delta);
                        back.x += 2000 * back.x_speed * delta;
                    }
                    else if (event.key.keysym.sym == SDLK_UP && self_playing == true) {
                        //moving all of the objects up by the up arrow
                        arrowsUp(platforms, platforms_count, &delta);
                        arrowsUp(stars, stars_count, &delta);
                        arrowsUp(stalaktites, stalaks_count, &delta);
                        arrowsUp(fairies, fairies_count, &delta);
                        back.y += 50 * back.y_speed * delta;
                    }
                    else if (event.key.keysym.sym == SDLK_DOWN && self_playing == true) {
                        //moving all of the objects down by the down arrow
                        arrowsDown(platforms, platforms_count, &delta);
                        arrowsDown(stars, stars_count, &delta);
                        arrowsDown(stalaktites, stalaks_count, &delta);
                        arrowsDown(fairies, fairies_count, &delta);
                        back.y -= 50 * back.y_speed * delta;
                    }
                    else if (event.key.keysym.sym == SDLK_n) {
                        //starting the game over by clicking N
                        quit = 1;
                        games = MAX_HEART;
                        fairy_counter = 0;
                        star_counter = 0;
                    }
                    else if (event.key.keysym.sym == SDLK_z && self_playing == false) {
                        //start the jump if the z key is clicked
                        self_playing = false;
                        if (!Two) {
                            //if it is not doublejump
                            isJumping = 1;
                            isFalling = false;
                            startSpeed(platforms, platforms_count);
                            startSpeed(stalaktites, stalaks_count);
                            startSpeed(stars, stars_count);
                            startSpeed(fairies, fairies_count);
                            back.y_speed = -6;
                           //giving all of the objects start jump speed 
                        }
                        jump_counter++;// increasing the times, that user pressed z, to count the double jump
                        if (jump_counter > 1) {
                            Two = true;
                        }
                    }
                    else if (event.key.keysym.sym == SDLK_x && dash == false) {
                        //counting the times, that user pressed x, to make the right dash(not infinity dash)
                        dash_counter++;
                        dash = true;
                        dash_time1 = SDL_GetTicks();
                    }
                    else if (event.key.keysym.sym == SDLK_d) {
                        //if it is self_playing mode
                        if (self_playing == true) {
                            self_playing = false;
                        }
                        else {
                            self_playing = true;
                        }
                    }
                    break;
                case SDL_QUIT:
                    quit = 1;
                    games--;
                    break;
                case SDLK_n:
                    quit = 1;
                    games = 4;
                    break;
                case SDLK_z:
                    break;

                };
            };
            frames++;

        };
        SDL_FreeSurface(charset);
        SDL_FreeSurface(screen);
        SDL_DestroyTexture(scrtex);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

        SDL_Quit();
    };
    return 0;
}