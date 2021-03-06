#include "main.hpp"

#include "api/io/io.hpp"

#include <SDL2/SDL2_gfxPrimitives.h>

MainVar mainVar;

MainVar* getMain(){
    return &mainVar;
}

int main(int argc, char* argv[]){

    if (SDL_MAJOR_VERSION != 2){
        cerr << "ERROR :: cannot load missmatch SDL verion (" << SDL_MAJOR_VERSION << "!=2" << endl;
        return EXIT_FAILURE;
    }

    IS_DEBUG = false;

    TIME.now = time(0);
    TIME.ltm = localtime(&TIME.now);
    ZOOM = 1;
    
    PLAYER = new Entity(true);
    KEYPAD = new Keypad();

    mainVar.launched = readXML("data\\main.xml");

    while (mainVar.launched){

        TIME.startTick = SDL_GetTicks();

        event();
        update();
        draw();
        time();

        DELTA_TIME = SDL_GetTicks() - TIME.startTick;
    }

    destroy();

    return EXIT_SUCCESS;
}

void event(){
    SDL_Event e;

    while (SDL_PollEvent(&e)){
        switch (e.type){

            case SDL_QUIT:
                mainVar.launched = false;
                break;
            
            case SDL_MOUSEMOTION:
                mainVar.event.mouse.x = e.motion.x;
                mainVar.event.mouse.y = e.motion.y;
                break;

            case SDL_MOUSEBUTTONDOWN:
                switch (e.button.button){
                    case SDL_BUTTON_LEFT:
                        MOUSEBTN.left = true;
                        MOUSEDOWN.left = true;
                        break;
                    
                    case SDL_BUTTON_MIDDLE:
                        MOUSEBTN.middle = true;
                        MOUSEDOWN.middle = true;
                        break;
                    
                    case SDL_BUTTON_RIGHT:
                        MOUSEBTN.right = true;
                        MOUSEDOWN.right = true;
                        break;
                        
                    default:
                        break;
                }
                break;
            
            case SDL_MOUSEBUTTONUP:
                switch (e.button.button){
                    case SDL_BUTTON_LEFT:
                        MOUSEBTN.left = false;
                        MOUSEUP.left = true;
                        break;
                    
                    case SDL_BUTTON_MIDDLE:
                        MOUSEBTN.middle = false;
                        MOUSEUP.left = true;
                        break;
                    
                    case SDL_BUTTON_RIGHT:
                        MOUSEBTN.right = false;
                        MOUSEUP.left = true;
                        break;
                        
                    default:
                        break;
                }
                break;
            
            default:
                KEYPAD->event(e);
                break;
        }
    }
}

void updateWidgets(void){
    int i=0;
    for (TextButton* btn : WIDGETS.textButtons){
        if (btn){
            btn->update();
            i++;
        } else {
            WIDGETS.textButtons.erase(WIDGETS.textButtons.begin() + i);
        }
    }

    i=0;
    for (ImageButton* btn : WIDGETS.imageButtons){
        if (btn){
            btn->update();
            i++;
        } else {
            WIDGETS.imageButtons.erase(WIDGETS.imageButtons.begin() + i);
        }
    }

    i=0;
    for (SpriteButton* btn : WIDGETS.spriteButtons){
        if (btn){
            btn->update();
            i++;
        } else {
            WIDGETS.spriteButtons.erase(WIDGETS.spriteButtons.begin() + i);
        }
    }
}

void resetMouse(void){
    MOUSEUP.left = false;
    MOUSEUP.middle = false;
    MOUSEUP.right = false;

    MOUSEDOWN.left = false;
    MOUSEDOWN.middle = false;
    MOUSEDOWN.right = false;
}

void update(){
    int exec = SDL_GetTicks();

    updateWidgets();
    update_ammunitions((void*)NULL);
    SDL_Thread* particle = SDL_CreateThread(update_particles, "particles", (void*)NULL);

    if (!PAUSE){
        int i = 0;
        for (Entity* entity : ENTITY){
            if (entity->should_delete()){
                ENTITY.erase(ENTITY.begin() + i);
                delete entity;
                continue;
            } else {
                entity->update();
            }
            i++;
        }

        if (PLAYER->is_linked()){
            PLAYER->update();
            CAMERA.x = - PLAYER->getX() + ((windowWidth() - PLAYER->getW()) / 2);
            CAMERA.y = - PLAYER->getY() + ((windowHeight() - PLAYER->getH()) / 2);
        }
    }
    
    resetMouse();
    SDL_WaitThread(particle, NULL);
    mainVar.time.updateExecT = SDL_GetTicks() - exec;
}

