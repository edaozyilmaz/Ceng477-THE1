#include "Camera.h"
#include <string.h>
#include <cmath>

Camera::Camera(int id,                      // Id of the camera
               const char* imageName,       // Name of the output PPM file 
               const Vector3f& pos,         // Camera position
               const Vector3f& gaze,        // Camera gaze direction
               const Vector3f& up,          // Camera up direction
               const ImagePlane& imgPlane)  // Image plane parameters
{
	/***********************************************
     *                                             *
	 * TODO: Implement this function               *
     *                                             *
     ***********************************************
	 */
     this->id = id;
     strcpy(this->imageName, imageName);
     this->pos.x = pos.x;
     this->pos.y = pos.y;
     this->pos.z = pos.z;
     this->gaze.x = gaze.x/sqrt(pow(gaze.x,2)+pow(gaze.y,2)+pow(gaze.z,2));
     this->gaze.y = gaze.y/sqrt(pow(gaze.x,2)+pow(gaze.y,2)+pow(gaze.z,2));
     this->gaze.z = gaze.z/sqrt(pow(gaze.x,2)+pow(gaze.y,2)+pow(gaze.z,2));
     this->up.x = up.x/sqrt(pow(up.x,2)+pow(up.y,2)+pow(up.z,2));
     this->up.y = up.y/sqrt(pow(up.x,2)+pow(up.y,2)+pow(up.z,2));
     this->up.z = up.z/sqrt(pow(up.x,2)+pow(up.y,2)+pow(up.z,2));
     this->imgPlane = imgPlane;
}

/* Takes coordinate of an image pixel as row and col, and
 * returns the ray going through that pixel. 
 */
Ray Camera::getPrimaryRay(int col, int row) const
{
	/***********************************************
     *                                             *
	 * TODO: Implement this function               *
     *                                             *
     ***********************************************
	 */

     Vector3f origin;
     origin.x = this->pos.x;
     origin.y = this->pos.y;
     origin.z = this->pos.z;


     Vector3f direction,m,q,s,cross;
     float u = (col + 0.5)*(imgPlane.right -imgPlane.left)/imgPlane.nx;
     float v = (row + 0.5)*(imgPlane.top -imgPlane.bottom)/imgPlane.ny;

     m.x= origin.x + imgPlane.distance * gaze.x;
     m.y= origin.y + imgPlane.distance * gaze.y;
     m.z= origin.z + imgPlane.distance * gaze.z;

     cross.x = -(up.y*gaze.z) + up.z*gaze.y;
     cross.y = -(up.z*gaze.x) + up.x*gaze.z;
     cross.z = -(up.x*gaze.y) + up.y*gaze.x;

     cross.x = cross.x/sqrt(pow(cross.x,2)+pow(cross.y,2)+pow(cross.z,2));
     cross.y = cross.y/sqrt(pow(cross.x,2)+pow(cross.y,2)+pow(cross.z,2));
     cross.z = cross.z/sqrt(pow(cross.x,2)+pow(cross.y,2)+pow(cross.z,2));

     q.x = m.x + imgPlane.left*cross.x + imgPlane.top*up.x;
     q.y = m.y + imgPlane.left*cross.y + imgPlane.top*up.y;
     q.z = m.z + imgPlane.left*cross.z + imgPlane.top*up.z;

     
     s.x = q.x + u*cross.x - v*up.x;
     s.y = q.y + u*cross.y - v*up.y;
     s.z = q.z + u*cross.z - v*up.z;

     direction.x = s.x - origin.x;
     direction.y = s.y - origin.y;
     direction.z = s.z - origin.z;

     return Ray(origin, direction);

}

