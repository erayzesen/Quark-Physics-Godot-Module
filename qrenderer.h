/************************************************************************************
 * MIT License
 *
 * Copyright (c) 2023 Eray Zesen
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * https://github.com/erayzesen/QuarkPhysics
 * https://github.com/erayzesen/Quark-Physics-Godot-Module
 *
**************************************************************************************/

#ifndef QRENDERER_H
#define QRENDERER_H
#include "core/color.h"

class QWorldNode;
class QRenderer
{

public:
    QRenderer(){};
    ~QRenderer(){};

    static void RenderDebug(QWorldNode *worldNode);

    static void RenderVector(QWorldNode *worldNode);

};

class QDebugColors{
    public:
        const Color COLLIDER_DYNAMIC=Color::named("limegreen"); //green
        const Color COLLIDER_DYNAMIC_SLEEPING=Color::named("seagreen"); //green
        const Color COLLIDER_STATIC=Color::named("deepskyblue"); //blue
        const Color SPRING=Color::named("darkgray"); // gray
        const Color SPRING_INTERNAL=Color::named("dimgray"); // dark gray
        const Color SPRING_EXTERNAL=Color::named("burlywood"); // burlywood
        const Color JOINT=Color::named("goldenrod"); // goldenrod
        const Color RAYCAST=Color::named("darksalmon"); // darksalmon
};










#endif // QRENDERER_H
