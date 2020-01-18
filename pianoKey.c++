#include<vector>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include<SDL2/SDL_mixer.h>
#include<thread>
#include"SDL_extra.h"
#include"pianoKey.h"

PianoKey::PianoKey(){
    type = 'w';
    KEY_WIDTH = 0;
    KEY_HEIGHT = 0;
    xPos = 0;
    yPos = 0;
    SDL_Color mColor = {0xFF, 0xFF, 0xFF};
}

PianoKey::PianoKey(int typeKey, int note,SDL_Renderer* gRenderer) {
    mTexture.loadFromFile("key.png", 32, 56, 23, gRenderer);
    if(typeKey == 0){
        type = 'w';
        mColor = {0xFF, 0xFF, 0xFF};
        KEY_WIDTH = 150;
        KEY_HEIGHT = 500;
        mTexture.setColor(mColor.r, mColor.g, mColor.b);
    } else if(typeKey == 1){
        type = 'b';
        mColor = {0x0F, 0x0F, 0x0F};
        KEY_WIDTH = 67;
        KEY_HEIGHT = 350;
        mTexture.setColor(mColor.r, mColor.g, mColor.b);
    }
    xPos = 0;
    yPos = 0;
}

PianoKey::~PianoKey() {
    mTexture.free();
}

LTexture PianoKey::getLTexture() {
    return mTexture;
}

void PianoKey::renderLTexture(SDL_Renderer* gRenderer) {
    SDL_Rect renderQuad = {0, 0, KEY_WIDTH, KEY_HEIGHT};
    mTexture.render(gRenderer, xPos, yPos, &renderQuad);
}

void PianoKey::setPosition(int x, int y){
    xPos = x;
    yPos = y;
}

SDL_Point PianoKey::getPosition(){
    return {xPos, yPos};
}

struct AudioData {
    Uint8* pos;
    Uint32 length;
};

void PianoKey::playNote(Mix_Chunk* note){
    Mix_PlayChannel(-1, note, 0);
}

void playThreadedNote(Mix_Chunk* note, SDL_Event* event){
    Mix_PlayChannel(-1, note, 0);
}

void PianoKey::handleEvent(SDL_Event* event, Mix_Chunk* note, SDL_Point** points){
    //Get mouse position
    int x, y;
    SDL_GetMouseState(&x, &y);

    //checks if mouse is inside button
    bool inside = true;

    if(x > xPos + KEY_WIDTH)
        inside = false;
        
    if(x < xPos) 
        inside = false;
        
    if(y > yPos + KEY_HEIGHT)
        inside = false;
        
    if(y < yPos)
        inside = false;

    if(this->type == 'w'){
        std::vector<SDL_Point*> collision = detectCollision(points);
        if(collision.size()>0){
            if(y<350){
                if(collision.size()>1){
                    for(size_t i{0}; i<collision.size(); ++i){
                        if(!(x>collision[i]->x+67 || x<collision[i]->x)){
                            inside = false;
                        }
                        if(!(x<collision[i]->x+67 || x>collision[i]->x))
                                inside = false;
                    }
                } else{
                    if(!(x>collision[0]->x+67 || x<collision[0]->x))
                            inside = false;
                }
            }
        }
    }
        
    if(!inside){
        mTexture.setColor(mColor.r, mColor.g, mColor.b);
    } else {
        std::vector<bool> keyBuffer = {false, false, false};
        switch(event->type){
            case SDL_MOUSEMOTION:
                keyBuffer[0] = true;
                break;

            case SDL_MOUSEBUTTONDOWN:
                keyBuffer[1] = true;
                break;

            case SDL_MOUSEBUTTONUP:
                keyBuffer[2] = true;
                break;
                
            default:
                mTexture.setColor(mColor.r, mColor.g, mColor.b);
                break;
        }
        if(keyBuffer[0]){
            if(type == 'w')
                mTexture.setColor(mColor.r-52, mColor.g-52, mColor.b-52);
            if(type == 'b')
                mTexture.setColor(mColor.r-10, mColor.g-10, mColor.b-10);
            keyBuffer[0] = false;
        }
        if(keyBuffer[1]){
            if(type == 'w')
                mTexture.setColor(mColor.r-200, mColor.g-200, mColor.b-200);
            if(type == 'b')
                mTexture.setColor(mColor.r-15, mColor.g-15, mColor.b-15);
            std::thread threadedNotePlayer(playThreadedNote, note, event);
            threadedNotePlayer.join();
            //this->playNote(note);
            keyBuffer[1] = false;
        }
        if(keyBuffer[2]){
            mTexture.setColor(mColor.r, mColor.g, mColor.b);
            keyBuffer[2] = false;
        }
    }
}

std::vector<SDL_Point*> PianoKey::detectCollision(SDL_Point** points){
    std::vector<SDL_Point*> allPoints;
    for(size_t i{0}; i<5; ++i){
        if(points[i]->x < this->xPos){
            if(points[i]->x+67 > this->xPos){
                allPoints.push_back(points[i]);
            }
        }
        if(points[i]->x < this->xPos+this->KEY_WIDTH){
            if(points[i]->x+67 > this->xPos+this->KEY_WIDTH){
                allPoints.push_back(points[i]);
            }
        }
    }

    if(allPoints.size()!=0)
        return allPoints;

    return {};
}

bool PianoKey::handleKeyDown(SDL_Event* event){
    while(SDL_PollEvent(event)!=0){
        if(event->type == SDL_MOUSEBUTTONUP){
            return false;
        }
    }
    return true;
}