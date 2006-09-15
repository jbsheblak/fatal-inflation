//
// GameX (Release 2 - Vectors / Matricies / CameraX)
// Matrix Class Header
// 
// Copyright (C) 2002 Rama C. Hoetzlein
// 
// This software is released under the GameX GNU GPL 
// Open Source Liscense. See the GameX documentation included
// with this source code for terms of modification, 
// distribution and re-release. 
//

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <memory.h>
#include <math.h>

#ifndef MATRIX_DEF
	#define MATRIX_DEF
											// **** Switches	
	#define MATRIX_DEBUG					// Enable Debugging Output
	#define MATRIX_INITIALIZE				// Initializes vectors	

	#include "gamex-debug.hpp"
	#include "gamex-vector.hpp"
	#include "gamex-defines.hpp"

	#define MSG_GETROWS		27975
	#define MSG_GETCOLS		55781
	#define MSG_MULT		58154
	#define MSG_SIZE		27831
	#define MSG_SIZESAFE	65442
	#define MSG_INSERTROW	46865
	#define MSG_INSERTCOL	9135
	#define MSG_TRANSPOSE	51755
	#define MSG_IDENTITY	13042
	#define MSG_ROTATEX		5491
	#define MSG_ROTATEY		47272
	#define MSG_ROTATEZ		23517
	#define MSG_BASIS		58298
	#define MSG_DIV			60383
											// Forward Referencing
	class MatrixI;
	class MatrixF;

	class Matrix {
	public:
		// Member Virtual Functions		
		virtual Matrix &operator= (unsigned char c)=0;
		virtual Matrix &operator= (int c)=0;
		virtual Matrix &operator= (float c)=0;		
		virtual Matrix &operator= (MatrixI &op)=0;
		virtual Matrix &operator= (MatrixF &op)=0;
		
		virtual Matrix &operator+= (unsigned char c)=0;
		virtual Matrix &operator+= (int c)=0;
		virtual Matrix &operator+= (float c)=0;		
		virtual Matrix &operator+= (MatrixI &op)=0;
		virtual Matrix &operator+= (MatrixF &op)=0;

		virtual Matrix &operator-= (unsigned char c)=0;
		virtual Matrix &operator-= (int c)=0;
		virtual Matrix &operator-= (float c)=0;		
		virtual Matrix &operator-= (MatrixI &op)=0;
		virtual Matrix &operator-= (MatrixF &op)=0;

		virtual Matrix &operator*= (unsigned char c)=0;
		virtual Matrix &operator*= (int c)=0;
		virtual Matrix &operator*= (float c)=0;		
		virtual Matrix &operator*= (MatrixI &op)=0;
		virtual Matrix &operator*= (MatrixF &op)=0;

		virtual Matrix &operator/= (unsigned char c)=0;
		virtual Matrix &operator/= (int c)=0;
		virtual Matrix &operator/= (float c)=0;		
		virtual Matrix &operator/= (MatrixI &op)=0;
		virtual Matrix &operator/= (MatrixF &op)=0;

		virtual Matrix &Multiply (MatrixF &op)=0;
		virtual Matrix &Size (int r, int c)=0;
		virtual Matrix &SizeSafe (int r, int c)=0;
		virtual Matrix &InsertRow (int r)=0;
		virtual Matrix &InsertCol (int c)=0;
		virtual Matrix &Transpose (void)=0;
		virtual Matrix &Identity (int order)=0;
		/*inline Matrix &RotateX (float ang);
		inline Matrix &RotateY (float ang);
		inline Matrix &RotateZ (float ang); */
		virtual Matrix &Basis (Vector3DF &c1, Vector3DF &c2, Vector3DF &c3)=0;
		virtual Matrix &GaussJordan (MatrixF &b)=0;

		virtual int GetRows(void)=0;
		virtual int GetCols(void)=0;
		virtual int GetLength(void)=0;		

		virtual unsigned char *GetDataC (void)=0;
		virtual int	*GetDataI (void)=0;
		virtual float *GetDataF (void)=0;
	};
	
	// MatrixI Declaration	
	#define VNAME		I
	#define VTYPE		int

	class MatrixI {
	public:
		VTYPE *data;
		int rows, cols, len;		
	
		// Constructors/Destructors
		inline MatrixI ();
		inline ~MatrixI ();
		inline MatrixI (int r, int c);

		// Member Functions
		inline VTYPE &operator () (int r, int c);
		inline MatrixI &operator= (unsigned char c);
		inline MatrixI &operator= (int c);
		inline MatrixI &operator= (float c);		
		inline MatrixI &operator= (MatrixI &op);
		inline MatrixI &operator= (MatrixF &op);
		
		inline MatrixI &operator+= (unsigned char c);
		inline MatrixI &operator+= (int c);
		inline MatrixI &operator+= (float c);		
		inline MatrixI &operator+= (MatrixI &op);
		inline MatrixI &operator+= (MatrixF &op);

		inline MatrixI &operator-= (unsigned char c);
		inline MatrixI &operator-= (int c);
		inline MatrixI &operator-= (float c);		
		inline MatrixI &operator-= (MatrixI &op);
		inline MatrixI &operator-= (MatrixF &op);

		inline MatrixI &operator*= (unsigned char c);
		inline MatrixI &operator*= (int c);
		inline MatrixI &operator*= (float c);		
		inline MatrixI &operator*= (MatrixI &op);
		inline MatrixI &operator*= (MatrixF &op);

		inline MatrixI &operator/= (unsigned char c);
		inline MatrixI &operator/= (int c);
		inline MatrixI &operator/= (float c);		
		inline MatrixI &operator/= (MatrixI &op);
		inline MatrixI &operator/= (MatrixF &op);

		inline MatrixI &Multiply (MatrixF &op);
		inline MatrixI &Size (int r, int c);
		inline MatrixI &SizeSafe (int r, int c);
		inline MatrixI &InsertRow (int r);
		inline MatrixI &InsertCol (int c);
		inline MatrixI &Transpose (void);
		inline MatrixI &Identity (int order);		
		inline MatrixI &Basis (Vector3DF &c1, Vector3DF &c2, Vector3DF &c3);
		inline MatrixI &GaussJordan (MatrixF &b);

		inline int GetRows(void);
		inline int GetCols(void);
		inline int GetLength(void);
		inline VTYPE *GetData(void);

		inline unsigned char *GetDataC (void)	{return NULL;}
		inline int *GetDataI (void)				{return data;}
		inline float *GetDataF (void)			{return NULL;}
	};
	#undef VNAME
	#undef VTYPE

	// MatrixF Declaration	
	#define VNAME		F
	#define VTYPE		float

	class MatrixF {
	public:	
		VTYPE *data;
		int rows, cols, len;		

		// Constructors/Destructors		
		inline MatrixF ();
		inline ~MatrixF ();
		inline MatrixF (int r, int c);

		// Member Functions
		inline VTYPE &operator () (int r, int c);
		inline MatrixF &operator= (unsigned char c);
		inline MatrixF &operator= (int c);
		inline MatrixF &operator= (float c);		
		inline MatrixF &operator= (MatrixI &op);
		inline MatrixF &operator= (MatrixF &op);
		
		inline MatrixF &operator+= (unsigned char c);
		inline MatrixF &operator+= (int c);
		inline MatrixF &operator+= (float c);		
		inline MatrixF &operator+= (MatrixI &op);
		inline MatrixF &operator+= (MatrixF &op);

		inline MatrixF &operator-= (unsigned char c);
		inline MatrixF &operator-= (int c);
		inline MatrixF &operator-= (float c);		
		inline MatrixF &operator-= (MatrixI &op);
		inline MatrixF &operator-= (MatrixF &op);

		inline MatrixF &operator*= (unsigned char c);
		inline MatrixF &operator*= (int c);
		inline MatrixF &operator*= (float c);		
		inline MatrixF &operator*= (MatrixI &op);
		inline MatrixF &operator*= (MatrixF &op);		

		inline MatrixF &operator/= (unsigned char c);
		inline MatrixF &operator/= (int c);
		inline MatrixF &operator/= (float c);		
		inline MatrixF &operator/= (MatrixI &op);
		inline MatrixF &operator/= (MatrixF &op);

		inline MatrixF &Multiply4x4 (MatrixF &op);
		inline MatrixF &Multiply (MatrixF &op);
		inline MatrixF &Size (int r, int c);
		inline MatrixF &SizeSafe (int r, int c);
		inline MatrixF &InsertRow (int r);
		inline MatrixF &InsertCol (int c);
		inline MatrixF &Transpose (void);
		inline MatrixF &Identity (int order);
		inline MatrixF &RotateX (float ang);
		inline MatrixF &RotateY (float ang);
		inline MatrixF &RotateZ (float ang);
		inline MatrixF &RotateX (float sinAng, float cosAng);
		inline MatrixF &RotateY (float sinAng, float cosAng);
		inline MatrixF &RotateZ (float sinAng, float cosAng);
		inline MatrixF &Basis (Vector3DF &c1, Vector3DF &c2, Vector3DF &c3);
		inline MatrixF &GaussJordan (MatrixF &b);

		inline int GetRows(void);
		inline int GetCols(void);
		inline int GetLength(void);
		inline VTYPE *GetData(void);
		inline void GetRowVec (int r, Vector3DF &v);

		inline unsigned char *GetDataC (void)	{return NULL;}
		inline int *GetDataI (void)				{return NULL;}
		inline float *GetDataF (void)			{return data;}
	};
	#undef VNAME
	#undef VTYPE

    // Matrix Code Definitions (Inlined)

	// MatrixI Code Definition
	#define VNAME		I
	#define VTYPE		int

	// Constructors/Destructors

	inline MatrixI::MatrixI (void) {data = NULL; Size (0,0);}
	inline MatrixI::~MatrixI (void) {if (data!=NULL) delete[] data;}
	inline MatrixI::MatrixI (int r, int c) {data = NULL; Size (r,c);}

	// Member Functions

	inline VTYPE &MatrixI::operator () (int r, int c)
	{
		#ifdef MATRIX_DEBUG
			if (data==NULL) debug.Output ("MatrixI::op(): Matrix data is null\n", "");
			if (r<0 || r>=rows) debug.Output ("MatrixI:op(): Row is out of bounds\n", "");
			if (c<0 || c>=cols) debug.Output ("MatrixI:op(): Col is out of bounds\n", "");
		#endif
		return *(data + (r*cols+c));
	}
	inline MatrixI &MatrixI::operator= (unsigned char op)	{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ = (VTYPE) op; return *this;}
	inline MatrixI &MatrixI::operator= (int op)				{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ = (VTYPE) op; return *this;}
	inline MatrixI &MatrixI::operator= (float op)			{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ = (VTYPE) op; return *this;}
	inline MatrixI &MatrixI::operator= (MatrixI &op)			{
		#ifdef MATRIX_DEBUG		
			if (op.data==NULL)						debug.Output ("MatrixI::m=op: Operand matrix (op) data is null\n", "");        
		#endif
		if (rows!=op.rows || cols!=op.cols || data==NULL) Size (op.rows, op.cols);		
		memcpy (data, op.data, len);	// Use only for matricies of like types
		return *this;
	}
	inline MatrixI &MatrixI::operator= (MatrixF &op)			{
		#ifdef MATRIX_DEBUG		
			if (op.data==NULL)						debug.Output ("MatrixI::m=op: Operand matrix (op) data is null\n", "");        
		#endif
		if (rows!=op.rows || cols!=op.cols || data==NULL) Size (op.rows, op.cols);		
		VTYPE *n, *ne;
		float *b;
		n = data; ne = data + len; b = op.data;
		for (; n<ne;) *n++ = (VTYPE) *b++;	
		//memcpy (data, op.data, len);	
		return *this;
	}

	inline MatrixI &MatrixI::operator+= (unsigned char op)	{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ += (VTYPE) op; return *this;}
	inline MatrixI &MatrixI::operator+= (int op)				{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ += (VTYPE) op; return *this;}
	inline MatrixI &MatrixI::operator+= (float op)			{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ += (VTYPE) op; return *this;}
	inline MatrixI &MatrixI::operator+= (MatrixI &op)		{
		#ifdef MATRIX_DEBUG
			if (data==NULL)							debug.Output ("MatrixI::m+=op: Matrix data is null\n", "");
			if (op.data==NULL)						debug.Output ("MatrixI::m+=op: Operand matrix (op) data is null\n", "");
			if (rows!=op.rows || cols!=op.cols)		debug.Output ("MatrixI::m+=op: Matricies must be same size\n", "");
		#endif	
		VTYPE *n, *ne;
		int *b;
		n = data; ne = data + len; b = op.data;
		for (; n<ne;) *n++ += (VTYPE) *b++;
		return *this;
	}
	inline MatrixI &MatrixI::operator+= (MatrixF &op)		{
		#ifdef MATRIX_DEBUG
			if (data==NULL)							debug.Output ("MatrixI::m+=op: Matrix data is null\n", "");
			if (op.data==NULL)						debug.Output ("MatrixI::m+=op: Operand matrix (op) data is null\n", "");
			if (rows!=op.rows || cols!=op.cols)		debug.Output ("MatrixI::m+=op: Matricies must be the same size\n", "");		
		#endif
		VTYPE *n, *ne;
		float *b;
		n = data; ne = data + len; b = op.data;
		for (; n<ne;) *n++ += (VTYPE) *b++;
		return *this;
	}

	inline MatrixI &MatrixI::operator-= (unsigned char op)	{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ -= (VTYPE) op; return *this;}
	inline MatrixI &MatrixI::operator-= (int op)				{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ -= (VTYPE) op; return *this;}
	inline MatrixI &MatrixI::operator-= (float op)			{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ -= (VTYPE) op; return *this;}
	inline MatrixI &MatrixI::operator-= (MatrixI &op)		{
		#ifdef MATRIX_DEBUG
			if (data==NULL)							debug.Output ("MatrixI::m-=op: Matrix data is null\n", "");
			if (op.data==NULL)						debug.Output ("MatrixI::m-=op: Operand matrix (op) data is null\n", "");
			if (rows!=op.rows || cols!=op.cols)		debug.Output ("MatrixI::m-=op: Matricies must be same size\n", "");
		#endif	
		VTYPE *n, *ne;
		int *b;
		n = data; ne = data + len; b = op.data;
		for (; n<ne;) *n++ -= (VTYPE) *b++;
		return *this;
	}
	inline MatrixI &MatrixI::operator-= (MatrixF &op)		{
		#ifdef MATRIX_DEBUG
			if (data==NULL)							debug.Output ("MatrixI::m-=op: Matrix data is null\n", "");
			if (op.data==NULL)						debug.Output ("MatrixI::m-=op: Operand matrix (op) data is null\n", "");
			if (rows!=op.rows || cols!=op.cols)		debug.Output ("MatrixI::m-=op: Matricies must be the same size\n", "");		
		#endif
		VTYPE *n, *ne;
		float *b;
		n = data; ne = data + len; b = op.data;
		for (; n<ne;) *n++ -= (VTYPE) *b++;
		return *this;
	}

	inline MatrixI &MatrixI::operator*= (unsigned char op)	{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ *= (VTYPE) op; return *this;}
	inline MatrixI &MatrixI::operator*= (int op)				{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ *= (VTYPE) op; return *this;}
	inline MatrixI &MatrixI::operator*= (float op)			{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ *= (VTYPE) op; return *this;}
	inline MatrixI &MatrixI::operator*= (MatrixI &op)		{
		#ifdef MATRIX_DEBUG
			if (data==NULL)							debug.Output ("MatrixI::m*=op: Matrix data is null\n", "");
			if (op.data==NULL)						debug.Output ("MatrixI::m*=op: Operand matrix (op) data is null\n", "");
			if (rows!=op.rows || cols!=op.cols)		debug.Output ("MatrixI::m*=op: Matricies must be same size\n", "");
		#endif	
		VTYPE *n, *ne;
		int *b;
		n = data; ne = data + len; b = op.data;
		for (; n<ne;) *n++ *= (VTYPE) *b++;
		return *this;
	}
	inline MatrixI &MatrixI::operator*= (MatrixF &op)		{
		#ifdef MATRIX_DEBUG
			if (data==NULL)							debug.Output ("MatrixI::m*=op: Matrix data is null\n", "");
			if (op.data==NULL)						debug.Output ("MatrixI::m*=op: Operand matrix (op) data is null\n", "");
			if (rows!=op.rows || cols!=op.cols)		debug.Output ("MatrixI::m*=op: Matricies must be the same size\n", "");		
		#endif
		VTYPE *n, *ne;
		float *b;
		n = data; ne = data + len; b = op.data;
		for (; n<ne;) *n++ *= (VTYPE) *b++;
		return *this;
	}

	inline MatrixI &MatrixI::operator/= (unsigned char op)	{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ /= (VTYPE) op; return *this;}
	inline MatrixI &MatrixI::operator/= (int op)				{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ /= (VTYPE) op; return *this;}
	inline MatrixI &MatrixI::operator/= (float op)			{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ /= (VTYPE) op; return *this;}
	inline MatrixI &MatrixI::operator/= (MatrixI &op)		{
		#ifdef MATRIX_DEBUG
			if (data==NULL)							debug.Output ("MatrixI::m/=op: Matrix data is null\n", "");
			if (op.data==NULL)						debug.Output ("MatrixI::m/=op: Operand matrix (op) data is null\n", "");
			if (rows!=op.rows || cols!=op.cols)		debug.Output ("MatrixI::m/=op: Matricies must be same size\n", "");
		#endif	
		VTYPE *n, *ne;
		int *b;
		n = data; ne = data + len; b = op.data;
		for (; n<ne;) if (*b!=(VTYPE) 0) {*n++ /= (VTYPE) *b++;} else {*n++ = (VTYPE) 0; b++;}
		return *this;
	}
	inline MatrixI &MatrixI::operator/= (MatrixF &op)		{
		#ifdef MATRIX_DEBUG
			if (data==NULL)							debug.Output ("MatrixI::m/=op: Matrix data is null\n", "");
			if (op.data==NULL)						debug.Output ("MatrixI::m/=op: Operand matrix (op) data is null\n", "");
			if (rows!=op.rows || cols!=op.cols)		debug.Output ("MatrixI::m/=op: Matricies must be the same size\n", "");		
		#endif
		VTYPE *n, *ne;
		float *b;
		n = data; ne = data + len; b = op.data;
		for (; n<ne;) if (*b!=(VTYPE) 0) {*n++ /= (VTYPE) *b++;} else {*n++ = (VTYPE) 0; b++;}
		return *this;
	}

	inline MatrixI &MatrixI::Multiply (MatrixF &op) {
		#ifdef MATRIX_DEBUG 
			if (data==NULL)						debug.Output ("MatrixI::m mult op: Matrix data is null\n", "");
			if (op.data==NULL)					debug.Output ("MatrixI::m mult op: Operand matrix (op) data is null\n", "");
			if (cols!=op.rows)					debug.Output ("MatrixI::m mult op: Matricies not compatible (m.cols != op.rows)\n", "");
		#endif
		if (cols==op.rows) {
			VTYPE *newdata, *n, *ne, *a, *as;		// Pointers into A and new A matricies
			float *b, *bs, *bce, *be;				// Pointers into B matrix
			int newr = rows, newc = op.cols;		// Set new rows and columns
			int newlen = newr * newc;				// Determine new matrix size
			newdata = new VTYPE[newlen];			// Allocate new matrix to hold multiplication
			if (newdata==NULL)						{debug.Output ("MatrixF::m*=op: Cannot allocate new matrix.\n", ""); exit(-1);}
			ne = newdata + newlen;					// Calculate end of new matrix
			int bskip = op.cols;					// Calculate row increment for B matrix	
			bce = op.data + bskip;					// Calculate end of first row in B matrix
			be = op.data + op.rows*op.cols;			// Calculate end of B matrix	
			as = data; bs = op.data;				// Goto start of A and B matricies
			for (n=newdata ;n<ne;) {				// Compute C = A*B		
				a = as; b = bs;						// Goto beginning of row in A, top of col in B
				*n = (VTYPE) 0;						// Initialize n element in C
				for (; b<be;) {*n += (VTYPE) ((*a++) * (*b)); b += bskip;}	// Compute n element in C
				if (++bs >= bce) {					// If last col in B..
					bs = op.data;					// Go back to first column in B
					as += cols;					// Goto next row in A
				}
				n++;								// Goto next element in C
			}	
			delete[] data;							// Destroy old A matrix
			data = newdata; rows = newr; cols = newc; len = newlen;		// Replace with new A matrix	
		}
		return *this;
	}

	inline MatrixI &MatrixI::Size (int r, int c)
	{
		if (data!=NULL) {
			if (rows!=r || cols!=c) {delete[] data;	len = (rows = r) * (cols = c); data = new VTYPE[len];}
		} else {
			len = (rows = r) * (cols = c); data = new VTYPE[len];
		}
		#ifdef MATRIX_DEBUG
			if (data==NULL) debug.Output ("MatrixI::Size: Out of memory for construction.\n", "");
		#endif	
		#ifdef MATRIX_INITIALIZE
			memset (data, 0, sizeof(VTYPE)*len);		
		#endif		
		return *this;
	}
	inline MatrixI &MatrixI::SizeSafe (int r, int c)
	{
		VTYPE *newdata;
		int newlen;
		VTYPE *n, *ne;
		VTYPE *b, *be;
		int bskip;
			
				
		if (data!=NULL) {
			newlen = r*c;		
			newdata = new VTYPE[newlen];
			#ifdef MATRIX_DEBUG
				if (newdata==NULL)
					debug.Output ("MatrixI::SizeSafe: Out of memory for construction.\n", "");
			#endif		
			if (r>=rows && c>=cols) {			// New size is larger (in both r and c)			
				memset (newdata, 0, newlen*sizeof(VTYPE));	// Clear new matrix
				ne = data + len;					// Calculate end of current matrix
				b = newdata;						// Start of new matrix
				be = newdata + cols;				// Last filled column+1 in new matrix
				bskip = c-cols;
				for (n = data; n<ne;) {				// Fill new matrix with old
					for (; b<be;) *b++ = *n++;
					b += bskip; 
					be += c;
				}
			} else if (r<rows && c<cols) {		// New size is smaller (in both r and c)
				ne = newdata + newlen;			// Calculate end of new matrix
				b = data;						// Start of old matrix
				be = data + c;					// Last retrieved column+1 in old matrix
				bskip = cols-c;
				for (n = newdata; n<ne;) {		// Fill new matrix with old
					for (; b<be;) *n++ = *b++;
					b += bskip;
					be += c;
				}
			} else {							// Asymetrical resize
				#ifdef MATRIX_DEBUG
					debug.Output ("MatrixI::SizeSafe: Asymetrical resize NOT YET IMPLEMENTED.\n", "");
				#endif
				exit (202);
			}
			delete[] data;
			rows = r; cols = c;
			data = newdata; len = newlen;
		} else {
			len = (rows = r) * (cols = c);
			data = new VTYPE[len];
			#ifdef MATRIX_DEBUG
				if (data==NULL)
					debug.Output ("MatrixI::SizeSafe: Out of memory for construction.\n", "");
			#endif
		}	
		return *this;
	}
	inline MatrixI &MatrixI::InsertRow (int r)
	{
		VTYPE *newdata;
		VTYPE *r_src, *r_dest;
		int newlen;

		if (data!=NULL) {
			newlen = (rows+1)*cols;
			newdata = new VTYPE[newlen];
			#ifdef MATRIX_DEBUG
				if (newdata==NULL)
					debug.Output ("MatrixI::InsertRow: Out of memory for construction.\n", "");
			#endif
			memcpy (newdata, data, r*cols*sizeof(VTYPE));
			if (r<rows) {
				r_src = data + r*cols;
				r_dest = newdata + (r+1)*cols;		
				if (r<rows) memcpy (r_dest, r_src, (rows-r)*cols*sizeof(VTYPE));		
			}
			r_dest = newdata + r*cols;
			memset (r_dest, 0, cols*sizeof(VTYPE));
			rows++;		
			delete[] data;
			data = newdata; len = newlen;	
		} else {		
			#ifdef MATRIX_DEBUG
				debug.Output ("MatrixI::InsertRow: Cannot insert row in a null matrix.\n", "");
			#endif
		}
		return *this;
	}
	inline MatrixI &MatrixI::InsertCol (int c)
	{
		VTYPE *newdata;
		int newlen;

		if (data!=NULL) {		
			newlen = rows*(cols+1);
			newdata = new VTYPE[newlen];
			#ifdef MATRIX_DEBUG
				if (newdata==NULL)
					debug.Output ("MatrixI::InsertCol: Out of memory for construction.\n", "");
			#endif
			VTYPE *n, *ne;
			VTYPE *b, *be;
			int bskip, nskip;
			
			if (c>0) {				
				n = data;							// Copy columns to left of c
				ne = data + len;
				nskip = (cols-c);
				b = newdata;
				be = newdata + c;
				bskip = (cols-c)+1;
				for (; n<ne;) {
					for (; b<be; ) *b++ = *n++;
					b += bskip;
					be += (cols+1);
					n += nskip;
				}
			}
			if (c<cols) {
				n = data + c;						// Copy columns to right of c
				ne = data + len;
				nskip = c;
				b = newdata + (c+1);
				be = newdata + (cols+1);
				bskip = c+1;
				for (; n<ne;) {
					for (; b<be; ) *b++ = *n++;
					b += bskip;
					be += (cols+1);
					n += nskip;
				}
			}
			cols++;
			for (n=newdata+c, ne=newdata+len; n<ne; n+=cols) *n = (VTYPE) 0;		
			delete[] data;
			data = newdata; len = newlen;					
		} else {		
			#ifdef MATRIX_DEBUG
				debug.Output ("MatrixF::InsertCol: Cannot insert col in a null matrix.\n", "");
			#endif
		}
		return *this;
	}
	inline MatrixI &MatrixI::Transpose (void)
	{
		VTYPE *newdata;
		int r = rows;
		
		if (data!=NULL) {
			if (rows==1) {
				rows = cols; cols = 1;
			} else if (cols==1) {
				cols = rows; rows = 1;	
			} else {		
				newdata = new VTYPE[len];
				#ifdef MATRIX_DEBUG
					if (newdata==NULL)
						debug.Output ("MatrixF::Transpose: Out of memory for construction.\n", "");
				#endif	
				VTYPE *n, *ne;
				VTYPE *b, *be;			
				n = data;						// Goto start of old matrix
				ne = data + len;
				b = newdata;					// Goto start of new matrix
				be = newdata + len;					
				for (; n<ne; ) {				// Copy rows of old to columns of new
					for (; b<be; b+=r) *b  = *n++;
					b -= len;
					b++;
				}
			}		
			delete[] data;
			data = newdata;
			rows = cols; cols = r;
		} else {
			#ifdef MATRIX_DEBUG
				debug.Output ("MatrixI::Transpose: Cannot transpose a null matrix.\n", "");
			#endif
		}
		return *this;
	}
	inline MatrixI &MatrixI::Identity (int order)
	{
  		Size (order, order);
		VTYPE *n, *ne;	
		memset (data, 0, len*sizeof(VTYPE));	// Fill matrix with zeros
		n = data;
		ne = data + len;
		for (; n<ne; ) {
			*n = 1;								// Set diagonal element to 1
			n+= cols;							
			n++;								// Next diagonal element
		}
		return *this;
	}
	inline MatrixI &MatrixI::Basis (Vector3DF &c1, Vector3DF &c2, Vector3DF &c3)
	{
		Size (4,4);
		VTYPE *n = data;	
		*n++ = (VTYPE) c1.X(); *n++ = (VTYPE) c2.X(); *n++ = (VTYPE) c3.X(); *n++ = (VTYPE) 0;
		*n++ = (VTYPE) c1.Y(); *n++ = (VTYPE) c2.Y(); *n++ = (VTYPE) c3.Y(); *n++ = (VTYPE) 0;
		*n++ = (VTYPE) c1.Z(); *n++ = (VTYPE) c2.Z(); *n++ = (VTYPE) c3.Z(); *n++ = (VTYPE) 0;
		*n++ = (VTYPE) 0; *n++ = (VTYPE) 0; *n++ = (VTYPE) 0; *n++ = (VTYPE) 0;
		return *this;
	}
	inline MatrixI &MatrixI::GaussJordan (MatrixF &b)
	{
		// Gauss-Jordan solves the matrix equation Ax = b
		// Given the problem:
		//		A*x = b		(where A is 'this' matrix and b is provided)
		// The solution is:
		//		Ainv*b = x
		// This function returns Ainv in A and x in b... that is:
		//		A (this) -> Ainv
		//		b -> solution x
		//

		#ifdef MATRIX_DEBUG
			debug.Output ("MatrixI::GaussJordan: Not implemented for int matrix\n","");
		#endif	
		return *this;
	}
	inline int MatrixI::GetRows(void)				{return rows;}
	inline int MatrixI::GetCols(void)				{return cols;}
	inline int MatrixI::GetLength(void)			{return len;}
	inline VTYPE *MatrixI::GetData(void)			{return data;}

	#undef VTYPE
	#undef VNAME


	// MatrixF Code Definition
	#define VNAME		F
	#define VTYPE		float

	// Constructors/Destructors

	inline MatrixF::MatrixF (void) {data = NULL; Size (0,0);}
	inline MatrixF::~MatrixF (void) {if (data!=NULL) delete[] data;}
	inline MatrixF::MatrixF (int r, int c) {data = NULL; Size (r,c);}

	// Member Functions

	inline VTYPE &MatrixF::operator () (int r, int c)
	{
		#ifdef MATRIX_DEBUG
			if (data==NULL) debug.Output ("MatrixF::op(): Matrix data is null\n", "");
			if (r<0 || r>=rows) debug.Output ("MatrixF:op(): Row is out of bounds\n", "");
			if (c<0 || c>=cols) debug.Output ("MatrixF:op(): Col is out of bounds\n", "");
		#endif
		return *(data + (r*cols+c));
	}
	inline MatrixF &MatrixF::operator= (unsigned char op)	{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ = (VTYPE) op; return *this;}
	inline MatrixF &MatrixF::operator= (int op)				{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ = (VTYPE) op; return *this;}
	inline MatrixF &MatrixF::operator= (float op)			{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ = (VTYPE) op; return *this;}
	inline MatrixF &MatrixF::operator= (MatrixI &op)			{
		#ifdef MATRIX_DEBUG		
			if (op.data==NULL)						debug.Output ("MatrixF::m=op: Operand matrix (op) data is null\n", "");        
		#endif
		if (rows!=op.rows || cols!=op.cols || data==NULL) Size (op.rows, op.cols);		
		VTYPE *n, *ne;
		int *b;
		n = data; ne = data + len; b = op.data;
		for (; n<ne;) *n++ = (VTYPE) *b++;
		//memcpy (data, op.data, len*sizeof(VTYPE));	// Use only for matricies of like types
		return *this;
	}
	inline MatrixF &MatrixF::operator= (MatrixF &op)			{
		#ifdef MATRIX_DEBUG		
			if (op.data==NULL)						debug.Output ("MatrixF::m=op: Operand matrix (op) data is null\n", "");        
		#endif
		if (rows!=op.rows || cols!=op.cols || data==NULL) Size (op.rows, op.cols);		
		memcpy (data, op.data, len*sizeof(VTYPE));	
		return *this;
	}

	inline MatrixF &MatrixF::operator+= (unsigned char op)	{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ += (VTYPE) op; return *this;}
	inline MatrixF &MatrixF::operator+= (int op)				{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ += (VTYPE) op; return *this;}
	inline MatrixF &MatrixF::operator+= (float op)			{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ += (VTYPE) op; return *this;}
	inline MatrixF &MatrixF::operator+= (MatrixI &op)		{
		#ifdef MATRIX_DEBUG
			if (data==NULL)							debug.Output ("MatrixF::m+=op: Matrix data is null\n", "");
			if (op.data==NULL)						debug.Output ("MatrixF::m+=op: Operand matrix (op) data is null\n", "");
			if (rows!=op.rows || cols!=op.cols)		debug.Output ("MatrixF::m+=op: Matricies must be same size\n", "");
		#endif	
		VTYPE *n, *ne;
		int *b;
		n = data; ne = data + len; b = op.data;
		for (; n<ne;) *n++ += (VTYPE) *b++;
		return *this;
	}
	inline MatrixF &MatrixF::operator+= (MatrixF &op)		{
		#ifdef MATRIX_DEBUG
			if (data==NULL)							debug.Output ("MatrixF::m+=op: Matrix data is null\n", "");
			if (op.data==NULL)						debug.Output ("MatrixF::m+=op: Operand matrix (op) data is null\n", "");
			if (rows!=op.rows || cols!=op.cols)		debug.Output ("MatrixF::m+=op: Matricies must be the same size\n", "");		
		#endif
		VTYPE *n, *ne;
		float *b;
		n = data; ne = data + len; b = op.data;
		for (; n<ne;) *n++ += (VTYPE) *b++;
		return *this;
	}

	inline MatrixF &MatrixF::operator-= (unsigned char op)	{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ -= (VTYPE) op; return *this;}
	inline MatrixF &MatrixF::operator-= (int op)				{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ -= (VTYPE) op; return *this;}
	inline MatrixF &MatrixF::operator-= (float op)			{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ -= (VTYPE) op; return *this;}
	inline MatrixF &MatrixF::operator-= (MatrixI &op)		{
		#ifdef MATRIX_DEBUG
			if (data==NULL)							debug.Output ("MatrixF::m-=op: Matrix data is null\n", "");
			if (op.data==NULL)						debug.Output ("MatrixF::m-=op: Operand matrix (op) data is null\n", "");
			if (rows!=op.rows || cols!=op.cols)		debug.Output ("MatrixF::m-=op: Matricies must be same size\n", "");
		#endif	
		VTYPE *n, *ne;
		int *b;
		n = data; ne = data + len; b = op.data;
		for (; n<ne;) *n++ -= (VTYPE) *b++;
		return *this;
	}
	inline MatrixF &MatrixF::operator-= (MatrixF &op)		{
		#ifdef MATRIX_DEBUG
			if (data==NULL)							debug.Output ("MatrixF::m-=op: Matrix data is null\n", "");
			if (op.data==NULL)						debug.Output ("MatrixF::m-=op: Operand matrix (op) data is null\n", "");
			if (rows!=op.rows || cols!=op.cols)		debug.Output ("MatrixF::m-=op: Matricies must be the same size\n", "");		
		#endif
		VTYPE *n, *ne;
		float *b;
		n = data; ne = data + len; b = op.data;
		for (; n<ne;) *n++ -= (VTYPE) *b++;
		return *this;
	}

	inline MatrixF &MatrixF::operator*= (unsigned char op)	{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ *= (VTYPE) op; return *this;}
	inline MatrixF &MatrixF::operator*= (int op)				{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ *= (VTYPE) op; return *this;}
	inline MatrixF &MatrixF::operator*= (float op)			{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ *= (VTYPE) op; return *this;}
	inline MatrixF &MatrixF::operator*= (MatrixI &op)		{
		#ifdef MATRIX_DEBUG
			if (data==NULL)							debug.Output ("MatrixF::m*=op: Matrix data is null\n", "");
			if (op.data==NULL)						debug.Output ("MatrixF::m*=op: Operand matrix (op) data is null\n", "");
			if (rows!=op.rows || cols!=op.cols)		debug.Output ("MatrixF::m*=op: Matricies must be same size\n", "");
		#endif	
		VTYPE *n, *ne;
		int *b;
		n = data; ne = data + len; b = op.data;
		for (; n<ne;) *n++ *= (VTYPE) *b++;
		return *this;
	}
	inline MatrixF &MatrixF::operator*= (MatrixF &op)		{
		#ifdef MATRIX_DEBUG
			if (data==NULL)							debug.Output ("MatrixF::m*=op: Matrix data is null\n", "");
			if (op.data==NULL)						debug.Output ("MatrixF::m*=op: Operand matrix (op) data is null\n", "");
			if (rows!=op.rows || cols!=op.cols)		debug.Output ("MatrixF::m*=op: Matricies must be the same size\n", "");		
		#endif
		VTYPE *n, *ne;
		float *b;
		n = data; ne = data + len; b = op.data;
		for (; n<ne;) *n++ *= (VTYPE) *b++;
		return *this;
	}

	inline MatrixF &MatrixF::operator/= (unsigned char op)	{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ /= (VTYPE) op; return *this;}
	inline MatrixF &MatrixF::operator/= (int op)				{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ /= (VTYPE) op; return *this;}
	inline MatrixF &MatrixF::operator/= (float op)			{VTYPE *n = data, *nlen = data + len; for (;n<nlen;) *n++ /= (VTYPE) op; return *this;}
	inline MatrixF &MatrixF::operator/= (MatrixI &op)		{
		#ifdef MATRIX_DEBUG
			if (data==NULL)							debug.Output ("MatrixF::m/=op: Matrix data is null\n", "");
			if (op.data==NULL)						debug.Output ("MatrixF::m/=op: Operand matrix (op) data is null\n", "");
			if (rows!=op.rows || cols!=op.cols)		debug.Output ("MatrixF::m/=op: Matricies must be same size\n", "");
		#endif	
		VTYPE *n, *ne;
		int *b;
		n = data; ne = data + len; b = op.data;
		for (; n<ne;) if (*b!=(VTYPE) 0) {*n++ /= (VTYPE) *b++;} else {*n++ = (VTYPE) 0; b++;}
		return *this;
	}
	inline MatrixF &MatrixF::operator/= (MatrixF &op)		{
		#ifdef MATRIX_DEBUG
			if (data==NULL)							debug.Output ("MatrixF::m/=op: Matrix data is null\n", "");
			if (op.data==NULL)						debug.Output ("MatrixF::m/=op: Operand matrix (op) data is null\n", "");
			if (rows!=op.rows || cols!=op.cols)		debug.Output ("MatrixF::m/=op: Matricies must be the same size\n", "");		
		#endif
		VTYPE *n, *ne;
		float *b;
		n = data; ne = data + len; b = op.data;
		for (; n<ne;) 
			if (*b!=(VTYPE) 0) {
				*n++ /= (VTYPE) *b++;
			} else {
				*n++ = (VTYPE) 0; b++;
			}
		return *this;
	}

	inline MatrixF &MatrixF::Multiply (MatrixF &op) {
		#ifdef MATRIX_DEBUG 
			if (data==NULL)						debug.Output ("MatrixF::m*=op: Matrix data is null\n", "");
			if (op.data==NULL)					debug.Output ("MatrixF::m*=op: Operand matrix (op) data is null\n", "");
			if (cols!=op.rows)					debug.Output ("MatrixF::m*=op: Matricies not compatible (m.cols != op.rows)\n", "");
		#endif
		if (cols==op.rows) {
			VTYPE *newdata, *n, *ne, *a, *as;		// Pointers into A and new A matricies
			float *b, *bs, *bce, *be;				// Pointers into B matrix
			int newr = rows, newc = op.cols;		// Set new rows and columns
			int newlen = newr * newc;				// Determine new matrix size
			newdata = new VTYPE[newlen];			// Allocate new matrix to hold multiplication
			if (newdata==NULL)						{debug.Output ("MatrixF::m*=op: Cannot allocate new matrix.\n", ""); exit(-1);}
			ne = newdata + newlen;					// Calculate end of new matrix
			int bskip = op.cols;					// Calculate row increment for B matrix	
			bce = op.data + bskip;					// Calculate end of first row in B matrix
			be = op.data + op.rows*op.cols;			// Calculate end of B matrix	
			as = data; bs = op.data;				// Goto start of A and B matricies
			for (n=newdata ;n<ne;) {				// Compute C = A*B		
				a = as; b = bs;						// Goto beginning of row in A, top of col in B
				*n = (VTYPE) 0;						// Initialize n element in C
				for (; b<be;) {*n += (*a++) * (*b); b += bskip;}	// Compute n element in C
				if (++bs >= bce) {					// If last col in B..
					bs = op.data;					// Go back to first column in B
					as += cols;					// Goto next row in A
				}
				n++;								// Goto next element in C
			}	
			delete[] data;							// Destroy old A matrix
			data = newdata; rows = newr; cols = newc; len = newlen;		// Replace with new A matrix	
		}
		return *this;
	}

	inline MatrixF &MatrixF::Multiply4x4 (MatrixF &op) {
		#ifdef MATRIX_DEBUG 
			if (data==NULL)						debug.Output ("MatrixF::Multiply4x4 m*=op: Matrix data is null\n", "");
			if (op.data==NULL)					debug.Output ("MatrixF::Multiply4x4 m*=op: Operand matrix (op) data is null\n", "");
			if (rows!=4 || cols!=4)				debug.Output ("MatrixF::Multiply4x4 m*=op: Matrix m is not 4x4", "");
			if (op.rows!=4 || op.cols!=4)		debug.Output ("MatrixF::Multiply4x4 m*=op: Matrix op is not 4x4", "");
		#endif
		register float c1, c2, c3, c4;					// Temporary storage
		VTYPE *n, *a, *b1, *b2, *b3, *b4;
		a = data;	n = data; 
		b1 = op.data; b2 = op.data + 4; b3 = op.data + 8; b4 = op.data + 12;

		c1 = *a++;	c2 = *a++;	c3 = *a++; c4 = *a++;					// Calculate First Row
		*n++ = c1*(*b1++) + c2*(*b2++) + c3*(*b3++) + c4*(*b4++);	
		*n++ = c1*(*b1++) + c2*(*b2++) + c3*(*b3++) + c4*(*b4++);
		*n++ = c1*(*b1++) + c2*(*b2++) + c3*(*b3++) + c4*(*b4++);
		*n++ = c1*(*b1) + c2*(*b2) + c3*(*b3) + c4*(*b4);
		b1 -= 3 ; b2 -= 3; b3 -= 3; b4 -= 3;

		c1 = *a++;	c2 = *a++;	c3 = *a++; c4 = *a++;					// Calculate Second Row
		*n++ = c1*(*b1++) + c2*(*b2++) + c3*(*b3++) + c4*(*b4++);	
		*n++ = c1*(*b1++) + c2*(*b2++) + c3*(*b3++) + c4*(*b4++);
		*n++ = c1*(*b1++) + c2*(*b2++) + c3*(*b3++) + c4*(*b4++);
		*n++ = c1*(*b1) + c2*(*b2) + c3*(*b3) + c4*(*b4);
		b1 -= 3 ; b2 -= 3; b3 -= 3; b4 -= 3;

		c1 = *a++;	c2 = *a++;	c3 = *a++; c4 = *a++;					// Calculate Third Row
		*n++ = c1*(*b1++) + c2*(*b2++) + c3*(*b3++) + c4*(*b4++);	
		*n++ = c1*(*b1++) + c2*(*b2++) + c3*(*b3++) + c4*(*b4++);
		*n++ = c1*(*b1++) + c2*(*b2++) + c3*(*b3++) + c4*(*b4++);
		*n++ = c1*(*b1) + c2*(*b2) + c3*(*b3) + c4*(*b4);
		b1 -= 3 ; b2 -= 3; b3 -= 3; b4 -= 3;

		c1 = *a++;	c2 = *a++;	c3 = *a++; c4 = *a;						// Calculate Four Row
		*n++ = c1*(*b1++) + c2*(*b2++) + c3*(*b3++) + c4*(*b4++);	
		*n++ = c1*(*b1++) + c2*(*b2++) + c3*(*b3++) + c4*(*b4++);
		*n++ = c1*(*b1++) + c2*(*b2++) + c3*(*b3++) + c4*(*b4++);
		*n = c1*(*b1) + c2*(*b2) + c3*(*b3) + c4*(*b4);	

		return *this;
	}


	inline MatrixF &MatrixF::Size (int r, int c)
	{
		if (data!=NULL) {
			if (rows!=r || cols!=c) {delete[] data;	len = (rows = r) * (cols = c); data = new VTYPE[len];}
		} else {
			len = (rows = r) * (cols = c); data = new VTYPE[len];
		}
		#ifdef MATRIX_DEBUG
			if (data==NULL) debug.Output ("MatrixF::Size: Out of memory for construction.\n", "");
		#endif	
		#ifdef MATRIX_INITIALIZE
			memset (data, 0, sizeof(VTYPE)*len);		
		#endif		
		return *this;
	}
	inline MatrixF &MatrixF::SizeSafe (int r, int c)
	{
		VTYPE *newdata;
		int newlen;
		VTYPE *n, *ne;
		VTYPE *b, *be;
		int bskip;
			
				
		if (data!=NULL) {
			newlen = r*c;		
			newdata = new VTYPE[newlen];
			#ifdef MATRIX_DEBUG
				if (newdata==NULL)
					debug.Output ("MatrixF::SizeSafe: Out of memory for construction.\n", "");
			#endif		
			if (r>=rows && c>=cols) {			// New size is larger (in both r and c)			
				memset (newdata, 0, newlen*sizeof(VTYPE));	// Clear new matrix
				ne = data + len;					// Calculate end of current matrix
				b = newdata;						// Start of new matrix
				be = newdata + cols;				// Last filled column+1 in new matrix
				bskip = c-cols;
				for (n = data; n<ne;) {				// Fill new matrix with old
					for (; b<be;) *b++ = *n++;
					b += bskip; 
					be += c;
				}
			} else if (r<rows && c<cols) {		// New size is smaller (in both r and c)
				ne = newdata + newlen;			// Calculate end of new matrix
				b = data;						// Start of old matrix
				be = data + c;					// Last retrieved column+1 in old matrix
				bskip = cols-c;
				for (n = newdata; n<ne;) {		// Fill new matrix with old
					for (; b<be;) *n++ = *b++;
					b += bskip;
					be += c;
				}
			} else {							// Asymetrical resize
				#ifdef MATRIX_DEBUG
					debug.Output ("MatrixF::SizeSafe: Asymetrical resize NOT YET IMPLEMENTED.\n", "");
				#endif
				exit (202);
			}
			delete[] data;
			rows = r; cols = c;
			data = newdata; len = newlen;
		} else {
			len = (rows = r) * (cols = c);
			data = new VTYPE[len];
			#ifdef MATRIX_DEBUG
				if (data==NULL)
					debug.Output ("MatrixF::SizeSafe: Out of memory for construction.\n", "");
			#endif
		}	
		return *this;
	}
	inline MatrixF &MatrixF::InsertRow (int r)
	{
		VTYPE *newdata;
		VTYPE *r_src, *r_dest;
		int newlen;

		if (data!=NULL) {
			newlen = (rows+1)*cols;
			newdata = new VTYPE[newlen];
			#ifdef MATRIX_DEBUG
				if (newdata==NULL)
					debug.Output ("MatrixF::InsertRow: Out of memory for construction.\n", "");
			#endif
			memcpy (newdata, data, r*cols*sizeof(VTYPE));
			if (r<rows) {
				r_src = data + r*cols;
				r_dest = newdata + (r+1)*cols;		
				if (r<rows) memcpy (r_dest, r_src, (rows-r)*cols*sizeof(VTYPE));		
			}
			r_dest = newdata + r*cols;
			memset (r_dest, 0, cols*sizeof(VTYPE));
			rows++;
			delete[] data;
			data = newdata; len = newlen;	
		} else {		
			#ifdef MATRIX_DEBUG
				debug.Output ("MatrixF::InsertRow: Cannot insert row in a null matrix.\n", "");
			#endif
		}
		return *this;
	}
	inline MatrixF &MatrixF::InsertCol (int c)
	{
		VTYPE *newdata;
		int newlen;

		if (data!=NULL) {		
			newlen = rows*(cols+1);
			newdata = new VTYPE[newlen];
			#ifdef MATRIX_DEBUG
				if (newdata==NULL)
					debug.Output ("MatrixF::InsertCol: Out of memory for construction.\n", "");
			#endif
			VTYPE *n, *ne;
			VTYPE *b, *be;
			int bskip, nskip;
			
			if (c>0) {				
				n = data;							// Copy columns to left of c
				ne = data + len;
				nskip = (cols-c);
				b = newdata;
				be = newdata + c;
				bskip = (cols-c)+1;
				for (; n<ne;) {
					for (; b<be; ) *b++ = *n++;
					b += bskip;
					be += (cols+1);
					n += nskip;
				}
			}
			if (c<cols) {
				n = data + c;						// Copy columns to right of c
				ne = data + len;
				nskip = c;
				b = newdata + (c+1);
				be = newdata + (cols+1);
				bskip = c+1;
				for (; n<ne;) {
					for (; b<be; ) *b++ = *n++;
					b += bskip;
					be += (cols+1);
					n += nskip;
				}
			}
			cols++;
			for (n=newdata+c, ne=newdata+len; n<ne; n+=cols) *n = (VTYPE) 0;		
			delete[] data;
			data = newdata; len = newlen;					
		} else {		
			#ifdef MATRIX_DEBUG
				debug.Output ("MatrixF::InsertCol: Cannot insert col in a null matrix.\n", "");
			#endif
		}
		return *this;
	}
	inline MatrixF &MatrixF::Transpose (void)
	{
		VTYPE *newdata;
		int r = rows;
		
		if (data!=NULL) {
			if (rows==1) {
				rows = cols; cols = 1;
			} else if (cols==1) {
				cols = rows; rows = 1;	
			} else {		
				newdata = new VTYPE[len];
				#ifdef MATRIX_DEBUG
					if (newdata==NULL)
						debug.Output ("MatrixF::Transpose: Out of memory for construction.\n", "");
				#endif	
				VTYPE *n, *ne;
				VTYPE *b, *be;			
				n = data;						// Goto start of old matrix
				ne = data + len;
				b = newdata;					// Goto start of new matrix
				be = newdata + len;					
				for (; n<ne; ) {				// Copy rows of old to columns of new
					for (; b<be; b+=r) *b  = *n++;
					b -= len;
					b++;
				}
			}		
			delete[] data;
			data = newdata;
			rows = cols; cols = r;
		} else {
			#ifdef MATRIX_DEBUG
				debug.Output ("MatrixF::Transpose: Cannot transpose a null matrix.\n", "");
			#endif
		}
		return *this;
	}
	inline MatrixF &MatrixF::Identity (int order)
	{
  		Size (order, order);
		VTYPE *n, *ne;	
		memset (data, 0, len*sizeof(VTYPE));	// Fill matrix with zeros
		n = data;
		ne = data + len;
		for (; n<ne; ) {
			*n = 1;								// Set diagonal element to 1
			n+= cols;							
			n++;								// Next diagonal element
		}
		return *this;
	}
	inline MatrixF &MatrixF::RotateX (float ang)
	{
		Size (4,4);
		VTYPE *n = data;
		float c,s,a;
		a = ang * DEGtoRAD;
		c = cosf(a);
		s = sinf(a);	
		*n = 1; n += 5;
		*n++ = (VTYPE) c;
		*n = (VTYPE) -s; n+=3;
		*n++ = (VTYPE) s;
		*n = (VTYPE) c; n+=5;
		*n = 1; 
		return *this;
	}
	inline MatrixF &MatrixF::RotateY (float ang)
	{
		Size (4,4);
		VTYPE *n = data;
		float c,s,a;
		a = ang * DEGtoRAD;
		c = cosf(a);
		s = sinf(a);	
		*n = (VTYPE) c;		n+=2;
		*n = (VTYPE) s;		n+=3;
		*n = 1;				n+=3;
		*n = (VTYPE) -s;	n+=2;
		*n = (VTYPE) c;		n+=5;
		*n = 1;
		return *this;
	}
	inline MatrixF &MatrixF::RotateZ (float ang)
	{
		Size (4,4);		
		VTYPE *n = data;
		float c,s,a;
		a = ang * DEGtoRAD;
		c = cosf(a);
		s = sinf(a);	
		*n++ = (VTYPE) c;
		*n = (VTYPE) -s; n+=3;
		*n++ = (VTYPE) s;
		*n = (VTYPE) c; n+=5; 
		*n = 1; n+=5; *n = 1;
		return *this;	
	}
	inline MatrixF &MatrixF::RotateX (float sinAng, float cosAng)
	{
		Size (4,4);
		VTYPE *n = data;
		float c = cosAng;
		float s = sinAng;
		*n = 1; n += 5;
		*n++ = (VTYPE) c;
		*n = (VTYPE) -s; n+=3;
		*n++ = (VTYPE) s;
		*n = (VTYPE) c; n+=5;
		*n = 1; 
		return *this;
	}
	inline MatrixF &MatrixF::RotateY (float sinAng, float cosAng)
	{
		Size (4,4);
		VTYPE *n = data;
		float c = cosAng;
		float s = sinAng;
		*n = (VTYPE) c;		n+=2;
		*n = (VTYPE) s;		n+=3;
		*n = 1;				n+=3;
		*n = (VTYPE) -s;	n+=2;
		*n = (VTYPE) c;		n+=5;
		*n = 1;
		return *this;
	}
	inline MatrixF &MatrixF::RotateZ (float sinAng, float cosAng)
	{
		Size (4,4);		
		VTYPE *n = data;
		float c = cosAng;
		float s = sinAng;
		*n++ = (VTYPE) c;
		*n = (VTYPE) -s; n+=3;
		*n++ = (VTYPE) s;
		*n = (VTYPE) c; n+=5; 
		*n = 1; n+=5; *n = 1;
		return *this;	
	}
	inline MatrixF &MatrixF::Basis (Vector3DF &c1, Vector3DF &c2, Vector3DF &c3)
	{
		Size (4,4);
		VTYPE *n = data;	
		*n++ = (VTYPE) c1.X(); *n++ = (VTYPE) c2.X(); *n++ = (VTYPE) c3.X(); *n++ = (VTYPE) 0;
		*n++ = (VTYPE) c1.Y(); *n++ = (VTYPE) c2.Y(); *n++ = (VTYPE) c3.Y(); *n++ = (VTYPE) 0;
		*n++ = (VTYPE) c1.Z(); *n++ = (VTYPE) c2.Z(); *n++ = (VTYPE) c3.Z(); *n++ = (VTYPE) 0;
		*n++ = (VTYPE) 0; *n++ = (VTYPE) 0; *n++ = (VTYPE) 0; *n++ = (VTYPE) 0;
		return *this;
	}
	#define		SWAP(a, b)		{temp=(a); (a)=(b); (b)=temp;}
	inline MatrixF &MatrixF::GaussJordan (MatrixF &b)
	{
		// Gauss-Jordan solves the matrix equation Ax = b
		// Given the problem:
		//		A*x = b		(where A is 'this' matrix and b is provided)
		// The solution is:
		//		Ainv*b = x
		// This function returns Ainv in A and x in b... that is:
		//		A (this) -> Ainv
		//		b -> solution x
		//
		
		MatrixI index_col, index_row;	
		MatrixI piv_flag;
		int r, c, c2, rs, cs;
		float piv_val;
		int piv_row, piv_col;
		float pivinv, dummy, temp;

		#ifdef MATRIX_DEBUG
			if (rows!=cols) debug.Output ("MatrixF::GaussJordan: Number of rows and cols of A must be equal.\n", "");
			if (rows!=b.rows) debug.Output ("MatrixF::GaussJordan: Number of rows of A and rows of b must be equal.\n", "");
		#endif

		index_col.Size (1, cols);
		index_row.Size (1, cols);
		piv_flag.Size (1, rows);
		piv_flag = 0;
		for (c = 0; c < cols; c++) {
			piv_val = 0.0;		
			for (rs = 0; rs < rows; rs++) {
				if (piv_flag(0, rs) != 1 )				
					for (cs = 0; cs < cols; cs++) {
						if (piv_flag(0, cs) == 0) {
							if (fabsf((*this) (rs, cs)) >= piv_val) {
								piv_val = fabsf((*this) (rs, cs));
								piv_row = rs;
								piv_col = cs;
							}
						} else if (piv_flag(0, cs)>1) {
							#ifdef MATRIX_DEBUG
								debug.Output ("MatrixF::GaussJordan: Singular matrix (dbl pivs).\n", "");
								//Print ();
							#endif
						}
					}
			}
			piv_flag(0, piv_col)++;
			if (piv_row != piv_col) {
				for (c2 = 0; c2 < cols; c2++) SWAP ((*this) (piv_row, c2), (*this) (piv_col, c2));
				for (c2 = 0; c2 < b.cols; c2++) SWAP (b(piv_row, c2), b(piv_col, c2));
			}
			index_row (0, c) = piv_row;
			index_col (0, c) = piv_col;
			if ((*this) (piv_col, piv_col) == 0.0) {
				#ifdef MATRIX_DEBUG
					debug.Output ("MatrixF::GaussJordan: Singular matrix (0 piv).\n", "");
					//Print ();
				#endif
			}
			pivinv = 1.0f / ((*this) (piv_col, piv_col));
			(*this) (piv_col, piv_col) = 1.0;
			for (c2 = 0; c2 < cols; c2++) (*this) (piv_col, c2) *= pivinv;
			for (c2 = 0; c2 < b.cols; c2++) b(piv_col, c2) *= pivinv;
			for (r = 0; r < rows; r++) {
				if (r != piv_col) {
					dummy = (*this) (r, piv_col);
					(*this) (r, piv_col) = 0.0;
					for (c2 = 0; c2 < cols; c2++) (*this) (r, c2) -= (*this) (piv_col, c2)*dummy;
					for (c2 = 0; c2 < b.cols; c2++) b(r, c2) -= b(piv_col, c2)*dummy;
				}
			}
		}	
		for (c = cols-1; c >= 0; c--) {
			if (index_row(0, c) != index_col(0, c))
				for (r = 0; r < rows; r++)
					SWAP ((*this) (r, index_row(0,c)), (*this) (r, index_col(0,c)) );
		}
		return *this;	
	}
	inline int MatrixF::GetRows(void)				{return rows;}
	inline int MatrixF::GetCols(void)				{return cols;}
	inline int MatrixF::GetLength(void)				{return len;}
	inline VTYPE *MatrixF::GetData(void)			{return data;}
	inline void MatrixF::GetRowVec (int r, Vector3DF &v)
	{ 
		VTYPE *n = data + r*cols;
		v.x = *n++; v.y = *n++; v.z= *n++;
	}

	#undef VTYPE
	#undef VNAME
#endif