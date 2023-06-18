#include "qmesh_node.h"
#include "qbody_node.h"
#include "qrenderer.h"
#include "core/math/triangulate.h"

void QMeshNode::_notification(int what) {
    switch (what){

    case NOTIFICATION_POST_ENTER_TREE:
        if(Engine::get_singleton()->is_editor_hint()==false){
            on_post_enter_tree();
        }else{
            if(useMeshData==true){
                update_mesh_data();
            }
        }
        break;
    case NOTIFICATION_DRAW:
        if(Engine::get_singleton()->is_editor_hint()==true){
            if(enableVectorRendering)
                vector_render_in_editor();
            debug_render_in_editor();
        }
        break;

    case NOTIFICATION_PROCESS:
        if(Engine::get_singleton()->is_editor_hint()==false)
            on_process();
        break;
    case NOTIFICATION_PARENTED:
        if(Engine::get_singleton()->is_editor_hint()==false && meshObject!=nullptr){
            QBodyNode *bodyNode=QBodyNode::type_cast(get_parent() );
            if(bodyNode!=nullptr){
                bodyNode->add_mesh_node(this);
                ownerBodyNode=bodyNode;
            }
        }
        break;
    case NOTIFICATION_UNPARENTED:
        if(Engine::get_singleton()->is_editor_hint()==false && meshObject!=nullptr){
            if(ownerBodyNode!=nullptr){
                ownerBodyNode->remove_mesh_node(this);
            }
        }
        break;
    case NOTIFICATION_PREDELETE:
        if(Engine::get_singleton()->is_editor_hint()==false && meshObject!=nullptr){
            if(ownerBodyNode!=nullptr){
                ownerBodyNode->remove_mesh_node(this);
            }
        }
        break;

    
    default:
        break;
    }
}



void QMeshNode::debug_render_in_editor() {
    QDebugColors colors;
    Color colliderColor=colors.COLLIDER_DYNAMIC;
   
    QBodyNode *parent_body=QBodyNode::type_cast(get_parent() );
    if(parent_body!=nullptr){
        colliderColor=parent_body->get_mode()==QBodyNode::Modes::STATIC ? colors.COLLIDER_STATIC:colliderColor;
    }

    //Draw springs
    if(enableSprings){
        //default springs
        for(int i=0;i<meshData.springList.size();i++){
            pair<int,int> spring=meshData.springList[i];
            QVector pA=meshData.particlePositions[spring.first];
            QVector pB=meshData.particlePositions[spring.second];
            draw_line(Vector2(pA.x,pA.y),Vector2(pB.x,pB.y),colors.SPRING);
        }
        //internal springs
        for(int i=0;i<meshData.internalSpringList.size();i++){
            pair<int,int> spring=meshData.internalSpringList[i];
            QVector pA=meshData.particlePositions[spring.first];
            QVector pB=meshData.particlePositions[spring.second];
            draw_line(Vector2(pA.x,pA.y),Vector2(pB.x,pB.y),colors.SPRING_INTERNAL);
        }
    }

    //Draw polygons
    if(enablePolygons){
        for(int i=0;i<meshData.closedPolygonList.size();i++){
            vector<int> polygon=meshData.closedPolygonList[i];
            for(int n=0;n<polygon.size();n++){
                QVector p=meshData.particlePositions[ polygon[n] ];
                QVector pn=meshData.particlePositions[polygon[(n+1)%polygon.size()] ];
                draw_line(Vector2(p.x,p.y),Vector2(pn.x,pn.y),colliderColor);
            }
        }
    }
    //Draw particles
    for(int i=0;i<meshData.particlePositions.size();i++){
        QVector particlePos=meshData.particlePositions[i];
        Vector2 pos=Vector2( particlePos.x,particlePos.y );
        float radius=meshData.particleRadValues[i];
        if(radius==0.5f){
            draw_circle(pos,radius,colliderColor);
        }else{
            int particleCount=round( (2*M_PI*radius)/2 );
            particleCount=max(particleCount,8);
            draw_arc(pos,radius,0,M_PI*2,particleCount,colliderColor,1.0f);
            if(meshData.particlePositions.size()==1 ){
                draw_line(pos,pos+Vector2(radius,0),colliderColor,1.0f );
            }
        }
    }

}

