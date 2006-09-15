//
// GameX (Release 2 - Vectors / Matricies / CameraX)
// CameraX Class Code
// 
// Copyright (C) 2002 Rama C. Hoetzlein
// 
// This software is released under the GameX GNU GPL 
// Open Source License. See the GameX documentation included
// with this source code for terms of modification, 
// distribution and re-release. 
//
// 3D CAMERA IMPLEMENTATION 
// 
// The camera transformation of an arbitrary point is:
//
//		Q' = Q * T * R * P
//
// where Q	= 3D point
//		 Q' = Screen point
//		 T = Camera Translation (moves camera to origin)
//		 R = Camera Rotation (rotates camera to point 'up' along Z axis)
//		 P = Projection (projects points onto XY plane)
// 
// T is a unit-coordinate system translated to origin from camera:
//		[1	0  0  0]
//		[0	1  0  0]
//		[0	0  1  0]
//		[-cx -cy -cz 0] 	where c is camera location
// R is a basis matrix: 
//
// P is a projection matrix:
//
	
#include "gamex-camera.hpp"

CameraX::CameraX ()
{	
	Vector3DF from (-150, -150, 40);	// Default camera
	Vector3DF to (0, 0, 0);
	Vector3DF up (0, 0, 1);
	
	view_matrix.Size(4,4);
	proj_matrix.Size(4,4);

	proj_type = CAMERA_PERSPECT;
	near_dist = 1.0f;
	far_dist = 10000.0f;
	height_dist = 1.0f;
	near_clip = 0.0f;
	roll = 0.0f;
	offset_x = 0.0f;
	offset_y = 0.0f;
	MakeProjMatrix ();
	SetFromPosition (from);
	SetToPosition (to);
	SetUpVector (up);
	SetWindow (0, 0, 800, 600);
}

void CameraX::MakeDirVector ()
{
	dir_vector = to_pos;
	dir_vector -= from_pos;
	dir_vector.Normalize();
}

void CameraX::MakeViewMatrix ()
{
	MatrixF basis(4, 4);
	Vector3DF up = up_vector;
	Vector3DF dir;
	Vector3DF temp;
	dir.Set (dir_vector.x, dir_vector.y, dir_vector.z);

	view_matrix = 0.0f;
	view_matrix(0, 0) = 1.0f;	
	view_matrix(1, 1) = 1.0f;
	view_matrix(2, 2) = 1.0f;
	view_matrix(3, 3) = 1.0f;
	view_matrix(3, 0) = -from_pos.X();
	view_matrix(3, 1) = -from_pos.Y();
	view_matrix(3, 2) = -from_pos.Z();

	temp = dir;
	temp *= up_vector.Dot(dir);
	up -= temp;
	up.Normalize();
	side_vector = dir;
	side_vector.Cross (up);
	basis.Basis (side_vector, up, dir);
	
	view_matrix.Multiply (basis);

	if(roll != 0.0f) {
		MatrixF rollM(4, 4);
		rollM = 0.0f;
		rollM(0,0) = cosf(roll);
		rollM(1,0) = -sinf(roll);
		rollM(0,1) = sinf(roll);
		rollM(1,1) = cosf(roll);
		rollM(2,2) = 1.0f;
		rollM(3,3) = 1.0f;
		view_matrix.Multiply (rollM);
	}

	transform_matrix = view_matrix;
	transform_matrix.Multiply (proj_matrix);
}

void CameraX::MakeProjMatrix ()
{
	switch (proj_type) {
	case CAMERA_PERSPECT: {
		proj_matrix = 0.0f;
		proj_matrix(0,0) = near_dist/height_dist;
		proj_matrix(1,1) = near_dist/height_dist;
		proj_matrix(2,2) = far_dist/(far_dist-near_dist);	
		proj_matrix(3,2) = (-near_dist*far_dist)/(far_dist-near_dist);
		proj_matrix(2,3) = 1.0f;
	}
	}
	transform_matrix = view_matrix;
	transform_matrix.Multiply (proj_matrix);
}

void CameraX::SetFromPosition (Vector3DF &from)
{
	from_pos = from;	
	MakeDirVector ();
	MakeViewMatrix ();
}

void CameraX::SetFromAngles (Vector3DF &angs)
{
	float ang = angs.x*DEGtoRAD;
	float tilt = angs.y*DEGtoRAD;
	float dolly = angs.z;
	float cosTilt = cosf(tilt);
	from_pos.x = to_pos.x - (sinf(ang)*cosTilt*dolly);
	from_pos.y = to_pos.y - (cosf(ang)*cosTilt*dolly);
	from_pos.z = to_pos.z + (sinf(tilt)*dolly);
	MakeDirVector ();
	MakeViewMatrix ();
}

void CameraX::SetToPosition (Vector3DF &to)
{
	to_pos = to;	
	MakeDirVector ();
	MakeViewMatrix();
}

void CameraX::SetUpVector (Vector3DF &up)
{
	up_vector = up; 
	MakeViewMatrix();
}

void CameraX::SetRollAngle (float angle)
{
	roll = angle * -DEGtoRAD;
	MakeViewMatrix();
}

void CameraX::SetNearPlane (float n)
{
	near_dist = n;
	MakeProjMatrix();
}

void CameraX::SetFarPlane (float f)
{
	far_dist = f;
	MakeProjMatrix();
}

