//
// GameX (Release 2 - Vectors / Matricies / CameraX)
// Vector Class Code 
// 
// Copyright (C) 2002 Rama C. Hoetzlein
// 
// This software is released under the GameX GNU GPL 
// Open Source Liscense. See the GameX documentation included
// with this source code for terms of modification, 
// distribution and re-release. 
//


#include "gamex-vector.hpp"
#include "gamex-matrix.hpp"

Vector3DF &Vector3DF::operator*= (MatrixF &op)
{
	float *m = op.GetDataF ();
	float xa, ya, za;
	xa  = x * (*m++);	ya  = x * (*m++);	za  = x * (*m++);	m++;
	xa += y * (*m++);	ya += y * (*m++);	za += y * (*m++);	m++;
	xa += z * (*m++);	ya += z * (*m++);	za += z * (*m++);	m++;
	xa +=     (*m++);	ya +=     (*m++);	za +=     (*m  );
	x = xa; y = ya; z = za;
	return *this;
}


Vector4DF &Vector4DF::operator*= (MatrixF &op)
{
	float *m = op.GetDataF ();
	float xa, ya, za, wa;
	xa  = x * (*m++);	ya  = x * (*m++);	za  = x * (*m++);	wa  = x * (*m++);
	xa += y * (*m++);	ya += y * (*m++);	za += y * (*m++);	wa += y * (*m++);
	xa += z * (*m++);	ya += z * (*m++);	za += z * (*m++);	wa += z * (*m++);
	xa += w * (*m++);	ya += w * (*m++);	za += w * (*m++);	wa += w * (*m  );
	x = xa; y = ya; z = za; w = wa;
	return *this;
}

Vector4DF &Vector4DF::Cross (const Vector4DF &v, const Vector4DF &v2)
{
	float ax =    x, ay =    y, az =    z, aw =    w;
	float bx =  v.x, by =  v.y, bz =  v.z, bw =  v.w;
	float cx = v2.x, cy = v2.y, cz = v2.z, cw = v2.w;
	float a = (bx * cy) - (by * cx);
	float b = (bx * cz) - (bz * cx);
	float c = (bx * cw) - (bw * cx);
	float d = (by * cz) - (bz * cy);
	float e = (by * cw) - (bw * cy);
	float f = (bz * cw) - (bw * cz);
	x = ((ay * f) - (az * e) + (aw * d));
	y = ((ax * f) + (az * c) - (aw * b));
	z = ((ax * e) - (ay * c) + (aw * a));
	w = (-(ax * d) + (ay * b) - (az * a));
	return *this;
}
		
