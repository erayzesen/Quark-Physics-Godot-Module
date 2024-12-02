#include "register_types.h"
#include "core/class_db.h"

#include "qworld_node.h"
#include "qbody_node.h"
#include "qrigidbody_node.h"
#include "qsoftbody_node.h"
#include "qareabody_node.h"
#include "qmesh_node.h"
#include "qmesh_rect_node.h"
#include "qmesh_polygon_node.h"
#include "qmesh_circle_node.h"
#include "qmesh_external_node.h"
#include "qparticle_object.h"
#include "qspring_object.h"
#include "qjoint_object.h"
#include "qraycast_object.h"
#include "qplatformerbody_node.h"


void register_quark_physics_types(){
    ClassDB::register_virtual_class<QBodyNode>();
    ClassDB::register_class<QPlatformerBodyNode>();
    ClassDB::register_class<QRigidBodyNode>();
    ClassDB::register_class<QSoftBodyNode>();
    ClassDB::register_class<QAreaBodyNode>();
    ClassDB::register_class<QWorldNode>();
    ClassDB::register_class<QMeshNode>();
    ClassDB::register_class<QMeshRectNode>();
    ClassDB::register_class<QMeshPolygonNode>();
    ClassDB::register_class<QMeshCircleNode>();
    ClassDB::register_class<QMeshExternalNode>();
    ClassDB::register_class<QParticleObject>();
    ClassDB::register_class<QSpringObject>();
    ClassDB::register_class<QJointObject>();
    ClassDB::register_class<QRaycastObject>();
    
}

void unregister_quark_physics_types(){

}