void drawWidgets(void){
    int i=0;
    for (Text* text : WIDGETS.texts){
        if (!text || !text->draw()){
            WIDGETS.texts.erase(WIDGETS.texts.begin() + i);
            if (text) delete text;
            text = NULL;
            continue;
        }
        i++;
    }

    i=0;
    for (TextButton* btn : WIDGETS.textButtons){
        if (!btn || !btn->draw()){
            WIDGETS.textButtons.erase(WIDGETS.textButtons.begin() + i);
            if (btn) delete btn;
            btn = NULL;
            continue;
        }
        i++;
    }

    i=0;
    for (ImageButton* btn : WIDGETS.imageButtons){
        if (!btn || !btn->draw()){
            WIDGETS.imageButtons.erase(WIDGETS.imageButtons.begin() + i);
            if (btn) delete btn;
            btn = NULL;
            continue;
        }
        i++;
    }

    i=0;
    for (SpriteButton* btn : WIDGETS.spriteButtons){
        if (!btn || !btn->draw()){
            WIDGETS.spriteButtons.erase(WIDGETS.spriteButtons.begin() + i);
            if (btn) delete btn;
            btn = NULL;
            continue;
        }
        i++;
    }
}

void drawLayers(int z){
    int lr=0;

    for (Layer* lyr : LAYERS){
        if (lyr){
            if (lyr->getZ() == z){
                int i=0;
                for (Light* l : LIGHTS){
                    if (!l || !l->draw(z)){
                        LIGHTS.erase(LIGHTS.begin() + i);
                        if (l) delete l;
                        continue;
                    }
                    i++;
                }

                i=0;
                for (Entity* entity : ENTITY){
                    if (!entity->drawLight(z)){
                        delete entity;
                        ENTITY.erase(ENTITY.begin() + i);
                        continue;
                    }
                    i++;
                }

                if (PLAYER->is_linked()) PLAYER->drawLight(z);
                
                if (!lyr->draw()){
                    LAYERS.erase(LAYERS.begin() + lr);
                    delete lyr;
                    lyr = NULL;
                    continue;
                }
            }
        } else {
            LAYERS.erase(LAYERS.begin() + lr);
            if (lyr) delete lyr;
            lyr = NULL;
            continue;
        }
        lr++;
    }

    int i=0;
    for (Entity* entity : ENTITY){
        if (entity->getZ() == z){
            if (!entity->draw()){
                ENTITY.erase(ENTITY.begin() + i);
                delete entity;
                continue;
            }
            i++;
        }
    }

    if (PLAYER->getZ() == z && PLAYER->is_linked()) PLAYER->draw();
    
    draw_ammunitions(z);
    draw_particles(z);
}

void drawImages(void){
    int i=0;
    for (Image* image : IMAGES){
        if (!image->draw()){
            delete image;
            IMAGES.erase(IMAGES.begin() + i);
            continue;
        }
        i++;
    }
}

void draw(){
    int exec = SDL_GetTicks();

    SDL_SetRenderDrawColor(RENDERER, WINDOW_R, WINDOW_G, WINDOW_B, WINDOW_A);
    SDL_SetRenderDrawBlendMode(RENDERER, SDL_BLENDMODE_NONE);

    if (SDL_RenderClear(RENDERER)){
        if (IS_ERR_OPEN) ERR << "ERROR :: SDL_RenderClear(), reason : " << SDL_GetError << endl;
        mainVar.launched = false;
        return;
    }
    
    for (int i=-11; i<11; i++){ 
        drawLayers(i);
    }

    drawImages();
    drawWidgets();
    
    SDL_RenderPresent(RENDERER);
    mainVar.time.drawExecT = SDL_GetTicks() - exec;
}

void time(){

    if(SDL_GetTicks() < mainVar.time.tick+1000){
        mainVar.time._fps++;

    } else {
        mainVar.time.FPS = mainVar.time._fps;

        //printf("FPS : %d\n", mainVar.time.FPS);

        mainVar.time.tick = SDL_GetTicks();
        mainVar.time._fps=0;
    }

    int finnalTick = SDL_GetTicks();

    mainVar.time.execTime = (finnalTick - mainVar.time.startTick) > mainVar.time.maxDelay ? mainVar.time.maxDelay : (finnalTick - mainVar.time.startTick);
    SDL_Delay(mainVar.time.maxDelay-mainVar.time.execTime);
}

void freeAmmunitions_type(void){
    for (Ammunition_type* a : AMMUNITION_TYPE){
        if (a) delete a;
    }
    AMMUNITION_TYPE.clear();
}

void freeFont(void){
    if(TTF_WasInit()){
        
        if (MAINVAR->font.font){
            if (IS_LOG_OPEN) LOG << "TTF_CloseFont()" << endl;
            TTF_CloseFont(MAINVAR->font.font);
            MAINVAR->font.font = NULL;
        }

        if (IS_LOG_OPEN) LOG << "TTF_Quit()" << endl;
        TTF_Quit();
    }
}

void freeWindow(void){
    if (IS_LOG_OPEN) LOG << "SDL_DestroyRenderer()" << endl;
    if (MAINVAR->renderer) SDL_DestroyRenderer(MAINVAR->renderer);
    
    if (IS_LOG_OPEN) LOG << "SDL_DestroyWindow()" << endl;
    if (MAINVAR->window) SDL_DestroyWindow(MAINVAR->window);

    if (IS_LOG_OPEN) LOG << "SDL_Quit()" << endl;
    SDL_Quit();
}

