#include "Shape.h"
#include "Scene.h"
#include <cstdio>
#include <math.h>

Shape::Shape(void)
{
}

Shape::Shape(int id, int matIndex)
    : id(id), matIndex(matIndex)
{
}


Sphere::Sphere(void)
{}

/* Constructor for sphere. You will implement this. */
Sphere::Sphere(int id, int matIndex, int cIndex, float R)
    : Shape(id, matIndex)
{
	/***********************************************
     *                                             *
	 * TODO: Implement this function               *
     *                                             *
     ***********************************************
	 */
    this->cIndex = cIndex;
    this->R = R;
}

/* Sphere-ray intersection routine. You will implement this.
Note that ReturnVal structure should hold the information related to the intersection point, e.g., coordinate of that point, normal at that point etc.
You should to declare the variables in ReturnVal structure you think you will need. It is in defs.h file. */
ReturnVal Sphere::intersect(const Ray & ray) const
{
	/***********************************************
     *                                             *
	 * TODO: Implement this function               *
     *                                             *
     ***********************************************
	 */
    Vector3f center;
    center = pScene->vertices[this->cIndex-1];
    float B = 2*(ray.direction.x*(ray.origin.x - center.x) + ray.direction.y*(ray.origin.y - center.y) + ray.direction.z*(ray.origin.z - center.z));
    float A = (ray.direction.x)*(ray.direction.x) + (ray.direction.y)*(ray.direction.y) + (ray.direction.z)*(ray.direction.z);
    float C = (ray.origin.x - center.x)*(ray.origin.x - center.x) + (ray.origin.y - center.y)*(ray.origin.y - center.y) + (ray.origin.z - center.z)*(ray.origin.z - center.z) - (this->R)*(this->R);

    ReturnVal ret;
    if (B*B -4*A*C < pScene->intTestEps){
        ret.check = false;
        return ret;
    }
    float t1 = (-B -sqrt(B*B -4*A*C))/(2*A);
    float t2 = (-B +sqrt(B*B -4*A*C))/(2*A);
    float t = (t1<=t2) ? t1 : t2;
    if(t1 > pScene->intTestEps && t2 > pScene->intTestEps){
        ret.check =true;
        ret.coordinate = ray.getPoint(t);

        ret.normal.x = (-center.x + ret.coordinate.x)/this->R;
        ret.normal.y = (-center.y + ret.coordinate.y)/this->R;
        ret.normal.z = (-center.z + ret.coordinate.z)/this->R;
    }

    return ret;
}

Triangle::Triangle(void)
{}

/* Constructor for triangle. You will implement this. */
Triangle::Triangle(int id, int matIndex, int p1Index, int p2Index, int p3Index)
    : Shape(id, matIndex)
{
	/***********************************************
     *                                             *
	 * TODO: Implement this function               *
     *                                             *
     ***********************************************
	 */
    this->p1Index = p1Index;
    this->p2Index = p2Index;
    this->p3Index = p3Index;
}