void QMeshNode::vector_render_in_editor() {

    Vector<Color> fillColors;
    fillColors.push_back(fillColor);
    Vector<Color> strokeColors;
    strokeColors.push_back(strokeColor);
    
    //draw polygons
    for(auto pg:meshData.closedPolygonList){
        Vector<Vector2> polygonPoints;
        for(int i=0;i<pg.size();i++){
            int pi=pg[i];
            QVector qpos=meshData.particlePositions[pi];
            polygonPoints.push_back( Vector2(qpos.x,qpos.y) );
        }
        if(polygonPoints.size()>0)
            polygonPoints.push_back(polygonPoints[0] );

        if(enableFill){
            if(enableTriangulation){
                Vector<int> triangles;
                Triangulate::triangulate(polygonPoints,triangles);
                //Geometry::decompose_polygon_in_convex(polygonPoints);
                for(int t=0;t<triangles.size();t+=3 ){
                    if( (t+1)>=triangles.size() && (t+2)>=triangles.size() )
						continue;
                    Vector<Vector2> tri;
                    int pi1=triangles[t];
                    int pi2=triangles[t+1];
                    int pi3=triangles[t+2];
                    tri.push_back( polygonPoints[pi1] );
                    tri.push_back( polygonPoints[pi2] );
                    tri.push_back( polygonPoints[pi3] );

                    Vector<int> tri_indexes;
					bool isValidTri=Triangulate::triangulate(tri,tri_indexes);
                    if(isValidTri)
                        draw_polygon(tri,fillColors,Vector<Vector2>(),Ref<Texture>(),Ref<Texture>(),enableAntialias);
                    
                }
            }else{
                draw_polygon(polygonPoints, fillColors,Vector<Vector2>(),Ref<Texture>(),Ref<Texture>(),enableAntialias);
            }
        }
        if(enableStroke)
            draw_polyline_colors(polygonPoints,strokeColors,strokeWidth,enableAntialias);
    }

    //draw particles
    for(int i=0;i<meshData.particlePositions.size();i++){
        float r=meshData.particleRadValues[i];
        if(r<=0.5)
            continue;
        QVector p=meshData.particlePositions[i];
        Vector2 pos=Vector2(p.x,p.y);
        int particleCount=round( (2*M_PI*r)/2 );
        particleCount=max(particleCount,8);
        if(enableFill)
            draw_circle(pos,r,fillColor);
        if(enableStroke)
            draw_arc(pos,r,0,M_PI*2,particleCount,strokeColor,strokeWidth,enableAntialias);
    }

}

void QMeshNode::on_process() {
	update();
}

void QMeshNode::on_post_enter_tree() {
    if(isConfigured==false){
        if(meshObject==nullptr){
            if(useMeshData==true){
                update_mesh_data();
                meshObject=QMesh::CreateWithMeshData(meshData);

                //Creating object and node versions of the mesh
                for(int i=0;i<meshObject->GetParticleCount();i++ ){ //particles
                    QParticleObject *p=new QParticleObject(meshObject->GetParticleAt(i));
                    particleObjects.push_back(p);
                }
                for(int i=0;i<meshObject->GetSpringCount();i++ ){ //springs
                    QSpringObject *s=new QSpringObject(meshObject->GetSpringAt(i));
                    springObjects.push_back(s);
                }
                for(int i=0;i<meshObject->GetClosedPolygonCount();i++ ){ //polygons
                    vector<QParticle*> originalPolygon=meshObject->GetClosedPolygonAt(i);
                    vector<QParticleObject*> polygon;
                    for(int n=0;n<originalPolygon.size();n++ ){
                        QParticleObject *p=get_particle_object_with_object(originalPolygon[n]);
                        polygon.push_back(p);
                    } 
                    closedPolygons.push_back(polygon);    
                }

            }else{
                meshObject=new QMesh();
            }
        }
        set_mesh_position(get_position());
        set_mesh_rotation(get_rotation());
        if(ownerBodyNode==nullptr){
            QBodyNode *bodyNode=QBodyNode::type_cast(get_parent());
            if(bodyNode!=nullptr){
                bodyNode->add_mesh_node(this);
                ownerBodyNode=bodyNode;
            }
        }

        set_process(true);

        isConfigured=true;
    }
    
}

