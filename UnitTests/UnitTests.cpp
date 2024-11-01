#include "pch.h"
#include "CppUnitTest.h"
#include "../ObjForGL/objLoader.hpp"
#include "../ObjForGl/objForGL.hpp"
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

std::string cur_folder = std::filesystem::path(__FILE__).parent_path().string() + "\\";
std::string yoda_path = cur_folder + "yoda\\yoda.obj";
std::string two_group_test = cur_folder + "TestObjs\\two_group_test.obj";

namespace UnitTests
{
	TEST_CLASS(TestObjLoader)
	{
	public:

		TEST_METHOD(TESTLoadObjFile)
		{
			ObjLoader obj_loader;
			Assert::IsTrue(obj_loader.loadObjFile(two_group_test));
		}

		TEST_METHOD(TESTStringSplit)
		{
			std::string line = "v 0.000000 1.000000 0.000000";
			std::vector<std::string> expected = { "v", "0.000000", "1.000000", "0.000000" };
			std::vector<std::string> actual;
			split_string(line, actual, ' ');
			for (int i = 0; i < expected.size(); i++) {
				Assert::AreEqual(expected[i], actual[i]);
			}
		}
	};

	TEST_CLASS(TestObjForGL)
	{
	public:
		TEST_METHOD(TESTMeshIsReturnedForEachGroup)
		{
			// Test that a mesh is returned for each group in the obj file.
			ObjLoader obj_loader;
			obj_loader.loadObjFile(two_group_test);
			const std::vector<rc::ObjMesh>& meshes = obj_loader.getObjMeshes();
			Assert::AreEqual(2, (int)meshes.size());
		}

		TEST_METHOD(TESTMeshesHaveCorrectNumberOfVerticesAndFaces)
		{
			ObjLoader obj_loader;
			obj_loader.loadObjFile(two_group_test);
			int expected_g1_vertices = 7;
			int expected_g1_faces = 3;
			int expected_g2_vertices = 11;
			int expected_g2_faces = 5;
			
			const std::vector<rc::ObjMesh>& meshes = obj_loader.getObjMeshes();
			Assert::AreEqual(expected_g1_vertices, (int)meshes[0].NV());
			Assert::AreEqual(expected_g1_faces, (int)meshes[0].NF());
			Assert::AreEqual(expected_g2_vertices, (int)meshes[1].NV());
			Assert::AreEqual(expected_g2_faces, (int)meshes[1].NF());
		}
		TEST_METHOD(TESTYodaNVertices)
		{
			ObjLoader obj_loader;
			obj_loader.loadObjFile(yoda_path);

			const int expected_hair_vertices = 15840;
			const std::vector<rc::ObjMesh>& meshes = obj_loader.getObjMeshes();
			Assert::AreEqual(expected_hair_vertices, (int)meshes[0].NV());
		}
		// faces come as v/vt/vn or v//vn or v/vt or v
		// we will be given a line of three or four v/vt/vn or v//vn or v/vt or v
		TEST_METHOD(TESTReadThreeVTNFaces)
		{
			// Get the v/vt/vn from f 1/2/3 101/102/103 1001/1002/1003
			// and see that we have a vertex face of objFaceIndeces(1, 101, 1001)
			// vertex_texture face of objFaceIndeces(2, 101, 1002) etc
			rc::ObjFaceIndeces expected_vertex_face = { 1, 101, 1001 };
			rc::ObjFaceIndeces expected_texture_face = { 2, 102, 1002 };
			rc::ObjFaceIndeces expected_normal_face = { 3, 103, 1003 };
			std::string line = "f 1/2/3 101/102/103 1001/1002/1003";
			rc::ObjMesh mesh;
			ObjLoader obj_loader;
			obj_loader.add_face_indexes(mesh, line);
			for (int i = 0; i < 3; i++) {
				Assert::AreEqual(expected_vertex_face[i], mesh.vertex_faces[0][i]);
				Assert::AreEqual(expected_texture_face[i], mesh.tex_coord_faces[0][i]);
				Assert::AreEqual(expected_normal_face[i], mesh.normal_faces[0][i]);
			}
		}
		TEST_METHOD(TESTReadFourVTNFaces)
		{
			// Given a quad
			// Get the v/vt/vn from f 1/2/3 101/102/103 1001/1002/1003 10001/10002/10003
			// and see that we have a vertex face of objFaceIndeces(1, 101, 1001, 10001)
			rc::ObjFaceIndeces expected_vf_a = { 1, 101, 1001 };
			rc::ObjFaceIndeces expected_vf_b = { 1, 1001, 10001 };
			std::string line = "f 1/2/3 101/102/103 1001/1002/1003 10001/10002/10003";
			rc::ObjMesh mesh;
			ObjLoader obj_loader;
			obj_loader.add_face_indexes(mesh, line);
			for (int i = 0; i < 3; i++) {
				Assert::AreEqual(expected_vf_a[i], mesh.vertex_faces[0][i]);
				Assert::AreEqual(expected_vf_b[i], mesh.vertex_faces[1][i]);
			}

		}
		TEST_METHOD(TESTReadVNFaces)
		{
			// Given a face with no texture coordinates
		}
		TEST_METHOD(TESTReadVTFaces)
		{
			// Given a face with no normals
		}
		TEST_METHOD(TESTReadVFaces)
		{
			// Given a face with no normals or texture coordinates
		}

		TEST_METHOD(TESTGLMeshRuns)
		{
			rc::GLMesh glMesh;

			ObjLoader obj_loader;
			obj_loader.loadObjFile(two_group_test);

			const std::vector<rc::ObjMesh>& meshes = obj_loader.getObjMeshes();
			// Convert the obj mesh to a gl mesh
			glMesh.transformObjToGL(meshes[0]);
			Assert::IsTrue(glMesh.NV() > 0);
			Assert::AreEqual(glMesh.NV(), glMesh.NN());
			Assert::AreEqual(glMesh.NV(), glMesh.NT());

		}
	};
}
