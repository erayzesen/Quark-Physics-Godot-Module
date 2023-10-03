#include "qworld_node.h"
#include "core/engine.h"
#include "qrenderer.h"



void QWorldNode::_notification(int what) {
	switch (what)
    {
    case NOTIFICATION_READY:
		if(Engine::get_singleton()->is_editor_hint()==false)
        	on_ready();
        break;
    case NOTIFICATION_PROCESS:
		if(Engine::get_singleton()->is_editor_hint()==false)
        	on_process();
		break;
	
	case NOTIFICATION_DRAW:
		if(Engine::get_singleton()->is_editor_hint()==false){
			QRenderer::RenderVector(this);
			if(enableDebugRenderer)
        		QRenderer::RenderDebug(this);
				
		}
		break;
	case NOTIFICATION_PREDELETE :
		
		break;
    
    default:
        break;
    }
}
Array QWorldNode::get_collisions(Object *bodyA, Object *bodyB) {
	Array res;
	QBodyNode *qbodyA=QBodyNode::type_cast(bodyA);
	if(qbodyA==nullptr){
		print_error("Quark Physics Error: Not valid body type : bodyA | QWorldNode.get_collisions() ");
		return res;
	}
	QBodyNode *qbodyB=QBodyNode::type_cast(bodyB);
	if(qbodyB==nullptr){
		print_error("Quark Physics Error: Not valid body type : bodyB | QWorldNode.get_collisions() ");
		return res;
	}
	auto contacts=worldObject->GetCollisions(qbodyA->bodyObject,qbodyB->bodyObject);
	for(int i=0;i<contacts.size();i++){
		QCollision::Contact oc=contacts[i];
		Dictionary c;
		c["position"]=Vector2(oc.position.x,oc.position.y);
		c["normal"]=Vector2(oc.normal.x,oc.normal.y);
		c["penetration"]=oc.penetration;
		QBodyNode *particleBodyNode=get_body_node_with_object(oc.particle->GetOwnerMesh()->GetOwnerBody() );
		QMeshNode *particleMeshNode=particleBodyNode->get_mesh_node_with_object( oc.particle->GetOwnerMesh() );
		c["particle"]=particleMeshNode->get_particle_object_with_object(oc.particle);
		Array refParticles;
		for(int n=0;n<oc.referenceParticles.size();n++){
			QParticle *p=oc.referenceParticles[n];
			QBodyNode *pBodyNode=get_body_node_with_object(p->GetOwnerMesh()->GetOwnerBody() );
			QMeshNode *pMeshNode=pBodyNode->get_mesh_node_with_object( p->GetOwnerMesh() );
			QParticleObject *pObj=pMeshNode->get_particle_object_with_object(p);
			refParticles.append( pObj );
		}
		c["reference_particles"]=refParticles;
		
		res.append(c);
		
	}
	return res;
}

Array QWorldNode::get_bodies_hit_by_point(Vector2 point, int max_body_count, bool only_rigid_bodies, int layers_bit) {
	Array res;
	vector<QBody *> bodyList=worldObject->GetBodiesHitByPoint(QVector(point.x,point.y),max_body_count,only_rigid_bodies,layers_bit );
	for(int i=0;i<bodyList.size();i++){
		QBodyNode *bn=get_body_node_with_object(bodyList[i]);
		res.append(bn);
	}
	return res;
}

Array QWorldNode::get_particles_close_to_point(Vector2 point, float distance, int max_particle_count, bool except_rigid_bodies, int layers_bit) {
	Array res;
	vector<QParticle *> particleList=worldObject->GetParticlesCloseToPoint(QVector(point.x,point.y),distance,max_particle_count,except_rigid_bodies,layers_bit);
	for(int i=0;i<particleList.size();i++){
		QParticle *p=particleList[i];
		QBodyNode *pBodyNode=get_body_node_with_object(p->GetOwnerMesh()->GetOwnerBody() );
		QMeshNode *pMeshNode=pBodyNode->get_mesh_node_with_object( p->GetOwnerMesh() );
		QParticleObject *pObj=pMeshNode->get_particle_object_with_object(p);
		res.append(pObj);
	}
	return res;
}

