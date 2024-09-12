#include "qmesh_node.h"
#include "qbody_node.h"
#include "qrenderer.h"
#include "core/math/triangulate.h"
#include "core/math/geometry.h"

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
        
        for(int n=0;n<meshData.polygon.size();n++){
            QVector p=meshData.particlePositions[ meshData.polygon[n] ];
            QVector pn=meshData.particlePositions[meshData.polygon[(n+1)%meshData.polygon.size()] ];
            draw_line(Vector2(p.x,p.y),Vector2(pn.x,pn.y),colliderColor);
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
    
    Vector<Vector2> polygonPoints;
    for(int i=0;i<meshData.polygon.size();i++){
        int pi=meshData.polygon[i];
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
                
                int polygonParticleCount=meshObject->GetPolygonParticleCount(); //poltgon
                for(int n=0;n<polygonParticleCount;n++ ){
                    QParticleObject *p=get_particle_object_with_object(meshObject->GetParticleFromPolygon(n));
                    polygon.push_back(p);
                } 
                vector<QParticleObject*> polygon;
                
                    
                

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
    ClassDB::bind_method(D_METHOD("get_polygon_area"),&QMeshNode::get_polygon_area );
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
    ClassDB::bind_method(D_METHOD("get_average_position_and_rotation","particle_collection"),&QMeshNode::get_average_position_and_rotation );
    ClassDB::bind_method(D_METHOD("get_matching_particle_positions","particle_collection","target_position","target_rotation"),&QMeshNode::get_matching_particle_positions );

    //Spring Operations
    ClassDB::bind_method(D_METHOD("add_spring","spring_object"),&QMeshNode::add_spring );
    ClassDB::bind_method(D_METHOD("remove_spring","spring_object"),&QMeshNode::remove_spring );
    ClassDB::bind_method(D_METHOD("remove_spring_at","index"),&QMeshNode::remove_spring_at );
    ClassDB::bind_method(D_METHOD("get_spring","index"),&QMeshNode::get_spring );
    ClassDB::bind_method(D_METHOD("get_spring_count"),&QMeshNode::get_spring_count );

    //Polygon Operations
    ClassDB::bind_method(D_METHOD("set_polygon","particleCollection"),&QMeshNode::set_polygon );
    ClassDB::bind_method(D_METHOD("add_particle_to_polygon","particleObject"),&QMeshNode::add_particle_to_polygon );
    ClassDB::bind_method(D_METHOD("remove_particle_from_polygon","particleObject"),&QMeshNode::remove_particle_from_polygon );
    ClassDB::bind_method(D_METHOD("remove_particle_from_polygon_at","index"),&QMeshNode::remove_particle_from_polygon_at );
    ClassDB::bind_method(D_METHOD("remove_polygon"),&QMeshNode::remove_polygon );
    ClassDB::bind_method(D_METHOD("get_particle_from_polygon","index"),&QMeshNode::get_particle_from_polygon );
    ClassDB::bind_method(D_METHOD("get_polygon_particle_count"),&QMeshNode::get_polygon_particle_count );
    ClassDB::bind_method(D_METHOD("get_min_angle_constraint_of_polygon"),&QMeshNode::get_min_angle_constraint_of_polygon );
    ClassDB::bind_method(D_METHOD("set_min_angle_constraint_of_polygon","value"),&QMeshNode::set_min_angle_constraint_of_polygon );
    ClassDB::bind_method(D_METHOD("get_decomposited_polygon"),&QMeshNode::get_decomposited_polygon );


    

    //Rendering features
     ClassDB::bind_method(D_METHOD("get_vector_rendering_enabled"),&QMeshNode::get_vector_rendering_enabled );
     ClassDB::bind_method(D_METHOD("get_antialias_enabled"),&QMeshNode::get_antialias_enabled );
     ClassDB::bind_method(D_METHOD("get_triangulation_enabled"),&QMeshNode::get_triangulation_enabled );
     ClassDB::bind_method(D_METHOD("get_fill_enabled"),&QMeshNode::get_fill_enabled );
     ClassDB::bind_method(D_METHOD("get_fill_color"),&QMeshNode::get_fill_color );
     ClassDB::bind_method(D_METHOD("get_stroke_enabled"),&QMeshNode::get_stroke_enabled );
     ClassDB::bind_method(D_METHOD("get_stroke_width"),&QMeshNode::get_stroke_width );
     ClassDB::bind_method(D_METHOD("get_stroke_color"),&QMeshNode::get_stroke_color );

     //Render helper operations
     ClassDB::bind_method(D_METHOD("get_curved_polygon_points","curve_amount","margin","origin","bake_interval"),&QMeshNode::get_curved_polygon_points );
     ClassDB::bind_method(D_METHOD("get_inner_shadow_of_polygon","polygon_points","offset"),&QMeshNode::get_inner_shadow_of_polygon );
     ClassDB::bind_method(D_METHOD("get_specified_side_points_of_polygon","polygon_points","axis","points_count_range","scale", "origin"),&QMeshNode::get_specified_side_points_of_polygon );

     

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

float QMeshNode::get_polygon_area() {
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

Array QMeshNode::get_average_position_and_rotation(Array particle_collection) {
    Array result;
    vector<QParticle*> particleObjects;
    for(int i=0;i<particle_collection.size();i++){
        if(particle_collection.get(i).get_type()!=Variant::OBJECT){
            print_error("Quark Physics Error: The element of the polygon array isn't a valid particle object type! | QMeshNode.add_polygon() ");
            return result;
        }
        Object *element=particle_collection.get(i);
        if(element->get_class()!="QParticleObject"){
            print_error("Quark Physics Error: The element of the polygon array isn't a valid particle object type! | QMeshNode.add_polygon() ");
            return result;
        }
        QParticleObject *pObj=static_cast<QParticleObject*>(element);
        if(pObj!=nullptr){
            particleObjects.push_back(pObj->particleObject);
        }
    }
    pair<QVector,float> resultRaw=meshObject->GetAveragePositionAndRotation(particleObjects);
    result.append(Vector2(resultRaw.first.x,resultRaw.first.y ));
    result.append(resultRaw.second);
    
	return result;
}

Array QMeshNode::get_matching_particle_positions(Array particle_collection, Vector2 target_position, float target_rotation) {
	Array result;
    vector<QParticle*> particleObjects;
    for(int i=0;i<particle_collection.size();i++){
        if(particle_collection.get(i).get_type()!=Variant::OBJECT){
            print_error("Quark Physics Error: The element of the polygon array isn't a valid particle object type! | QMeshNode.add_polygon() ");
            return result;
        }
        Object *element=particle_collection.get(i);
        if(element->get_class()!="QParticleObject"){
            print_error("Quark Physics Error: The element of the polygon array isn't a valid particle object type! | QMeshNode.add_polygon() ");
            return result;
        }
        QParticleObject *pObj=static_cast<QParticleObject*>(element);
        if(pObj!=nullptr){
            particleObjects.push_back(pObj->particleObject);
        }
    }
    vector<QVector> resultRaw=meshObject->GetMatchingParticlePositions(particleObjects,QVector(target_position.x,target_position.y),target_rotation );
    for(int i=0;i<resultRaw.size();++i ){
        result.append(Vector2(resultRaw[i].x,resultRaw[i].y) );
    }
    return result;
}

float QMeshNode::get_min_angle_constraint_of_polygon() {
	return meshObject->GetMinAngleConstraintOfPolygon();
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

QMeshNode *QMeshNode::set_min_angle_constraint_of_polygon(bool value) {
	meshObject->SetMinAngleConstraintOfPolygon(value);
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
    
    
    vector<QParticleObject*> &pol=polygon;
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
        polygon.clear();
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

QMeshNode *QMeshNode::set_polygon(Array particleCollection) {

	vector<QParticleObject*> polygonParticleObjects;
    vector<QParticle*> polygonParticles;

    for(int i=0;i<particleCollection.size();i++){
        if(particleCollection.get(i).get_type()!=Variant::OBJECT){
            print_error("Quark Physics Error: The element of the polygon array isn't a valid particle object type! | QMeshNode.add_polygon() ");
            return this;
        }
        Object *element=particleCollection.get(i);
        if(element->get_class()!="QParticleObject"){
            print_error("Quark Physics Error: The element of the polygon array isn't a valid particle object type! | QMeshNode.add_polygon() ");
            return this;
        }
        QParticleObject *particleObject=static_cast<QParticleObject*>(element);
        polygonParticleObjects.push_back(particleObject);
        polygonParticles.push_back(particleObject->particleObject);
    }

    polygon=polygonParticleObjects;
    meshObject->SetPolygon(polygonParticles);

    return  this;
}

QMeshNode *QMeshNode::add_particle_to_polygon(Object *particleObject) {

    
    if(particleObject->get_class()!="QParticleObject"){
        print_error("Quark Physics Error: There is no a valid particle object type! | QMeshNode.add_particle_to_polygon() ");
        return this;
    }
    QParticleObject *po=static_cast<QParticleObject*>(particleObject);
    polygon.push_back(po);
    meshObject->AddParticleToPolygon(po->particleObject);

	return this;
}

QMeshNode *QMeshNode::remove_particle_from_polygon(Object *particleObject) {
    if(particleObject->get_class()!="QParticleObject"){
        print_error("Quark Physics Error: There is no a valid particle object type! | QMeshNode.remove_particle_from_polygon() ");
        return this;
    }
    QParticleObject *po=static_cast<QParticleObject*>(particleObject);

    auto findedIt=find(polygon.begin(),polygon.end(),po );
    if(findedIt!=polygon.end() ){
        int index=findedIt-polygon.begin();
        remove_particle_from_polygon_at(index);
    }


	return this;
}

QMeshNode *QMeshNode::remove_particle_from_polygon_at(int index) {
    if(index>=polygon.size() ){
        print_error("Quark Physics Error: the index value is out of the range! | QMeshNode.remove_particle_from_polygon_at() ");
        return this;
    }
    if(polygon.size()==3){
        print_error("Quark Physics Warning: The number of particles of the polygon has been less than 3! | QMeshNode.remove_particle_from_polygon_at() ");
        return this;
    }
    QParticleObject *po=static_cast<QParticleObject*>(polygon[index] );
    meshObject->RemoveParticleFromPolygonAt(index);
    polygon.erase(polygon.begin()+index );
    
	return this;
}

QMeshNode *QMeshNode::remove_polygon() {
    meshObject->RemovePolygon();
    polygon.clear();
	return this;
}

QParticleObject *QMeshNode::get_particle_from_polygon(int index) {
    if(index>=polygon.size() ){
        print_error("Quark Physics Error: the index value is out of the range! | QMeshNode.get_particle_from_polygon() ");
        return nullptr;
    }
	return polygon[index];
}

int QMeshNode::get_polygon_particle_count() {
	return polygon.size() ;
}

Array QMeshNode::get_decomposited_polygon() {
    Vector<Vector2> polygonPoints;
    for (size_t i=0;i<meshObject->GetPolygonParticleCount();++i ){
        QVector p=meshObject->GetParticleFromPolygon(i)->GetGlobalPosition();
        polygonPoints.push_back( Vector2(p.x,p.y) );

    }

    Vector<Vector<Vector2>> decompositedPolygon=Geometry::decompose_polygon_in_convex(polygonPoints);

    Array result;

    for(int i=0;i<decompositedPolygon.size();++i ){
        Vector<Vector2> polyOrg=decompositedPolygon.get(i);
        Array poly;
        for(size_t n=0;n<polyOrg.size();++n ){
            poly.append(polyOrg[n]);
        }
        result.append(poly);
    }

	return result;
}

PoolVector2Array QMeshNode::get_curved_polygon_points(float curve_amount,float margin,Vector2 origin,float bake_interval) {
    PoolVector2Array res;
    size_t polygonSize=polygon.size();
    if(polygonSize<3 ){
        return res;
    }
    curvedPolygon->clear_points();
    Vector2 firstPoint;
    Vector2 firstCurveVec;
    curvedPolygon->set_bake_interval(bake_interval);
	for (size_t i=0;i<polygonSize;++i ){
        Vector2 pp=polygon[ (i-1+polygonSize) % polygonSize ]->get_global_position();
        Vector2 cp=polygon[i]->get_global_position();
        Vector2 np=polygon[ (i+1) % polygonSize ]->get_global_position();
        Vector2 curveUnit=(np-pp).normalized();
        Vector2 curveNormal=curveUnit.tangent();
        Vector2 curveVec=curveUnit*curve_amount;

        Vector2 finalPoint=(cp-origin)-curveNormal*margin; 

        if(i==0){
            firstPoint=finalPoint;
            firstCurveVec=curveVec;
        }

        curvedPolygon->add_point(finalPoint,-curveVec,curveVec);
    }
    curvedPolygon->add_point(firstPoint,-firstCurveVec,firstCurveVec);
    
    return curvedPolygon->get_baked_points();
}

Vector<Vector2> QMeshNode::get_inner_shadow_of_polygon(Vector<Vector2> polygon_points,Vector2 offset) {
    Vector<Vector2> clipVertices;
    for (size_t i=0;i<polygon_points.size();++i ){
        clipVertices.push_back(polygon_points[i]+offset);
    }
    
    Vector<Vector<Vector2> > clippedPolygons=Geometry::clip_polygons_2d(polygon_points,clipVertices);
    Vector<Vector2> shadowVertices;
    if (clippedPolygons.size()>0 ){
        shadowVertices=clippedPolygons[0];
    }

	return shadowVertices;
}

Vector<Vector2> QMeshNode::get_specified_side_points_of_polygon(Vector<Vector2> polygon_points, Vector2 axis, int points_count_range, float scale,Vector2 origin) {
    Vector<Vector2> res;
    size_t p_size=polygon_points.size();
    if (p_size<3 ){
        return res;
    }
    size_t extreme_point_index;
    float max_dist=-INFINITY;
    for (size_t i=0;i<p_size;++i ){
        Vector2 p=polygon_points[i];
        float dist=p.dot(axis);
        if(dist>max_dist){
            max_dist=dist;
            extreme_point_index=i;
        }
    }
    size_t current_index=(extreme_point_index-points_count_range+p_size)%p_size;
    size_t end_index=(extreme_point_index+points_count_range)%p_size;
    while (current_index!=end_index){
        Vector2 p=( polygon_points[current_index]-origin )*scale+origin;
        res.push_back(p);
        current_index=(current_index+1)%p_size;
    }

	return res;
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





