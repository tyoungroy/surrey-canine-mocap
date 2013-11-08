/*
 * MiscUtils.h
 *
 *  Created on: 9 Jul 2013
 *      Author: cm00215
 */

#ifndef MISCUTILS_H_
#define MISCUTILS_H_

#include <string>
#include <vector>
#include <fstream>
#include <stdio.h>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Group>
#include <osg/MatrixTransform>
#include <osgGA/GUIEventHandler>
#include <osgGA/TrackballManipulator>
#include <osgText/Text>
#include <osgText/Text3D>
#include <osgText/Font3D>
#include <osgText/Font>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/PolygonMode>
#include <osgViewer/CompositeViewer>
#include <osg/Geometry>
#include <osgUtil/SmoothingVisitor>
#include <osg/GraphicsContext>
#include <osg/BlendFunc>
#include <osg/LineWidth>
#include <osg/Program>

#include "boost/filesystem.hpp"
#include "boost/algorithm/string/predicate.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/shared_ptr.hpp"

#include "opencv2/opencv.hpp"

#include "CudaVec.h"

inline void get_file_names(std::string path_in,
		std::vector<std::string>* file_names) {
	boost::filesystem::path path(path_in.c_str());
	boost::filesystem::directory_iterator end_iter;
	if (boost::filesystem::exists(path)
			&& boost::filesystem::is_directory(path)) {
		for (boost::filesystem::directory_iterator dir_iter(path);
				dir_iter != end_iter; ++dir_iter) {
			if (boost::filesystem::is_regular_file(dir_iter->status())) {
				file_names->push_back(dir_iter->path().filename().string());
			}
		}
	} else {
		std::cout << "Error: Cannot find path " << path_in << std::endl;
		exit(-1);
	}
	std::sort(file_names->begin(), file_names->end());
}

inline void get_dir_names(std::string path_in,
		std::vector<std::string>* file_names) {
	boost::filesystem::path path(path_in.c_str());
	boost::filesystem::directory_iterator end_iter;
	if (boost::filesystem::exists(path)
			&& boost::filesystem::is_directory(path)) {
		for (boost::filesystem::directory_iterator dir_iter(path);
				dir_iter != end_iter; ++dir_iter) {
			if (boost::filesystem::is_directory(dir_iter->status())) {
				file_names->push_back(dir_iter->path().filename().string());
			}
		}
	} else {
		std::cout << "Error: Cannot find path " << path_in << std::endl;
		exit(-1);
	}
	std::sort(file_names->begin(), file_names->end());
}

inline osg::Matrix cvmat4x4_2_osgmat(cv::Mat input) {

//	//NB: these lines here modify the matrix because of the rotation of the coordinate
//	//system that is done when saving reconstructed geometry, but not when saving the poses:
//	input.val[1] *= -1;
//	input.val[2] *= -1;
//	input.val[4] *= -1;
//	input.val[7] *= -1;
//	input.val[8] *= -1;
//	input.val[11] *= -1;

	float val_transposed[16];
	//Transpose:
	for (int r = 0; r < 4; r++) {
		for (int c = 0; c < 4; c++) {
			//DEBUG: print it:
			//std::cout << val[r*4 + c] << "\t";
			val_transposed[c * 4 + r] = ((float*) input.data)[r * 4 + c];
		}
		//DEBUG: print it:
		//std::cout << std::endl;
	}

	return osg::Matrix(val_transposed);
}

inline osg::Matrix3 cvmat3x3_2_osgmat(cv::Mat input) {

	osg::Matrix3 result;

	//Transpose:
	for (int r = 0; r < 3; r++) {
		for (int c = 0; c < 3; c++) {
			result(r, c) = input.at<float>(c, r);
			//DEBUG: print it:
			//std::cout << result(r,c) << "\t";
		}
		//DEBUG: print it:
		//std::cout << std::endl;
	}

	//std::cout << result << std::endl;

	return result;
}

