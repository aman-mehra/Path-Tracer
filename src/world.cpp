#include "world.h"
#include <iostream>


using namespace std;
// #define max_depth 0

Color World::lightIntersection(Ray& ray){

	float max_t = FLT_MAX;
	Color intensity = Color(0);

	for(int i=0; i<lightSourceList.size(); i++){
		if (lightSourceList[i]->getSolidity() == true){
			lightSourceList[i]->intersect(ray);
			if (ray.didHit()){
				if (ray.getParameter()<max_t){
					max_t = ray.getParameter();
					intensity = lightSourceList[i]->getIntensity() ;
				}
			}
		}
	}

	return intensity;
}

float World::firstIntersection(Ray& ray)
{
	for(int i=0; i<objectList.size(); i++)
		objectList[i]->intersect(ray);

	return ray.getParameter();
}

Color World::shade_ray(Ray& ray)
{

	// Check if we light source is hit
	Color intensity = lightIntersection(ray);
	float light_t = FLT_MAX;
	float light_hit = ray.didHit();
	if (light_hit)
		light_t = ray.getParameter();

	// check if a closer object is hit
	float obj_t = firstIntersection(ray);
	if(ray.didHit() && ray.getLevel() <= max_depth){
		if (obj_t < light_t){
			// if (ray.getLevel()>1){
			// 	std::cout <<  ray.getLevel() << std::endl;
			// }
			return (ray.intersected())->shade(ray);
		}
	}

	// return hit light source
	if (light_hit)
		return intensity;

	if (ray.getLevel()>0)
		return Color(0);

	return background;
}

Color World::light_ray(const Ray& ray)
{
	float lights = (float) lightSourceList.size();
	int ind = rand()%lightSourceList.size();
	// printf("%d\n", ind);
	Vector3D l = lightSourceList[ind]->getPosition() - ray.getPosition();
	float dist = l.length();
	l.normalize();
	float cos_o = (float) lightSourceList[ind]->getcoso(-l);
	float cos_i = (float) dotProduct(l,ray.getNormal());
	Color total_intensity = Color(0);
	if(cos_o < 0 && cos_i < 0){
		return total_intensity;
	}
	int curLevel = ray.getLevel();
	Ray shadow_ray = Ray(ray.getPosition(),l,curLevel+1);
	float t = firstIntersection(shadow_ray);
	lightSourceList[ind]->intersect(shadow_ray);
	if(t == ray.getParameter()){
		return total_intensity;
	}
	else{
		total_intensity = lightSourceList[ind]->getIntensity();
		// printf("%f\n", cos_i * cos_o * (lights/(dist * dist)));
		float intense = 1.0;
		return (total_intensity * cos_i * cos_o * intense * (lights/(dist * dist)));
	}
}