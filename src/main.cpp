#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1000;  // reduce to increase fps
const unsigned int SCR_HEIGHT = 1000; // reduce to increase fps
const unsigned int iterPerFrame = 30; // reduce to increase fps


bool hasEnding(std::string const& fullString, std::string const& ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    }
    else {
        return false;
    }
}

void printFramebufferStatus()
{
    switch (glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
        
    case GL_FRAMEBUFFER_COMPLETE:
        std::cout << "GL_FRAMEBUFFER_COMPLETE";
        break;
    case GL_FRAMEBUFFER_UNDEFINED:
        std::cout << "GL_FRAMEBUFFER_UNDEFINED";
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        std::cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT ";
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        std::cout << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT ";
        break;
    default:
        std::cout << "other error";
    }
    std::cout << '\n';
}

unsigned int loadShader(std::string fileName)
{
    std::ostringstream sstream;
    std::ifstream fs("src/shaders/" + fileName);
    sstream << fs.rdbuf();
    const std::string str(sstream.str());
    const char* ptr = str.c_str();

    unsigned int shader;
    if (hasEnding(fileName, ".vert"))
    {
        shader = glCreateShader(GL_VERTEX_SHADER);
    }
    else if (hasEnding(fileName, ".frag")) {
        shader = glCreateShader(GL_FRAGMENT_SHADER);
    }
    else
    {
        std::cout << "Error, invalid shader file name\n";
    }

    glShaderSource(shader, 1, &ptr, NULL);
    glCompileShader(shader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::" + fileName + "::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

GLuint create2DF32framebuffer(GLuint tex)
{
    GLuint frameBuff;
    glGenFramebuffers(1, &frameBuff);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuff);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

  //  printFramebufferStatus();
    return frameBuff;
}

GLuint create2DF32framebuffer(GLuint tex0, GLuint tex1)
{
    GLuint frameBuff;
    glGenFramebuffers(1, &frameBuff);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuff);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex0, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, tex1, 0);

    //  printFramebufferStatus();
    return frameBuff;
}



GLuint create2DF32texture(GLsizei width, GLsizei height, GLenum format)
{
    GLuint tex;
   // glCreateTextures(GL_TEXTURE_2D , 1, &tex); // fails
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    GLenum internalFormat = 0;

    switch (format)
    {
    case GL_R:
        internalFormat = GL_R32F;
        break;
    case GL_RG:
        internalFormat = GL_RG32F;
        break;
    case GL_RGB:
        internalFormat = GL_RGB32F;
        break;
    case GL_RGBA:
        internalFormat = GL_RGBA32F;
        break;
    default:
        return 0;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_FLOAT, nullptr); // glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, 100, 100, 0, GL_RG, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_NEAREST
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return tex;
}


