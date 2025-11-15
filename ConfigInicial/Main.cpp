// Std. Includes
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include "SOIL2/SOIL2.h"
#include "stb_image.h"

// Properties
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void Animation();
void CrearSalpicaduras(glm::vec3 posicionImpacto);

// Camera
Camera camera(glm::vec3(0.0f, 5.0f, 15.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// Variables para animación del tambor
bool animTambor = false;
float rotTambor = 0.0f;

// Variables para animación de las manecillas
bool animManecilla = false;
float rotManecilla = 0.0f;

// Variables para animación de la PUERTA PRINCIPAL (01)
bool animPuerta01 = false;
float rotPuerta01 = 0.0f;
bool abrirPuerta01 = true;

// Variables para animación de las PUERTAS INTERNAS (02, 03, 04, 05)
bool animPuertasInternas = false;
float rotPuerta02 = 0.0f;
float rotPuerta03 = 0.0f;
float rotPuerta04 = 0.0f;
float rotPuerta05 = 0.0f;
bool abrirPuertasInternas = true;

// Variables para animación de CORTINAS
bool animCortinas = false;
float posCortinaDerecha = 4.8794f;
float posCortinaIzquierda = -4.909f;
bool abrirCortinas = true;

// ========== VARIABLES PARA ANIMACIÓN DE CAJONES ==========
bool animCajonGrande = false;
float posCajonGrandeZ = -2.3328f;  // Posición inicial Z
bool abrirCajonGrande = true;

bool animCajonChico = false;
float posCajonChicoZ = -2.3395f;  // Posición inicial Z
bool abrirCajonChico = true;

// Límites de apertura de cajones (EJE Z)
const float POS_INICIAL_CAJON_GRANDE_Z = -2.3328f;  // Posición cerrado
const float POS_FINAL_CAJON_GRANDE_Z = -1.9006f;    // Posición abierto
const float POS_INICIAL_CAJON_CHICO_Z = -2.3395f;   // Posición cerrado
const float POS_FINAL_CAJON_CHICO_Z = -1.9006f;     // Posición abierto

// ========== SISTEMA DE BILLBOARDS PARA AGUA ==========
bool aguaEncendida = false;

// ========== SISTEMA DE BILLBOARDS PARA VAPOR ==========
bool vaporEncendido = false;

// ========== SISTEMA DE ILUMINACIÓN ==========
bool lucesEncendidas = false;
bool solEncendido = false;

// Posiciones de los 5 focos - AJUSTA SEGÚN TUS MODELOS EN BLENDER
glm::vec3 posicionesFocos[] = {
    glm::vec3(-1.8f, 2.3f, -2.5f),    // Foco 1 - Cuarto
    glm::vec3(0.0f, 2.3f, -0.5f),     // Foco 2 - Pasillo
    glm::vec3(3.0f, 2.3f, -2.5f),     // Foco 3 - Cuarto Vacío
    glm::vec3(5.5f, 2.3f, -4.0f),     // Foco 4 - Cocina
    glm::vec3(-2.0f, 2.3f, -6.0f)     // Foco 5 - Lavandería
};

// ========== VARIABLES PARA ANIMACIÓN DE PECES ==========
bool animPeces = true;  // Animación activa por defecto
float anguloCarrusel = 0.0f;  // Ángulo de rotación del carrusel
float tiempoPeces = 0.0f;     // Tiempo para la trayectoria senoidal

// Configuración del sistema de partículas DE AGUA
const int NUM_GOTAS = 150;  // Aumentado para incluir salpicaduras
struct Particula {
    glm::vec3 posicion;
    glm::vec3 velocidad;
    float vida;
    float tamano;       // Tamaño de la gota
    bool activa;
    bool esSalpicadura; // Diferenciar gotas principales de salpicaduras
};
Particula particulas[NUM_GOTAS];

// Configuración del sistema de partículas DE VAPOR
const int NUM_VAPOR = 80;
Particula particulasVapor[NUM_VAPOR];

// Coordenadas de la boquilla del lavabo
glm::vec3 posicionBoquilla = glm::vec3(-2.741f, 1.6193f, -6.3229f);

// Coordenadas de la plancha (donde sale el vapor)
glm::vec3 posicionPlancha = glm::vec3(-5.2276f, 0.74471f, -5.2297f);

// Coordenadas Y del lavabo (donde colisionan las gotas)
float alturaLavabo = 0.87475f;  // Fondo del lavabo donde salpican

// Configuración de físicas DEL AGUA
const float GRAVEDAD = 9.8f;
const float TIEMPO_VIDA_GOTA = 3.0f;          // Más tiempo de vida
const float TIEMPO_VIDA_SALPICADURA = 0.8f;   // Salpicaduras viven menos
const float TASA_GENERACION = 0.015f;         // Más gotas por segundo

// Configuración de físicas DEL VAPOR
const float TIEMPO_VIDA_VAPOR = 4.0f;
const float TASA_GENERACION_VAPOR = 0.03f;

// Límites de apertura de puertas
const float MAX_APERTURA_PUERTA = 80.0f;  // Puerta principal abre a 80 grados
const float MIN_APERTURA_PUERTA = 0.0f;

// Límites de posición de cortinas
const float POS_INICIAL_CORTINA_DERECHA = 4.8794f;
const float POS_FINAL_CORTINA_DERECHA = 3.49625f;
const float POS_INICIAL_CORTINA_IZQUIERDA = -4.909f;
const float POS_FINAL_CORTINA_IZQUIERDA = -3.49663f;

int main()
{
    srand(static_cast<unsigned int>(time(0)));

    // Init GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Proyecto Final Orihuela y Frias", nullptr, nullptr);

    if (nullptr == window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

    // Set callbacks
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    // Viewport and OpenGL options
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_DEPTH_TEST);

    // Setup shaders
    Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
    Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");
    Shader billboardShader("Shader/billboard.vs", "Shader/billboard.frag");

    // ========== LOAD MODELS ==========
    Model primerPiso((char*)"Models/PRIMER_PISO.obj");
    Model ventanas((char*)"Models/VENTANAS.obj");
    Model pasto((char*)"Models/PASTO.obj");
    Model segundoPiso((char*)"Models/SEGUNDO_PISO.obj");
    Model puerta01((char*)"Models/PUERTA_CASA.obj");
    Model puerta02((char*)"Models/PUERTA_DORMITORIO.obj");
    Model puerta03((char*)"Models/PUERTA_CUARTO_VACIO.obj");
    Model puerta04((char*)"Models/PUERTA_COCINA.obj");
    Model puerta05((char*)"Models/PUERTA_LAVANDERIA.obj");
    Model cortinaDerecha((char*)"Models/CORTINA_DERECHA.obj");
    Model cortinaIzquierda((char*)"Models/CORTINA_IZQUIERDA.obj");
    Model cortinas((char*)"Models/CORTINAS_2DO_PISO.obj");
    Model lamparas((char*)"Models/LAMPARAS.obj");
    Model foco01((char*)"Models/FOCO_CUARTO.obj");
    Model foco02((char*)"Models/FOCO_PASILLO.obj");
    Model foco03((char*)"Models/FOCO_VACIO.obj");
    Model foco04((char*)"Models/FOCO_COCINA.obj");
    Model foco05((char*)"Models/FOCO_LAV.obj");
    Model sol((char*)"Models/SOL.obj");
    Model mesaCocina((char*)"Models/MESA_COCINA.obj");
    Model comida((char*)"Models/COMIDA.obj");
    Model estufa((char*)"Models/ESTUFA.obj");
    Model pintura((char*)"Models/PINTURA.obj");
    Model percheroPared((char*)"Models/PERCHERO_PARED.obj");
    Model lavabo((char*)"Models/LAVABO.obj");
    Model lavadora((char*)"Models/LAVADORA.obj");
    Model lavadoraTambor((char*)"Models/LAVADORA_TAMBOR.obj");
    Model burro((char*)"Models/BURRO.obj");
    Model plancha((char*)"Models/PLANCHA.obj");
    Model cama((char*)"Models/CAMA.obj");
    Model roperoAlto((char*)"Models/ROPERO_ALTO.obj");
    Model adornosPecera((char*)"Models/ADORNOS_PECERA.obj");
    Model pecera((char*)"Models/PECERA.obj");
    Model peces((char*)"Models/PECES.obj");
    Model roperoChico((char*)"Models/ROPERO_CHICO.obj");
    Model cajonsote((char*)"Models/CAJON_GRANDE.obj");
    Model cajonsitos((char*)"Models/CAJONES_CHICOS.obj");
    Model carrito((char*)"Models/CARRITO.obj");
    Model perchero((char*)"Models/PERCHERO.obj");
    Model flores((char*)"Models/FLORES.obj");
    Model mesita((char*)"Models/MESITA.obj");
    Model campana((char*)"Models/CAMPANA.obj");
    Model repisa((char*)"Models/REPISA.obj");
    Model relojRedondo((char*)"Models/RELOJ_REDONDO.obj");
    Model ropa((char*)"Models/ROPA.obj");
    Model libros((char*)"Models/LIBROS.obj");
    Model refri((char*)"Models/REFRI.obj");
    Model manecilla((char*)"Models/MANECILLAS.obj");
    Model perilla01((char*)"Models/PERILLA01.obj");
    Model perilla02((char*)"Models/PERILLA02.obj");

    // Inicializar partículas de agua
    for (int i = 0; i < NUM_GOTAS; i++)
    {
        particulas[i].activa = false;
        particulas[i].vida = 0.0f;
        particulas[i].tamano = 0.28f;
        particulas[i].esSalpicadura = false;
    }

    // Inicializar partículas de vapor
    for (int i = 0; i < NUM_VAPOR; i++)
    {
        particulasVapor[i].activa = false;
        particulasVapor[i].vida = 0.0f;
    }

    // ========== CARGAR TEXTURA DE GOTA ==========
    GLuint texturaGota;
    glGenTextures(1, &texturaGota);
    glBindTexture(GL_TEXTURE_2D, texturaGota);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int texWidth, texHeight, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("Images/GOTA.png", &texWidth, &texHeight, &nrChannels, 0);

    if (data)
    {
        GLenum format = GL_RGB;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Textura de gota cargada: " << texWidth << "x" << texHeight << " (" << nrChannels << " canales)" << std::endl;
    }
    else
    {
        std::cout << "Error al cargar textura: Images/GOTA.png" << std::endl;
    }
    stbi_image_free(data);

    // ========== CARGAR TEXTURA DE VAPOR ==========
    GLuint texturaVapor;
    glGenTextures(1, &texturaVapor);
    glBindTexture(GL_TEXTURE_2D, texturaVapor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    data = stbi_load("Images/VAPOR.png", &texWidth, &texHeight, &nrChannels, 0);

    if (data)
    {
        GLenum format = GL_RGB;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, texWidth, texHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "Textura de vapor cargada: " << texWidth << "x" << texHeight << " (" << nrChannels << " canales)" << std::endl;
    }
    else
    {
        std::cout << "Error al cargar textura: Images/VAPOR.png" << std::endl;
    }
    stbi_image_free(data);

    // ========== GEOMETRÍA DEL BILLBOARD ==========
    float verticesBillboard[] = {
        -0.05f, -0.05f,   0.0f, 0.0f,
         0.05f, -0.05f,   1.0f, 0.0f,
         0.05f,  0.05f,   1.0f, 1.0f,
        -0.05f,  0.05f,   0.0f, 1.0f
    };

    unsigned int indicesBillboard[] = {
        0, 1, 2,
        2, 3, 0
    };

    GLuint VAO_billboard, VBO_billboard, EBO_billboard;
    glGenVertexArrays(1, &VAO_billboard);
    glGenBuffers(1, &VBO_billboard);
    glGenBuffers(1, &EBO_billboard);

    glBindVertexArray(VAO_billboard);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_billboard);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesBillboard), verticesBillboard, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_billboard);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesBillboard), indicesBillboard, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Projection matrix
    glm::mat4 projection = glm::perspective(camera.GetZoom(), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Frame time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Events
        glfwPollEvents();
        DoMovement();

        // Animación
        Animation();

        // ========== FONDO HELLO KITTY - ROSA PASTEL 💖 ==========
        glClearColor(1.0f, 0.8f, 0.9f, 1.0f);  // Rosa pastel suave
        // Otras opciones:
        // glClearColor(0.8f, 0.9f, 1.0f, 1.0f);  // Celeste pastel
        // glClearColor(0.9f, 0.85f, 1.0f, 1.0f);  // Lavanda pastel
        // glClearColor(1.0f, 0.9f, 0.85f, 1.0f);  // Melocotón pastel

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ========== CONFIGURAR ILUMINACIÓN ==========
        lightingShader.Use();

        GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
        glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

        // Luz direccional (sol) - CONTROLADA POR TECLA S
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.3f, -1.0f, -0.5f);

        if (solEncendido)
        {
            // Sol encendido - Luz más intensa
            glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.5f, 0.5f, 0.55f);
            glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.8f, 0.8f, 0.85f);
            glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.3f, 0.3f, 0.35f);
        }
        else
        {
            // Sol apagado - Luz reducida (como estaba antes)
            glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.3f, 0.3f, 0.35f);
            glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.5f, 0.5f, 0.55f);
            glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.2f, 0.2f, 0.25f);
        }

        // Luces puntuales (focos) - INTENSIDAD BALANCEADA
        for (int i = 0; i < 5; i++)  // 5 focos ahora
        {
            std::string pointLight = "pointLights[" + std::to_string(i) + "]";

            glUniform3f(glGetUniformLocation(lightingShader.Program, (pointLight + ".position").c_str()),
                posicionesFocos[i].x, posicionesFocos[i].y, posicionesFocos[i].z);

            if (lucesEncendidas)
            {
                glUniform3f(glGetUniformLocation(lightingShader.Program, (pointLight + ".ambient").c_str()), 0.15f, 0.13f, 0.06f);
                glUniform3f(glGetUniformLocation(lightingShader.Program, (pointLight + ".diffuse").c_str()), 0.4f, 0.35f, 0.18f);
                glUniform3f(glGetUniformLocation(lightingShader.Program, (pointLight + ".specular").c_str()), 0.25f, 0.25f, 0.15f);
            }
            else
            {
                glUniform3f(glGetUniformLocation(lightingShader.Program, (pointLight + ".ambient").c_str()), 0.0f, 0.0f, 0.0f);
                glUniform3f(glGetUniformLocation(lightingShader.Program, (pointLight + ".diffuse").c_str()), 0.0f, 0.0f, 0.0f);
                glUniform3f(glGetUniformLocation(lightingShader.Program, (pointLight + ".specular").c_str()), 0.0f, 0.0f, 0.0f);
            }

            glUniform1f(glGetUniformLocation(lightingShader.Program, (pointLight + ".constant").c_str()), 1.0f);
            glUniform1f(glGetUniformLocation(lightingShader.Program, (pointLight + ".linear").c_str()), 0.09f);
            glUniform1f(glGetUniformLocation(lightingShader.Program, (pointLight + ".quadratic").c_str()), 0.032f);
        }

        // Spotlight (linterna de la cámara - apagada por defecto)
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"),
            camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"),
            camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.09f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.032f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));

        glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 32.0f);

        // Camera matrices
        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // ========== PRIMER PISO ==========
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        primerPiso.Draw(lightingShader);

        // ========== PASTO ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        pasto.Draw(lightingShader);

        // ========== SEGUNDO PISO ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        segundoPiso.Draw(lightingShader);

        // ========== PUERTA 01 (PRINCIPAL) ==========
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.4973f, 0.911f, 0.81453f));
        model = glm::rotate(model, glm::radians(rotPuerta01), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        puerta01.Draw(lightingShader);

        // ========== PUERTA 02 (INTERNA) ==========
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.985f, 1.0471f, -1.5316f));
        model = glm::rotate(model, glm::radians(rotPuerta02), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        puerta02.Draw(lightingShader);

        // ========== PUERTA 03 (INTERNA) ==========
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0104f, 0.90583f, -2.4666f));
        model = glm::rotate(model, glm::radians(rotPuerta03), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        puerta03.Draw(lightingShader);

        // ========== PUERTA 04 (INTERNA) ==========
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.5334f, 0.98508f, -2.9855f));
        model = glm::rotate(model, glm::radians(rotPuerta04), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        puerta04.Draw(lightingShader);

        // ========== PUERTA 05 (INTERNA) ==========
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.085749f, 1.0696f, -4.9291f));
        model = glm::rotate(model, glm::radians(rotPuerta05), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        puerta05.Draw(lightingShader);

        // ========== CORTINA DERECHA (CON ANIMACIÓN) ==========
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(posCortinaDerecha, 1.5033f, -0.18378f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        cortinaDerecha.Draw(lightingShader);

        // ========== CORTINA IZQUIERDA (CON ANIMACIÓN) ==========
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(posCortinaIzquierda, 1.5033f, -0.18378f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        cortinaIzquierda.Draw(lightingShader);

        // ========== CORTINAS 2DO PISO ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        cortinas.Draw(lightingShader);

        // ========== LAMPARAS ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        lamparas.Draw(lightingShader);

        // ========== FOCOS INDIVIDUALES (USAR LAMPSHADER) ==========
        lampShader.Use();
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // Foco 1 - Cuarto
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        foco01.Draw(lampShader);

        // Foco 2 - Pasillo
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        foco02.Draw(lampShader);

        // Foco 3 - Cuarto Vacío
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        foco03.Draw(lampShader);

        // Foco 4 - Cocina
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        foco04.Draw(lampShader);

        // Foco 5 - Lavandería
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        foco05.Draw(lampShader);

        // ========== SOL (SOLO SI ESTÁ ENCENDIDO) ==========
        if (solEncendido)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(15.0f, 15.0f, -10.0f));  // Posición del sol (arriba y lejos)
            model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));  // Tamaño grande
            glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
            sol.Draw(lampShader);
        }

        // Volver a usar lightingShader
        lightingShader.Use();

        // ========== MESA COCINA ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        mesaCocina.Draw(lightingShader);

        // ========== COMIDA ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        comida.Draw(lightingShader);

        // ========== ESTUFA ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        estufa.Draw(lightingShader);

        // ========== PINTURA ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        pintura.Draw(lightingShader);

        // ========== PERCHERO PARED ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        percheroPared.Draw(lightingShader);

        // ========== LAVABO ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        lavabo.Draw(lightingShader);

        // ========== LAVADORA ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        lavadora.Draw(lightingShader);

        // ========== LAVADORA TAMBOR (ROTACIÓN) ==========
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.6562f, 0.6059f, -6.0319f));
        model = glm::rotate(model, glm::radians(rotTambor), glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        lavadoraTambor.Draw(lightingShader);

        // ========== BURRO ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        burro.Draw(lightingShader);

        // ========== PLANCHA ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        plancha.Draw(lightingShader);

        // ========== CAMA ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        cama.Draw(lightingShader);

        // ========== ROPERO ALTO ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        roperoAlto.Draw(lightingShader);

        // ========== ROPERO CHICO ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        roperoChico.Draw(lightingShader);

        // ========== CAJON GRANDE (CON ANIMACIÓN EN Z) ==========
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.8336f, 0.24662f, posCajonGrandeZ));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        cajonsote.Draw(lightingShader);

        // ========== CAJON CHICO (CON ANIMACIÓN EN Z) ==========
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.8409f, 0.58786f, posCajonChicoZ));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        cajonsitos.Draw(lightingShader);

        // ========== CARRITO ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        carrito.Draw(lightingShader);

        // ========== PERCHERO ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        perchero.Draw(lightingShader);

        // ========== FLORES ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        flores.Draw(lightingShader);

        // ========== MESITA ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        mesita.Draw(lightingShader);

        // ========== CAMPANA ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        campana.Draw(lightingShader);

        // ========== REPISA ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        repisa.Draw(lightingShader);

        // ========== RELOJ REDONDO ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        relojRedondo.Draw(lightingShader);

        // ========== ROPA ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        ropa.Draw(lightingShader);

        // ========== LIBROS ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        libros.Draw(lightingShader);

        // ========== REFRI ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        refri.Draw(lightingShader);

        // ========== MANECILLA ==========
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.27422f, 1.82f, -3.304f));
        model = glm::rotate(model, glm::radians(rotManecilla), glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        manecilla.Draw(lightingShader);

        // ========== PERILLA01 ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        perilla01.Draw(lightingShader);

        // ========== PERILLA02 ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        perilla02.Draw(lightingShader);

        // ========== ADORNOS PECERA ==========
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        adornosPecera.Draw(lightingShader);

        // ========== PECES (CON ANIMACIÓN COMPLEJA) ==========
        model = glm::mat4(1.0f);

        // Posición central de la pecera
        glm::vec3 centroPecera = glm::vec3(-4.2546f, 0.81017f, -2.6099f);

        // ===== AJUSTE 1: RADIO Y AMPLITUD REDUCIDOS PARA NO SALIRSE =====
        // Dimensiones pecera: X=0.441m, Y=0.323m, Z=0.269m
        // El eje más pequeño es Z (0.269m), semi-profundidad = 0.1345m
        // Usamos radio de 0.08m para dejar margen (el modelo del pez también ocupa espacio)
        float radioCarrusel = 0.08f;

        // Calcular posición en trayectoria circular (carrusel en XZ)
        float offsetX = radioCarrusel * cos(anguloCarrusel);
        float offsetZ = radioCarrusel * sin(anguloCarrusel);

        // Movimiento senoidal en Y con amplitud reducida
        // Semi-altura de la pecera: 0.323/2 = 0.1615m
        // Usamos amplitud de 0.06m para mantenernos bien dentro
        float amplitudSeno = 0.06f;
        float frecuenciaSeno = 2.0f;
        float offsetY = amplitudSeno * sin(frecuenciaSeno * tiempoPeces);

        // Aplicar transformaciones
        model = glm::translate(model, centroPecera);
        model = glm::translate(model, glm::vec3(offsetX, offsetY, offsetZ));
        model = glm::rotate(model, anguloCarrusel, glm::vec3(0.0f, 1.0f, 0.0f));  // Rotación carrusel

        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        peces.Draw(lightingShader);

        // ========== OBJETOS CON TRANSPARENCIA (PECERA + VENTANAS) ==========
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // PECERA (transparente)
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        pecera.Draw(lightingShader);

        // VENTANAS (transparentes)
        model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        ventanas.Draw(lightingShader);

        glDisable(GL_BLEND);

        // ========== DIBUJAR BILLBOARDS DE AGUA ==========
        if (aguaEncendida)
        {
            glDepthMask(GL_FALSE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            billboardShader.Use();
            glUniformMatrix4fv(glGetUniformLocation(billboardShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(glGetUniformLocation(billboardShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texturaGota);
            glUniform1i(glGetUniformLocation(billboardShader.Program, "texture1"), 0);

            glBindVertexArray(VAO_billboard);

            glm::vec3 cameraRight = glm::vec3(view[0][0], view[1][0], view[2][0]);
            glm::vec3 cameraUp = glm::vec3(view[0][1], view[1][1], view[2][1]);

            for (int i = 0; i < NUM_GOTAS; i++)
            {
                if (particulas[i].activa)
                {
                    model = glm::mat4(1.0f);
                    model = glm::translate(model, particulas[i].posicion);

                    model[0] = glm::vec4(cameraRight, 0.0f);
                    model[1] = glm::vec4(cameraUp, 0.0f);
                    model[2] = glm::vec4(glm::cross(cameraRight, cameraUp), 0.0f);

                    // Tamaño variable según tipo de partícula
                    float anchoGota = particulas[i].tamano;
                    float altoGota = particulas[i].tamano * 1.5f;  // Más alargada

                    // ===== AJUSTE 2: SALPICADURAS MÁS GRANDES =====
                    // Salpicaduras son ahora 2x más grandes que antes (antes era 0.3x, ahora 0.6x)
                    if (particulas[i].esSalpicadura)
                    {
                        anchoGota *= 0.6f;
                        altoGota *= 0.6f;
                    }

                    model = glm::scale(model, glm::vec3(anchoGota, altoGota, 1.0f));

                    glUniformMatrix4fv(glGetUniformLocation(billboardShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                }
            }

            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_BLEND);
            glDepthMask(GL_TRUE);
        }

        // ========== DIBUJAR BILLBOARDS DE VAPOR ==========
        if (vaporEncendido)
        {
            glDepthMask(GL_FALSE);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            billboardShader.Use();
            glUniformMatrix4fv(glGetUniformLocation(billboardShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(glGetUniformLocation(billboardShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texturaVapor);
            glUniform1i(glGetUniformLocation(billboardShader.Program, "texture1"), 0);

            glBindVertexArray(VAO_billboard);

            glm::vec3 cameraRight = glm::vec3(view[0][0], view[1][0], view[2][0]);
            glm::vec3 cameraUp = glm::vec3(view[0][1], view[1][1], view[2][1]);

            for (int i = 0; i < NUM_VAPOR; i++)
            {
                if (particulasVapor[i].activa)
                {
                    model = glm::mat4(1.0f);
                    model = glm::translate(model, particulasVapor[i].posicion);

                    model[0] = glm::vec4(cameraRight, 0.0f);
                    model[1] = glm::vec4(cameraUp, 0.0f);
                    model[2] = glm::vec4(glm::cross(cameraRight, cameraUp), 0.0f);

                    float escala = 1.2f;
                    model = glm::scale(model, glm::vec3(escala, escala, 1.0f));

                    glUniformMatrix4fv(glGetUniformLocation(billboardShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                }
            }

            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable(GL_BLEND);
            glDepthMask(GL_TRUE);
        }

        // Swap buffers
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO_billboard);
    glDeleteBuffers(1, &VBO_billboard);
    glDeleteBuffers(1, &EBO_billboard);
    glDeleteTextures(1, &texturaGota);
    glDeleteTextures(1, &texturaVapor);

    glfwTerminate();
    return 0;
}

// Función de animación
void Animation()
{
    float velocidadPuerta = 60.0f * deltaTime;
    float velocidadCortina = 1.5f * deltaTime;
    float velocidadCajon = 0.8f * deltaTime;  // Velocidad de los cajones

    // Animación del tambor
    if (animTambor)
    {
        rotTambor += 90.0f * deltaTime;
        if (rotTambor >= 360.0f)
        {
            rotTambor -= 360.0f;
        }
    }

    // Animación de las manecillas
    if (animManecilla)
    {
        rotManecilla += 30.0f * deltaTime;
        if (rotManecilla >= 360.0f)
        {
            rotManecilla -= 360.0f;
        }
    }

    // ========== ANIMACIÓN DE PECES (CARRUSEL + SENOIDAL) ==========
    if (animPeces)
    {
        // Rotación del carrusel (giro sobre el eje Y)
        anguloCarrusel += 0.5f * deltaTime;  // Velocidad de rotación lenta
        if (anguloCarrusel >= 2.0f * 3.14159f)  // 2π radianes = 360°
        {
            anguloCarrusel -= 2.0f * 3.14159f;
        }

        // Tiempo para la trayectoria senoidal
        tiempoPeces += deltaTime;
    }

    // ========== ANIMACIÓN PUERTA PRINCIPAL (01) ==========
    if (animPuerta01)
    {
        if (abrirPuerta01)
        {
            rotPuerta01 += velocidadPuerta;
            if (rotPuerta01 >= MAX_APERTURA_PUERTA)
            {
                rotPuerta01 = MAX_APERTURA_PUERTA;
                animPuerta01 = false;
            }
        }
        else
        {
            rotPuerta01 -= velocidadPuerta;
            if (rotPuerta01 <= MIN_APERTURA_PUERTA)
            {
                rotPuerta01 = MIN_APERTURA_PUERTA;
                animPuerta01 = false;
            }
        }
    }

    // ========== ANIMACIÓN PUERTAS INTERNAS (02, 03, 04, 05) ==========
    if (animPuertasInternas)
    {
        if (abrirPuertasInternas)
        {
            rotPuerta02 += velocidadPuerta;
            rotPuerta03 += velocidadPuerta;
            rotPuerta04 += velocidadPuerta;
            rotPuerta05 += velocidadPuerta;

            if (rotPuerta02 >= MAX_APERTURA_PUERTA)
            {
                rotPuerta02 = MAX_APERTURA_PUERTA;
                rotPuerta03 = MAX_APERTURA_PUERTA;
                rotPuerta04 = MAX_APERTURA_PUERTA;
                rotPuerta05 = MAX_APERTURA_PUERTA;
                animPuertasInternas = false;
            }
        }
        else
        {
            rotPuerta02 -= velocidadPuerta;
            rotPuerta03 -= velocidadPuerta;
            rotPuerta04 -= velocidadPuerta;
            rotPuerta05 -= velocidadPuerta;

            if (rotPuerta02 <= MIN_APERTURA_PUERTA)
            {
                rotPuerta02 = MIN_APERTURA_PUERTA;
                rotPuerta03 = MIN_APERTURA_PUERTA;
                rotPuerta04 = MIN_APERTURA_PUERTA;
                rotPuerta05 = MIN_APERTURA_PUERTA;
                animPuertasInternas = false;
            }
        }
    }

    // ========== ANIMACIÓN CORTINAS ==========
    if (animCortinas)
    {
        if (abrirCortinas)
        {
            posCortinaDerecha -= velocidadCortina;
            posCortinaIzquierda += velocidadCortina;

            if (posCortinaDerecha <= POS_FINAL_CORTINA_DERECHA)
            {
                posCortinaDerecha = POS_FINAL_CORTINA_DERECHA;
                posCortinaIzquierda = POS_FINAL_CORTINA_IZQUIERDA;
                animCortinas = false;
            }
        }
        else
        {
            posCortinaDerecha += velocidadCortina;
            posCortinaIzquierda -= velocidadCortina;

            if (posCortinaDerecha >= POS_INICIAL_CORTINA_DERECHA)
            {
                posCortinaDerecha = POS_INICIAL_CORTINA_DERECHA;
                posCortinaIzquierda = POS_INICIAL_CORTINA_IZQUIERDA;
                animCortinas = false;
            }
        }
    }

    // ========== ANIMACIÓN CAJÓN GRANDE (TECLA J) ==========
    if (animCajonGrande)
    {
        if (abrirCajonGrande)
        {
            // Mover hacia adelante (aumentar Z, de -2.33 a -1.90)
            posCajonGrandeZ += velocidadCajon;
            if (posCajonGrandeZ >= POS_FINAL_CAJON_GRANDE_Z)
            {
                posCajonGrandeZ = POS_FINAL_CAJON_GRANDE_Z;
                animCajonGrande = false;
            }
        }
        else
        {
            // Cerrar cajón (disminuir Z, de -1.90 a -2.33)
            posCajonGrandeZ -= velocidadCajon;
            if (posCajonGrandeZ <= POS_INICIAL_CAJON_GRANDE_Z)
            {
                posCajonGrandeZ = POS_INICIAL_CAJON_GRANDE_Z;
                animCajonGrande = false;
            }
        }
    }

    // ========== ANIMACIÓN CAJÓN CHICO (TECLA K) ==========
    if (animCajonChico)
    {
        if (abrirCajonChico)
        {
            // Mover hacia adelante (aumentar Z, de -2.33 a -1.90)
            posCajonChicoZ += velocidadCajon;
            if (posCajonChicoZ >= POS_FINAL_CAJON_CHICO_Z)
            {
                posCajonChicoZ = POS_FINAL_CAJON_CHICO_Z;
                animCajonChico = false;
            }
        }
        else
        {
            // Cerrar cajón (disminuir Z, de -1.90 a -2.33)
            posCajonChicoZ -= velocidadCajon;
            if (posCajonChicoZ <= POS_INICIAL_CAJON_CHICO_Z)
            {
                posCajonChicoZ = POS_INICIAL_CAJON_CHICO_Z;
                animCajonChico = false;
            }
        }
    }

    // ========== SISTEMA DE PARTÍCULAS (AGUA) ==========
    if (aguaEncendida)
    {
        static float tiempoGeneracion = 0.0f;
        tiempoGeneracion += deltaTime;

        if (tiempoGeneracion >= TASA_GENERACION)
        {
            tiempoGeneracion = 0.0f;

            // Buscar slot para nueva gota principal
            for (int i = 0; i < NUM_GOTAS; i++)
            {
                if (!particulas[i].activa)
                {
                    particulas[i].activa = true;
                    particulas[i].esSalpicadura = false;

                    // POSICIÓN: Todas salen del MISMO PUNTO (sin dispersión)
                    particulas[i].posicion = posicionBoquilla;

                    // VELOCIDAD: Caída vertical con muy poca variación lateral
                    particulas[i].velocidad = glm::vec3(
                        ((rand() % 5) - 2) / 1000.0f,  // Mínima variación X
                        -0.3f,                          // Velocidad inicial hacia abajo
                        ((rand() % 5) - 2) / 1000.0f   // Mínima variación Z
                    );

                    // TAMAÑO: 3.5x más grande con variación aleatoria
                    float tamanoBase = 0.28f;  // 0.08 * 3.5
                    particulas[i].tamano = tamanoBase + ((rand() % 20) - 10) / 200.0f;

                    particulas[i].vida = TIEMPO_VIDA_GOTA;
                    break;
                }
            }
        }

        // Actualizar todas las partículas
        for (int i = 0; i < NUM_GOTAS; i++)
        {
            if (particulas[i].activa)
            {
                if (!particulas[i].esSalpicadura)
                {
                    // GOTAS PRINCIPALES: Física con gravedad
                    particulas[i].velocidad.y -= GRAVEDAD * deltaTime;
                    particulas[i].posicion += particulas[i].velocidad * deltaTime;
                    particulas[i].vida -= deltaTime;

                    // Colisión con el fondo del lavabo
                    if (particulas[i].posicion.y <= alturaLavabo)
                    {
                        // Crear salpicaduras en el punto de impacto
                        CrearSalpicaduras(particulas[i].posicion);
                        particulas[i].activa = false;
                    }

                    // Muerte por tiempo
                    if (particulas[i].vida <= 0.0f)
                    {
                        particulas[i].activa = false;
                    }
                }
                else
                {
                    // SALPICADURAS: Física de rebote
                    particulas[i].velocidad.y -= GRAVEDAD * deltaTime;
                    particulas[i].posicion += particulas[i].velocidad * deltaTime;
                    particulas[i].vida -= deltaTime;

                    // Las salpicaduras mueren al caer de nuevo o por tiempo
                    if (particulas[i].posicion.y <= alturaLavabo || particulas[i].vida <= 0.0f)
                    {
                        particulas[i].activa = false;
                    }
                }
            }
        }
    }
    else
    {
        // Desactivar todas las partículas cuando se apaga el agua
        for (int i = 0; i < NUM_GOTAS; i++)
        {
            particulas[i].activa = false;
        }
    }

    // ========== SISTEMA DE PARTÍCULAS (VAPOR) ==========
    if (vaporEncendido)
    {
        static float tiempoGeneracionVapor = 0.0f;
        tiempoGeneracionVapor += deltaTime;

        if (tiempoGeneracionVapor >= TASA_GENERACION_VAPOR)
        {
            tiempoGeneracionVapor = 0.0f;

            for (int i = 0; i < NUM_VAPOR; i++)
            {
                if (!particulasVapor[i].activa)
                {
                    particulasVapor[i].activa = true;
                    particulasVapor[i].posicion = posicionPlancha;

                    particulasVapor[i].posicion.x += ((rand() % 10) - 5) / 100.0f;
                    particulasVapor[i].posicion.z += ((rand() % 10) - 5) / 100.0f;

                    particulasVapor[i].velocidad = glm::vec3(
                        0.0f,
                        0.5f,
                        0.0f
                    );

                    particulasVapor[i].vida = TIEMPO_VIDA_VAPOR;
                    break;
                }
            }
        }

        for (int i = 0; i < NUM_VAPOR; i++)
        {
            if (particulasVapor[i].activa)
            {
                float tiempoTranscurrido = TIEMPO_VIDA_VAPOR - particulasVapor[i].vida;

                float amplitud = 0.3f;
                float frecuencia = 2.0f;
                particulasVapor[i].velocidad.x = amplitud * cos(frecuencia * tiempoTranscurrido);
                particulasVapor[i].velocidad.z = amplitud * sin(frecuencia * tiempoTranscurrido);

                particulasVapor[i].posicion += particulasVapor[i].velocidad * deltaTime;
                particulasVapor[i].vida -= deltaTime;

                if (particulasVapor[i].vida <= 0.0f || particulasVapor[i].posicion.y > 3.0f)
                {
                    particulasVapor[i].activa = false;
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < NUM_VAPOR; i++)
        {
            particulasVapor[i].activa = false;
        }
    }
}

void DoMovement()
{
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP])
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }

    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN])
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }

    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT])
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }

    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT])
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    // Tecla T para activar/desactivar la animación del tambor
    if (key == GLFW_KEY_T && action == GLFW_PRESS)
    {
        animTambor = !animTambor;
    }

    // Tecla M para activar/desactivar la animación de las manecillas
    if (key == GLFW_KEY_M && action == GLFW_PRESS)
    {
        animManecilla = !animManecilla;
    }

    // ========== TECLA P PARA LA PUERTA PRINCIPAL ==========
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        animPuerta01 = true;
        abrirPuerta01 = (rotPuerta01 < MAX_APERTURA_PUERTA / 2.0f);
    }

    // ========== TECLA O PARA LAS PUERTAS INTERNAS ==========
    if (key == GLFW_KEY_O && action == GLFW_PRESS)
    {
        animPuertasInternas = true;
        abrirPuertasInternas = (rotPuerta02 < MAX_APERTURA_PUERTA / 2.0f);
    }

    // ========== TECLA C PARA LAS CORTINAS ==========
    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        animCortinas = true;
        float puntoMedio = (POS_INICIAL_CORTINA_DERECHA + POS_FINAL_CORTINA_DERECHA) / 2.0f;
        abrirCortinas = (posCortinaDerecha > puntoMedio);
    }

    // ========== TECLA J PARA EL CAJÓN GRANDE ==========
    if (key == GLFW_KEY_J && action == GLFW_PRESS)
    {
        animCajonGrande = true;
        float puntoMedio = (POS_INICIAL_CAJON_GRANDE_Z + POS_FINAL_CAJON_GRANDE_Z) / 2.0f;
        abrirCajonGrande = (posCajonGrandeZ < puntoMedio);  // Si está más atrás que el medio, abrir
        std::cout << "Cajón Grande " << (abrirCajonGrande ? "ABRIENDO" : "CERRANDO") << std::endl;
    }

    // ========== TECLA K PARA EL CAJÓN CHICO ==========
    if (key == GLFW_KEY_K && action == GLFW_PRESS)
    {
        animCajonChico = true;
        float puntoMedio = (POS_INICIAL_CAJON_CHICO_Z + POS_FINAL_CAJON_CHICO_Z) / 2.0f;
        abrirCajonChico = (posCajonChicoZ < puntoMedio);  // Si está más atrás que el medio, abrir
        std::cout << "Cajón Chico " << (abrirCajonChico ? "ABRIENDO" : "CERRANDO") << std::endl;
    }

    // ========== TECLA G PARA EL AGUA ==========
    if (key == GLFW_KEY_G && action == GLFW_PRESS)
    {
        aguaEncendida = !aguaEncendida;
        std::cout << "Agua " << (aguaEncendida ? "ENCENDIDA" : "APAGADA") << std::endl;
    }

    // ========== TECLA L PARA LAS LUCES ==========
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
    {
        lucesEncendidas = !lucesEncendidas;
        std::cout << "Luces " << (lucesEncendidas ? "ENCENDIDAS" : "APAGADAS") << std::endl;
    }

    // ========== TECLA U PARA EL SOL ==========
    if (key == GLFW_KEY_U && action == GLFW_PRESS)
    {
        solEncendido = !solEncendido;
        std::cout << "Sol " << (solEncendido ? "ENCENDIDO" : "APAGADO") << std::endl;
    }

    // ========== TECLA V PARA EL VAPOR ==========
    if (key == GLFW_KEY_V && action == GLFW_PRESS)
    {
        vaporEncendido = !vaporEncendido;
        std::cout << "Vapor " << (vaporEncendido ? "ENCENDIDO" : "APAGADO") << std::endl;
    }

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            keys[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            keys[key] = false;
        }
    }
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;

    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}

