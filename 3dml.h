#pragma once
#ifndef _3DML_
#define _3DML_

#include "include.h"
#include "matrix.h"
#include "config.h"

#define OF_CHILD				0x01
#define OF_HIDE					0x02
#define MF_CAMERA_ENABLED		0x01
#define MF_PLAY_ON				0x02
#define MF_VERT_LINK			0x04
#define MF_MATERIAL_LINK		0x08
#define MF_ANIMA_LINK			0x10
#define MF_NO_LOOP				0x20
#define B_NO_RELATION			0xff
#define W_NO_RELATION			0xffff

#define STATIC_ANIMA	0
#define DINAMIC_ANIMA	1

struct Vector3d
{
	GLfloat	x, z, y;
};

struct Camera
{
	GLushort time;
	Vector3d tpos;
	GLfloat y_rot, angle_of_attack, kren, R;
};

struct ObjAnimation 
{
	GLubyte	id;
	GLushort	pos_count;
	GLushort	rot_count;
	GLushort	scale_count;
	GLushort	text_count;
	GLushort	fvert_count;
	GLushort	frame_limit;
	
	struct PosFrame 
	{
		GLushort	time;
		Vector3d	pos;
		Vector3d	dif;
		GLboolean	spline;
	} * pos;
	
	struct RotFrame 
	{
		GLushort	time;
		GLfloat	rot[3];
	} * rot;
	
	struct ScaleFrame 
	{
		GLushort	time;
		Vector3d	scale;
	} * scale;
	
	struct TextFrame 
	{
		GLushort	time;
		GLfloat	texpar[4];
	} * texture;
	
	struct VertFrame 
	{
		GLushort	time;
		Vector3d	* verts;
		Vector3d	* norms;
	} * vertex;
};

struct Object
{
	char		szName[11];
	GLubyte	flags;
	GLushort	vertcount;
	GLushort	normcount;
	Vector3d	* vertex;
	Vector3d	* normal;
	GLushort	planecount;
	GLushort	material;
	GLfloat	matrix[16];
	
	struct Plane3d
	{
		GLushort v0, v1, v2;
		GLushort n0, n1, n2;
	} * planes;
	
	struct Maplist
	{
		GLfloat u, v;
	} * maplist;
	
	GLushort	childcount;
	GLushort	* childs;
	GLushort	parent;
	GLubyte	animacount;
	GLubyte	* animation;		
};	

struct Material 
{
	char szName[10];
	
	struct Facture 
	{
		GLfloat	color[4];
		GLfloat	specular[3];
		GLfloat	emission[3];
		GLfloat	shininess;
	} ci;
	
	char			* szTexture;
	unsigned int	text_ind;
};

struct Anima
{
	char		szName[16];
	GLubyte	id;
	GLushort	cstate_count;
	GLushort	limit;
	Camera		* camera_states;
};

struct Perspective
{
	GLfloat fovy; 
    GLfloat aspect; 
    GLfloat zNear;
    GLfloat zFar;
};

class MODEL_3D 
{
		GLushort			objcount;
		Object				* objects;
		GLushort			matcount;
		Material			* material;
		GLubyte			animacount;
		GLubyte			cur_anima;
		Anima				* animation;
		GLushort			objanimacount;
		GLushort			* objmat;
		ObjAnimation		* objanimation;
		Perspective		pp;
		GLubyte			flags;
		GLfloat			timer;
		GLushort			cur_obj;
		GLfloat			anima_speed;
		GLfloat			tr_matrix[16];
		GLfloat			cam_matrix[16];
		GLvoid				DrawObject(Object&, GLushort);
		GLboolean			LoadTextures(Material&, char*, GLboolean);
		GLvoid				SetCamera(Camera &);
		GLvoid				GetRoot(char*, char*, GLubyte);
		GLvoid				ConvertPath(char*);
		GLubyte 			IncTimer();
		
		static GLfloat 	animation_timer_delta;

