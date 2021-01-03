/*
 * GameRenderer.cpp
 *
 *  Created on: Aug 16, 2015
 *      Author: axel
 */

#include <stddef.h>
#include <SDL2/SDL_blendmode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <utility>
#include <math.h>

#include "../Game/Board.h"
#include "../Game/Game.h"
#include "../Game/VsGame.h"
#include "../SDLContext.h"
#include "VsGameRenderer.h"

#include "helper.h"

using namespace std;

VsGameRenderer::VsGameRenderer(VsGame &game) :
    GameRenderer(game),
    _game(game),
    _boardRenderer0(_game.getBoard(0)),
    _boardRenderer1(_game.getBoard(1)),
    _gbqr0(_game.getBoard(0)),
    _gbqr1(_game.getBoard(1)),
    _b0Shake(0),
    _b1Shake(0) {
    _texture = SDL_CreateTexture(_SDLRenderer, SDL_PIXELFORMAT_RGBA8888,
                                 SDL_TEXTUREACCESS_TARGET, 640, 480);
    _bg = _SDLContext.makeTextureFromImage("assets/forest.png");

    
}

void VsGameRenderer::tick() {
    if (!_game.isPaused()) {
        _boardRenderer0.tick();
        _boardRenderer1.tick();
        handleParticles();
        handleShake();
    }
}

