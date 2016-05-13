#ifndef OPENGLMODEL_H
#define OPENGLMODEL_H

#include <GL\GL.h>
#include <GL\glew.h>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb\stb_image.h"

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <iostream>

using namespace std;

class Model {

    class Mesh {

    private:
        Model* model;

    public:
        Mesh(const aiMesh* mesh, Model* model) :
            buffer{ 0 }, vao{ 0 },
            texture_diffuse{ 0 }, 
            texture_specular{ 0 },
            texture_ambient{ 0 } {
            this->model = model;
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

        void draw(GLuint program) {
            activeTextureNum(0, texture_diffuse,  program, "material.diffuse");
            activeTextureNum(1, texture_specular, program, "material.specular");
            activeTextureNum(2, texture_ambient,  program, "material.ambient");

            //glUniform3fv(glGetUniformLocation(program, "material.ambient"), 1, color_ambient);
            //glUniform3fv(glGetUniformLocation(program, "material.diffuse"), 1, color_diffuse);
            //glUniform3fv(glGetUniformLocation(program, "material.specular"), 1, color_specular);
            //glUniform3fv(glGetUniformLocation(program, "material.emissive"), 1, color_emissive);
            glUniform1f(glGetUniformLocation(program, "material.shininess"), shininess);
            glUniform1f(glGetUniformLocation(program, "material.shininess_strength"), shininess_strength);

            glBindVertexArray(vao);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, NULL);
            glBindVertexArray(0);

            disableAllTexture();
        };

        void activeTextureNum(int num, GLuint id, GLuint program, const string& name) {
            glActiveTexture(GL_TEXTURE0 + num);
            glBindTexture(GL_TEXTURE_2D, id);
            glUniform1i(glGetUniformLocation(program, name.c_str()), num);
        }

        void disableAllTexture() {
            for (size_t i = 0; i < 8; i++) {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, 0);
            }
        }

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
        GLuint texture_ambient, texture_diffuse, texture_specular;

        float shininess, shininess_strength;
        float color_ambient[4] = { 1, 1, 1, 1 };
        float color_diffuse[4] = { 1, 1, 1, 1 };
        float color_specular[4] = { 0.0 };
        float color_emissive[4] = { 0.0 };

        // obtener los datos de cada mesh
        void load(const aiMesh* mesh) {

            vertex.reserve(mesh->mNumVertices);
            uv.reserve(mesh->mNumVertices);
            normal.reserve(mesh->mNumVertices);
            indices.reserve(3 * mesh->mNumFaces);

            for (unsigned int i = 0; i < mesh->mNumVertices; i++) {

                // Obtener la posicion de cada vertice
                const aiVector3D* pos = &(mesh->mVertices[i]);
                vertex.push_back(glm::vec3(pos->x, pos->y, pos->z));

                // Obtener las coordenadas de textura
                if (mesh->HasTextureCoords(0)) {
                    const aiVector3D* UVW = &(mesh->mTextureCoords[0][i]);
                    uv.push_back(glm::vec2(UVW->x, UVW->y));
                }
                else uv.push_back(glm::vec2(0, 0));

                // Obtener los vectores normales
                if (mesh->HasNormals()) {
                    const aiVector3D* n = &(mesh->mNormals[i]);
                    normal.push_back(glm::vec3(n->x, n->y, n->z));
                }
            }

            // Obtener los indices 
            for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
                indices.push_back(mesh->mFaces[i].mIndices[0]);
                indices.push_back(mesh->mFaces[i].mIndices[1]);
                indices.push_back(mesh->mFaces[i].mIndices[2]);
            }

            load_material(mesh, aiTextureType_AMBIENT, texture_ambient);
            load_material(mesh, aiTextureType_DIFFUSE, texture_diffuse);
            load_material(mesh, aiTextureType_SPECULAR, texture_specular);

