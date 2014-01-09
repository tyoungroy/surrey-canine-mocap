/*
 * SkeletonFitting.h
 *
 *  Created on: 30 Oct 2013
 *      Author: m04701
 */

#ifndef SKELETONCONTROLLER_H_
#define SKELETONCONTROLLER_H_

#include "Skeleton.h"
#include "RenderSkeletonization.h"
#include "Skeletonization3D.h"
#include "SkeletonFitting.h"
#include "SkeletonMixer.h"
#include "MessageHandler.h"
#include "SkeletonState.h"

#include <osgUtil/LineSegmentIntersector>
#include <osg/MatrixTransform>

#include <iostream>
using std::cout;
using std::endl;

enum Fitting_State {
	EMPTY, ADD_POINTS, MOVE_POINTS, POINTS_SET
};

class SkeletonController {
	public:
		SkeletonController();

		virtual ~SkeletonController();

		//Set root node for this class, it should be call after creation
		void set_data(camVecT camera_arr,
				osg::ref_ptr<osg::Group> render_skel_group);

		Fitting_State getState() const;
		void setState(Fitting_State state);

		//Handle mouse events, to set up fitting points
		bool handle(const osgGA::GUIEventAdapter& ea,
				osgGA::GUIActionAdapter& aa);

		void update_dynamics(int disp_frame_no);
	private:
		enum Mod_State {
			ROTATE, TRANSLATE, INV_KIN
		};

		void reset_state();
		void draw_edit_text();

		void load_skeleton_from_file(std::string file_name);
		void save_skeleton_to_file(std::string file_name);

		//Handle mouse events, to set up fitting points
		bool handle_mouse_events(const osgGA::GUIEventAdapter& ea,
				osgGA::GUIActionAdapter& aa);

		//Handle mouse events, to set up fitting points
		bool handle_keyboard_events(const osgGA::GUIEventAdapter& ea,
				osgGA::GUIActionAdapter& aa);

		osg::Vec3 get_mouse_vec(int x, int y);

		void mix_skeleton_sizes();

		void finish_bone_trans();

		//Class that creates a skeleton from a given set of frames
		boost::shared_ptr<Skeletonization3D> skeletonized3D;

		//Class that renders all skeleton related objects
		RenderSkeletonization skel_renderer;

		//Class with methods to modify(and create???) a skeleton to fit into a
		//cloud of points that represent a skeleton.
		SkeletonFitting skel_fitter;

		boost::shared_ptr<Skeleton> skeleton;

		SkeletonMixer skel_mixer;

		SkeletonState skel_state;

		Fitting_State state;

		int current_frame;

		bool is_point_selected;
		int selected_point_index;
		osg::ref_ptr<osg::MatrixTransform> selected_point;
		int last_mouse_pos_x;
		int last_mouse_pos_y;
		bool move_on_z;
		Mod_State mod_state;
		bool change_all_frames;
		bool only_root;
		bool transforming_skeleton;
		bool delete_skel;
		Axis rotate_axis;
		bool show_joint_axis;
		bool manual_mark_up;
		float rotate_scale_factor;
		float translate_scale_factor;
		float inv_kin_scale_factor;
		float swivel_angle;

		MessageHandler msg_handler;
};

#endif /* SKELETONCONTROLLER_H_ */
