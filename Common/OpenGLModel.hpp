#ifndef OPENGLMODEL_H
#define OPENGLMODEL_H

#include <GL\GL.h>
#include <GL\glew.h>

#include <assimp\Importer.hpp>      // C++ importer interface
#include <assimp\scene.h>           // Output data structure
#include <assimp\postprocess.h>     // Post processing flags

#include <glm\glm.hpp>

#include <string>
#include <vector>
#include <memory>
#include <iostream>

using namespace std;

class Mesh {

public:
    Mesh(const aiMesh* mesh) {
        load(mesh);
        create();
    }

    ~Mesh() {
        glDeleteBuffers(4, buffer);
        glDeleteVertexArrays(1, &vao);
    }

    // dibujar el mesh
    void draw() {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
        glBindVertexArray(0);
    };

    // inicializar el mesh
    void init(const aiMesh* mesh) {
        load(mesh);
        create();
    };

private:
    vector<glm::vec3> vertex;
    vector<glm::vec3> normal;
    vector<glm::vec2> uv;
    vector<unsigned int> indices;

    GLuint buffer[4];
    GLuint vao;

    // obtener los datos de cada mesh
    void load(const aiMesh* mesh) {

        vertex.reserve(mesh->mNumVertices);
        uv.reserve(mesh->mNumVertices);
        normal.reserve(mesh->mNumVertices);
        indices.reserve(3 * mesh->mNumFaces);

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {

            // Obtener la posicion de cada vertice
            aiVector3D pos = mesh->mVertices[i];
            vertex.push_back(glm::vec3(pos.x, pos.y, pos.z));

            // Obtener las coordenadas de textura
            if (mesh->HasTextureCoords(0)) {
                aiVector3D UVW = mesh->mTextureCoords[0][i];
                uv.push_back(glm::vec2(UVW.x, UVW.y));
            }

            // Obtener los vectores normales
            if (mesh->HasNormals()) {
                aiVector3D n = mesh->mNormals[i];
                normal.push_back(glm::vec3(n.x, n.y, n.z));
            }
        }

        // Obtener los indices 
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            indices.push_back(mesh->mFaces[i].mIndices[0]);
            indices.push_back(mesh->mFaces[i].mIndices[1]);
            indices.push_back(mesh->mFaces[i].mIndices[2]);
        }
    }

    void create() {
        // generar y activar el VAO
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // generar dos ids para los buffer
        glGenBuffers(4, buffer);

        // buffer de vertices
        glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
        glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(glm::vec3), &vertex[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(0);

        // buffer de textura 
        if (!uv.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
            glBufferData(GL_ARRAY_BUFFER, uv.size() * sizeof(glm::vec2), &uv[0], GL_STATIC_DRAW);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(1);
        }

        // buffer de normales
        if (!normal.empty()) {
            glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
            glBufferData(GL_ARRAY_BUFFER, normal.size() * sizeof(glm::vec3), &normal[0], GL_STATIC_DRAW);
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
            glEnableVertexAttribArray(2);
        }

        // buffer de indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[3]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // desactivar el VAO
        glBindVertexArray(0);
    }

};

class Model {

private:
    vector<shared_ptr<Mesh>> meshes;

    // procesar recusivamente cada nodo de la escena
    void processNode(const aiNode* node, const aiScene* scene)
    {
        // obtener los mesh de esta escena
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            shared_ptr<Mesh> mesh(new Mesh(scene->mMeshes[node->mMeshes[i]]));
            meshes.push_back(mesh);
        }

        // procesar los hijos del nodo
        for (unsigned int i = 0; i < node->mNumChildren; i++)
            this->processNode(node->mChildren[i], scene);
    }

public:
    // cargar el archivo deseado
    void init(const std::string& file_name) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(file_name, aiProcess_Triangulate);

        if (scene && scene->mRootNode)
            processNode(scene->mRootNode, scene);
        else cout << importer.GetErrorString() << endl;
    }

    // dibujar la escena completa
    void draw() {
        for (auto m : meshes) m->draw();
    }
};

#endif