inline osg::Geode* create_axes(void) {
	osg::ref_ptr<osg::ShapeDrawable> a_x = new osg::ShapeDrawable;
	a_x->setShape(new osg::Box(osg::Vec3(0.05, 0, 0), 0.1, 0.005, 0.005));
	a_x->setColor(osg::Vec4(1, 0, 0, 1));

	osg::ref_ptr<osg::ShapeDrawable> a_y = new osg::ShapeDrawable;
	a_y->setShape(new osg::Box(osg::Vec3(0, 0.05, 0), 0.005, 0.1, 0.005));
	a_y->setColor(osg::Vec4(0, 1, 0, 1));

	osg::ref_ptr<osg::ShapeDrawable> a_z = new osg::ShapeDrawable;
	a_z->setShape(new osg::Box(osg::Vec3(0, 0, 0.05), 0.005, 0.005, 0.1));
	a_z->setColor(osg::Vec4(0, 0, 1, 1));

	osg::ref_ptr<osg::Geode> axes = new osg::Geode;
	axes->addDrawable(a_x.get());
	axes->addDrawable(a_y.get());
	axes->addDrawable(a_z.get());
	return axes.release();
}

inline osg::Group* create_3D_label(std::string text, osg::Vec3 offset,
		osg::Vec3 colour) {
	osg::ref_ptr<osg::Group> label = new osg::Group;
	osg::ref_ptr<osg::MatrixTransform> offset_xform = new osg::MatrixTransform;
	offset_xform->setMatrix(osg::Matrix::translate(offset));

	osg::ref_ptr<osg::Billboard> label_billboard = new osg::Billboard;
	label_billboard->setMode(osg::Billboard::POINT_ROT_EYE);

	osg::ref_ptr<osgText::Text3D> label_text = new osgText::Text3D;
	osg::ref_ptr<osgText::Font3D> g_font3D = osgText::readFont3DFile(
			"fonts/arial.ttf");

	osg::ref_ptr<osgText::Font3D> g_font = osgText::readFont3DFile(
			"FreeSans.ttf");
	label_text->setFont(g_font.get());
	label_text->setCharacterSize(0.05);		//5cm
	label_text->setCharacterDepth(0.0025);		//2.5mm
	label_text->setAxisAlignment(osgText::TextBase::XZ_PLANE);
	label_text->setPosition(osg::Vec3(0, 0, 0));
	label_text->setText(text);
	label_text->setColor(osg::Vec4(colour + osg::Vec3(0.1f, 0.1f, 0.1f), 1.0f));//(Make a little brighter for distinctiveness)

	label_billboard->addDrawable(label_text.get());

	offset_xform->addChild(label_billboard);
	label->addChild(offset_xform.get());

	return label.release();
}

//Use OpenCV to convert from an HSV value to and RGB value
//(useful for easily specifying a more varied spectrum of colours for visualization)
inline osg::Vec3 hsv_2_rgb(osg::Vec3 hsv_in) {
	cv::Mat hsv(1, 1, CV_32FC3, cv::Scalar(hsv_in.x(), hsv_in.y(), hsv_in.z()));
	cv::Mat rgb(1, 1, CV_32FC3);
	cv::cvtColor(hsv, rgb, CV_HSV2RGB);

//	//DEBUG:
//	cout << "HSV: (" <<
//			((float*)(hsv.data))[0] << ", " <<
//			((float*)(hsv.data))[1] << ", " <<
//			((float*)(hsv.data))[2] << ")" << endl;
//	cout << "RGB: (" <<
//			((float*)(rgb.data))[0] << ", " <<
//			((float*)(rgb.data))[1] << ", " <<
//			((float*)(rgb.data))[2] << ")" << endl;

	return osg::Vec3(((float*) (rgb.data))[0], ((float*) (rgb.data))[1],
			((float*) (rgb.data))[2]);
}

inline float3 osgvec_2_float3(osg::Vec3 vec) {
	float3 temp;
	temp.x = vec.x();
	temp.y = vec.y();
	temp.z = vec.z();
	return temp;
}

