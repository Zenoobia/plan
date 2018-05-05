#define GLEW_STATIC

#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>
#include <imguifilesystem.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
#include <string>
#include <vector>

/* macro to detect where GL errors happens */
#define CHECK_GL_ERROR __CheckForGLError(__FILE__, __LINE__)


using namespace std;
using namespace glm;


/* main functions */
bool Initialize();
void Render();
void CleanUp();
void __CheckForGLError(const char* filename, int line);
void APIENTRY OpenGLDebugCallback(
		GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userparam);

void OpenGLKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void OpenGLResizeCallback(GLFWwindow* window, int new_width, int new_height) {
	//glViewport(0, 0, m_window_width = new_width, m_window_height = new_height);
	glViewport(0, 0, new_width, new_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, new_width, new_height, 0.0, 0.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
}

/* variables */
GLuint vertexbuffer = 0;
GLuint texture = 0;


/* typical vertex format */
struct Vertex
{
	vec3 Position;
	vec2 TexCoord;
	vec3 Normal;
	vec3 Tangent;
	vec3 Bitangent;
	uvec4 NodeIndices;
	vec4 NodeWeights;
};

/* typical texel format */
struct Texel
{
	GLubyte R = 0;
	GLubyte G = 0;
	GLubyte B = 0;
	GLubyte A = 0;
};


/* data for the GL debug callback */
struct DebugUserParams {
	bool ShowNotifications = true;
} debuguserparams;


int main(int argc, char* argv[])
{
	/* Initialize the library */
	if (!glfwInit())
		return 1;

	bool fullscreen = false;
	bool debugcontext = true;

	/* Create a windowed mode window and its OpenGL context */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, debugcontext ? GLFW_TRUE : GLFW_FALSE);
	GLFWmonitor* monitor = fullscreen ? glfwGetPrimaryMonitor() : NULL;
	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", monitor, NULL);
	if (!window)
	{
		glfwTerminate();
		return 1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* Initialize GLEW */
	if (glewInit() != GLEW_OK)
	{
		glfwTerminate();
		return 1;
	}

	/* Register debug callback if debug context is available */
	GLint contextflags = 0;
	glGetIntegerv(GL_CONTEXT_FLAGS, &contextflags);
	if (contextflags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(OpenGLDebugCallback, &debuguserparams);
	}

	// Register key callback
	glfwSetKeyCallback(window, OpenGLKeyCallback);

  // Initialize ImGui
  ImGui_ImplGlfwGL3_Init(window, true);
  ImGui::StyleColorsDark();

	//if (Initialize())
	//Initialize();
	{
		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			Render();

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
	}
#if 0
	else {
		cin.get(); /* ... wait and show errors in console */
	}
#endif

	CleanUp();

	glfwTerminate();
	return 0;
}


bool Initialize()
{
	/* check if GL_NVX_gpu_memory_info is supported */
	GLint extensioncount = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &extensioncount);

	bool extension_available = false;
	for (int i = 0; i < extensioncount; i++)
	{
		string extension((const char*)glGetStringi(GL_EXTENSIONS, i));
		if (extension == "GL_NVX_gpu_memory_info")
		{
			extension_available = true;
			break;
		}
	}

	if (!extension_available)
	{
		cout << "GL_NVX_gpu_memory_info not supported" << endl;
		return false;
	}



	/*
	   --- quote from http://developer.download.nvidia.com/opengl/specs/GL_NVX_gpu_memory_info.txt ---

	   These will return the memory status of the context's associated GPU memory.
	   The status returned is not intended as an exact measurement of the
	   system's current status.  It will provide a approximate indicator of
	   the overall GPU memory utilization so that an application can determine
	   when the resource usage is approaching the full capacity of the GPU memory
	   and it may need to adjust its usage pattern to avoid performance limiting
	   swapping.  Each query returns a integer where the values have the
	   following meanings:

	   GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX
	   - dedicated video memory, total size (in kb) of the GPU memory

	   GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX
	   - total available memory, total size (in Kb) of the memory
	   available for allocations

	   GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX
	   - current available dedicated video memory (in kb),
	   currently unused GPU memory

	   GPU_MEMORY_INFO_EVICTION_COUNT_NVX
	   - count of total evictions seen by system

	   GPU_MEMORY_INFO_EVICTED_MEMORY_NVX
	   - size of total video memory evicted (in kb)
	   */


	/* query current GPU memory */
	struct
	{
		/* each in kB */
		GLint mem_info_dedicated_vidmem = 0;
		GLint mem_info_total_available_memory = 0;
		GLint mem_info_current_available_memory = 0;
		GLint mem_info_eviction_count = 0;
		GLint mem_info_evicted_memory = 0;
	} memory_before, memory_after;


	glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX,            &memory_before.mem_info_dedicated_vidmem);
	glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX,    &memory_before.mem_info_total_available_memory);
	glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX,    &memory_before.mem_info_current_available_memory);
	glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX,            &memory_before.mem_info_eviction_count);
	glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX,            &memory_before.mem_info_evicted_memory);


	// create all objects
	glCreateBuffers(1, &vertexbuffer);
	glCreateTextures(GL_TEXTURE_2D, 1, &texture);

	/* setup vertex buffer (just 1 million vertices) */
	vector<Vertex> vertices(1000000);
	glNamedBufferData(vertexbuffer, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	/* setup texture (4096 x 4096 rgba8) */
	vector<Texel> texels(4096 * 4096);
	glTextureStorage2D(texture, 1, GL_RGBA8, 4096, 4096);
	glTextureSubImage2D(texture, 0, 0, 0, 4096, 4096, GL_RGBA, GL_UNSIGNED_BYTE, texels.data());
	glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX,            &memory_after.mem_info_dedicated_vidmem);
	glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX,    &memory_after.mem_info_total_available_memory);
	glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX,    &memory_after.mem_info_current_available_memory);
	glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTION_COUNT_NVX,            &memory_after.mem_info_eviction_count);
	glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX,            &memory_after.mem_info_evicted_memory);


	/* what we expect: */
	GLint mem_consumption_expected = (sizeof(Vertex) * 1000000 + 4096 * 4096 * 4) / 1024;


	cout
		<< "BEFORE creation of resources:" << endl
		<< "mem_info_dedicated_vidmem: \t"            << memory_before.mem_info_dedicated_vidmem << " kB" << endl
		<< "mem_info_total_available_memory: \t"    << memory_before.mem_info_total_available_memory << " kB" << endl
		<< "mem_info_current_available_memory: \t"    << memory_before.mem_info_current_available_memory << " kB" << endl
		<< "mem_info_eviction_count: \t"            << memory_before.mem_info_eviction_count << " kB" << endl
		<< "mem_info_evicted_memory: \t"            << memory_before.mem_info_evicted_memory << " kB" << endl
		<< "AFTER creation of resources:" << endl
		<< "mem_info_dedicated_vidmem: \t"            << memory_after.mem_info_dedicated_vidmem << " kB" << endl
		<< "mem_info_total_available_memory: \t"    << memory_after.mem_info_total_available_memory << " kB" << endl
		<< "mem_info_current_available_memory: \t"    << memory_after.mem_info_current_available_memory << " kB" << endl
		<< "mem_info_eviction_count: \t"            << memory_after.mem_info_eviction_count << " kB" << endl
		<< "mem_info_evicted_memory: \t"            << memory_after.mem_info_evicted_memory << " kB" << endl
		<< "EXPECTED:" << endl
		<< "mem_consumption_expected: \t"            << mem_consumption_expected << " kB" << endl
		<< "what the extension gives us: \t"
		<< (memory_before.mem_info_current_available_memory - memory_after.mem_info_current_available_memory) << " kB" << endl
		<< endl;


	CHECK_GL_ERROR;


	return true;
}