QWorldNode *QWorldNode::add_collision_exception(Object *bodyA, Object *bodyB) {
	QBodyNode *qbodyA=QBodyNode::type_cast(bodyA);
	QBodyNode *qbodyB=QBodyNode::type_cast(bodyB);
	if(qbodyA==nullptr || qbodyB==nullptr){
		print_error("Quark Physics Error: Not valid body type! | QWorldNode.add_collision_exception() ");
		return this;
	}
	worldObject->AddCollisionException(qbodyA->bodyObject,qbodyB->bodyObject);
	return this;
}

QWorldNode *QWorldNode::remove_collision_exception(Object *bodyA, Object *bodyB) {
	QBodyNode *qbodyA=QBodyNode::type_cast(bodyA);
	QBodyNode *qbodyB=QBodyNode::type_cast(bodyB);
	if(qbodyA==nullptr || qbodyB==nullptr){
		print_error("Quark Physics Error: Not valid body type! | QWorldNode.remove_collision_exception() ");
		return this;
	}
	worldObject->RemoveCollisionException(qbodyA->bodyObject,qbodyB->bodyObject);
	return this;
}

QWorldNode *QWorldNode::remove_matching_collision_exception(Object *body) {
	QBodyNode *qbody=QBodyNode::type_cast(body);
	if(qbody==nullptr){
		print_error("Quark Physics Error: Not valid body type! | QWorldNode.remove_matching_collision_exception() ");
		return this;
	}
	worldObject->RemoveMatchingCollisionException(qbody->bodyObject);
	return this;
}

QWorldNode *QWorldNode::add_body(Object *body_node) {
	QBodyNode *qbody_node=QBodyNode::type_cast(body_node);
	if(qbody_node==nullptr){
		print_error("Quark Physics Error: Not valid body type! | QWorldNode.add_body() ");
		return this;
	}
	worldObject->AddBody(qbody_node->bodyObject);
	bodyNodes.push_back(qbody_node);
	qbody_node->worldNode=this;
	//print_line("The Body Added!");
	return this;
}

QWorldNode *QWorldNode::remove_body_node(Object *body_node) {
	int index=get_body_index(body_node);
	if(index==-1)
		return this;
	remove_body_at(index);
	return this;
}

QWorldNode *QWorldNode::remove_body_at(int index) {
	if(index>=bodyNodes.size()){
		print_error("Quark Physics Error: Out of the size! | QWorldNode.remove_body_at() ");
		return this;
	}
	QBodyNode *bodyNode=get_body_at(index);
	if (worldObject!=nullptr){
		worldObject->RemoveBody(bodyNode->bodyObject);
	}
	bodyNodes.erase(bodyNodes.begin()+index);

	return this;
}

QBodyNode *QWorldNode::get_body_at(int index) {
	return bodyNodes[index];
}

int QWorldNode::get_body_count() {
	return bodyNodes.size();
}

int QWorldNode::get_body_index(Object *body_node) {
	QBodyNode *qbody_node=QBodyNode::type_cast(body_node);
	int index=-1;
	if(qbody_node==nullptr){
		print_error("Quark Physics Error: Not valid body type! | QWorldNode.get_body_index() ");
		return index;
	}
	auto it=find(bodyNodes.begin(),bodyNodes.end(),qbody_node);
	if (it!=bodyNodes.end() ){
		index=it-bodyNodes.begin();
	}
	return index;
}


QBodyNode *QWorldNode::get_body_node_with_object(QBody *body) {
	for(auto b:bodyNodes){
		if(b->bodyObject==body)
			return b;
	}
	return nullptr;
}

//Joints

QWorldNode *QWorldNode::add_joint(Object *joint_object) {
	if(joint_object==nullptr){
		print_error("Quark Physics Error: Not valid joint type! | QWorldNode.add_joint() ");
		return this;
	}
	if(joint_object->get_class()!="QJointObject"){
		print_error("Quark Physics Error: Not valid joint type! | QWorldNode.add_joint() ");
		return this;
	}
	QJointObject *qjoint_object=static_cast<QJointObject*>(joint_object);
	jointObjects.push_back(qjoint_object);
	worldObject->AddJoint(qjoint_object->jointObject);
	return this;
}

QWorldNode *QWorldNode::remove_joint(Object *joint_object) {
	int index=get_joint_index(joint_object);
	if(index==-1){
		print_error("Quark Physics Error: Out of the size! | QWorldNode.remove_joint() ");
		return this;
	}
	remove_joint_at(index);
	return this;
}

