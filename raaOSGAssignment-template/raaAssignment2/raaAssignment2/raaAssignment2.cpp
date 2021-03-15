#include <windows.h>
#include <iostream>
#include <math.h>
#include <osgViewer/Viewer>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgViewer/ViewerEventHandlers>
#include <osg/AnimationPath>
#include <osg/Matrix>
#include <osg/Material>
#include <osg/ShapeDrawable>
#include "raaInputController.h"
#include "raaAssetLibrary.h"
#include "raaFacarde.h"
#include "raaSwitchActivator.h"
#include "raaRoadTileFacarde.h"
#include "raaAnimationPointFinder.h"
#include "raaAnimatedFacarde.h"
#include "raaCarFacarde.h"
#include "raaTrafficSystem.h"
#include "TrafficLightFacarde.h"


typedef std::vector<raaAnimationPointFinder>raaAnimationPointFinders;
osg::Group* g_pRoot = 0; // root of the sg
float g_fTileSize = 472.441f; // width/depth of the standard road tiles
std::string g_sDataPath = "../../Data/";

enum raaRoadTileType
{
	Normal,
	LitTJunction,
	LitXJunction,
};

void addRoadTile(std::string sAssetName, std::string sPartName, int xUnit, int yUnit, float fRot, osg::Group* pParent)
{
	raaFacarde* pFacarde = new raaRoadTileFacarde(raaAssetLibrary::getNamedAsset(sAssetName, sPartName), osg::Vec3(g_fTileSize * xUnit, g_fTileSize * yUnit, 0.0f), fRot);
	pParent->addChild(pFacarde->root());
}

osg::Node* buildAnimatedVehicleAsset()
{
	osg::Group* pGroup = new osg::Group();

	osg::Geode* pGB = new osg::Geode();
	osg::ShapeDrawable* pGeomB = new osg::ShapeDrawable(new osg::Box(osg::Vec3(0.0f, 0.0f, 0.0f), 100.0f, 60.0f, 40.0f));
	osg::Material* pMat = new osg::Material();
	pMat->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.3f, 0.3f, 0.1f, 1.0f));
	pMat->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.8f, 0.8f, 0.3f, 1.0f));
	pMat->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 1.0f, 0.6f, 1.0f));

	pGroup->addChild(pGB);
	pGB->addDrawable(pGeomB);

	pGB->getOrCreateStateSet()->setAttribute(pMat, osg::StateAttribute::ON || osg::StateAttribute::OVERRIDE);
	pGB->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE), osg::StateAttribute::ON || osg::StateAttribute::OVERRIDE);

	return pGroup;
}



osg::AnimationPath* createAnimationPath(raaAnimationPointFinders apfs, osg::Group* pRoadGroup)
{
	float fAnimTime = 0.0f;
	osg::AnimationPath* ap = new osg::AnimationPath();

	for (int i = 0; i < apfs.size(); i++)
	{
		float fDistance = 0.0f;
		osg::Vec3 vs;
		osg::Vec3 ve;

		vs.set(apfs[i].translation().x(), apfs[i].translation().y(), apfs[i].translation().z());

		if (i == apfs.size() - 1)
			ve.set(apfs[0].translation().x(), apfs[0].translation().y(), apfs[0].translation().z());
		else
			ve.set(apfs[i + 1].translation().x(), apfs[i + 1].translation().y(), apfs[i + 1].translation().z());

		float fXSqr = pow((ve.x() - vs.x()), 2);
		float fYSqr = pow((ve.y() - vs.y()), 2);
		float fZSqr = pow((ve.z() - vs.z()), 2);

		fDistance = sqrt(fXSqr + fYSqr);
		ap->insert(fAnimTime, osg::AnimationPath::ControlPoint(apfs[i].translation(), apfs[i].rotation()));
		fAnimTime += (fDistance / 10.0f);
	}

	return ap;
}

