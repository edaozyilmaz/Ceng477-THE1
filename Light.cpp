#include "Light.h"
#include <cmath>

/* Constructor. Implemented for you. */
PointLight::PointLight(const Vector3f & position, const Vector3f & intensity)
    : position(position), intensity(intensity)
{
}

// Compute the contribution of light at point p using the
// inverse square law formula
Vector3f PointLight::computeLightContribution(const Vector3f& p)
{
	/***********************************************
     *                                             *
	 * TODO: Implement this function               *
     *                                             *
     ***********************************************
	 */
     Vector3f d;
     d.x = p.x - position.x;
     d.y = p.y - position.y;
     d.z = p.z - position.z;

     float d2 = pow(d.x,2) + pow(d.y,2) + pow(d.z,2);
     if (d2 == 0){
         Vector3f irradience;
         irradience.x = 0;
         irradience.y = 0;
         irradience.z = 0;

         return irradience;
     }
     Vector3f irradience;
     irradience.x = intensity.x/d2;
     irradience.y = intensity.y/d2;
     irradience.z = intensity.z/d2;

     return irradience;
}