void QMeshNode::_bind_methods() {
	//Get
    ClassDB::bind_method(D_METHOD("get_mesh_position"),&QMeshNode::get_mesh_position );
    ClassDB::bind_method(D_METHOD("get_mesh_global_position"),&QMeshNode::get_mesh_global_position );
    ClassDB::bind_method(D_METHOD("get_mesh_rotation"),&QMeshNode::get_mesh_rotation );
    ClassDB::bind_method(D_METHOD("get_mesh_global_rotation"),&QMeshNode::get_mesh_global_rotation );
    ClassDB::bind_method(D_METHOD("get_initial_area"),&QMeshNode::get_initial_area );
    ClassDB::bind_method(D_METHOD("get_initial_polygons_area"),&QMeshNode::get_initial_polygons_area );
    ClassDB::bind_method(D_METHOD("get_area"),&QMeshNode::get_area );
    ClassDB::bind_method(D_METHOD("get_polygons_area"),&QMeshNode::get_polygons_area );
    ClassDB::bind_method(D_METHOD("get_circumference"),&QMeshNode::get_circumference );
    ClassDB::bind_method(D_METHOD("get_owner_body_node"),&QMeshNode::get_owner_body_node );
    ClassDB::bind_method(D_METHOD("get_springs_enabled"),&QMeshNode::get_springs_enabled );
    ClassDB::bind_method(D_METHOD("get_polygons_enabled"),&QMeshNode::get_polygons_enabled );
    //Set
    ClassDB::bind_method(D_METHOD("set_mesh_position"),&QMeshNode::set_mesh_position );
    ClassDB::bind_method(D_METHOD("set_mesh_global_position"),&QMeshNode::set_mesh_global_position );
    ClassDB::bind_method(D_METHOD("set_mesh_rotation"),&QMeshNode::set_mesh_rotation );
    ClassDB::bind_method(D_METHOD("set_springs_enabled","value"),&QMeshNode::set_springs_enabled );
    ClassDB::bind_method(D_METHOD("set_polygons_enabled","value"),&QMeshNode::set_polygons_enabled );
    //Particle Operations
    ClassDB::bind_method(D_METHOD("add_particle","particle_object"),&QMeshNode::add_particle );
    ClassDB::bind_method(D_METHOD("remove_particle","particle_object"),&QMeshNode::remove_particle );
    ClassDB::bind_method(D_METHOD("remove_particle_at","index"),&QMeshNode::remove_particle_at );
    ClassDB::bind_method(D_METHOD("get_particle","index"),&QMeshNode::get_particle );
    ClassDB::bind_method(D_METHOD("get_particle_count"),&QMeshNode::get_particle_count );

    //Spring Operations
    ClassDB::bind_method(D_METHOD("add_spring","spring_object"),&QMeshNode::add_spring );
    ClassDB::bind_method(D_METHOD("remove_spring","spring_object"),&QMeshNode::remove_spring );
    ClassDB::bind_method(D_METHOD("remove_spring_at","index"),&QMeshNode::remove_spring_at );
    ClassDB::bind_method(D_METHOD("get_spring","index"),&QMeshNode::get_spring );
    ClassDB::bind_method(D_METHOD("get_spring_count"),&QMeshNode::get_spring_count );

    //Polygon Operations
    ClassDB::bind_method(D_METHOD("add_polygon","polygon"),&QMeshNode::add_polygon );
    ClassDB::bind_method(D_METHOD("remove_polygon_at","index"),&QMeshNode::remove_polygon_at );
    ClassDB::bind_method(D_METHOD("get_polygon_count"),&QMeshNode::get_polygon_count );
    ClassDB::bind_method(D_METHOD("get_polygon","index"),&QMeshNode::get_polygon );


    //Rendering features
     ClassDB::bind_method(D_METHOD("get_vector_rendering_enabled"),&QMeshNode::get_vector_rendering_enabled );
     ClassDB::bind_method(D_METHOD("get_antialias_enabled"),&QMeshNode::get_antialias_enabled );
     ClassDB::bind_method(D_METHOD("get_triangulation_enabled"),&QMeshNode::get_triangulation_enabled );
     ClassDB::bind_method(D_METHOD("get_fill_enabled"),&QMeshNode::get_fill_enabled );
     ClassDB::bind_method(D_METHOD("get_fill_color"),&QMeshNode::get_fill_color );
     ClassDB::bind_method(D_METHOD("get_stroke_enabled"),&QMeshNode::get_stroke_enabled );
     ClassDB::bind_method(D_METHOD("get_stroke_width"),&QMeshNode::get_stroke_width );
     ClassDB::bind_method(D_METHOD("get_stroke_color"),&QMeshNode::get_stroke_color );

     ClassDB::bind_method(D_METHOD("set_vector_rendering_enabled","value"),&QMeshNode::set_vector_rendering_enabled );
     ClassDB::bind_method(D_METHOD("set_antialias_enabled","value"),&QMeshNode::set_antialias_enabled );
     ClassDB::bind_method(D_METHOD("set_triangulation_enabled","value"),&QMeshNode::set_triangulation_enabled );
     ClassDB::bind_method(D_METHOD("set_fill_enabled","value"),&QMeshNode::set_fill_enabled );
     ClassDB::bind_method(D_METHOD("set_fill_color","value"),&QMeshNode::set_fill_color );
     ClassDB::bind_method(D_METHOD("set_stroke_enabled","value"),&QMeshNode::set_stroke_enabled );
     ClassDB::bind_method(D_METHOD("set_stroke_width","value"),&QMeshNode::set_stroke_width );
     ClassDB::bind_method(D_METHOD("set_stroke_color","value"),&QMeshNode::set_stroke_color );

    //Props
    ADD_PROPERTY( PropertyInfo(Variant::BOOL , "enable_springs"), "set_springs_enabled","get_springs_enabled" );
    ADD_PROPERTY( PropertyInfo(Variant::BOOL , "enable_polygons"), "set_polygons_enabled","get_polygons_enabled" );

    ADD_GROUP("Rendering","");
    ADD_PROPERTY( PropertyInfo(Variant::BOOL, "enable_vector_rendering"),"set_vector_rendering_enabled","get_vector_rendering_enabled" );
    ADD_PROPERTY( PropertyInfo(Variant::BOOL, "antialias"),"set_antialias_enabled","get_antialias_enabled" );
    ADD_PROPERTY( PropertyInfo(Variant::BOOL, "triangulation"),"set_triangulation_enabled","get_triangulation_enabled" );
    ADD_PROPERTY( PropertyInfo(Variant::BOOL, "enable_fill"),"set_fill_enabled","get_fill_enabled" );
    ADD_PROPERTY( PropertyInfo(Variant::COLOR, "fill_color"),"set_fill_color","get_fill_color" );
    ADD_PROPERTY( PropertyInfo(Variant::BOOL, "enable_stroke"),"set_stroke_enabled","get_stroke_enabled" );
    ADD_PROPERTY( PropertyInfo(Variant::REAL, "stroke_width",PROPERTY_HINT_EXP_RANGE,"0,9999,0.5"),"set_stroke_width","get_stroke_width" );
    ADD_PROPERTY( PropertyInfo(Variant::COLOR, "stroke_color"),"set_stroke_color","get_stroke_color" );
}
//GET