GLuint createProgram(GLuint vertexShader, GLuint fragmentShader, std::string name)
{
    int success;
    char infoLog[512];

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::" + name + "::LINKING_FAILED\n" << infoLog << std::endl;
    }
    return shaderProgram;
}


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Fire Simulation", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::cout << "glew fail\n";
    }


    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    // build and compile our shader programs

    GLuint simulationVertexShader = loadShader("sim.vert");

    GLuint reactFragmentShader = loadShader("react.frag");
    GLuint calcDivergenceFragmentShader = loadShader("calcDivergence.frag");
    GLuint fixDivergenceFragmentShader = loadShader("fixDivergence.frag");
    GLuint advectFragmentShader = loadShader("advect.frag");

    GLuint displayFragmentShader = loadShader("display.frag"); 

    GLuint reactProgram = createProgram(simulationVertexShader, reactFragmentShader, "reactProgram");
    GLuint calcDivergenceProgram = createProgram(simulationVertexShader, calcDivergenceFragmentShader, "calcDivergenceProgram");
    GLuint fixDivergenceProgram = createProgram(simulationVertexShader, fixDivergenceFragmentShader, "fixDivergenceProgram");
    GLuint advectProgram = createProgram(simulationVertexShader, advectFragmentShader, "advectProgram");
    GLuint displayProgram = createProgram(simulationVertexShader, displayFragmentShader, "displayProgram");

    glDeleteShader(simulationVertexShader);
    glDeleteShader(calcDivergenceFragmentShader);
    glDeleteShader(fixDivergenceFragmentShader);
    glDeleteShader(advectFragmentShader);
    glDeleteShader(displayFragmentShader);
    


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    // 
     // // quad that fills the screen, so fragment shader is run for every pixel //
  // X, Y,  U, V  (x4)
    const float quadVertices[] = {
        // X,   Y,   U,   V
          1.0,-1.0, 1.0, 0.0,
         -1.0,-1.0, 0.0, 0.0,
          1.0, 1.0, 1.0, 1.0,
         -1.0, 1.0, 0.0, 1.0,
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    unsigned int positionAttribLocation = 0; // glGetAttribLocation(vertexShader, "vertPosition");
    unsigned int texCoordAttribLocation = 1; // glGetAttribLocation(vertexShader, "vertTexCoord");

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(positionAttribLocation);

    glVertexAttribPointer(texCoordAttribLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(texCoordAttribLocation);

/*
velocity texture:
[0] horizontal velocity
[1] vertical velocity
[2] pressure
[3] temperature


gas texture:
[0] oxygen
[1] methane
[2] CO2
[3] -
*/

    GLuint velocity_texture_0 = create2DF32texture(SCR_WIDTH, SCR_HEIGHT, GL_RGBA);
    GLuint velocity_texture_1 = create2DF32texture(SCR_WIDTH, SCR_HEIGHT, GL_RGBA);

    GLuint gas_texture_0 = create2DF32texture(SCR_WIDTH, SCR_HEIGHT, GL_RGBA);
    GLuint gas_texture_1 = create2DF32texture(SCR_WIDTH, SCR_HEIGHT, GL_RGBA);

    GLuint framebuff_0 = create2DF32framebuffer(velocity_texture_0, gas_texture_0);
    GLuint framebuff_1 = create2DF32framebuffer(velocity_texture_1, gas_texture_1);



    float texelSizeX = 1.0 / float(SCR_WIDTH);
    float texelSizeY = 1.0 / float(SCR_HEIGHT);


    glUseProgram(advectProgram);
    //glUniform1i(glGetUniformLocation(advectProgram, "velTex"), 0);
    glUniform2f(glGetUniformLocation(advectProgram, "texelSize"), texelSizeX, texelSizeY);
    glUseProgram(calcDivergenceProgram);
    glUniform2f(glGetUniformLocation(calcDivergenceProgram, "texelSize"), texelSizeX, texelSizeY);
    glUseProgram(fixDivergenceProgram);
    glUniform2f(glGetUniformLocation(fixDivergenceProgram, "texelSize"), texelSizeX, texelSizeY);

    /*
    glUseProgram(displayProgram);
    glUniform1i(glGetUniformLocation(displayProgram, "velTex"), 0); // replaced by layout(binding = 0) 
    */
    
    // render loop
    // -----------

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        float btn = 0.0;

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            btn = 1.0;
        }else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            btn = 2.0;
        }

        glUseProgram(advectProgram);
        glUniform3f(glGetUniformLocation(advectProgram, "mousePos"), mouseX / SCR_WIDTH, (SCR_HEIGHT - mouseY) / SCR_HEIGHT, btn);
 

        static unsigned int iterCnt = 0;
        static double prevTime = 0;
        double curr_time = glfwGetTime();
        if (curr_time - prevTime > 1.0)
        {
            prevTime = curr_time;
            std::cout << iterCnt << " IPS\n"; // iterations per second
            iterCnt = 0;
        }
        // iterCnt++;

        // simulate
        // ------
        
      //  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
       // glClear(GL_COLOR_BUFFER_BIT);

        const GLenum colAttach_0_1[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

        for (int i = 0; i < iterPerFrame; i++) { // simulation loop

            iterCnt++;

            glUseProgram(advectProgram);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, velocity_texture_0);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gas_texture_0);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuff_1);
            glDrawBuffers(2, colAttach_0_1);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            // for (int i = 0; i < 1; i++) {

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, velocity_texture_1);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuff_0);
            glUseProgram(calcDivergenceProgram);
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, velocity_texture_0);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuff_1);
            glUseProgram(fixDivergenceProgram);
            glDrawBuffer(GL_COLOR_ATTACHMENT0);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            //  }

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, velocity_texture_1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gas_texture_1);
            glBindFramebuffer(GL_FRAMEBUFFER, framebuff_0);
            glUseProgram(reactProgram);
            glDrawBuffers(2, colAttach_0_1);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        }

        // render
        // ------
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // draw to canvas
        glUseProgram(displayProgram);
       // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
       // glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, velocity_texture_0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gas_texture_0);

        
        //glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents(); 



        /*
        static double prevTime = 0;
        double tagetFPS = 60;
        double targetFrameTime = 1.0 / tagetFPS;
        double curr_time = glfwGetTime();
        double dur = (curr_time - prevTime);
        prevTime = curr_time;
        double sleep_ms = (targetFrameTime - dur) * 1000.0;
        if (sleep_ms > 0.0) // ensures that we don't have a dur < 0.0 which converts to a durDW of 0.
        {
            std::chrono::milliseconds sleepTime = std::chrono::milliseconds((long)sleep_ms);
            std::cout << dur*1000.0 << '\t' << sleep_ms << '\n';
            std::this_thread::sleep_for(sleepTime);
        }
       */


        



        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
        {
            static GLenum prevError = 0;
            if(err != prevError)
                std::cout << "error 0x" << std::hex << err << '\n';
            prevError = err;
        }
        
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glDeleteProgram(calcDivergenceProgram);
    glDeleteProgram(advectProgram);
    glDeleteProgram(displayProgram);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