//Enable custom shaders for splat rendering of geometry
inline void enable_splat_rendering(osg::ref_ptr<osg::Geode> geometry_in) {

	//Vertex shader:
	//-------------------------
	static const char* pass_through_vertex = { "#version 120\n"
			"#extension GL_EXT_geometry_shader4 : enable\n"
			"varying out vec3 normal;\n"
			"varying out vec4 colour;\n"
			"attribute vec3 vert_attribs;\n"
			"varying out float pt_radius;\n"
			"void main(void)\n"
			"{\n"
			"gl_Position = gl_Vertex;\n"
			"normal = normalize(gl_Normal);\n"
			"colour = gl_Color;\n"
			"pt_radius = vert_attribs.x;\n"
			"}\n" };
	//-------------------------

	//Normal visualization with lines:
	//-------------------------
	/*static const char* normal_vis_geom =
	 {
	 "#version 120\n"
	 "#extension GL_EXT_geometry_shader4 : enable\n"
	 "varying in vec3 normal[];\n"
	 "void main()\n"
	 "{\n"
	 "// assert(gl_VerticesIn == 3);\n"
	 "// assert(GL_GEOMETRY_INPUT_TYPE_EXT == GL_TRIANGLES);\n"
	 "// assert(GL_GEOMETRY_OUTPUT_TYPE_EXT == GL_LINE_STRIP);\n"
	 "// assert(GL_GEOMETRY_VERTICES_OUT_EXT == 6);\n"
	 "for(int i = 0; i < gl_VerticesIn; ++i)\n"
	 "{\n"
	 //"int i = 0;\n"
	 "gl_Position = gl_ModelViewProjectionMatrix * gl_PositionIn[i];\n"
	 "gl_FrontColor = gl_FrontColorIn[i];\n"
	 "EmitVertex();\n"
	 "gl_Position = gl_ModelViewProjectionMatrix * (gl_PositionIn[i] + (vec4(normal[i], 0)) * 0.5);\n"
	 "gl_FrontColor = vec4(0,1,0,1);\n"
	 "EmitVertex();\n"
	 "EndPrimitive();\n"
	 "}"
	 "}"
	 };*/
	//-------------------------
	//Triangle strip-based circular splat oriented using normal:
	//------------------
	static const char* splat_geom =
			{ "#version 120\n"
					"#extension GL_EXT_geometry_shader4 : enable\n"
					"varying in vec3 normal[];\n"
					"varying in vec4 colour[];\n"
					"varying in float pt_radius[];\n"
					"varying out vec3 normal_geom_out;\n"
					"varying out vec4 colour_geom_out;\n"
					"uniform float pi_2;\n"
					"uniform int num_segments;\n"
					"void main()\n"
					"{\n"

					//Get orientation matrix (derived using z-axis from normal, x-axis in x=0 plane):
					"float b_y = (normal[0].z)/(sqrt(normal[0].y*normal[0].y + normal[0].z*normal[0].z));\n"
					"float b_z = -(normal[0].y*b_y)/(normal[0].z);\n"
					"vec3 b = vec3(0, b_y, b_z);\n"
					"vec3 c = cross(normal[0], b);\n"
					"mat3 R = mat3(b, c, normal[0]);\n"

					//Per-vertex property for radius
					"float r = pt_radius[0];\n"

					//Central vertex:
					"vec4 central_vert = gl_PositionIn[0];\n"
					//Boundary vertices:
					"for(int i = 0; i <= num_segments; ++i)\n"
					"{\n"
					"float theta = float(i)/num_segments * pi_2;\n"
					"float x = r*cos(theta);\n"
					"float y = r*sin(theta);\n"
					"vec4 splat_local_pos = vec4((R*vec3(x,y,0)),1);\n"
					"vec4 object_local_pos = central_vert + splat_local_pos - vec4(0,0,0,1);\n"
					"gl_Position = gl_ModelViewProjectionMatrix * object_local_pos;\n"
					"normal_geom_out = normal[0];\n"
					"colour_geom_out = colour[0];\n"
					"EmitVertex();\n"
					"gl_Position = gl_ModelViewProjectionMatrix * central_vert;\n"
					"normal_geom_out = normal[0];\n"
					"colour_geom_out = colour[0];\n"
					"EmitVertex();\n"
					"}"
					"EndPrimitive();\n"
					"}" };
	//------------------

	//Very basic diffuse lighting fragment shader:
	//------------------
	/*static const char* diffuse_frag = {
	 "varying in vec3 normal_geom_out;\n"
	 "void main()\n"
	 "{\n"
	 "float intensity = dot(normalize(vec3(100,100,100)), normal_geom_out);\n"
	 "gl_FragColor = vec4(intensity, intensity, intensity, 1);\n"
	 "}\n"
	 };*/
	//------------------
	//Colour passthrough fragment shader:
	//------------------
	/*static const char* colour_frag = {
	 "varying in vec3 normal_geom_out;\n"
	 "varying in vec4 colour_geom_out;\n"
	 "void main()\n"
	 "{\n"
	 "gl_FragColor = colour_geom_out;\n"
	 "}\n"
	 };*/
	//------------------
	//Normal colouring fragment shader:
	//------------------
	static const char* normal_colour_frag =
			{ "varying in vec3 normal_geom_out;\n"
					"void main()\n"
					"{\n"
					//"	vec3 colour = 0.5*normal_geom_out + 0.5;\n"//Local normals
					"	vec3 colour = 0.5*gl_NormalMatrix*normal_geom_out + 0.5;\n"//Global normals
					"	gl_FragColor = vec4(colour, 1);\n"
					"}\n" };
	//------------------

//	//Setup shaders (normal vis):
//	//------------------
//	osg::ref_ptr<osg::Program> norm_vis_prog = new osg::Program;
//	norm_vis_prog->addShader(
//			new osg::Shader(osg::Shader::VERTEX, pass_through_vertex) );
//	norm_vis_prog->addShader(
//			new osg::Shader(osg::Shader::GEOMETRY, normal_vis_geom) );
//	norm_vis_prog->setParameter( GL_GEOMETRY_VERTICES_OUT_EXT, 2 );
//	norm_vis_prog->setParameter( GL_GEOMETRY_INPUT_TYPE_EXT,
//			GL_POINTS );
//	//GL_TRIANGLES );
//	norm_vis_prog->setParameter( GL_GEOMETRY_OUTPUT_TYPE_EXT,
//			GL_LINE_STRIP );
//	//------------------

	//Number of segments to use for surfel approximation:
	int num_segments = 6;

	//Setup shaders:
	//------------------
	osg::ref_ptr<osg::Program> splat_prog = new osg::Program;
	splat_prog->addShader(
			new osg::Shader(osg::Shader::VERTEX, pass_through_vertex));
	splat_prog->addShader(new osg::Shader(osg::Shader::GEOMETRY, splat_geom));
	splat_prog->addShader(new osg::Shader(osg::Shader::FRAGMENT,
	//colour_frag));
			normal_colour_frag));
	splat_prog->setParameter( GL_GEOMETRY_VERTICES_OUT_EXT,
			2 * num_segments + 2);
	splat_prog->setParameter( GL_GEOMETRY_INPUT_TYPE_EXT, GL_POINTS);
	splat_prog->setParameter( GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP);
	splat_prog->addBindAttribLocation("vert_attribs", 6);
	//------------------

	//Assign shader program to geometry:
	//------------------
	osg::StateSet* stateset2 = geometry_in->getOrCreateStateSet();
	stateset2->setAttribute(splat_prog.get());
	stateset2->addUniform(new osg::Uniform("num_segments", int(num_segments)));
	stateset2->addUniform(new osg::Uniform("pi_2", float(3.14159 * 2)));
	//------------------

}

