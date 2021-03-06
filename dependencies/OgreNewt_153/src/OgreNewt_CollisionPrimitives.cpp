#include <OgreNewt_Prerequisites.h>
#include <OgreNewt_CollisionPrimitives.h>
#include <OgreNewt_Tools.h>


namespace OgreNewt
{

	namespace CollisionPrimitives
	{

		// OgreNewt::CollisionPrimitives::Null
		Null::Null(const OgreNewt::World *world) : Collision( world )
		{
			m_col = NewtonCreateNull( m_world->getNewtonWorld() );
		}


		// OgreNewt::CollisionPrimitives::Box
		Box::Box( const World* world, const Ogre::Vector3& size, const Ogre::Quaternion& orient, const Ogre::Vector3& pos ) : ConvexCollision( world )
		{
			float matrix[16];

			OgreNewt::Converters::QuatPosToMatrix( orient, pos, &matrix[0] );

			m_col = NewtonCreateBox( m_world->getNewtonWorld(), (float)size.x, (float)size.y, (float)size.z, &matrix[0] );
		}



		// OgreNewt::CollisionPrimitives::Ellipsoid
		Ellipsoid::Ellipsoid( const World* world, const Ogre::Vector3& size, const Ogre::Quaternion& orient, const Ogre::Vector3& pos ) : ConvexCollision( world )
		{
			float matrix[16];

			OgreNewt::Converters::QuatPosToMatrix( orient, pos, &matrix[0] );

			m_col = NewtonCreateSphere( m_world->getNewtonWorld(), (float)size.x, (float)size.y, (float)size.z, &matrix[0] );
		}


		// OgreNewt::CollisionPrimitives::Cylinder
		Cylinder::Cylinder( const World* world, Ogre::Real radius, Ogre::Real height, 
			const Ogre::Quaternion& orient, const Ogre::Vector3& pos ) : ConvexCollision( world )
		{
			float matrix[16];

			OgreNewt::Converters::QuatPosToMatrix( orient, pos, &matrix[0] );

			m_col = NewtonCreateCylinder( m_world->getNewtonWorld(), (float)radius, (float)height, &matrix[0] );
		}


		// OgreNewt::CollisionPrimitives::Capsule
		Capsule::Capsule( const World* world, Ogre::Real radius, Ogre::Real height, 
			const Ogre::Quaternion& orient, const Ogre::Vector3& pos ) : ConvexCollision( world )
		{
			float matrix[16];

			OgreNewt::Converters::QuatPosToMatrix( orient, pos, &matrix[0] );

			m_col = NewtonCreateCapsule( m_world->getNewtonWorld(), (float)radius, (float)height, &matrix[0] );
		}


		// OgreNewt::CollisionPrimitives::Cone
		Cone::Cone( const World* world, Ogre::Real radius, Ogre::Real height, 
			const Ogre::Quaternion& orient, const Ogre::Vector3& pos ) : ConvexCollision( world )
		{
			float matrix[16];

			OgreNewt::Converters::QuatPosToMatrix( orient, pos, &matrix[0] );

			m_col = NewtonCreateCone( m_world->getNewtonWorld(), (float)radius, (float)height, &matrix[0] );
		}

		// OgreNewt::CollisionPrimitives::ChamferCylinder
		ChamferCylinder::ChamferCylinder( const World* world, Ogre::Real radius, Ogre::Real height, 
			const Ogre::Quaternion& orient, const Ogre::Vector3& pos ) : ConvexCollision( world )
		{
			float matrix[16];

			OgreNewt::Converters::QuatPosToMatrix( orient, pos, &matrix[0] );

			m_col = NewtonCreateChamferCylinder( m_world->getNewtonWorld(), (float)radius, (float)height, &matrix[0] );
		}