Vector2 QMeshNode::get_mesh_position() {
    QVector value=meshObject->GetPosition();
	return Vector2(value.x,value.y);
}

Vector2 QMeshNode::get_mesh_global_position() {
    QVector value=meshObject->GetGlobalPosition();
	return Vector2(value.x,value.y);
}

float QMeshNode::get_mesh_rotation() {
	return meshObject->GetRotation();
}

float QMeshNode::get_mesh_global_rotation() {
	return meshObject->GetGlobalRotation();
}

float QMeshNode::get_initial_area() {
	return meshObject->GetInitialArea();
}

float QMeshNode::get_initial_polygons_area() {
	return meshObject->GetInitialPolygonsArea();
}

float QMeshNode::get_area() {
	return meshObject->GetArea();
}

float QMeshNode::get_polygons_area() {
	return meshObject->GetPolygonsArea();
}

float QMeshNode::get_circumference(){
    return meshObject->GetCircumference();
}

bool QMeshNode::get_springs_enabled() {
	return enableSprings;
}

bool QMeshNode::get_polygons_enabled() {
	return enablePolygons;
}

QBodyNode * QMeshNode::get_owner_body_node(){
    return ownerBodyNode;
}

//SET

QMeshNode *QMeshNode::set_mesh_position(Vector2 value) {
    meshObject->SetPosition(QVector(value.x,value.y) );
	return this;
}
QMeshNode *QMeshNode::set_mesh_global_position(Vector2 value) {
    meshObject->SetGlobalPosition(QVector(value.x,value.y) );
	return this;
}