            if (mesh->mMaterialIndex >= 0) {
                // obtener el material correspondiente a este Mesh
                const aiMaterial* material = model->scene->mMaterials[mesh->mMaterialIndex];

                if (material->Get(AI_MATKEY_SHININESS, shininess) != AI_SUCCESS) shininess = 128.0;
                if (material->Get(AI_MATKEY_SHININESS_STRENGTH, shininess_strength) != AI_SUCCESS) shininess_strength = 1.0;

                aiColor4D diffuse, ambient, specular, emisive;

                if (aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse) == AI_SUCCESS) {
                    aiColorToFloat(diffuse, color_diffuse);
                }

                if (aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular) == AI_SUCCESS) {
                    aiColorToFloat(specular, color_specular);
                }

                if (aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &ambient) == AI_SUCCESS) {
                    aiColorToFloat(ambient, color_ambient);
                }

                if (aiGetMaterialColor(material, AI_MATKEY_COLOR_EMISSIVE, &emisive) == AI_SUCCESS) {
                    aiColorToFloat(emisive, color_emissive);
                }
            }
        }

        inline void aiColorToFloat(aiColor4D& src, float dst[4]) {
            dst[0] = src.r;
            dst[1] = src.g;
            dst[2] = src.b;
            dst[3] = src.a;
        }

        void load_material(const aiMesh* mesh, aiTextureType ttype, GLuint& texture) {
            if (mesh->mMaterialIndex >= 0) {
                const aiMaterial* material = model->scene->mMaterials[mesh->mMaterialIndex];

                for (unsigned int i = 0; i < material->GetTextureCount(ttype); i++) {
                    aiString path;
                    if (AI_SUCCESS == material->GetTexture(ttype, i, &path)) {
                        const string tex_path = path.C_Str();

                        if (model->textures.count(tex_path) == 0) {
                            texture = TextureFromFile(texture_path(path.C_Str()));
                            model->textures.insert({ tex_path, texture });
                        }
                        else texture = model->textures[tex_path];
                    }
                }
            }
        }

        GLint TextureFromFile(const std::string& filename)
        {
            GLuint textureID = 0;
            glGenTextures(1, &textureID);

            int width, height, comp;
            unsigned char *image = stbi_load(filename.c_str(), &width, &height, &comp, 3);

            glBindTexture(GL_TEXTURE_2D, textureID);
            
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

            glBindTexture(GL_TEXTURE_2D, 0);

            stbi_image_free(image);

            return textureID;
        }

        std::string texture_path(const std::string& path) {
            size_t start = path.find_last_of("\\/");
            string tex_path = start == string::npos ? path : path.substr(start + 1);
            return model->directory.empty() ? tex_path : model->directory + "/" + tex_path;
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

public:
    string directory;
    const aiScene* scene;
    map<string, GLuint> textures;

private:
    vector<shared_ptr<Mesh>> meshes;

    // procesar recusivamente cada nodo de la escena
    void processNode(const aiNode* node, const aiScene* scene)
    {
        // obtener los mesh de esta escena
        for (unsigned int i = 0; i < node->mNumMeshes; i++) {
            shared_ptr<Mesh> mesh(new Mesh(scene->mMeshes[node->mMeshes[i]], this));
            meshes.push_back(mesh);
        }

        // procesar los hijos del nodo
        for (unsigned int i = 0; i < node->mNumChildren; i++)
            this->processNode(node->mChildren[i], scene);
    }

public:
    // cargar el archivo deseado
    void init(const std::string& file_name) {

        size_t index = file_name.find_last_of("\\/");
        directory = index == string::npos ? "" : file_name.substr(0, index);

        Assimp::Importer importer;
        scene = importer.ReadFile(file_name, aiProcess_Triangulate | aiProcess_FlipUVs);

        if (scene && scene->mRootNode)
            processNode(scene->mRootNode, scene);
        else cout << importer.GetErrorString() << endl;
    }

    // dibujar la escena completa
    void draw() {
        for (auto& m : meshes) m->draw();
    }

    void draw(GLuint program) {
        for (auto& m : meshes) m->draw(program);
    }
};

#endif