SDL_Texture *VsGameRenderer::renderGame() {

    SDL_Texture *gbq = _gbqr0.renderQueue();
    SDL_Texture *gbq2 = _gbqr1.renderQueue();

    SDL_SetRenderTarget(_SDLRenderer, _texture);
    SDL_SetRenderDrawColor(_SDLRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(_SDLRenderer);

    SDL_RenderCopy(_SDLRenderer, _bg, NULL, NULL);
    //SDL_RenderCopy(_SDLRenderer, _2pbg, NULL, NULL);

    if (!_game.isPaused()) {

        renderBoard(0);
        renderBoard(1);
        SDL_Rect gbqp = {258, 307, 38, 120};
        SDL_RenderCopy(_SDLRenderer, gbq, NULL, &gbqp);
        gbqp.x = 344;
        SDL_RenderCopy(_SDLRenderer, gbq2, NULL, &gbqp);
        renderParticles();

    }
    renderStatsText();
    renderMatchPoints();

    if (_game.isPaused()) {
        SDL_SetTextureColorMod(_texture, 0x50, 0x50, 0x50);
        SDL_RenderCopy(_SDLRenderer, _texture, NULL, NULL);
        SDL_SetTextureColorMod(_texture, 0xFF, 0xFF, 0xFF);
        _game.getPauseMenu().render();
    } else {
        SDL_SetTextureColorMod(_texture, 0xFF, 0xFF, 0xFF);
    }

    //TODO: make this state more fancy
    if (_game.getState() == Game::State::ENDED
        && SDL_GetTicks() % 1000 >= 500) {
        _SDLContext.renderText("PUSH START", {255, 255, 255},
                               _SDLContext._fontSquare, 134, 342);
    }

    return _texture;
}

void VsGameRenderer::renderStatsText() {
    
    const int INNER_WIDTH = BOARD1_X - BOARD0_X - BoardRenderer::BOARD_WIDTH;

    //Character box -- 4
    SDL_Rect char_box;
    char_box.x = BOARD0_X+BoardRenderer::BOARD_WIDTH;
    char_box.y = BOARD0_Y;
    char_box.w = INNER_WIDTH;
    char_box.h = BoardRenderer::TILE_SIZE * 4;

    SDL_SetRenderDrawColor(_SDLRenderer,0,0,0,200);
    SDL_RenderFillRect(_SDLRenderer,&char_box);
    
    SDL_SetRenderDrawColor(_SDLRenderer,255,255,255,255);
    SDL_RenderDrawRect(_SDLRenderer, &char_box);

    //Game wins box -- 2
    SDL_Rect point_box;
    point_box.x = BOARD0_X+BoardRenderer::BOARD_WIDTH;
    point_box.y = BOARD0_Y + char_box.h;
    point_box.w = INNER_WIDTH;
    point_box.h = BoardRenderer::TILE_SIZE * 2;

    SDL_SetRenderDrawColor(_SDLRenderer,0,0,0,200);
    SDL_RenderFillRect(_SDLRenderer,&point_box);
    
    SDL_SetRenderDrawColor(_SDLRenderer,255,255,255,255);
    SDL_RenderDrawRect(_SDLRenderer, &point_box);

    _SDLContext.renderText("POINT", {255, 255, 255}, _SDLContext._fontPs,
                           point_box.x+(point_box.w/2), point_box.y+4 );

    std::ostringstream os1;
    os1 << std::setw(2) << std::setfill('0') << _game.getP1Points();
    _SDLContext.renderText(os1.str(), {255, 255, 255}, _SDLContext._fontPs,
                           point_box.x+(point_box.w/4), point_box.y+4+32 );

    std::ostringstream os2;
    os2 << std::setw(2) << std::setfill('0') << _game.getP1Points();
    _SDLContext.renderText(os2.str(), {255, 255, 255}, _SDLContext._fontPs,
                           point_box.x+((3*point_box.w)/4), point_box.y+4+32 );

    //Level Box -- 2
    SDL_Rect level_box;
    level_box.x = BOARD0_X+BoardRenderer::BOARD_WIDTH;
    level_box.y = BOARD0_Y + char_box.h + point_box.h;
    level_box.w = INNER_WIDTH;
    level_box.h = BoardRenderer::TILE_SIZE * 2;

    SDL_SetRenderDrawColor(_SDLRenderer,0,0,0,200);
    SDL_RenderFillRect(_SDLRenderer,&level_box);
    
    SDL_SetRenderDrawColor(_SDLRenderer,255,255,255,255);
    SDL_RenderDrawRect(_SDLRenderer, &level_box);

    _SDLContext.renderText("LEVEL", {255, 255, 255}, _SDLContext._fontPs,
                           level_box.x+(point_box.w/2), level_box.y+4 );

    //Time Box -- 2
    SDL_Rect time_box;
    time_box.x = BOARD0_X+BoardRenderer::BOARD_WIDTH;
    time_box.y = BOARD0_Y + char_box.h + point_box.h + level_box.h;
    time_box.w = INNER_WIDTH;
    time_box.h = BoardRenderer::TILE_SIZE * 2;

    SDL_SetRenderDrawColor(_SDLRenderer,0,0,0,200);
    SDL_RenderFillRect(_SDLRenderer,&time_box);
    
    SDL_SetRenderDrawColor(_SDLRenderer,255,255,255,255);
    SDL_RenderDrawRect(_SDLRenderer, &time_box);

    _SDLContext.renderText("TIME", {255, 255, 255}, _SDLContext._fontPs,
                           time_box.x+(time_box.w/2), time_box.y+4 );

    renderGameTime(time_box.x+(time_box.w/2), time_box.y+4+32);

    //Counter Box -- 2
    SDL_Rect counter_box;
    counter_box.x = BOARD0_X+BoardRenderer::BOARD_WIDTH;
    counter_box.y = BOARD0_Y + char_box.h + point_box.h + level_box.h + time_box.h;
    counter_box.w = INNER_WIDTH;
    counter_box.h = BoardRenderer::TILE_SIZE * 2;

    SDL_SetRenderDrawColor(_SDLRenderer,0,0,0,200);
    SDL_RenderFillRect(_SDLRenderer,&counter_box);
    
    SDL_SetRenderDrawColor(_SDLRenderer,255,255,255,255);
    SDL_RenderDrawRect(_SDLRenderer, &counter_box);

    //points
    // std::ostringstream os;
    // os << "\u25c0" << std::setw(2) << std::setfill('0') << _game.getP1Points()
    //    << ' ' << std::setw(2) << std::setfill('0') << _game.getP2Points()
    //    << "\u25b6";
    // _SDLContext.renderText(os.str(), {255, 255, 255}, _SDLContext._fontPs,
    //                        264, 180);
}

void VsGameRenderer::renderMatchPoints() {
//p1
    SDL_Rect sprite = {0, 361, 21, 21};
    SDL_Rect pos = {260, 35, 21, 21};
    for (int i = 0; i < VsGame::MATCH_POINTS; ++i) {
        if (_game.getP1MatchPoints() >= VsGame::MATCH_POINTS - i) {
            sprite.x = 21;
        } else {
            sprite.x = 0;
        }
        SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sprite, &pos);
        pos.y += 23;
    }
//p2
    pos = {359, 35, 21, 21};
    for (int i = 0; i < VsGame::MATCH_POINTS; ++i) {
        if (_game.getP2MatchPoints() >= VsGame::MATCH_POINTS - i) {
            sprite.x = 21;
        } else {
            sprite.x = 0;
        }
        SDL_RenderCopy(_SDLRenderer, _spriteSheet, &sprite, &pos);
        pos.y += 23;
    }
}

