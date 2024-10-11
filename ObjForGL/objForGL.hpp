#ifndef OBJ_FOR_GL_HPP
#define OBJ_FOR_GL_HPP

#include <vector>

namespace rc
{
	struct Vector3
	{
		Vector3() { x = 0; y = 0; z = 0; };
		Vector3(float x_, float y_, float z_) { x = x_; y = y_; z = z_; };
		float x, y, z;
	};

	// struct for a mesh in OpenGL. Each vertex, normal, and position index corresponds to the 
	// same vertice in the mesh.
	struct glMesh
	{
		std::vector<Vector3> vertices;
		std::vector<Vector3> normals;
		std::vector<Vector3> tex_coords;
	};

	struct Material
	{
		Material() { name = ""; ka = Vector3(); kd = Vector3(); ks = Vector3(); ns = 0; };

		std::string name;
		// ambient
		Vector3 ka;
		// diffuse
		Vector3 kd;
		// specular
		Vector3 ks;
		// specular exponent
		float ns;
	};

	struct Vertex
	{
		Vertex() { position = Vector3(); normal = Vector3(); tex_coord = Vector3(); };
		Vertex(Vector3 position_, Vector3 normal_, Vector3 tex_coord_) { position = position_; normal = normal_; tex_coord = tex_coord_; };
		Vector3 position;
		Vector3 normal;
		Vector3 tex_coord;
	};

	// This holds the index into the array for either vertices, normals, or textures for each face.
	struct objFaceIndeces {
		int index[3];

		int& operator[](int i) {
			return index[i];
		}
	};

	// This holds the data in the obj file to be converted into a glMesh. 

	class ObjMesh
	{
	private:

	public:
		ObjMesh() { group_name = ""; };
		std::string group_name;
		std::vector<Vector3> vertices;
		std::vector<Vector3> normals;
		std::vector<Vector3> tex_coords;
		std::vector<objFaceIndeces> vertex_faces;
		std::vector<objFaceIndeces> normal_faces;
		std::vector<objFaceIndeces> tex_coord_faces;

		const int NV() const { return vertices.size(); };
		const int NF() const { return vertex_faces.size(); };
	};


}
#endif // OBJ_FOR_GL_HPP