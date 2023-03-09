/***********************************************************************
 * Created by Peter Harrison on 22/12/2017.
 * Copyright (c) 2017 Peter Harrison
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without l> imitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/

#include "hardware.h"
#include "maze.h"
#include "sensors.h"
#include "mouse.h"
#include "motors.h"
#include "test.h"
#include "ui.h"
#include "streaming.h"






unsigned long eventInterval = 125; // in milliseconds
unsigned long eventTrigger;

static unsigned long boot_magic __attribute__((section(".noinit")));
int main(){
    mazeInit(emptyMaze);
    mouseState = FRESH_START;
    boot_magic = 0xFEEDBEEF;
    mazeFlood(GOAL);
    mouseInit();
    sensorsEnable();
    doButton();

    return 0;
}