QWorldNode *QWorldNode::remove_joint_at(int index) {
	worldObject->RemoveJointAt(index);
	jointObjects.erase(jointObjects.begin()+index);
	return this;
}


int QWorldNode::get_joint_count() {
	return jointObjects.size();
}

QJointObject *QWorldNode::get_joint_at(int index) {
	return jointObjects[index];
}

int QWorldNode::get_joint_index(Object *joint_object) {
	if(joint_object==nullptr){
		print_error("Quark Physics Error: Not valid joint type! | QWorldNode.add_joint() ");
		return -1;
	}
	if(joint_object->get_class()!="QJointObject"){
		print_error("Quark Physics Error: Not valid joint type! | QWorldNode.add_joint() ");
		return -1;
	}
	QJointObject *qjoint_object=static_cast<QJointObject*>(joint_object);
	auto it=find(jointObjects.begin(),jointObjects.end(),qjoint_object);
	int index=it-jointObjects.begin();
	return index;
}

//Springs
QWorldNode *QWorldNode::add_spring(Object *spring_object) {
	if(spring_object==nullptr){
		print_error("Quark Physics Error: Not valid spring type! | QWorldNode.add_spring() ");
		return this;
	}
	if(spring_object->get_class()!="QSpringObject"){
		print_error("Quark Physics Error: Not valid spring type! | QWorldNode.add_spring() ");
		return this;
	}
	QSpringObject *qspring_object=static_cast<QSpringObject*>(spring_object);
	springObjects.push_back(qspring_object);
	worldObject->AddSpring(qspring_object->springObject);
	return this;
}

QWorldNode *QWorldNode::remove_spring(Object *spring_object) {
	int index=get_spring_index(spring_object);
	if(index==-1){
		print_error("Quark Physics Error: Out of the size! | QWorldNode.remove_spring() ");
		return this;
	}
	remove_spring_at(index);
	return this;
}

QWorldNode *QWorldNode::remove_spring_at(int index) {
	worldObject->RemoveSpringAt(index);
	springObjects.erase(springObjects.begin()+index);
	return this;
}


int QWorldNode::get_spring_count() {
	return springObjects.size();
}

QSpringObject *QWorldNode::get_spring_at(int index) {
	return springObjects[index];
}

int QWorldNode::get_spring_index(Object *spring_object) {
	if(spring_object==nullptr){
		print_error("Quark Physics Error: Not valid spring type! | QWorldNode.get_spring_index() ");
		return -1;
	}
	if(spring_object->get_class()!="QSpringObject"){
		print_error("Quark Physics Error: Not valid spring type! | QWorldNode.get_spring_index() ");
		return -1;
	}
	QSpringObject *qspring_object=static_cast<QSpringObject*>(spring_object);
	auto it=find(springObjects.begin(),springObjects.end(),qspring_object);
	int index=it-springObjects.begin();
	return index;
}

//Raycasts
QWorldNode *QWorldNode::add_raycast(Object *raycast_object) {
	if(raycast_object==nullptr){
		print_error("Quark Physics Error: Not valid raycast type! | QWorldNode.add_raycast() ");
		return this;
	}
	if(raycast_object->get_class()!="QRaycastObject"){
		print_error("Quark Physics Error: Not valid raycast type! | QWorldNode.add_raycast() ");
		return this;
	}
	QRaycastObject *qraycast_object=static_cast<QRaycastObject*>(raycast_object);
	raycastObjects.push_back(qraycast_object);
	worldObject->AddRaycast(qraycast_object->raycastObject);
	qraycast_object->worldNode=this;
	return this;
}

QWorldNode *QWorldNode::remove_raycast(Object *raycast_object) {
	int index=get_raycast_index(raycast_object);
	if(index==-1){
		print_error("Quark Physics Error: Out of the size! | QWorldNode.remove_raycast() ");
		return this;
	}
	remove_raycast_at(index);
	return this;
}

QWorldNode *QWorldNode::remove_raycast_at(int index) {
	worldObject->RemoveRaycastAt(index);
	raycastObjects.erase(raycastObjects.begin()+index);
	return this;
}


int QWorldNode::get_raycast_count() {
	return raycastObjects.size();
}

QRaycastObject *QWorldNode::get_raycast_at(int index) {
	return raycastObjects[index];
}

