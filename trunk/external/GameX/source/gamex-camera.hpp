//
// GameX (Release 2 - Vectors / Matricies / CameraX)
// CameraX Class Header
// 
// Copyright (C) 2002 Rama C. Hoetzlein
// 
// This software is released under the GameX GNU GPL 
// Open Source License. See the GameX documentation included
// with this source code for terms of modification, 
// distribution and re-release. 
//

#ifndef GAMEX_CAMERAX_DEF
	#define GAMEX_CAMERAX_DEF

	#include "gamex-matrix.hpp"
	#include "gamex-vector.hpp"

	#ifndef DEGtoRAD
		#define DEGtoRAD (3.1415926538f/180.0f)
	#endif

	#define CAMERA_PERSPECT		0
	#define CAMERA_PARALLEL		1

	class CameraX {
	public:
		CameraX ();

		void SetFromPosition (Vector3DF &from);
		void SetFromAngles (Vector3DF &angs); // x is x rotation, y is pitch, z is zoom distance
		void SetToPosition (Vector3DF &to);
		void SetUpVector (Vector3DF &up);
		void SetNearPlane (float n);
		void SetRollAngle (float angle);
		inline void SetOffset (float x, float y) {offset_x = x; offset_y = y;}
		void SetFarPlane (float f);
		void SetViewHeight (float h);
		void SetProjection (int proj_const);
		inline void SetNearClip (float zdepth) {near_clip = zdepth;}
		inline float GetNearClip (void) {return near_clip;}

		void SetWindow (int x1, int y1, int x2, int y2);
		void GetWindow (int&x1, int&y1, int&x2, int&y2) {x1 = (int)cam_x1; y1 = (int)cam_y1; x2 = (int)cam_x2; y2 = (int)cam_y2;}

		void ViewTransform (Vector3DF &p, Vector4DF &q);
		void ProjectTransform (Vector4DF &q);
		void ScreenTransform (Vector4DF &q, Vector3DF &p);
		void CameraTransform (Vector3DF &p, Vector4DF &q);
		void WindowTransform (Vector4DF &q, float &x, float &y, float &z);
		void InverseTransform (Vector3DF &p, Vector4DF &q);
		
		void Project (Vector3DF &q, float &x, float &y, float &z);

		inline float GetRollAngle () {return roll;}
		inline float GetHeight () {return height_dist;}
		inline float GetNear () {return near_dist;}
		inline float GetFar () {return far_dist;}
		inline float GetPixelsPerUnit() {return (cam_x2-cam_x1);}
		inline Vector3DF &From () {return from_pos;}
		inline Vector3DF &To () {return to_pos;}
		inline MatrixF * GetViewMatrix(void) {return &view_matrix;}
		inline MatrixF * GetProjMatrix(void) {return &proj_matrix;}
		inline Vector3DF & GetDirVector(void) {return dir_vector;}
		inline Vector3DF & GetUpVector(void) {return up_vector;}
		inline Vector3DF & GetSideVector(void) {return side_vector;}
	private:
		void MakeDirVector ();			// Calculate direction (using to and from vectors)
		void MakeViewMatrix ();			// Calculate view matrix (using dir, from and side vectors)
		void MakeProjMatrix ();			// Calculate projection matrix (using proj. parameters)

		int proj_type;					// Camera projection type

		Vector3DF from_pos, to_pos;		// Camera To and From positions
		float roll;						// Camera twist about its axis
										// Camera Tview vectors (N, V, and U)
		float offset_x, offset_y;
		Vector3DF dir_vector, up_vector, side_vector;
		MatrixF view_matrix;			// Camera Tview matrix
										// Camera Tpersp parameters (d, f, and h)		
		float near_dist, far_dist, height_dist, near_clip;	
		MatrixF proj_matrix;			// Camera Tpersp (also Tparallel or Tortho)
					
		MatrixF transform_matrix;		// Camera Transform (Tview * Tpersp)

		float cam_x1, cam_y1;				// Camera Window (Maps -1<x,y<1 to 0<x,y<win)
		float cam_x2, cam_y2;
		float cam_sx, cam_sy;
		float cam_cx, cam_cy;
	};

#endif