/* Triangle-ray intersection routine. You will implement this.
Note that ReturnVal structure should hold the information related to the intersection point, e.g., coordinate of that point, normal at that point etc.
You should to declare the variables in ReturnVal structure you think you will need. It is in defs.h file. */
ReturnVal Triangle::intersect(const Ray & ray) const
{
	/***********************************************
     *                                             *
	 * TODO: Implement this function               *
     *                                             *
     ***********************************************
	 */
    float beta,gama,alfa,t;
    ReturnVal ret;
    Vector3f a = pScene->vertices[this->p1Index-1];
    Vector3f b = pScene->vertices[this->p2Index-1];
    Vector3f c = pScene->vertices[this->p3Index-1];

    float m = (((a.x-b.x) * ((a.y-c.y)*(ray.direction.z) - (ray.direction.y)*(a.z-c.z)))
			+ ((a.y-b.y) * ((a.z-c.z)*(ray.direction.x) - (ray.direction.z)*(a.x-c.x)))
			+ ((a.z-b.z) * ((a.x-c.x)*(ray.direction.y) - (ray.direction.x)*(a.y-c.y))));
    if (m == 0.00){
        ret.check =false;
        return ret;
    }

    t = ((a.x-ray.origin.x) * ((a.y-b.y)*(a.z-c.z) - (a.y-c.y)*(a.z-b.z))
			+ (a.y-ray.origin.y) * ((a.x-c.x)*(a.z-b.z) - (a.x-b.x)*(a.z-c.z))
			+ (a.z-ray.origin.z) * ((a.x-b.x)*(a.y-c.y) - (a.y-b.y)*(a.x-c.x)))/m;

    beta = ((a.z-c.z) * (ray.direction.x*(a.y-ray.origin.y) - (a.x-ray.origin.x)*ray.direction.y)
			+ (a.y-c.y) * (ray.direction.z*(a.x-ray.origin.x) - (a.z-ray.origin.z)*ray.direction.x)
			+ (a.x-c.x) * (ray.direction.y*(a.z-ray.origin.z) - (a.y-ray.origin.y)*ray.direction.z))/m;

    gama = ((a.x-b.x) * (ray.direction.z*(a.y-ray.origin.y) - (a.z-ray.origin.z)*ray.direction.y)
			+ (a.y-b.y) * (ray.direction.x*(a.z-ray.origin.z) - (a.x-ray.origin.x)*ray.direction.z)
			+ (a.z-b.z) * (ray.direction.y*(a.x-ray.origin.x) - (a.y-ray.origin.y)*ray.direction.x))/m;

    alfa = 1- gama-beta;


    if (0 > beta || beta > 1){
        ret.check =false;
        return ret;
    }
    if(t <= 0.00){
        ret.check =false;
        return ret;
    }
    if (0 > gama || gama > 1){
        ret.check =false;
        return ret;
    }
    if (0 > alfa || alfa > 1){
        ret.check =false;
        return ret;
    }

    ret.check =true;
    ret.t = t;
    ret.coordinate = ray.getPoint(t);
    Vector3f normal;
    normal.x = ((b.y-a.y)*(c.z-a.z) - (b.z-a.z)*(c.y-a.y));
    normal.y = ((b.z-a.z)*(c.x-a.x) - (b.x-a.x)*(c.z-a.z));
    normal.z = ((b.x-a.x)*(c.y-a.y) - (b.y-a.y)*(c.x-a.x));
    ret.normal.x = (normal.x)/(sqrt(pow(normal.x, 2) + pow(normal.y, 2) + pow(normal.z, 2)));
    ret.normal.y = (normal.y)/(sqrt(pow(normal.x, 2) + pow(normal.y, 2) + pow(normal.z, 2)));
    ret.normal.z = (normal.z)/(sqrt(pow(normal.x, 2) + pow(normal.y, 2) + pow(normal.z, 2)));

    return ret;

}

Mesh::Mesh()
{}

/* Constructor for mesh. You will implement this. */
Mesh::Mesh(int id, int matIndex, const vector<Triangle>& faces)
    : Shape(id, matIndex)
{
	/***********************************************
     *                                             *
	 * TODO: Implement this function               *
     *                                             *
     ***********************************************
	 */
    this->faces = faces;
}

/* Mesh-ray intersection routine. You will implement this.
Note that ReturnVal structure should hold the information related to the intersection point, e.g., coordinate of that point, normal at that point etc.
You should to declare the variables in ReturnVal structure you think you will need. It is in defs.h file. */
ReturnVal Mesh::intersect(const Ray & ray) const
{
	/***********************************************
     *                                             *
	 * TODO: Implement this function               *
     *                                             *
     ***********************************************
	 */
    ReturnVal actual;
    actual.t = INT8_MAX;
    for(int i=0; i<this->faces.size(); i++){
        ReturnVal ret;
        ret = faces[i].intersect(ray);
        if(ret.check == true){

             if(actual.t > ret.t)
                actual = ret;
         }
    }
    return actual;
}