void freeWidgets(void){
    if (IS_LOG_OPEN) LOG << "freeWidgets()" << endl;

    for (Text* text : WIDGETS.texts){
        if (text) delete text;
        text = NULL;
    }
    WIDGETS.texts.clear();

    for (TextButton* btn : WIDGETS.textButtons){
        if (btn) delete btn;
        btn = NULL;
    }
    WIDGETS.textButtons.clear();

    for (ImageButton* btn : WIDGETS.imageButtons){
        if (btn) delete btn;
        btn = NULL;
    }
    WIDGETS.imageButtons.clear();

    for (SpriteButton* btn : WIDGETS.spriteButtons){
        if (btn) delete btn;
        btn = NULL;
    }
    WIDGETS.spriteButtons.clear();

    for (Image* image : IMAGES){
        delete image;
        image = NULL;
    }
    IMAGES.clear();
}

void freeParticle(void){
    for (Particles_type* p : PARTICLES_TYPE){
        if (p) delete p;
    }
    PARTICLES_TYPE.clear();

    for (Particles* p : PARTICLES){
        if (p) delete p;
    }
    PARTICLES.clear();
}

void freeEntity(void){
    if (IS_LOG_OPEN) LOG << "freeEntity()" << endl;;
    for (Entity* entity : ENTITY){
        if (entity) delete entity;
    }
    ENTITY.clear();

    PLAYER->unlink();
    if (IS_LOG_OPEN) LOG << "\tended" << endl;;
}

void freeEntity_types(void){
    for (Entity_type* type : ENTITY_TYPES){
        delete type;
    }
    ENTITY_TYPES.clear();
}

void freeEquipments(void){
    for (Equipment_type* e : EQUIPMENTS){
        if (e) delete e;
    }
}

void freeParticles(void){
    for (Particles*p : PARTICLES){
        if (p) delete p;
    }
    PARTICLES.clear();
}

void destroy(void){

    freeWidgets();
    freeParticles();

    for (Layer* lyr : LAYERS){
        delete lyr;
        lyr = NULL;
    }
    LAYERS.clear();

    clearLights();
    freeEntity_types();
    freeEntity();
    freeEquipments();
    clear_ammunitions();
    clear_Ammunition_type();
    clear_particles();
    clear_particles_type();
    
    delete KEYPAD;
    delete PLAYER;

    freeFont();
    freeWindow();

    if (IS_LOG_OPEN) LOG << "\n\t-= exit =-" << endl;

    if (IS_LOG_OPEN) LOG.close();
    if (IS_ERR_OPEN) ERR.close();
}

void setLog(string path){
    mainVar.log_file = path;
}
void setErr(string path){
    mainVar.err_file = path;
}

bool loadLog(void){
    if (IS_LOG_OPEN) LOG.close();

    LOG.open(MAINVAR->log_file, ios::out);

    if (!LOG){
        cerr << "cannot load '" << MAINVAR->log_file << "' file" << endl;
        return false;
    }

    LOG << "LOG file, init at : " << HOURE << ":" << MINUTE << ":" << SECOND << endl;

    return true;
}

bool loadErr(void){
    if (IS_ERR_OPEN) ERR.close();

    ERR.open(MAINVAR->log_file, ios::out);

    if (!ERR){
        cerr << "cannot load '" << MAINVAR->err_file << "' file" << endl;
        return false;
    }

    ERR << "ERROR file, init at : " << HOURE << ":" << MINUTE << ":" << SECOND << endl;
    return true;
}

void closeLog(void){
    LOG.close();
}

void closeERR(void){
    ERR.close();
}

Point getMousePos(void){
    return (Point){MOUSE.x, MOUSE.y};
}
int windowWidth(void){
    int w, h;
    SDL_GetWindowSize(WINDOW, &w, &h);
    return w;
}

int windowHeight(void){
    int w, h;
    SDL_GetWindowSize(WINDOW, &w, &h);
    return h;
}

bool setIcon(string path){
    path = DIR + path;
    SDL_Surface* surface = SDL_LoadBMP(path.c_str());

    if (!surface){
        if (IS_ERR_OPEN) ERR << "ERROR :: SDL_LoadBMP('" << path << "')" << ", reason : " << SDL_GetError() << endl;
        return false;
    }

    SDL_SetWindowIcon(WINDOW, surface);
    SDL_FreeSurface(surface);
    
    return true;
}

void SetColor(int r, int g, int b){
    WINDOW_R = r;
    WINDOW_G = g;
    WINDOW_B = b;
}

void SetColor(int r, int g, int b, int a){
    WINDOW_R = r;
    WINDOW_G = g;
    WINDOW_B = b;
    WINDOW_A = a;
}