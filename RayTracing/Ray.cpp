//=============================================================================
//
//  CLASS Ray - IMPLEMENTATION
//
//=============================================================================
#define RAYTRACER_RAY_CPP

//== INCLUDES =================================================================
#include <algorithm>
#include <iostream>
#include <assert.h>
#include <math.h>
#include "Ray.h"

//== IMPLEMENTATION ===========================================================
Ray Ray::reflectedRay(
	const QVector3D   &v3Point,
	const QVector3D   &v3Normal )  const
{
    QVector3D raydirect = getDir();
    v3Normal.normalized();
    float  VdotN = QVector3D::dotProduct(raydirect,v3Normal);
    double bias = 0.6f;
    QVector3D reflection_direction = raydirect - 2*v3Normal*VdotN;

    Ray result;
    reflection_direction.normalize();
    result.setDir(reflection_direction);
    result.setOrigin(v3Point + v3Normal * bias );



    // Please program your solution of the first half of task b) here.
	return result;
};

//-----------------------------------------------------------------------------
bool Ray::refractedRay(
	const QVector3D   &v3Point,
	const QVector3D   &v3Normal,
	double            rRefractiveIndex,
	Ray               &clRefractedRay )  const
{

    QVector3D incoming_dir = getDir();
    double air = 1;
    QVector3D n = v3Normal;
    n.normalize();
    double bias = 0.6f;
    
    float cosTheta1 = QVector3D::dotProduct(incoming_dir, v3Normal);
    if(cosTheta1 < 0.0){//outside the surface
        cosTheta1 = -cosTheta1;
    }
    else//inside the surface
    {
        
        double tmp;
        tmp = air;
        air = rRefractiveIndex;
        rRefractiveIndex = air;
        n = -n;
    
        
    }
    float tau = air/ rRefractiveIndex;
    float cosTheta2 = 1 - (tau*tau) * ( 1 - cosTheta1*cosTheta1);//haven't sqrt yet

    if( cosTheta2 < 0.0 )
        return false;
    
    else{
        clRefractedRay.setOrigin(v3Point + n * -bias );
        QVector3D t = tau * (incoming_dir + n * cosTheta1) - sqrtf(cosTheta2)*n;
        clRefractedRay.setDir(t);
        

        return true;
    }
}
