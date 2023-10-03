//----------------------------------------------------------------------------
// opengl-tutorial03 - HW1 
// + changes to allow multiple triangles with different colors
// + added right triangle, square, cube
//
// Robert Perez, rjperez@udel.edu
// Spring 2023
//----------------------------------------------------------------------------

// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>

//----------------------------------------------------------------------------

// these along with Model matrix make MVP transform

glm::mat4 Projection;
glm::mat4 View;

// some globals necessary to get information to shaders

GLuint MatrixID;
GLuint vertexbuffer;
GLuint colorbuffer;

void draw_triangle(glm::mat4, float, float, float);
void draw_right_triangle(glm::mat4, float, float, float);
void draw_square(glm::mat4, float, float, float);
void draw_cube(glm::mat4, float, float, float);

//----------------------------------------------------------------------------

// 2 x 2 x 2 cube centered on (0, 0, 0)

void draw_cube(glm::mat4 Model, float r, float g, float b)
{
  // +Z, -Z
  
  draw_square(Model * glm::translate(glm::vec3(0.0f, 0.0f, +1.0f)), r, g, b);
  draw_square(Model * glm::translate(glm::vec3(0.0f, 0.0f, -1.0f)), 0.5*r, 0.5*g, 0.5*b);

  // +X, -X

  glm::mat4 RY = glm::rotate((float) (0.5*M_PI),
                 glm::vec3(    0.0f,
                    1.0f,
                    0.0f));

  draw_square(Model * glm::translate(glm::vec3(+1.0f, 0.0f, 0.0f)) * RY, g, b, r);
  draw_square(Model * glm::translate(glm::vec3(-1.0f, 0.0f, 0.0f)) * RY, 0.5*g, 0.5*b, 0.5*r);

  // +Y, -Y

  glm::mat4 RX = glm::rotate((float) (0.5*M_PI),
                 glm::vec3(    1.0f,
                    0.0f,
                    0.0f));

  draw_square(Model * glm::translate(glm::vec3(0.0f, +1.0f, 0.0f)) * RX, b, r, g);
  draw_square(Model * glm::translate(glm::vec3(0.0f, -1.0f, 0.0f)) * RX, 0.5*b, 0.5*r, 0.5*g);

}

//----------------------------------------------------------------------------

// 2 x 2 square centered on (0, 0)

void draw_square(glm::mat4 Model, float r, float g, float b)
{
  glm::mat4 M = glm::scale(glm::vec3(-1.0f, -1.0f, 0.0f));

  //  draw_right_triangle(Model * M, 1.0-r, 1.0-g, 1.0-b);
  draw_right_triangle(Model * M, r, g, b);
  draw_right_triangle(Model, r, g, b);
}

//----------------------------------------------------------------------------

// with shear, bottom-left at (-1, -1), bottom-right at (1, -1),
// top-right at (1, 1)

void draw_right_triangle(glm::mat4 Model, float r, float g, float b)
{
  glm::mat4 S = glm::shearX3D (glm::mat4(1.0f), 0.5f, 0.0f);
  glm::mat4 T = glm::translate(glm::vec3(-1.0f, 1.0f, 0.0f));
  
  draw_triangle(Model * glm::inverse(T) * S * T, r, g, b);
}

//----------------------------------------------------------------------------

// bottom-left at (-1, -1), bottom-right at (1, -1),
// top at (0, 1)

// Draw triangle with particular modeling transformation and color (r, g, b) (in range [0, 1])
// Refers to globals in section above (but does not change them)

void draw_triangle(glm::mat4 Model, float r, float g, float b)
{
  // Our ModelViewProjection : multiplication of our 3 matrices
  glm::mat4 MVP = Projection * View * Model;

  // make this transform available to shaders
  glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

  // 1st attribute buffer : vertices
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glVertexAttribPointer(0,                  // attribute. 0 to match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
            );
  
  // all vertices same color

  GLfloat g_color_buffer_data[] = {
    r, g, b,
    r, g, b,
    r, g, b,
  };
  
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
  
  // 2nd attribute buffer : colors
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
  glVertexAttribPointer(1,                                // attribute. 1 to match the layout in the shader.
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
            );

  // Draw the triangle !
  glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle
  
  glDisableVertexAttribArray(0);
  glDisableVertexAttribArray(1);
}

