#include "Scene.h"
#include "Camera.h"
#include "Light.h"
#include "Material.h"
#include "Shape.h"
#include "tinyxml2.h"
#include "Image.h"
#include <cmath>

using namespace tinyxml2;

/*
 * Must render the scene from each camera's viewpoint and create an image.
 * You can use the methods of the Image class to save the image as a PPM file.
 */
void Scene::renderScene(void)
{
	/***********************************************
     *                                             *
	 * TODO: Implement this function               *
     *                                             *
     ***********************************************
	 */

	for(int i=0; i < cameras.size(); i++){
		Image image = Image(cameras[i]->imgPlane.nx, cameras[i]->imgPlane.ny);
		for(int x=0; x < cameras[i]->imgPlane.nx;x++){
			for(int y=0; y < cameras[i]->imgPlane.ny; y++){
				Ray ray1 = cameras[i]->getPrimaryRay(x, y);
				Vector3f coormin;
				int obj;
				ReturnVal ret;
				check= false;
				float tmin = INT8_MAX;
				float pixel1 = 0,pixel2=0,pixel3=0;

				Vector3f dene = pixel(ray1, pixel1, pixel2, pixel3,maxRecursionDepth);
				pixel1 += dene.x;
				pixel2 += dene.y;
				pixel3 += dene.z;
				if(check == false){
					//background
					unsigned char r,g,b;
					r = pScene->backgroundColor.r;
					g = pScene->backgroundColor.g;
					b = pScene->backgroundColor.b;
					Color color = {r,g,b};
					image.setPixelValue(x,y,color);
				}
				else{
					//intersect
					unsigned char r,g,b;
					r = pixel1 > 255.00 ? 255.00: pixel1;
					g = pixel2 > 255.00 ? 255.00: pixel2;
					b = pixel3 > 255.00 ? 255.00: pixel3;
					Color color = {r,g,b};
					image.setPixelValue(x,y,color);
				}
			}
		}
		image.saveImage(cameras[i]->imageName);
	}
}

Vector3f Scene::Unit(const Vector3f &vec){
	Vector3f vec1;
	float vec_len = sqrt(pow(vec.x, 2) + pow(vec.y,2) + pow(vec.z,2));
	vec1.x = vec.x/vec_len;
	vec1.y = vec.y/vec_len;
	vec1.z = vec.z/vec_len;

	return vec1;
}

