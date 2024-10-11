#ifndef OBJLOADER_H
#define OBJLOADER_H

#include "objForGL.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cassert>

bool is_obj_file(const std::string& path) {
	return path.substr(path.size() - 4, 4) == ".obj";
}

size_t split_string(const std::string& line, std::vector<std::string>& tokens, char delimiter) {
	std::string token;
	std::istringstream tokenStream(line);
	while (std::getline(tokenStream, token, delimiter)) {
		tokens.push_back(token);
	}
	return tokens.size();
}

class ObjLoader
{
    // OBJ File format:
	// mtllib [filename].mtl
	// v [x] [y] [z]
	// vt [u] [v]
	// vn [x] [y] [z]
	// f [v1]/[vt1]/[vn1] [v2]/[vt2]/[vn2] [v3]/[vt3]/[vn3]
	// usemtl [material_name]
	// s [number]
	// o [object_name]
	// g [group_name]
	// [comment]
private:
	std::vector<rc::ObjMesh> _meshes;

	void _write_three_token_face_to_mesh(rc::ObjMesh& mesh, const std::vector<std::string>& tokens, std::vector<std::string>& scratch) {
		rc::objFaceIndeces v_face, vt_face, vn_face;
		for (int i = 0; i < 3; i++) {
			split_string(tokens[i], scratch, '/');
			v_face[i] = std::stoi(scratch[0]);
			vt_face[i] = std::stoi(scratch[1]);
			vn_face[i] = std::stoi(scratch[2]);
		}
		mesh.vertex_faces.push_back(v_face);
		mesh.tex_coord_faces.push_back(vt_face);
		mesh.normal_faces.push_back(vn_face);
	}
	
	void _add_faces_to_mesh(rc::ObjMesh& mesh, const std::vector<std::string>& tokens) {
		// The face format is f [v1]/[vt1]/[vn1] [v2]/[vt2]/[vn2] [v3]/[vt3]/[vn3]
		// or f [v1]/[vt1]/[vn1] [v2]/[vt2]/[vn2] [v3]/[vt3]/[vn3] [v4]/[vt4]/[vn4]
		// This creates two triangles like so:
		// 3-------2
		// |      /|
		// |    /  |
		// |  /    |
		// |/      |
		// 0-------1
		// and creates two faces [0, 1, 2] and [0, 2, 3]
		// split the line by spaces and create a objFaceIndeces from the next 3 values
		// If the first token is 'f', then disregard it
		int f_start_index = tokens[0] == "f" ? 1 : 0;
		int n_face_tokens = tokens.size() - f_start_index;
		assert(n_face_tokens == 3 or n_face_tokens == 4, "Only support for triangles and quads");
		static std::vector<std::string> v_n_t_faces;
		static std::vector<std::string> first_triangle_tokens= { tokens[f_start_index], tokens[f_start_index + 1], tokens[f_start_index + 2] };
		static std::vector<std::string> second_triangle_tokens;
		
		_write_three_token_face_to_mesh(mesh, first_triangle_tokens, v_n_t_faces);

		if(n_face_tokens == 4)
		{
			second_triangle_tokens = { tokens[f_start_index], tokens[f_start_index + 2], tokens[f_start_index + 3] };
			_write_three_token_face_to_mesh(mesh, second_triangle_tokens, v_n_t_faces);
		}
	}

	// Reads the file and adds obj meshes to the _meshes vector.
	void _build_obj_meshes(std::ifstream& file) {
		rc::ObjMesh mesh;
		bool reading_f = false;
		std::string line;
		while (std::getline(file, line)) {
			// We need the first two characters to determine what the line is.
			if (line.size() < 2) {
				continue;
			}
			if (line[0] == 'f' and line[1] == ' ') {
				reading_f = true;
				std::vector<std::string> tokens;
				split_string(line, tokens, ' ');
				_add_faces_to_mesh(mesh, tokens);
			}
			if (reading_f and line[0] != 'f') {
				// create a new mesh and continue
				_meshes.push_back(mesh);
				mesh = rc::ObjMesh();
				reading_f = false;
			}
			if (line[0] == 'v' and line[1] == ' ') {
				// Add the vertex into the mesh. 
				// Vertex format in the obj file is v 0.000000 1.000000 0.000000
				// split the line by spaces and create a vec3 from the next 3 values
				std::vector<std::string> tokens;
				split_string(line, tokens, ' ');
				mesh.vertices.push_back(rc::Vector3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])));

			}
		}
		// add the last mesh if it has any data
		if (mesh.vertices.size() > 0) {
			_meshes.push_back(mesh);
		}
	}


public:
	ObjLoader() {};
	~ObjLoader() {}; // Remove this line

    bool loadObjFile(const std::string& path) 
    {
        if (!is_obj_file(path)) {
            return false;
        }
		// add the current path to the file name
		
		std::ifstream file(path);
		if (!file.is_open()) {
			std::cerr << "Failed to open file: " << path << std::endl;
			return false;
		}
		_build_obj_meshes(file);
		return true;
    }

	std::vector<rc::ObjMesh>& getObjMeshes() { return _meshes; }
	const std::vector<rc::ObjMesh>& getObjMeshes() const { return _meshes; }
};

#endif // OBJLOADER_H