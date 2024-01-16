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

#ifndef QMESH_NODE_H
#define QMESH_NODE_H

#include "scene/2d/node_2d.h"
#include "QuarkPhysics/qmesh.h"
#include "qparticle_object.h"
#include "qspring_object.h"
#include "core/engine.h"

class QBodyNode;
class QMeshNode: public Node2D{
    GDCLASS(QMeshNode,Node2D);
protected:
    bool enableSprings=true;
    bool enablePolygons=true;
    float particleRadius=0.5f;
    QMesh::MeshData meshData;
    QMesh *meshObject=nullptr;
    QBodyNode *ownerBodyNode=nullptr;
    virtual void update_mesh_data(){};
    static void _bind_methods();
    bool useMeshData=false;
    bool isConfigured=false;

    //Render Features
    bool enableVectorRendering=false;
    bool enableAntialias=true;
    bool enableTriangulation=false;
    bool enableFill=true;
    Color fillColor=Color::named("white");
    bool enableStroke=false;
    float strokeWidth=3.0f;
    Color strokeColor=Color::named("black");


    //Collections
    vector<QParticleObject*> particleObjects;
    vector<QSpringObject*> springObjects;
    vector<QParticleObject*> polygon;

    void debug_render_in_editor();
    void vector_render_in_editor();
    void on_process();
    void on_post_enter_tree();
    void _notification(int what);
public:
    QMeshNode(){
       
    };
    ~QMeshNode(){
        if(meshObject!=nullptr)
            delete meshObject;
    }


    

    //Get Methods
    Vector2 get_mesh_position();
    Vector2 get_mesh_global_position();

    float get_mesh_rotation();
    float get_mesh_global_rotation();
    float get_initial_area();
    float get_initial_polygons_area();
    float get_area();
    float get_polygons_area();
    float get_circumference();
    bool get_springs_enabled();
    bool get_polygons_enabled();

    QBodyNode *get_owner_body_node();

    //Set Methods
    QMeshNode *set_mesh_position(Vector2 value);
    QMeshNode *set_mesh_global_position(Vector2 value);
    QMeshNode *set_mesh_rotation(float value);
    QMeshNode *set_springs_enabled(bool value);
    QMeshNode *set_polygons_enabled(bool value);
    

    //Particle Operations
    QMeshNode *add_particle(Object *particle_object);
    QMeshNode *remove_particle(Object *particle_object);
    QMeshNode *remove_particle_at(int index);
    QParticleObject *get_particle(int index);
    int get_particle_count();

    QParticleObject *get_particle_object_with_object(QParticle *particle_object);


    //Spring Operations
    QMeshNode *add_spring(Object *spring_object);
    QMeshNode *remove_spring(Object *spring_object);
    QMeshNode *remove_spring_at(int index);
    QSpringObject *get_spring(int index);
    int get_spring_count();

    //Polygon Operations
    QMeshNode *set_polygon(Array particleCollection);
    QMeshNode *add_particle_to_polygon(Object *particleObject);
    QMeshNode *remove_particle_from_polygon(Object *particleObject);
    QMeshNode *remove_particle_from_polygon_at(int index);
    QMeshNode *remove_polygon();
    QParticleObject *get_particle_from_polygon(int index);
    int get_polygon_particle_count();

    //Rendering Features
    bool get_vector_rendering_enabled(){
        return enableVectorRendering;
    }

    bool get_antialias_enabled(){
        return enableAntialias;
    }

    bool get_triangulation_enabled(){
        return enableTriangulation;
    }

    bool get_fill_enabled(){
        return enableFill;
    }

    Color get_fill_color(){
        return fillColor;
    }

    bool get_stroke_enabled(){
        return enableStroke;
    }

    float get_stroke_width(){
        return strokeWidth;
    }

    Color get_stroke_color(){
        return strokeColor;
    }

    QMeshNode *set_vector_rendering_enabled(bool value){
        enableVectorRendering=value;
        update();
        return this;
    }

    QMeshNode *set_antialias_enabled(bool value){
        enableAntialias=value;
        update();
        return this;
    }

    QMeshNode *set_triangulation_enabled(bool value){
        enableTriangulation=value;
        update();
        return this;
    }

    QMeshNode *set_fill_enabled(bool value){
        enableFill=value;
        update();
        return this;
    }

    QMeshNode *set_fill_color(Color value){
        fillColor=value;
        update();
        return this;
    }

    QMeshNode *set_stroke_width(float value){
        strokeWidth=value;
        update();
        return this;
    }

    QMeshNode *set_stroke_enabled(bool value){
        enableStroke=value;
        update();
        return this;
    }

    QMeshNode *set_stroke_color(Color value){
        strokeColor=value;
        update();
        return this;
    }

    //Type casting

    static QMeshNode *type_cast(Object *obj);

    


    friend class QBodyNode;
    friend class QWorldNode;
    friend class QRenderer;

};

#endif // QMESH_NODE_H