Vector3f Scene::pixel(const Ray & ray1,float pixel1, float pixel2, float pixel3,int dep){
	Vector3f coormin, wi;
	int obj;
	ReturnVal ret;
	check= false;
	float tmin = INT8_MAX;
	//float pixel1 = 0,pixel2=0,pixel3=0;

		for (int j = 0; j < objects.size(); j++){
					ReturnVal ret1 = objects[j]->intersect(ray1);
					if (ret1.check == true){
						if (tmin > ray1.gett(ret1.coordinate)){
							tmin = ray1.gett(ret1.coordinate);
							coormin = ret1.coordinate;
							check =true;
							ret = ret1;
							obj = j;
							//Ambient Shading
				 			pixel1 += pScene->materials[objects[obj]->matIndex - 1]->ambientRef.x * pScene->ambientLight.x;
				 			pixel2 += pScene->materials[objects[obj]->matIndex - 1]->ambientRef.y * pScene->ambientLight.y;
				 			pixel3 += pScene->materials[objects[obj]->matIndex - 1]->ambientRef.z * pScene->ambientLight.z;
						}
					}
				}

				if(ret.check==true){
					for(int zart=0; zart < pScene->lights.size(); zart++){
						Vector3f irradience = pScene->lights[zart]->computeLightContribution(coormin);
					 	float wi_len = (sqrt(pow(-coormin.x + pScene->lights[zart]->position.x,2) + pow(-coormin.y + pScene->lights[zart]->position.y, 2) + pow(-coormin.z + pScene->lights[zart]->position.z,2)));
					 	wi.x = (-coormin.x + pScene->lights[zart]->position.x);
						wi.y = (-coormin.y + pScene->lights[zart]->position.y);
					 	wi.z = (-coormin.z + pScene->lights[zart]->position.z);
					 	wi = Unit(wi);

						Vector3f wi_new;
					 	wi_new.x = coormin.x + wi.x*pScene->shadowRayEps;
					 	wi_new.y = coormin.y + wi.y*pScene->shadowRayEps;
						wi_new.z = coormin.z + wi.z*pScene->shadowRayEps;

						Ray s_t = Ray(wi_new,wi);

						//Shadow
					 	int flag = 0;
					 	for (int p = 0; p < objects.size(); p++){
					 		if(objects[p]->intersect(s_t).check == true){
					 			ReturnVal ret2 = objects[p]->intersect(s_t);
					 			float ret_len = sqrt(pow(ret2.coordinate.x-coormin.x,2)+pow(ret2.coordinate.y-coormin.y,2)+pow(ret2.coordinate.z-coormin.z,2));
					 		
					 			if(ret_len < wi_len){
					 				flag++;
					 			}
							}
						}
					
						if(flag==0){
							//Diffuse Shading
							float cos1 = wi.x*ret.normal.x + wi.y*ret.normal.y + wi.z*ret.normal.z;
							if(cos1 < 0)
								cos1 = 0;

							pixel1 += pScene->materials[objects[obj]->matIndex - 1]->diffuseRef.x * cos1 * irradience.x;
							pixel2 += pScene->materials[objects[obj]->matIndex - 1]->diffuseRef.y * cos1 * irradience.y;
							pixel3 += pScene->materials[objects[obj]->matIndex - 1]->diffuseRef.z * cos1 * irradience.z;

					 		//Specular Shading
							Vector3f h;
							h.x = (wi.x - Unit(ray1.direction).x);
							h.y = (wi.y - Unit(ray1.direction).y);
							h.z = (wi.z - Unit(ray1.direction).z);
							h = Unit(h);

							float cos_spec = ret.normal.x*h.x + ret.normal.y*h.y + ret.normal.z*h.z;
							if(cos_spec < 0)
								cos_spec = 0;

							pixel1 += pScene->materials[objects[obj]->matIndex - 1]->specularRef.x * pow(cos_spec, pScene->materials[objects[obj]->matIndex - 1]->phongExp) * irradience.x;
							pixel2 += pScene->materials[objects[obj]->matIndex - 1]->specularRef.y * pow(cos_spec, pScene->materials[objects[obj]->matIndex - 1]->phongExp) * irradience.y;
							pixel3 += pScene->materials[objects[obj]->matIndex - 1]->specularRef.z * pow(cos_spec, pScene->materials[objects[obj]->matIndex - 1]->phongExp) * irradience.z;
						}
				 	}
				 	
				 		//Mirror
				 		for(int i=0;i<objects.size();i++){
				 			bool mirror = (pScene->materials[objects[obj]->matIndex - 1]->mirrorRef.x)&&
				 			(pScene->materials[objects[obj]->matIndex - 1]->mirrorRef.y)&&
				 			(pScene->materials[objects[obj]->matIndex - 1]->mirrorRef.z);
				 			if(mirror && dep > 0){
				 				float n_wo = - Unit(ray1.direction).x*ret.normal.x - Unit(ray1.direction).y*ret.normal.y - Unit(ray1.direction).z*ret.normal.z;

								Vector3f wr;
								wr.x = 2*(n_wo)*ret.normal.x + Unit(ray1.direction).x; 
								wr.y = 2*(n_wo)*ret.normal.y + Unit(ray1.direction).y; 
								wr.z = 2*(n_wo)*ret.normal.z + Unit(ray1.direction).z;

								wr = Unit(wr);

								Vector3f wi_epsilon;
								wi_epsilon.x = wr.x*pScene->shadowRayEps + coormin.x;
								wi_epsilon.y = wr.y*pScene->shadowRayEps + coormin.y;
								wi_epsilon.z = wr.z*pScene->shadowRayEps + coormin.z;

								//wi_epsilon = Unit(wi_epsilon);
								
				 				Ray mirrorR =  Ray(wi_epsilon, wr);
				 				ReturnVal ret_mir = objects[i]->intersect(mirrorR);
				 				if(ret_mir.check==true && ret_mir.t > 0 && i!=obj){
					 				Vector3f p = pixel(mirrorR,pixel1,pixel2,pixel3,dep-1);
					 				pixel1 += pScene->materials[objects[obj]->matIndex - 1]->mirrorRef.x * p.x;
					 				pixel2 += pScene->materials[objects[obj]->matIndex - 1]->mirrorRef.y * p.y;
					 				pixel3 += pScene->materials[objects[obj]->matIndex - 1]->mirrorRef.z * p.z;
				 				}
				 			}
				 		}
				}
			
	
	Vector3f pix;
	pix.x=pixel1;
	pix.y=pixel2;
	pix.z = pixel3;
				
	return pix;
}


