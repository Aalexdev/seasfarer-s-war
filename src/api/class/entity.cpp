#include "api/class/entity.hpp"
#include "main.hpp"

Part_type* Part_Type_list::search(std::string name){
    for (Part_type* type : this->types){
        if (type->getName() == name){
            return type;
        }
    }
    if (IS_ERR_OPEN) ERR << "ERROR :: Part_Type_list::search('" << name << "') cannot reconize '" << name << "'" << endl;
    return nullptr;
}

Part_Type_list::Part_Type_list(){
    if (IS_LOG_OPEN) LOG << "Part_Type_list::Part_Type_list()" << endl;
}

Part_Type_list::~Part_Type_list(){
    if (IS_LOG_OPEN) LOG << "Part_Type_list::~Part_Type_list()" << endl;
    for (Part_type* type : this->types){
        if (type) delete type;
    }
    this->types.clear();
}

void Part_Type_list::push(Part_type* type){
    if (IS_LOG_OPEN) LOG << "Part_Type_list::push()" << endl;
    this->types.push_back(type);
}
void Part_Type_list::pop(void){
    if (IS_LOG_OPEN) LOG << "Part_Type_list::pop()" << endl;
    this->types.pop_back();
}
Part_type::Part_type(){

}

Part_type::~Part_type(){
    name.clear();
}

bool Part_type::read_from_xml(XMLNode* node){
    if (IS_LOG_OPEN) LOG << "Part_type::read_from_xml()" << endl;

    if (!node){
        if (IS_ERR_OPEN) ERR << "ERROR :: Part_type::read_from_xml(), reason : cannot read from a null xml Node" << endl;
        return false;
    }

    for (int a=0; a<node->attributes.size; a++){
        XMLAttribute attr = node->attributes.data[a];

        if (!strcmp(attr.key, "name")){
            this->name = attr.value;
        } else {
            if (IS_ERR_OPEN) ERR << "WARNING :: Part_type::read_from_xml(), reason : cannot reconize '" << attr.key << "' part attribute" << endl;
        }
    }

    return false;
}

Entity_type::Entity_type(){
    if (IS_LOG_OPEN) LOG << "Entity_type::Entity_type()" << endl;
    this->name = "Uncknow";
    this->rect = (SDL_Rect){0, 0, 0, 0};
    this->mass = 0;
}

Entity_type::~Entity_type(){
    if (IS_LOG_OPEN) LOG << "Entity_type::~Entity_type()" << endl;
    if (this->texture) SDL_DestroyTexture(this->texture);
    this->name.clear();
}

bool Entity_type::load_from_xml(XMLNode* node){
    if (IS_LOG_OPEN) LOG << "Entity_type::load_from_xml" << endl;
    if (!node){
        if (IS_ERR_OPEN) ERR << "ERROR :: Entity_load::load_from_xml, reason : cannot load from a null node" << endl;
        return false;
    }

    for (int a=0; a<node->attributes.size; a++){
        XMLAttribute attr = node->attributes.data[a];

        if (!strcmp(attr.key, "name")){

            if (!this->used_name(attr.value)){
                this->name = attr.value;
            } else {
                if (IS_ERR_OPEN) ERR << "ERROR :: Entity_type::load_from_xml, reason : name '" << attr.value << "' is erealy used" << endl;
                return false;
            }

        } else {
            if (IS_ERR_OPEN) ERR << "WARNING :: Entity_type::load_from_xml, reason : cannot reconize '" << attr.key << "' entity attribute" << endl;
        }
    }

    for (int c=0; c<node->children.size; c++){
        XMLNode* child = XMLNode_child(node, c);

        if (!strcmp(child->tag, "texture")){
            for (int a=0; a<child->attributes.size; a++){
                XMLAttribute attr = child->attributes.data[a];

                if (!strcmp(attr.key, "path")){
                    int w=this->rect.w, h=this->rect.h;
                    this->texture = loadTexture(attr.value, &this->rect);
                } else if (!strcmp(attr.key, "w")){
                    sscanf(attr.value, "%d", &this->rect.w);
                } else if (!strcmp(attr.key, "h")){
                    sscanf(attr.value, "%d", &this->rect.h);
                } else {
                    if (IS_ERR_OPEN) ERR << "WARNING :: Entity_type::load_from_xml, reason : cannot reconize '" << attr.key << "' texture attribute" << endl;
                }
            }
        } else if (!strcmp(child->tag, "perform")){
            for (int a=0; a<child->attributes.size; a++){
                XMLAttribute attr = child->attributes.data[a];

                if (!strcmp(attr.key, "mass")){
                    sscanf(attr.value, "%f", &this->mass);
                } else if (!strcmp(attr.key, "strength")){
                    sscanf(attr.value, "%f", &this->strength);
                } else if (!strcmp(attr.key, "maxSpeed")){
                    sscanf(attr.value, "%d", &this->maxSpeed);
                } else if (!strcmp(attr.key, "minSpeed")){
                    sscanf(attr.value, "%d", &this->minSpeed);
                } else {
                    if (IS_ERR_OPEN) ERR << "ERROR :: Entity_type;;load_from_xml, reason : cannot reconize '" << attr.key << "' perform attribute" << endl;
                }
            }
        } else if (!strcmp(child->tag, "layer")){
            for (int a=0; a<child->attributes.size; a++){
                XMLAttribute attr = child->attributes.data[a];

                if (!strcmp(attr.key, "max")){
                    sscanf(attr.value, "%d", &this->layerMax);
                } else if (!strcmp(attr.key, "min")){
                    sscanf(attr.value, "%d", &this->layerMin);
                } else {
                    if (IS_ERR_OPEN) ERR << "WARNING :: Entity_type::load_from_xml, reason : cannot reconize '" << attr.key << "' layer attribute" << endl;
                }
            }
        } else {
            if (IS_LOG_OPEN) ERR << "WARNING :: Entity_type::load_from_xml, reason : cannot reconize '" << child->tag << "' entity child" << endl;
        }
    }

    return true;
}

