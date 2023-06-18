#include "qrenderer.h"
#include "qmesh_node.h"
#include "qbody_node.h"
#include "qworld_node.h"
#include "thirdparty/misc/triangulator.h"


void QRenderer::RenderDebug(QWorldNode *worldNode) {
	QDebugColors colors;

    vector<QBodyNode*> bodyCollection=worldNode->bodyNodes;

	//Drawing Body Meshes
	for(int n=0;n<bodyCollection.size();n++){
		QBodyNode *bodyNode=bodyCollection[n];
		for(int m=0;m<bodyNode->get_mesh_count();m++){
			QMeshNode *meshNode=bodyNode->get_mesh_at(m);
			QMesh *meshObject=meshNode->meshObject;

			//Drawing springs
			if(bodyNode->bodyObject->GetBodyType()!=QBody::BodyTypes::RIGID){

				for(int i=0;i<meshObject->GetSpringCount();i++){
					QSpring* spring=meshObject->GetSpringAt(i);
					Color springColor=spring->GetIsInternal() ? colors.SPRING_INTERNAL:colors.SPRING;
					QVector pA=spring->GetParticleA()->GetGlobalPosition();
					QVector pB=spring->GetParticleB()->GetGlobalPosition();
					worldNode->draw_line(Vector2(pA.x,pA.y),Vector2(pB.x,pB.y),springColor);
					
				}
			}

			Color colliderColor=bodyNode->get_mode()==QBody::Modes::STATIC ? colors.COLLIDER_STATIC:colors.COLLIDER_DYNAMIC;
			//Drawing polygons
			for(int i=0;i<meshObject->GetClosedPolygonCount();i++){
				vector<QParticle*> polygon=meshObject->GetClosedPolygonAt(i);
				for(int n=0;n<polygon.size();n++){
					QVector p=polygon[n]->GetGlobalPosition();
					QVector pn=polygon[(n+1)%polygon.size()]->GetGlobalPosition();
					worldNode->draw_line(Vector2(p.x,p.y),Vector2(pn.x,pn.y),colliderColor);
				}
			}
			//Drawing particles
			for(int i=0;i<meshObject->GetParticleCount();i++){
				QParticle *particle=meshObject->GetParticleAt(i);
				Vector2 pos=Vector2( particle->GetGlobalPosition().x, particle->GetGlobalPosition().y );
				float radius=particle->GetRadius();
				if(radius==0.5f){
            		worldNode->draw_circle(pos,radius,colliderColor);
				}else{
					int particleCount=round( (2*M_PI*radius)/2 );
					particleCount=max(particleCount,8);
					worldNode->draw_arc(pos,radius,0,M_PI*2,particleCount,colliderColor,1.0f);
					if( bodyNode->bodyObject->GetBodyType()==QBody::BodyTypes::RIGID && meshObject->GetParticleCount()==1){
						worldNode->draw_line(pos,pos+Vector2(radius,0).rotated(meshObject->GetGlobalRotation() ),colliderColor,1.0f );
					}

				}
                

			}
			

		}
	}

	//Drawing Joints
	for(int i=0;i<worldNode->jointObjects.size();i++){
		QJointObject *j=worldNode->jointObjects[i];
        if(j->get_enabled()==false)
            continue;
        Vector2 pA=j->get_anchor_global_position_a();
        Vector2 pB=j->get_anchor_global_position_b();
        worldNode->draw_line(pA,pB,colors.JOINT,1.0);
        worldNode->draw_circle(pA,3.0,colors.JOINT);
        worldNode->draw_circle(pB,3.0,colors.JOINT);
	}

    //Drawing springs
    for(int i=0;i<worldNode->springObjects.size();i++){
		QSpringObject *s=worldNode->springObjects[i];
        if(s->get_enabled()==false)
            continue;
        Vector2 pA=s->get_particle_a()->get_global_position();
        Vector2 pB=s->get_particle_b()->get_global_position();
        worldNode->draw_line(pA,pB,colors.SPRING_EXTERNAL,1.0);
        worldNode->draw_circle(pA,3.0,colors.SPRING_EXTERNAL);
        worldNode->draw_circle(pB,3.0,colors.SPRING_EXTERNAL);
	}

    //Drawing raycasts
    for(int i=0;i<worldNode->raycastObjects.size();i++){
		QRaycastObject *r=worldNode->raycastObjects[i];
        Vector2 pos=r->get_position();
        Vector2 ray=r->get_ray_vector();
        vector<QRaycast::Contact> *contacts=r->raycastObject->GetContacts();
        if(contacts->size()>0){
            QVector qContactPos=contacts->at(0).position;
            Vector2 contactPos=Vector2(qContactPos.x,qContactPos.y);
            worldNode->draw_line(pos,contactPos,colors.RAYCAST,1.0);
            Color passive_col=colors.RAYCAST;
			worldNode->draw_circle(contactPos,3.0f,colors.RAYCAST);
            passive_col.a=0.1f;
            worldNode->draw_line(contactPos,pos+ray,passive_col,1.0);
        }else{
            worldNode->draw_line(pos,pos+ray,colors.RAYCAST,1.0);
        }

	}
}