inline void surfel_ply_write(osg::ref_ptr<osg::Geode> geom_in,
		std::string file_name) {
	std::cout << "Saving geometry to PLY file..." << std::endl;

	std::ofstream out_file;
	out_file.open(file_name.c_str());

	//Header:
	//-----------------------------------

	out_file << "ply" << std::endl;
	out_file << "format ascii 1.0" << std::endl;
	out_file << "comment output geometry" << std::endl;
	out_file << "element vertex "
			<< geom_in->getDrawable(0)->asGeometry()->getVertexArray()->getNumElements()
			<< std::endl;
	out_file << "property float x" << std::endl;
	out_file << "property float y" << std::endl;
	out_file << "property float z" << std::endl;
	out_file << "property float nx" << std::endl;
	out_file << "property float ny" << std::endl;
	out_file << "property float nz" << std::endl;
	out_file << "property uchar red" << std::endl;
	out_file << "property uchar green" << std::endl;
	out_file << "property uchar blue" << std::endl;
	out_file << "property float radius" << std::endl;
	out_file << "property int timestamp" << std::endl;
	out_file << "property int label" << std::endl;
	out_file << "end_header" << std::endl;
	//-----------------------------------

	//Vertices (including normals and colours, and other attributes):
	//-----------------------------------

	osg::Vec3Array* vertex_array =
			static_cast<osg::Vec3Array*>(geom_in->getDrawable(0)->asGeometry()->getVertexArray());
	osg::Vec3Array* normal_array =
			static_cast<osg::Vec3Array*>(geom_in->getDrawable(0)->asGeometry()->getNormalArray());
	osg::Vec4Array* colour_array =
			static_cast<osg::Vec4Array*>(geom_in->getDrawable(0)->asGeometry()->getColorArray());
	osg::Vec3Array* attrib_array =
			static_cast<osg::Vec3Array*>(geom_in->getDrawable(0)->asGeometry()->getVertexAttribArray(
					6));

	for (unsigned int i = 0; i < vertex_array->getNumElements(); i++) {

		out_file << (*vertex_array)[i].x() << "\t" << (*vertex_array)[i].y()
				<< "\t" << (*vertex_array)[i].z() << "\t";

		out_file << (*normal_array)[i].x() << "\t" << (*normal_array)[i].y()
				<< "\t" << (*normal_array)[i].z() << "\t";

		out_file << int(255 * (*colour_array)[i].x()) << "\t"
				<< int(255 * (*colour_array)[i].y()) << "\t"
				<< int(255 * (*colour_array)[i].z()) << "\t";

		out_file << (*attrib_array)[i].x() << "\t"
				<< int((*attrib_array)[i].y()) << "\t"
				<< int((*attrib_array)[i].z()) << "\t";

		out_file << std::endl;

	}
	//-----------------------------------

	out_file.close();

	std::cout << "Done." << std::endl;
}