void buildRoad(osg::Group* pRoadGroup)
{
	addRoadTile("roadTJunction", "tile0", 0, 0, -90.0f, pRoadGroup);
	addRoadTile("roadStraight", "tile1", -1, 0, 0.0f, pRoadGroup);
	addRoadTile("roadCurve", "tile2", -2, 0, 0.0f, pRoadGroup);
	addRoadTile("roadStraight", "tile3", -2, 1, -90.0f, pRoadGroup);
	addRoadTile("roadTJunction", "tile4", -2, 2, -180.0f, pRoadGroup);
	addRoadTile("roadStraight", "tile5", -1, 2, 0.0f, pRoadGroup);
	addRoadTile("roadXJunction", "tile6", 0, 2, 0.0f, pRoadGroup);
	addRoadTile("roadStraight", "tile7", 0, 1, -90.0f, pRoadGroup);
	addRoadTile("roadStraight", "tile8", -2, 3, -90.0f, pRoadGroup);
	addRoadTile("roadCurve", "tile9", -2, 4, -90.0f, pRoadGroup);
	addRoadTile("roadStraight", "tile10", -1, 4, 0.0f, pRoadGroup);
	addRoadTile("roadTJunction", "tile11", 0, 4, 90.0f, pRoadGroup);
	addRoadTile("roadStraight", "tile12", 0, 3, -90.0f, pRoadGroup);
	addRoadTile("roadStraight", "tile13", 1, 4, 0.0f, pRoadGroup);
	addRoadTile("roadCurve", "tile14", 2, 4, 180.0f, pRoadGroup);
	addRoadTile("roadStraight", "tile15", 2, 3, -90.0f, pRoadGroup);
	addRoadTile("roadTJunction", "tile16", 2, 2, 0.0f, pRoadGroup);
	addRoadTile("roadStraight", "tile17", 1, 2, 0.0f, pRoadGroup);
	addRoadTile("roadStraight", "tile18", 2, 1, -90.0f, pRoadGroup);
	addRoadTile("roadCurve", "tile19", 2, 0, 90.0f, pRoadGroup);
	addRoadTile("roadStraight", "tile20", 1, 0, 0.0f, pRoadGroup);
}