void QRenderer::RenderVector(QWorldNode *worldNode){

	vector<QBodyNode*> bodyCollection=worldNode->bodyNodes;

	for(int n=0;n<bodyCollection.size();n++){
		QBodyNode *bodyNode=bodyCollection[n];
		for(int m=0;m<bodyNode->get_mesh_count();m++){
			QMeshNode *meshNode=bodyNode->get_mesh_at(m);
			if(meshNode->enableVectorRendering==false)
				continue;
			QMesh *meshObject=meshNode->meshObject;

			Vector<Color> fillColors;
        	fillColors.push_back(meshNode->fillColor);	
			Vector<Color> strokeColors;
        	strokeColors.push_back(meshNode->strokeColor);
			//Drawing polygons
			for(int i=0;i<meshObject->GetClosedPolygonCount();i++){
				vector<QParticle*> polygon=meshObject->GetClosedPolygonAt(i);
				Vector<Vector2> polygonPoints;
				for(int n=0;n<polygon.size();n++){
					QVector p=polygon[n]->GetGlobalPosition();
					polygonPoints.push_back(Vector2(p.x,p.y) );
				}
				if(polygonPoints.size()>0)
            		polygonPoints.push_back(polygonPoints[0] );

				
				
				
				if(meshNode->enableFill){
					if(meshNode->enableTriangulation){
						Vector<int> triangles;
						Triangulate::triangulate(polygonPoints,triangles);
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
								worldNode->draw_polygon(tri,fillColors,Vector<Vector2>(),Ref<Texture>(),Ref<Texture>(),meshNode->enableAntialias);
							
						}
					}else{
						worldNode->draw_polygon(polygonPoints, fillColors,Vector<Vector2>(),Ref<Texture>(),Ref<Texture>(),meshNode->enableAntialias);
					}
				}
				if(meshNode->enableStroke){
					worldNode->draw_polyline_colors(polygonPoints,strokeColors,meshNode->strokeWidth,meshNode->enableAntialias);
				}
            		
			}
			//Drawing particles
			for(int i=0;i<meshObject->GetParticleCount();i++){
				QParticle *particle=meshObject->GetParticleAt(i);
				float r=particle->GetRadius();
				if(r<=0.5f){
            		continue;
				}
				Vector2 pos=Vector2( particle->GetGlobalPosition().x, particle->GetGlobalPosition().y );
				if(meshNode->enableFill){
					worldNode->draw_circle(pos,r,meshNode->fillColor);
				}
				if( meshNode->enableStroke){
					int particleCount=round( (2*M_PI*r)/2 );
					particleCount=max(particleCount,8);
					worldNode->draw_arc(pos,r,0,M_PI*2,particleCount,meshNode->strokeColor,meshNode->strokeWidth,meshNode->enableAntialias);		
				}
                

			}
			

		}
	}
	
}