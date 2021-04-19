#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
using namespace std;

PFNGLGETSTRINGPROC glad_glGetString = NULL;
PFNGLVIEWPORTPROC glad_glViewport = NULL;

static int max_loaded_major;
static int max_loaded_minor;

static const char *exts = NULL;
static int num_exts_i = 0;
static char **exts_i = NULL;

struct gladGLversionStruct GLVersion = { 0, 0 };

static void free_exts(void) {
    if (exts_i != NULL) {
        int index;
        for(index = 0; index < num_exts_i; index++) {
            free((char *)exts_i[index]);
        }
        free((void *)exts_i);
        exts_i = NULL;
    }
}

static int has_ext(const char *ext) {
#ifdef _GLAD_IS_SOME_NEW_VERSION
    if(max_loaded_major < 3) {
#endif
        const char *extensions;
        const char *loc;
        const char *terminator;
        extensions = exts;
        if(extensions == NULL || ext == NULL) {
            return 0;
        }

        while(1) {
            loc = strstr(extensions, ext);
            if(loc == NULL) {
                return 0;
            }

            terminator = loc + strlen(ext);
            if((loc == extensions || *(loc - 1) == ' ') &&
                (*terminator == ' ' || *terminator == '\0')) {
                return 1;
            }
            extensions = terminator;
        }
#ifdef _GLAD_IS_SOME_NEW_VERSION
    } else {
        int index;
        if(exts_i == NULL) return 0;
        for(index = 0; index < num_exts_i; index++) {
            const char *e = exts_i[index];

            if(exts_i[index] != NULL && strcmp(e, ext) == 0) {
                return 1;
            }
        }
    }
#endif

    return 0;
}


static int get_exts(void) {
#ifdef _GLAD_IS_SOME_NEW_VERSION
    if(max_loaded_major < 3) {
#endif
        exts = (const char *)glGetString(GL_EXTENSIONS);
#ifdef _GLAD_IS_SOME_NEW_VERSION
    } else {
        unsigned int index;

        num_exts_i = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &num_exts_i);
        if (num_exts_i > 0) {
            exts_i = (char **)malloc((size_t)num_exts_i * (sizeof *exts_i));
        }

        if (exts_i == NULL) {
            return 0;
        }

        for(index = 0; index < (unsigned)num_exts_i; index++) {
            const char *gl_str_tmp = (const char*)glGetStringi(GL_EXTENSIONS, index);
            size_t len = strlen(gl_str_tmp);

            char *local_str = (char*)malloc((len+1) * sizeof(char));
            if(local_str != NULL) {
                memcpy(local_str, gl_str_tmp, (len+1) * sizeof(char));
            }
            exts_i[index] = local_str;
        }
    }
#endif
    return 1;
}


static int find_extensionsGL(void) {
	if (!get_exts()) return 0;
	(void)&has_ext;
	free_exts();
	return 1;
}

