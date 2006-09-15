// GameX (Release 2 - Vectors / Matricies / CameraX)
// Vector Class Header
// 
// Copyright (C) 2002 Rama C. Hoetzlein
// 
// This software is released under the GameX GNU GPL 
// Open Source Liscense. See the GameX documentation included
// with this source code for terms of modification, 
// distribution and re-release. 
//
//
// Vector Operations Implemented:
//		=, +, -, *, / (on vectors and scalars) (* is dot product on vectors)
//		Cross			Cross product vector with op
//		Dot				Dot product vector with op
//		Dist (op)		Distance from vector to op
//		DistSq			Distance^2 from vector to op
//		Length ()		Length of vector
//		LengthSq ()		Length^2 of vector
//		Normalize ()	Normalizes vector
//

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <memory.h>
#include <math.h>

#ifndef VECTOR_DEF
	#define VECTOR_DEF

											// **** Switches	
	#define VECTOR_DEBUG					// Enable Debugging Output
	#define VECTOR_INITIALIZE				// Initializes vectors	
																// Forward Referencing
	class Vector2DI;
	class Vector2DF;
	class Vector3DI;
	class Vector3DF;
	class Vector4DF;
	class MatrixF;

	// Vector2DI Declaration

	#define VNAME		2DI
	#define VTYPE		int

	class Vector2DI {
	public:
		VTYPE x, y;

		// Constructors/Destructors
		inline Vector2DI();							
		inline ~Vector2DI();			
		inline Vector2DI (VTYPE xa, VTYPE ya);
		inline Vector2DI (const Vector2DI &op);				
		inline Vector2DI (const Vector2DF &op);				
		inline Vector2DI (const Vector3DI &op);				
		inline Vector2DI (const Vector3DF &op);				
		inline Vector2DI (const Vector4DF &op);

		// Member Functions
		inline Vector2DI &operator= (const Vector2DI &op);
		inline Vector2DI &operator= (const Vector2DF &op);
		inline Vector2DI &operator= (const Vector3DI &op);
		inline Vector2DI &operator= (const Vector3DF &op);
		inline Vector2DI &operator= (const Vector4DF &op);

		inline Vector2DI &operator+= (const Vector2DI &op);
		inline Vector2DI &operator+= (const Vector2DF &op);
		inline Vector2DI &operator+= (const Vector3DI &op);
		inline Vector2DI &operator+= (const Vector3DF &op);
		inline Vector2DI &operator+= (const Vector4DF &op);

		inline Vector2DI  operator+  (const Vector2DI &op) {Vector2DI v = Vector2DI(*this); v += op; return v;}
		inline Vector2DI  operator+  (const Vector2DF &op) {Vector2DI v = Vector2DI(*this); v += op; return v;}
		inline Vector2DI  operator+  (const Vector3DI &op) {Vector2DI v = Vector2DI(*this); v += op; return v;}
		inline Vector2DI  operator+  (const Vector3DF &op) {Vector2DI v = Vector2DI(*this); v += op; return v;}
		inline Vector2DI  operator+  (const Vector4DF &op) {Vector2DI v = Vector2DI(*this); v += op; return v;}

		inline Vector2DI &operator-= (const Vector2DI &op);
		inline Vector2DI &operator-= (const Vector2DF &op);
		inline Vector2DI &operator-= (const Vector3DI &op);
		inline Vector2DI &operator-= (const Vector3DF &op);
		inline Vector2DI &operator-= (const Vector4DF &op);
	
		inline Vector2DI  operator-  (const Vector2DI &op) {Vector2DI v = Vector2DI(*this); v -= op; return v;}
		inline Vector2DI  operator-  (const Vector2DF &op) {Vector2DI v = Vector2DI(*this); v -= op; return v;}
		inline Vector2DI  operator-  (const Vector3DI &op) {Vector2DI v = Vector2DI(*this); v -= op; return v;}
		inline Vector2DI  operator-  (const Vector3DF &op) {Vector2DI v = Vector2DI(*this); v -= op; return v;}
		inline Vector2DI  operator-  (const Vector4DF &op) {Vector2DI v = Vector2DI(*this); v -= op; return v;}

		inline Vector2DI &operator*= (const Vector2DI &op);
		inline Vector2DI &operator*= (const Vector2DF &op);
		inline Vector2DI &operator*= (const Vector3DI &op);
		inline Vector2DI &operator*= (const Vector3DF &op);
		inline Vector2DI &operator*= (const Vector4DF &op);

		inline Vector2DI  operator*  (const Vector2DI &op) {Vector2DI v = Vector2DI(*this); v *= op; return v;}
		inline Vector2DI  operator*  (const Vector2DF &op) {Vector2DI v = Vector2DI(*this); v *= op; return v;}
		inline Vector2DI  operator*  (const Vector3DI &op) {Vector2DI v = Vector2DI(*this); v *= op; return v;}
		inline Vector2DI  operator*  (const Vector3DF &op) {Vector2DI v = Vector2DI(*this); v *= op; return v;}
		inline Vector2DI  operator*  (const Vector4DF &op) {Vector2DI v = Vector2DI(*this); v *= op; return v;}

		inline Vector2DI &operator/= (const Vector2DI &op);
		inline Vector2DI &operator/= (const Vector2DF &op);
		inline Vector2DI &operator/= (const Vector3DI &op);
		inline Vector2DI &operator/= (const Vector3DF &op);
		inline Vector2DI &operator/= (const Vector4DF &op);

		inline Vector2DI  operator/  (const Vector2DI &op) {Vector2DI v = Vector2DI(*this); v /= op; return v;}
		inline Vector2DI  operator/  (const Vector2DF &op) {Vector2DI v = Vector2DI(*this); v /= op; return v;}
		inline Vector2DI  operator/  (const Vector3DI &op) {Vector2DI v = Vector2DI(*this); v /= op; return v;}
		inline Vector2DI  operator/  (const Vector3DF &op) {Vector2DI v = Vector2DI(*this); v /= op; return v;}
		inline Vector2DI  operator/  (const Vector4DF &op) {Vector2DI v = Vector2DI(*this); v /= op; return v;}

		// Note: The "2D cross product" is the equivalent of rotating 90 degrees CCW and and taking the dot product
		inline float Cross (const Vector2DI &v);
		inline float Cross (const Vector2DF &v);
		
		inline float Dot (const Vector2DI &v);
		inline float Dot (const Vector2DF &v);

		inline float Dist (const Vector2DI &v);
		inline float Dist (const Vector2DF &v);
		inline float Dist (const Vector3DI &v);
		inline float Dist (const Vector3DF &v);
		inline float Dist (const Vector4DF &v);

		inline float DistSq (const Vector2DI &v);
		inline float DistSq (const Vector2DF &v);
		inline float DistSq (const Vector3DI &v);
		inline float DistSq (const Vector3DF &v);
		inline float DistSq (const Vector4DF &v);
		
		inline Vector2DI &Normalize (void);
		inline float Length (void);
		inline float LengthSq (void);

		inline VTYPE &X(void);
		inline VTYPE &Y(void);
		inline VTYPE Z(void);
		inline VTYPE W(void);
		inline const VTYPE &X(void) const;
		inline const VTYPE &Y(void) const;
		inline const VTYPE Z(void) const;
		inline const VTYPE W(void) const;
		inline VTYPE *Data (void);
	};
	
	#undef VNAME
	#undef VTYPE

	// Vector2DF Declarations

	#define VNAME		2DF
	#define VTYPE		float

	class Vector2DF {
	public:
		VTYPE x, y;

		// Constructors/Destructors
		inline Vector2DF ();
		inline ~Vector2DF ();
		inline Vector2DF (VTYPE xa, VTYPE ya);
		inline Vector2DF (const Vector2DI &op);
		inline Vector2DF (const Vector2DF &op);
		inline Vector2DF (const Vector3DI &op);
		inline Vector2DF (const Vector3DF &op);
		inline Vector2DF (const Vector4DF &op);

		// Member Functions
		inline Vector2DF &operator= (const Vector2DI &op);
		inline Vector2DF &operator= (const Vector2DF &op);
		inline Vector2DF &operator= (const Vector3DI &op);
		inline Vector2DF &operator= (const Vector3DF &op);
		inline Vector2DF &operator= (const Vector4DF &op);
		
		inline Vector2DF &operator+= (const Vector2DI &op);
		inline Vector2DF &operator+= (const Vector2DF &op);
		inline Vector2DF &operator+= (const Vector3DI &op);
		inline Vector2DF &operator+= (const Vector3DF &op);
		inline Vector2DF &operator+= (const Vector4DF &op);

		inline Vector2DF  operator+  (const Vector2DI &op) {Vector2DF v = Vector2DF(*this); v += op; return v;}
		inline Vector2DF  operator+  (const Vector2DF &op) {Vector2DF v = Vector2DF(*this); v += op; return v;}
		inline Vector2DF  operator+  (const Vector3DI &op) {Vector2DF v = Vector2DF(*this); v += op; return v;}
		inline Vector2DF  operator+  (const Vector3DF &op) {Vector2DF v = Vector2DF(*this); v += op; return v;}
		inline Vector2DF  operator+  (const Vector4DF &op) {Vector2DF v = Vector2DF(*this); v += op; return v;}

		inline Vector2DF &operator-= (const Vector2DI &op);
		inline Vector2DF &operator-= (const Vector2DF &op);
		inline Vector2DF &operator-= (const Vector3DI &op);
		inline Vector2DF &operator-= (const Vector3DF &op);
		inline Vector2DF &operator-= (const Vector4DF &op);

		inline Vector2DF  operator-  (const Vector2DI &op) {Vector2DF v = Vector2DF(*this); v -= op; return v;}
		inline Vector2DF  operator-  (const Vector2DF &op) {Vector2DF v = Vector2DF(*this); v -= op; return v;}
		inline Vector2DF  operator-  (const Vector3DI &op) {Vector2DF v = Vector2DF(*this); v -= op; return v;}
		inline Vector2DF  operator-  (const Vector3DF &op) {Vector2DF v = Vector2DF(*this); v -= op; return v;}
		inline Vector2DF  operator-  (const Vector4DF &op) {Vector2DF v = Vector2DF(*this); v -= op; return v;}

		inline Vector2DF &operator*= (int op);
		inline Vector2DF &operator*= (float op);
		inline Vector2DF &operator*= (const Vector2DI &op);
		inline Vector2DF &operator*= (const Vector2DF &op);
		inline Vector2DF &operator*= (const Vector3DI &op);
		inline Vector2DF &operator*= (const Vector3DF &op);
		inline Vector2DF &operator*= (const Vector4DF &op);

		inline Vector2DF  operator*  (int op)              {Vector2DF v = Vector2DF(*this); v *= op; return v;}
		inline Vector2DF  operator*  (float op)            {Vector2DF v = Vector2DF(*this); v *= op; return v;}
		inline Vector2DF  operator*  (const Vector2DI &op) {Vector2DF v = Vector2DF(*this); v *= op; return v;}
		inline Vector2DF  operator*  (const Vector2DF &op) {Vector2DF v = Vector2DF(*this); v *= op; return v;}
		inline Vector2DF  operator*  (const Vector3DI &op) {Vector2DF v = Vector2DF(*this); v *= op; return v;}
		inline Vector2DF  operator*  (const Vector3DF &op) {Vector2DF v = Vector2DF(*this); v *= op; return v;}
		inline Vector2DF  operator*  (const Vector4DF &op) {Vector2DF v = Vector2DF(*this); v *= op; return v;}

		inline Vector2DF &operator/= (int op);
		inline Vector2DF &operator/= (float op);
		inline Vector2DF &operator/= (const Vector2DI &op);
		inline Vector2DF &operator/= (const Vector2DF &op);
		inline Vector2DF &operator/= (const Vector3DI &op);
		inline Vector2DF &operator/= (const Vector3DF &op);
		inline Vector2DF &operator/= (const Vector4DF &op);

		inline Vector2DF  operator/  (int op)              {Vector2DF v = Vector2DF(*this); v /= op; return v;}
		inline Vector2DF  operator/  (float op)            {Vector2DF v = Vector2DF(*this); v /= op; return v;}
		inline Vector2DF  operator/  (const Vector2DI &op) {Vector2DF v = Vector2DF(*this); v /= op; return v;}
		inline Vector2DF  operator/  (const Vector2DF &op) {Vector2DF v = Vector2DF(*this); v /= op; return v;}
		inline Vector2DF  operator/  (const Vector3DI &op) {Vector2DF v = Vector2DF(*this); v /= op; return v;}
		inline Vector2DF  operator/  (const Vector3DF &op) {Vector2DF v = Vector2DF(*this); v /= op; return v;}
		inline Vector2DF  operator/  (const Vector4DF &op) {Vector2DF v = Vector2DF(*this); v /= op; return v;}

		// Note: The "2D cross product" is the equivalent of rotating 90 degrees CCW and and taking the dot product
		inline float Cross (const Vector2DI &v);
		inline float Cross (const Vector2DF &v);
		
		inline float Dot (const Vector2DI &v);
		inline float Dot (const Vector2DF &v);

		inline float Dist (const Vector2DI &v);
		inline float Dist (const Vector2DF &v);
		inline float Dist (const Vector3DI &v);
		inline float Dist (const Vector3DF &v);
		inline float Dist (const Vector4DF &v);

		inline float DistSq (const Vector2DI &v);
		inline float DistSq (const Vector2DF &v);
		inline float DistSq (const Vector3DI &v);
		inline float DistSq (const Vector3DF &v);
		inline float DistSq (const Vector4DF &v);

		inline Vector2DF &Normalize (void);
		inline float Length (void);
		inline float LengthSq (void);

		inline VTYPE &X(void);
		inline VTYPE &Y(void);
		inline VTYPE Z(void);
		inline VTYPE W(void);
		inline const VTYPE &X(void) const;
		inline const VTYPE &Y(void) const;
		inline const VTYPE Z(void) const;
		inline const VTYPE W(void) const;
		inline VTYPE *Data (void);
	};
	
	#undef VNAME
	#undef VTYPE

	// Vector3DI Declaration

	#define VNAME		3DI
	#define VTYPE		int

	class Vector3DI {
	public:
		VTYPE x, y, z;
	
		// Constructors/Destructors
		inline Vector3DI();
		inline ~Vector3DI();
		inline Vector3DI (VTYPE xa, VTYPE ya, VTYPE za);
		inline Vector3DI (const Vector2DI &op);
		inline Vector3DI (const Vector2DF &op);
		inline Vector3DI (const Vector3DI &op);
		inline Vector3DI (const Vector3DF &op);
		inline Vector3DI (const Vector4DF &op);

		// Member Functions
		inline Vector3DI &operator= (const Vector2DI &op);
		inline Vector3DI &operator= (const Vector2DF &op);
		inline Vector3DI &operator= (const Vector3DI &op);
		inline Vector3DI &operator= (const Vector3DF &op);
		inline Vector3DI &operator= (const Vector4DF &op);
		
		inline Vector3DI &operator+= (const Vector2DI &op);
		inline Vector3DI &operator+= (const Vector2DF &op);
		inline Vector3DI &operator+= (const Vector3DI &op);
		inline Vector3DI &operator+= (const Vector3DF &op);
		inline Vector3DI &operator+= (const Vector4DF &op);

		inline Vector3DI  operator+  (const Vector2DI &op) {Vector3DI v = Vector3DI(*this); v += op; return v;}
		inline Vector3DI  operator+  (const Vector2DF &op) {Vector3DI v = Vector3DI(*this); v += op; return v;}
		inline Vector3DI  operator+  (const Vector3DI &op) {Vector3DI v = Vector3DI(*this); v += op; return v;}
		inline Vector3DI  operator+  (const Vector3DF &op) {Vector3DI v = Vector3DI(*this); v += op; return v;}
		inline Vector3DI  operator+  (const Vector4DF &op) {Vector3DI v = Vector3DI(*this); v += op; return v;}

		inline Vector3DI &operator-= (const Vector2DI &op);
		inline Vector3DI &operator-= (const Vector2DF &op);
		inline Vector3DI &operator-= (const Vector3DI &op);
		inline Vector3DI &operator-= (const Vector3DF &op);
		inline Vector3DI &operator-= (const Vector4DF &op);
	
		inline Vector3DI  operator-  (const Vector2DI &op) {Vector3DI v = Vector3DI(*this); v -= op; return v;}
		inline Vector3DI  operator-  (const Vector2DF &op) {Vector3DI v = Vector3DI(*this); v -= op; return v;}
		inline Vector3DI  operator-  (const Vector3DI &op) {Vector3DI v = Vector3DI(*this); v -= op; return v;}
		inline Vector3DI  operator-  (const Vector3DF &op) {Vector3DI v = Vector3DI(*this); v -= op; return v;}
		inline Vector3DI  operator-  (const Vector4DF &op) {Vector3DI v = Vector3DI(*this); v -= op; return v;}

		inline Vector3DI &operator*= (const Vector2DI &op);
		inline Vector3DI &operator*= (const Vector2DF &op);
		inline Vector3DI &operator*= (const Vector3DI &op);
		inline Vector3DI &operator*= (const Vector3DF &op);
		inline Vector3DI &operator*= (const Vector4DF &op);

		inline Vector3DI  operator*  (const Vector2DI &op) {Vector3DI v = Vector3DI(*this); v *= op; return v;}
		inline Vector3DI  operator*  (const Vector2DF &op) {Vector3DI v = Vector3DI(*this); v *= op; return v;}
		inline Vector3DI  operator*  (const Vector3DI &op) {Vector3DI v = Vector3DI(*this); v *= op; return v;}
		inline Vector3DI  operator*  (const Vector3DF &op) {Vector3DI v = Vector3DI(*this); v *= op; return v;}
		inline Vector3DI  operator*  (const Vector4DF &op) {Vector3DI v = Vector3DI(*this); v *= op; return v;}

		inline Vector3DI &operator/= (const Vector2DI &op);
		inline Vector3DI &operator/= (const Vector2DF &op);
		inline Vector3DI &operator/= (const Vector3DI &op);
		inline Vector3DI &operator/= (const Vector3DF &op);
		inline Vector3DI &operator/= (const Vector4DF &op);

		inline Vector3DI  operator/  (const Vector2DI &op) {Vector3DI v = Vector3DI(*this); v /= op; return v;}
		inline Vector3DI  operator/  (const Vector2DF &op) {Vector3DI v = Vector3DI(*this); v /= op; return v;}
		inline Vector3DI  operator/  (const Vector3DI &op) {Vector3DI v = Vector3DI(*this); v /= op; return v;}
		inline Vector3DI  operator/  (const Vector3DF &op) {Vector3DI v = Vector3DI(*this); v /= op; return v;}
		inline Vector3DI  operator/  (const Vector4DF &op) {Vector3DI v = Vector3DI(*this); v /= op; return v;}

		inline Vector3DI &Cross (const Vector3DI &v);
		inline Vector3DI &Cross (const Vector3DF &v);	
		
		inline float Dot (const Vector3DI &v);
		inline float Dot (const Vector3DF &v);

		inline float Dist (const Vector2DI &v);
		inline float Dist (const Vector2DF &v);
		inline float Dist (const Vector3DI &v);
		inline float Dist (const Vector3DF &v);
		inline float Dist (const Vector4DF &v);

		inline float DistSq (const Vector2DI &v);
		inline float DistSq (const Vector2DF &v);
		inline float DistSq (const Vector3DI &v);
		inline float DistSq (const Vector3DF &v);
		inline float DistSq (const Vector4DF &v);

		inline Vector3DI &Normalize (void);
		inline float Length (void);
		inline float LengthSq (void);

		inline VTYPE &X(void);
		inline VTYPE &Y(void);
		inline VTYPE &Z(void);
		inline VTYPE W(void);
		inline const VTYPE &X(void) const;
		inline const VTYPE &Y(void) const;
		inline const VTYPE &Z(void) const;
		inline const VTYPE W(void) const;
		inline VTYPE *Data (void);
	};
	
	#undef VNAME
	#undef VTYPE

	// Vector3DF Declarations

	#define VNAME		3DF
	#define VTYPE		float

	class Vector3DF {
	public:
		VTYPE x, y, z;
	
		// Constructors/Destructors
		inline Vector3DF();
		inline ~Vector3DF();
		inline Vector3DF (VTYPE xa, VTYPE ya, VTYPE za);
		inline Vector3DF (const Vector2DI &op);
		inline Vector3DF (const Vector2DF &op);
		inline Vector3DF (const Vector3DI &op);
		inline Vector3DF (const Vector3DF &op);
		inline Vector3DF (const Vector4DF &op);

		// Set Functions
		inline Vector3DF &Set (float xa, float ya, float za);
		
		// Member Functions
		inline Vector3DF &operator= (const Vector2DI &op);
		inline Vector3DF &operator= (const Vector2DF &op);
		inline Vector3DF &operator= (const Vector3DI &op);
		inline Vector3DF &operator= (const Vector3DF &op);
		inline Vector3DF &operator= (const Vector4DF &op);

		inline Vector3DF &operator+= (const Vector2DI &op);
		inline Vector3DF &operator+= (const Vector2DF &op);
		inline Vector3DF &operator+= (const Vector3DI &op);
		inline Vector3DF &operator+= (const Vector3DF &op);
		inline Vector3DF &operator+= (const Vector4DF &op);

		inline Vector3DF  operator+  (const Vector2DI &op) {Vector3DF v = Vector3DF(*this); v += op; return v;}
		inline Vector3DF  operator+  (const Vector2DF &op) {Vector3DF v = Vector3DF(*this); v += op; return v;}
		inline Vector3DF  operator+  (const Vector3DI &op) {Vector3DF v = Vector3DF(*this); v += op; return v;}
		inline Vector3DF  operator+  (const Vector3DF &op) {Vector3DF v = Vector3DF(*this); v += op; return v;}
		inline Vector3DF  operator+  (const Vector4DF &op) {Vector3DF v = Vector3DF(*this); v += op; return v;}

		inline Vector3DF &operator-= (const Vector2DI &op);
		inline Vector3DF &operator-= (const Vector2DF &op);
		inline Vector3DF &operator-= (const Vector3DI &op);
		inline Vector3DF &operator-= (const Vector3DF &op);
		inline Vector3DF &operator-= (const Vector4DF &op);
	
		inline Vector3DF  operator-  (const Vector2DI &op) {Vector3DF v = Vector3DF(*this); v -= op; return v;}
		inline Vector3DF  operator-  (const Vector2DF &op) {Vector3DF v = Vector3DF(*this); v -= op; return v;}
		inline Vector3DF  operator-  (const Vector3DI &op) {Vector3DF v = Vector3DF(*this); v -= op; return v;}
		inline Vector3DF  operator-  (const Vector3DF &op) {Vector3DF v = Vector3DF(*this); v -= op; return v;}
		inline Vector3DF  operator-  (const Vector4DF &op) {Vector3DF v = Vector3DF(*this); v -= op; return v;}

		inline Vector3DF &operator*= (int op);
		inline Vector3DF &operator*= (float op);
		inline Vector3DF &operator*= (const Vector2DI &op);
		inline Vector3DF &operator*= (const Vector2DF &op);
		inline Vector3DF &operator*= (const Vector3DI &op);
		inline Vector3DF &operator*= (const Vector3DF &op);
		inline Vector3DF &operator*= (const Vector4DF &op);
		Vector3DF &operator*= (MatrixF &op);				// see vector.cpp

		inline Vector3DF  operator*  (int op)              {Vector3DF v = Vector3DF(*this); v *= op; return v;}
		inline Vector3DF  operator*  (float op)            {Vector3DF v = Vector3DF(*this); v *= op; return v;}
		inline Vector3DF  operator*  (const Vector2DI &op) {Vector3DF v = Vector3DF(*this); v *= op; return v;}
		inline Vector3DF  operator*  (const Vector2DF &op) {Vector3DF v = Vector3DF(*this); v *= op; return v;}
		inline Vector3DF  operator*  (const Vector3DI &op) {Vector3DF v = Vector3DF(*this); v *= op; return v;}
		inline Vector3DF  operator*  (const Vector3DF &op) {Vector3DF v = Vector3DF(*this); v *= op; return v;}
		inline Vector3DF  operator*  (const Vector4DF &op) {Vector3DF v = Vector3DF(*this); v *= op; return v;}
		Vector3DF  operator*  (MatrixF &op)                {Vector3DF v = Vector3DF(*this); v *= op; return v;}

		inline Vector3DF &operator/= (int op);
		inline Vector3DF &operator/= (float op);
		inline Vector3DF &operator/= (const Vector2DI &op);
		inline Vector3DF &operator/= (const Vector2DF &op);
		inline Vector3DF &operator/= (const Vector3DI &op);
		inline Vector3DF &operator/= (const Vector3DF &op);
		inline Vector3DF &operator/= (const Vector4DF &op);

		inline Vector3DF  operator/  (int op)              {Vector3DF v = Vector3DF(*this); v /= op; return v;}
		inline Vector3DF  operator/  (float op)            {Vector3DF v = Vector3DF(*this); v /= op; return v;}
		inline Vector3DF  operator/  (const Vector2DI &op) {Vector3DF v = Vector3DF(*this); v /= op; return v;}
		inline Vector3DF  operator/  (const Vector2DF &op) {Vector3DF v = Vector3DF(*this); v /= op; return v;}
		inline Vector3DF  operator/  (const Vector3DI &op) {Vector3DF v = Vector3DF(*this); v /= op; return v;}
		inline Vector3DF  operator/  (const Vector3DF &op) {Vector3DF v = Vector3DF(*this); v /= op; return v;}
		inline Vector3DF  operator/  (const Vector4DF &op) {Vector3DF v = Vector3DF(*this); v /= op; return v;}

		inline Vector3DF &Cross (const Vector3DI &v);
		inline Vector3DF &Cross (const Vector3DF &v);	
		
		inline float Dot (const Vector3DI &v);
		inline float Dot (const Vector3DF &v);

		inline float Dist (const Vector2DI &v);
		inline float Dist (const Vector2DF &v);
		inline float Dist (const Vector3DI &v);
		inline float Dist (const Vector3DF &v);
		inline float Dist (const Vector4DF &v);

		inline float DistSq (const Vector2DI &v);
		inline float DistSq (const Vector2DF &v);
		inline float DistSq (const Vector3DI &v);
		inline float DistSq (const Vector3DF &v);
		inline float DistSq (const Vector4DF &v);

		inline Vector3DF &Normalize (void);
		inline float Length (void);
		inline float LengthSq (void);

		inline VTYPE &X(void);
		inline VTYPE &Y(void);
		inline VTYPE &Z(void);
		inline VTYPE W(void);
		inline const VTYPE &X(void) const;
		inline const VTYPE &Y(void) const;
		inline const VTYPE &Z(void) const;
		inline const VTYPE W(void) const;
		inline VTYPE *Data (void);
	};
	
	#undef VNAME
	#undef VTYPE

	// Vector4DF Declarations

	#define VNAME		4DF
	#define VTYPE		float

	class Vector4DF {
	public:
		VTYPE x, y, z, w;
	
		// Constructors/Destructors
		inline Vector4DF();
		inline ~Vector4DF();
		inline Vector4DF (VTYPE xa, VTYPE ya, VTYPE za, VTYPE wa);
		inline Vector4DF (const Vector2DI &op);
		inline Vector4DF (const Vector2DF &op);
		inline Vector4DF (const Vector3DI &op);
		inline Vector4DF (const Vector3DF &op);
		inline Vector4DF (const Vector4DF &op);

		// Member Functions
		inline Vector4DF &operator= (const Vector2DI &op);
		inline Vector4DF &operator= (const Vector2DF &op);
		inline Vector4DF &operator= (const Vector3DI &op);
		inline Vector4DF &operator= (const Vector3DF &op);
		inline Vector4DF &operator= (const Vector4DF &op);

		inline Vector4DF &operator+= (const Vector2DI &op);
		inline Vector4DF &operator+= (const Vector2DF &op);
		inline Vector4DF &operator+= (const Vector3DI &op);
		inline Vector4DF &operator+= (const Vector3DF &op);
		inline Vector4DF &operator+= (const Vector4DF &op);

		inline Vector4DF  operator+  (const Vector2DI &op) {Vector4DF v = Vector4DF(*this); v += op; return v;}
		inline Vector4DF  operator+  (const Vector2DF &op) {Vector4DF v = Vector4DF(*this); v += op; return v;}
		inline Vector4DF  operator+  (const Vector3DI &op) {Vector4DF v = Vector4DF(*this); v += op; return v;}
		inline Vector4DF  operator+  (const Vector3DF &op) {Vector4DF v = Vector4DF(*this); v += op; return v;}
		inline Vector4DF  operator+  (const Vector4DF &op) {Vector4DF v = Vector4DF(*this); v += op; return v;}

		inline Vector4DF &operator-= (const Vector2DI &op);
		inline Vector4DF &operator-= (const Vector2DF &op);
		inline Vector4DF &operator-= (const Vector3DI &op);
		inline Vector4DF &operator-= (const Vector3DF &op);
		inline Vector4DF &operator-= (const Vector4DF &op);
	
		inline Vector4DF  operator-  (const Vector2DI &op) {Vector4DF v = Vector4DF(*this); v -= op; return v;}
		inline Vector4DF  operator-  (const Vector2DF &op) {Vector4DF v = Vector4DF(*this); v -= op; return v;}
		inline Vector4DF  operator-  (const Vector3DI &op) {Vector4DF v = Vector4DF(*this); v -= op; return v;}
		inline Vector4DF  operator-  (const Vector3DF &op) {Vector4DF v = Vector4DF(*this); v -= op; return v;}
		inline Vector4DF  operator-  (const Vector4DF &op) {Vector4DF v = Vector4DF(*this); v -= op; return v;}

		inline Vector4DF &operator*= (int op);
		inline Vector4DF &operator*= (float op);
		inline Vector4DF &operator*= (const Vector2DI &op);
		inline Vector4DF &operator*= (const Vector2DF &op);
		inline Vector4DF &operator*= (const Vector3DI &op);
		inline Vector4DF &operator*= (const Vector3DF &op);
		inline Vector4DF &operator*= (const Vector4DF &op);
		Vector4DF &operator*= (MatrixF &op);				// see vector.cpp

		inline Vector4DF  operator*  (int op)              {Vector4DF v = Vector4DF(*this); v *= op; return v;}
		inline Vector4DF  operator*  (float op)            {Vector4DF v = Vector4DF(*this); v *= op; return v;}
		inline Vector4DF  operator*  (const Vector2DI &op) {Vector4DF v = Vector4DF(*this); v *= op; return v;}
		inline Vector4DF  operator*  (const Vector2DF &op) {Vector4DF v = Vector4DF(*this); v *= op; return v;}
		inline Vector4DF  operator*  (const Vector3DI &op) {Vector4DF v = Vector4DF(*this); v *= op; return v;}
		inline Vector4DF  operator*  (const Vector3DF &op) {Vector4DF v = Vector4DF(*this); v *= op; return v;}
		inline Vector4DF  operator*  (const Vector4DF &op) {Vector4DF v = Vector4DF(*this); v *= op; return v;}
		Vector4DF  operator*  (MatrixF &op)                {Vector4DF v = Vector4DF(*this); v *= op; return v;}

		inline Vector4DF &operator/= (int op);
		inline Vector4DF &operator/= (float op);
		inline Vector4DF &operator/= (const Vector2DI &op);
		inline Vector4DF &operator/= (const Vector2DF &op);
		inline Vector4DF &operator/= (const Vector3DI &op);
		inline Vector4DF &operator/= (const Vector3DF &op);
		inline Vector4DF &operator/= (const Vector4DF &op);

		inline Vector4DF  operator/  (int op)              {Vector4DF v = Vector4DF(*this); v /= op; return v;}
		inline Vector4DF  operator/  (float op)            {Vector4DF v = Vector4DF(*this); v /= op; return v;}
		inline Vector4DF  operator/  (const Vector2DI &op) {Vector4DF v = Vector4DF(*this); v /= op; return v;}
		inline Vector4DF  operator/  (const Vector2DF &op) {Vector4DF v = Vector4DF(*this); v /= op; return v;}
		inline Vector4DF  operator/  (const Vector3DI &op) {Vector4DF v = Vector4DF(*this); v /= op; return v;}
		inline Vector4DF  operator/  (const Vector3DF &op) {Vector4DF v = Vector4DF(*this); v /= op; return v;}
		inline Vector4DF  operator/  (const Vector4DF &op) {Vector4DF v = Vector4DF(*this); v /= op; return v;}

		Vector4DF &Cross (const Vector4DF &v, const Vector4DF &v2);	// see vector.cpp
		
		inline float Dot(const Vector4DF &v);

		inline float Dist (const Vector4DF &v);

		inline float DistSq (const Vector4DF &v);

		inline Vector4DF &Normalize (void);
		inline float Length (void);
		inline float LengthSq (void);

		inline VTYPE &X(void);
		inline VTYPE &Y(void);
		inline VTYPE &Z(void);
		inline VTYPE &W(void);
		inline const VTYPE &X(void) const;
		inline const VTYPE &Y(void) const;
		inline const VTYPE &Z(void) const;
		inline const VTYPE &W(void) const;
		inline VTYPE *Data (void);
	};
	
	#undef VNAME
	#undef VTYPE



    // Vector Code Definitions (Inlined)


	// Vector2DI Code Definition

	#define VNAME		2DI
	#define VTYPE		int

	// Constructors/Destructors
	inline Vector2DI::Vector2DI() {x=0; y=0;}
	inline Vector2DI::~Vector2DI() {}
	inline Vector2DI::Vector2DI (VTYPE xa, VTYPE ya) {x=xa; y=ya;}
	inline Vector2DI::Vector2DI (const Vector2DI &op) {x=(VTYPE) op.x; y=(VTYPE) op.y;}
	inline Vector2DI::Vector2DI (const Vector2DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y;}
	inline Vector2DI::Vector2DI (const Vector3DI &op) {x=(VTYPE) op.x; y=(VTYPE) op.y;}
	inline Vector2DI::Vector2DI (const Vector3DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y;}
	inline Vector2DI::Vector2DI (const Vector4DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y;}

	// Member Functions
	inline Vector2DI &Vector2DI::operator= (const Vector2DI &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; return *this;}
	inline Vector2DI &Vector2DI::operator= (const Vector2DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; return *this;}
	inline Vector2DI &Vector2DI::operator= (const Vector3DI &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; return *this;}
	inline Vector2DI &Vector2DI::operator= (const Vector3DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; return *this;}
	inline Vector2DI &Vector2DI::operator= (const Vector4DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; return *this;}	
		
	inline Vector2DI &Vector2DI::operator+= (const Vector2DI &op) {x+=(VTYPE) op.x; y+=(VTYPE) op.y; return *this;}
	inline Vector2DI &Vector2DI::operator+= (const Vector2DF &op) {x+=(VTYPE) op.x; y+=(VTYPE) op.y; return *this;}
	inline Vector2DI &Vector2DI::operator+= (const Vector3DI &op) {x+=(VTYPE) op.x; y+=(VTYPE) op.y; return *this;}
	inline Vector2DI &Vector2DI::operator+= (const Vector3DF &op) {x+=(VTYPE) op.x; y+=(VTYPE) op.y; return *this;}
	inline Vector2DI &Vector2DI::operator+= (const Vector4DF &op) {x+=(VTYPE) op.x; y+=(VTYPE) op.y; return *this;}

	inline Vector2DI &Vector2DI::operator-= (const Vector2DI &op) {x-=(VTYPE) op.x; y-=(VTYPE) op.y; return *this;}
	inline Vector2DI &Vector2DI::operator-= (const Vector2DF &op) {x-=(VTYPE) op.x; y-=(VTYPE) op.y; return *this;}
	inline Vector2DI &Vector2DI::operator-= (const Vector3DI &op) {x-=(VTYPE) op.x; y-=(VTYPE) op.y; return *this;}
	inline Vector2DI &Vector2DI::operator-= (const Vector3DF &op) {x-=(VTYPE) op.x; y-=(VTYPE) op.y; return *this;}
	inline Vector2DI &Vector2DI::operator-= (const Vector4DF &op) {x-=(VTYPE) op.x; y-=(VTYPE) op.y; return *this;}
		
	inline Vector2DI &Vector2DI::operator*= (const Vector2DI &op) {x*=(VTYPE) op.x; y*=(VTYPE) op.y; return *this;}
	inline Vector2DI &Vector2DI::operator*= (const Vector2DF &op) {x*=(VTYPE) op.x; y*=(VTYPE) op.y; return *this;}
	inline Vector2DI &Vector2DI::operator*= (const Vector3DI &op) {x*=(VTYPE) op.x; y*=(VTYPE) op.y; return *this;}
	inline Vector2DI &Vector2DI::operator*= (const Vector3DF &op) {x*=(VTYPE) op.x; y*=(VTYPE) op.y; return *this;}
	inline Vector2DI &Vector2DI::operator*= (const Vector4DF &op) {x*=(VTYPE) op.x; y*=(VTYPE) op.y; return *this;}

	inline Vector2DI &Vector2DI::operator/= (const Vector2DI &op) {x/=(VTYPE) op.x; y/=(VTYPE) op.y; return *this;}
	inline Vector2DI &Vector2DI::operator/= (const Vector2DF &op) {x/=(VTYPE) op.x; y/=(VTYPE) op.y; return *this;}
	inline Vector2DI &Vector2DI::operator/= (const Vector3DI &op) {x/=(VTYPE) op.x; y/=(VTYPE) op.y; return *this;}
	inline Vector2DI &Vector2DI::operator/= (const Vector3DF &op) {x/=(VTYPE) op.x; y/=(VTYPE) op.y; return *this;}
	inline Vector2DI &Vector2DI::operator/= (const Vector4DF &op) {x/=(VTYPE) op.x; y/=(VTYPE) op.y; return *this;}

	inline float Vector2DI::Cross (const Vector2DI &v)      {return (float)y*(float)v.x + (float)(-x)*(float)v.y;}
	inline float Vector2DI::Cross (const Vector2DF &v)      {return (float)y*(float)v.x + (float)(-x)*(float)v.y;}
			
	inline float Vector2DI::Dot(const Vector2DI &v)			{return (float) x*v.x + (float) y*v.y;}
	inline float Vector2DI::Dot(const Vector2DF &v)			{return (float) x*v.x + (float) y*v.y;}

	inline float Vector2DI::Dist (const Vector2DI &v)		{ float distsq = DistSq (v); if (distsq!=0) return sqrtf(distsq); return 0.0f;}
	inline float Vector2DI::Dist (const Vector2DF &v)		{ float distsq = DistSq (v); if (distsq!=0) return sqrtf(distsq); return 0.0f;}
	inline float Vector2DI::Dist (const Vector3DI &v)		{ float distsq = DistSq (v); if (distsq!=0) return sqrtf(distsq); return 0.0f;}
	inline float Vector2DI::Dist (const Vector3DF &v)		{ float distsq = DistSq (v); if (distsq!=0) return sqrtf(distsq); return 0.0f;}
	inline float Vector2DI::Dist (const Vector4DF &v)		{ float distsq = DistSq (v); if (distsq!=0) return sqrtf(distsq); return 0.0f;}
	inline float Vector2DI::DistSq (const Vector2DI &v)		{ float a,b; a = (float) x - (float) v.x; b = (float) y - (float) v.y; return (a*a + b*b);}
	inline float Vector2DI::DistSq (const Vector2DF &v)		{ float a,b; a = (float) x - (float) v.x; b = (float) y - (float) v.y; return (a*a + b*b);}
	inline float Vector2DI::DistSq (const Vector3DI &v)		{ float a,b; a = (float) x - (float) v.x; b = (float) y - (float) v.y; return (a*a + b*b);}
	inline float Vector2DI::DistSq (const Vector3DF &v)		{ float a,b; a = (float) x - (float) v.x; b = (float) y - (float) v.y; return (a*a + b*b);}
	inline float Vector2DI::DistSq (const Vector4DF &v)		{ float a,b; a = (float) x - (float) v.x; b = (float) y - (float) v.y; return (a*a + b*b);}

	inline Vector2DI &Vector2DI::Normalize (void) {
		float n = (float) x*x + (float) y*y;
		if (n!=0.0f) {
			n = sqrtf(n);
			x = (VTYPE) (((float) x*255)/n);
			y = (VTYPE) (((float) y*255)/n);				
		}
		return *this;
	}
	inline float Vector2DI::Length (void) { float n; n = (float) x*x + (float) y*y; if (n != 0.0f) return sqrtf(n); return 0.0f; }
	inline float Vector2DI::LengthSq (void) { return (float) x*x + (float) y*y; }

	inline VTYPE &Vector2DI::X(void)				{return x;}
	inline VTYPE &Vector2DI::Y(void)				{return y;}
	inline VTYPE Vector2DI::Z(void)				{return 0;}
	inline VTYPE Vector2DI::W(void)				{return 0;}
	inline const VTYPE &Vector2DI::X(void) const	{return x;}
	inline const VTYPE &Vector2DI::Y(void) const	{return y;}
	inline const VTYPE Vector2DI::Z(void) const	{return 0;}
	inline const VTYPE Vector2DI::W(void) const	{return 0;}
	inline VTYPE *Vector2DI::Data (void)			{return &x;}

	#undef VTYPE
	#undef VNAME

	// Vector2DF Code Definition

	#define VNAME		2DF
	#define VTYPE		float

	// Constructors/Destructors
	inline Vector2DF::Vector2DF() {x=0; y=0;}
	inline Vector2DF::~Vector2DF() {}
	inline Vector2DF::Vector2DF (VTYPE xa, VTYPE ya) {x=xa; y=ya;}
	inline Vector2DF::Vector2DF (const Vector2DI &op) {x=(VTYPE) op.x; y=(VTYPE) op.y;}
	inline Vector2DF::Vector2DF (const Vector2DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y;}
	inline Vector2DF::Vector2DF (const Vector3DI &op) {x=(VTYPE) op.x; y=(VTYPE) op.y;}
	inline Vector2DF::Vector2DF (const Vector3DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y;}
	inline Vector2DF::Vector2DF (const Vector4DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y;}

	// Member Functions
	inline Vector2DF &Vector2DF::operator= (const Vector2DI &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; return *this;}
	inline Vector2DF &Vector2DF::operator= (const Vector2DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; return *this;}
	inline Vector2DF &Vector2DF::operator= (const Vector3DI &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; return *this;}
	inline Vector2DF &Vector2DF::operator= (const Vector3DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; return *this;}
	inline Vector2DF &Vector2DF::operator= (const Vector4DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; return *this;}	

	inline Vector2DF &Vector2DF::operator+= (const Vector2DI &op) {x+=(VTYPE) op.x; y+=(VTYPE) op.y; return *this;}
	inline Vector2DF &Vector2DF::operator+= (const Vector2DF &op) {x+=(VTYPE) op.x; y+=(VTYPE) op.y; return *this;}
	inline Vector2DF &Vector2DF::operator+= (const Vector3DI &op) {x+=(VTYPE) op.x; y+=(VTYPE) op.y; return *this;}
	inline Vector2DF &Vector2DF::operator+= (const Vector3DF &op) {x+=(VTYPE) op.x; y+=(VTYPE) op.y; return *this;}
	inline Vector2DF &Vector2DF::operator+= (const Vector4DF &op) {x+=(VTYPE) op.x; y+=(VTYPE) op.y; return *this;}

	inline Vector2DF &Vector2DF::operator-= (const Vector2DI &op) {x-=(VTYPE) op.x; y-=(VTYPE) op.y; return *this;}
	inline Vector2DF &Vector2DF::operator-= (const Vector2DF &op) {x-=(VTYPE) op.x; y-=(VTYPE) op.y; return *this;}
	inline Vector2DF &Vector2DF::operator-= (const Vector3DI &op) {x-=(VTYPE) op.x; y-=(VTYPE) op.y; return *this;}
	inline Vector2DF &Vector2DF::operator-= (const Vector3DF &op) {x-=(VTYPE) op.x; y-=(VTYPE) op.y; return *this;}
	inline Vector2DF &Vector2DF::operator-= (const Vector4DF &op) {x-=(VTYPE) op.x; y-=(VTYPE) op.y; return *this;}
		
	inline Vector2DF &Vector2DF::operator*= (int op) {x*= (VTYPE) op; y*= (VTYPE) op; return *this;}
	inline Vector2DF &Vector2DF::operator*= (float op) {x*= (VTYPE) op; y*= (VTYPE) op; return *this;}
	inline Vector2DF &Vector2DF::operator*= (const Vector2DI &op) {x*=(VTYPE) op.x; y*=(VTYPE) op.y; return *this;}
	inline Vector2DF &Vector2DF::operator*= (const Vector2DF &op) {x*=(VTYPE) op.x; y*=(VTYPE) op.y; return *this;}
	inline Vector2DF &Vector2DF::operator*= (const Vector3DI &op) {x*=(VTYPE) op.x; y*=(VTYPE) op.y; return *this;}
	inline Vector2DF &Vector2DF::operator*= (const Vector3DF &op) {x*=(VTYPE) op.x; y*=(VTYPE) op.y; return *this;}
	inline Vector2DF &Vector2DF::operator*= (const Vector4DF &op) {x*=(VTYPE) op.x; y*=(VTYPE) op.y; return *this;}

	inline Vector2DF &Vector2DF::operator/= (int op) {x/= (VTYPE) op; y/= (VTYPE) op; return *this;}
	inline Vector2DF &Vector2DF::operator/= (float op) {x/= (VTYPE) op; y/= (VTYPE) op; return *this;}
	inline Vector2DF &Vector2DF::operator/= (const Vector2DI &op) {x/=(VTYPE) op.x; y/=(VTYPE) op.y; return *this;}
	inline Vector2DF &Vector2DF::operator/= (const Vector2DF &op) {x/=(VTYPE) op.x; y/=(VTYPE) op.y; return *this;}
	inline Vector2DF &Vector2DF::operator/= (const Vector3DI &op) {x/=(VTYPE) op.x; y/=(VTYPE) op.y; return *this;}
	inline Vector2DF &Vector2DF::operator/= (const Vector3DF &op) {x/=(VTYPE) op.x; y/=(VTYPE) op.y; return *this;}
	inline Vector2DF &Vector2DF::operator/= (const Vector4DF &op) {x/=(VTYPE) op.x; y/=(VTYPE) op.y; return *this;}

	inline float Vector2DF::Cross (const Vector2DI &v)      {return (float)y*(float)v.x + (float)(-x)*(float)v.y;}
	inline float Vector2DF::Cross (const Vector2DF &v)      {return (float)y*(float)v.x + (float)(-x)*(float)v.y;}
			
	inline float Vector2DF::Dot(const Vector2DI &v)			{return (float) x*v.x + (float) y*v.y;}
	inline float Vector2DF::Dot(const Vector2DF &v)			{return (float) x*v.x + (float) y*v.y;}

	inline float Vector2DF::Dist (const Vector2DI &v)		{ float distsq = DistSq (v); if (distsq!=0) return sqrtf(distsq); return 0.0f;}
	inline float Vector2DF::Dist (const Vector2DF &v)		{ float distsq = DistSq (v); if (distsq!=0) return sqrtf(distsq); return 0.0f;}
	inline float Vector2DF::Dist (const Vector3DI &v)		{ float distsq = DistSq (v); if (distsq!=0) return sqrtf(distsq); return 0.0f;}
	inline float Vector2DF::Dist (const Vector3DF &v)		{ float distsq = DistSq (v); if (distsq!=0) return sqrtf(distsq); return 0.0f;}
	inline float Vector2DF::Dist (const Vector4DF &v)		{ float distsq = DistSq (v); if (distsq!=0) return sqrtf(distsq); return 0.0f;}
	inline float Vector2DF::DistSq (const Vector2DI &v)		{ float a,b; a = (float) x - (float) v.x; b = (float) y - (float) v.y; return (a*a + b*b);}
	inline float Vector2DF::DistSq (const Vector2DF &v)		{ float a,b; a = (float) x - (float) v.x; b = (float) y - (float) v.y; return (a*a + b*b);}
	inline float Vector2DF::DistSq (const Vector3DI &v)		{ float a,b; a = (float) x - (float) v.x; b = (float) y - (float) v.y; return (a*a + b*b);}
	inline float Vector2DF::DistSq (const Vector3DF &v)		{ float a,b; a = (float) x - (float) v.x; b = (float) y - (float) v.y; return (a*a + b*b);}
	inline float Vector2DF::DistSq (const Vector4DF &v)		{ float a,b; a = (float) x - (float) v.x; b = (float) y - (float) v.y; return (a*a + b*b);}

	inline Vector2DF &Vector2DF::Normalize (void) {
		float n = (float) x*x + (float) y*y;
		if (n!=0.0f) {
			n = sqrtf(n);
			x /= n;
			y /= n;
		}
		return *this;
	}
	inline float Vector2DF::Length (void) { float n; n = (float) x*x + (float) y*y; if (n != 0.0f) return sqrtf(n); return 0.0f; }
	inline float Vector2DF::LengthSq (void) { return (float) x*x + (float) y*y; }

	inline VTYPE &Vector2DF::X(void)				{return x;}
	inline VTYPE &Vector2DF::Y(void)				{return y;}
	inline VTYPE Vector2DF::Z(void)					{return 0;}
	inline VTYPE Vector2DF::W(void)					{return 0;}
	inline const VTYPE &Vector2DF::X(void) const	{return x;}
	inline const VTYPE &Vector2DF::Y(void) const	{return y;}
	inline const VTYPE Vector2DF::Z(void) const		{return 0;}
	inline const VTYPE Vector2DF::W(void) const		{return 0;}
	inline VTYPE *Vector2DF::Data (void)			{return &x;}

	#undef VTYPE
	#undef VNAME

	// Vector3DI Code Definition

	#define VNAME		3DI
	#define VTYPE		int

	// Constructors/Destructors
	inline Vector3DI::Vector3DI() {x=0; y=0; z=0;}
	inline Vector3DI::~Vector3DI() {}
	inline Vector3DI::Vector3DI (VTYPE xa, VTYPE ya, VTYPE za) {x=xa; y=ya; z=za;}
	inline Vector3DI::Vector3DI (const Vector2DI &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; z=(VTYPE) 0;}
	inline Vector3DI::Vector3DI (const Vector2DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; z=(VTYPE) 0;}
	inline Vector3DI::Vector3DI (const Vector3DI &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; z=(VTYPE) op.z;}
	inline Vector3DI::Vector3DI (const Vector3DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; z=(VTYPE) op.z;}
	inline Vector3DI::Vector3DI (const Vector4DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; z=(VTYPE) op.z;}

	// Member Functions
	inline Vector3DI &Vector3DI::operator= (const Vector2DI &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; return *this;}
	inline Vector3DI &Vector3DI::operator= (const Vector2DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; return *this;}
	inline Vector3DI &Vector3DI::operator= (const Vector3DI &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; z=(VTYPE) op.z; return *this;}
	inline Vector3DI &Vector3DI::operator= (const Vector3DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; z=(VTYPE) op.z; return *this;}
	inline Vector3DI &Vector3DI::operator= (const Vector4DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; z=(VTYPE) op.z; return *this;}	
		
	inline Vector3DI &Vector3DI::operator+= (const Vector2DI &op) {x+=(VTYPE) op.x; y+=(VTYPE) op.y; return *this;}
	inline Vector3DI &Vector3DI::operator+= (const Vector2DF &op) {x+=(VTYPE) op.x; y+=(VTYPE) op.y; return *this;}
	inline Vector3DI &Vector3DI::operator+= (const Vector3DI &op) {x+=(VTYPE) op.x; y+=(VTYPE) op.y; z+=(VTYPE) op.z; return *this;}
	inline Vector3DI &Vector3DI::operator+= (const Vector3DF &op) {x+=(VTYPE) op.x; y+=(VTYPE) op.y; z+=(VTYPE) op.z; return *this;}
	inline Vector3DI &Vector3DI::operator+= (const Vector4DF &op) {x+=(VTYPE) op.x; y+=(VTYPE) op.y; z+=(VTYPE) op.z; return *this;}

	inline Vector3DI &Vector3DI::operator-= (const Vector2DI &op) {x-=(VTYPE) op.x; y-=(VTYPE) op.y; return *this;}
	inline Vector3DI &Vector3DI::operator-= (const Vector2DF &op) {x-=(VTYPE) op.x; y-=(VTYPE) op.y; return *this;}
	inline Vector3DI &Vector3DI::operator-= (const Vector3DI &op) {x-=(VTYPE) op.x; y-=(VTYPE) op.y; z-=(VTYPE) op.z; return *this;}
	inline Vector3DI &Vector3DI::operator-= (const Vector3DF &op) {x-=(VTYPE) op.x; y-=(VTYPE) op.y; z-=(VTYPE) op.z; return *this;}
	inline Vector3DI &Vector3DI::operator-= (const Vector4DF &op) {x-=(VTYPE) op.x; y-=(VTYPE) op.y; z-=(VTYPE) op.z; return *this;}
		
	inline Vector3DI &Vector3DI::operator*= (const Vector2DI &op) {x*=(VTYPE) op.x; y*=(VTYPE) op.y; return *this;}
	inline Vector3DI &Vector3DI::operator*= (const Vector2DF &op) {x*=(VTYPE) op.x; y*=(VTYPE) op.y; return *this;}
	inline Vector3DI &Vector3DI::operator*= (const Vector3DI &op) {x*=(VTYPE) op.x; y*=(VTYPE) op.y; z*=(VTYPE) op.z; return *this;}
	inline Vector3DI &Vector3DI::operator*= (const Vector3DF &op) {x*=(VTYPE) op.x; y*=(VTYPE) op.y; z*=(VTYPE) op.z; return *this;}
	inline Vector3DI &Vector3DI::operator*= (const Vector4DF &op) {x*=(VTYPE) op.x; y*=(VTYPE) op.y; z*=(VTYPE) op.z; return *this;}

	inline Vector3DI &Vector3DI::operator/= (const Vector2DI &op) {x/=(VTYPE) op.x; y/=(VTYPE) op.y; return *this;}
	inline Vector3DI &Vector3DI::operator/= (const Vector2DF &op) {x/=(VTYPE) op.x; y/=(VTYPE) op.y; return *this;}
	inline Vector3DI &Vector3DI::operator/= (const Vector3DI &op) {x/=(VTYPE) op.x; y/=(VTYPE) op.y; z/=(VTYPE) op.z; return *this;}
	inline Vector3DI &Vector3DI::operator/= (const Vector3DF &op) {x/=(VTYPE) op.x; y/=(VTYPE) op.y; z/=(VTYPE) op.z; return *this;}
	inline Vector3DI &Vector3DI::operator/= (const Vector4DF &op) {x/=(VTYPE) op.x; y/=(VTYPE) op.y; z/=(VTYPE) op.z; return *this;}

	inline Vector3DI &Vector3DI::Cross (const Vector3DI &v) {float ax = (float)x, ay = (float)y, az = (float)z; x = (VTYPE) (ay * (float) v.z - az * (float) v.y); y = (VTYPE) (-ax * (float) v.z + az * (float) v.x); z = (VTYPE) (ax * (float) v.y - ay * (float) v.x); return *this;}
	inline Vector3DI &Vector3DI::Cross (const Vector3DF &v) {float ax = (float)x, ay = (float)y, az = (float)z; x = (VTYPE) (ay * (float) v.z - az * (float) v.y); y = (VTYPE) (-ax * (float) v.z + az * (float) v.x); z = (VTYPE) (ax * (float) v.y - ay * (float) v.x); return *this;}
			
	inline float Vector3DI::Dot(const Vector3DI &v)			{return (float) (x*v.x) + (float) (y*v.y) + (float) (z*v.z);}
	inline float Vector3DI::Dot(const Vector3DF &v)			{return (float) x*v.x + (float) y*v.y + (float) z*v.z;}

	inline float Vector3DI::Dist (const Vector2DI &v)		{ float distsq = DistSq (v); if (distsq!=0) return sqrtf(distsq); return 0.0f;}
	inline float Vector3DI::Dist (const Vector2DF &v)		{ float distsq = DistSq (v); if (distsq!=0) return sqrtf(distsq); return 0.0f;}
	inline float Vector3DI::Dist (const Vector3DI &v)		{ float distsq = DistSq (v); if (distsq!=0) return sqrtf(distsq); return 0.0f;}
	inline float Vector3DI::Dist (const Vector3DF &v)		{ float distsq = DistSq (v); if (distsq!=0) return sqrtf(distsq); return 0.0f;}
	inline float Vector3DI::Dist (const Vector4DF &v)		{ float distsq = DistSq (v); if (distsq!=0) return sqrtf(distsq); return 0.0f;}
	inline float Vector3DI::DistSq (const Vector2DI &v)		{ float a,b,c; a = (float) x - (float) v.x; b = (float) y - (float) v.y; c = (float) z; return (a*a + b*b + c*c);}
	inline float Vector3DI::DistSq (const Vector2DF &v)		{ float a,b,c; a = (float) x - (float) v.x; b = (float) y - (float) v.y; c = (float) z; return (a*a + b*b + c*c);}
	inline float Vector3DI::DistSq (const Vector3DI &v)		{ float a,b,c; a = (float) x - (float) v.x; b = (float) y - (float) v.y; c = (float) z - (float) v.z; return (a*a + b*b + c*c);}
	inline float Vector3DI::DistSq (const Vector3DF &v)		{ float a,b,c; a = (float) x - (float) v.x; b = (float) y - (float) v.y; c = (float) z - (float) v.z; return (a*a + b*b + c*c);}
	inline float Vector3DI::DistSq (const Vector4DF &v)		{ float a,b,c; a = (float) x - (float) v.x; b = (float) y - (float) v.y; c = (float) z - (float) v.z; return (a*a + b*b + c*c);}

	inline Vector3DI &Vector3DI::Normalize (void) {
		float n = (float) x*x + (float) y*y + (float) z*z;
		if (n!=0.0f) {
			n = sqrtf(n);
			x = (VTYPE) (((float) x*255)/n);
			y = (VTYPE) (((float) y*255)/n);
			z = (VTYPE) (((float) z*255)/n);
		}
		return *this;
	}
	inline float Vector3DI::Length (void) { float n; n = (float) x*x + (float) y*y + (float) z*z; if (n != 0.0f) return sqrtf(n); return 0.0f; }
	inline float Vector3DI::LengthSq (void) { return (float) x*x + (float) y*y + (float) z*z; }

	inline VTYPE &Vector3DI::X(void)				{return x;}
	inline VTYPE &Vector3DI::Y(void)				{return y;}
	inline VTYPE &Vector3DI::Z(void)				{return z;}
	inline VTYPE Vector3DI::W(void)					{return 0;}
	inline const VTYPE &Vector3DI::X(void) const	{return x;}
	inline const VTYPE &Vector3DI::Y(void) const	{return y;}
	inline const VTYPE &Vector3DI::Z(void) const	{return z;}
	inline const VTYPE Vector3DI::W(void) const		{return 0;}
	inline VTYPE *Vector3DI::Data (void)			{return &x;}

	#undef VTYPE
	#undef VNAME

	// Vector3DF Code Definition

	#define VNAME		3DF
	#define VTYPE		float

	// Constructors/Destructors
	inline Vector3DF::Vector3DF() {x=0; y=0; z=0;}
	inline Vector3DF::~Vector3DF() {}
	inline Vector3DF::Vector3DF (VTYPE xa, VTYPE ya, VTYPE za) {x=xa; y=ya; z=za;}
	inline Vector3DF::Vector3DF (const Vector2DI &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; z=(VTYPE) 0;}
	inline Vector3DF::Vector3DF (const Vector2DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; z=(VTYPE) 0;}
	inline Vector3DF::Vector3DF (const Vector3DI &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; z=(VTYPE) op.z;}
	inline Vector3DF::Vector3DF (const Vector3DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; z=(VTYPE) op.z;}
	inline Vector3DF::Vector3DF (const Vector4DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; z=(VTYPE) op.z;}

	// Set Functions
	inline Vector3DF &Vector3DF::Set (float xa, float ya, float za)
	{
		x = xa; y = ya; z = za;
		return *this;
	}

	// Member Functions
	inline Vector3DF &Vector3DF::operator= (const Vector2DI &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; return *this;}
	inline Vector3DF &Vector3DF::operator= (const Vector2DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; return *this;}
	inline Vector3DF &Vector3DF::operator= (const Vector3DI &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; z=(VTYPE) op.z; return *this;}
	inline Vector3DF &Vector3DF::operator= (const Vector3DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; z=(VTYPE) op.z; return *this;}
	inline Vector3DF &Vector3DF::operator= (const Vector4DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; z=(VTYPE) op.z; return *this;}	
		
	inline Vector3DF &Vector3DF::operator+= (const Vector2DI &op) {x+=(VTYPE) op.x; y+=(VTYPE) op.y; return *this;}
	inline Vector3DF &Vector3DF::operator+= (const Vector2DF &op) {x+=(VTYPE) op.x; y+=(VTYPE) op.y; return *this;}
	inline Vector3DF &Vector3DF::operator+= (const Vector3DI &op) {x+=(VTYPE) op.x; y+=(VTYPE) op.y; z+=(VTYPE) op.z; return *this;}
	inline Vector3DF &Vector3DF::operator+= (const Vector3DF &op) {x+=(VTYPE) op.x; y+=(VTYPE) op.y; z+=(VTYPE) op.z; return *this;}
	inline Vector3DF &Vector3DF::operator+= (const Vector4DF &op) {x+=(VTYPE) op.x; y+=(VTYPE) op.y; z+=(VTYPE) op.z; return *this;}

	inline Vector3DF &Vector3DF::operator-= (const Vector2DI &op) {x-=(VTYPE) op.x; y-=(VTYPE) op.y; return *this;}
	inline Vector3DF &Vector3DF::operator-= (const Vector2DF &op) {x-=(VTYPE) op.x; y-=(VTYPE) op.y; return *this;}
	inline Vector3DF &Vector3DF::operator-= (const Vector3DI &op) {x-=(VTYPE) op.x; y-=(VTYPE) op.y; z-=(VTYPE) op.z; return *this;}
	inline Vector3DF &Vector3DF::operator-= (const Vector3DF &op) {x-=(VTYPE) op.x; y-=(VTYPE) op.y; z-=(VTYPE) op.z; return *this;}
	inline Vector3DF &Vector3DF::operator-= (const Vector4DF &op) {x-=(VTYPE) op.x; y-=(VTYPE) op.y; z-=(VTYPE) op.z; return *this;}
		
	inline Vector3DF &Vector3DF::operator*= (int op) {x*= (VTYPE) op; y*= (VTYPE) op; z*= (VTYPE) op; return *this;}
	inline Vector3DF &Vector3DF::operator*= (float op) {x*= (VTYPE) op; y*= (VTYPE) op; z*= (VTYPE) op; return *this;}
	inline Vector3DF &Vector3DF::operator*= (const Vector2DI &op) {x*=(VTYPE) op.x; y*=(VTYPE) op.y; return *this;}
	inline Vector3DF &Vector3DF::operator*= (const Vector2DF &op) {x*=(VTYPE) op.x; y*=(VTYPE) op.y; return *this;}
	inline Vector3DF &Vector3DF::operator*= (const Vector3DI &op) {x*=(VTYPE) op.x; y*=(VTYPE) op.y; z*=(VTYPE) op.z; return *this;}
	inline Vector3DF &Vector3DF::operator*= (const Vector3DF &op) {x*=(VTYPE) op.x; y*=(VTYPE) op.y; z*=(VTYPE) op.z; return *this;}
	inline Vector3DF &Vector3DF::operator*= (const Vector4DF &op) {x*=(VTYPE) op.x; y*=(VTYPE) op.y; z*=(VTYPE) op.z; return *this;}

	inline Vector3DF &Vector3DF::operator/= (int op) {x/= (VTYPE) op; y/= (VTYPE) op; z/= (VTYPE) op; return *this;}
	inline Vector3DF &Vector3DF::operator/= (float op) {x/= (VTYPE) op; y/= (VTYPE) op; z/= (VTYPE) op; return *this;}
	inline Vector3DF &Vector3DF::operator/= (const Vector2DI &op) {x/=(VTYPE) op.x; y/=(VTYPE) op.y; return *this;}
	inline Vector3DF &Vector3DF::operator/= (const Vector2DF &op) {x/=(VTYPE) op.x; y/=(VTYPE) op.y; return *this;}
	inline Vector3DF &Vector3DF::operator/= (const Vector3DI &op) {x/=(VTYPE) op.x; y/=(VTYPE) op.y; z/=(VTYPE) op.z; return *this;}
	inline Vector3DF &Vector3DF::operator/= (const Vector3DF &op) {x/=(VTYPE) op.x; y/=(VTYPE) op.y; z/=(VTYPE) op.z; return *this;}
	inline Vector3DF &Vector3DF::operator/= (const Vector4DF &op) {x/=(VTYPE) op.x; y/=(VTYPE) op.y; z/=(VTYPE) op.z; return *this;}

	inline Vector3DF &Vector3DF::Cross (const Vector3DI &v) {float ax = x, ay = y, az = z; x = (VTYPE) (ay * (float) v.z - az * (float) v.y); y = (VTYPE) (-ax * (float) v.z + az * (float) v.x); z = (VTYPE) (ax * (float) v.y - ay * (float) v.x); return *this;}
	inline Vector3DF &Vector3DF::Cross (const Vector3DF &v) {float ax = x, ay = y, az = z; x = (VTYPE) (ay * (float) v.z - az * (float) v.y); y = (VTYPE) (-ax * (float) v.z + az * (float) v.x); z = (VTYPE) (ax * (float) v.y - ay * (float) v.x); return *this;}
			
	inline float Vector3DF::Dot(const Vector3DI &v)			{return (float) v.x*x + (float) v.y*y + (float) v.z*z;}
	inline float Vector3DF::Dot(const Vector3DF &v)			{return x*v.x + y*v.y + z*v.z;}

	inline float Vector3DF::Dist (const Vector2DI &v)		{ float distsq = DistSq (v); if (distsq!=0) return sqrtf(distsq); return 0.0f;}
	inline float Vector3DF::Dist (const Vector2DF &v)		{ float distsq = DistSq (v); if (distsq!=0) return sqrtf(distsq); return 0.0f;}
	inline float Vector3DF::Dist (const Vector3DI &v)		{ float distsq = DistSq (v); if (distsq!=0) return sqrtf(distsq); return 0.0f;}
	inline float Vector3DF::Dist (const Vector3DF &v)		{ float distsq = DistSq (v); if (distsq!=0) return sqrtf(distsq); return 0.0f;}
	inline float Vector3DF::Dist (const Vector4DF &v)		{ float distsq = DistSq (v); if (distsq!=0) return sqrtf(distsq); return 0.0f;}
	inline float Vector3DF::DistSq (const Vector2DI &v)		{ float a,b,c; a = (float) x - (float) v.x; b = (float) y - (float) v.y; c = (float) z; return (a*a + b*b + c*c);}
	inline float Vector3DF::DistSq (const Vector2DF &v)		{ float a,b,c; a = (float) x - (float) v.x; b = (float) y - (float) v.y; c = (float) z; return (a*a + b*b + c*c);}
	inline float Vector3DF::DistSq (const Vector3DI &v)		{ float a,b,c; a = (float) x - (float) v.x; b = (float) y - (float) v.y; c = (float) z - (float) v.z; return (a*a + b*b + c*c);}
	inline float Vector3DF::DistSq (const Vector3DF &v)		{ float a,b,c; a = (float) x - (float) v.x; b = (float) y - (float) v.y; c = (float) z - (float) v.z; return (a*a + b*b + c*c);}
	inline float Vector3DF::DistSq (const Vector4DF &v)		{ float a,b,c; a = (float) x - (float) v.x; b = (float) y - (float) v.y; c = (float) z - (float) v.z; return (a*a + b*b + c*c);}

	inline Vector3DF &Vector3DF::Normalize (void) {if(x||y||z) {float invlen = 1.0f/sqrtf(x*x + y*y + z*z); x*=invlen; y*=invlen; z*=invlen;} return *this;}
	inline float Vector3DF::Length (void) { float n; n = (float) x*x + (float) y*y + (float) z*z; if (n != 0.0f) return sqrtf(n); return 0.0f; }
	inline float Vector3DF::LengthSq (void) { return (float) x*x + (float) y*y + (float) z*z; }

	inline VTYPE &Vector3DF::X(void)				{return x;}
	inline VTYPE &Vector3DF::Y(void)				{return y;}
	inline VTYPE &Vector3DF::Z(void)				{return z;}
	inline VTYPE Vector3DF::W(void)					{return 0;}
	inline const VTYPE &Vector3DF::X(void) const	{return x;}
	inline const VTYPE &Vector3DF::Y(void) const	{return y;}
	inline const VTYPE &Vector3DF::Z(void) const	{return z;}
	inline const VTYPE Vector3DF::W(void) const		{return 0;}
	inline VTYPE *Vector3DF::Data (void)			{return &x;}

	#undef VTYPE
	#undef VNAME

	// Vector4DF Code Definition

	#define VNAME		4DF
	#define VTYPE		float

	// Constructors/Destructors
	inline Vector4DF::Vector4DF() {x=0; y=0; z=0; w=0;}
	inline Vector4DF::~Vector4DF() {}
	inline Vector4DF::Vector4DF (VTYPE xa, VTYPE ya, VTYPE za, VTYPE wa) {x=xa; y=ya; z=za; w=wa;}
	inline Vector4DF::Vector4DF (const Vector2DI &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; z=(VTYPE) 0; w=(VTYPE) 0;}
	inline Vector4DF::Vector4DF (const Vector2DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; z=(VTYPE) 0; w=(VTYPE) 0;}
	inline Vector4DF::Vector4DF (const Vector3DI &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; z=(VTYPE) op.z; w=(VTYPE) 0;}
	inline Vector4DF::Vector4DF (const Vector3DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; z=(VTYPE) op.z; w=(VTYPE) 0;}
	inline Vector4DF::Vector4DF (const Vector4DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; z=(VTYPE) op.z; w=(VTYPE) op.w;}

	// Member Functions
	inline Vector4DF &Vector4DF::operator= (const Vector2DI &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; z=(VTYPE) 0; w=(VTYPE) 0;  return *this;}
	inline Vector4DF &Vector4DF::operator= (const Vector2DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; z=(VTYPE) 0; w=(VTYPE) 0;  return *this;}
	inline Vector4DF &Vector4DF::operator= (const Vector3DI &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; z=(VTYPE) op.z; w=(VTYPE) 0;  return *this;}
	inline Vector4DF &Vector4DF::operator= (const Vector3DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; z=(VTYPE) op.z; w=(VTYPE) 0; return *this;}
	inline Vector4DF &Vector4DF::operator= (const Vector4DF &op) {x=(VTYPE) op.x; y=(VTYPE) op.y; z=(VTYPE) op.z; w=(VTYPE) op.w; return *this;}	
		
	inline Vector4DF &Vector4DF::operator+= (const Vector2DI &op) {x+=(VTYPE) op.x; y+=(VTYPE) op.y; return *this;}
	inline Vector4DF &Vector4DF::operator+= (const Vector2DF &op) {x+=(VTYPE) op.x; y+=(VTYPE) op.y; return *this;}
	inline Vector4DF &Vector4DF::operator+= (const Vector3DI &op) {x+=(VTYPE) op.x; y+=(VTYPE) op.y; z+=(VTYPE) op.z; return *this;}
	inline Vector4DF &Vector4DF::operator+= (const Vector3DF &op) {x+=(VTYPE) op.x; y+=(VTYPE) op.y; z+=(VTYPE) op.z; return *this;}
	inline Vector4DF &Vector4DF::operator+= (const Vector4DF &op) {x+=(VTYPE) op.x; y+=(VTYPE) op.y; z+=(VTYPE) op.z; w+=(VTYPE) op.w; return *this;}	

	inline Vector4DF &Vector4DF::operator-= (const Vector2DI &op) {x-=(VTYPE) op.x; y-=(VTYPE) op.y; return *this;}
	inline Vector4DF &Vector4DF::operator-= (const Vector2DF &op) {x-=(VTYPE) op.x; y-=(VTYPE) op.y; return *this;}
	inline Vector4DF &Vector4DF::operator-= (const Vector3DI &op) {x-=(VTYPE) op.x; y-=(VTYPE) op.y; z-=(VTYPE) op.z; return *this;}
	inline Vector4DF &Vector4DF::operator-= (const Vector3DF &op) {x-=(VTYPE) op.x; y-=(VTYPE) op.y; z-=(VTYPE) op.z; return *this;}
	inline Vector4DF &Vector4DF::operator-= (const Vector4DF &op) {x-=(VTYPE) op.x; y-=(VTYPE) op.y; z-=(VTYPE) op.z; w-=(VTYPE) op.w; return *this;}	

	inline Vector4DF &Vector4DF::operator*= (int op) {x*= (VTYPE) op; y*= (VTYPE) op; z*= (VTYPE) op; w *= (VTYPE) op; return *this;}
	inline Vector4DF &Vector4DF::operator*= (float op) {x*= (VTYPE) op; y*= (VTYPE) op; z*= (VTYPE) op; w *= (VTYPE) op; return *this;}
	inline Vector4DF &Vector4DF::operator*= (const Vector2DI &op) {x*=(VTYPE) op.x; y*=(VTYPE) op.y; return *this;}
	inline Vector4DF &Vector4DF::operator*= (const Vector2DF &op) {x*=(VTYPE) op.x; y*=(VTYPE) op.y; return *this;}
	inline Vector4DF &Vector4DF::operator*= (const Vector3DI &op) {x*=(VTYPE) op.x; y*=(VTYPE) op.y; z*=(VTYPE) op.z; return *this;}
	inline Vector4DF &Vector4DF::operator*= (const Vector3DF &op) {x*=(VTYPE) op.x; y*=(VTYPE) op.y; z*=(VTYPE) op.z; return *this;}
	inline Vector4DF &Vector4DF::operator*= (const Vector4DF &op) {x*=(VTYPE) op.x; y*=(VTYPE) op.y; z*=(VTYPE) op.z; w*=(VTYPE) op.w; return *this;}	

	inline Vector4DF &Vector4DF::operator/= (int op) {x/= (VTYPE) op; y/= (VTYPE) op; z/= (VTYPE) op; w /= (VTYPE) op; return *this;}
	inline Vector4DF &Vector4DF::operator/= (float op) {x/= (VTYPE) op; y/= (VTYPE) op; z/= (VTYPE) op; w /= (VTYPE) op; return *this;}
	inline Vector4DF &Vector4DF::operator/= (const Vector2DI &op) {x/=(VTYPE) op.x; y/=(VTYPE) op.y; return *this;}
	inline Vector4DF &Vector4DF::operator/= (const Vector2DF &op) {x/=(VTYPE) op.x; y/=(VTYPE) op.y; return *this;}
	inline Vector4DF &Vector4DF::operator/= (const Vector3DI &op) {x/=(VTYPE) op.x; y/=(VTYPE) op.y; z/=(VTYPE) op.z; return *this;}
	inline Vector4DF &Vector4DF::operator/= (const Vector3DF &op) {x/=(VTYPE) op.x; y/=(VTYPE) op.y; z/=(VTYPE) op.z; return *this;}
	inline Vector4DF &Vector4DF::operator/= (const Vector4DF &op) {x/=(VTYPE) op.x; y/=(VTYPE) op.y; z/=(VTYPE) op.z; w/=(VTYPE) op.w; return *this;}	

	inline float Vector4DF::Dot (const Vector4DF &v)			{return (float) x*v.x + (float) y*v.y + (float) z*v.z + (float) w*v.w;}

	inline float Vector4DF::Dist (const Vector4DF &v)		{float distsq = DistSq (v); if (distsq!=0) return sqrtf(distsq); return 0.0f;}

	inline float Vector4DF::DistSq (const Vector4DF &v)		{float a,b,c,d; a = (float) x - (float) v.x; b = (float) y - (float) v.y; c = (float) z - (float) v.z; d = (float) w - (float) v.w; return (a*a + b*b + c*c + d*d);}

	inline Vector4DF &Vector4DF::Normalize (void) {if(x||y||z||w) {float invlen = 1.0f/sqrtf(x*x + y*y + z*z + w*w); x*=invlen; y*=invlen; z*=invlen; w*=invlen;} return *this;}
	inline float Vector4DF::Length (void) { float n; n = (float) x*x + (float) y*y + (float) z*z + (float) w*w; if (n != 0.0f) return sqrtf(n); return 0.0f; }
	inline float Vector4DF::LengthSq (void) { return (float) x*x + (float) y*y + (float) z*z + (float) w*w; }

	inline VTYPE &Vector4DF::X(void)				{return x;}
	inline VTYPE &Vector4DF::Y(void)				{return y;}
	inline VTYPE &Vector4DF::Z(void)				{return z;}
	inline VTYPE &Vector4DF::W(void)				{return w;}
	inline const VTYPE &Vector4DF::X(void) const	{return x;}
	inline const VTYPE &Vector4DF::Y(void) const	{return y;}
	inline const VTYPE &Vector4DF::Z(void) const	{return z;}
	inline const VTYPE &Vector4DF::W(void) const	{return w;}
	inline VTYPE *Vector4DF::Data (void)			{return &x;}

	#undef VTYPE
	#undef VNAME

#endif