#ifndef DEPTHMAPSURFEL_H_
#define DEPTHMAPSURFEL_H_

#include <string>
#include <vector>
#include <fstream>

#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osgGA/GUIEventHandler>
#include <osgText/Text>
#include <osgText/Font>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/PolygonMode>
#include <osgViewer/CompositeViewer>
#include <osg/Geometry>

#include <opencv2/opencv.hpp>
#include "opencv2/gpu/gpu.hpp"

#include "../Misc/SimpleTimer.h"

#include "../Misc/CudaVec.h"

//For surfelization of depth map

#include "../Misc/MiscUtils.h"

class DepthMapSurfel {
public:
	DepthMapSurfel();
	void init(int _width, int _height, osg::Vec3 _vis_colour);
	virtual ~DepthMapSurfel();
	void surfelise_depth_map(const cv::Mat* depth_map, const cv::Mat* rgb_image,
			const float3x3 K, const float3x3 inv_K, osg::Vec3 vis_colour,
			bool with_colour, float alpha);
	//Geode:
	osg::ref_ptr<osg::Geode> depth_surfel_geode;

private:
	osg::Geode* create_depth_map_surfel_geode();

	int rows;
	int cols;

	osg::Vec4 vis_colour;

	osg::ref_ptr<osg::Geometry> dm_geometry;
	osg::ref_ptr<osg::Vec3Array> dm_vertices;
	osg::ref_ptr<osg::Vec3Array> dm_normals;
	osg::ref_ptr<osg::Vec4Array> dm_colours;
	osg::ref_ptr<osg::Vec3Array> dm_attributes;

	const static double inv_sq_2;

};

#endif /* DEPTHMAPSURFEL_H_ */
