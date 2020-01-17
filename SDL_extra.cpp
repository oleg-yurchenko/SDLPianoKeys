//Header files
#include<stdio.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include<string>
#include"SDL_extra.h"

/*------------------------------------------Ltexture Class--------------------------------*/

//Texture Wrapper Class
LTexture::LTexture() {
    //Initialize
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

LTexture::~LTexture() {
    //Deallocate
    free();
}

bool LTexture::loadFromFile(std::string path, Uint8 r, Uint8 g, Uint8 b, SDL_Renderer* gRender) {
    //Get rid of preexisting textures
    free();

    //The final texture
    SDL_Texture* newTexture = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if(loadedSurface == NULL){
        printf("Unable to load image %s! SDL_image error: %s\n", path.c_str(), IMG_GetError());
    } else {
        //color key image
        SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, r, g, b));

        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(gRender, loadedSurface);
        if(newTexture == NULL) {
            printf("Unable to create texture from %s! SDL_ERROR: %s\n", path.c_str(), SDL_GetError());
        } else {
            //Get image dimensions
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }
        //Get rid of old loaded surface
        SDL_FreeSurface(loadedSurface);
    }

    //return success
    mTexture = newTexture;
    return mTexture != NULL;
}

bool LTexture::loadFromRenderedText(SDL_Renderer* gRenderer, TTF_Font* gFont, std::string textureText, SDL_Color textColor, SDL_Color bgColor){
    //Get rid of pre-existing textures
    free();

    SDL_Surface* textSurface {NULL};

    //Render textures for different scenarios
    if(bgColor.r != 255 && bgColor.g != 255 && bgColor.b != 255){
        textSurface = TTF_RenderText_Shaded(gFont, textureText.c_str(), textColor, bgColor);
    } else {
        if(textColor.a != 255){
            textSurface = TTF_RenderText_Blended(gFont, textureText.c_str(), textColor);
        }else{
            textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
        }
    }

    if(textSurface == NULL){
        printf("Unable to render text surface! SDL_TTF error: %s\n", TTF_GetError());
    } else {
        //Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
        if(mTexture == NULL){
            printf("Unable to create texture from rendered text! SDL_ERROR: %s\n", SDL_GetError());
        } else {
            //Get image dimensions
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }
        //get rid of old surfaces
        SDL_FreeSurface(textSurface);
    }

    return mTexture != NULL;
}

void LTexture::free() {
    //Free texture if it exists
    if(mTexture != NULL) {
        SDL_DestroyTexture(mTexture);
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue) {
    //Modulate Texture
    SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending) {\
    //Set blending function
    SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha) {
    //Modulate texture alpha
    SDL_SetTextureAlphaMod(mTexture, alpha);
}


void LTexture::render(SDL_Renderer* gRenderer, int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip){
    //Set rendering space and render to screen
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};

    //Set clip rendering dimensions
    if(clip != NULL){
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    //Render to screen
    SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getHeight() {
    return mHeight;
}

int LTexture::getWidth() {
    return mWidth;
}

/*-----------------------------------LButton Class-------------------------------*/
LButton::LButton(){
    mPosition.x = 0;
    mPosition.y = 0;

    mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
}

void LButton::setPosition(int x, int y){
    mPosition.x = x;
    mPosition.y = y;
}

void LButton::handleEvent(SDL_Event* event){
    //if mouse event happened
    if(event->type == SDL_MOUSEMOTION || event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEBUTTONUP){
        //get mouse position
        int x, y;
        SDL_GetMouseState(&x, &y);

        //Checks if mouse is inside button
        bool inside = true;

        //Mouse is left of the button
        if(x < mPosition.x){
            inside = false;
        }
        
        //Mouse is right of the button
        if(x > mPosition.x+BUTTON_WIDTH){
            inside = false;
        }
        
        //mouse above the button
        if(y > mPosition.y+BUTTON_HEIGHT){
            inside = false;
        }
        
        //Mouse below the button
        if(y < mPosition.y){
            inside = false;
        }
        
        //mouse is outside the button
        if(!inside){
            mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
        }

        //Mouse is inside the button
        else{
            //Set mouse over sprite
            switch(event->type){
                case SDL_MOUSEMOTION:
                mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
                break;

                case SDL_MOUSEBUTTONDOWN:
                mCurrentSprite = BUTTON_SPRITE_MOUSE_DOWN;
                break;

                case SDL_MOUSEBUTTONUP:
                mCurrentSprite = BUTTON_SPRITE_MOUSE_UP;
                break;
            }
        }
    }
}

int LButton::getCurrentSprite(){
    return mCurrentSprite;
}

/* Doesn't work for some reason unless defined inside of the cpp main file
void LButton::render(SDL_Renderer* gRenderer, LTexture spriteSheet, SDL_Rect gSpriteClips[]){
    //Show current button sprite
    spriteSheet.render(gRenderer, mPosition.x, mPosition.y, &gSpriteClips[mCurrentSprite]);
}
*/ 