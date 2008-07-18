/*
Open Asset Import Library (ASSIMP)
----------------------------------------------------------------------

Copyright (c) 2006-2008, ASSIMP Development Team
All rights reserved.

Redistribution and use of this software in source and binary forms, 
with or without modification, are permitted provided that the 
following conditions are met:

* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of the ASSIMP team, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the ASSIMP Development Team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------
*/

/** @file Quaternion structure, including operators when compiling in C++ */
#ifndef AI_QUATERNION_H_INC
#define AI_QUATERNION_H_INC

#include <math.h>
#include "aiTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------
/** Represents a quaternion in a 4D vector. */
struct aiQuaternion
{
#ifdef __cplusplus
	aiQuaternion() : w(0.0f), x(0.0f), y(0.0f), z(0.0f) {}
	aiQuaternion(float _w, float _x, float _y, float _z) : w(_w), x(_x), y(_y), z(_z) {}

	/** Construct from rotation matrix. Result is undefined if the matrix is not orthonormal. */
	aiQuaternion( const aiMatrix3x3& pRotMatrix);

	/** Construct from euler angles */
	aiQuaternion( float rotx, float roty, float rotz);

	/** Construct from an axis angle pair */
	aiQuaternion( aiVector3D axis, float angle);

	/** Returns a matrix representation of the quaternion */
	aiMatrix3x3 GetMatrix() const;

#endif // __cplusplus

	//! w,x,y,z components of the quaternion
	float w, x, y, z;	
} ;


#ifdef __cplusplus

// ---------------------------------------------------------------------------
// Constructs a quaternion from a rotation matrix
inline aiQuaternion::aiQuaternion( const aiMatrix3x3 &pRotMatrix)
{
	float t = 1 + pRotMatrix.a1 + pRotMatrix.b2 + pRotMatrix.c3;

	// large enough
	if( t > 0.00001f)
	{
		float s = sqrt( t) * 2.0f;
		x = (pRotMatrix.b3 - pRotMatrix.c2) / s;
		y = (pRotMatrix.c1 - pRotMatrix.a3) / s;
		z = (pRotMatrix.a2 - pRotMatrix.b1) / s;
		w = 0.25f * s;
	} // else we have to check several cases
	else if( pRotMatrix.a1 > pRotMatrix.b2 && pRotMatrix.a1 > pRotMatrix.c3 )  
	{	
		// Column 0: 
		float s = sqrt( 1.0f + pRotMatrix.a1 - pRotMatrix.b2 - pRotMatrix.c3) * 2.0f;
		x = -0.25f * s;
		y = (pRotMatrix.a2 + pRotMatrix.b1) / s;
		z = (pRotMatrix.c1 + pRotMatrix.a3) / s;
		w = (pRotMatrix.c2 - pRotMatrix.b3) / s;
	} 
	else if( pRotMatrix.b2 > pRotMatrix.c3) 
	{ 
		// Column 1: 
		float s = sqrt( 1.0f + pRotMatrix.b2 - pRotMatrix.a1 - pRotMatrix.c3) * 2.0f;
		x = (pRotMatrix.a2 + pRotMatrix.b1) / s;
		y = -0.25f * s;
		z = (pRotMatrix.b3 + pRotMatrix.c2) / s;
		w = (pRotMatrix.a3 - pRotMatrix.c1) / s;
	} else 
	{ 
		// Column 2:
		float s = sqrt( 1.0f + pRotMatrix.c3 - pRotMatrix.a1 - pRotMatrix.b2) * 2.0f;
		x = (pRotMatrix.c1 + pRotMatrix.a3) / s;
		y = (pRotMatrix.b3 + pRotMatrix.c2) / s;
		z = -0.25f * s;
		w = (pRotMatrix.b1 - pRotMatrix.a2) / s;
	}
}

// ---------------------------------------------------------------------------
// Construction from euler angles
inline aiQuaternion::aiQuaternion( float fPitch, float fYaw, float fRoll )
{
	const float fSinPitch(sin(fPitch*0.5F));
	const float fCosPitch(cos(fPitch*0.5F));
	const float fSinYaw(sin(fYaw*0.5F));
	const float fCosYaw(cos(fYaw*0.5F));
	const float fSinRoll(sin(fRoll*0.5F));
	const float fCosRoll(cos(fRoll*0.5F));
	const float fCosPitchCosYaw(fCosPitch*fCosYaw);
	const float fSinPitchSinYaw(fSinPitch*fSinYaw);
	x = fSinRoll * fCosPitchCosYaw     - fCosRoll * fSinPitchSinYaw;
	y = fCosRoll * fSinPitch * fCosYaw + fSinRoll * fCosPitch * fSinYaw;
	z = fCosRoll * fCosPitch * fSinYaw - fSinRoll * fSinPitch * fCosYaw;
	w = fCosRoll * fCosPitchCosYaw     + fSinRoll * fSinPitchSinYaw;
}

// ---------------------------------------------------------------------------
// Returns a matrix representation of the quaternion
inline aiMatrix3x3 aiQuaternion::GetMatrix() const
{
	aiMatrix3x3 resMatrix;
	resMatrix.a1 = 1.0f - 2.0f * (y * y + z * z);
	resMatrix.a2 = 2.0f * (x * y + z * w);
	resMatrix.a3 = 2.0f * (x * z - y * w);
	resMatrix.b1 = 2.0f * (x * y - z * w);
	resMatrix.b2 = 1.0f - 2.0f * (x * x + z * z);
	resMatrix.b3 = 2.0f * (y * z + x * w);
	resMatrix.c1 = 2.0f * (x * z + y * w);
	resMatrix.c2 = 2.0f * (y * z - x * w);
	resMatrix.c3 = 1.0f - 2.0f * (x * x + y * y);

	return resMatrix;
}

// ---------------------------------------------------------------------------
// Construction from an axis-angle pair
inline aiQuaternion::aiQuaternion( aiVector3D axis, float angle)
{
	axis.Normalize();

	const float sin_a = sin( angle / 2 );
    const float cos_a = cos( angle / 2 );
    x    = axis.x * sin_a;
    y    = axis.y * sin_a;
    z    = axis.z * sin_a;
    w    = cos_a;
}




} // end extern "C"
#endif // __cplusplus

#endif // AI_QUATERNION_H_INC