inline osg::Geode* surfel_ply_read(std::string file_name) {
	std::cout << "Reading geometry from PLY file..." << std::endl;

	std::ifstream in_file;
	in_file.open(file_name.c_str());

	//Read header:
	//-----------------------------------

	std::string line;
	char temp[1024];

	//First two lines specify format, ignore them:
	//in_file >> "ply" << std::endl;
	std::getline(in_file, line);
	std::cout << "Line: " << line << std::endl;
	//in_file << "format ascii 1.0" << std::endl;
	std::getline(in_file, line);
	std::cout << "Line: " << line << std::endl;
	//in_file << "comment output geometry" << std::endl;
	std::getline(in_file, line);

	//Number of vertices:
	std::getline(in_file, line);
	int num_elements;
	std::sscanf(line.c_str(), "%s %*s %d", temp, &num_elements);
	std::cout << "Num elements: " << num_elements << std::endl;

	while (!(line == std::string("end_header"))) {
		std::getline(in_file, line);
	}
//	in_file << "property float x" << std::endl;
//	in_file << "property float y" << std::endl;
//	in_file << "property float z" << std::endl;
//	in_file << "property float nx" << std::endl;
//	in_file << "property float ny" << std::endl;
//	in_file << "property float nz" << std::endl;
//	in_file << "property uchar red" << std::endl;
//	in_file << "property uchar green" << std::endl;
//	in_file << "property uchar blue" << std::endl;
//	in_file << "property float radius" << std::endl;
//	in_file << "property int timestamp" << std::endl;
//	in_file << "property int label" << std::endl;
//	in_file << "end_header" << std::endl;

	//TODO make more robust by actually checking the fields

	//-----------------------------------

	//Setup the geode:
	//-----------------------------------
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;

	//Allocate vertices and face indices:
	//------------------------
	osg::ref_ptr<osg::Vec3Array> vertex_array = new osg::Vec3Array(
			num_elements);
	osg::ref_ptr<osg::Vec3Array> normal_array = new osg::Vec3Array(
			num_elements);
	osg::ref_ptr<osg::Vec4Array> colour_array = new osg::Vec4Array(
			num_elements);
	osg::ref_ptr<osg::Vec3Array> attrib_array = new osg::Vec3Array(
			num_elements);
	//------------------------

	//Make geometry node:
	//------------------------
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
	geometry->setDataVariance(osg::Object::DYNAMIC);
	geometry->setVertexArray(vertex_array);
	geometry->setNormalArray(normal_array);
	geometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
	geometry->setColorArray(colour_array.get());
	geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
	//Note: use index not assigned to a built in attribute
	//(see http://www.opengl.org/sdk/docs/tutorials/ClockworkCoders/attributes.php)
	geometry->setVertexAttribArray(6, attrib_array.get());
	geometry->setVertexAttribBinding(6, osg::Geometry::BIND_PER_VERTEX);
	geometry->addPrimitiveSet(
			new osg::DrawArrays(GL_POINTS, 0, vertex_array->size()));
	//Use VBO to improve performance:
	geometry->setUseVertexBufferObjects(true);
	geometry->setUseDisplayList(false);

	geode->addDrawable(geometry);

	//------------------------

	//Enable splat rendering:
	//------------------------
	enable_splat_rendering(geode);
	//------------------------

	//-----------------------------------

	//TODO - binary file IO for more compact files and faster loading/saving.

	//Read vertices (including normals and colours, and other attributes):
	//-----------------------------------
	float temp_float;
	for (int i = 0; i < num_elements; i++) {

		in_file >> (*vertex_array)[i].x();
		in_file >> (*vertex_array)[i].y();
		in_file >> (*vertex_array)[i].z();

		in_file >> (*normal_array)[i].x();
		in_file >> (*normal_array)[i].y();
		in_file >> (*normal_array)[i].z();

		in_file >> temp_float;
		(*colour_array)[i].x() = temp_float / 255;
		in_file >> temp_float;
		(*colour_array)[i].y() = temp_float / 255;
		in_file >> temp_float;
		(*colour_array)[i].z() = temp_float / 255;

		in_file >> (*attrib_array)[i].x();
		in_file >> (*attrib_array)[i].y();
		in_file >> (*attrib_array)[i].z();

	}
	//-----------------------------------

	in_file.close();

	std::cout << "Done." << std::endl;

	return geode.release();

}