static void find_coreGL(void) {

    /* Thank you @elmindreda
     * https://github.com/elmindreda/greg/blob/master/templates/greg.c.in#L176
     * https://github.com/glfw/glfw/blob/master/src/context.c#L36
     */
    int i, major, minor;

    const char* version;
    const char* prefixes[] = {
        "OpenGL ES-CM ",
        "OpenGL ES-CL ",
        "OpenGL ES ",
        NULL
    };

    version = (const char*) glGetString(GL_VERSION);
    if (!version) return;

    for (i = 0;  prefixes[i];  i++) {
        const size_t length = strlen(prefixes[i]);
        if (strncmp(version, prefixes[i], length) == 0) {
            version += length;
            break;
        }
    }

/* PR #18 */
#ifdef _MSC_VER
    sscanf_s(version, "%d.%d", &major, &minor);
#else
    sscanf(version, "%d.%d", &major, &minor);
#endif

    GLVersion.major = major; GLVersion.minor = minor;
    max_loaded_major = major; max_loaded_minor = minor;
	GLAD_GL_VERSION_1_0 = (major == 1 && minor >= 0) || major > 1;
	GLAD_GL_VERSION_1_1 = (major == 1 && minor >= 1) || major > 1;
	GLAD_GL_VERSION_1_2 = (major == 1 && minor >= 2) || major > 1;
	GLAD_GL_VERSION_1_3 = (major == 1 && minor >= 3) || major > 1;
	GLAD_GL_VERSION_1_4 = (major == 1 && minor >= 4) || major > 1;
	GLAD_GL_VERSION_1_5 = (major == 1 && minor >= 5) || major > 1;
	GLAD_GL_VERSION_2_0 = (major == 2 && minor >= 0) || major > 2;
	GLAD_GL_VERSION_2_1 = (major == 2 && minor >= 1) || major > 2;
	GLAD_GL_VERSION_3_0 = (major == 3 && minor >= 0) || major > 3;
	GLAD_GL_VERSION_3_1 = (major == 3 && minor >= 1) || major > 3;
	GLAD_GL_VERSION_3_2 = (major == 3 && minor >= 2) || major > 3;
	GLAD_GL_VERSION_3_3 = (major == 3 && minor >= 3) || major > 3;
	GLAD_GL_VERSION_4_0 = (major == 4 && minor >= 0) || major > 4;
	GLAD_GL_VERSION_4_1 = (major == 4 && minor >= 1) || major > 4;
	GLAD_GL_VERSION_4_2 = (major == 4 && minor >= 2) || major > 4;
	GLAD_GL_VERSION_4_3 = (major == 4 && minor >= 3) || major > 4;
	GLAD_GL_VERSION_4_4 = (major == 4 && minor >= 4) || major > 4;
	GLAD_GL_VERSION_4_5 = (major == 4 && minor >= 5) || major > 4;
	GLAD_GL_VERSION_4_6 = (major == 4 && minor >= 6) || major > 4;
	if (GLVersion.major > 4 || (GLVersion.major >= 4 && GLVersion.minor >= 6)) {
		max_loaded_major = 4;
		max_loaded_minor = 6;
	}
}

int gladLoadGLLoader(GLADloadproc load) {
	GLVersion.major = 0; GLVersion.minor = 0;
	glGetString = (PFNGLGETSTRINGPROC)load("glGetString");
	if(glGetString == NULL) return 0;
	if(glGetString(GL_VERSION) == NULL) return 0;
//	find_coreGL();
//	load_GL_VERSION_1_0(load);
//	load_GL_VERSION_1_1(load);
//	load_GL_VERSION_1_2(load);
//	load_GL_VERSION_1_3(load);
//	load_GL_VERSION_1_4(load);
//	load_GL_VERSION_1_5(load);
//	load_GL_VERSION_2_0(load);
//	load_GL_VERSION_2_1(load);
//	load_GL_VERSION_3_0(load);
//	load_GL_VERSION_3_1(load);
//	load_GL_VERSION_3_2(load);
//	load_GL_VERSION_3_3(load);
//	load_GL_VERSION_4_0(load);
//	load_GL_VERSION_4_1(load);
//	load_GL_VERSION_4_2(load);
//	load_GL_VERSION_4_3(load);
//	load_GL_VERSION_4_4(load);
//	load_GL_VERSION_4_5(load);
//	load_GL_VERSION_4_6(load);

	if (!find_extensionsGL()) return 0;
	return GLVersion.major != 0 || GLVersion.minor != 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
        glViewport(0, 0, width, height);
    }


int main(int argc, char *argv[]){

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    /*
    * Create a window object here
    *
    */
    GLFWwindow* window = glfwCreateWindow(800, 600, "Ese Edited LearnOpenGL", NULL, NULL);
    if (window == NULL){
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    //Initializing glad before calling opengl function
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //innitialize a viewport window for size
    glViewport(0, 0, 800, 600);


    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    while(!glfwWindowShouldClose(window)){
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}




