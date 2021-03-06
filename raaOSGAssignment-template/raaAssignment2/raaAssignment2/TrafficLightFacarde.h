#pragma once

#include "raaFacarde.h"
#include <osg\material>
#include <osg\Geode>
#include "raaCollisionTarget.h"

class TrafficLightFacarde : public raaFacarde, public raaCollisionTarget
{
public:TrafficLightFacarde(osg::Node* pPart, osg::Vec3 vTrans, float fRot, float fScale, bool bIsHorizontal);
	  virtual ~TrafficLightFacarde();
	  virtual osg::Vec3f getWorldDetectionPoint(); // from raaCollisionTarget
	  virtual osg::Vec3f getWorldCollisionPoint(); // from raaCollisionTarget
	  void initLights(osg::Node* pPart);
	  void createMaterial(osg::Vec3f vColour, osg::Material* mat);
	  int m_iTrafficLightStatus;
	  void setRedTrafficLight();
	  void setAmberTrafficLight();
	  void setGreenTrafficLight();
	  bool m_bAscending = true;

protected:
	osg::Geode* m_pRedTrafficLight;
	osg::Geode* m_pAmberTrafficLight;
	osg::Geode* m_pGreenTrafficLight;

	osg::Material* m_pRedTrafficLightOnMaterial;
	osg::Material* m_pRedTrafficLightOffMaterial;

	osg::Material* m_pAmberTrafficLightOnMaterial;
	osg::Material* m_pAmberTrafficLightOffMaterial;

	osg::Material* m_pGreenTrafficLightOnMaterial;
	osg::Material* m_pGreenTrafficLightOffMaterial;
};

