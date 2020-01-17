#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include<SDL2/SDL_mixer.h>
#include"SDL_extra.h"
#include<stdio.h>
#include<string>
#include<time.h>
#include"pianoKey.h"

//Screen Dimensions
const int SCREEN_WIDTH {1050};
const int SCREEN_HEIGHT {500};

//Default Functions
bool init();
bool loadMedia();
void close();

//Create window
SDL_Window* gWindow {NULL};

//Create renderer
SDL_Renderer* gRenderer {NULL};

//Closing LTexture
LTexture gExitTexture;

//Global font
TTF_Font* gFont {NULL};

//Piano Keys
PianoKey* whiteKeys[7];
PianoKey* blackKeys[5];

//Audio wavs
std::string noteFiles[12] {"c.wav", "d.wav", "e.wav", "f.wav", "g.wav", "a.wav", "b.wav", "cs.wav", "eb.wav", "fs.wav", "gs.wav", "bb.wav"};
Mix_Chunk* notes[12];


int main(int argc, char* args[]){
    if(!init()){
        printf("Could not initialize!\n");
    } else {
        if(!loadMedia()){
            printf("Could not load media!\n");
        } else {
            //create loop flag
            bool quit {false};

            //Create event handler
            SDL_Event event;
            while(!quit){
                //Clear renderer
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderClear(gRenderer);

                //Handle events
                while(SDL_PollEvent(&event)!=0){
                    if(event.type == SDL_QUIT){
                        quit = true;
                        //SDL_RenderSetScale(gRenderer, SCREEN_WIDTH/gExitTexture.getWidth(), SCREEN_HEIGHT/gExitTexture.getHeight());
                        gExitTexture.render(gRenderer, (SCREEN_WIDTH-gExitTexture.getWidth())/2, (SCREEN_HEIGHT-gExitTexture.getHeight())/2);
                        SDL_RenderPresent(gRenderer);
                        SDL_Delay(1000);
                    }

                    //Get black key Positions for Collision
                    SDL_Point* blackPoints[5];

                    for(size_t i{0}; i<5; ++i){
                        blackPoints[i] = new SDL_Point {blackKeys[i]->getPosition()};
                    }

                    for(size_t i{0}; i<7; ++i){
                        whiteKeys[i]->handleEvent(&event, notes[i], blackPoints);
                    }
                    for(size_t i{0}; i<5; ++i){
                        blackKeys[i]->handleEvent(&event, notes[i+7]);
                    }
                }

                for(size_t i{0}; i<7; ++i){
                    whiteKeys[i]->renderLTexture(gRenderer);
                }
                for(size_t i{0}; i<5; ++i){
                    blackKeys[i]->renderLTexture(gRenderer);
                }

                SDL_RenderPresent(gRenderer);
            }
        }
    }

    close();
    return 0;
}

bool init(){
    //Success flag
    bool success {true};

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("Could not initialize SDL! SDL_ERROR: %s\n", SDL_GetError());
        success = false;
    } else {
        //create window
        gWindow = SDL_CreateWindow("Piano Sim", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(gWindow == NULL){
            printf("Failed to create window! SDL_ERROR: %s\n", SDL_GetError());
            success = false;
        } else {
            //Create renderer for window
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(gRenderer == NULL) {
                printf("Renderer could not be created! SDL_ERROR: %s\n", SDL_GetError());
                success = false;
            } else {
                //Initialize renderer color
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

                //Initialize PNG loading
                int imgFlags {IMG_INIT_PNG};
                if(!(IMG_Init(imgFlags)&imgFlags)){
                    printf("Could not initialize SDL_image! IMG_ERROR: %s\n", IMG_GetError());
                    success = false;
                }

                //Initialize TTF loading
                if(TTF_Init() == -1){
                    printf("Could not initialize SDL_ttf! TFF_ERROR: %s\n", TTF_GetError());
                    success = false;
                }

                //initialize Mixer
                int mixFlags {MIX_INIT_MP3};
                if(!(Mix_Init(mixFlags)&mixFlags)){
                    printf("Could not initialize SDL_mixer! MIX_ERROR: %s\n", Mix_GetError());
                    success = false;
                }
            }
        }
    }

    return success;
}

bool loadMedia(){
    //Success flag
    bool success = true;

    size_t whiteKeyXPositions[7] = {0, 150, 300, 450, 600, 750, 900};
    size_t blackKeyXPositions[5] = {whiteKeyXPositions[1]-33, whiteKeyXPositions[2]-33, whiteKeyXPositions[4]-33, 
                                    whiteKeyXPositions[5]-33, whiteKeyXPositions[6]-33};

    //Load Keys
    for(size_t i{0}; i<7; i++){
        whiteKeys[i] = new PianoKey(0, i,gRenderer);
        whiteKeys[i]->setPosition(whiteKeyXPositions[i], 0);
    }
    for(size_t i{0}; i<5; i++){
        blackKeys[i] = new PianoKey(1, i+7,gRenderer);
        blackKeys[i]->setPosition(blackKeyXPositions[i], 0);
    }

    //Load notes
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048)<0){
        printf("Unable to open audio! MIX_ERROR: %s\n", Mix_GetError());
        success = false;
    }

    for(size_t i{0}; i<12; ++i){
        notes[i] = Mix_LoadWAV(("audio/key-"+noteFiles[i]).c_str());
    }

    //Load global font
    gFont = TTF_OpenFont("XeroxSerifWideBold.ttf", 100);
    if(gFont == NULL){
        printf("Unable to create Global Font! TTF_ERROR: %s\n", TTF_GetError());
        success = false;
    } else {
        //Initialize Random Seed
        srand (time(NULL));
        //Create color for print
        int r = rand() % 255;
        int g = rand() % 255;
        int b = rand() % 255;
        SDL_Color gExitColor = {r, g, b, 255};

        if(!gExitTexture.loadFromRenderedText(gRenderer, gFont, "Exiting Application!", gExitColor)){
            printf("Unable to load rendered text exit image! TTF_Error: %s\n", TTF_GetError());
            success = false;
        }
    }

    return success;
}

void close() {
    gExitTexture.free();

    for(size_t i{0}; i<7;i++){
        delete whiteKeys[i];
    }
    for(size_t i{0}; i<5; i++){
        delete blackKeys[i];
    }
    
    SDL_DestroyWindow(gWindow);
    SDL_DestroyRenderer(gRenderer);
    gWindow = NULL;
    gRenderer = NULL;

    //Free note chunk
    for(size_t i{0}; i<12; ++i){
        Mix_FreeChunk(notes[i]);
    }

    Mix_Quit();
    SDL_Quit();
    IMG_Quit();
    TTF_Quit();
}
