#include "3dml.h"

GLfloat MODEL_3D::animation_timer_delta = 1.0f;

inline ObjAnimation * NewObjAnimation(GLushort count)
{
	ObjAnimation * na = new ObjAnimation[count];
	for (GLushort i = 0; i < count; i++){
		na[i].pos		= NULL;
		na[i].rot		= NULL;
		na[i].scale		= NULL;
		na[i].texture	= NULL;
		na[i].vertex	= NULL;
	}
	return na;
}

inline GLvoid DelObjAnimation(ObjAnimation * oanimation, GLushort count)
{
	for (GLushort i = 0; i < count; i++){
		if (oanimation[i].pos)		delete oanimation[i].pos;
		if (oanimation[i].rot)		delete oanimation[i].rot;
		if (oanimation[i].scale)	delete oanimation[i].scale;
		if (oanimation[i].texture)	delete oanimation[i].texture;
		if (oanimation[i].vertex) {
			for (int j = 0; j < oanimation[i].fvert_count; j++){
				delete[] oanimation[i].vertex[j].norms;
				delete[] oanimation[i].vertex[j].verts;
			}
			delete[] oanimation[i].vertex;
		}
	}
	delete[] oanimation;
}

inline Object * NewObject(GLushort count)
{
	Object * newobj		= new Object[count];
	for (GLushort i = 0; i < count; i++){
		newobj[i].flags			= 0;
		newobj[i].childcount	= 0;
		newobj[i].vertex		= NULL;
		newobj[i].planes		= NULL;
		newobj[i].normal		= NULL;
		newobj[i].maplist		= NULL;
		newobj[i].childs		= NULL;
		newobj[i].material		= 0;
		newobj[i].animation		= NULL;		
		newobj[i].animacount	= 0;		
		newobj[i].parent		= 0xffff;
		E(newobj[i].matrix);
	}
	return newobj;
}

inline GLvoid DelObject(Object * Obj, GLushort count)
{
	for (GLushort i = 0 ; i < count; i++){
		if (Obj[i].childs)		delete[] Obj[i].childs;
		if (Obj[i].animation)	delete[] Obj[i].animation;
		if (Obj[i].vertex)		delete[] Obj[i].vertex;
		if (Obj[i].planes)		delete[] Obj[i].planes;
		if (Obj[i].normal)		delete[] Obj[i].normal;
		if (Obj[i].maplist)		delete[] Obj[i].maplist;		
	}
	delete[] Obj;
}

GLvoid MODEL_3D::InitModel() 
{
	objects		 = NULL;
	material	 = NULL;
	animation	 = NULL;
	objanimation = NULL;
	cur_anima	 = B_NO_RELATION;
	cur_obj		 = W_NO_RELATION;
	timer = 0;
	flags		 = 0;
	anima_speed	 = 1.0f;
	objcount	 = 0;
	matcount	 = 0;
	animacount	 = 0;
	objanimacount= 0;
	objmat		 = NULL;
	E(tr_matrix);
}

GLboolean MODEL_3D::SelectAnimation(char * name)
{
	if (! name) {
		cur_anima = B_NO_RELATION; 
		return GL_TRUE;
	}
	for (GLubyte i = 0; i < animacount; i++) {
		if (!strcmp(animation[i].szName, name)) {
			cur_anima = i;
			return GL_TRUE;
		}
	}
	return GL_FALSE;
}

GLushort MODEL_3D::GetAnimaLen(char * name = 0)
{
	if ((! name) && (cur_anima != B_NO_RELATION)) {
		return animation[cur_anima].limit;
	}
	for (GLubyte i = 0; i < animacount; i++) {
		if (!strcmp(animation[i].szName, name)) {
			return animation[i].limit;
		}
	}
	return 0;
}

GLboolean MODEL_3D::SelectObj(char * name)
{
	if (! name) {
		cur_obj = W_NO_RELATION; 
		return GL_TRUE;
	}
	for (GLushort i = 0; i < objcount; i++) {
		if (!strcmp(objects[i].szName, name)) {
			cur_obj = i;
			return GL_TRUE;
		}
	}
	return GL_FALSE;
}

GLboolean MODEL_3D::SetObjMaterial(char * name)
{
	if (cur_obj != W_NO_RELATION) {
		for (GLushort i = 0; i < matcount; i++) {
			if (!strcmp(material[i].szName, name)) {
				objmat[cur_obj] = i;
				return GL_TRUE;
			}
		}				
	}
	return GL_FALSE;
}

