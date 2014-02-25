/*
 * SkeletonFitting.h
 *
 *  Created on: 5 Nov 2013
 *      Author: m04701
 */

#ifndef SKELETONFITTING_H_
#define SKELETONFITTING_H_

#include "CloudClusterer.h"
#include "LegFitter.h"
#include "BodyFitter.h"

#include "osg/Array"
#include "opencv2/opencv.hpp"

#include "boost/shared_ptr.hpp"

class SkeletonFitting {
public:
	SkeletonFitting(boost::shared_ptr<Skeleton> skeleton,
			boost::shared_ptr<Skeletonization3D> skeletonization3d,
			const camVecT& camera_arr);
	virtual ~SkeletonFitting();

	void calculate_for_frame(int frame_num);

	void fit_skeleton_to_cloud();

	const std::vector<Skeleton::Skel_Leg>& getLabels() const;
private:
	boost::shared_ptr<Skeletonization3D> skeletonizator;
	boost::shared_ptr<Skeleton> skeleton;
	LegFitter leg_fitter;
	BodyFitter body_fitter;
	CloudClusterer cloud_clusterer;

	osg::ref_ptr<osg::Vec3Array> cloud;
	std::vector<Skeleton::Skel_Leg> labels;

	bool first_call;
	int current_frame;
};

#endif /* SKELETONFITTING_H_ */