// Draw Tank
void draw_tank(glm::mat4 Model, float r, float g, float b, float scale_factor)
{
  draw_cube(Model * glm::scale(glm::vec3(1.0f, scale_factor * 0.6f, 0.8f)), 0.5f, 0.5f, 0.5f);

  // Draws the turret
  float body_height = 0.6f * scale_factor;
  float turret_height = 0.7f * scale_factor;
  glm::mat4 T = glm::translate(glm::vec3(0.0f, body_height * 0.5f + turret_height * 0.5f + 0.05f * scale_factor, 0.0f));
  draw_cube(Model * T * glm::scale(glm::vec3(scale_factor * 0.72f, scale_factor * 0.6f, scale_factor * 0.8f)), 1.0f, 1.0f, 1.0f);

  // Draws the gun
  float gun_length = 0.8f * scale_factor;
  glm::mat4 G = glm::translate(glm::vec3(scale_factor * 0.72f + gun_length * 0.5f, 0.2f * scale_factor, 0.0f));
  draw_cube(Model * T * G * glm::scale(glm::vec3(gun_length * 2.0f, scale_factor * 0.3f, scale_factor * 0.3f)), 1.0f, 1.0f, 1.0f);

  // Draws the hatch
  glm::mat4 A1 = glm::translate(glm::vec3(scale_factor * 0.4f, turret_height * 0.5f + 0.05f * scale_factor, scale_factor * 0.4f));
  draw_cube(Model * T * A1 * glm::scale(glm::vec3(scale_factor * 0.3f, scale_factor * 0.3f, scale_factor * 0.3f)), 0.5f, 0.5f, 0.5f);

  // Draws the antenna
  glm::mat4 A2 = glm::translate(glm::vec3(scale_factor * -0.4f, turret_height * 0.5f + 0.05f * scale_factor, scale_factor * -0.4f));
  draw_cube(Model * T * A2 * glm::scale(glm::vec3(scale_factor * 0.05f, scale_factor * 2.0f, scale_factor * 0.05f)), 0.5f, 0.5f, 0.5f);
}
//draws a circular pattern of tanks in which each tank is slighly rotating
void draw_pattern() {
  const int num_tanks = 10;
  const float radius = 5.0f;
  const float height = 0.0f;
  const glm::vec3 center = glm::vec3(0.0f, height, 0.0f);
    
  for (int i = 0; i < num_tanks; i++) {//Produces the tanks for size of circle
    float angle = 2.0f * glm::pi<float>() * i / num_tanks;
    float x = radius * std::cos(angle);
    float z = radius * std::sin(angle);
    float r = (float)rand() / RAND_MAX;
    float g = (float)rand() / RAND_MAX;
    float b = (float)rand() / RAND_MAX;

    glm::vec3 tank_pos = glm::vec3(x, height, z);
    glm::vec3 direction = glm::normalize(center - tank_pos);
    float yaw = atan2(direction.z, direction.x) + glm::pi<float>();

    draw_tank(glm::translate(tank_pos) * glm::rotate(yaw, glm::vec3(0.0f, 1.0f, 0.0f)), r, g, b, 0.5f);
  }
}
//----------------------------------------------------------------------------

int main( void )
{
  // Initialise GLFW
  if( !glfwInit() )
    {
      fprintf( stderr, "Failed to initialize GLFW\n" );
      getchar();
      return -1;
    }
  
  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL
  
  // Open a window and create its OpenGL context
  window = glfwCreateWindow( 1024, 768, "Tutorial 03 - Matrices", NULL, NULL);
  if( window == NULL ){
    fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
    getchar();
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  
  // Initialize GLEW
  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
    return -1;
  }
  
  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  
  // Darker blue background
  glClearColor(0.0f, 0.0f, 0.2f, 0.0f);
  glEnable(GL_DEPTH_TEST);

  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);
  
  // Create and compile our GLSL program from the shaders
  GLuint programID = LoadShaders( "MultiColorSimpleTransform.vertexshader", "MultiColor.fragmentshader" );
  
  // Get a handle for our "MVP" uniform
  MatrixID = glGetUniformLocation(programID, "MVP");
  
  // Projection matrix : 45¡ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 200.0f);
  // Or, for an ortho camera :
  //  Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In camera coordinates
  //  Projection = glm::ortho(-3.0f,3.0f,-3.0f,3.0f,0.0f,100.0f); // In camera coordinates
  
  // Camera matrix -- same for all triangles drawn
  View       = glm::lookAt(glm::vec3(4,3,3), // Camera is at (4,3,3), in World Space
               glm::vec3(0,0,0), // and looks at the origin
               glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
               );
  
  // geometry of "template" triangle

  static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f,  1.0f, 0.0f,
  };
  static const GLushort g_element_buffer_data[] = { 0, 1, 2 };
  
  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
  
  // handle for color information.  we don't set it here because it can change for each triangle

  glGenBuffers(1, &colorbuffer);
  
  // Model matrix -- changed for each triangle drawn
  glm::mat4 Model;
  
  do{
    
    // Clear the screen
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Use our shader
    glUseProgram(programID);
    
      
//Use v to switch between views
      bool isIsometricView = false;

      do {
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

          glUseProgram(programID);

          // Switch between views
          if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
              isIsometricView = !isIsometricView;
          }

          if (isIsometricView) {
              View = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f);
          } else {
              View = glm::lookAt(glm::vec3(0, 10, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, -1));
          }

          // Draw pattern
          draw_pattern();

          // Swap buffers
          glfwSwapBuffers(window);
          glfwPollEvents();

      } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
               glfwWindowShouldClose(window) == 0);



    // more/other calls to draw_triangle() ...
      //HOW TO DRAW MY OBJECTS
      // draw the tank
      //draw_tank(glm::mat4(1.0f), 0.5f, 0.5f, 0.5f, 0.5f);
      //draw the pattern
      //draw_pattern();
      
      
      
 // green, scaled by 0.5
    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

  } // Check if the ESC key was pressed or the window was closed
  while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
     glfwWindowShouldClose(window) == 0 );
  
  // Cleanup VBO and shader
  glDeleteBuffers(1, &vertexbuffer);
  glDeleteProgram(programID);
  glDeleteVertexArrays(1, &VertexArrayID);
  
  // Close OpenGL window and terminate GLFW
  glfwTerminate();
  
  return 0;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