QMeshNode *QMeshNode::set_mesh_rotation(float value) {
    meshObject->SetRotation(value);
	return this;
}

QMeshNode *QMeshNode::set_springs_enabled(bool value) {
    enableSprings=value;
    update();
	return this;
}

QMeshNode *QMeshNode::set_polygons_enabled(bool value) {
	enablePolygons=value;
    update();
	return this;
}


//Particle Operations
QMeshNode *QMeshNode::add_particle(Object *particle_object) {
    if(particle_object->get_class()!="QParticleObject"){
        print_error("Quark Physics Error: Not a valid particle object type! | QMeshNode.add_particle() ");
        return this;
    }
    QParticleObject * qparticle_object=static_cast<QParticleObject*>(particle_object);
    particleObjects.push_back(qparticle_object);
    meshObject->AddParticle(qparticle_object->particleObject);
	return this;
}

QMeshNode *QMeshNode::remove_particle(Object *particle_object) {
    if(particle_object->get_class()!="QParticleObject"){
        print_error("Quark Physics Error: Not a valid particle object type! | QMeshNode.add_particle() ");
        return this;
    }
    QParticleObject * qparticle_object=static_cast<QParticleObject*>(particle_object);
    

    auto findedIt=find(particleObjects.begin(),particleObjects.end(),qparticle_object );
    if(findedIt!=particleObjects.end()){
        int index=findedIt-particleObjects.begin();
        remove_particle_at(index);
    }
	return this;
}

QMeshNode *QMeshNode::remove_particle_at(int index) {

    QParticleObject *particle_object=particleObjects[index];
    //Remove linked springs
    int i=0;
    while(i<springObjects.size()){
        QSpringObject *sp=springObjects[i];
        if(sp->get_particle_a()==particle_object || sp->get_particle_b()==particle_object){
            springObjects.erase(springObjects.begin()+i);
        }else{
            i++;
        }
    }
    //Remove linked polygons
    i=0;
    while(i<closedPolygons.size()){
        vector<QParticleObject*> &pol=closedPolygons[i];
        bool matched=false;
        int n=0;
        while(n<pol.size() ){
            if(pol[n]==particle_object){
                pol.erase(pol.begin()+n );
                matched=true;
            }else{
                ++n;
            }
        }
        if(matched==true && pol.size()<3){
            closedPolygons.erase(closedPolygons.begin()+i);
        }else{
            ++i;
        }
    }

    particleObjects.erase(particleObjects.begin()+index);
    meshObject->RemoveParticleAt(index);
	return this;
}

