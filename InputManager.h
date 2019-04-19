/*
 * InputManager.h
 *
 *  Created on: Aug 19, 2015
 *      Author: axel
 */

#ifndef INPUTMANAGER_H_
#define INPUTMANAGER_H_

#include <array>

class InputManager {
 public:
  static InputManager &getInstance();

  void poll();

  bool _quit;
  int _keys_len;
  const uint8_t *_keys;
  uint8_t *_prevKeys;

  bool keyDown(int); // key pressed down this poll
  bool anyKeyDown();

  bool keyUp(int);

  bool keyPressed(int); // key is down now
  int getKeyDown();

 private:
  InputManager();

  InputManager(InputManager const &) = delete;

  void operator=(InputManager const &) = delete;
};

#endif /* INPUTMANAGER_H_ */