void Render()
{
	/* clear framebuffer */
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	/* check for any error */
	CHECK_GL_ERROR;
}


void CleanUp()
{
	// destroy all objects
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteTextures(1, &texture);

	CHECK_GL_ERROR;
}


void __CheckForGLError(const char* filename, int line)
{
	for (GLenum error; (error = glGetError()) != GL_NO_ERROR;)
	{
		cout << (char)7 << "OpenGL Error:\t" << filename << "  line " << line << "\n\t";
		if (error == GL_INVALID_ENUM)
			cout << "GL_INVALID_ENUM";
		if (error == GL_INVALID_VALUE)
			cout << "GL_INVALID_VALUE";
		if (error == GL_INVALID_OPERATION)
			cout << "GL_INVALID_OPERATION";
		if (error == GL_STACK_OVERFLOW)
			cout << "GL_STACK_OVERFLOW";
		if (error == GL_STACK_UNDERFLOW)
			cout << "GL_STACK_UNDERFLOW";
		if (error == GL_OUT_OF_MEMORY)
			cout << "GL_OUT_OF_MEMORY";
		if (error == GL_INVALID_FRAMEBUFFER_OPERATION)
			cout << "GL_INVALID_FRAMEBUFFER_OPERATION";
		if (error == GL_CONTEXT_LOST)
			cout << "GL_CONTEXT_LOST";
		cout << endl << endl;
		cin.get();
	}
}