void createCarOne(osg::Group* pRoadGroup)
{
	raaAnimationPointFinders apfs;
	osg::AnimationPath* ap = new osg::AnimationPath();

	apfs.push_back(raaAnimationPointFinder("tile0", 4, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile1", 1, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile1", 3, pRoadGroup));
	
	apfs.push_back(raaAnimationPointFinder("tile2", 3, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile2", 4, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile2", 5, pRoadGroup));
	
	apfs.push_back(raaAnimationPointFinder("tile3", 1, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile3", 3, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile4", 5, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile4", 6, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile4", 7, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile5", 2, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile5", 0, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile6", 0, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile6", 1, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile6", 2, pRoadGroup));
	
	apfs.push_back(raaAnimationPointFinder("tile12", 1, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile12", 3, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile11", 0, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile11", 3, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile11", 4, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile13", 2, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile13", 0, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile14", 3, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile14", 4, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile14", 5, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile15", 2, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile15", 0, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile16", 5, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile16", 6, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile16", 7, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile17", 1, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile17", 3, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile6", 11, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile6", 12, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile6", 4, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile7", 2, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile7", 0, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile0", 0, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile0", 3, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile0", 4, pRoadGroup));


	ap = createAnimationPath(apfs, pRoadGroup);
	// NOTE: you will need to extend or develop the car facarde to manage the animmation speed and events
	raaCarFacarde* pCar = new raaCarFacarde(g_pRoot, raaAssetLibrary::getNamedAsset("vehicle", "car0"), ap, 50.0);
	g_pRoot->addChild(pCar->root());
}

void createCarTwo(osg::Group* pRoadGroup) {
	raaAnimationPointFinders apfs;
	osg::AnimationPath* ap = new osg::AnimationPath();

	apfs.push_back(raaAnimationPointFinder("tile20", 2, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile20", 0, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile19", 0, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile19", 1, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile19", 2, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile18", 1, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile18", 3, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile16", 8, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile16", 2, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile15", 1, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile15", 3, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile14", 0, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile14", 1, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile14", 2, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile13", 1, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile13", 3, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile11", 8, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile11", 9, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile11", 7, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile12", 2, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile12", 0, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile6", 5, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile6", 6, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile6", 7, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile5", 1, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile5", 3, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile4", 0, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile4", 1, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile4", 2, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile3", 2, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile3", 0, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile2", 0, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile2", 1, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile2", 2, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile1", 2, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile1", 0, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile0", 8, pRoadGroup));
	apfs.push_back(raaAnimationPointFinder("tile0", 2, pRoadGroup));

	apfs.push_back(raaAnimationPointFinder("tile20", 2, pRoadGroup));
	ap = createAnimationPath(apfs, pRoadGroup);

	raaCarFacarde* pCar = new raaCarFacarde(g_pRoot, raaAssetLibrary::getNamedAsset("vehicle", "car1"), ap, 50.0);
	g_pRoot->addChild(pCar->root());
}

void createTrafficLights(osg::Group* pTrafficLightGroup) {
	TrafficLightFacarde* tlFacarde1 =
		new TrafficLightFacarde(raaAssetLibrary::getClonedAsset("trafficLight", "trafficLight0"),
			osg::Vec3(-175.0f, 1125.0f, 0.0f), -90.0f, 0.08f);
	pTrafficLightGroup->addChild(tlFacarde1->root());

	TrafficLightFacarde* tlFacarde2 =
		new TrafficLightFacarde(raaAssetLibrary::getClonedAsset("trafficLight", "trafficLight1"),
			osg::Vec3(200.0f, 750.0f, 0.0f), 90.0f, 0.08f);

	pTrafficLightGroup->addChild(tlFacarde2->root());

}
int main(int argc, char** argv)
{
	raaAssetLibrary::start();
	raaTrafficSystem::start();

	osgViewer::Viewer viewer;

	for (int i = 0; i < argc; i++)
	{
		if (std::string(argv[i]) == "-d") g_sDataPath = argv[++i];
	}

	// the root of the scene - use for rendering
	g_pRoot = new osg::Group();
	g_pRoot->ref();

	// build asset library - instances or clones of parts can be created from this
	raaAssetLibrary::loadAsset("roadStraight", g_sDataPath + "roadStraight.osgb");
	raaAssetLibrary::loadAsset("roadCurve", g_sDataPath + "roadCurve.osgb");
	raaAssetLibrary::loadAsset("roadTJunction", g_sDataPath + "roadTJunction.osgb");
	raaAssetLibrary::loadAsset("roadXJunction", g_sDataPath + "roadXJunction.osgb");
	raaAssetLibrary::loadAsset("trafficLight", g_sDataPath + "raaTrafficLight.osgb");
	raaAssetLibrary::insertAsset("vehicle", buildAnimatedVehicleAsset());

	// add a group node to the scene to hold the road sub-tree
	osg::Group* pRoadGroup = new osg::Group();
	g_pRoot->addChild(pRoadGroup);

	// Create road
	buildRoad(pRoadGroup);

	// Add car one
	createCarOne(pRoadGroup);

	// Add car two
	createCarTwo(pRoadGroup);

	// Traffic Lights
	osg::Group* trafficLightGroup = new osg::Group();
	g_pRoot->addChild(trafficLightGroup);
	createTrafficLights(trafficLightGroup);


	// osg setup stuff
	osg::GraphicsContext::Traits* pTraits = new osg::GraphicsContext::Traits();
	pTraits->x = 20;
	pTraits->y = 20;
	pTraits->width = 600;
	pTraits->height = 480;
	pTraits->windowDecoration = true;
	pTraits->doubleBuffer = true;
	pTraits->sharedContext = 0;

	osg::GraphicsContext* pGC = osg::GraphicsContext::createGraphicsContext(pTraits);
	osgGA::KeySwitchMatrixManipulator* pKeyswitchManipulator = new osgGA::KeySwitchMatrixManipulator();
	pKeyswitchManipulator->addMatrixManipulator('1', "Trackball", new osgGA::TrackballManipulator());
	pKeyswitchManipulator->addMatrixManipulator('2', "Flight", new osgGA::FlightManipulator());
	pKeyswitchManipulator->addMatrixManipulator('3', "Drive", new osgGA::DriveManipulator());
	viewer.setCameraManipulator(pKeyswitchManipulator);
	osg::Camera* pCamera = viewer.getCamera();
	pCamera->setGraphicsContext(pGC);
	pCamera->setViewport(new osg::Viewport(0, 0, pTraits->width, pTraits->height));

	// add own event handler - this currently switches on an off the animation points
	viewer.addEventHandler(new raaInputController(g_pRoot));

	// add the state manipulator
	viewer.addEventHandler(new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()));

	// add the thread model handler
	viewer.addEventHandler(new osgViewer::ThreadingHandler);

	// add the window size toggle handler
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);

	// add the stats handler
	viewer.addEventHandler(new osgViewer::StatsHandler);

	// add the record camera path handler
	viewer.addEventHandler(new osgViewer::RecordCameraPathHandler);

	// add the LOD Scale handler
	viewer.addEventHandler(new osgViewer::LODScaleHandler);

	// add the screen capture handler
	viewer.addEventHandler(new osgViewer::ScreenCaptureHandler);

	// set the scene to render
	viewer.setSceneData(g_pRoot);

	viewer.realize();

	return viewer.run();
}