int QWorldNode::get_raycast_index(Object *raycast_object) {
	if(raycast_object==nullptr){
		print_error("Quark Physics Error: Not valid raycast type! | QWorldNode.get_raycast_index() ");
		return -1;
	}
	if(raycast_object->get_class()!="QRaycastObject"){
		print_error("Quark Physics Error: Not valid raycast type! | QWorldNode.get_raycast_index() ");
		return -1;
	}
	QRaycastObject *qraycast_object=static_cast<QRaycastObject*>(raycast_object);
	auto it=find(raycastObjects.begin(),raycastObjects.end(),qraycast_object);
	int index=it-raycastObjects.begin();
	return index;
}

Array QWorldNode::raycast_to(Vector2 position,Vector2 ray_vector,int collidable_layers_bit,bool enable_containing_bodies){
	Array res;
	vector<QRaycast::Contact> contactList=QRaycast::RaycastTo(this->worldObject,QVector(position.x,position.y),QVector(ray_vector.x,ray_vector.y),collidable_layers_bit,enable_containing_bodies);
	for(int i=0;i<contactList.size();i++){
		QRaycast::Contact oc=contactList[i];
		Dictionary c={};
		c["body"]=this->get_body_node_with_object(oc.body);
		c["position"]=Vector2(oc.position.x,oc.position.y);
		c["normal"]=Vector2(oc.normal.x,oc.normal.y);
		c["distance"]=oc.distance;
		res.append(c);
	}
	return res;

}



void QWorldNode::on_process() {
	update_world();
	update();
	//Update rigid body node positions and rotations according to the own physics object
	for(auto bodyNode:bodyNodes){
		if(bodyNode->bodyObject->GetSimulationModel()==QBody::SimulationModels::RIGID_BODY){
			QVector pos=bodyNode->bodyObject->GetPosition();
			float rot=bodyNode->bodyObject->GetRotation();
			bodyNode->set_global_position(Vector2( pos.x,pos.y ) );
			bodyNode->set_global_rotation(rot);
		}
	}
}

void QWorldNode::on_ready(){

}

void QWorldNode::set_debug_renderer_enabled(bool value) {
	enableDebugRenderer=value;
}

bool QWorldNode::get_debug_renderer_enabled() {
	return enableDebugRenderer;
}

