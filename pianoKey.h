#ifndef PIANO_KEY
#define PIANO_KEY
#include<vector>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include<SDL2/SDL_mixer.h>
#include<thread>
#include"SDL_extra.h"

class PianoKey {
    public:
        PianoKey();
        PianoKey(int typeKey, int note, SDL_Renderer* gRenderer);
        ~PianoKey();

        LTexture getLTexture();
        void renderLTexture(SDL_Renderer* gRenderer);
        void setPosition(int x, int y);
        SDL_Point getPosition();
        void playNote(Mix_Chunk* note);
        void handleEvent(SDL_Event* event, Mix_Chunk* note, SDL_Point** points = NULL);
        std::vector<SDL_Point*> detectCollision(SDL_Point** points);
        bool handleKeyDown(SDL_Event* event);

    private:
        LTexture mTexture;
        int KEY_WIDTH;
        int KEY_HEIGHT;
        int xPos;
        int yPos;
        char type;
        SDL_Color mColor;
};

#endif