void CameraX::SetViewHeight (float h)
{
	height_dist = h;
	MakeProjMatrix();
}

void CameraX::SetProjection (int proj_const)
{
	proj_type = proj_const;
}

void CameraX::SetWindow (int x1, int y1, int x2, int y2)
{
	cam_x1 = (float)x1; 				cam_y1 = (float)y1;
	cam_x2 = (float)x2; 				cam_y2 = (float)y2;
	cam_sx = cam_x2 - cam_x1;			cam_sy = cam_y2 - cam_y1;
	cam_cx = cam_x1 + cam_sx/2.0f;		cam_cy = cam_y1 + cam_sy/2.0f;
}


//*** Projection Pipelines:
//
//	view_matrix =			Rotation and Translation basis for Camera
//	proj_matrix =			Perspective (or other) Projection matrix
//	transform_matrix =		view_matrix * proj_matrix
//
// Standard Projection Pipelines:
//
//		view_matrix 		  proj_matrix		  4D to 3D vector
//	p --ViewTransform--> q --ProjTransform--> q --ScreenTransform --> p
//		(returns projected point p in range -1 < x, y, z < 1)
//
// Window Projection Pipeline:
//
//		view_matrix 		  proj_matrix		  4D to 3D vector		  Remap to Window
//	p --ViewTransform--> q --ProjTransform--> q --ScreenTransform --> q --WindowTransform--> (x,y)
//		(returns points in range -win < x, y < win, 0 < z < far)
//
// Combined View/Project Pipeline:
//
//		transform_matrix	   Remap to Window
//	p --CameraTransform--> q --WindowTransform--> (x,y)

void CameraX::ViewTransform (Vector3DF &p, Vector4DF &q)
{
	q = p;	
	q.W() = 1;
	q *= view_matrix;
}

void CameraX::ProjectTransform (Vector4DF &q)
{
	q *= proj_matrix;
}

void CameraX::CameraTransform (Vector3DF &p, Vector4DF &q)
{
	q = p;
	q *= transform_matrix;
}

void CameraX::ScreenTransform (Vector4DF &q, Vector3DF &p)
{
	p = q;
}

void CameraX::WindowTransform (Vector4DF &q, float &x, float &y, float &z)
{
	float sx, sy;

	sx = cam_x2 - cam_x1;
	sy = cam_y2 - cam_y1;
	x = sx/2.0f + cam_x1 + (sx*q.x/q.w);
	y = sy/2.0f + cam_y1 - (sy*q.y/q.w);
	z = q.z / q.w;
}

// Inverse Pipeline:
//
//
//	p -->InverseTransform--> q
//		(given points in range -1 < x, y < 1, z = fixed at near)
//

//		view_matrix 		  proj_matrix		  4D to 3D vector
//	p --ViewTransform--> q --ProjTransform--> q --ScreenTransform --> p
//		(returns projected point p in range -1 < x, y, z < 1)

void CameraX::InverseTransform (Vector3DF &p, Vector4DF &q)
{
	MatrixF inv_proj(4, 4);
	MatrixF inv_view(4, 4);
	Vector3DF up = up_vector;
	Vector3DF temp = dir_vector;

	temp *= up_vector.Dot(dir_vector);
	up -= temp;
	up.Normalize(); 
	side_vector = dir_vector;
	side_vector.Cross (up);
	
	inv_view.Basis (side_vector, up, dir_vector);
	inv_view.Transpose ();

	inv_proj = 0.0f;
	inv_proj(0,0) = height_dist/near_dist;
	inv_proj(1,1) = height_dist/near_dist;
	inv_proj(2,2) = (far_dist-near_dist)/far_dist;
	inv_proj(3,2) = (far_dist-near_dist)/(-near_dist*far_dist);
	inv_proj(2,3) = 1.0f;

	p.Z() = near_dist;
	q = p; 
//	q *= (float) (-near_dist*far_dist / (p.Z()*(far_dist-near_dist)) + near_dist);
	q *= inv_proj;
	q *= inv_view;
}

// Accelerated Pipeline:
//
//	  p -> (4DF) q -> transform_matrix -> window_transform -> (x, y, z)
//		(returns points in range:
//				 camx1 < x < camx2
//				 camy1 < y < camy2
//					 0 < z < far)
//
void CameraX::Project (Vector3DF &p, float &x, float &y, float &z)
{
	float *m = transform_matrix.GetDataF ();
	float qx, qy, qz, qw;
		
	// Explicit vector4DF-matrixF multiplication (p * transform_matrix) 
	qx	= p.x * (*m++); qy	= p.x * (*m++); qz	= p.x * (*m++); qw	= p.x * (*m++);
	qx += p.y * (*m++); qy += p.y * (*m++); qz += p.y * (*m++); qw += p.y * (*m++);
	qx += p.z * (*m++); qy += p.z * (*m++); qz += p.z * (*m++); qw += p.z * (*m++);
	qx +=		(*m++); qy +=		(*m++); qz +=		(*m++); qw +=		(*m  );
	if(qw == 0.0f) qw = 0.000001f; // avoid divide-by-zero
	float csx = cam_sx/qw;
	x = cam_cx + (csx * qx) + offset_x;
	y = cam_cy - (csx * qy) + offset_y;
	z = qz;
	if (z>far_dist) z = -z;
}
