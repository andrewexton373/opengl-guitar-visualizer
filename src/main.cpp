
/* Guitar Visualizer: Shows Amplitude and Fundamental Frequency Being Played!
 - Andrew Exton
 - Used program 4 (heightmapping) base code from CPE 471 - Eckhardt
*/

#include <iostream>
#include <glad/glad.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "GLSL.h"
#include "Program.h"
#include "Pyramid.hpp"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "Camera.h"
#include "Audio.h"
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;

#include "Accelerate/accelerate.h"

/**** OpenAL setup ****/
#define FREQ 22050   // Sample rate
#define CAP_SIZE 2048 // How much to capture at a time (affects latency)

short buffer[FREQ*2]; // A buffer to hold captured audio

ALubyte texels[100*100*4];
ALubyte amplitudes2[1024];
ushort coreFrequency = 440;
float amplitudes[1024];
float buf2[2048];

class Freq {
public:
    ushort hz;
    float magnitude;
    
    Freq(ushort hz, float magnitude) {
        this->hz = hz;
        this->magnitude = magnitude;
    }
};

vector<Freq> frequencies;


class Application : public EventCallbacks
{

public:
    
    
	WindowManager * windowManager = nullptr;
    
    Audio audio;

    
    Camera camera;

	// Our shader programs
    std::shared_ptr<Program> landscapeProg, amplitudeProg, fundamentalFreqProg, skysphereProg, pyramidProg;

	// Shape to be used (from obj file)
	shared_ptr<Shape> sphere, skysphere;

	// Textures
	GLuint Texture, TextureSky, Texture2;

    // VAOs
    GLuint VertexArrayIDBox, Pyramid_VAO;

    // VBOs
    GLuint VertexBufferIDBox, VertexBufferTex, VertexBufferNormal, Pyramid_VBO, Pyramid_EBO;
    