GLvoid MODEL_3D::SetMeshLink(MODEL_3D & source)
{
	if (!(flags & MF_VERT_LINK) && objects) {
		DelObject(objects, objcount);
	}
	objects = source.objects;
	objcount = source.objcount;
	flags |= MF_VERT_LINK;
}

GLvoid MODEL_3D::SetMaterialLink(MODEL_3D & source)
{
	if (material && !(flags & MF_MATERIAL_LINK)) {
		for (GLushort i = 0; i < matcount; i++) {
			if (material[i].szTexture) {
				delete[] material[i].szTexture;
			}
			if (material[i].text_ind) {
				glDeleteTextures(1, &material[i].text_ind);
			}
		}
		delete[] material;
	}
	if (objmat) {
		delete[] objmat;
	}
	objmat = new GLushort [source.objcount];
	for (GLushort i = 0; i < source.objcount; i++) {
		objmat[i] = source.objmat[i];
	}
	material = source.material;
	matcount = source.matcount;
	flags |= MF_MATERIAL_LINK;
}

GLvoid MODEL_3D::SetAnimationLink(MODEL_3D & source)
{
	if (!(flags & MF_ANIMA_LINK)){
		if (objanimation) {
			DelObjAnimation(objanimation, objanimacount);
		}
		for (GLushort i = 0; i < animacount; i++) {
			if (animation[i].camera_states) {
				delete[] animation[i].camera_states;
			}
		}
		if (animation) {
			delete[] animation;
		}
	}
	objanimation = source.objanimation;
	objanimacount = source.objanimacount;
	animation = source.animation;
	animacount = source.animacount;
	flags |= MF_ANIMA_LINK;
}

Material * MODEL_3D::GetMatAdress(char * name)
{
	for (GLushort i = 0; i < matcount; i++) {
		if (!strcmp(material[i].szName, name)) {
			return &material[i];
		}
	}
	return NULL;
}

GLvoid MODEL_3D::Free() 
{
	if (!(flags & MF_VERT_LINK) && objects) {
		DelObject(objects, objcount);
	}
	if (material && !(flags & MF_MATERIAL_LINK)) {
		for (GLushort i = 0; i < matcount; i++) {
			if (material[i].szTexture) {
				delete[] material[i].szTexture;
			}
			if (material[i].text_ind) {
				glDeleteTextures(1, &material[i].text_ind);
			}
		}
		delete[] material;
	}
	if (animation && !(flags & MF_ANIMA_LINK)) {
		if (objanimation) {
			DelObjAnimation(objanimation, objanimacount);
		}
		for (GLushort i = 0; i < animacount; i++) {
			if (animation[i].camera_states) {
				delete[] animation[i].camera_states;
			}
		}
		if (animation) {
			delete[] animation;
		}
	}
	if (objmat) {
		delete[] objmat;
	}
	InitModel();
}


GLubyte MODEL_3D::Draw()
{
	GLushort i;
	if (animacount > cur_anima && (flags & MF_CAMERA_ENABLED)) {
		for (i = 0; i < animation[cur_anima].cstate_count; i++) {
			if (animation[cur_anima].camera_states[i].time == timer) {
				SetCamera(animation[cur_anima].camera_states[i]);
				break;
			} else if (animation[cur_anima].camera_states[i].time > timer) {
				if (i) {
					GLfloat koef = GLfloat(timer - animation[cur_anima].camera_states[i - 1].time) / GLfloat(animation[cur_anima].camera_states[i].time - animation[cur_anima].camera_states[i - 1].time);
					Camera cam;
					cam.angle_of_attack = animation[cur_anima].camera_states[i - 1].angle_of_attack +
										 (animation[cur_anima].camera_states[i].angle_of_attack - 
										 animation[cur_anima].camera_states[i - 1].angle_of_attack) * koef;
					cam.y_rot = animation[cur_anima].camera_states[i - 1].y_rot +
										 (animation[cur_anima].camera_states[i].y_rot - 
										 animation[cur_anima].camera_states[i - 1].y_rot) * koef;
					cam.kren = animation[cur_anima].camera_states[i - 1].kren +
										 (animation[cur_anima].camera_states[i].kren - 
										 animation[cur_anima].camera_states[i - 1].kren) * koef;
					cam.tpos.x = animation[cur_anima].camera_states[i - 1].tpos.x +
										 (animation[cur_anima].camera_states[i].tpos.x - 
										 animation[cur_anima].camera_states[i - 1].tpos.x) * koef;
					cam.tpos.y = animation[cur_anima].camera_states[i - 1].tpos.y +
										 (animation[cur_anima].camera_states[i].tpos.y - 
										 animation[cur_anima].camera_states[i - 1].tpos.y) * koef;
					cam.tpos.z = animation[cur_anima].camera_states[i - 1].tpos.z +
										 (animation[cur_anima].camera_states[i].tpos.z - 
										 animation[cur_anima].camera_states[i - 1].tpos.z) * koef;					
					cam.R = animation[cur_anima].camera_states[i - 1].R +
										 (animation[cur_anima].camera_states[i].R - 
										 animation[cur_anima].camera_states[i - 1].R) * koef;
					SetCamera(cam);
				} 
				break;
			}
		}
	}
	glPushMatrix();
	glMultMatrixf(tr_matrix);
	for (i = 0; i < objcount; i++) {
		if (objects[i].flags & OF_CHILD) continue;
		DrawObject(objects[i], objmat[i]);
	}
	glPopMatrix();
	
	return IncTimer();
}