bool Entity::set(std::string name){
    if (IS_LOG_OPEN) LOG << "Entity::set('" << name << "')" << endl;
    if (name.empty()){
        if (IS_ERR_OPEN) ERR << "ERROR :: Entity::set, reason : cannot load a null type name" << endl;
        return false;
    }

    for (Entity_type* type : ENTITY_TYPES){
        if (type->get_name() == name){
            this->type = type;

            this->rect.w = type->getRect().w;
            this->rect.h = type->getRect().h;

            return true;
        }
    }

    if (IS_ERR_OPEN) ERR << "ERROR :: Entity::set, reason : cannot load '" << name << "' entity name" << endl;
    return false;
}

bool Entity_type::used_name(std::string name){
    for (Entity_type* type : ENTITY_TYPES){
        if (type->get_name() == name){
            return true;
        }
    }
    return false;
}

bool Entity::load_from_xml(XMLNode* node){
    if (IS_LOG_OPEN) LOG << "Entity::load_from_xml()" << endl;
    if (!node){
        if (IS_ERR_OPEN) ERR << "ERROR :: Entity::load_from_xml, reason : cannot load from a null node" << endl;
        return false;
    }

    for (int a=0; a<node->attributes.size; a++){
        XMLAttribute attr = node->attributes.data[a];

        if (!strcmp(attr.key, "x")){
            sscanf(attr.value, "%d", &this->rect.x);
        } else if (!strcmp(attr.key, "y")){
            sscanf(attr.value, "%d", &this->rect.y);
        } else if (!strcmp(attr.key, "type")){
            if (!this->set(attr.value)) return false;
        } else if (!strcmp(attr.key, "height")){
            sscanf(attr.value, "%d", &this->z);
        } else if (!strcmp(attr.key, "angle")){
            sscanf(attr.value, "%f", &this->angle);
        } else if (!strcmp(attr.key, "health")){
            sscanf(attr.value, "%d", &this->health);
        } else {
            if (IS_ERR_OPEN) ERR << "WARNING :: Entity::load_from_xml, reason : cannot reconize '" << attr.key  << "' summonEntity attribute" << endl;
        }
    }


    for (int c=0; c<node->children.size; c++){
        XMLNode* child = XMLNode_child(node, c);

        if (!strcmp(child->tag, "team")){
            for (int a=0; a<child->attributes.size; a++){
                XMLAttribute attr = child->attributes.data[a];

                if (!strcmp(attr.key, "team")){
                    
                } else {
                    if (IS_ERR_OPEN) ERR << "WARNING :: Entity::load_from_xml, reason : cannot reconize '" << attr.key << "' team attribute" << endl;
                }
            }
        } else if (!strcmp(child->tag, "events")){
            if (!this->is_player) {
                if (IS_ERR_OPEN) ERR << "ERROR :: Entity::load_from_xml, reason : cannot set events of a not player entity" << endl;
            } else {
                for (int e=0; e<child->children.size; e++){
                    XMLNode* event_type = XMLNode_child(child, e);

                    if (!strcmp(event_type->tag, "keypad")){
                        for (int k=0; k<event_type->children.size; k++){
                            XMLNode* key = XMLNode_child(event_type, k);

                            if (!strcmp(key->tag, "key")){

                                std::string tag, out;
                                bool type = true;
                                for (int a=0; a<key->attributes.size; a++){
                                    XMLAttribute attr = key->attributes.data[a];

                                    if (!strcmp(attr.key, "tag")){
                                        tag = attr.value;
                                    } else if (!strcmp(attr.key, "out")){
                                        out = attr.value;
                                    } else if (!strcmp(attr.key, "type")){
                                        if (!strcmp(attr.value, "sticky")){
                                            type = false;
                                        } else if(!strcmp(attr.value, "push")){
                                            type = true;
                                        } else {
                                            if (IS_ERR_OPEN) ERR << "WARNING :: Entity::load_from_xml, reason : cannot reconize '" << attr.value << "' type value" << endl;
                                        }
                                    } else {
                                        if (IS_ERR_OPEN) ERR << "WARNING :: Entity::load_from_xml, reason : cannot reconize '" << attr.key << "' key attribute" << endl;
                                    }
                                }

                                if (!tag.empty() && !out.empty()){
                                    if (out == "engineUp"){
                                        PLAYER_CONTROL.engineUp = SDL_GetScancodeFromName(tag.c_str());
                                        PLAYER_CONTROL.engineUP_type = type;
                                    } else if (out == "engineDown"){
                                        PLAYER_CONTROL.engineDown = SDL_GetScancodeFromName(tag.c_str());
                                        PLAYER_CONTROL.engineDown_type = type;
                                    } else if (out == "turnLeft"){
                                        PLAYER_CONTROL.turnLeft = SDL_GetScancodeFromName(tag.c_str());
                                        PLAYER_CONTROL.turnLeft_type = type;
                                    } else if (out == "turnRight"){
                                        PLAYER_CONTROL.turnRight = SDL_GetScancodeFromName(tag.c_str());
                                        PLAYER_CONTROL.turnRight_type = type;
                                    } else if (out == "layerUp"){
                                        PLAYER_CONTROL.layerUp = SDL_GetScancodeFromName(tag.c_str());
                                        PLAYER_CONTROL.layerUp_type = type;
                                    } else if (out == "layerDown"){
                                        PLAYER_CONTROL.layerDown = SDL_GetScancodeFromName(tag.c_str());
                                        PLAYER_CONTROL.layerDown_type = type;
                                    } else {
                                        if (IS_ERR_OPEN) ERR << "ERROR :: Entity::load_from_xml, reason : cannot reconize '" << out << "' key out" << endl;
                                    }
                                } else {
                                    if (IS_ERR_OPEN) ERR << "ERROR :: Entity::load_from_xml, reason : cannot load a key from an incomplet key node" << endl;
                                }
                            } else {
                                if (IS_ERR_OPEN) ERR << "WARNING :: Entity::load_from_xml, reason : cannot reconize '" << key->tag << "' keypad child" << endl;
                            }
                        }
                    } else if (!strcmp(event_type->tag, "mouse")){

                    } else {
                        if (IS_ERR_OPEN) ERR << "WARNING :: Entity::load_from_xml, reason : cannot reconize '" << event_type->tag << "' events child" << endl;
                    }
                }
            }
            
        } else if (!strcmp(child->tag, "parts")){

        } else {
            if (IS_ERR_OPEN) ERR << "WARNING :: Entity::load_from_xml, reason : cannot reconize '" << child->tag << "' summonEntity child" << endl;
        }
    }

    return true;
}