		// OgreNewt::CollisionPrimitives::ConvexHull
		ConvexHull::ConvexHull( const World* world, Ogre::Entity* entity, bool useTempBuffers, const Ogre::Quaternion& orient, const Ogre::Vector3& pos ) : ConvexCollision( world )
		{
			Ogre::Vector3 scale = Ogre::Vector3::UNIT_SCALE;


			// get the mesh!
			Ogre::MeshPtr mesh = entity->getMesh();

			// get scale, if attached to node
			Ogre::Node * node = entity->getParentNode();
			if (node) scale = node->getScale();

			//find number of submeshes
			unsigned short sub = mesh->getNumSubMeshes();

			size_t total_verts = 0;

			Ogre::VertexData* v_data;
			bool addedShared = false;

			for (unsigned short i=0;i<sub;i++)
			{
				Ogre::SubMesh* sub_mesh = mesh->getSubMesh(i);
                Ogre::SubEntity* sub_entity = entity->getSubEntity(i);

				if (sub_mesh->useSharedVertices)
				{
					if (!addedShared)
					{
                        v_data = useTempBuffers ? 
                            entity->_getSkelAnimVertexData() : mesh->sharedVertexData;
						total_verts += v_data->vertexCount;

						addedShared = true;
					}
				}
				else
				{
                    v_data = useTempBuffers ?
                        sub_entity->_getSkelAnimVertexData() : sub_mesh->vertexData;
					total_verts += v_data->vertexCount;
				}
			}

			addedShared = false;

			//make array to hold vertex positions!
			Ogre::Vector3* vertices = new Ogre::Vector3[total_verts];
			unsigned int offset = 0;

			//loop back through, adding vertices as we go!
			for (unsigned short i=0;i<sub;i++)
			{
				Ogre::SubMesh* sub_mesh = mesh->getSubMesh(i);
                Ogre::SubEntity* sub_entity = entity->getSubEntity(i);

				Ogre::VertexDeclaration* v_decl;
				const Ogre::VertexElement* p_elem;
				float* v_Posptr;
				size_t v_count;

				v_data = NULL;

				if (sub_mesh->useSharedVertices)
				{
					if (!addedShared)
					{
                        v_data = useTempBuffers ? 
                            entity->_getSkelAnimVertexData() : mesh->sharedVertexData;
						v_count = v_data->vertexCount;
						v_decl = v_data->vertexDeclaration;
						p_elem = v_decl->findElementBySemantic( Ogre::VES_POSITION );
						addedShared = true;
					}
				}
				else
				{
                    v_data = useTempBuffers ?
                        sub_entity->_getSkelAnimVertexData() : sub_mesh->vertexData;
					v_count = v_data->vertexCount;
					v_decl = v_data->vertexDeclaration;
					p_elem = v_decl->findElementBySemantic( Ogre::VES_POSITION );
				}

				if (v_data)
				{
					size_t start = v_data->vertexStart;
					//pointer
					Ogre::HardwareVertexBufferSharedPtr v_sptr = v_data->vertexBufferBinding->getBuffer( p_elem->getSource() );
					unsigned char* v_ptr = static_cast<unsigned char*>(v_sptr->lock( Ogre::HardwareBuffer::HBL_READ_ONLY ));
					unsigned char* v_offset;

					//loop through vertex data...
					for (size_t j=start; j<(start+v_count); j++)
					{
						//get offset to Position data!
						v_offset = v_ptr + (j * v_sptr->getVertexSize());
						p_elem->baseVertexPointerToElement( v_offset, &v_Posptr );

						//now get vertex positions...
						vertices[offset].x = *v_Posptr; v_Posptr++;
						vertices[offset].y = *v_Posptr; v_Posptr++;
						vertices[offset].z = *v_Posptr; v_Posptr++;

						vertices[offset] *= scale;

						offset++;
					}

					//unlock buffer
					v_sptr->unlock();
				}

		
			}

			float matrix[16];

			OgreNewt::Converters::QuatPosToMatrix( orient, pos, &matrix[0] );

			//okay, let's try making the ConvexHull!
			m_col = NewtonCreateConvexHull( m_world->getNewtonWorld(), (int)total_verts, (float*)&vertices[0].x, sizeof(Ogre::Vector3), &matrix[0] );

			delete []vertices;

		}


