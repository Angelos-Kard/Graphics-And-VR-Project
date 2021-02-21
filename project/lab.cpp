//#define TESTING    //It loads a simple plane (with texture) and the particles
//#define QUAD_RAIN  //It loads particles as quads in order to be easily visible

// Include C++ headers
#include <iostream>
#include <string>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Shader loading utilities and other
#include <common/shader.h>
#include <common/util.h>
#include <common/camera.h>
#include <model.h>
#include <texture.h>
#include "FountainEmitter.h"
#include "OrbitEmitter.h"
//#define STB_IMAGE_IMPLEMENTATION
//#include <stbi_image.h>
//delete the includes afterwards
#include <chrono>
using namespace std::chrono;
//

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <windows.h>
//#include <MMSystem.h>

using namespace std;
using namespace glm;

// Function prototypes
void initialize();
void createContext();
void mainLoop();
void free();

#define W_WIDTH 1024
//#define W_WIDTH 1920
#define W_HEIGHT 768
//#define W_HEIGHT 1080
#define TITLE "Rain"


// Global variables
GLFWwindow* window;
Camera* camera;
//Shaders
GLuint particleShaderProgram, normalShaderProgram;
//Matrices & Locations
GLuint projectionMatrixLocation, viewMatrixLocation, modelMatrixLocation, projectionAndViewMatrix, projectionMatrixLocation2, viewMatrixLocation2;
GLuint translationMatrixLocation, rotationMatrixLocation, scaleMatrixLocation;
//Textures and Samplers
GLuint rainTexture, diffuceColorSampler, landscapeTexture, pathTexture, grassTexture, rock_tipTexture, greenTexture, rockTexture, diffuceColorSampler2, sunTexture;
GLuint ps_logo, cross_icon, square_icon, circle_icon, triangle_icon;
GLuint waterTexture, water2Texture, waterTextureSampler, water2TextureSampler, raindropTexture, raindropTextureSampler, timeUniform;
//Flag & Location
int flood_flag = 0;
GLuint flood_flag_location;

//Lighting
//glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
//glm::vec3 lightPos(-30.0f, 80.0f, -20.0f);
glm::vec3 lightPos;
GLuint diffuse_Location, ambient_Location, specular_Location, Light_power_Location;
glm::vec3 ambient = vec3(0.95f, 0.95f, 0.95f), diffuse = vec3(0.95f, 0.95f, 0.95f), specular = vec3(0.95f, 0.95f, 0.95f);


glm::vec3 La, Ld, Ls;
GLfloat light_power;
float west_east = 0.0f, up_down = 40.0f, north_south = -10.0f, angle_light = -45.0f;
GLuint lightPosLocation, lightPosLocation2;
int light_mode = 1;

glm::vec3 slider_emitter_pos(0.0f, 30.0f, 0.0f);
int particles_slider = 5000;

void pollKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
void joystick_callback(int jid, int event);
void pseudo_pollJoystick(int time_interval);

bool game_paused = false;

bool use_sorting = false;
bool use_rotations = false;

float wind_x = 0.0f;
float wind_z = 0.0f;
bool speed = false;

float flood_height = 2.0f; //MAX: 5.0f
bool it_is_raining = false;
int audio_plays = 0; //0: No Rain -- 1: Light Rain -- 2: Heavy Rain -- 3: Thunderstorm


float height_threshold = 100.0f;

glm::vec4 background_color = glm::vec4(0.5f, 0.5f, 0.5f, 0.0f);

int buttonCount;
const unsigned char* button;
int axisCount;
const float* theaxis;



/* Skybox Attempt
unsigned int textureID;

vector<std::string> faces
{
        "right.png",
        "left.png",
        "top.png",
        "bottom.png",
        "front.png",
        "back.png"
};
unsigned int cubemapTexture;
GLuint cubeVAO, cubeVBO;
//*/