GLubyte MODEL_3D::IncTimer()
{
	if (animacount > cur_anima && (flags & MF_PLAY_ON)) {
		timer += anima_speed * animation_timer_delta;
		if (GLushort(timer) >= animation[cur_anima].limit || timer < 0) {
			if (flags & MF_NO_LOOP) {
				StopAnimation();
			}
			timer = 0;
			return 1;
		}		
	}
	return 0;
}

GLvoid MODEL_3D::DrawObject(Object& obj, GLushort nmaterial)
{
	if (!(obj.flags & OF_HIDE)) {
		GLushort i, j;
		GLboolean blend_on = GL_FALSE, tex_on = GL_FALSE;
		if (nmaterial != W_NO_RELATION) {
			glMaterialfv(GL_FRONT, GL_SPECULAR, material[nmaterial].ci.specular);
			glMaterialfv(GL_FRONT, GL_SHININESS, &material[nmaterial].ci.shininess);
			glMaterialfv(GL_FRONT, GL_EMISSION, material[nmaterial].ci.emission);
			if (material[nmaterial].ci.color[3] < 0.98f){
				glEnable(GL_ALPHA_TEST);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				blend_on = GL_TRUE;
			}
			glColor4fv(material[nmaterial].ci.color);
			if (material[nmaterial].text_ind && obj.maplist){
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, material[nmaterial].text_ind);
				tex_on = GL_TRUE;
			}
		}
		glPushMatrix();
		GLfloat matr[16];	
		mcopy(matr, obj.matrix, 4, 4);
		Object::Maplist * maplst = obj.maplist;
		Vector3d * tmpverts = obj.vertex, * tmpnorms = obj.normal;
		GLboolean delmapls = GL_FALSE, delvert = GL_FALSE;
		if (cur_anima != B_NO_RELATION) {
			GLubyte index = B_NO_RELATION;
			for (i = 0; i < obj.animacount; i++) {
				if (objanimation[obj.animation[i]].id == animation[cur_anima].id) {
					index = obj.animation[i];
					break;
				}
			}
			if (index != B_NO_RELATION) {
				GLushort time = GLushort(timer) % objanimation[index].frame_limit;
				GLfloat koef;
				for (i = 0; i < objanimation[index].pos_count; i++) {
					if (objanimation[index].pos[i].time > time) {
						koef = GLfloat(time - objanimation[index].pos[i - 1].time) / GLfloat(objanimation[index].pos[i].time - objanimation[index].pos[i - 1].time);
						if (i && objanimation[index].pos[i].spline && objanimation[index].pos[i - 1].spline) {						
							GLfloat k1 = (2 * koef * koef * koef - 3 * koef * koef + 1), 
								  k2 = (koef * koef * koef - 2 * koef * koef + koef),
								  k3 = (-2 * koef * koef * koef + 3 * koef * koef),
								  k4 = (koef * koef * koef - koef * koef);
								Move16fv(matr,
									  objanimation[index].pos[i - 1].pos.x * k1 + objanimation[index].pos[i - 1].dif.x * k2 + 
									  objanimation[index].pos[i].pos.x * k3 + objanimation[index].pos[i].dif.x * k4,
									  objanimation[index].pos[i - 1].pos.y * k1 + objanimation[index].pos[i - 1].dif.y * k2 + 
									  objanimation[index].pos[i].pos.y * k3 + objanimation[index].pos[i].dif.y * k4,
									  objanimation[index].pos[i - 1].pos.z * k1 + objanimation[index].pos[i - 1].dif.z * k2 + 
									  objanimation[index].pos[i].pos.z * k3 + objanimation[index].pos[i].dif.z * k4);						
						} else {
							Move16fv(matr,
									  objanimation[index].pos[i - 1].pos.x + (objanimation[index].pos[i].pos.x - objanimation[index].pos[i - 1].pos.x) * koef,
									  objanimation[index].pos[i - 1].pos.y + (objanimation[index].pos[i].pos.y - objanimation[index].pos[i - 1].pos.y) * koef,
									  objanimation[index].pos[i - 1].pos.z + (objanimation[index].pos[i].pos.z - objanimation[index].pos[i - 1].pos.z) * koef);				
						} 
						break;
					} else if (objanimation[index].pos[i].time == time || i == objanimation[index].pos_count - 1) {
						Move16fv(matr,
								  objanimation[index].pos[i].pos.x,
								  objanimation[index].pos[i].pos.y,
								  objanimation[index].pos[i].pos.z);					
						break;
					}
				}
				for (i = 0; i < objanimation[index].scale_count; i++) {
					if (objanimation[index].scale[i].time > time) {
						if (i) {
							koef = GLfloat(time - objanimation[index].scale[i - 1].time) / GLfloat(objanimation[index].scale[i].time - objanimation[index].scale[i - 1].time);
							Scale16fv(matr, 
									 objanimation[index].scale[i - 1].scale.x + (objanimation[index].scale[i].scale.x - objanimation[index].scale[i - 1].scale.x) * koef,
									 objanimation[index].scale[i - 1].scale.y + (objanimation[index].scale[i].scale.y - objanimation[index].scale[i - 1].scale.y) * koef,
									 objanimation[index].scale[i - 1].scale.z + (objanimation[index].scale[i].scale.z - objanimation[index].scale[i - 1].scale.z) * koef);
						} 
						break;
					} else if (objanimation[index].scale[i].time == time || i == objanimation[index].scale_count - 1) {
						Scale16fv(matr,
								 objanimation[index].scale[i].scale.x,
								 objanimation[index].scale[i].scale.y,
								 objanimation[index].scale[i].scale.z);					
						break;
					}
				}
				for (i = 0; i < objanimation[index].rot_count; i++) {
					if (objanimation[index].rot[i].time > time) {
						if (i) {
							koef = GLfloat(time - objanimation[index].rot[i - 1].time) / GLfloat(objanimation[index].rot[i].time - objanimation[index].rot[i - 1].time);
							Rotate16fv(matr, objanimation[index].rot[i - 1].rot[0] + (objanimation[index].rot[i].rot[0] - objanimation[index].rot[i - 1].rot[0]) * koef, ORT_X);
							Rotate16fv(matr, objanimation[index].rot[i - 1].rot[1] + (objanimation[index].rot[i].rot[1] - objanimation[index].rot[i - 1].rot[1]) * koef, ORT_Y);
							Rotate16fv(matr, objanimation[index].rot[i - 1].rot[2] + (objanimation[index].rot[i].rot[2] - objanimation[index].rot[i - 1].rot[2]) * koef, ORT_Z);						
						}
						break;
					} else if (objanimation[index].rot[i].time == time || i == objanimation[index].rot_count - 1) {
						Rotate16fv(matr, objanimation[index].rot[i].rot[0], ORT_X);
						Rotate16fv(matr, objanimation[index].rot[i].rot[1], ORT_Y);
						Rotate16fv(matr, objanimation[index].rot[i].rot[2], ORT_Z);
						break;
					}
				}
				if (tex_on) {
					GLfloat mapl[] = {1, 1, 0, 0};
					for (i = 0; i < objanimation[index].text_count; i++) {
						if (objanimation[index].texture[i].time > time) {
							if (i) {
								koef = GLfloat(time - objanimation[index].texture[i - 1].time) / GLfloat(objanimation[index].texture[i].time - objanimation[index].texture[i - 1].time);
								for (j = 0; j < 4; j++) {
									mapl[j] = objanimation[index].texture[i - 1].texpar[j] + (objanimation[index].texture[i].texpar[j] - objanimation[index].texture[i - 1].texpar[j]) * koef;
								}
								maplst = new Object::Maplist[obj.vertcount];
								for (j = 0; j < obj.vertcount; j++) {
									maplst[j].u = obj.maplist[j].u * mapl[0] + mapl[2];
									maplst[j].v = obj.maplist[j].v * mapl[1] + mapl[3];
								}
								delmapls = GL_TRUE;
							}
							break;
						} else if (objanimation[index].texture[i].time == time || i == objanimation[index].text_count - 1) {
							for (j = 0; j < 4; j++) mapl[j] = objanimation[index].texture[i].texpar[j];
							maplst = new Object::Maplist[obj.vertcount];
							for (j = 0; j < obj.vertcount; j++) {
								maplst[j].u = obj.maplist[j].u * mapl[0] + mapl[2];
								maplst[j].v = obj.maplist[j].v * mapl[1] + mapl[3];
							}
							delmapls = GL_TRUE;
							break;
						}
					}
				}
				for (i = 0; i < objanimation[index].fvert_count; i++) {
					if (objanimation[index].vertex[i].time > time) {
						if (i) {
							koef = GLfloat(time - objanimation[index].vertex[i - 1].time) / GLfloat(objanimation[index].vertex[i].time - objanimation[index].vertex[i - 1].time);
							tmpverts = new Vector3d [obj.vertcount];
							tmpnorms = new Vector3d [obj.normcount];
							delvert = GL_TRUE;
							GLushort j;
							for (j = 0; j < obj.vertcount; j++) {
								tmpverts[j].x = objanimation[index].vertex[i - 1].verts[j].x + (objanimation[index].vertex[i].verts[j].x - objanimation[index].vertex[i - 1].verts[j].x) * koef;
								tmpverts[j].y = objanimation[index].vertex[i - 1].verts[j].y + (objanimation[index].vertex[i].verts[j].y - objanimation[index].vertex[i - 1].verts[j].y) * koef;
								tmpverts[j].z = objanimation[index].vertex[i - 1].verts[j].z + (objanimation[index].vertex[i].verts[j].z - objanimation[index].vertex[i - 1].verts[j].z) * koef;
							}
							for (j = 0; j < obj.normcount; j++) {
								tmpnorms[j].x = objanimation[index].vertex[i - 1].norms[j].x + (objanimation[index].vertex[i].norms[j].x - objanimation[index].vertex[i - 1].norms[j].x) * koef;
								tmpnorms[j].y = objanimation[index].vertex[i - 1].norms[j].y + (objanimation[index].vertex[i].norms[j].y - objanimation[index].vertex[i - 1].norms[j].y) * koef;
								tmpnorms[j].z = objanimation[index].vertex[i - 1].norms[j].z + (objanimation[index].vertex[i].norms[j].z - objanimation[index].vertex[i - 1].norms[j].z) * koef;
							}
						}
						break;
					} else if (objanimation[index].vertex[i].time == time || i == objanimation[index].fvert_count - 1) {
						tmpverts = objanimation[index].vertex[i].verts;
						tmpnorms = objanimation[index].vertex[i].norms;
						break;
					}
				}			
			}
		}
		glMultMatrixf(matr);
		for (i = 0; i < obj.planecount; i++) {
			glBegin(GL_TRIANGLES);
			glNormal3f(	tmpnorms[obj.planes[i].n0].x,
						tmpnorms[obj.planes[i].n0].y,
						tmpnorms[obj.planes[i].n0].z);
			if (tex_on) {
				glTexCoord2d(maplst[obj.planes[i].v0].u,
							 -maplst[obj.planes[i].v0].v);
			}
			glVertex3f(	tmpverts[obj.planes[i].v0].x,
						tmpverts[obj.planes[i].v0].y,
						tmpverts[obj.planes[i].v0].z);
			glNormal3f(	tmpnorms[obj.planes[i].n1].x,
						tmpnorms[obj.planes[i].n1].y,
						tmpnorms[obj.planes[i].n1].z);
			if (tex_on) {
				glTexCoord2d(maplst[obj.planes[i].v1].u,
							 -maplst[obj.planes[i].v1].v);
			}
			glVertex3f(	tmpverts[obj.planes[i].v1].x,
						tmpverts[obj.planes[i].v1].y,
						tmpverts[obj.planes[i].v1].z);
			glNormal3f(	tmpnorms[obj.planes[i].n2].x,
						tmpnorms[obj.planes[i].n2].y,
						tmpnorms[obj.planes[i].n2].z);
			if (tex_on) {
				glTexCoord2d(maplst[obj.planes[i].v2].u,
							 -maplst[obj.planes[i].v2].v);
			}
			glVertex3f(	tmpverts[obj.planes[i].v2].x,
						tmpverts[obj.planes[i].v2].y,
						tmpverts[obj.planes[i].v2].z);
			glEnd();
		}
		if (blend_on) {
			glDisable(GL_BLEND);
			glDisable(GL_ALPHA_TEST);
		}
		if (tex_on) {
			glDisable(GL_TEXTURE_2D);
		}
		if (delmapls) {
			delete[] maplst;
		}
		if (delvert) {
			delete[] tmpverts;
			delete[] tmpnorms;
		}
		for (i = 0; i < obj.childcount; i++) {
			DrawObject(objects[obj.childs[i]], objmat[obj.childs[i]]);
		}
		glPopMatrix();
	}
}

