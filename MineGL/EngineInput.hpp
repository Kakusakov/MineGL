#pragma once

bool isKeyRegistered(int key);
void registerKey(int key);

bool getKeyDown(int key);
bool getKeyUp(int key);
bool getKeyHeld(int key);

bool getKey(int key);

float getTime();
float getDeltaTime();

float getMousePosX();
float getMousePosY();
float getMouseDeltaX();
float getMouseDeltaY();

void updateInput();
