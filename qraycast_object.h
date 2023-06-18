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

#ifndef QRAYCAST_OBJECT_H
#define QRAYCAST_OBJECT_H


#include "core/object.h"
#include "QuarkPhysics/qraycast.h""
#include "qbody_node.h"



class QRaycastObject: public Object{
    GDCLASS(QRaycastObject,Object);
protected:
    static void _bind_methods();
    QRaycast *raycastObject=nullptr;
    QWorldNode *worldNode;
    
public:
    QRaycastObject(){
        raycastObject=new QRaycast(QVector::Zero(),QVector::Zero(),false);
    };
    ~QRaycastObject(){
        if(raycastObject!=nullptr)
            delete raycastObject;
    };

    QRaycastObject * configure(Vector2 position, Vector2 ray_vector, bool enable_containing_bodies=false){
        set_position(position);
        set_ray_vector(ray_vector);
        set_containing_bodies_enabled(enable_containing_bodies);
    };
    
    Vector2 get_position();
    float get_rotation();
    Vector2 get_ray_vector();
    bool get_containing_bodies_enabled();

    QRaycastObject* set_position(Vector2 value);
    QRaycastObject* set_rotation(float value);
    QRaycastObject* set_ray_vector(Vector2 value);
    QRaycastObject* set_containing_bodies_enabled(bool value);

    Array get_contacts();
    
    friend class QWorldNode;
    friend class QRenderer;
};




#endif // QRAYCAST_OBJECT_H