	public:		
		MODEL_3D() 
		{			
			pp.fovy		 = 45.0f;
			pp.aspect	 = 1;
			pp.zFar		 = 500.0f;
			pp.zNear	 = 1;
			InitModel();
		}
		~MODEL_3D()
		{
			Free();
		}
		GLvoid 			InitModel();
		GLvoid 			SetAnimaSpeed(GLfloat);
		GLfloat 			GetAnimaSpeed();
		GLboolean 			Load(char*);
		GLubyte 			Draw();
		GLvoid 			SetupPerspective(GLfloat, GLfloat, GLfloat, GLfloat);
		GLboolean 			SelectAnimation(char *);
		GLushort 			GetAnimaLen(char *);
		GLvoid 			SetTimerValue(GLuint);
		GLvoid 			ResetAnimation();
		GLvoid 			PlayAnimation();
		GLvoid 			LoopAnimation();
		GLvoid 			StopAnimation();
		GLvoid 			EnableCamera();
		GLvoid 			DisableCamera();
		GLvoid 			MoveAt(GLfloat, GLfloat, GLfloat);
		GLvoid 			ResetMatrix();
		GLvoid 			MoveRel(GLfloat, GLfloat, GLfloat);
		GLvoid 			Rotate(GLfloat, int);
		GLvoid 			Scale(GLfloat, GLfloat, GLfloat);
		GLboolean 			SelectObj(char *);
		GLvoid 			MoveObjAt(GLfloat, GLfloat, GLfloat);
		GLvoid 			MoveObjRel(GLfloat, GLfloat, GLfloat);
		GLvoid 			RotateObj(GLfloat, int);
		GLvoid 			ScaleObj(GLfloat, GLfloat, GLfloat);
		GLboolean 			SetObjMaterial(char *);
		GLvoid 			SetMeshLink(MODEL_3D &);
		GLvoid 			SetMaterialLink(MODEL_3D &);
		GLvoid 			SetAnimationLink(MODEL_3D &);
		Material * 		GetMatAdress(char *);
		GLvoid 			SetCamera(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
		GLvoid 			Free();

		static GLvoid	SetAnimaTimerDelta(GLfloat);
};

inline GLvoid MODEL_3D::MoveAt(GLfloat x, GLfloat y, GLfloat z)
{
	tr_matrix[12] = x;
	tr_matrix[13] = y;
	tr_matrix[14] = z;
}

inline GLvoid MODEL_3D::SetAnimaSpeed(GLfloat speed) 
{
	anima_speed = speed;
}

inline GLfloat MODEL_3D::GetAnimaSpeed() 
{
	return anima_speed;
}

inline GLvoid MODEL_3D::SetupPerspective(GLfloat fov, GLfloat aspect, GLfloat zNear, GLfloat zFar)
{
	pp.fovy = fov;
	pp.aspect = aspect;
	pp.zNear = zNear;
	pp.zFar = zFar;
}

inline GLvoid MODEL_3D::SetTimerValue(GLuint time)
{
	if (cur_anima != B_NO_RELATION)	{
		timer = GLfloat(time % animation[cur_anima].limit);
	}
}

inline GLvoid MODEL_3D::ResetAnimation() 
{
	timer = 0;
}

inline GLvoid MODEL_3D::PlayAnimation() 
{
	flags |= (MF_PLAY_ON | MF_NO_LOOP);
}

inline GLvoid MODEL_3D::LoopAnimation() 
{
	flags |= MF_PLAY_ON;
}

inline GLvoid MODEL_3D::StopAnimation() 
{
	flags &= ~(MF_PLAY_ON | MF_NO_LOOP);
}

inline GLvoid MODEL_3D::EnableCamera()
{
	flags |= MF_CAMERA_ENABLED;
}

inline GLvoid MODEL_3D::DisableCamera()
{
	flags &= ~MF_CAMERA_ENABLED;
}

inline GLvoid MODEL_3D::ResetMatrix()
{
	E(tr_matrix);
}

inline GLvoid MODEL_3D::MoveRel(GLfloat dx, GLfloat dy, GLfloat dz)
{
	Move16fv(tr_matrix, dx, dy, dz);
}

inline GLvoid MODEL_3D::Rotate(GLfloat a, int ort)
{
	Rotate16fv(tr_matrix, a, ort);
}

inline GLvoid MODEL_3D::Scale(GLfloat kx, GLfloat ky, GLfloat kz)
{
	Scale16fv(tr_matrix, kx, ky, kz);
}

inline GLvoid MODEL_3D::MoveObjAt(GLfloat x, GLfloat y, GLfloat z)
{
	if (cur_obj != W_NO_RELATION){
		objects[cur_obj].matrix[12] = x;
		objects[cur_obj].matrix[13] = y;
		objects[cur_obj].matrix[14] = z;
	}
}

inline GLvoid MODEL_3D::MoveObjRel(GLfloat dx, GLfloat dy, GLfloat dz)
{
	if (cur_obj != W_NO_RELATION) {
		Move16fv(objects[cur_obj].matrix, dx, dy, dz);
	}
}

inline GLvoid MODEL_3D::RotateObj(GLfloat a, int ort)
{
	if (cur_obj != W_NO_RELATION) {
		Rotate16fv(objects[cur_obj].matrix, a, ort);
	}
}

inline GLvoid MODEL_3D::ScaleObj(GLfloat kx, GLfloat ky, GLfloat kz)
{
	if (cur_obj != W_NO_RELATION) {
		Scale16fv(objects[cur_obj].matrix, kx, ky, kz);
	}
}

inline GLvoid MODEL_3D::SetAnimaTimerDelta(GLfloat value)
{
	animation_timer_delta = value;
}

#endif //_3DML_