// Parses XML file.
Scene::Scene(const char *xmlPath)
{
	const char *str;
	XMLDocument xmlDoc;
	XMLError eResult;
	XMLElement *pElement;

	maxRecursionDepth = 1;
	shadowRayEps = 0.001;

	eResult = xmlDoc.LoadFile(xmlPath);

	XMLNode *pRoot = xmlDoc.FirstChild();

	pElement = pRoot->FirstChildElement("MaxRecursionDepth");
	if(pElement != nullptr)
		pElement->QueryIntText(&maxRecursionDepth);

	pElement = pRoot->FirstChildElement("BackgroundColor");
	str = pElement->GetText();
	sscanf(str, "%f %f %f", &backgroundColor.r, &backgroundColor.g, &backgroundColor.b);

	pElement = pRoot->FirstChildElement("ShadowRayEpsilon");
	if(pElement != nullptr)
		pElement->QueryFloatText(&shadowRayEps);

	pElement = pRoot->FirstChildElement("IntersectionTestEpsilon");
	if(pElement != nullptr)
		eResult = pElement->QueryFloatText(&intTestEps);

	// Parse cameras
	pElement = pRoot->FirstChildElement("Cameras");
	XMLElement *pCamera = pElement->FirstChildElement("Camera");
	XMLElement *camElement;
	while(pCamera != nullptr)
	{
        int id;
        char imageName[64];
        Vector3f pos, gaze, up;
        ImagePlane imgPlane;

		eResult = pCamera->QueryIntAttribute("id", &id);
		camElement = pCamera->FirstChildElement("Position");
		str = camElement->GetText();
		sscanf(str, "%f %f %f", &pos.x, &pos.y, &pos.z);
		camElement = pCamera->FirstChildElement("Gaze");
		str = camElement->GetText();
		sscanf(str, "%f %f %f", &gaze.x, &gaze.y, &gaze.z);
		camElement = pCamera->FirstChildElement("Up");
		str = camElement->GetText();
		sscanf(str, "%f %f %f", &up.x, &up.y, &up.z);
		camElement = pCamera->FirstChildElement("NearPlane");
		str = camElement->GetText();
		sscanf(str, "%f %f %f %f", &imgPlane.left, &imgPlane.right, &imgPlane.bottom, &imgPlane.top);
		camElement = pCamera->FirstChildElement("NearDistance");
		eResult = camElement->QueryFloatText(&imgPlane.distance);
		camElement = pCamera->FirstChildElement("ImageResolution");
		str = camElement->GetText();
		sscanf(str, "%d %d", &imgPlane.nx, &imgPlane.ny);
		camElement = pCamera->FirstChildElement("ImageName");
		str = camElement->GetText();
		strcpy(imageName, str);

		cameras.push_back(new Camera(id, imageName, pos, gaze, up, imgPlane));

		pCamera = pCamera->NextSiblingElement("Camera");
	}

	// Parse materals
	pElement = pRoot->FirstChildElement("Materials");
	XMLElement *pMaterial = pElement->FirstChildElement("Material");
	XMLElement *materialElement;
	while(pMaterial != nullptr)
	{
		materials.push_back(new Material());

		int curr = materials.size() - 1;

		eResult = pMaterial->QueryIntAttribute("id", &materials[curr]->id);
		materialElement = pMaterial->FirstChildElement("AmbientReflectance");
		str = materialElement->GetText();
		sscanf(str, "%f %f %f", &materials[curr]->ambientRef.r, &materials[curr]->ambientRef.g, &materials[curr]->ambientRef.b);
		materialElement = pMaterial->FirstChildElement("DiffuseReflectance");
		str = materialElement->GetText();
		sscanf(str, "%f %f %f", &materials[curr]->diffuseRef.r, &materials[curr]->diffuseRef.g, &materials[curr]->diffuseRef.b);
		materialElement = pMaterial->FirstChildElement("SpecularReflectance");
		str = materialElement->GetText();
		sscanf(str, "%f %f %f", &materials[curr]->specularRef.r, &materials[curr]->specularRef.g, &materials[curr]->specularRef.b);
		materialElement = pMaterial->FirstChildElement("MirrorReflectance");
		if(materialElement != nullptr)
		{
			str = materialElement->GetText();
			sscanf(str, "%f %f %f", &materials[curr]->mirrorRef.r, &materials[curr]->mirrorRef.g, &materials[curr]->mirrorRef.b);
		}
				else
		{
			materials[curr]->mirrorRef.r = 0.0;
			materials[curr]->mirrorRef.g = 0.0;
			materials[curr]->mirrorRef.b = 0.0;
		}
		materialElement = pMaterial->FirstChildElement("PhongExponent");
		if(materialElement != nullptr)
			materialElement->QueryIntText(&materials[curr]->phongExp);

		pMaterial = pMaterial->NextSiblingElement("Material");
	}

	// Parse vertex data
	pElement = pRoot->FirstChildElement("VertexData");
	int cursor = 0;
	Vector3f tmpPoint;
	str = pElement->GetText();
	while(str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
		cursor++;
	while(str[cursor] != '\0')
	{
		for(int cnt = 0 ; cnt < 3 ; cnt++)
		{
			if(cnt == 0)
				tmpPoint.x = atof(str + cursor);
			else if(cnt == 1)
				tmpPoint.y = atof(str + cursor);
			else
				tmpPoint.z = atof(str + cursor);
			while(str[cursor] != ' ' && str[cursor] != '\t' && str[cursor] != '\n')
				cursor++;
			while(str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
				cursor++;
		}
		vertices.push_back(tmpPoint);
	}

	// Parse objects
	pElement = pRoot->FirstChildElement("Objects");

	// Parse spheres
	XMLElement *pObject = pElement->FirstChildElement("Sphere");
	XMLElement *objElement;
	while(pObject != nullptr)
	{
		int id;
		int matIndex;
		int cIndex;
		float R;

		eResult = pObject->QueryIntAttribute("id", &id);
		objElement = pObject->FirstChildElement("Material");
		eResult = objElement->QueryIntText(&matIndex);
		objElement = pObject->FirstChildElement("Center");
		eResult = objElement->QueryIntText(&cIndex);
		objElement = pObject->FirstChildElement("Radius");
		eResult = objElement->QueryFloatText(&R);

		objects.push_back(new Sphere(id, matIndex, cIndex, R));

		pObject = pObject->NextSiblingElement("Sphere");
	}

	// Parse triangles
	pObject = pElement->FirstChildElement("Triangle");
	while(pObject != nullptr)
	{
		int id;
		int matIndex;
		int p1Index;
		int p2Index;
		int p3Index;

		eResult = pObject->QueryIntAttribute("id", &id);
		objElement = pObject->FirstChildElement("Material");
		eResult = objElement->QueryIntText(&matIndex);
		objElement = pObject->FirstChildElement("Indices");
		str = objElement->GetText();
		sscanf(str, "%d %d %d", &p1Index, &p2Index, &p3Index);

		objects.push_back(new Triangle(id, matIndex, p1Index, p2Index, p3Index));

		pObject = pObject->NextSiblingElement("Triangle");
	}

	// Parse meshes
	pObject = pElement->FirstChildElement("Mesh");
	while(pObject != nullptr)
	{
		int id;
		int matIndex;
		int p1Index;
		int p2Index;
		int p3Index;
		int cursor = 0;
		int vertexOffset = 0;
		vector<Triangle> faces;

		eResult = pObject->QueryIntAttribute("id", &id);
		objElement = pObject->FirstChildElement("Material");
		eResult = objElement->QueryIntText(&matIndex);
		objElement = pObject->FirstChildElement("Faces");
		objElement->QueryIntAttribute("vertexOffset", &vertexOffset);
		str = objElement->GetText();
		while(str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
			cursor++;
		while(str[cursor] != '\0')
		{
			for(int cnt = 0 ; cnt < 3 ; cnt++)
			{
				if(cnt == 0)
					p1Index = atoi(str + cursor) + vertexOffset;
				else if(cnt == 1)
					p2Index = atoi(str + cursor) + vertexOffset;
				else
					p3Index = atoi(str + cursor) + vertexOffset;
				while(str[cursor] != ' ' && str[cursor] != '\t' && str[cursor] != '\n')
					cursor++;
				while(str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
					cursor++;
			}
			faces.push_back(*(new Triangle(-1, matIndex, p1Index, p2Index, p3Index)));
		}

		objects.push_back(new Mesh(id, matIndex, faces));

		pObject = pObject->NextSiblingElement("Mesh");
	}

	// Parse lights
	int id;
	Vector3f position;
	Vector3f intensity;
	pElement = pRoot->FirstChildElement("Lights");

	XMLElement *pLight = pElement->FirstChildElement("AmbientLight");
	XMLElement *lightElement;
	str = pLight->GetText();
	sscanf(str, "%f %f %f", &ambientLight.r, &ambientLight.g, &ambientLight.b);

	pLight = pElement->FirstChildElement("PointLight");
	while(pLight != nullptr)
	{
		eResult = pLight->QueryIntAttribute("id", &id);
		lightElement = pLight->FirstChildElement("Position");
		str = lightElement->GetText();
		sscanf(str, "%f %f %f", &position.x, &position.y, &position.z);
		lightElement = pLight->FirstChildElement("Intensity");
		str = lightElement->GetText();
		sscanf(str, "%f %f %f", &intensity.r, &intensity.g, &intensity.b);

		lights.push_back(new PointLight(position, intensity));

		pLight = pLight->NextSiblingElement("PointLight");
	}
}
