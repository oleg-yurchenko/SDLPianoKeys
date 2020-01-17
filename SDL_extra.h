#ifndef SDL_EXTRA
#define SDL_EXTRA
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include<string>

class LTexture {
    public:
        //Initialize variables
        LTexture();

        //Deallocates memory
        ~LTexture();

        //Loads image at specified path
        bool loadFromFile(std::string path, Uint8 r, Uint8 g, Uint8 b, SDL_Renderer* gRender);
    
        //Creates image from font string
        bool loadFromRenderedText(SDL_Renderer* gRenderer, TTF_Font* gFont, std::string textureText, SDL_Color textColor, SDL_Color bgColor = {255,255,255});

        //Deallocates texture
        void free();

        //Set color modulation
        void setColor(Uint8 red, Uint8 green, Uint8 blue);

        //Set blend mode
        void setBlendMode(SDL_BlendMode blending);

        //Set alpha modulation
        void setAlpha(Uint8 alpha);

        //Renders texture at given point
        void render(SDL_Renderer* gRenderer, int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

        //Gets image dimensions
        int getWidth();
        int getHeight();
    
    private:
        //actual hardware texture
        SDL_Texture* mTexture;

        //Image dimensions
        int mWidth;
        int mHeight;
};



//The mouse button
class LButton {
    public:
        //Button Constants
        const int BUTTON_WIDTH = 300;
        const int BUTTON_HEIGHT = 200;
        const int TOTAL_BUTTONS = 4;

        enum LButtonSprite {
            BUTTON_SPRITE_MOUSE_OUT = 0,
            BUTTON_SPRITE_MOUSE_OVER_MOTION = 1,
            BUTTON_SPRITE_MOUSE_DOWN = 2,
            BUTTON_SPRITE_MOUSE_UP = 3,
            BUTTON_SPRITE_TOTAL = 4
        };

        //Initializes internal variables
        LButton();

        //Sets top left position
        void setPosition(int x, int y);

        //Handles mouse event
        void handleEvent(SDL_Event* event);

        //Get's sprite
        int getCurrentSprite();
        /*See SDL_extra.cpp
        //Show button sprite
        void render(SDL_Renderer* gRenderer, LTexture spriteSheet, SDL_Rect gSpriteClips[]);
        */
    
    private:
        //Top left position
        SDL_Point mPosition;

        //Currently used global sprite
        LButtonSprite mCurrentSprite;
};
#endif