void renderHelpingWindow() {
    //static int counter = 0;

    ImGui::Begin("Debugging Window");                          // Create a window called "Hello, world!" and append into it.

    //ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

    if (glfwJoystickPresent(GLFW_JOYSTICK_1))
    {
        ImGui::Text("Controller 1 is connected!\n");
        ImGui::Text("Hold 'R1' for Tutorial");
    }
    else
    {
        ImGui::Text("No Controller is connected");
        ImGui::SameLine();
        if ((int)glfwGetTime() % 3 == 0) ImGui::Text(".");
        else if ((int)glfwGetTime() % 3 == 1) ImGui::Text("..");
        else ImGui::Text("...");
        ImGui::Text("PS4 Controller is supported.");
    }

    ImGui::Text("");
    ImGui::ColorEdit3("Background", &background_color[0]);

    //ImGui::SliderFloat("x position", &slider_emitter_pos[0], -30.0f, 30.0f);
    //ImGui::SliderFloat("y position", &slider_emitter_pos[1],  0.0f, 50.0f);
    //ImGui::SliderFloat("z position", &slider_emitter_pos[2], -30.0f, 30.0f);

    ImGui::Text("");
    ImGui::Text("West");
    ImGui::SameLine();
    ImGui::Text("\t\t\t\t\tEast");
    ImGui::SliderFloat("Wind - X Axis", &wind_x, -30.0f, 30.0f);
    
    ImGui::Text("");
    ImGui::Text("North");
    ImGui::SameLine();
    
    ImGui::Text("\t\t\t\t  South");
    ImGui::SliderFloat("Wind - Z Axis", &wind_z, -20.0f, 30.0f);

    ImGui::Text("");
    ImGui::SliderFloat("Light Position", &angle_light, -100.0f, 100.0f);

    //ImGui::SliderFloat("height", &height_threshold, 0, 200);
    ImGui::Text("");
    if (use_rotations) ImGui::SliderInt("Rain", &particles_slider, 0, 17500);
    else ImGui::SliderInt("Rain", &particles_slider, 0, 23500);



    /*
    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);
    */
    ImGui::Checkbox("Use sorting", &use_sorting);
    ImGui::Checkbox("Use rotations", &use_rotations);
    
    ImGui::Checkbox("Increase moving speed (for debugging)", &speed);

    ImGui::Text("");
    ImGui::Text("Max Particles:\n- Rotations ON: 17,500\n- Rotations OFF: 23,500");

    ImGui::Text("");
    if (ImGui::Button("   ")) // Buttons return true when clicked (most widgets return true when edited/activated)
        light_mode = (light_mode + 1) % 4;
    ImGui::SameLine();
    ImGui::Text("Light Mode: %d -", light_mode);
    ImGui::SameLine();
    if (light_mode==0) ImGui::Text("Night");
    if (light_mode==1) ImGui::Text("Day");
    if (light_mode==2) ImGui::Text("Sunset");
    if (light_mode==3) ImGui::Text("Cloudy");


    ImGui::Text("");
    ImGui::Text("The program was tested with 60 FPS Cap");

    ImGui::Text("Performance %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);


    int compass = degrees(-camera->horizontalAngle + 3.14f) < 0.0f ? 360 + (int)degrees(-camera->horizontalAngle + 3.14f)%360 : (int)degrees(-camera->horizontalAngle + 3.14f)%360;
    ImGui::Text("");
    ImGui::Text("Camera Points At: %do", compass);
    ImGui::SameLine();
    
    if (compass > 160 && compass < 200) ImGui::Text("(South)");
    else if (compass >= 110 && compass <= 160) ImGui::Text("(Southeast)");
    else if (compass > 70 && compass < 110) ImGui::Text("(East)");
    else if (compass >= 20 && compass <= 70) ImGui::Text("(Northeast)");
    else if (compass > 340 || compass < 20) ImGui::Text("(North)");
    else if (compass >= 290 && compass <= 340) ImGui::Text("(Northwest)");
    else if (compass > 250 && compass < 290) ImGui::Text("(West)");
    else if (compass >= 200 && compass <= 250) ImGui::Text("(Southwest)");
    
    


    ImGui::End();
 
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

void renderTutorialWindow() //Tutorial for Controller
{
    ImGui::Begin("Tutorial");
    ImGui::Text(">> Use 'Left Analog' to move");
    ImGui::Text(">> Use 'Right Analog' to look around");
    ImGui::Text(">> Use 'D-PAD Up' to move upwards");
    ImGui::Text(">> Use 'D-PAD Down' to move downwards");
    ImGui::Text(">> Use 'D-PAD Left' and 'D-Pad Right' to change the position of the light");
    ImGui::Text(">> Press 'L1' to change light mode*");
    ImGui::Text(">> Press 'Options' to pause the particles*");
    ImGui::Text(">> Release 'R1' to close\n   the tutorial window");

    ImGui::Text(">> Hold");
    ImGui::SameLine();
    ImGui::Image((void*)cross_icon, ImVec2(20, 20));
    ImGui::SameLine();
    ImGui::Text("and press 'L2' or 'R2' to control");
    ImGui::Text("   the amount of droplets");

    ImGui::Text(">> Hold");
    ImGui::SameLine();
    ImGui::Image((void*)circle_icon, ImVec2(20, 20));
    ImGui::SameLine();
    ImGui::Text("and press 'L2' or 'R2' to control");
    ImGui::Text("   the direction of the wind at X-axis");

    ImGui::Text(">> Hold");
    ImGui::SameLine();
    ImGui::Image((void*)square_icon, ImVec2(20, 20));
    ImGui::SameLine();
    ImGui::Text("and press 'L2' or 'R2' to control");
    ImGui::Text("   the direction of the wind at Z-axis");

    ImGui::Text(">> Press");
    ImGui::SameLine();
    ImGui::Image((void*)triangle_icon, ImVec2(20, 20));
    ImGui::SameLine();
    ImGui::Text("to turn on or off the rotations*");

    ImGui::Text(">> Press");
    ImGui::SameLine();
    ImGui::Image((void*)ps_logo, ImVec2(20, 20));
    ImGui::SameLine();
    ImGui::Text("to terminate the program");

    ImGui::Text("");
    ImGui::Text("* WORK IN PROGRESS - POSSIBLE BUGS -\nPREFER THE CHECKBOX/KEYBOARD BUTTON");

    ImGui::End();  
}

void createContext() {
    
    particleShaderProgram = loadShaders(
        "ParticleShader.vertexshader",
        "ParticleShader.fragmentshader");

    normalShaderProgram = loadShaders(
        "StandardShading.vertexshader",
        "StandardShading.fragmentshader");

    projectionAndViewMatrix = glGetUniformLocation(particleShaderProgram, "PV");

    translationMatrixLocation = glGetUniformLocation(normalShaderProgram, "T");
    rotationMatrixLocation = glGetUniformLocation(normalShaderProgram, "R");
    scaleMatrixLocation = glGetUniformLocation(normalShaderProgram, "S");

    modelMatrixLocation = glGetUniformLocation(normalShaderProgram, "M");

    viewMatrixLocation = glGetUniformLocation(normalShaderProgram, "V");
    projectionMatrixLocation = glGetUniformLocation(normalShaderProgram, "P");

    viewMatrixLocation2 = glGetUniformLocation(particleShaderProgram, "V");
    projectionMatrixLocation2 = glGetUniformLocation(particleShaderProgram, "P");

    lightPosLocation = glGetUniformLocation(normalShaderProgram, "lightPos");
    lightPosLocation2 = glGetUniformLocation(particleShaderProgram, "lightPos");
    ambient_Location = glGetUniformLocation(normalShaderProgram, "La");
    diffuse_Location = glGetUniformLocation(normalShaderProgram, "Ld");
    specular_Location = glGetUniformLocation(normalShaderProgram, "Ls");
    Light_power_Location = glGetUniformLocation(normalShaderProgram, "light_power");
    
    flood_flag_location = glGetUniformLocation(normalShaderProgram, "flood_flag");

    diffuceColorSampler = glGetUniformLocation(particleShaderProgram, "texture0");
    diffuceColorSampler2 = glGetUniformLocation(normalShaderProgram, "texture1");

    rainTexture = loadSOIL("los_textures/water.png");
    //landscapeTexture = loadSOIL("los_textures/aerial_grass_rock_diff_2k.jpg");
    //landscapeTexture = loadSOIL("los_textures/aerial_grass_rock_diff_2k.jpg");
    //landscapeTexture = loadSOIL("los_textures/water.png");
    pathTexture = loadSOIL("los_textures/dirt-texture.jpg");

    ps_logo = loadSOIL("los_textures/ps_button.png");
    cross_icon = loadSOIL("los_textures/cross_icon.png");
    square_icon = loadSOIL("los_textures/square_icon.png");
    circle_icon = loadSOIL("los_textures/circle_icon.png");
    triangle_icon = loadSOIL("los_textures/triangle_icon.png");

    waterTextureSampler = glGetUniformLocation(normalShaderProgram, "waterTextureSampler");
    waterTexture = loadSOIL("los_textures/water.png");
    
    water2TextureSampler = glGetUniformLocation(normalShaderProgram, "water2TextureSampler");
    water2Texture = loadSOIL("los_textures/water2.bmp");
    
    raindropTextureSampler = glGetUniformLocation(normalShaderProgram, "raindropTextureSampler");
    raindropTexture = loadSOIL("los_textures/rain_drop_texture.jpg");

    timeUniform = glGetUniformLocation(normalShaderProgram, "time");

    sunTexture = loadSOIL("los_textures/sun_texture.jpg");

#ifndef TESTING
    grassTexture = loadSOIL("los_textures/grass.jpg");
    rock_tipTexture = loadSOIL("los_textures/rock-tip.jpg");
    greenTexture = loadSOIL("los_textures/green.jpg");
    rockTexture = loadSOIL("los_textures/rock.jpg");
    
#endif // !TESTING

    glfwSetKeyCallback(window, pollKeyboard);

    glfwSetJoystickCallback(joystick_callback);
    if (glfwJoystickPresent(GLFW_JOYSTICK_1))
    {
        button = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
        theaxis = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axisCount);
    }

    /* Skybox
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    //*/
}

void free() {
    glDeleteProgram(particleShaderProgram);
    glfwTerminate();
}

void mainLoop() {
    
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    camera->position = vec3(0, 5, 30);

    light_power = 10.0f;


#ifndef QUAD_RAIN
    auto* droplet = new Drawable("obj_files/droplet.obj");
    //auto* droplet = new Drawable("obj_files/droplet.obj"); //More Detailed - Low Perfomance
#else
    auto* quad = new Drawable("obj_files/quad.obj");
#endif // !QUAD_RAIN


#ifndef TESTING
    //auto* green = new Drawable("obj_files/LandscapeObjects/green.obj");
    auto* grass = new Drawable("obj_files/LandscapeObjects/grass_v2.obj");
    auto* path = new Drawable("obj_files/LandscapeObjects/path.obj");
    auto* rock = new Drawable("obj_files/LandscapeObjects/rock_v2.obj");
    //auto* rock_base = new Drawable("obj_files/LandscapeObjects/rock_base.obj");
    auto* rock_tip = new Drawable("obj_files/LandscapeObjects/rock_tip.obj");
    //auto* rock_2 = new Drawable("obj_files/LandscapeObjects/rock_2.obj");
    auto* lake = new Drawable("obj_files/LandscapeObjects/lake.obj");
    auto* sun = new Drawable("obj_files/LandscapeObjects/sun.obj");
    auto* flood = new Drawable("obj_files/LandscapeObjects/flood_plane.obj");
#else
    auto* plane = new Drawable("obj_files/plane.obj");
#endif // !TESTING

    
    //auto* landscape = new Drawable("obj_files/landscape_v2.obj");
    //auto* landscape = new Drawable("obj_files/landscape_more_triangles_v1.obj");

    
    /* Skybox
    cubemapTexture = loadCubemap(faces);
    //*/
#ifndef QUAD_RAIN
    FountainEmitter f_emitter = FountainEmitter(droplet, particles_slider);
#else
    FountainEmitter f_emitter = FountainEmitter(quad, particles_slider);
#endif // !QUAD_RAIN

    
    
    //OrbitEmitter o_emitter = OrbitEmitter(quad, particles_slider, 10, 80);

    float t = glfwGetTime();
    
    do {
    //    if (wind_x == 0.0f && wind_z == 0.0f) use_rotations = false;
    //    else use_rotations = true;
        if (use_rotations == true && particles_slider > 17500) particles_slider = 17500;
        
        f_emitter.changeParticleNumber(particles_slider);
        f_emitter.emitter_pos = slider_emitter_pos;
        f_emitter.use_rotations = use_rotations;
        f_emitter.use_sorting = use_sorting;
        f_emitter.height_threshold = height_threshold;
        f_emitter.wind_x = wind_x;
        f_emitter.wind_z = wind_z;

        float currentTime = glfwGetTime();
        float dt = currentTime - t;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        glClearColor(background_color[0], background_color[1], background_color[2], background_color[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        if (light_mode == 0)
        { 
            ambient = vec3(0.1f, 0.1f, 0.1f);
            diffuse = vec3(0.1f, 0.1f, 0.1f);
            specular = vec3(0.1f, 0.1f, 0.1f);
        }
        else if (light_mode == 1)
        {
            ambient = vec3(0.95f, 0.95f, 0.95f);
            diffuse = vec3(0.95f, 0.95f, 0.95f);
            specular = vec3(0.5f, 0.5f, 0.5f);
        }
        else if (light_mode == 2)
        {
            ambient = vec3(0.95f, 0.65f, 0.0f);
            diffuse = vec3(0.95f, 0.95f, 0.95f);
            specular = vec3(0.5f, 0.5f, 0.5f);
        }
        else if (light_mode == 3)
        {
            ambient = vec3(0.1f, 0.1f, 0.1f);
            diffuse = vec3(0.5f, 0.5f, 0.5f);
            specular = vec3(0.1f, 0.1f, 0.1f);
        }




        glUseProgram(particleShaderProgram);

        
        // camera
        if (speed) camera->speed = 15.0f;
        else camera->speed = 6.0f;
        camera->update();
        mat4 projectionMatrix = camera->projectionMatrix;
        mat4 viewMatrix = camera->viewMatrix;

        auto PV = projectionMatrix * viewMatrix;
        glUniformMatrix4fv(projectionAndViewMatrix, 1, GL_FALSE, &PV[0][0]);
        glUniformMatrix4fv(viewMatrixLocation2, 1, GL_FALSE, &viewMatrix[0][0]);
        glUniformMatrix4fv(projectionMatrixLocation2, 1, GL_FALSE, &projectionMatrix[0][0]);

        lightPos = vec3(glm::rotate(mat4(1.0f), glm::radians(-angle_light), vec3(0.0f, 0.0f, 1.0f)) * vec4(west_east, up_down, north_south, 1.0f));
        auto lightPos_cameraspace = vec3(viewMatrix * glm::mat4(1.0) * vec4(lightPos, 1.0f));
        glUniform3f(lightPosLocation2, lightPos.x, lightPos.y, lightPos.z);
        
        //*/ Use particle based drawing
        glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, fireTexture);
        glBindTexture(GL_TEXTURE_2D, rainTexture);
        glUniform1i(diffuceColorSampler, 0);
        if(!game_paused) {
            f_emitter.updateParticles(currentTime, dt, camera->position);
            //o_emitter.updateParticles(currentTime, dt, camera->position);
        }
        f_emitter.renderParticles();
        //o_emitter.renderParticles();
        //*/
        
        
        
        //*// Use standard drawing procedure
        glUseProgram(normalShaderProgram);
        
        glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
        glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);

        //auto p = f_emitter.p_attributes[i];
        //auto modelMatrix = glm::scale(mat4(1.0f), vec3(4.0f, 4.0f, 4.0f));
        auto r = glm::rotate(mat4(), radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
        auto s = glm::scale(glm::mat4(), glm::vec3(3, 4, 3));
        auto tr = glm::translate(glm::mat4(), glm::vec3(-5.0f, 0.0f, -30.0f)); //Normal Mode
        //auto tr = glm::translate(glm::mat4(), glm::vec3(-5.0f, -40.0f, -30.0f)); //God Mode
        //auto tr = glm::translate(glm::mat4(), glm::vec3(-5.0f, 0.0f, 5.0f));

        glUniformMatrix4fv(translationMatrixLocation, 1, GL_FALSE, &tr[0][0]);
        glUniformMatrix4fv(rotationMatrixLocation, 1, GL_FALSE, &r[0][0]);
        glUniformMatrix4fv(scaleMatrixLocation, 1, GL_FALSE, &s[0][0]);

#ifndef TESTING

        La = ambient;
        Ld = diffuse;
        Ls = specular;

        glUniform3f(ambient_Location, La.x, La.y, La.z);
        glUniform3f(diffuse_Location, Ld.x, Ld.y, Ld.z);
        glUniform3f(specular_Location, Ls.x, Ls.y, Ls.z);

        glUniform1f(Light_power_Location, light_power);
        
        glUniform3f(lightPosLocation, lightPos_cameraspace.x, lightPos_cameraspace.y, lightPos_cameraspace.z);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, waterTexture);
        glUniform1i(waterTextureSampler, 1);

        glUniform1f(timeUniform, (float)glfwGetTime() / 20.0);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, water2Texture);
        glUniform1i(water2TextureSampler, 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, raindropTexture);
        glUniform1i(raindropTextureSampler, 3);

        flood_flag = 0;
        glUniform1i(flood_flag_location, flood_flag);

        flood_flag = 2;
        glUniform1i(flood_flag_location, flood_flag);
        lake->bind();
        glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, fireTexture);
        glBindTexture(GL_TEXTURE_2D, rock_tipTexture);
        glUniform1i(diffuceColorSampler2, 0);
        //auto modelMatrix = t * r * s;
        //glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
        lake->draw();


        flood_flag = 0;
        glUniform1i(flood_flag_location, flood_flag);
        path->bind();
        glBindTexture(GL_TEXTURE_2D, pathTexture);
        glUniform1i(diffuceColorSampler2, 0);
        path->draw();

        flood_flag = 2;
        glUniform1i(flood_flag_location, flood_flag);
        grass->bind();
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        glUniform1i(diffuceColorSampler2, 0);
        grass->draw();


        rock_tip->bind();
        glBindTexture(GL_TEXTURE_2D, rock_tipTexture);
        glUniform1i(diffuceColorSampler2, 0);
        rock_tip->draw();


        rock->bind();
        glBindTexture(GL_TEXTURE_2D, rockTexture);
        glUniform1i(diffuceColorSampler2, 0);
        rock->draw();


        if (particles_slider != 0) it_is_raining = true;
        else it_is_raining = false;

        if (it_is_raining == true && flood_height < 5.0f && game_paused == false)
        {
            flood_height += particles_slider / (750 * 10000.0f);
            if (flood_height > 5.0f) flood_height = 5.0f;
        }
        else if (it_is_raining == false && flood_height > 0.0f && game_paused == false)
        {
            flood_height -= 0.1f / 500;
            if (flood_height < 0.0f) flood_height = 0.0f;
        }
        
        tr = glm::translate(glm::mat4(), glm::vec3(-5.0f, flood_height, -30.0f));
        glUniformMatrix4fv(translationMatrixLocation, 1, GL_FALSE, &tr[0][0]);

        flood_flag = 1;
        glUniform1i(flood_flag_location, flood_flag);

        flood->bind();

        glBindTexture(GL_TEXTURE_2D, waterTexture);
        glUniform1i(diffuceColorSampler2, 0);
        flood->draw();


        /*
        flood_flag = 2;
        glUniform1i(flood_flag_location, flood_flag);

        r = glm::mat4();
        s = glm::mat4();
        tr = glm::translate(glm::mat4(), lightPos);
        //tr = glm::mat4();
        
        
        glUniformMatrix4fv(translationMatrixLocation, 1, GL_FALSE, &tr[0][0]);
        glUniformMatrix4fv(rotationMatrixLocation, 1, GL_FALSE, &r[0][0]);
        glUniformMatrix4fv(scaleMatrixLocation, 1, GL_FALSE, &s[0][0]);
        //*/
        
        /*
        sun->bind();

        glBindTexture(GL_TEXTURE_2D, sunTexture);
        glUniform1i(diffuceColorSampler2, 0);
        sun->draw();
        //*/
        
#else
        plane->bind();

        glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, fireTexture);
        glBindTexture(GL_TEXTURE_2D, pathTexture);
        glUniform1i(diffuceColorSampler2, 0);
        //auto modelMatrix = t * r * s;
        //glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
        plane->draw();
