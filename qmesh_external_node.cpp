#include "qmesh_external_node.h"
#include "qbody_node.h"
#include "core/os/file_access.h"


void QMeshExternalNode::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_file_path"),&QMeshExternalNode::get_file_path );
	ClassDB::bind_method(D_METHOD("get_mesh_index"),&QMeshExternalNode::get_mesh_index );
    

    ClassDB::bind_method(D_METHOD("set_file_path","value"),&QMeshExternalNode::set_file_path );
	ClassDB::bind_method(D_METHOD("set_mesh_index","value"),&QMeshExternalNode::set_mesh_index );


    ADD_PROPERTY( PropertyInfo(Variant::STRING, "file_path",PROPERTY_HINT_FILE,"*.qmesh"),"set_file_path","get_file_path" );
    ADD_PROPERTY( PropertyInfo(Variant::INT, "mesh_index",PROPERTY_HINT_RANGE,"0,100"),"set_mesh_index","get_mesh_index" );
    
}

void QMeshExternalNode::update_mesh_data() {
    meshData=QMesh::MeshData();
    if(filePath==""){
        update();
        return;
    }

    FileAccess *file=nullptr;
    file=FileAccess::open(filePath,FileAccess::READ);

    if(file==nullptr){
        print_error("Quark Physics Error: The file doesn't exist! | filePath: " + filePath + " | QMeshExternalNode::update_mesh_data() ");
        return;
    }

    string data=file->get_as_utf8_string().utf8().get_data();   
    
    
    auto meshDataList=QMesh::GetMeshDatasFromJsonData( data );
    if(meshDataList.size()==0 || meshIndex>=meshDataList.size() ){
        update();
        return;
    }
    meshData=meshDataList.at(meshIndex);
    update();
}



void QMeshExternalNode::set_file_path(String value) {
    filePath=value;
    update_mesh_data();
}

String  QMeshExternalNode::get_file_path() {
    return filePath;
}


void QMeshExternalNode::set_mesh_index(int value) {
    meshIndex=value;
    update_mesh_data();
}

int  QMeshExternalNode::get_mesh_index() {
    return meshIndex;
}