		// OgreNewt::CollisionPrimitives::ConvexHull
		ConvexHull::ConvexHull( const World* world, const Ogre::Vector3* verts, int vertcount, const Ogre::Quaternion& orient, const Ogre::Vector3& pos ) : ConvexCollision( world )
		{
			float matrix[16];
			OgreNewt::Converters::QuatPosToMatrix( orient, pos, &matrix[0] );

			//make the collision primitive.
			m_col = NewtonCreateConvexHull( m_world->getNewtonWorld(), vertcount, (float*)&verts[0].x, sizeof(Ogre::Vector3), &matrix[0]);

		}




		TreeCollision::TreeCollision( const World* world) : Collision(world)
		{
		}

		TreeCollision::TreeCollision( const World* world, Ogre::Entity* entity, bool optimize, bool useTempBuffers ) : Collision( world )
		{
			Ogre::Vector3 scale = Ogre::Vector3::UNIT_SCALE;

			start();

			//now get the mesh!
			Ogre::MeshPtr mesh = entity->getMesh();

			//get scale
			Ogre::Node* node = entity->getParentNode();
			if (node) scale = node->getScale();

			//find number of sub-meshes
			unsigned short sub = mesh->getNumSubMeshes();

			for (unsigned short cs=0;cs<sub;cs++)
			{
				Ogre::SubMesh* sub_mesh = mesh->getSubMesh(cs);
                Ogre::SubEntity* sub_entity = entity->getSubEntity(cs);

				//vertex data!
				Ogre::VertexData* v_data;

				if (sub_mesh->useSharedVertices)
				{
                    v_data = useTempBuffers ? 
                        entity->_getSkelAnimVertexData() : mesh->sharedVertexData;
				}
				else
				{
                    v_data = useTempBuffers ?
                        sub_entity->_getSkelAnimVertexData() : sub_mesh->vertexData;
				}

				//let's find more information about the Vertices...
				Ogre::VertexDeclaration* v_decl = v_data->vertexDeclaration;
				const Ogre::VertexElement* p_elem = v_decl->findElementBySemantic( Ogre::VES_POSITION );

				// get pointer!
				Ogre::HardwareVertexBufferSharedPtr v_sptr = v_data->vertexBufferBinding->getBuffer( p_elem->getSource() );
				unsigned char* v_ptr = static_cast<unsigned char*>(v_sptr->lock( Ogre::HardwareBuffer::HBL_READ_ONLY ));

				//now find more about the index!!
				Ogre::IndexData* i_data = sub_mesh->indexData;
				size_t index_count = i_data->indexCount;
				size_t poly_count = index_count / 3;

				// get pointer!
				Ogre::HardwareIndexBufferSharedPtr i_sptr = i_data->indexBuffer;

				// 16 or 32 bit indices?
				bool uses32bit = ( i_sptr->getType() == Ogre::HardwareIndexBuffer::IT_32BIT );
				unsigned long* i_Longptr;
				unsigned short* i_Shortptr;


				if ( uses32bit)
				{
					i_Longptr = static_cast<unsigned long*>(i_sptr->lock( Ogre::HardwareBuffer::HBL_READ_ONLY ));

				}
				else
				{
					i_Shortptr = static_cast<unsigned short*>(i_sptr->lock( Ogre::HardwareBuffer::HBL_READ_ONLY ));
				}


				//now loop through the indices, getting polygon info!
				int i_offset = 0;

				for (size_t i=0; i<poly_count; i++)
				{
					Ogre::Vector3 poly_verts[3];
					unsigned char* v_offset;
					float* v_Posptr;
					int idx;

					if (uses32bit)
					{
						for (int j=0;j<3;j++)
						{
							idx = i_Longptr[i_offset+j];		// index to first vertex!
							v_offset = v_ptr + (idx * v_sptr->getVertexSize());
							p_elem->baseVertexPointerToElement( v_offset, &v_Posptr );
							//now get vertex position from v_Posptr!
							poly_verts[j].x = *v_Posptr; v_Posptr++;
							poly_verts[j].y = *v_Posptr; v_Posptr++;
							poly_verts[j].z = *v_Posptr; v_Posptr++;

							poly_verts[j] *= scale;
						}
					}
					else
					{
						for (int j=0;j<3;j++)
						{
							idx = i_Shortptr[i_offset+j];		// index to first vertex!
							v_offset = v_ptr + (idx * v_sptr->getVertexSize());
							p_elem->baseVertexPointerToElement( v_offset, &v_Posptr );
							//now get vertex position from v_Posptr!

							// switch poly winding.
							poly_verts[j].x = *v_Posptr; v_Posptr++;
							poly_verts[j].y = *v_Posptr; v_Posptr++;
							poly_verts[j].z = *v_Posptr; v_Posptr++;

							poly_verts[j] *= scale;
						}
					}

					addPoly( poly_verts, cs );
					i_offset += 3;
				}

				//unlock the buffers!
				v_sptr->unlock();
				i_sptr->unlock();

			}
			//done!
			finish( optimize );
		}