void Entity::reset(void){
    this->type = nullptr;
    this->z = 0;
    this->angle = 0;

    this->speed=0;
    this->acceleration = 0;
    this->strength = 0;
    this->acceleration = 0;
    this->turn_strength = 0;
    this->turn_speed = 0;
}

Entity::Entity(){
    if (IS_LOG_OPEN) LOG << "Entity::Entity()" << endl;
    this->reset();
}

Entity::Entity(bool isPlayer){
    if (IS_LOG_OPEN) LOG << "Entity::Entity()" << endl;
    this->reset();
    this->is_player = isPlayer;
}

Entity::~Entity(){
    if (IS_LOG_OPEN) LOG << "Entity::~Entity()" << endl;
    
    this->unlink();
}

void Entity::update(){

    if (this->is_player){
        
        if (KEYPAD->getKey(PLAYER_CONTROL.engineUp)){
            this->strength = this->type->getStrength();
        } else if (KEYPAD->getKey(PLAYER_CONTROL.engineDown)){
            this->strength = -this->type->getStrength();
        } else {
            if (this->speed > 0){
                if (PLAYER_CONTROL.engineUP_type) this->strength = -this->type->getStrength();
            } else {
                if (PLAYER_CONTROL.engineDown_type) this->strength = this->type->getStrength();
            }
        }

        if (KEYPAD->getKey(PLAYER_CONTROL.turnLeft)){
            this->turn_strength = -this->type->getStrength();
        } else if (KEYPAD->getKey(PLAYER_CONTROL.turnRight)){
            this->turn_strength = this->type->getStrength();
        } else {
            if (this->turn_speed > 0){
                if (PLAYER_CONTROL.turnRight_type) this->turn_strength = -this->type->getStrength();
            } else {
                if (PLAYER_CONTROL.turnLeft_type) this->turn_strength = this->type->getStrength();
            }
        }

        if (this->strength != 0 && this->type->getMass() != 0){
            this->acceleration = this->strength / this->type->getMass();
        } else {
            this->acceleration = 0;
        }

        this->speed += this->acceleration * (MAINVAR->time.execTime);
        
        if (this->speed > this->type->getMaxSpeed()){
            this->speed = this->type->getMaxSpeed();
        } else if (this->speed < this->type->getMinSpeed()){
            this->speed = this->type->getMinSpeed();
        }

        if (this->turn_strength != 0 && this->type->getMass() != 0){
            this->turn_acceleration = this->turn_strength / (this->type->getMass() * this->type->getRect().h) * 2;
        } else {
            this->turn_acceleration = 0;
        }

        this->turn_speed += this->turn_acceleration * (MAINVAR->time.execTime);
    }

    if (this->speed!=0){
        this->is_mouving = true;
        int x, y;
        setAngleM(&x, &y, this->speed, this->angle-90);
        this->rect.x += x;
        this->rect.y += y;
    } else {
        this->is_mouving = false;
    }

    if (this->turn_speed != 0){
        this->angle += this->turn_speed;
    }
}

