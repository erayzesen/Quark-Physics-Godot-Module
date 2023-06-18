#include "qrigidbody_node.h"
#include "qworld_node.h"
#include "core/engine.h"

void QRigidBodyNode::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_fixed_rotation_enabled"), &QRigidBodyNode::get_fixed_rotation_enabled );
    ClassDB::bind_method(D_METHOD("get_kinematic_enabled"), &QRigidBodyNode::get_kinematic_enabled );
    ClassDB::bind_method(D_METHOD("get_kinematic_collisions_enabled"), &QRigidBodyNode::get_kinematic_collisions_enabled );

    ClassDB::bind_method(D_METHOD("set_fixed_rotation_enabled","value"), &QRigidBodyNode::set_fixed_rotation_enabled );
    ClassDB::bind_method(D_METHOD("set_kinematic_enabled","value"), &QRigidBodyNode::set_kinematic_enabled );
    ClassDB::bind_method(D_METHOD("set_kinematic_collisions_enabled","value"), &QRigidBodyNode::set_kinematic_collisions_enabled );
    ClassDB::bind_method(D_METHOD("set_body_position_and_collide","value","with_previous_position"), &QRigidBodyNode::set_body_position_and_collide );
    ClassDB::bind_method(D_METHOD("apply_force","value","r","update_mesh_transforms"), &QRigidBodyNode::apply_force );
    ClassDB::bind_method(D_METHOD("apply_impulse","value","r"), &QRigidBodyNode::apply_impulse );

    ADD_PROPERTY( PropertyInfo(Variant::BOOL,"fixed_rotation"), "set_fixed_rotation_enabled","get_fixed_rotation_enabled" );
    ADD_PROPERTY( PropertyInfo(Variant::BOOL,"kinematic"), "set_kinematic_enabled","get_kinematic_enabled" );
    ADD_PROPERTY( PropertyInfo(Variant::BOOL,"kinematic_collisions"), "set_kinematic_collisions_enabled","get_kinematic_collisions_enabled" );

}



bool QRigidBodyNode::get_fixed_rotation_enabled() {
	auto body=(QRigidBody*)bodyObject;
    return body->GetFixedRotationEnabled();
}

bool QRigidBodyNode::get_kinematic_enabled() {
	auto body=(QRigidBody*)bodyObject;
    return body->GetKinematicEnabled();
}

bool QRigidBodyNode::get_kinematic_collisions_enabled() {
	auto body=(QRigidBody*)bodyObject;
    return body->GetKinematicCollisionsEnabled();
}

QRigidBodyNode *QRigidBodyNode::set_fixed_rotation_enabled(bool value) {
	auto body=(QRigidBody*)bodyObject;
    body->SetFixedRotationEnabled(value);
    return this;
}

QRigidBodyNode *QRigidBodyNode::set_kinematic_enabled(bool value) {
	auto body=(QRigidBody*)bodyObject;
    body->SetKinematicEnabled(value);
    update_meshes_draw();
    return this;
}

QRigidBodyNode *QRigidBodyNode::set_kinematic_collisions_enabled(bool value) {
	auto body=(QRigidBody*)bodyObject;
    body->SetKinematicCollisionsEnabled(value);
    return this;
}

QRigidBodyNode *QRigidBodyNode::set_body_position_and_collide(Vector2 value, bool with_previous_position) {
	auto body=(QRigidBody*)bodyObject;
    body->SetPositionAndCollide(QVector(value.x,value.y),with_previous_position);
    return this;
}


QRigidBodyNode * QRigidBodyNode::apply_force(Vector2 value, Vector2 r, bool update_mesh_transforms)
{
    auto body=(QRigidBody*)bodyObject;
    body->ApplyForce(QVector(value.x,value.y), QVector(r.x,r.y),update_mesh_transforms);
    return this;
}

QRigidBodyNode *QRigidBodyNode::apply_impulse(Vector2 value, Vector2 r) {
	auto body=(QRigidBody*)bodyObject;
    body->ApplyImpulse(QVector(value.x,value.y), QVector(r.x,r.y));
    return this;
}