		TreeCollision::TreeCollision(const OgreNewt::World *world, int numVertices, int numIndices, const float *vertices, const int *indices, bool optimize) : OgreNewt::Collision( world )
		{
			start();
 
			int numPolys = numIndices / 3;

			Ogre::Vector3 *vecVertices = new Ogre::Vector3[numVertices];

			for (int curVertex = 0; curVertex < numVertices; curVertex++)
			{
				vecVertices[curVertex].x = vertices[0 + curVertex * 3];
				vecVertices[curVertex].y = vertices[1 + curVertex * 3];
				vecVertices[curVertex].z = vertices[2 + curVertex * 3];
			}

			for ( int poly = 0; poly < numPolys; poly++ )
			{
				Ogre::Vector3 poly_verts[3];

				poly_verts[0] = vecVertices[indices[0 + poly * 3]];
				poly_verts[1] = vecVertices[indices[1 + poly * 3]];
				poly_verts[2] = vecVertices[indices[2 + poly * 3]];

				addPoly( poly_verts, 0 );
			}

			delete [] vecVertices;

		     finish( optimize );
		}


		TreeCollision::TreeCollision( const World* world, int numVertices, Ogre::Vector3* vertices, Ogre::IndexData* indexData, bool optimize) : Collision( world )
		{
			start();

			unsigned int numPolys = indexData->indexCount / 3;
			Ogre::HardwareIndexBufferSharedPtr hwIndexBuffer=indexData->indexBuffer;
			size_t indexSize=hwIndexBuffer->getIndexSize();
			void* indices=hwIndexBuffer->lock(Ogre::HardwareBuffer::HBL_READ_ONLY);

			assert((indexSize==2) || (indexSize==4));

			if (indexSize==2)
			{
				unsigned short* curIndex=(unsigned short*)indices;
				for ( unsigned int poly = 0; poly < numPolys; poly++ )
				{
					Ogre::Vector3 poly_verts[3];

					//invert vertex winding (otherwise, raycasting won't work???)
					poly_verts[0] = vertices[*curIndex]; curIndex++;
					poly_verts[2] = vertices[*curIndex]; curIndex++;
					poly_verts[1] = vertices[*curIndex]; curIndex++;

					addPoly( poly_verts, 0 );
				}
			}
			else
			{
				unsigned int* curIndex=(unsigned int*)indices;
				for ( unsigned int poly = 0; poly < numPolys; poly++ )
				{
					Ogre::Vector3 poly_verts[3];

					poly_verts[0] = vertices[*curIndex]; curIndex++;
					poly_verts[2] = vertices[*curIndex]; curIndex++;
					poly_verts[1] = vertices[*curIndex]; curIndex++;

					addPoly( poly_verts, 0 );
				}
			}
      
			hwIndexBuffer->unlock();
			finish( optimize );
		} 


		void TreeCollision::start()
		{
			m_col = NewtonCreateTreeCollision( m_world->getNewtonWorld(), NULL );
			NewtonTreeCollisionBeginBuild( m_col );
		}

		void TreeCollision::addPoly( Ogre::Vector3* polys, unsigned int ID )
		{
			NewtonTreeCollisionAddFace( m_col, 3, (float*)&polys[0].x, sizeof(Ogre::Vector3), ID );
		}

		void TreeCollision::finish( bool optimize)
		{
			NewtonTreeCollisionEndBuild( m_col, optimize );
		}