bool Entity::draw(){
    if (!this->type){
        if (IS_ERR_OPEN) ERR << "ERROR :: Entity::draw, reason : cannot draw a entity who is not linked to a type" << endl;
        return false;
    }

    SDL_Rect temp = {this->rect.x + CAMERA.x, this->rect.y + CAMERA.y, (int)(this->rect.w / ZOOM), (int)(this->rect.h / ZOOM)};
    if (SDL_RenderCopyEx(RENDERER, this->type->getTexture(), NULL, &temp, this->angle, 0, SDL_FLIP_NONE)){
        if (IS_ERR_OPEN) ERR << "ERROR :: SDL_RenderCopy, reason : " << SDL_GetError() << endl;
        return false;
    }

    if (IS_DEBUG){
        SDL_SetRenderDrawColor(RENDERER, 255, 0, 0, 255);
        int index = 0, x, y;
        for (HitPoint *point : this->hitPoints){

            if (index == 0){
                x = this->hitPoints.back()->x;
                y = this->hitPoints.back()->y;
            }

            if (SDL_RenderDrawLine(RENDERER, x, y, point->x, point->y)){
                cerr << "SDL_RenderDrawLine Error : " << SDL_GetError() << endl;
                return false;
            }

            x = point->x;
            y = point->y;

            index++;
        }

        SDL_SetRenderDrawColor(RENDERER, 0, 0, 0, 0);
    }
    return true;
}

void Entity::unlink(void){
    this->type = nullptr;

    for (HitPoint* hpnt : this->hitPoints){
        if (hpnt) delete hpnt;
    }
    this->hitPoints.clear();

    this->angle = 0;
    this->speed = 0;
}