GLboolean MODEL_3D::Load(char* szName)
{
	char szCurRoot[256];
	
	// Loading of the model 
	FILE * f = fopen(szName, "r");
	if (f == NULL) {
		return GL_FALSE;
	}
	
	GetRoot((char*)szName, szCurRoot, 1);
	
	GLushort bf, oid, i, j, k, * reltbl, count, fc, pit, 
			rit, sit, tit, vit, time, flag;
	GLubyte loadto, tnlen, tmp;
	char gi_buf[3];
	
	// Check magic bytes
	fread(&bf, sizeof(GLushort), 1, f);
	if (bf != 0x464d) {
		fclose(f);
		return GL_FALSE;
	}
	
	// Read count of objects
	fread(&objcount, sizeof(GLushort), 1, f);
	// Create objects and material storages
	objects = NewObject(objcount);
	objmat = new GLushort [objcount];
	
	// Loading data of each objects
	for (i = 0; i < objcount; i++) {
		// object name
		fread(objects[i].szName, sizeof(char), 11, f);
		
		// load vertex data
		fread(&objects[i].vertcount, sizeof(GLushort), 1, f);
		objects[i].vertex  = new Vector3d [objects[i].vertcount];
		objects[i].maplist = new Object::Maplist [objects[i].vertcount];
		fread(objects[i].vertex, sizeof(Vector3d), objects[i].vertcount, f);
		fread(objects[i].maplist, sizeof(Object::Maplist), objects[i].vertcount, f);
		
		// load normales
		fread(&objects[i].normcount, sizeof(GLushort), 1, f);
		objects[i].normal = new Vector3d [objects[i].normcount];
		fread(objects[i].normal, sizeof(Vector3d), objects[i].normcount, f);
		
		// laod planes
		fread(&objects[i].planecount, sizeof(GLushort), 1, f);
		objects[i].planes = new Object::Plane3d [objects[i].planecount];
		fread(objects[i].planes, sizeof(Object::Plane3d), objects[i].planecount, f);
		fread(objects[i].matrix, sizeof(GLfloat), 16, f);
		
		// loading material links
		fread(&objects[i].material, sizeof(GLushort), 1, f);
		objmat[i] = objects[i].material;
		fread(&bf, sizeof(GLushort), 1, f);
		fread(gi_buf, sizeof(char), 3, f);
		fread(&count, sizeof(GLushort), 1, f);
		for (j = 0; j < count; j++) {
			fread(&tmp, sizeof(GLubyte), 1, f);
			fseek(f, tmp * 8, SEEK_CUR);			
		}
	}
	
	// load table of parentship
	reltbl = new GLushort [objcount];
	fread(reltbl, sizeof(GLushort), objcount, f);
	for (i = 0; i < objcount; i++) {
		for (j = 0; j < objcount; j++) {
			if (reltbl[j] == i) objects[i].childcount++;
		}
		if (objects[i].childcount) {
			objects[i].childs = new GLushort [objects[i].childcount];
			k = 0;
			for (j = 0; j < objcount; j++) {
				if (reltbl[j] == i){
					objects[i].childs[k] = j;
					objects[j].flags |= OF_CHILD;
					objects[j].parent = i;
					k++;
				}
			}
		}
	}
	
	// loading materials
	struct TFacture {
		GLfloat color[4];
		GLfloat specular[3];
		GLfloat diffuse[3];
		GLfloat ambient[3];
		GLfloat emission[3];
		GLfloat shininess[1];
	} ci;
	
	fread(&matcount, sizeof(GLushort), 1, f);
	material = new Material[matcount];
	for (i = 0; i < matcount; i++) {
		fread(material[i].szName, sizeof(char), 10, f);
		fread(&bf, sizeof(GLushort), 1, f);
		fread(&ci, sizeof(TFacture), 1, f);
		fread(&tnlen, sizeof(GLubyte), 1, f);
		if (tnlen) {
			material[i].szTexture = new char [tnlen];
			fread(material[i].szTexture, sizeof(char), tnlen, f);
		} else {
			material[i].szTexture = NULL;
		}
		for (j = 0; j < 3; j++){
			material[i].ci.color[j]		= ci.color[j];
			material[i].ci.emission[j]	= ci.emission[j];
			material[i].ci.specular[j]	= ci.specular[j];
		}
		material[i].ci.color[3]			= ci.color[3];
		material[i].ci.shininess		= ci.shininess[0];
		material[i].text_ind			= 0;
		LoadTextures(material[i], szCurRoot, GL_TRUE);
	}
	
	// Loading animation
	fread(&bf, sizeof(GLushort), 1, f);
	animacount = (GLubyte)bf;
	animation = new Anima[animacount];
	for (i = 0; i < animacount; i++) {
		fread(animation[i].szName, sizeof(char), 16, f);
		fread(&bf, sizeof(GLushort), 1, f);
		animation[i].id = GLubyte(bf);
		fread(&animation[i].limit, sizeof(GLushort), 1, f);
		fread(&animation[i].cstate_count, sizeof(GLushort), 1, f);
		if (animation[i].cstate_count) {
			animation[i].camera_states = new Camera[animation[i].cstate_count];
			for (j = 0; j < animation[i].cstate_count; j++) {
				fread(&animation[i].camera_states[j], sizeof(Camera), 1, f);
			}
		} else {
			animation[i].camera_states = NULL;
		}
	}
	fread(&count, sizeof(GLushort), 1, f);
	fread(&objanimacount, sizeof(GLushort), 1, f);
	objanimation = NewObjAnimation(objanimacount);
	loadto = 0;
	for (i = 0; i < count; i++){
		fread(&oid, sizeof(GLushort), 1, f);
		fread(&objects[oid].animacount, sizeof(GLushort), 1, f);
		objects[oid].animation = new GLubyte [objects[oid].animacount];
		for (j = 0; j < objects[oid].animacount; j++){
			objects[oid].animation[j] = loadto;
			fread(&bf, sizeof(GLushort), 1, f);
			objanimation[loadto].id = GLubyte(bf);
			fread(&objanimation[loadto].frame_limit, sizeof(GLushort), 1, f);
			fread(&objanimation[loadto].pos_count, sizeof(GLushort), 1, f);
			if (objanimation[loadto].pos_count) {
				objanimation[loadto].pos = new ObjAnimation::PosFrame[objanimation[loadto].pos_count];
			}
			fread(&objanimation[loadto].rot_count, sizeof(GLushort), 1, f);
			if (objanimation[loadto].rot_count) {
				objanimation[loadto].rot = new ObjAnimation::RotFrame[objanimation[loadto].rot_count];
			}
			fread(&objanimation[loadto].scale_count, sizeof(GLushort), 1, f);
			if (objanimation[loadto].scale_count) {
				objanimation[loadto].scale = new ObjAnimation::ScaleFrame[objanimation[loadto].scale_count];
			}
			fread(&objanimation[loadto].text_count, sizeof(GLushort), 1, f);
			if (objanimation[loadto].text_count) {
				objanimation[loadto].texture = new ObjAnimation::TextFrame[objanimation[loadto].text_count];
			}
			fread(&objanimation[loadto].fvert_count, sizeof(GLushort), 1, f);
			if (objanimation[loadto].fvert_count) {
				objanimation[loadto].vertex = new ObjAnimation::VertFrame[objanimation[loadto].fvert_count];
			}
			fread(&fc, sizeof(GLushort), 1, f);
			pit = rit = sit = tit = vit = 0;
			for (k = 0; k < fc; k++) {
				fread(&time, sizeof(GLushort), 1, f);
				fread(&flag, sizeof(GLubyte), 1, f);
				if (flag & 0x01) {
					fread(&objanimation[loadto].pos[pit].pos, sizeof(Vector3d), 1, f);
					fread(&objanimation[loadto].pos[pit].dif, sizeof(Vector3d), 1, f);
					fread(&objanimation[loadto].pos[pit].spline, sizeof(GLubyte), 1, f);
					objanimation[loadto].pos[pit].time = time;
					pit++;
				} 
				if (flag & 0x02) {
					fread(&objanimation[loadto].rot[rit].rot, sizeof(GLfloat), 3, f);
					objanimation[loadto].rot[rit].time = time;
					rit++;
				}
				if (flag & 0x04) {
					fread(&objanimation[loadto].scale[sit].scale.x, sizeof(GLfloat), 1, f);
					fread(&objanimation[loadto].scale[sit].scale.y, sizeof(GLfloat), 1, f);
					fread(&objanimation[loadto].scale[sit].scale.z, sizeof(GLfloat), 1, f);
					objanimation[loadto].scale[sit].time = time;
					sit++;
				}
				if (flag & 0x08) {
					fread(&objanimation[loadto].texture[tit].texpar, sizeof(GLfloat), 4, f);
					objanimation[loadto].texture[tit].time = time;
					tit++;
				}
				if (flag & 0x10) {
					objanimation[loadto].vertex[vit].verts = new Vector3d [objects[oid].vertcount];
					objanimation[loadto].vertex[vit].norms = new Vector3d [objects[oid].normcount];
					fread(objanimation[loadto].vertex[vit].verts, sizeof(Vector3d), objects[oid].vertcount, f);
					fread(objanimation[loadto].vertex[vit].norms, sizeof(Vector3d), objects[oid].normcount, f);
					objanimation[loadto].vertex[vit].time = time;
					vit++;
				}
			}
			loadto++;
		}
	}
	
	fclose(f);
	
	return GL_TRUE;
}