//TODO Maybe it would be better to put this method in some abstract class
//that Skeletonization classes would inherit from. Lest actually see how much
//they turn out to have in common

//Given a uchar 1 channel image, and a start points it returns true and a position
//if it finds a white pixel next to it. False otherwise.
inline bool get_neighbor_white_pixel(cv::Mat& img, int i_row, int i_col,
		int &res_row, int &res_col) {
	bool go_top, go_bot, go_left, go_right;
	//TODO Maybe it would be better to calculate this as they are needed
	//and not all together in the beggining
	go_top = i_row > 0;
	go_bot = i_col < img.rows - 1;
	go_left = i_col > 0;
	go_right = i_col < img.cols - 1;

	//Search order is
	// 1 0 2
	// 3 x 4
	// 6 5 7
	//This is done since bone merging starts from the bottom, so we want to give
	//priority to follow the path of the bone upwards. Left over right is an
	//arbitrary decision
	if (go_top && (int) img.at<uchar>(i_row - 1, i_col) == 255) {
		res_row = i_row - 1;
		res_col = i_col;
		return true;
	} else if (go_top && go_left
			&& (int) img.at<uchar>(i_row - 1, i_col - 1) == 255) {
		res_row = i_row - 1;
		res_col = i_col - 1;
		return true;
	} else if (go_top && go_right
			&& (int) img.at<uchar>(i_row - 1, i_col + 1) == 255) {
		res_row = i_row - 1;
		res_col = i_col + 1;
		return true;
	} else if (go_left && (int) img.at<uchar>(i_row, i_col - 1) == 255) {
		res_row = i_row;
		res_col = i_col - 1;
		return true;
	} else if (go_right && (int) img.at<uchar>(i_row, i_col + 1) == 255) {
		res_row = i_row;
		res_col = i_col + 1;
		return true;
	} else if (go_bot && (int) img.at<uchar>(i_row + 1, i_col) == 255) {
		res_row = i_row + 1;
		res_col = i_col;
		return true;
	} else if (go_bot && go_left
			&& (int) img.at<uchar>(i_row + 1, i_col - 1) == 255) {
		res_row = i_row + 1;
		res_col = i_col - 1;
		return true;
	} else if (go_bot && go_right
			&& (int) img.at<uchar>(i_row + 1, i_col + 1) == 255) {
		res_row = i_row + 1;
		res_col = i_col + 1;
		return true;
	}
	return false;
}

inline std::ostream& operator<<(std::ostream &out, const osg::Vec3 &vector) {
	out << "[" << vector.x() << ", " << vector.y() << ", " << vector.z() << "]";
	return out;
}

#endif /* MISCUTILS_H_ */