// ===== AJUSTE 2: FUNCIÓN PARA CREAR SALPICADURAS MÁS GRANDES Y MÁS LEJOS =====
void CrearSalpicaduras(glm::vec3 posicionImpacto)
{
    // Crear 5-8 salpicaduras por cada gota que cae
    int numSalpicaduras = 5 + (rand() % 4);  // Entre 5 y 8

    for (int j = 0; j < numSalpicaduras; j++)
    {
        // Buscar un slot libre para la salpicadura
        for (int i = 0; i < NUM_GOTAS; i++)
        {
            if (!particulas[i].activa)
            {
                particulas[i].activa = true;
                particulas[i].esSalpicadura = true;

                // Posición: Donde impactó la gota (en el fondo del lavabo)
                particulas[i].posicion = posicionImpacto;
                particulas[i].posicion.y = alturaLavabo;  // En el fondo

                // Ángulo aleatorio para dispersión radial
                float angulo = (rand() % 360) * 3.14159f / 180.0f;

                // ===== MÁS LEJOS: Aumentar distancia de dispersión 3x =====
                // Antes: 0.05 a 0.20 unidades
                // Ahora: 0.15 a 0.60 unidades (3x más lejos)
                float distancia = 0.15f + (rand() % 45) / 100.0f;  // 0.15 a 0.60

                // Velocidad: Salpica hacia arriba y hacia los lados
                // ===== MÁS LEJOS: Aumentar velocidad lateral y vertical =====
                particulas[i].velocidad = glm::vec3(
                    cos(angulo) * distancia * 5.0f,     // Dispersión en X (antes 2.0f, ahora 5.0f)
                    0.8f + (rand() % 20) / 100.0f,      // Sube 0.8-1.0 unidades (antes 0.3-0.4)
                    sin(angulo) * distancia * 5.0f      // Dispersión en Z (antes 2.0f, ahora 5.0f)
                );

                // ===== MÁS GRANDES: Tamaño aumentado 2.5x =====
                // Antes: 0.08 + variación pequeña
                // Ahora: 0.20 + variación (2.5x más grandes)
                particulas[i].tamano = 0.20f + (rand() % 10) / 100.0f;

                // Tiempo de vida corto
                particulas[i].vida = TIEMPO_VIDA_SALPICADURA;

                break;  // Pasar a la siguiente salpicadura
            }
        }
    }
}