/*
 * BoardController.cpp
 *
 *  Created on: 9.12.2015
 *      Author: axelw
 */

#include "BoardController.h"
#include "../debug.h"

BoardController::BoardController(Board &b) : _board(b) {
	debug_print("BoardController created");
}

BoardController::~BoardController() {
}

