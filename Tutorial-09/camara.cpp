#include "OpenGLWindow.hpp"
#include "OpenGLShader.hpp"

#include <glm\gtc\matrix_inverse.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtx\euler_angles.hpp>
#include <glm\gtx\matrix_cross_product.hpp>

#include <string>
#include <vector>
#include <memory>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

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

class Tutorial_09 : public OpenGLWindow {

public:
    Tutorial_09() {
        mv_matrix = 0;
        view_matrix = 0;
        proj_matrix = 0;
    }

private:
    void onstart() override {
        glEnable(GL_DEPTH_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        glClearColor(0.25f, 0.80f, 0.75f, 1.0f);

        GLuint program = shader_simple.compile("shaders/phong.vertex_shader", "shaders/phong.fragment_shader");

        mv_matrix = glGetUniformLocation(program, "mv_matrix");
        view_matrix = glGetUniformLocation(program, "view_matrix");
        proj_matrix = glGetUniformLocation(program, "proj_matrix");

        model.init("model/grid.assbin");
    }

    void onrender(double time) override {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Usar el shader
        shader_simple.use();

        // obtener los parametros de visualizacion
        glm::mat4 R = glm::yawPitchRoll(glm::radians(yaw), glm::radians(pitch), 0.0f);
        cameraFront = glm::vec3(R * glm::vec4(0, 0, -1, 0));
        cameraUp = glm::vec3(R * glm::vec4(0, 1, 0, 0));

        // craer la matriz de vista
        glm::mat4 View = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // Matriz de modelo, se aplica una rotacion sobre el eje Y 
        glm::mat4 Model;
        Model = glm::scale(Model, glm::vec3(0.5f));

        // Matriz de proyeccion y visualizacion
        glm::mat4 Projection = glm::perspective(45.0f, aspect_ratio, 0.1f, 100.0f);

        //Establecer las matrices 
        glUniformMatrix4fv(mv_matrix, 1, GL_FALSE, glm::value_ptr(View * Model));
        glUniformMatrix4fv(view_matrix, 1, GL_FALSE, glm::value_ptr(View));
        glUniformMatrix4fv(proj_matrix, 1, GL_FALSE, glm::value_ptr(Projection));

        // dibujar el modelo
        model.draw();

        // desactivar el uso del shader
        shader_simple.unUse();

        // actualizar el desplazamiento y ratacion de la camara
        chechKeys();
    }

    void chechKeys() {
        if (isKeyPress(GLFW_KEY_W))
            cameraPos += cameraSpeed * cameraFront;
        if (isKeyPress(GLFW_KEY_S))
            cameraPos -= cameraSpeed * cameraFront;
        if (isKeyPress(GLFW_KEY_A))
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (isKeyPress(GLFW_KEY_D))
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }

    inline bool isKeyPress(int key) {
        return (glfwGetKey(window, key) == GLFW_PRESS);
    }

    void onkey(int key, int scancode, int action, int mods) override {

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);

        //if (key == GLFW_KEY_W)
        //    cameraPos += cameraSpeed * cameraFront;
        //if (key == GLFW_KEY_S)
        //    cameraPos -= cameraSpeed * cameraFront;
        //if (key == GLFW_KEY_A)
        //    cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        //if (key == GLFW_KEY_D)
        //    cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

        //if (key == GLFW_KEY_E)
        //    yaw += cameraSpeed / 2.0f;
        //if (key == GLFW_KEY_R)
        //    yaw -= cameraSpeed / 2.0f;

        //if (key == GLFW_KEY_Q)
        //    pitch += cameraSpeed / 2.0f;
        //if (key == GLFW_KEY_Z)
        //    pitch -= cameraSpeed / 2.0f;
    }

    void onmouse(double xpos, double ypos) override {

        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glfwSetCursorPos(window, width / 2.0, height / 2.0);

        GLfloat xoffset = ((width / 2.0) - xpos) * mouseSensitive;
        GLfloat yoffset = ((height / 2.0) - ypos) * mouseSensitive;

        yaw += xoffset;
        pitch += yoffset;
    }

    Model model;
    OpenGLShader shader_simple;
    GLuint mv_matrix, view_matrix, proj_matrix;

    // variables utilizadas para la camara 

    glm::vec3 cameraPos   = glm::vec3(0.0f, 0.5f, 3.0f);  // posicion inicial de la camara
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

    float yaw = 0, pitch = 0;

    GLfloat cameraSpeed = 0.05f;    // velocidad de movimiento
    GLfloat mouseSensitive = 0.15f; // velocidad de rotacion 
};

int main() {
    Tutorial_09 win_app;

    if (win_app.init("OpenGL Moderno - Camara", 1280, 720)) {

        // ocultar el cursor y ubicarlo en el centro de la ventana
        glfwSetInputMode(win_app.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPos(win_app.getGLFWwindow(), 1280 / 2, 720 / 2);

        win_app.info();
        win_app.run();
    }

    return 0;
}