    Pyramid pyramid;
    
	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
//        if (key == GLFW_KEY_W && action == GLFW_PRESS)
//        {
//            camera.w = 1;
//        }
//        if (key == GLFW_KEY_W && action == GLFW_RELEASE)
//        {
//            camera.w = 0;
//        }
//        if (key == GLFW_KEY_S && action == GLFW_PRESS)
//        {
//            camera.s = 1;
//        }
//        if (key == GLFW_KEY_S && action == GLFW_RELEASE)
//        {
//            camera.s = 0;
//        }
//        if (key == GLFW_KEY_A && action == GLFW_PRESS)
//        {
//            camera.a = 1;
//        }
//        if (key == GLFW_KEY_A && action == GLFW_RELEASE)
//        {
//            camera.a = 0;
//        }
//        if (key == GLFW_KEY_D && action == GLFW_PRESS)
//        {
//            camera.d = 1;
//        }
//        if (key == GLFW_KEY_D && action == GLFW_RELEASE)
//        {
//            camera.d = 0;
//        }
//        if (key == GLFW_KEY_E && action == GLFW_PRESS)
//        {
//            camera.e = 1;
//        }
//        if (key == GLFW_KEY_E && action == GLFW_RELEASE)
//        {
//            camera.e = 0;
//        }
//        if (key == GLFW_KEY_F && action == GLFW_PRESS)
//        {
//            camera.f = 1;
//        }
//        if (key == GLFW_KEY_F && action == GLFW_RELEASE)
//        {
//            camera.f = 0;
//        }
//        if (key == GLFW_KEY_R && action == GLFW_PRESS)
//        {
//            camera.r = 1;
//        }
//        if (key == GLFW_KEY_R && action == GLFW_RELEASE)
//        {
//            camera.r = 0;
//        }
//        if (key == GLFW_KEY_T && action == GLFW_PRESS)
//        {
//            camera.t = 1;
//        }
//        if (key == GLFW_KEY_T && action == GLFW_RELEASE)
//        {
//            camera.t = 0;
//        }
    }
    
    void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
		}
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}
    
	void init(const std::string& resourceDirectory)
	{
        
		GLSL::checkVersion();
		
		// Set background color.
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Culling
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);

		// Transparency
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		// Landscape
		landscapeProg = make_shared<Program>();
		landscapeProg->setVerbose(true);
		landscapeProg->setShaderNames(resourceDirectory + "/landscape_vert.glsl", resourceDirectory + "/landscape_frag.glsl");
		if (!landscapeProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		landscapeProg->init();
		landscapeProg->addUniform("P");
		landscapeProg->addUniform("MV");
		landscapeProg->addAttribute("vertPos");
		landscapeProg->addAttribute("vertNor");
		landscapeProg->addAttribute("vertTex");

        // Amplitude Sphere
        amplitudeProg = make_shared<Program>();
        amplitudeProg->setVerbose(true);
        amplitudeProg->setShaderNames(resourceDirectory + "/amplitude_vert.glsl", resourceDirectory + "/amplitude_frag.glsl");
        if (!amplitudeProg->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        amplitudeProg->init();
        amplitudeProg->addUniform("P");
        amplitudeProg->addUniform("MV");
        amplitudeProg->addAttribute("vertPos");
        amplitudeProg->addAttribute("vertNor");
        amplitudeProg->addAttribute("vertTex");
        
        // Fundamental Frequency Sphere
        fundamentalFreqProg = make_shared<Program>();
        fundamentalFreqProg->setVerbose(true);
        fundamentalFreqProg->setShaderNames(resourceDirectory + "/core_freq_vert.glsl", resourceDirectory + "/core_freq_frag.glsl");
        if (!fundamentalFreqProg->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        fundamentalFreqProg->init();
        fundamentalFreqProg->addUniform("P");
        fundamentalFreqProg->addUniform("MV");
        fundamentalFreqProg->addUniform("time");
        fundamentalFreqProg->addAttribute("vertPos");
        fundamentalFreqProg->addAttribute("vertNor");
        fundamentalFreqProg->addAttribute("vertTex");
        
        // Skysphere
        skysphereProg = make_shared<Program>();
        skysphereProg->setVerbose(true);
        skysphereProg->setShaderNames(resourceDirectory + "/simple_vertsky.glsl", resourceDirectory + "/simple_fragsky.glsl");
        if (!skysphereProg->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        skysphereProg->init();
        skysphereProg->addUniform("P");
        skysphereProg->addUniform("MV");
        skysphereProg->addAttribute("vertPos");
        skysphereProg->addAttribute("vertNor");
        skysphereProg->addAttribute("vertTex");
        
        // FIRE
        // Initialize the GLSL program.
        pyramidProg = std::make_shared<Program>();
        pyramidProg->setVerbose(true);
        pyramidProg->setShaderNames(resourceDirectory + "/fire_vertex.glsl", resourceDirectory + "/fire_fragment.glsl");
        if (!pyramidProg->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1); //make a breakpoint here and check the output window for the error message!
        }
        pyramidProg->addUniform("P");
        pyramidProg->addUniform("V");
        pyramidProg->addUniform("M");
        pyramidProg->addUniform("LightSource");
        pyramidProg->addAttribute("vPos");

    }
    
    void initGeom(const std::string& resourceDirectory)
	{
        
        // load sphere obj
        sphere = make_shared<Shape>();
        sphere->loadMesh(resourceDirectory + "/sphere.obj");
        sphere->resize();
        sphere->init();
        
        // PINK FLOYD PYRAMID
        
        // GENERATE CYLINDER
        int BASE_POINTS = 3;
        pyramid = Pyramid(BASE_POINTS);
        
        // generate the VAO
        glGenVertexArrays(1, &Pyramid_VAO);
        glBindVertexArray(Pyramid_VAO);
        
        glGenBuffers(1, &Pyramid_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, Pyramid_VBO);
        glBufferData(GL_ARRAY_BUFFER, pyramid.getVertexArraySize(), pyramid.getVertexArray(), GL_DYNAMIC_DRAW);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        glGenBuffers(1, &Pyramid_EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Pyramid_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, pyramid.getIndexArraySize(), pyramid.getIndexArray(), GL_STATIC_DRAW);
        
        glBindVertexArray(0);
        
        //generate the VAO
		glGenVertexArrays(1, &VertexArrayIDBox);
		glBindVertexArray(VertexArrayIDBox);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferIDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferIDBox);

		GLfloat *ver= new GLfloat[10000 * 18]; //100 x 100 squares * 6 vertices for a square * 3 dimensions
		int verc = 0;
			for (int i = 0; i < 100; i++) {
				for (int j = 0; j < 100; j++) {
                    //x, y, z
					ver[verc++] = 0.0 + j, ver[verc++] = 0.0 + i, ver[verc++] = 0.0;
					ver[verc++] = 1.0 + j, ver[verc++] = 0.0 + i, ver[verc++] = 0.0;
					ver[verc++] = 0.0 + j, ver[verc++] = 1.0 + i, ver[verc++] = 0.0;
					ver[verc++] = 1.0 + j, ver[verc++] = 0.0 + i, ver[verc++] = 0.0;
					ver[verc++] = 1.0 + j, ver[verc++] = 1.0 + i, ver[verc++] = 0.0;
					ver[verc++] = 0.0 + j, ver[verc++] = 1.0 + i, ver[verc++] = 0.0;
				}

		}
				
		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, 10000*18*sizeof(float), ver, GL_STATIC_DRAW);
		//we need to set up the vertex array
		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferTex);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferTex);

		float t = 1. / 100.;
		GLfloat *cube_tex=new GLfloat[10000 * 12];
		int texc = 0;
		for (int i = 0; i < 100; i++) {
			for (int j = 0; j < 100; j++) {
                //x, y, get z from heightmap image
				cube_tex[texc++] = (GLfloat)j*t, cube_tex[texc++] = (GLfloat)i*t;
				cube_tex[texc++] = (GLfloat)(j + 1)*t, cube_tex[texc++] = (GLfloat)i*t;
				cube_tex[texc++] = (GLfloat)j*t, cube_tex[texc++] = (GLfloat)(i + 1)*t;
				cube_tex[texc++] = (GLfloat)(j + 1)*t, cube_tex[texc++] = 0.0 + (GLfloat)i*t;
				cube_tex[texc++] = (GLfloat)(j + 1)*t, cube_tex[texc++] = (GLfloat)(i + 1)*t;
				cube_tex[texc++] = (GLfloat)j*t, cube_tex[texc++] = (GLfloat)(i + 1)*t;
			}
		}

		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, 10000*12*sizeof(float), cube_tex, GL_STATIC_DRAW);
		//we need to set up the vertex array
		glEnableVertexAttribArray(2);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        /** normals **/
        //generate vertex buffer to hand off to OGL
        glGenBuffers(1, &VertexBufferNormal);
        //set the current state to focus on our vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, VertexBufferNormal);
        GLfloat *nor= new GLfloat[10000 * 18]; //100 x 100 squares * 6 vertices for a square * 3 dimensions
        for (int i=0; i<10000*18; i+=3) {
            nor[i] = 0.0;
            nor[i+1] = 0.0;
            nor[i+2] = 1.0;
        }
        
        //actually memcopy the data - only do this once
        glBufferData(GL_ARRAY_BUFFER, 10000*18*sizeof(float), nor, GL_STATIC_DRAW);
        //we need to set up the vertex array
        glEnableVertexAttribArray(1);
        //key function to get up how many elements to pull out at a time (3)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		int width, height, channels;
		char filepath[1000];
        
        // dynamic audio texture
        glGenTextures(1, &Texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // ocean texture
		string str = resourceDirectory + "/oceantex.jpg";
		strcpy(filepath, str.c_str());
        unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture2);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Texture2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
        
        // sky texture
        str = resourceDirectory + "/starstex.jpg";
        strcpy(filepath, str.c_str());
        data = stbi_load(filepath, &width, &height, &channels, 4);
        glGenTextures(1, &TextureSky);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, TextureSky);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //set the 2 textures (audio waves and ocean waves) to the correct samplers in the fragment shader:
        GLuint Tex1Location = glGetUniformLocation(landscapeProg->pid, "tex");//tex, tex2... sampler in the fragment shader
        GLuint Tex2Location = glGetUniformLocation(landscapeProg->pid, "tex2");
        // Then bind the uniform samplers to texture units:
        glUseProgram(landscapeProg->pid);
        glUniform1i(Tex1Location, 0);
        glUniform1i(Tex2Location, 1);
        
	}

    
    void render()
    {
        // Get current frame buffer size.
        int width, height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
        glViewport(0, 0, width, height);
        
        // Apply orthographic projection....
        mat4 P = perspective((float)(3.14159 / 4.), (float)((float)width/ (float)height), 0.1f, 1000.0f);
        mat4 V = camera.process();
        
        generate_shader_texture_from_sample(); // GENREATE TEXELS FOR SHADER
        
        // Clear framebuffer.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        draw_skysphere(P, V);
        
        glEnable(GL_DEPTH_TEST);
        glFrontFace(GL_CCW);
        
//        draw_landscape(P, V);
        draw_fire_flares(P, V);


        draw_amplitude_sphere(P, V);
        draw_fundamental_freqency_sphere(P, V);
        

    }
    
    /**** draw skysphere ***/
    void draw_skysphere(mat4 &P, mat4 &V) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        skysphereProg->bind();
        glUniformMatrix4fv(skysphereProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        glDisable(GL_DEPTH_TEST);
        glFrontFace(GL_CW);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TextureSky);
        
        float pih = 3.1415926 / 2.0;
        mat4 R = rotate(mat4(1), pih, vec3(1, 0, 0));
        mat4 T = translate(mat4(1), -camera.pos);
        mat4 MV = V * T * R;
        glUniformMatrix4fv(skysphereProg->getUniform("MV"), 1, GL_FALSE, &MV[0][0]);
        
        sphere->draw(skysphereProg);
        
        skysphereProg->unbind();
    }
    
    void generate_shader_texture_from_sample() {
        int width = 100;
        int height = 100;
        
        int j=0;
        for (int i=0; i<100; i++) {
            for (int k=0; k<400; k+=4) {
                texels[(400*i)+k] = amplitudes2[j];
            }
            j+=10;
        }
        
        glActiveTexture(GL_TEXTURE0); // TEXELS
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texels);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    
    void draw_landscape(mat4 &P, mat4 &V) {
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        landscapeProg->bind();
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, Texture2);
        
        mat4 RV = camera.getR();
        RV = transpose(RV);

        mat4 T = translate(mat4(1), vec3(-50, -5, 0));
        
        float pih = -3.1415926 / 2.0;
        mat4 R = rotate(mat4(1), pih, vec3(1,0,0));
        
        mat4 MV = V * T * R;
        
        glUniformMatrix4fv(landscapeProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        glUniformMatrix4fv(landscapeProg->getUniform("MV"), 1, GL_FALSE, &MV[0][0]);
        
        glBindVertexArray(VertexArrayIDBox);
        
        glDrawArrays(GL_TRIANGLES, 0, 10000*6);
        landscapeProg->unbind();
    }
    
    /****** draw audio sphere - BASED ON AMPLITUDE *****/
    void draw_amplitude_sphere(mat4 &P, mat4 &V) {
        
        amplitudeProg->bind();
        
        float time = glfwGetTime();
        
        mat4 R = rotate(mat4(1), 0.8f * time, vec3(0,0,1));
        mat4 S = scale(mat4(1), vec3(20, 20, 20));
        mat4 T = translate(mat4(1), vec3(0, 0, -90));
        mat4 MV = V * T * S * R;
        
        glUniformMatrix4fv(amplitudeProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        glUniformMatrix4fv(amplitudeProg->getUniform("MV"), 1, GL_FALSE, &MV[0][0]);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        glPointSize(8.0f);
        sphere->draw(amplitudeProg);
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        sphere->draw(amplitudeProg);


        amplitudeProg->unbind();
    }
    
    /*** blue audio sphere - BASED ON FREQUENCY ****/
    void draw_fundamental_freqency_sphere(mat4 &P, mat4 &V) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        fundamentalFreqProg->bind();
        
        int count =0;
        float threshold = 100000;
        float time = glfwGetTime();
        float pih = 3.1415926 / 2.0;

        
        for (int i=0; i<frequencies.size(); i++) {
            if(frequencies[i].magnitude > threshold && count < 5) {
                mat4 R = rotate(mat4(1), 0.1f * time, vec3(1,0,0));
                mat4 T = translate(mat4(1), vec3(0, 0, -90));
                mat4 S = scale(mat4(1.0f), vec3(frequencies[i].hz));
                mat4 MV = V * T * S * R;
                
                glUniformMatrix4fv(fundamentalFreqProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
                glUniformMatrix4fv(fundamentalFreqProg->getUniform("MV"), 1, GL_FALSE, &MV[0][0]);
                glUniform1fv(fundamentalFreqProg->getUniform("time"), 1, &time);

                
                sphere->draw(fundamentalFreqProg);
                count++;

            }
        }
        
        fundamentalFreqProg->unbind();
    }
    
    void draw_fire_flares(mat4 &P, mat4 &V) {
        
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        pyramidProg->bind();
        
        glBindVertexArray(Pyramid_VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Pyramid_EBO);
        float time = glfwGetTime();
        
        mat4 PyramidS = scale(mat4(1.0f), vec3(3,10,3));
        mat4 PyramidR = rotate(glm::mat4(1.0f), time, vec3(0.0f, 1.0f, 0.0f));
        mat4 PyramidT = translate(mat4(1.0f), vec3(0, -6, 0));
        mat4 M = PyramidT*PyramidS*PyramidR;
        
        vec3 LightSource = vec3(8,8,8);

        glUniformMatrix4fv(pyramidProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        glUniformMatrix4fv(pyramidProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
        glUniformMatrix4fv(pyramidProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        glUniform3fv(pyramidProg->getUniform("LightSource"), 1, (GLfloat *) &LightSource[0]);


        glDrawElements(GL_TRIANGLES, pyramid.getFaceCount() * 3, GL_UNSIGNED_SHORT, (void*)0);
        
        pyramidProg->unbind();
    }
    
    
    //******* set up fourier transform for raw audio data *******//
    void fft(short buffer[]) {
        
        // clear texel buffer
        for (int i=0; i<40000; i++) {
            texels[i] = 0;
        }
        
        // create float array from short array
        copy(buffer, buffer + 2048, buf2);
        
        const int n = 2048;
        const int log2n = 11; // 2^11 = 2048
        
        DSPSplitComplex a;
        a.realp = new float[n/2];
        a.imagp = new float[n/2];
        
        // prepare the fft algo (you want to reuse the setup across fft calculations)
        FFTSetup setup = vDSP_create_fftsetup(log2n, kFFTRadix2);
        
        // copy the input to the packed complex array that the fft algo uses
        vDSP_ctoz((DSPComplex *) buf2, 2, &a, 1, n/2);
        
        // calculate the fft
        vDSP_fft_zrip(setup, &a, 1, log2n, FFT_FORWARD);
        
        int k=0;
        frequencies.clear();

        // Calculate Amplitudes + Magnitudes
        for (size_t i = 0; i < n / 2; i++) {
            float real = a.realp[i];
            float img = a.imagp[i];
            float magnitude = sqrt(real * real + img * img);
            
            Freq freq = Freq(i, magnitude);
            frequencies.push_back(freq);

            amplitudes[k] = img;
            k++;
        }
        
        sort(frequencies.begin(), frequencies.end(), sort_frequencies);
        
        // map each value from 'float' array to size of byte (originally size of short)
        for (int i=0; i<1024; i++) {
            ALubyte amplitude = (amplitudes[i] - SHRT_MIN)/(SHRT_MAX - SHRT_MIN) * 255;
            amplitudes2[i] = amplitude;
        }

    }
    
    static bool sort_frequencies(const Freq &lhs, const Freq &rhs) {
        return lhs.magnitude > rhs.magnitude;
    }
    
    
};


//*********************************************************************************************************
int main(int argc, char **argv)
{
	// Where the resources are loaded from
	std::string resourceDir = "../../resources";
	if (argc >= 2) {
		resourceDir = argv[1];
	}

	Application *application = new Application();
    
    // Inititalize audio
    Audio *audio = new Audio();
    audio->initAL();
    
    // Set up window
	WindowManager *windowManager = new WindowManager();
	windowManager->init(1920, 1080);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// Initialize appication
    application->init(resourceDir);
	application->initGeom(resourceDir);
    
    
	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
        audio->readAudio(); // Fill Buffer
        application->fft(audio->buffer); // Fast Fourier Transform Buffer

        application->render(); // Render w/ FFT Audio Data
        
        glfwSwapBuffers(windowManager->getHandle()); // Swap front and back buffers.
        
        glfwPollEvents(); // Poll for and process events.
	}
    
    // quit + clean up program
    audio->cleanAL();
	windowManager->shutdown();
	return 0;
    
}