#endif // !TESTING


        /*/

        //Audio
        if (particles_slider == 0 && audio_plays != 0)
        {
            PlaySound(NULL, NULL, SND_ASYNC);
            audio_plays = 0;
        }
        else if (particles_slider > 0 && particles_slider < 10000 && audio_plays != 1 && game_paused == false)
        {
            PlaySound("audio_files/light-rain.wav", NULL, SND_LOOP|SND_ASYNC);
            audio_plays = 1;
        }
        else if (particles_slider >= 12000 && particles_slider < 15000 && audio_plays != 2 && game_paused == false)
        {
            PlaySound("audio_files/heavy-rain.wav", NULL, SND_LOOP | SND_ASYNC);
            audio_plays = 2;
        }
        else if (particles_slider >= 15000 && audio_plays != 3 && game_paused == false)
        {
            PlaySound("audio_files/thunderstorm.wav", NULL, SND_LOOP | SND_ASYNC);
            audio_plays = 3;
        }
        //*/


        /* Skybox
        glBindVertexArray(cubeVAO);


        mat4 cubeMVP = projectionMatrix * viewMatrix * mat4();
        glUniformMatrix4fv(MVPLocation, 1, GL_FALSE, &cubeMVP[0][0]);

        glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

        //*/

        //Rain Control -- Controller
        if (glfwJoystickPresent(GLFW_JOYSTICK_1) && button[1] == GLFW_PRESS) //Must hold the Cross button
        {
            if (theaxis[4] > 0.0f && (use_rotations ? particles_slider < 17500 : particles_slider < 23500)) particles_slider += 100;  //Increase Particles -- //Use of R2 
            else if (theaxis[3] > 0.0f && particles_slider > 0) particles_slider -= 100; //Decrease Particles -- //Use of L2
        }

        //Wind Control -- Controller
        else if (glfwJoystickPresent(GLFW_JOYSTICK_1) && button[2] == GLFW_PRESS) //Must hold the Circle button
        {
            if (theaxis[4] > 0.0f && wind_x < 30.0f) wind_x += 0.25;
            else if (theaxis[3] > 0.0f && wind_x > -30.0f) wind_x -= 0.25;
        }

        //Wind Control -- Controller
        else if (glfwJoystickPresent(GLFW_JOYSTICK_1) && button[0] == GLFW_PRESS) //Must hold the Rectangle button
        {
            if (theaxis[4] > 0.0f && wind_z < 30.0f) wind_z += 0.25;
            else if (theaxis[3] > 0.0f && wind_z > -20.0f) wind_z -= 0.25;
        }

        //Light
        else if (glfwJoystickPresent(GLFW_JOYSTICK_1) && button[15] == GLFW_PRESS) angle_light += 1.0f;
        else if (glfwJoystickPresent(GLFW_JOYSTICK_1) && button[17] == GLFW_PRESS) angle_light -= 1.0f;
        

        //Tutorial Rendering
        if (glfwJoystickPresent(GLFW_JOYSTICK_1) && button[5] == GLFW_PRESS)
            renderTutorialWindow();

        /*
        if (glfwJoystickPresent(GLFW_JOYSTICK_1) && button[9] == GLFW_PRESS)
        {
            
            game_paused = !game_paused;
            
        }
        //*/

        //Poll Joystick
        if (glfwJoystickPresent(GLFW_JOYSTICK_1)) pseudo_pollJoystick((int)glfwGetTime() * 1000);


        renderHelpingWindow();
        glfwPollEvents();
        glfwSwapBuffers(window);
        t = currentTime;

    } while (((glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) && (glfwJoystickPresent(GLFW_JOYSTICK_1)?button[12]!=GLFW_PRESS:true)) &&
             glfwWindowShouldClose(window) == 0);
}