QParticleObject *QMeshNode::get_particle(int index) {
	return particleObjects[index];
}

int QMeshNode::get_particle_count() {
	return particleObjects.size();
}

QParticleObject *QMeshNode::get_particle_object_with_object(QParticle *particle_object) {
    for(auto particleObject:particleObjects){
        if(particleObject->particleObject==particle_object){
            return particleObject;
        }
    }
	return nullptr;
}

//Spring Operations
QMeshNode *QMeshNode::add_spring(Object *spring_object) {
	if(spring_object->get_class()!="QSpringObject"){
        print_error("Quark Physics Error: Not a valid spring object type! | QMeshNode.add_spring() ");
        return this;
    }
    QSpringObject * qspring_object=static_cast<QSpringObject*>(spring_object);
    springObjects.push_back(qspring_object);
    meshObject->AddSpring(qspring_object->springObject);
	return this;
}

QMeshNode *QMeshNode::remove_spring(Object *spring_object) {
	if(spring_object->get_class()!="QSpringObject"){
        print_error("Quark Physics Error: Not a valid particle object type! | QMeshNode.add_spring() ");
        return this;
    }
    QSpringObject * qspring_object=static_cast<QSpringObject*>(spring_object);
    auto findedIt=find(springObjects.begin(),springObjects.end(),qspring_object );
    if(findedIt!=springObjects.end()){
        int index=findedIt-springObjects.begin();
        remove_spring_at(index);
    }
	return this;
}

QMeshNode *QMeshNode::remove_spring_at(int index) {
	springObjects.erase(springObjects.begin()+index);
    meshObject->RemoveSpringAt(index);
	return this;
}

QSpringObject *QMeshNode::get_spring(int index) {
	return springObjects[index];
}

int QMeshNode::get_spring_count() {
	return springObjects.size();
}


//Polygon Operations
QMeshNode *QMeshNode::add_polygon(Array polygon) {
    //type check
    vector<QParticleObject*> polygonParticleObjects;
    vector<QParticle*> polygonParticles;

    for(int i=0;i<polygon.size();i++){
        if(polygon.get(i).get_type()!=Variant::OBJECT){
            print_error("Quark Physics Error: The element of the polygon array isn't a valid particle object type! | QMeshNode.add_polygon() ");
            return this;
        }
        Object *element=polygon.get(i);
        if(element->get_class()!="QParticleObject"){
            print_error("Quark Physics Error: The element of the polygon array isn't a valid particle object type! | QMeshNode.add_polygon() ");
            return this;
        }
        QParticleObject *particleObject=static_cast<QParticleObject*>(element);
        polygonParticleObjects.push_back(particleObject);
        polygonParticles.push_back(particleObject->particleObject);
    }
    closedPolygons.push_back(polygonParticleObjects);
    meshObject->AddClosedPolygon(polygonParticles);

	return this;
}

QMeshNode *QMeshNode::remove_polygon_at(int index) {
	closedPolygons.erase(closedPolygons.begin()+index);
    meshObject->RemoveClosedPolygonAt(index);
	return this;
}

Array QMeshNode::get_polygon(int index) {
	Array res;
    vector<QParticleObject*> &targetPolygon=closedPolygons[index];
    for(int i=0;i<targetPolygon.size();i++){
        res.append(targetPolygon[i]);
    }
    return res;
}

int QMeshNode::get_polygon_count() {
	return closedPolygons.size();
}

QMeshNode *QMeshNode::type_cast(Object *obj) {
	Node2D *node=Object::cast_to<Node2D>(obj);
    if(node!=nullptr){
        if(node->get_class()=="QMeshNode" || node->get_class()=="QMeshRectNode" || node->get_class()=="QMeshPolygonNode" || node->get_class()=="QMeshCircleNode" || node->get_class()=="QMeshExternalNode"){
            return static_cast<QMeshNode*>(node);
        }
    }
    return nullptr;
}

//PARTICLES