		int TreeCollisionSceneParser::count = 0;
		
		
		TreeCollisionSceneParser::TreeCollisionSceneParser( OgreNewt::World* world ) : TreeCollision( world )
		{
		}
		
		void TreeCollisionSceneParser::parseScene( Ogre::SceneNode *startNode, bool optimize)
		{
			count = 0;

			start();

			// parse the individual nodes.
			Ogre::Quaternion rootOrient = Ogre::Quaternion::IDENTITY;
			Ogre::Vector3 rootPos = Ogre::Vector3::ZERO;

			_parseNode( startNode, rootOrient, rootPos );

			finish( optimize );
		}

		void TreeCollisionSceneParser::_parseNode(Ogre::SceneNode *node, const Ogre::Quaternion &curOrient, const Ogre::Vector3 &curPos)
		{
			// parse this scene node.
			// do children first.
			Ogre::Quaternion thisOrient = curOrient * node->getOrientation();
			Ogre::Vector3 thisPos = curPos + (curOrient * node->getPosition() * node->getScale());
			Ogre::Vector3 thisScale = node->getScale();

			Ogre::SceneNode::ChildNodeIterator child_it = node->getChildIterator();

			while (child_it.hasMoreElements())
			{
				_parseNode( (Ogre::SceneNode*)child_it.getNext(), thisOrient, thisPos );
			}


			// now add the polys from this node.
			//now get the mesh!
			unsigned int num_obj = node->numAttachedObjects();
			for (unsigned int co=0; co<num_obj; co++)
			{
				Ogre::MovableObject* obj = node->getAttachedObject(co);
				if (obj->getMovableType() != "Entity")
					continue;
			
				Ogre::Entity* ent = (Ogre::Entity*)obj;

				if (!entityFilter(node, ent))
					continue;

				Ogre::MeshPtr mesh = ent->getMesh();

				//find number of sub-meshes
				unsigned short sub = mesh->getNumSubMeshes();

				for (unsigned short cs=0;cs<sub;cs++)
				{
					Ogre::SubMesh* sub_mesh = mesh->getSubMesh(cs);

					//vertex data!
					Ogre::VertexData* v_data;

					if (sub_mesh->useSharedVertices)
					{	
						v_data = mesh->sharedVertexData;
					}
					else
					{
						v_data = sub_mesh->vertexData;
					}
		
					//let's find more information about the Vertices...
					Ogre::VertexDeclaration* v_decl = v_data->vertexDeclaration;
					const Ogre::VertexElement* p_elem = v_decl->findElementBySemantic( Ogre::VES_POSITION );
		
					// get pointer!
					Ogre::HardwareVertexBufferSharedPtr v_sptr = v_data->vertexBufferBinding->getBuffer( p_elem->getSource() );
					unsigned char* v_ptr = static_cast<unsigned char*>(v_sptr->lock( Ogre::HardwareBuffer::HBL_READ_ONLY ));
		
					//now find more about the index!!
					Ogre::IndexData* i_data = sub_mesh->indexData;
					size_t index_count = i_data->indexCount;
					size_t poly_count = index_count / 3;
		
					// get pointer!
					Ogre::HardwareIndexBufferSharedPtr i_sptr = i_data->indexBuffer;
		
					// 16 or 32 bit indices?
					bool uses32bit = ( i_sptr->getType() == Ogre::HardwareIndexBuffer::IT_32BIT );
					unsigned long* i_Longptr;
					unsigned short* i_Shortptr;
		
					if ( uses32bit)
					{
						i_Longptr = static_cast<unsigned long*>(i_sptr->lock( Ogre::HardwareBuffer::HBL_READ_ONLY ));
					}
					else
					{
						i_Shortptr = static_cast<unsigned short*>(i_sptr->lock( Ogre::HardwareBuffer::HBL_READ_ONLY ));
					}

					//now loop through the indices, getting polygon info!
					int i_offset = 0;

					for (size_t i=0; i<poly_count; i++)
					{
						Ogre::Vector3 poly_verts[3];
						unsigned char* v_offset;
						float* v_Posptr;
						int idx;

						if (uses32bit)
						{
							for (int j=0;j<3;j++)
							{
								idx = i_Longptr[i_offset+j];		// index to first vertex!
								v_offset = v_ptr + (idx * v_sptr->getVertexSize());
								p_elem->baseVertexPointerToElement( v_offset, &v_Posptr );
								//now get vertex position from v_Posptr!
								poly_verts[j].x = *v_Posptr; v_Posptr++;
								poly_verts[j].y = *v_Posptr; v_Posptr++;
								poly_verts[j].z = *v_Posptr; v_Posptr++;
	
								poly_verts[j] = thisPos + (thisOrient * (poly_verts[j] * thisScale));
							}
						}
						else
						{
							for (int j=0;j<3;j++)
							{
								idx = i_Shortptr[i_offset+j];		// index to first vertex!
								v_offset = v_ptr + (idx * v_sptr->getVertexSize());
								p_elem->baseVertexPointerToElement( v_offset, &v_Posptr );
								//now get vertex position from v_Posptr!

								// switch poly winding.
								poly_verts[j].x = *v_Posptr; v_Posptr++;
								poly_verts[j].y = *v_Posptr; v_Posptr++;
								poly_verts[j].z = *v_Posptr; v_Posptr++;
							
								poly_verts[j] = thisPos + (thisOrient * (poly_verts[j] * thisScale));
							}
						}
					
						addPoly( poly_verts, getID(node, ent, cs) );
						i_offset += 3;
					}

					//unlock the buffers!
					v_sptr->unlock();
					i_sptr->unlock();

				}
			}

		}





