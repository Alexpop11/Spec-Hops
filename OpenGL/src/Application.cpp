#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <random>

// Particle structure
struct Particle {
    float x, y;    // Position
    float vx, vy;  // Velocity
};

// Shader sources
const char* computeShaderSource = R"(
#version 430 core
layout(local_size_x = 256) in;
layout(std430, binding = 0) buffer ParticleBuffer {
    Particle particles[];
};

uniform float deltaTime;
uniform vec2 bounds;

void main() {
    uint gid = gl_GlobalInvocationID.x;
    Particle particle = particles[gid];

    // Update position
    particle.x += particle.vx * deltaTime;
    particle.y += particle.vy * deltaTime;

    // Bounce off edges
    if (particle.x < -bounds.x || particle.x > bounds.x) {
        particle.vx = -particle.vx;
    }
    if (particle.y < -bounds.y || particle.y > bounds.y) {
        particle.vy = -particle.vy;
    }

    particles[gid] = particle;
}
)";

const char* vertexShaderSource = R"(
#version 430 core
layout(location = 0) in vec2 position;
void main() {
    gl_Position = vec4(position, 0.0, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 430 core
out vec4 fragColor;
void main() {
    fragColor = vec4(1.0, 1.0, 1.0, 1.0);
}
)";

GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    
    // Check for compilation errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Shader compilation error: " << infoLog << std::endl;
        return 0;
    }
    return shader;
}

GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // Check for linking errors
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Program linking error: " << infoLog << std::endl;
        return 0;
    }
    return program;
}


int main() {
   std::cout <<"entered main" << std::endl; 
      /* Initialize the library */
   if (!glfwInit())
      return -1;

   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



   // Get the primary monitor
   GLFWmonitor*       primaryMonitor = glfwGetPrimaryMonitor();
   const GLFWvidmode* mode           = glfwGetVideoMode(primaryMonitor);

   // Create a fullscreen window
   GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "SpaceBoom", NULL, NULL);
   if (!window) {
      std::cerr << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return -1;
   }

   /* Make the window's context current */
   glfwMakeContextCurrent(window);

   glfwSwapInterval(1);

   if (glewInit() != GLEW_OK)
      std::cout << "Error!" << std::endl;

   std::cout << "current version of GL: " << glGetString(GL_VERSION) << std::endl;

    // Create and compile shaders
    GLuint computeShader = compileShader(GL_COMPUTE_SHADER, computeShaderSource);
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    if (!computeShader || !vertexShader || !fragmentShader) {
        std::cerr << "Failed to compile shaders" << std::endl;
        return -1;
    }

    std::cout << "compiled shaders" << std::endl;

    // Create and link compute program
    GLuint computeProgram = linkProgram(computeShader, 0);
    if (!computeProgram) {
        std::cerr << "Failed to link compute program" << std::endl;
        return -1;
    }

    std::cout << "linked compute program" << std::endl;

    // Create and link render program
    GLuint renderProgram = linkProgram(vertexShader, fragmentShader);
    if (!renderProgram) {
        std::cerr << "Failed to link render program" << std::endl;
        return -1;
    }

   std::cout << "linked render program" << std::endl;

    // Generate particles
    const int NUM_PARTICLES = 10000;
    std::vector<Particle> particles(NUM_PARTICLES);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1.0, 1.0);

    std::cout << "generated particles" << std::endl;

    for (auto& p : particles) {
        p.x = dis(gen);
        p.y = dis(gen);
        p.vx = dis(gen) * 0.1f;
        p.vy = dis(gen) * 0.1f;
    }

    std::cout << "initialized particles" << std::endl;

    // Create SSBO for particles
    GLuint particleBuffer;
    glGenBuffers(1, &particleBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Particle) * NUM_PARTICLES, particles.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleBuffer);

   std::cout << "created particle buffer" << std::endl;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Update particles using compute shader
        glUseProgram(computeProgram);
        glUniform1f(glGetUniformLocation(computeProgram, "deltaTime"), 0.016f);
        glUniform2f(glGetUniformLocation(computeProgram, "bounds"), 1.0f, 1.0f);
        glDispatchCompute(NUM_PARTICLES / 256 + 1, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        std::cout << "updated particles" << std::endl;

        // Render particles
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(renderProgram);
        glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), nullptr);
        glEnableVertexAttribArray(0);
        glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);

         std::cout << "rendered particles" << std::endl;

        glfwSwapBuffers(window);
        glfwPollEvents();

        std::cout << "swapped buffers" << std::endl;
    }

    // Cleanup
    glDeleteProgram(computeProgram);
    glDeleteProgram(renderProgram);
    glDeleteBuffers(1, &particleBuffer);
    glfwTerminate();

    return 0;
}