void initialize() {
    // Initialize GLFW
    if (!glfwInit()) {
        throw runtime_error("Failed to initialize GLFW\n");
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(W_WIDTH, W_HEIGHT, TITLE, NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        throw runtime_error(string(string("Failed to open GLFW window.") +
                            " If you have an Intel GPU, they are not 3.3 compatible." +
                            "Try the 2.1 version.\n"));
    }

    glfwMakeContextCurrent(window);

    // Start GLEW extension handler
    glewExperimental = GL_TRUE;

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        throw runtime_error("Failed to initialize GLEW\n");
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Hide the mouse and enable unlimited movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, W_WIDTH / 2, W_HEIGHT / 2);

    // Gray background color
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    //glEnable(GL_CULL_FACE);
    // glFrontFace(GL_CW);
    // glFrontFace(GL_CCW);

    // enable point size when drawing points
    glEnable(GL_PROGRAM_POINT_SIZE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Log
    logGLParameters();

   
    //glfwSwapInterval(0); //Turns Off Vsync

    // Create camera
    camera = new Camera(window);

    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
                                 camera->onMouseMove(xpos, ypos);
                             }
    );
}

void pollKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Task 2.1:
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        game_paused = !game_paused;
        PlaySound(NULL, NULL, SND_ASYNC);
        audio_plays = 0;
    }

    if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS) {
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            camera->active = true;
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            camera->active = false;
        }

    }
}

