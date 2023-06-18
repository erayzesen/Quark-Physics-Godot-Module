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

#ifndef QWORLD_NODE_H
#define QWORLD_NODE_H

#include "QuarkPhysics/qworld.h"
#include "QuarkPhysics/qbody.h"
#include "scene/2d/node_2d.h"
#include "qbody_node.h"
#include "qrigidbody_node.h"
#include "qjoint_object.h"
#include "qspring_object.h"
#include "qraycast_object.h"


class QWorldNode: public Node2D{
    GDCLASS(QWorldNode,Node2D);
    QWorld *worldObject;
    vector<QBodyNode*> bodyNodes;
    vector<QJointObject*> jointObjects;
    vector<QSpringObject*> springObjects;
    vector<QRaycastObject*> raycastObjects;
    bool enableDebugRenderer=true;
    void set_debug_renderer_enabled(bool value);
    bool get_debug_renderer_enabled();
protected:
    void _notification(int what);
    void on_process();
    void on_ready();
    static void _bind_methods();

public:
    QWorldNode(){
        worldObject=new QWorld();
        set_process(true);
    };
    ~QWorldNode(){
        if(worldObject!=nullptr)
            delete worldObject;
    };

    //Get Methods
    Vector2 get_gravity(){
        QVector g=worldObject->GetGravity();
        return Vector2(g.x,g.y);
    };
    bool get_enable_sleeping(){
        return worldObject->GetEnableSleeping();
    };
    bool get_enable_broadphase(){
        return worldObject->GetEnableBroadphase();
    };
    int get_iteration_count(){
        return worldObject->GetIterationCount();
    };
    float get_time_scale(){
        return worldObject->GetTimeScale();
    };

    //Set Methods
    QWorldNode *set_gravity(Vector2 value){
        worldObject->SetGravity(QVector(value.x,value.y) );
        return this;
    };
    QWorldNode *set_enable_sleeping(bool value){
        worldObject->SetEnableSleeping(value);
        return this;
    };
    QWorldNode *set_enable_broadphase(bool value){
        worldObject->SetEnableBroadphase(value);
        return this;
    };
    QWorldNode *set_iteration_count(int value){
        worldObject->SetIterationCount(value);
        return this;
    };
    QWorldNode *set_time_scale(float value){
        worldObject->SetTimeScale(value);
        return this;
    };

    //Collisions

    Array get_collisions(Object *bodyA, Object *bodyB);
    Array get_bodies_hit_by_point(Vector2 point,int max_body_count,bool only_rigid_bodies, int layers_bit=-1);
    Array get_particles_close_to_point(Vector2 point, float distance, int max_particle_count=1, bool except_rigid_bodies=true,int layers_bit=-1);

    //Collision Exceptions
    QWorldNode *add_collision_exception(Object *bodyA, Object *bodyB );
    QWorldNode *remove_collision_exception(Object *bodyA, Object *bodyB );
    QWorldNode *remove_matching_collision_exception(Object *body );


    


    //Body Operations
    QWorldNode *add_body(Object* body_node);
    QWorldNode *remove_body_node(Object* body_node);
    QWorldNode *remove_body_at(int index);
    QBodyNode* get_body_at(int index);
    int get_body_count();
    int get_body_index(Object *body_node);
    QBodyNode *get_body_node_with_object(QBody *body); //It's not for binding.

    //Joint Operations
    QWorldNode *add_joint(Object* joint_object);
    QWorldNode *remove_joint(Object* joint_object);
    QWorldNode *remove_joint_at(int index);
    int get_joint_count();
    QJointObject *get_joint_at(int index);
    int get_joint_index(Object* joint_object);

    //Spring Operations
    QWorldNode *add_spring(Object* spring_object);
    QWorldNode *remove_spring(Object* spring_object);
    QWorldNode *remove_spring_at(int index);
    int get_spring_count();
    QSpringObject *get_spring_at(int index);
    int get_spring_index(Object* spring_object);

    //Raycast Operations
    QWorldNode *add_raycast(Object* raycast_object);
    QWorldNode *remove_raycast(Object* raycast_object);
    QWorldNode *remove_raycast_at(int index);
    int get_raycast_count();
    QRaycastObject *get_raycast_at(int index);
    int get_raycast_index(Object* raycast_object);
    Array raycast_to(Vector2 position,Vector2 ray_vector,int collidable_layers_bit=-1,bool enable_containing_bodies=false);



    QWorldNode *update_world(){
        worldObject->Update();
        return this;
    }



    friend class QRenderer;




};


#endif // QWORLD_NODE_H
