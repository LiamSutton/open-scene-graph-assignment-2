#include "TrafficLightControl.h"

int change; // used to swap between ascending and descending traffic light flows
TrafficLightControl::TrafficLightControl(osg::Node* pPart, osg::Vec3 vTrans, float fRot, float fScale) : raaNodeCallbackFacarde(pPart, vTrans, fRot, fScale)
{
	timeCount = 0;
	change = 1;
}

TrafficLightControl::~TrafficLightControl()
{
	
}

void TrafficLightControl::operator() (osg::Node* node, osg::NodeVisitor* nv)
{
	if (timeCount == 250)
	{
		int noOfLightsUpdate = m_lTrafficLights.size();
		for (trafficLightList::iterator it = m_lTrafficLights.begin(); it != m_lTrafficLights.end(); it++)
		{
			changeTrafficLight(*it); 
			if ((*it)->m_iTrafficLightStatus == 1)
			{
				noOfLightsUpdate--;
			}
		}

		timeCount = 0;
	}
	timeCount++;
}

void TrafficLightControl::changeTrafficLight(TrafficLightFacarde* pTrafficLight)
{
	int change = pTrafficLight->m_bAscending ? 1 : -1;
	pTrafficLight->m_iTrafficLightStatus+=change; // alter the status by the current direction. IE: ASC = 1, 2, 3 and DESC = 2, 1
	
	if (pTrafficLight->m_iTrafficLightStatus > 3) // out of bounds of the status, we know we will now be descending therefore alter the change value and set it to amber
	{
		pTrafficLight->m_bAscending = false;
		pTrafficLight->m_iTrafficLightStatus = 2;
	}
	if (pTrafficLight->m_iTrafficLightStatus < 1) { // out of bounds of the status, we know we will now be ascending therefore alter the change value and set it to amber
		pTrafficLight->m_bAscending = true;
		pTrafficLight->m_iTrafficLightStatus = 2;
	}

	if (pTrafficLight->m_iTrafficLightStatus == 1) // set red
	{
		pTrafficLight->setRedTrafficLight();
	}
	if (pTrafficLight->m_iTrafficLightStatus == 2) // set amber
	{
		pTrafficLight->setAmberTrafficLight();
	}
	if (pTrafficLight->m_iTrafficLightStatus == 3) // set green
	{
		pTrafficLight->setGreenTrafficLight();
	}
}

void TrafficLightControl::addTrafficLight(TrafficLightFacarde* pTrafficLight)
{
	m_lTrafficLights.push_back(pTrafficLight);
}