		// OgreNewt::CollisionPrimitives::CompoundCollision
		CompoundCollision::CompoundCollision( const World* world, std::vector<CollisionPtr> col_array ) : Collision( world )
		{
			//get the number of elements.
			unsigned int num = col_array.size();

			// create simple array.
			NewtonCollision** array = new NewtonCollision*[num];

			for (unsigned int i=0;i<num;i++)
			{
				array[i] = (NewtonCollision*)col_array[i]->getNewtonCollision();
			}

			m_col = NewtonCreateCompoundCollision( world->getNewtonWorld(), num, array );

			delete[] array;

		}


		// OgreNewt::CollisionPrimitives::Pyramid
		Pyramid::Pyramid( const World* world, const Ogre::Vector3& size, const Ogre::Quaternion& orient, const Ogre::Vector3& pos ) : ConvexCollision( world )
		{
			float matrix[16];

			OgreNewt::Converters::QuatPosToMatrix( orient, pos, &matrix[0] );

			// create a simple pyramid collision primitive using the Newton Convex Hull interface.
			// this function places the center of mass 1/3 up y from the base.

			float* vertices = new float[15];
			unsigned short idx = 0;

			// make the bottom base.
			for (int ix=-1; ix<=1; ix+=2)
			{
				for (int iz=-1; iz<=1; iz+=2)
				{
					vertices [idx++] = (size.x/2.0) * ix;
					vertices [idx++] = -(size.y/3.0);
					vertices [idx++] = (size.z/2.0) * iz;
				}
			}

			// make the tip.
			vertices [idx++] = 0.0f;
			vertices [idx++] = (size.y*2.0/3.0);
			vertices [idx++] = 0.0f;

			//make the collision primitive.
			m_col = NewtonCreateConvexHull( m_world->getNewtonWorld(), 5, vertices, sizeof(float)*3, &matrix[0]);

			delete []vertices;

		}

		HullModifier::HullModifier(World* world, CollisionPtr wrappedCollision) : Collision(world)
		{
			m_col = NewtonCreateConvexHullModifier(world->getNewtonWorld(), wrappedCollision->getNewtonCollision());
		}

		void HullModifier::setMatrix(const Ogre::Matrix4& matrix)
		{
			NewtonConvexHullModifierSetMatrix(m_col, matrix.transpose()[0]);
		}

		Ogre::Matrix4 HullModifier::getMatrix() const
		{
			Ogre::Matrix4 matrix;
			NewtonConvexHullModifierGetMatrix(m_col, matrix[0]);
			return matrix.transpose();
		}

	}	// end namespace CollisionPrimitives

}	// end namespace OgreNewt