GLboolean MODEL_3D::LoadTextures(Material& mat, char* szCurDir, GLboolean mipmap)
{
	SDL_Surface * image;
	GLenum texture_format;
	GLint nOfColors;
	if (mat.szTexture){
		char tmp[256];
		strcpy(tmp, szCurDir);
		strcat(tmp, mat.szTexture);
		ConvertPath(tmp);
		
#ifdef DEBUG_MODE
printf("Loading texture `%s` \n", tmp);
#endif
		
		image = IMG_Load(tmp);
		if (image) {
			nOfColors = image->format->BytesPerPixel;
			if (nOfColors == 4) {
                if (image->format->Rmask == 0x000000ff) {
					texture_format = GL_RGBA;
				} else {
					texture_format = GL_BGRA;
				}
			} else if (nOfColors == 3) {
                if (image->format->Rmask == 0x000000ff) {
					texture_format = GL_RGB;
				} else {
					texture_format = GL_BGR;
				}
			}
			glGenTextures(1, &mat.text_ind);
			glBindTexture(GL_TEXTURE_2D, mat.text_ind); 
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			
			if (mipmap) {
				gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image->w, image->h,
							texture_format, GL_UNSIGNED_BYTE, image->pixels);
			} else {
				glTexImage2D(GL_TEXTURE_2D, 0, nOfColors, image->w, image->h, 0,
							texture_format, GL_UNSIGNED_BYTE, image->pixels);
			}
						 
			SDL_FreeSurface(image);
		} else {
			mat.text_ind = 0;
			return GL_FALSE;
		}
	}
	return GL_TRUE;
}