//*
void joystick_callback(int jid, int event)
{
    if (event == GLFW_CONNECTED)
    {
        std::cout << "Connected " << glfwGetJoystickName(GLFW_JOYSTICK_1) << endl;
        button = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
        theaxis = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axisCount);
    }
    else if (event == GLFW_DISCONNECTED)
    {
        std::cout << "Disconnected" << endl;
    }
}
//*/

void pseudo_pollJoystick(int time)
{
    static int i = 500;
    static int timelog = 0;
    //int sensitivity_log = 100;
    //int pause_time = 250;
    

    if (time % i < 250 /*pause_time*/ && time - timelog > 150 /*sensitivity_log*/)
    {
        if (button[9] == GLFW_PRESS)
        {
            game_paused = !game_paused;
            PlaySound(NULL, NULL, SND_ASYNC);
            audio_plays = 0;
            //std::cout << time << " " << time % 2 << "---" << game_paused << std::endl;
            timelog = time;
            i = 2;
        }
        else if (button[3] == GLFW_PRESS)
        {
            use_rotations = !use_rotations;
            timelog = time;
            i = 2;
        }
        else if (button[10] == GLFW_PRESS)
        {
            speed = !speed;
            timelog = time;
            i = 2;
        }
        else if (button[4] == GLFW_PRESS)
        {
            light_mode = (light_mode + 1) % 4;
            timelog = time;
            i = 2;
        }
    }
    else if (i == 2 && time - timelog > 150 /*sensitivity_log*/) i = 250 /*pause_time*/;


}

/*
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
//*/




int main(void) {
    try {
        initialize();
        createContext();
        mainLoop();
        free();
    } catch (exception& ex) {
        cout << ex.what() << endl;
        getchar();
        free();
        return -1;
    }

    return 0;
}