void QWorldNode::_bind_methods() {
	//Get
	ClassDB::bind_method(D_METHOD("get_gravity"),&QWorldNode::get_gravity );
	ClassDB::bind_method(D_METHOD("get_enable_sleeping"),&QWorldNode::get_enable_sleeping );
	ClassDB::bind_method(D_METHOD("get_enable_broadphase"),&QWorldNode::get_enable_broadphase );
	ClassDB::bind_method(D_METHOD("get_iteration_count"),&QWorldNode::get_iteration_count );
	ClassDB::bind_method(D_METHOD("get_time_scale"),&QWorldNode::get_time_scale );
	ClassDB::bind_method(D_METHOD("get_debug_renderer_enabled"),&QWorldNode::get_debug_renderer_enabled );
	ClassDB::bind_method(D_METHOD("get_enabled"),&QWorldNode::get_enabled );

	//Set
	ClassDB::bind_method(D_METHOD("set_gravity","value"),&QWorldNode::set_gravity );
	ClassDB::bind_method(D_METHOD("set_enable_sleeping","value"),&QWorldNode::set_enable_sleeping );
	ClassDB::bind_method(D_METHOD("set_enable_broadphase","value"),&QWorldNode::set_enable_broadphase );
	ClassDB::bind_method(D_METHOD("set_iteration_count","value"),&QWorldNode::set_iteration_count );
	ClassDB::bind_method(D_METHOD("set_time_scale","value"),&QWorldNode::set_time_scale );
	ClassDB::bind_method(D_METHOD("set_debug_renderer_enabled","value"),&QWorldNode::set_debug_renderer_enabled );
	ClassDB::bind_method(D_METHOD("set_enabled","value"),&QWorldNode::set_enabled );

	//Body
    ClassDB::bind_method(D_METHOD("add_body_node","body_node"),&QWorldNode::add_body );
    ClassDB::bind_method(D_METHOD("remove_body_node","body_node"),&QWorldNode::remove_body_node );
    ClassDB::bind_method(D_METHOD("remove_body_at","index"),&QWorldNode::remove_body_at );
    ClassDB::bind_method(D_METHOD("get_body_at","index"),&QWorldNode::get_body_at );
    ClassDB::bind_method(D_METHOD("get_body_count"),&QWorldNode::get_body_count );
    ClassDB::bind_method(D_METHOD("get_body_index","body_node"),&QWorldNode::get_body_index );

	//Joint 
	ClassDB::bind_method(D_METHOD("add_joint","joint_object"),&QWorldNode::add_joint );
	ClassDB::bind_method(D_METHOD("remove_joint","joint_object"),&QWorldNode::remove_joint );
	ClassDB::bind_method(D_METHOD("remove_joint_at","index"),&QWorldNode::remove_joint_at );
	ClassDB::bind_method(D_METHOD("get_joint_count"),&QWorldNode::get_joint_count );
	ClassDB::bind_method(D_METHOD("get_joint_at"),&QWorldNode::get_joint_at );
	ClassDB::bind_method(D_METHOD("get_joint_index","joint_object"),&QWorldNode::get_joint_index );


	//Spring
	ClassDB::bind_method(D_METHOD("add_spring","spring_object"),&QWorldNode::add_spring );
	ClassDB::bind_method(D_METHOD("remove_spring","spring_object"),&QWorldNode::remove_spring );
	ClassDB::bind_method(D_METHOD("remove_spring_at","index"),&QWorldNode::remove_spring_at );
	ClassDB::bind_method(D_METHOD("get_spring_count"),&QWorldNode::get_spring_count );
	ClassDB::bind_method(D_METHOD("get_spring_at"),&QWorldNode::get_spring_at );
	ClassDB::bind_method(D_METHOD("get_spring_index","spring_object"),&QWorldNode::get_spring_index );

	//Raycast
	ClassDB::bind_method(D_METHOD("add_raycast","raycast_object"),&QWorldNode::add_raycast );
	ClassDB::bind_method(D_METHOD("remove_raycast","raycast_object"),&QWorldNode::remove_raycast );
	ClassDB::bind_method(D_METHOD("remove_raycast_at","index"),&QWorldNode::remove_raycast_at );
	ClassDB::bind_method(D_METHOD("get_raycast_count"),&QWorldNode::get_raycast_count );
	ClassDB::bind_method(D_METHOD("get_raycast_at"),&QWorldNode::get_raycast_at );
	ClassDB::bind_method(D_METHOD("get_raycast_index","raycast_object"),&QWorldNode::get_raycast_index );

	//Some operations
	ClassDB::bind_method(D_METHOD("get_collisions","bodyA","bodyB"),&QWorldNode::get_collisions );
	ClassDB::bind_method(D_METHOD("get_bodies_hit_by_point","point","max_body_count","only_rigid_bodies","layers_bit"),&QWorldNode::get_bodies_hit_by_point );
	ClassDB::bind_method(D_METHOD("get_particles_close_to_point","point","distance","max_particle_count","except_rigid_bodies","layers_bit"),&QWorldNode::get_particles_close_to_point );
	ClassDB::bind_method(D_METHOD("raycast_to","position","ray_vector","collidable_layers_bit","enable_containing_bodies"),&QWorldNode::raycast_to );
	
	//Collision exceptions
	ClassDB::bind_method(D_METHOD("add_collision_exception","bodyA","bodyB"),&QWorldNode::add_collision_exception );
	ClassDB::bind_method(D_METHOD("remove_collision_exception","bodyA","bodyB"),&QWorldNode::remove_collision_exception );
	ClassDB::bind_method(D_METHOD("remove_matching_collision_exception","body"),&QWorldNode::remove_matching_collision_exception );


	//Update step
	ClassDB::bind_method(D_METHOD("update_world"),&QWorldNode::update_world );

	//Properties
	ADD_PROPERTY( PropertyInfo(Variant::VECTOR2, "gravity"),"set_gravity","get_gravity" );
	ADD_PROPERTY( PropertyInfo(Variant::INT, "iteration_count"),"set_iteration_count","get_iteration_count" );
	ADD_PROPERTY( PropertyInfo(Variant::BOOL, "enable_sleeping"),"set_enable_sleeping","get_enable_sleeping" );
	ADD_PROPERTY( PropertyInfo(Variant::REAL, "time_scale"),"set_time_scale","get_time_scale" );
	ADD_PROPERTY( PropertyInfo(Variant::BOOL, "debug_renderer"),"set_debug_renderer_enabled","get_debug_renderer_enabled" );
	
	


}