GLvoid MODEL_3D::SetCamera(Camera & cp)
{
	E(cam_matrix);
	Move16fv(cam_matrix, cp.tpos.x, cp.tpos.y, cp.tpos.z);
	Rotate_ex16fv(cam_matrix, cp.kren, 2);
	Rotate_ex16fv(cam_matrix, cp.y_rot, 1);
	Rotate_ex16fv(cam_matrix, cp.angle_of_attack, 0);	
	Move16fv(cam_matrix, 0, 0, -cp.R);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(pp.fovy, pp.aspect, pp.zNear, pp.zFar);
	glMultMatrixf(cam_matrix);
	glMatrixMode(GL_MODELVIEW);
}

GLvoid MODEL_3D::SetCamera(GLfloat x, GLfloat y, GLfloat z, GLfloat y_rot, GLfloat kren, GLfloat aoa, GLfloat r)
{
	Camera c;
	c.tpos.x = x;
	c.tpos.y = y;
	c.tpos.z = z;
	c.y_rot = y_rot;
	c.kren = kren;
	c.angle_of_attack = aoa;
	c.R = r;
	SetCamera(c);
}

GLvoid MODEL_3D::GetRoot(char *szFile, char *szRoot, GLubyte add_slash)
{
	char *p = szRoot, *last = p;
	strcpy(szRoot, szFile);
	while (*p){
		if (*p == '\\' || *p == '/') last = p;
		p++;
	}
	if (add_slash && last != szRoot) *(last + 1) = 0;
	else *last = 0;
}

inline GLvoid MODEL_3D::ConvertPath(char *szPath)
{
	char *p = szPath;
	for (; *p; p++) {
		if (*p == '\\') *p = '/';
	}
}