VsGameRenderer::~VsGameRenderer() {
    SDL_DestroyTexture(_bg);
    //SDL_DestroyTexture(_2pbg);
}

void VsGameRenderer::shakeBoard(int id, int duration) {
    if (id == 0) {
        _b0Shake = duration;
    } else if (id == 1) {
        _b1Shake = duration;
    }
}

void VsGameRenderer::handleShake() {
    if (_b0Shake > 0) {
        --_b0Shake;
    }
    if (_b1Shake > 0) {
        --_b1Shake;
    }
}

void VsGameRenderer::renderBoard(int id) {
    SDL_Rect pos;
    double shake;
    SDL_Texture *boardTexture;
    pos.h = BoardRenderer::BOARD_HEIGHT;
    pos.w = BoardRenderer::BOARD_WIDTH;

    if (id == 0) {
        pos.x = BOARD0_X;
        pos.y = BOARD0_Y;
        shake = _b0Shake;
        boardTexture = _boardRenderer0.renderBoard();
    } else {
        pos.x = BOARD1_X;
        pos.y = BOARD1_Y;
        shake = _b1Shake;
        boardTexture = _boardRenderer1.renderBoard();
    }

    int shakeOffset = .5 * shake * sin(shake);
    SDL_Rect src = {0, 0, BoardRenderer::BOARD_WIDTH,
                    BoardRenderer::BOARD_HEIGHT};

    if (shakeOffset >= 0) {
        pos.y += shakeOffset;
        pos.h -= shakeOffset;
        src.h -= shakeOffset;
    } else {
        pos.h += shakeOffset;
        src.y -= shakeOffset;
        src.h += shakeOffset;
    }



    SDL_SetRenderTarget(_SDLRenderer, _texture);

	SDL_SetRenderDrawColor(_SDLRenderer,0,0,0,200);
    SDL_RenderFillRect(_SDLRenderer,&pos);

    SDL_SetRenderDrawColor(_SDLRenderer,255,255,255,255);
    SDL_RenderDrawRect(_SDLRenderer, &pos);

    SDL_RenderCopy(_SDLRenderer, boardTexture, &src, &pos);

	

    // cout << "1" << endl;
    // vector<Point> points;
    // points.push_back(Point(pos.x,pos.y));
    // points.push_back(Point(pos.x,pos.y+pos.h));
    // points.push_back(Point(pos.x+pos.w,pos.y));
    // points.push_back(Point(pos.x+pos.w,pos.y+pos.h));

    // cout << "2" << endl;

    // PolygonShape ps(points);

    // cout << "3" << endl;

    // SDL_Color color = {.r = 255, .g = 255, .b = 255, .a = 255 };
    
    // cout << "4" << endl;

    // DrawFilledPolygon(ps,color,_SDLRenderer);

    // cout << "5" << endl;

    // SDL_RenderPresent(_SDLRenderer);

    
}