/* debug */
void APIENTRY OpenGLDebugCallback(
		GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userparam)
{
	/* debug user params */
	DebugUserParams* params = (DebugUserParams*)userparam;

	/* filter out unnecessary warnings */
	if (!params->ShowNotifications)
		if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
			return;

	/* source */
	string str_source;
	if (source == GL_DEBUG_SOURCE_API) str_source = "API";
	if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM) str_source = "Window System";
	if (source == GL_DEBUG_SOURCE_SHADER_COMPILER) str_source = "Shader Compiler";
	if (source == GL_DEBUG_SOURCE_THIRD_PARTY) str_source = "Third Party";
	if (source == GL_DEBUG_SOURCE_APPLICATION) str_source = "Application";
	if (source == GL_DEBUG_SOURCE_OTHER) str_source = "Other";

	/* type */
	string str_type;
	if (type == GL_DEBUG_TYPE_ERROR) str_type = "Error";
	if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR) str_type = "Deprecated Behavior";
	if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR) str_type = "Undefined Behavior";
	if (type == GL_DEBUG_TYPE_PORTABILITY) str_type = "Portability";
	if (type == GL_DEBUG_TYPE_PERFORMANCE) str_type = "Performance";
	if (type == GL_DEBUG_TYPE_MARKER) str_type = "Marker";
	if (type == GL_DEBUG_TYPE_PUSH_GROUP) str_type = "Push Group";
	if (type == GL_DEBUG_TYPE_POP_GROUP) str_type = "Pop Group";
	if (type == GL_DEBUG_TYPE_OTHER) str_type = "Other";

	/* severity */
	string str_severity;
	if (severity == GL_DEBUG_SEVERITY_HIGH) str_severity = "High";
	if (severity == GL_DEBUG_SEVERITY_MEDIUM) str_severity = "Medium";
	if (severity == GL_DEBUG_SEVERITY_LOW) str_severity = "Low";
	if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) str_severity = "Notification";

	/* print message */
	cout << "OpenGL Debug Message:" << endl;
	cout << "----------------------------------" << endl;
	cout << "ID: \t\t" << id << endl;
	cout << "Source: \t" << str_source << endl;
	cout << "Type: \t\t" << str_type << endl;
	cout << "Severity: \t" << str_severity << endl;
	cout << "Message: \t" << message << endl;
	cout << "----------------------------------" << endl << endl;
}
