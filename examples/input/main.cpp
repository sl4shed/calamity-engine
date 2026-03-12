#include "backend/engine.hpp"
#include "backend/node.hpp"
#include "backend/definitions.hpp"
#include "backend/input.hpp"
#include "backend/audio.hpp"
#include "backend/graphics.hpp"
#include "backend/logger.hpp"
#include "backend/services.hpp"

int main() {
    Logger::init();
    Engine engine = Engine();
    Input input = Input();
    Graphics graphics = Graphics();
    Services::init(&graphics, &engine, &input);




    bool running = true;
    while(running) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }
        engine.update();
        engine.render(graphics);
    }
}
