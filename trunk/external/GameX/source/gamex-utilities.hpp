//
// GameX - Utilities Header - General classes used by GameX
// 
// This software is released under the GameX GNU GPL 
// Open Source License. See the GameX documentation included
// with this source code for terms of modification, 
// distribution and re-release. 
// 

#ifndef GAMEX_UTILITIES

	#define GAMEX_UTILITIES

	// a structure-like class for a color (red, green, blue values)
	// which may or may not contain transparency (alpha values) and varying color across 4 corners
	class ColorX {
	public:
		int c[4][4]; // color component values which range from 0 to 255, inclusive
		// c[where][what] -- "where" is top-left,top-right,bottom-right,bottom-left
		//				  -- "what" is red,green,blue,alpha(opacity)

		// default color of white
		inline ColorX() {for(int i=0;i<4;i++) for(int j=0;j<4;j++) c[i][j] = 255;}

		// a normal red,green,blue color, with full opacity
		inline ColorX(int r, int g, int b) {for(int i=0;i<4;i++) {c[i][0] = r; c[i][1] = g; c[i][2] = b; c[i][3] = 255;}}

		// a normal red,green,blue color, with opacity value a
		inline ColorX(int r, int g, int b, int a) {for(int i=0;i<4;i++) {c[i][0] = r; c[i][1] = g; c[i][2] = b; c[i][3] = a;}}

		// a color that's different at each corner and blended in-between, with full opacity
		inline ColorX(int r1, int g1, int b1, int r2, int g2, int b2, int r3, int g3, int b3, int r4, int g4, int b4) {c[0][0] = r1; c[0][1] = g1; c[0][2] = b1; c[0][3] = 255; c[1][0] = r2; c[1][1] = g2; c[1][2] = b2; c[1][3] = 255; c[2][0] = r3; c[2][1] = g3; c[2][2] = b3; c[2][3] = 255; c[3][0] = r4; c[3][1] = g4; c[3][2] = b4; c[3][3] = 255;}

		// a color that's different at each corner and blended in-between, with opacity values a#
		inline ColorX(int r1, int g1, int b1, int a1, int r2, int g2, int b2, int a2, int r3, int g3, int b3, int a3, int r4, int g4, int b4, int a4) {c[0][0] = r1; c[0][1] = g1; c[0][2] = b1; c[0][3] = a1; c[1][0] = r2; c[1][1] = g2; c[1][2] = b2; c[1][3] = a2; c[2][0] = r3; c[2][1] = g3; c[2][2] = b3; c[2][3] = a3; c[3][0] = r4; c[3][1] = g4; c[3][2] = b4; c[3][3] = a4;}

		// a color that's 4 colors combined into separate regions
		inline ColorX(ColorX&c1, ColorX&c2, ColorX&c3, ColorX&c4) {for(int i = 0 ; i < 4 ; i++) {c[0][i] = c1.c[0][i]; c[1][i] = c2.c[0][i]; c[2][i] = c3.c[0][i]; c[3][i] = c4.c[0][i];}}

		// a copy of another ColorX
		inline ColorX(ColorX&c0) {for(int i=0;i<4;i++) for(int j=0;j<4;j++) c[i][j] = c0.c[i][j];}

		inline ~ColorX() {}

		inline bool HasTransparency(void) {return (c[0][3]<252 || c[1][3]<252 || c[2][3]<252 || c[3][3]<252);}
		inline bool HasOpacity(void) {return (c[0][3]>3 || c[1][3]>3 || c[2][3]>3 || c[3][3]>3);}
		inline bool HasColor(void) {for(int i=0;i<4;i++) if(c[0][i]>3 || c[1][i]>3 || c[2][i]>3 || c[3][i]>3) return true; return false;}
		inline bool HasShading(void) {for(int i=0;i<4;i++) if(c[0][i]<252 || c[1][i]<252 || c[2][i]<252 || c[3][i]<252) return true; return false;}
		inline int GetRed(int r, int where=0) {return c[where][0];}
		inline int GetGreen(int g, int where=0) {return c[where][1];}
		inline int GetBlue(int b, int where=0) {return c[where][2];}
		inline int GetAlpha(int a, int where=0) {return c[where][3];}

		inline void CapComponents(void) {for(int i=0;i<4;i++) for(int j=0;j<4;j++) {if(c[i][j] < 0) c[i][j] = 0; if(c[i][j] > 255) c[i][j] = 255;}}
		inline void Invert(void) {for(int i=0;i<4;i++) for(int j=0;j<3;j++) c[i][j] = 255 - c[i][j];}
		inline void SetRed(int r) {for(int i=0;i<4;i++) c[i][0] = r;}
		inline void SetGreen(int g) {for(int i=0;i<4;i++) c[i][1] = g;}
		inline void SetBlue(int b) {for(int i=0;i<4;i++) c[i][2] = b;}
		inline void SetAlpha(int a) {for(int i=0;i<4;i++) c[i][3] = a;}

		inline ColorX &operator=  (const ColorX&op)    {for(int i=0;i<4;i++) for(int j=0;j<4;j++) c[i][j] = op.c[i][j]; return *this;}
		inline ColorX &operator*= (const Vector4DF&op) {for(int i=0;i<4;i++) {c[i][0] = (int)((float)c[i][0]*op.x); c[i][1] = (int)((float)c[i][1]*op.y); c[i][2] = (int)((float)c[i][2]*op.z); c[i][3] = (int)((float)c[i][3]*op.w); } return *this;}
		inline ColorX &operator*= (int op)			   {for(int i=0;i<4;i++) for(int j=0;j<3;j++) c[i][j] *= op; return *this;}
		inline ColorX &operator*= (float op)		   {for(int i=0;i<4;i++) for(int j=0;j<3;j++) c[i][j] = (int)((float)c[i][j]*op); return *this;}
		inline ColorX  operator*  (const Vector4DF&op) {ColorX nc = ColorX(*this); nc *= op; return nc;}
		inline ColorX  operator*  (int op)			   {ColorX nc = ColorX(*this); nc *= op; return nc;}
		inline ColorX  operator*  (float op)		   {ColorX nc = ColorX(*this); nc *= op; return nc;}
		inline ColorX &operator/= (const Vector4DF&op) {for(int i=0;i<4;i++) {c[i][0] = (int)((float)c[i][0]/op.x); c[i][1] = (int)((float)c[i][1]/op.y); c[i][2] = (int)((float)c[i][2]/op.z); c[i][3] = (int)((float)c[i][3]/op.w); } return *this;}
		inline ColorX &operator/= (int op)			   {for(int i=0;i<4;i++) for(int j=0;j<3;j++) c[i][j] /= op; return *this;}
		inline ColorX &operator/= (float op)		   {for(int i=0;i<4;i++) for(int j=0;j<3;j++) c[i][j] = (int)((float)c[i][j]/op); return *this;}
		inline ColorX  operator/  (const Vector4DF&op) {ColorX nc = ColorX(*this); nc /= op; return nc;}
		inline ColorX  operator/  (int op)			   {ColorX nc = ColorX(*this); nc /= op; return nc;}
		inline ColorX  operator/  (float op)		   {ColorX nc = ColorX(*this); nc /= op; return nc;}
		inline ColorX &operator+= (const Vector4DF&op) {for(int i=0;i<4;i++) {c[i][0] += (int)(op.x); c[i][1] += (int)(op.y); c[i][2] += (int)(op.z); c[i][3] += (int)(op.w); } return *this;}
		inline ColorX &operator+= (const ColorX&op)    {for(int i=0;i<4;i++) for(int j=0;j<4;j++) c[i][j] += op.c[i][j]; return *this;}
		inline ColorX &operator+= (int op)			   {for(int i=0;i<4;i++) for(int j=0;j<3;j++) c[i][j] += op; return *this;}
		inline ColorX  operator+  (const Vector4DF&op) {ColorX nc = ColorX(*this); nc += op; return nc;}
		inline ColorX  operator+  (const ColorX&op)    {ColorX nc = ColorX(*this); nc += op; return nc;}
		inline ColorX  operator+  (int op)			   {ColorX nc = ColorX(*this); nc += op; return nc;}
		inline ColorX &operator-= (const Vector4DF&op) {for(int i=0;i<4;i++) {c[i][0] -= (int)(op.x); c[i][1] -= (int)(op.y); c[i][2] -= (int)(op.z); c[i][3] -= (int)(op.w); } return *this;}
		inline ColorX &operator-= (const ColorX&op)    {for(int i=0;i<4;i++) for(int j=0;j<4;j++) c[i][j] -= op.c[i][j]; return *this;}
		inline ColorX &operator-= (int op)			   {for(int i=0;i<4;i++) for(int j=0;j<3;j++) c[i][j] -= op; return *this;}
		inline ColorX  operator-  (const Vector4DF&op) {ColorX nc = ColorX(*this); nc -= op; return nc;}
		inline ColorX  operator-  (const ColorX&op)    {ColorX nc = ColorX(*this); nc -= op; return nc;}
		inline ColorX  operator-  (int op)			   {ColorX nc = ColorX(*this); nc -= op; return nc;}
	};

	// holds values that determine the coordinates of a 2-dimensional quadrilateral
	// holds x,y pairs in the following order: top-left, top-right, bottom-left, bottom-right
	class Quad2D {
	public:
		float x[4]; // left(of top), right(of top), left(of bottom), right(of bottom)
		float y[4]; // top(of left), top(of right), bottom(of left), bottom(of right)
		inline Quad2D() {}
		inline ~Quad2D() {}
		inline Quad2D(float x0,float y0,  float x1,float y1,  float x2,float y2,  float x3,float y3) {x[0]=x0; y[0]=y0;   x[1]=x1; y[1]=y1;   x[2]=x2; y[2]=y2;   x[3]=x3; y[3]=y3;}
		inline Quad2D(float left, float top, float right, float bottom) {x[0]=left; x[2]=left; y[0]=top; y[1]=top; x[1]=right; x[3]=right; y[2]=bottom; y[3]=bottom;}
	};

	// holds values that determine the coordinates of a 3-dimensional quadrilateral
	// Vector3DF vertice values in the following order: 
	class Quad3D {
	public:
		Vector3DF v[4]; // 4 vertices

		inline Quad3D() {}
		inline ~Quad3D() {}
		inline Quad3D(Vector3DF&v0,  Vector3DF&v1,	Vector3DF&v2,  Vector3DF&v3) {v[0]=v0; v[1]=v1; v[2]=v2; v[3]=v3;}
	};

	// holds the information necessary to save calls to DrawImageEx commands and draw them in sorted order
	class GameXDrawState {
	public:
		inline GameXDrawState() {next = NULL; prev = NULL;}
		inline ~GameXDrawState() {}

		GameXDrawState * next; // has next and previous fields so can act as a linked list element
		GameXDrawState * prev; // for efficient insertion into drawing list sorted by a merge-sort

		// these states are pointers, meaning they must remain valid across frames,
		// which hopefully is true since images shouldn't be created, drawn, destroyed each frame
		ImageX *				src; // source image
		ImageX *				dst; // destination image or VIEWPORT if drawing normally to screen
		CameraX *				cam; // camera used to project images in 3D

		// most of the following are not pointers
		// because otherwise the data used to define the state could be changed before the state is drawn
		// this should change though since most of the Set functions already make a copy
		int 					vertices_3D; // 0 for drawing to screen coords, 1 for drawing to 3D polygon coords, 2 for drawing to 3D point-sprite coords, 3 for drawing with auto-scaling to 3D point-sprite coords
		Quad2D					dst_quad2d; // used when draw_vertices_3D is 0
		Quad3D					dst_quad3d; // used when draw_vertices_3D is 1
		Vector3DF				dst_point3d; // used when draw_vertices_3D is 2
		RECT					src_rect; // where on our source to draw from (must be a rectangular texture)
		ColorX					colors; // what colors/transparency to shade the image with, white meaning default/no shading
		D3DTEXTUREOP			color_effect; // how to use the shading colors/transparency, default is multiply (D3DTOP_MODULATE)
		float					angle; // how many clockwise degrees to rotate whatever we draw as we draw it
		float					scalex, scaley; // factor to scale what we're drawing, horizontally and vertically (is done before rotation)
		float					transx, transy; // pixels of translation in x and y, for adjusting 3D drawing
		int 					warp_mode; // 1 if image warping on, 2 if auto-perspective-correction warping on, 0 if all warp values assumed 1.0f
		float					warp [4]; // how much to warp each vertex of the image, 1.0f is default, 0.0f is pushed in all the way, etc.
		DrawFlags				flags;
		float					x [4]; // x screen coords
		float					y [4]; // y screen coords
		float					z [4]; // screen depth sorting values for sorting and z-buffering
		float					depth2D; // z-depth of 2D objects
		float					zcomp; // combination of depths, for comparison when sorting
		RECT					dst_bounds; // clipping boundary
//		__int64 align; // unused -- here so GameXDrawState objects aligns to 32-byte boundaries
	};

#endif