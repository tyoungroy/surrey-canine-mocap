/*
 * SkeletonState.cpp
 *
 *  Created on: 11 Dec 2013
 *      Author: m04701
 */

#include "SkeletonState.h"

SkeletonState::SkeletonState() {
	offsets = new osg::Vec3Array;
	lengths = new osg::Vec3Array;
}

SkeletonState::~SkeletonState() {
	// TODO Auto-generated destructor stub
}

void SkeletonState::save_state(boost::shared_ptr<Skeleton> skeleton,
		int frame_num) {

	if (skeleton->get_num_bones() != rotations.size()) {
		init(skeleton->get_num_bones());
	}

	for (unsigned int i = 0; i < skeleton->get_num_bones(); i++) {
		Node* node = skeleton->get_node(i);
		rotations.at(i) = node->quat_arr.at(frame_num);
		offsets->at(i) = node->offset;
		lengths->at(i) = node->length;
	}
}

void SkeletonState::restore_state(boost::shared_ptr<Skeleton> skeleton,
		int frame_num) {
	for (unsigned int i = 0; i < skeleton->get_num_bones(); i++) {
		Node* node = skeleton->get_node(i);
		node->quat_arr.at(frame_num) = rotations.at(i);
		node->offset = offsets->at(i);
		node->length = lengths->at(i);
	}
}

void SkeletonState::init(unsigned int size) {
	rotations.resize(size);
	offsets->resize(size);
	lengths->resize(size);
}
