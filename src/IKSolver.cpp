/*
 * IKSolver.cpp
 *
 *  Created on: 29 Jan 2014
 *      Author: m04701
 */

#include "IKSolver.h"

IKSolver::IKSolver() {
	num_joints = 0;
	need_extra_joints = true;
}

void IKSolver::start_chain() {
	chain = KDL::Chain();
	current_joints = KDL::JntArray();
	num_joints = 0;
	need_extra_joints = true;
}
void IKSolver::add_bone_to_chain(const float3& offset, const float4& rot) {
	//Create a 3DOF joint
	KDL::Joint kdl_jointx(KDL::Joint::RotZ);
	KDL::Joint kdl_jointy(KDL::Joint::RotY);
	KDL::Joint kdl_jointz(KDL::Joint::RotX);

	//Set bone length
	KDL::Vector kdl_offset(offset.x, offset.y, offset.z);

	//In KDL a 3DOF joint are two 1DOF joints without length
	//and a third one with with the bone length
	KDL::Segment segmentx(kdl_jointx);
	KDL::Segment segmenty(kdl_jointy);
	KDL::Segment segmentz(kdl_jointz, KDL::Frame(kdl_offset));

	//Put the segment in the chain
	chain.addSegment(segmentx);
	chain.addSegment(segmenty);
	chain.addSegment(segmentz);

	int index = current_joints.rows();
	current_joints.resize(current_joints.rows() + 3);

	KDL::Rotation rot_i = KDL::Rotation::Quaternion(rot.x, rot.y, rot.z, rot.w);
	rot_i.GetEulerZYX(current_joints(index), current_joints(index + 1),
			current_joints(index + 2));

	//For our user only one segment was added
	num_joints++;
}

bool IKSolver::solve_chain(const float3& goal_position, unsigned int max_ite,
		float accuracy) {
	if (need_extra_joints) {
		//To be able to solve without aiming we introduce a new virtual joint
		//to cancel the rotations introduced by the chain
		KDL::Segment extra_joint1(KDL::Joint(KDL::Joint::RotZ));
		KDL::Segment extra_joint2(KDL::Joint(KDL::Joint::RotY));
		KDL::Segment extra_joint3(KDL::Joint(KDL::Joint::RotX));

		chain.addSegment(extra_joint1);
		chain.addSegment(extra_joint2);
		chain.addSegment(extra_joint3);

		current_joints.resize(current_joints.rows() + 3);
		need_extra_joints = false;
	}

	//Forward position solver
	KDL::ChainFkSolverPos_recursive fksolver1(chain);

	//Inverse velocity solver
	KDL::ChainIkSolverVel_pinv iksolver1v(chain);

	//Inverse position solver with velocity
	KDL::ChainIkSolverPos_NR iksolver1(chain, fksolver1, iksolver1v, max_ite,
			accuracy);

	//Creation of result array
	solved_joints = KDL::JntArray(chain.getNrOfJoints());

	//Destination frame has identity matrix for rotation
	// and goal position for translation
	KDL::Frame F_dest(
			KDL::Vector(goal_position.x, goal_position.y, goal_position.z));

	//Call the solver
	int exit_flag = iksolver1.CartToJnt(current_joints, F_dest, solved_joints);

	//On success update the current position, this helps the solver to converge
	//faster and produces fluid movements when moving bones manually
	if (exit_flag >= 0) {
		current_joints = solved_joints;
	}
	return exit_flag >= 0;
}

unsigned int IKSolver::get_num_joints() const {
	return num_joints;
}

void IKSolver::get_rotation_joint(unsigned int index, float4& rot) {
	double x, y, z, w;

	//Since we have three joints in the chain for every one the user added
	index = 3 * index;

	//Rotations angles in every axes
	double angle_z = solved_joints(index);
	double angle_y = solved_joints(index + 1);
	double angle_x = solved_joints(index + 2);

	//The total rotation is the product of all the single axes rotations
	(chain.getSegment(index).getJoint().pose(angle_z).M
			* chain.getSegment(index + 1).getJoint().pose(angle_y).M
			* chain.getSegment(index + 2).getJoint().pose(angle_x).M).GetQuaternion(
			x, y, z, w);
	rot.x = x;
	rot.y = y;
	rot.z = z;
	rot.w = w;
}
