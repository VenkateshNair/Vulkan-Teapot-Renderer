
//headers
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include"stb_image.h"

#include "VK.h"
#include"Teapot.h"


// vulkan related header files
#define VK_USE_PLATFORM_WIN32_KHR
#include<vulkan/vulkan.h>


// glm related macros and header files
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE


#include"glm/glm.hpp" //hpp - cpp specific
#include "glm/gtc/matrix_transform.hpp" 


// vulkan related libraries
#pragma comment(lib,"vulkan-1.lib")

//define
#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define INFO_LOG "[INFO]  :"
#define ERROR_LOG "[ERROR]:"




//global function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


//global variables
HWND ghwnd = NULL;
BOOL bFullScreen = FALSE;
BOOL bWindowMinimized = FALSE;
WINDOWPLACEMENT wpPrev;
DWORD dwStyle;


FILE* gpFile = NULL;
BOOL gbActiveWindow = FALSE;
const char* gpszAppName = "ARTR";


// vulkan related global variables
// instance extension related variables
uint32_t enableInstanceExtensionCount = 0;                      // 32 bit type 
const char* enabledInstanceExtensionNames_array[3];             // VK_KHR_SURFACE_EXTENSION_NAME and VK_KHR_WIN32_SURFACE_EXTENSION_NAME and VK_EXT_DEBUG_REPORT_EXTENSION_NAME 
VkInstance vkInstance = VK_NULL_HANDLE;


// vulkan presentation surface
VkSurfaceKHR vkSurfaceKHR = VK_NULL_HANDLE;


// vulkan physical device related
VkPhysicalDevice vkPhysicalDevice_selected = VK_NULL_HANDLE;
uint32_t graphicsQueueFamilyIndex_Selected = UINT32_MAX;
VkPhysicalDeviceMemoryProperties vkPhysicalDeviceMemoryProperties;


uint32_t physicalDeviceCount = 0;
VkPhysicalDevice* vkPhysicalDevice_array = NULL;


// instance extension related variables
uint32_t enableDeviceExtensionCount = 0;                      // 32 bit type 
const char* enabledDeviceExtensionNames_array[1];             // VK_KHR_SWAPCHAIN_EXTENSION_NAME


//vulkan device
VkDevice vkdevice = VK_NULL_HANDLE;


//device queue
VkQueue vkQueue = VK_NULL_HANDLE;


//color format and color space
VkFormat vkFormat_color = VK_FORMAT_UNDEFINED;
VkColorSpaceKHR vkColorSpaceKHR = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;


VkPresentModeKHR vkPresentModeKHR = VK_PRESENT_MODE_FIFO_KHR;


// swapchain related variables
int winWidth = WIN_WIDTH;
int winHeight = WIN_HEIGHT;
VkSwapchainKHR vkSwapchainKHR = VK_NULL_HANDLE;
VkExtent2D vkExtent2D_swapchain; //struct




//swapchain images and imageview 
uint32_t swapchainImageCount = UINT32_MAX;
VkImage* swapchainImage_array = NULL;
VkImageView* swapchainImageView_array = NULL;


// for depth image
VkImage vkImage_Depth = VK_NULL_HANDLE;
VkDeviceMemory vkDeviceMemory_Depth = VK_NULL_HANDLE;
VkImageView VkImageView_Depth = VK_NULL_HANDLE;
VkFormat vkFormat_Depth = VK_FORMAT_UNDEFINED;




// command pool
VkCommandPool vkCommandPool = VK_NULL_HANDLE;


VkCommandBuffer* vkCommandBuffer_array = NULL;


// renderpass
VkRenderPass vkRenderPass = VK_NULL_HANDLE;


// framebuffer
VkFramebuffer* vkFramebuffers_array = NULL;


// Semaphore
VkSemaphore vkSemaphore_backbuffer = VK_NULL_HANDLE;
VkSemaphore vkSemaphore_rendercomplete = VK_NULL_HANDLE;


// VkFence
VkFence* vkFence_array = NULL;


// clear color value
VkClearColorValue vkClearColorValue; // float32[4] will be filled
VkClearDepthStencilValue vkClearDepthStencilValue;
// render
BOOL bInitialized = FALSE;
uint32_t currentImageIndex = UINT32_MAX;


//validation
BOOL bValidation = TRUE;
uint32_t enabledValidationLayerCount = 0;
const char* enabledValidationLayerNames_array[1]; // for VK_LAYER_KHRONOS_validation
VkDebugReportCallbackEXT vkDebugReportCallbackEXT;
PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT_fnptr = NULL;


//shader related variable
VkShaderModule vkShaderModule_vertex_shader = VK_NULL_HANDLE;
VkShaderModule vkShaderModule_fragment_shader = VK_NULL_HANDLE;


// descriptorSetLayout
VkDescriptorSetLayout vkDescriptorSetLayout = VK_NULL_HANDLE;


//pipelinelayout
VkPipelineLayout vkPipelineLayout = VK_NULL_HANDLE;


// descriptor pool
VkDescriptorPool vkDescriptorPool = VK_NULL_HANDLE;
// descriptor set
VkDescriptorSet vkDescriptorSet = VK_NULL_HANDLE;




VkViewport vkViewport;
VkRect2D vkRect2D_scissor; // to enable scissor test , it sets what needs to be cropped
VkPipeline vkPipeline = VK_NULL_HANDLE;

unsigned int numVertices;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
 
// vertex buffer related variables
typedef struct {
	VkBuffer vkBuffer;
	VkDeviceMemory vkDeviceMemory;
}vertexData;


//position
vertexData vertexData_position;
vertexData vertexData_color;
vertexData vertexData_normal;
vertexData vertexData_texcord;
vertexData vertexData_index;

// uniform related declarations
struct MyUniformData {
	// matrices related uniform
	glm::mat4 modelMatrix;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;

	// light related uniform
	float lightDiffuse[4];
	float lightPosition[4];
	float materialDiffuse[4];

	//key pressed related uniform
	unsigned int lKeyIsPressed;
	unsigned int padding_l[3];  // Pad to 16-byte alignment


	unsigned int tKeyIsPressed;
	unsigned int padding_t[3];  // Pad to 16-byte alignment
};


struct UniformData {
	VkBuffer vkBuffer;
	VkDeviceMemory vkDeviceMemory;
};
UniformData uniformData;


// for rotation
BOOL bLight = FALSE;


float* pPositions = NULL;
float* pNormals = NULL;
float* pTexCoords = NULL;
unsigned int* pElements = NULL;

unsigned int numFaceIndices = 0;
unsigned int numElements = 0;
unsigned int numVerts = 0;

float angleTeapot = 0.0f;
BOOL bAnimate = FALSE;
BOOL bTexture = FALSE;


// texture related variables
VkImage vkImage_texture = VK_NULL_HANDLE;
VkDeviceMemory vkDeviceMemory_texture = VK_NULL_HANDLE;
VkImageView vkImageView_texture = VK_NULL_HANDLE;
VkSampler vkSampler_texture = VK_NULL_HANDLE;

//WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//function declaration
	VkResult initialize(void);
	VkResult display(void);
	void uninitialize(void);
	void update(void);


	//local variables
	BOOL bDone = FALSE;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[256];
	WNDCLASSEX wndclass;
	int ixCenter = (GetSystemMetrics(SM_CXSCREEN) / 2) - 400;
	int iyCenter = (GetSystemMetrics(SM_CYSCREEN) / 2) - 300;
	VkResult vkResult = VK_SUCCESS;


	gpFile = fopen("VMNlog.txt", "w");
	if (gpFile == NULL)
	{
		MessageBox(NULL, TEXT("Failed to create Log File"), TEXT("Message Box"), MB_OK);
	}
	else
	{
		wsprintf(szAppName, TEXT("%s"), gpszAppName);
	}


	//initialization of WndClass


	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hInstance = hInstance;
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);


	RegisterClassEx(&wndclass);


	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("Windows : Vulkan 1 : By Venkatesh Nair"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		ixCenter,
		iyCenter,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);


	ghwnd = hwnd;
	vkResult = initialize();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s WinMain :: initialize() is failed \n", ERROR_LOG);
		DestroyWindow(hwnd);
		hwnd = NULL;
	}
	else
	{
		fprintf(gpFile, "%s WinMain :: initialize() succeeded \n", INFO_LOG);
	}
	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);


	//game loop
	while (bDone == FALSE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDone = TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == TRUE)
			{
				if (bWindowMinimized == FALSE)
				{
					vkResult = display();


					if (vkResult != VK_FALSE &&
						vkResult != VK_SUCCESS &&
						vkResult != VK_ERROR_OUT_OF_DATE_KHR &&
						vkResult != VK_SUBOPTIMAL_KHR)   //maybe we are still in resize() even if display has bInitialized = TRUE
					{
						fprintf(gpFile, "%s WinMain :: display() failed \n", ERROR_LOG);
						bDone = TRUE;
					}
					if (bAnimate == TRUE)
					{
						update();
					}
				}
			}
		}
	}


	uninitialize();
	return((int)msg.wParam);
}


//definition of WndProc
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//function declarations
	void ToogleFullScreen(void);
	VkResult resize(int, int);




	switch (iMsg)
	{
	case WM_SETFOCUS:
		gbActiveWindow = TRUE;
		break;


	case WM_KILLFOCUS:
		gbActiveWindow = FALSE;
		break;


	case WM_ERASEBKGND:
		break;


	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
		{
			bWindowMinimized = TRUE;
		}
		else
		{
			bWindowMinimized = FALSE;
			resize(LOWORD(lParam), HIWORD(lParam));
		}
		break;


	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_CREATE:
		fprintf(gpFile, "%s WM_CREATE :: Window Created \n", INFO_LOG);
		memset(&wpPrev, 0, sizeof(WINDOWPLACEMENT));
		wpPrev.length = sizeof(WINDOWPLACEMENT);
		break;

	case WM_CHAR:
		switch (wParam)
		{
		case 'L':
		case 'l':
			bLight = !bLight;
			break;
		case 'A':
		case 'a':
			if (bAnimate == FALSE)
			{
				bAnimate = TRUE;
			}
			else
			{
				bAnimate = FALSE;
			}
			break;
		case 'T':
		case 't':
			bTexture = !bTexture;
			break;
		default:
			break;
		}
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;
		case 0x046:
		case 0x066:
			ToogleFullScreen();
			break;
		default:
			break;
		}
		break;
	case WM_DESTROY:
		fprintf(gpFile, "%s Window Destroyed \n", INFO_LOG);
		PostQuitMessage(0);
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}


void ToogleFullScreen(void)
{
	MONITORINFO mi = { sizeof(MONITORINFO) };
	if (bFullScreen == FALSE)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wpPrev) &&
				GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd,
					HWND_TOP,
					mi.rcMonitor.left,
					mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
		bFullScreen = TRUE;
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd,
			HWND_TOP,
			0, 0, 0, 0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
		ShowCursor(TRUE);
		bFullScreen = FALSE;
	}
}


VkResult initialize(void)
{
	//function declaration
	VkResult createVulkanInstance(void);
	VkResult getSupportedSurface(void);
	VkResult getPhysicalDevice(void);
	VkResult printVkInfo(void);
	VkResult createVulkanDevice(void);
	void getDeviceQueue(void);
	VkResult createSwapchain(VkBool32);
	VkResult createImagesAndImageViews(void);
	VkResult createCommandPool(void);
	VkResult createCommandBuffers(void);
	void addTriangle(float single_vertex[3][3], float single_normal[3][3], float single_texCoord[3][2]);
	VkResult createVertexBuffers(void);
	VkResult createTexture(const char*);
	VkResult createIndexBuffer(void);
	VkResult createUniformBuffer(void);
	VkResult createShaders(void);
	VkResult createDescriptorSetLayout(void);
	VkResult createPipelineLayout(void);
	VkResult createDescriptorPool(void);
	VkResult createDescriptorSet(void);
	VkResult createRenderPass(void);
	VkResult createPipeline(void);
	VkResult createFramebuffers(void);
	VkResult createSemaphores(void);
	VkResult createFences(void);
	VkResult buildCommandBuffers(void);






	VkResult vkResult = VK_SUCCESS;


	// code
	vkResult = createVulkanInstance();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s initialize :: createVulkanInstance() is failed \n", ERROR_LOG);
	}
	else
	{
		fprintf(gpFile, "%s initialize :: createVulkanInstance() succeeded \n", INFO_LOG);
	}


	// create vulkan presentation surface
	vkResult = getSupportedSurface();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s initialize :: getSupportedSurface() is failed \n", ERROR_LOG);
	}
	else
	{
		fprintf(gpFile, "%s initialize :: getSupportedSurface() succeeded \n", INFO_LOG);
	}


	// set required selected device and its queue family index
	vkResult = getPhysicalDevice();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s initialize :: getPhysicalDevice() is failed \n", ERROR_LOG);
	}
	else
	{
		fprintf(gpFile, "%s initialize :: getPhysicalDevice() succeeded \n", INFO_LOG);
	}


	vkResult = printVkInfo();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s initialize :: printVkInfo() is failed \n", ERROR_LOG);
	}
	else
	{
		fprintf(gpFile, "%s initialize :: printVkInfo() succeeded \n", INFO_LOG);
	}


	vkResult = createVulkanDevice();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s initialize :: createVulkanDevice() is failed \n", ERROR_LOG);
	}
	else
	{
		fprintf(gpFile, "%s initialize :: createVulkanDevice() succeeded \n", INFO_LOG);
	}
	getDeviceQueue();




	vkResult = createSwapchain(VK_FALSE);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s initialize :: createSwapChain() is failed . \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s initialize :: createSwapChain() succeeded  \n", INFO_LOG);
	}


	vkResult = createImagesAndImageViews();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s initialize :: createImagesAndImageViews() is failed . \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s initialize :: createImagesAndImageViews() succeeded  \n", INFO_LOG);
	}


	vkResult = createCommandPool();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s initialize :: createCommandPool() is failed . \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s initialize :: createCommandPool() succeeded  \n", INFO_LOG);
	}


	vkResult = createCommandBuffers();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s initialize :: createCommandBuffers() is failed . \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s initialize :: createCommandBuffers() succeeded  \n", INFO_LOG);
	}


	numFaceIndices = sizeof(face_indicies) / sizeof(face_indicies[0]);
	//position
	pPositions = (float*)malloc(numFaceIndices * 3 * sizeof(float));
	//normal
	pNormals = (float*)malloc(numFaceIndices * 3 * sizeof(float));
	//texcord
	pTexCoords = (float*)malloc(numFaceIndices * 2 * sizeof(float));
	//elements
	pElements = (unsigned int*)malloc(numFaceIndices * 3 * sizeof(unsigned int));

	//declare temp arrays to hold single triangle vertices
	float vert[3][3];
	float norm[3][3];
	float tex[3][2];

	for (int i = 0; i < (int)numFaceIndices; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			vert[j][0] = vertices[face_indicies[i][j + 0]][0];
			vert[j][1] = vertices[face_indicies[i][j + 0]][1];
			vert[j][2] = vertices[face_indicies[i][j + 0]][2];

			norm[j][0] = normals[face_indicies[i][j + 3]][0];
			norm[j][1] = normals[face_indicies[i][j + 3]][1];
			norm[j][2] = normals[face_indicies[i][j + 3]][2];

			tex[j][0] = textures[face_indicies[i][j + 6]][0];
			tex[j][1] = textures[face_indicies[i][j + 6]][1];
		}
		addTriangle(vert, norm, tex);
	}



	vkResult = createVertexBuffers();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s initialize :: createVertexBuffers() is failed . \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s initialize :: createVertexBuffers() succeeded  \n", INFO_LOG);
	}

	vkResult = createIndexBuffer();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s initialize :: createIndexBuffer() is failed . \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s initialize :: createIndexBuffer() succeeded  \n", INFO_LOG);
	}


	vkResult = createTexture("stone.png");
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s initialize :: createTexture() is failed . \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s initialize :: createTexture() succeeded  \n", INFO_LOG);
	}


	vkResult = createUniformBuffer();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s initialize :: createUniformBuffer() is failed . \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s initialize :: createUniformBuffer() succeeded  \n", INFO_LOG);
	}


	vkResult = createShaders();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s initialize :: createShaders() is failed . \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s initialize :: createShaders() succeeded  \n", INFO_LOG);
	}




	vkResult = createDescriptorSetLayout();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s initialize :: createDescriptorSetLayout() is failed . \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s initialize :: createDescriptorSetLayout() succeeded  \n", INFO_LOG);
	}


	vkResult = createPipelineLayout();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s initialize :: createPipelineLayout() is failed . \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s initialize :: createPipelineLayout() succeeded  \n", INFO_LOG);
	}


	vkResult = createDescriptorPool();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s initialize :: createDescriptorPool() is failed . \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s initialize :: createDescriptorPool() succeeded  \n", INFO_LOG);
	}


	vkResult = createDescriptorSet();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s initialize :: createDescriptorSet() is failed . \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s initialize :: createDescriptorSet() succeeded  \n", INFO_LOG);
	}


	vkResult = createRenderPass();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s initialize :: createRenderPass() is failed . \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s initialize :: createRenderPass() succeeded  \n", INFO_LOG);
	}


	vkResult = createPipeline();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s initialize :: createPipeline() is failed . \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s initialize :: createPipeline() succeeded  \n", INFO_LOG);
	}


	vkResult = createFramebuffers();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s initialize :: createFramebuffers() is failed . \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s initialize :: createFramebuffers() succeeded  \n", INFO_LOG);
	}


	vkResult = createSemaphores();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s initialize :: createSemaphores() is failed . \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s initialize :: createFramebuffers() succeeded  \n", INFO_LOG);
	}


	vkResult = createFences();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s initialize :: createFences() is failed . \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s initialize :: createFences() succeeded  \n", INFO_LOG);
	}


	// initialize clearcolor values
	memset((void*)&vkClearColorValue, 0, sizeof(VkClearColorValue));
	memset((void*)&vkClearDepthStencilValue, 0, sizeof(VkClearDepthStencilValue));
	vkClearColorValue.float32[0] = 0.0f;  // Red
	vkClearColorValue.float32[1] = 0.0f;  // Green
	vkClearColorValue.float32[2] = 0.0f;  // Blue
	vkClearColorValue.float32[3] = 1.0f;  // Alpha (opacity).


	vkClearDepthStencilValue.depth = 1.0f; //float


	// set default stencil value
	vkClearDepthStencilValue.stencil = 0;




	vkResult = buildCommandBuffers();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s initialize :: buildCommandBuffers() is failed . \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s initialize :: buildCommandBuffers() succeeded  \n", INFO_LOG);
	}


	// intialization is complete
	bInitialized = TRUE;


	return vkResult;
}


VkResult display(void)
{
	// function declaration
	VkResult resize(int, int);
	VkResult updateUniformBuffer(void);


	// variable declaration
	VkResult vkResult = VK_SUCCESS;


	// if control comes here before initialization is completed , return false
	if (bInitialized == FALSE)
	{
		fprintf(gpFile, "%s display :: initialization yet not completed \n", INFO_LOG);
		return (VkResult)VK_FALSE;
	}


	// accuire index of next swapchain image
	//UINT64_MAX is timeout value waiting for vkSwapchainKHR
	//Even after waiting for UINT64_MAX if vkSwapchainKHR is not available , vkAcquireNextImageKHR returns VK_NOT_READY
	//vkSemaphore_backbuffer is not waiting for Swapchain , it is waiting for another queue to release semaphore
	//optional fence to wait the Host , for now VK_NULL_HANDLE


	vkResult = vkAcquireNextImageKHR(vkdevice, vkSwapchainKHR, UINT64_MAX, vkSemaphore_backbuffer, VK_NULL_HANDLE, &currentImageIndex);
	if (vkResult != VK_SUCCESS)
	{
		if (vkResult == VK_ERROR_OUT_OF_DATE_KHR || vkResult == VK_SUBOPTIMAL_KHR)
		{
			resize(winWidth, winHeight);
		}
		else
		{
			fprintf(gpFile, "%s display :: vkAcquireNextImageKHR()  failed \n", ERROR_LOG);
			return vkResult;
		}
	}


	// use fence to allow host to wait for the completion of the execution of previous command buffer
	// we are giving fence count
	// vkFence_array[currentImageIndex] wait for this fence
	//VK_TRUE wait for all fences to be signaled
	//UINT64_MAX timeout value
	vkResult = vkWaitForFences(vkdevice, 1, &vkFence_array[currentImageIndex], VK_TRUE, UINT64_MAX);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s display :: vkWaitForFences()  failed \n", ERROR_LOG);
		return vkResult;
	}


	// ready fences for next use
	vkResult = vkResetFences(vkdevice, 1, &vkFence_array[currentImageIndex]);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s display :: vkResetFences()  failed \n", ERROR_LOG);
		return vkResult;
	}


	//one of the member of vksubmitinfo struct requires array of pipeline stages , we have only 1 of completion of color attachment still we need one array
	const VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;


	//delcare memset and initialize vkSubmitInfoStruct
	VkSubmitInfo vkSubmitInfo;
	memset((void*)&vkSubmitInfo, 0, sizeof(VkSubmitInfo));
	vkSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	vkSubmitInfo.pNext = NULL;
	vkSubmitInfo.pWaitDstStageMask = &waitDstStageMask; // destination stage mask
	vkSubmitInfo.waitSemaphoreCount = 1;
	vkSubmitInfo.pWaitSemaphores = &vkSemaphore_backbuffer; // semaphore to wait for
	vkSubmitInfo.commandBufferCount = 1;
	vkSubmitInfo.pCommandBuffers = &vkCommandBuffer_array[currentImageIndex]; // command buffer to execute
	vkSubmitInfo.signalSemaphoreCount = 1;
	vkSubmitInfo.pSignalSemaphores = &vkSemaphore_rendercomplete; // semaphore to signal


	//submit above work to queue
	// second parambeter is number of submitinfo structs
	// third parameter is array of submitinfo structs
	// fourth parameter is fence to signal when all command buffers are done
	//vkQueueSubmit - submits command buffers to a queue
	vkResult = vkQueueSubmit(vkQueue, 1, &vkSubmitInfo, vkFence_array[currentImageIndex]);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s display :: vkQueueSubmit()  failed \n", ERROR_LOG);
		return vkResult;
	}


	// we are going to present the rendered image after declaring and initialzing the VkPresentInfo struct
	VkPresentInfoKHR vkPresentInfoKHR;
	memset((void*)&vkPresentInfoKHR, 0, sizeof(VkPresentInfoKHR));


	vkPresentInfoKHR.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	vkPresentInfoKHR.pNext = NULL;
	vkPresentInfoKHR.swapchainCount = 1;
	vkPresentInfoKHR.pSwapchains = &vkSwapchainKHR;
	vkPresentInfoKHR.waitSemaphoreCount = 1;
	vkPresentInfoKHR.pWaitSemaphores = &vkSemaphore_rendercomplete;
	vkPresentInfoKHR.pImageIndices = &currentImageIndex;


	//present the queue
	vkResult = vkQueuePresentKHR(vkQueue, &vkPresentInfoKHR);
	if (vkResult != VK_SUCCESS)
	{
		if (vkResult == VK_ERROR_OUT_OF_DATE_KHR || vkResult == VK_SUBOPTIMAL_KHR)
		{
			resize(winWidth, winHeight);
		}
		else
		{
			fprintf(gpFile, "%s display :: vkQueuePresentKHR()  failed \n", ERROR_LOG);
			return vkResult;
		}
	}




	updateUniformBuffer();
	vkDeviceWaitIdle(vkdevice);
	return vkResult;


}


void update(void)
{
	angleTeapot = angleTeapot + 0.5f;
	if (angleTeapot >= 360.0f)
	{
		angleTeapot = angleTeapot - 360.0f;
	}
}


VkResult resize(int width, int height)
{
	// function declarations
	VkResult createSwapchain(VkBool32);
	VkResult createImagesAndImageViews(void);
	VkResult createCommandBuffers(void);
	VkResult createPipelineLayout(void);
	VkResult createPipeline(void);
	VkResult createRenderPass(void);
	VkResult createFramebuffers(void);
	VkResult buildCommandBuffers(void);


	// variable declarations


	// code
	if (height <= 0)
		height = 1;
	VkResult vkResult = VK_SUCCESS;


	if (bInitialized == FALSE)
	{
		fprintf(gpFile, "%s resize :: bInitialized is FALSE \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}


	// reset bInitialized
	bInitialized = FALSE;


	// set to window client area dimensions
	winWidth = width;
	winHeight = height;


	// wait for device to complete task
	if (vkdevice)
	{
		vkDeviceWaitIdle(vkdevice);
	}


	//check if swapchain exists
	if (vkSwapchainKHR == VK_NULL_HANDLE)
	{
		fprintf(gpFile, "%s resize ::swapchain is null \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}


	if (VkImageView_Depth)
	{
		vkDestroyImageView(vkdevice, VkImageView_Depth, NULL);
		VkImageView_Depth = VK_NULL_HANDLE;
		fprintf(gpFile, "%s resize :: vkDestroyImageView -  VkImageView_Depth image view freed \n", INFO_LOG);
	}


	if (vkImage_Depth)
	{
		vkDestroyImage(vkdevice, vkImage_Depth, NULL);
		vkImage_Depth = VK_NULL_HANDLE;
		fprintf(gpFile, "%s resize :: vkDestroyImage -  VkImageView_Depth image view freed \n", INFO_LOG);
	}


	if (vkDeviceMemory_Depth)
	{
		vkFreeMemory(vkdevice, vkDeviceMemory_Depth, NULL);
		vkDeviceMemory_Depth = VK_NULL_HANDLE;
		fprintf(gpFile, "%s resize :: vkFreeMemory() succeed for vkDeviceMemory_Depth \n", INFO_LOG);
	}




	//destroy framebuffer
	for (uint32_t i = 0; i < swapchainImageCount; i++)
	{
		vkDestroyFramebuffer(vkdevice, vkFramebuffers_array[i], NULL);
		fprintf(gpFile, "%s resize :: vkDestroyFramebuffer() successful \n", INFO_LOG);
	}


	if (vkFramebuffers_array)
	{
		free(vkFramebuffers_array);
		vkFramebuffers_array = NULL;
		fprintf(gpFile, "%s resize :: vkFramebuffers_array() memory freed \n", INFO_LOG);


	}


	//destroy command buffer
	for (uint32_t i = 0; i < swapchainImageCount; i++)
	{
		vkFreeCommandBuffers(vkdevice, vkCommandPool, 1, &vkCommandBuffer_array[i]);
		fprintf(gpFile, "%s resize :: vkFreeCommandBuffers() successful \n", INFO_LOG);
	}


	if (vkCommandBuffer_array)
	{
		free(vkCommandBuffer_array);
		vkCommandBuffer_array = NULL;
		fprintf(gpFile, "%s resize :: vkCommandBuffer_array() memory freed \n", INFO_LOG);
	}


	//destroy pipeline
	if (vkPipeline)
	{
		vkDestroyPipeline(vkdevice, vkPipeline, NULL);
		vkPipeline = VK_NULL_HANDLE;
		fprintf(gpFile, "%s resize :: vkDestroyPipeline() successful \n", INFO_LOG);
	}


	//pipeline layout
	if (vkPipelineLayout)
	{
		vkDestroyPipelineLayout(vkdevice, vkPipelineLayout, NULL);
		vkPipelineLayout = NULL;
		fprintf(gpFile, "%s resize :: vkDestroyPipelineLayout() successful \n", INFO_LOG);
	}




	//destroy renderpass
	if (vkRenderPass)
	{
		vkDestroyRenderPass(vkdevice, vkRenderPass, NULL);
		vkRenderPass = VK_NULL_HANDLE;
		fprintf(gpFile, "%s resize :: vkDestroyRenderPass() successful \n", INFO_LOG);
	}




	//destroy image views
	for (uint32_t i = 0; i < swapchainImageCount; i++)
	{
		vkDestroyImageView(vkdevice, swapchainImageView_array[i], NULL);
		fprintf(gpFile, "%s resize :: vkDestroyImageView() successful for %d \n", INFO_LOG, i);
	}


	if (swapchainImageView_array)
	{
		free(swapchainImageView_array);
		swapchainImageView_array = NULL;
		fprintf(gpFile, "%s resize :: swapchainImageView_array freed \n", INFO_LOG);
	}


	//free swapchain images
	//for (uint32_t i = 0; i < swapchainImageCount; i++)
	//{
	//	vkDestroyImage(vkdevice, swapchainImage_array[i], NULL);
	//	fclose(gpFile);
	//	gpFile = NULL;
	//	fprintf(gpFile, "%s: vkDestroyImage() successful to destroy swapchainImage_array\n", __func__);
	//}


	if (swapchainImage_array)
	{
		free(swapchainImage_array);
		swapchainImage_array = NULL;
		fprintf(gpFile, "%s resize :: swapchainImage_array freed \n", INFO_LOG);
	}


	//destroy swapchain
	if (vkSwapchainKHR)
	{
		vkDestroySwapchainKHR(vkdevice, vkSwapchainKHR, NULL);
		vkSwapchainKHR = NULL;
		fprintf(gpFile, "%s resize :: vkDestroySwapchainKHR() successful \n", INFO_LOG);
	}


	// re-creation logic for resize


	//create swapchain
	vkResult = createSwapchain(VK_FALSE);


	if (vkResult != VK_SUCCESS)
	{
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		fprintf(gpFile, "%s resize :: createSwapchain() failed \n", ERROR_LOG);
		return (vkResult);
	}


	//create vulkan images and image views
	vkResult = createImagesAndImageViews();


	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s resize :: createImagesAndImageViews() failed \n", ERROR_LOG);
		return (vkResult);
	}


	//renderpass
	vkResult = createRenderPass();


	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s resize :: createRenderPass() failed \n", ERROR_LOG);
		return (vkResult);
	}


	//pipeline layout
	vkResult = createPipelineLayout();


	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s resize :: createPipelineLayout() failed \n", ERROR_LOG);
		return (vkResult);
	}




	//pipeline
	vkResult = createPipeline();


	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s resize :: createPipeline() failed \n", ERROR_LOG);
		return (vkResult);
	}


	//framebuffer
	vkResult = createFramebuffers();


	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s resize :: createFramebuffers() failed \n", ERROR_LOG);
		return (vkResult);
	}


	//command buffer
	vkResult = createCommandBuffers();


	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s resize :: createCommandBuffers() failed \n", ERROR_LOG);
		return (vkResult);
	}


	//build command buffers
	vkResult = buildCommandBuffers();


	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s resize :: buildCommandBuffers() failed \n", ERROR_LOG);
		return (vkResult);
	}




	bInitialized = TRUE;


	return vkResult;
}




void uninitialize(void)
{


	void ToogleFullScreen(void);


	if (bFullScreen == TRUE)
	{
		ToogleFullScreen();
	}


	if (ghwnd)
	{
		ghwnd = NULL;
	}


	if (vkdevice)
	{
		vkDeviceWaitIdle(vkdevice);
		fprintf(gpFile, "%s uninitialize :: vkDeviceWaitIdle is Done \n", INFO_LOG);


		//destroy fences
		for (uint32_t i = 0; i < swapchainImageCount; i++)
		{
			vkDestroyFence(vkdevice, vkFence_array[i], NULL);
			fprintf(gpFile, "%s uninitialize :: vkDestroyFence() succeed \n", INFO_LOG);
		}


		if (vkFence_array)
		{
			free(vkFence_array);
			vkFence_array = NULL;
		}


		//destroy semaphore
		if (vkSemaphore_rendercomplete)
		{
			vkDestroySemaphore(vkdevice, vkSemaphore_rendercomplete, NULL);
			vkSemaphore_rendercomplete = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkDestroySemaphore() for vkSemaphore_rendercomplete succeed \n", INFO_LOG);
		}
		if (vkSemaphore_backbuffer)
		{
			vkDestroySemaphore(vkdevice, vkSemaphore_backbuffer, NULL);
			vkSemaphore_backbuffer = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkDestroySemaphore() for vkSemaphore_backbuffer succeed \n", INFO_LOG);
		}


		if (VkImageView_Depth)
		{
			vkDestroyImageView(vkdevice, VkImageView_Depth, NULL);
			VkImageView_Depth = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkDestroyImageView -  VkImageView_Depth image view freed \n", INFO_LOG);
		}


		if (vkImage_Depth)
		{
			vkDestroyImage(vkdevice, vkImage_Depth, NULL);
			vkImage_Depth = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkDestroyImage -  VkImageView_Depth image view freed \n", INFO_LOG);
		}


		if (vkDeviceMemory_Depth)
		{
			vkFreeMemory(vkdevice, vkDeviceMemory_Depth, NULL);
			vkDeviceMemory_Depth = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkFreeMemory() succeed for vkDeviceMemory_Depth \n", INFO_LOG);
		}


		//destroy framebuffers
		for (uint32_t i = 0; i < swapchainImageCount; i++)
		{
			vkDestroyFramebuffer(vkdevice, vkFramebuffers_array[i], NULL);
			fprintf(gpFile, "%s uninitialize :: vkDestroyFramebuffer() succeed \n", INFO_LOG);
		}


		if (vkFramebuffers_array)
		{
			free(vkFramebuffers_array);
			vkFramebuffers_array = NULL;
		}


		if (vkPipeline)
		{
			vkDestroyPipeline(vkdevice, vkPipeline, NULL);
			vkPipeline = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkDestroyPipeline() succeed \n", INFO_LOG);
		}


		//destroy renderpass
		if (vkRenderPass)
		{
			vkDestroyRenderPass(vkdevice, vkRenderPass, NULL);
			vkRenderPass = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkDestroyRenderPass() succeed \n", INFO_LOG);
		}


		// destroy descriptor pool 
		// destroying descriptor pool also deletes descriptor sets created from that pool
		if (vkDescriptorPool)
		{
			vkDestroyDescriptorPool(vkdevice, vkDescriptorPool, NULL);
			vkDescriptorPool = VK_NULL_HANDLE;
			vkDescriptorSet = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkDestroyDescriptorPool() succeed , destroying vkDescriptorPool & vkDescriptorSet \n", INFO_LOG);
		}


		if (vkPipelineLayout)
		{
			vkDestroyPipelineLayout(vkdevice, vkPipelineLayout, NULL);
			vkPipelineLayout = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: VkDestroyPipelineLayout() succeed \n", INFO_LOG);
		}


		if (vkDescriptorSetLayout)
		{
			vkDestroyDescriptorSetLayout(vkdevice, vkDescriptorSetLayout, NULL);
			vkDescriptorSetLayout = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkDestroyDescriptorSetLayout() succeed \n", INFO_LOG);
		}


		if (vkShaderModule_fragment_shader)
		{
			vkDestroyShaderModule(vkdevice, vkShaderModule_fragment_shader, NULL);
			vkShaderModule_fragment_shader = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkDestroyShaderModule() succeed for Fragment shader\n", INFO_LOG);
		}


		if (vkShaderModule_vertex_shader)
		{
			vkDestroyShaderModule(vkdevice, vkShaderModule_vertex_shader, NULL);
			vkShaderModule_vertex_shader = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkDestroyShaderModule() succeed for Vertex shader\n", INFO_LOG);
		}

		// texture related
		if (vkImageView_texture)
		{
			vkDestroyImageView(vkdevice, vkImageView_texture, NULL);
			vkImageView_texture = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkDestroyImageView() succeed for vkImageView_texture \n", INFO_LOG);
		}

		if (vkDeviceMemory_texture)
		{
			vkFreeMemory(vkdevice, vkDeviceMemory_texture, NULL);
			vkDeviceMemory_texture = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkFreeMemory() succeed for vkDeviceMemory_texture \n", INFO_LOG);
		}


		if (vkSampler_texture)
		{
			vkDestroySampler(vkdevice, vkSampler_texture, NULL);
			vkSampler_texture = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkDestroySampler() succeed for vkSampler_texture \n", INFO_LOG);
		}

		if (vkImage_texture)
		{
			vkDestroyImage(vkdevice, vkImage_texture, NULL);
			vkImage_texture = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkDestroyImage() succeed for vkImage_texture \n", INFO_LOG);
		}

		// destroy uniform buffer
		if (uniformData.vkDeviceMemory)
		{
			vkFreeMemory(vkdevice, uniformData.vkDeviceMemory, NULL);
			uniformData.vkDeviceMemory = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkFreeMemory() succeed for uniformData \n", INFO_LOG);
		}


		if (uniformData.vkBuffer)
		{
			vkDestroyBuffer(vkdevice, uniformData.vkBuffer, NULL);
			uniformData.vkBuffer = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkDestroyBuffer() succeed for uniformData \n", INFO_LOG);
		}

		//destroy index buffer
		if (vertexData_index.vkDeviceMemory)
		{
			vkFreeMemory(vkdevice, vertexData_index.vkDeviceMemory, NULL);
			vertexData_index.vkDeviceMemory = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkFreeMemory() succeed for vertexData_index \n", INFO_LOG);
		}


		if (vertexData_index.vkBuffer)
		{
			vkDestroyBuffer(vkdevice, vertexData_index.vkBuffer, NULL);
			vertexData_index.vkBuffer = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkDestroyBuffer() succeed for vertexData_index \n", INFO_LOG);
		}

		//destroy texcord
		if (vertexData_texcord.vkDeviceMemory)
		{
			vkFreeMemory(vkdevice, vertexData_texcord.vkDeviceMemory, NULL);
			vertexData_texcord.vkDeviceMemory = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkFreeMemory() succeed for vertexData_texcord \n", INFO_LOG);
		}


		if (vertexData_texcord.vkBuffer)
		{
			vkDestroyBuffer(vkdevice, vertexData_texcord.vkBuffer, NULL);
			vertexData_texcord.vkBuffer = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkDestroyBuffer() succeed for vertexData_texcord \n", INFO_LOG);
		}

		// destory vertex normal buffer
		if (vertexData_normal.vkDeviceMemory)
		{
			vkFreeMemory(vkdevice, vertexData_normal.vkDeviceMemory, NULL);
			vertexData_normal.vkDeviceMemory = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkFreeMemory() succeed for vertexData_normal \n", INFO_LOG);
		}


		if (vertexData_normal.vkBuffer)
		{
			vkDestroyBuffer(vkdevice, vertexData_normal.vkBuffer, NULL);
			vertexData_normal.vkBuffer = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkDestroyBuffer() succeed for vertexData_normal \n", INFO_LOG);
		}

		// destory vertex color buffer
		if (vertexData_color.vkDeviceMemory)
		{
			vkFreeMemory(vkdevice, vertexData_color.vkDeviceMemory, NULL);
			vertexData_color.vkDeviceMemory = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkFreeMemory() succeed for vertexData_color \n", INFO_LOG);
		}


		if (vertexData_color.vkBuffer)
		{
			vkDestroyBuffer(vkdevice, vertexData_color.vkBuffer, NULL);
			vertexData_color.vkBuffer = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkDestroyBuffer() succeed for vertexData_color \n", INFO_LOG);
		}


		if (pElements)
		{
			free(pElements);
			pElements = NULL;
		}
		if (pTexCoords)
		{
			free(pTexCoords);
			pTexCoords = NULL;
		}
		if (pNormals)
		{
			free(pNormals);
			pNormals = NULL;
		}
		if (pPositions)
		{
			free(pPositions);
			pPositions = NULL;
		}

		// destory vertex position buffer
		if (vertexData_position.vkDeviceMemory)
		{
			vkFreeMemory(vkdevice, vertexData_position.vkDeviceMemory, NULL);
			vertexData_position.vkDeviceMemory = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkFreeMemory() succeed for vertexData_position \n", INFO_LOG);
		}


		if (vertexData_position.vkBuffer)
		{
			vkDestroyBuffer(vkdevice, vertexData_position.vkBuffer, NULL);
			vertexData_position.vkBuffer = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkDestroyBuffer() succeed for vertexData_position \n", INFO_LOG);
		}


		for (uint32_t i = 0; i < swapchainImageCount; i++)
		{
			vkFreeCommandBuffers(vkdevice, vkCommandPool, 1, &vkCommandBuffer_array[i]);
			fprintf(gpFile, "%s uninitialize :: vkFreeCommandBuffers() succeed \n", INFO_LOG);
		}


		if (vkCommandBuffer_array)
		{
			free(vkCommandBuffer_array);
			vkCommandBuffer_array = NULL;
			fprintf(gpFile, "%s uninitialize :: vkCommandBuffer_array freed \n", INFO_LOG);
		}


		if (vkCommandPool)
		{
			vkDestroyCommandPool(vkdevice, vkCommandPool, NULL);
			vkCommandPool = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkDestroyCommandPool() succeed \n", INFO_LOG);
		}


		for (uint32_t i = 0; i < swapchainImageCount; i++)
		{
			vkDestroyImageView(vkdevice, swapchainImageView_array[i], NULL);
			fprintf(gpFile, "%s uninitialize :: vkDestroyImage -  swapchainImageView_array image views freed \n", INFO_LOG);
		}


		if (swapchainImageView_array)
		{
			free(swapchainImageView_array);
			swapchainImageView_array = NULL;
		}


		// below code is commented because swapchainImage_array is destroyed while ImageView is destroyed


		//for (uint32_t i = 0; i < swapchainImageCount; i++)
		//{
		//	vkDestroyImage(vkdevice, swapchainImage_array[i], NULL);
		//	fclose(gpFile);
		//	gpFile = NULL;
		//}


		if (swapchainImage_array)
		{
			free(swapchainImage_array);
			swapchainImage_array = NULL;
			fprintf(gpFile, "%s uninitialize :: swapchainImage_array freed \n", INFO_LOG);
		}


		// destroy swapchain
		if (vkSwapchainKHR)
		{
			vkDestroySwapchainKHR(vkdevice, vkSwapchainKHR, NULL);
			vkSwapchainKHR = VK_NULL_HANDLE;
			fprintf(gpFile, "%s uninitialize :: vkDestroySwapchainKHR succeeded \n", INFO_LOG);
		}


		//when we destroy vkDevice queues are automatically destroyed
		//destroy vulkan device


		vkDestroyDevice(vkdevice, NULL);
		vkdevice = VK_NULL_HANDLE;
	}


	// destroy vulkan instance


	if (vkSurfaceKHR)
	{
		vkDestroySurfaceKHR(vkInstance, vkSurfaceKHR, NULL); // destroy function is generic and not platform specific
		vkSurfaceKHR = VK_NULL_HANDLE;
		fprintf(gpFile, "%s uninitialize :: vkDestroySurfaceKHR() succeeded \n", INFO_LOG);


	}


	if (vkDebugReportCallbackEXT && vkDestroyDebugReportCallbackEXT_fnptr)
	{
		vkDestroyDebugReportCallbackEXT_fnptr(vkInstance, vkDebugReportCallbackEXT,
			NULL);
		vkDebugReportCallbackEXT = VK_NULL_HANDLE;
		vkDestroyDebugReportCallbackEXT_fnptr = NULL;
	}


	if (vkInstance)
	{
		vkDestroyInstance(vkInstance, NULL);
		vkInstance = VK_NULL_HANDLE;
		fprintf(gpFile, "%s uninitialize :: vkDestroyInstance() succeeded \n", INFO_LOG);


	}




	if (gpFile)
	{
		fprintf(gpFile, "%s uninitialize() :: Log file closing\n", INFO_LOG);
		fclose(gpFile);
		gpFile = NULL;
	}
}


///// Vulkan Related functions /////


VkResult createVulkanInstance(void)
{
	//function declaration
	VkResult fillInstanceExtensionNames(void);
	VkResult fillValidationLayerNames(void);
	VkResult createValidationCallbackFunction(void);


	VkResult vkResult = VK_SUCCESS;


	vkResult = fillInstanceExtensionNames();


	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createVulkanInstance :: fillInstanceExtensionNames() is failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createVulkanInstance :: fillInstanceExtensionNames() succeeded \n", INFO_LOG);
	}


	if (bValidation == TRUE)
	{
		// fill validation layer
		vkResult = fillValidationLayerNames();


		if (vkResult != VK_SUCCESS)
		{
			fprintf(gpFile, "%s createVulkanInstance :: fillValidationLayerNames() is failed \n", ERROR_LOG);
			return vkResult;
		}
		else
		{
			fprintf(gpFile, "%s createVulkanInstance :: fillValidationLayerNames() succeeded \n", INFO_LOG);
		}
	}




	VkApplicationInfo vkApplicationInfo;
	memset((void*)&vkApplicationInfo, 0, sizeof(VkApplicationInfo));
	vkApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	vkApplicationInfo.pNext = NULL; //pointer to next
	vkApplicationInfo.pApplicationName = gpszAppName;
	vkApplicationInfo.applicationVersion = 1;
	vkApplicationInfo.pEngineName = gpszAppName;
	vkApplicationInfo.engineVersion = 1;
	vkApplicationInfo.apiVersion = VK_VERSION_1_3;


	VkInstanceCreateInfo vkInstanceCreateInfo;
	memset((void*)&vkInstanceCreateInfo, 0, sizeof(VkInstanceCreateInfo));
	vkInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	vkInstanceCreateInfo.pNext = NULL; //pointer to next
	vkInstanceCreateInfo.pApplicationInfo = &vkApplicationInfo;
	vkInstanceCreateInfo.enabledExtensionCount = enableInstanceExtensionCount;
	vkInstanceCreateInfo.ppEnabledExtensionNames = enabledInstanceExtensionNames_array;
	if (bValidation == TRUE)
	{
		vkInstanceCreateInfo.enabledLayerCount = enabledValidationLayerCount;
		vkInstanceCreateInfo.ppEnabledLayerNames = enabledValidationLayerNames_array;
	}
	else
	{
		vkInstanceCreateInfo.enabledLayerCount = 0;
		vkInstanceCreateInfo.ppEnabledLayerNames = NULL;
	}


	vkResult = vkCreateInstance(&vkInstanceCreateInfo, NULL, &vkInstance);


	if (vkResult == VK_ERROR_INCOMPATIBLE_DRIVER)
	{
		fprintf(gpFile, "%s createVulkanInstance :: vkCreateInstance() failed due to  VK_ERROR_INCOMPATIBLE_DRIVER %d \n", ERROR_LOG, vkResult);
		return vkResult;
	}
	else if (vkResult == VK_ERROR_EXTENSION_NOT_PRESENT)
	{
		fprintf(gpFile, "%s createVulkanInstance :: vkCreateInstance() failed due to  VK_ERROR_EXTENSION_NOT_PRESENT %d \n", ERROR_LOG, vkResult);
		return vkResult;
	}
	else if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createVulkanInstance :: vkCreateInstance() failed due to unknown reason %d \n", ERROR_LOG, vkResult);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createVulkanInstance :: vkCreateInstance() succeeded \n", INFO_LOG);
	}


	// do for validation call backs
	if (bValidation == TRUE)
	{
		vkResult = createValidationCallbackFunction();
		if (vkResult != VK_SUCCESS)
		{
			fprintf(gpFile, "%s createVulkanInstance :: createValidationCallbackFunction() is failed \n", ERROR_LOG);
			return vkResult;
		}
		else
		{
			fprintf(gpFile, "%s createVulkanInstance :: createValidationCallbackFunction() succeeded \n", INFO_LOG);
		}
	}


	return vkResult;


}


VkResult fillInstanceExtensionNames(void)
{
	VkResult vkResult = VK_SUCCESS;


	uint32_t instanceExtensionCount = 0;
	vkResult = vkEnumerateInstanceExtensionProperties(NULL, &instanceExtensionCount, NULL);  // vkEnumerateExtensionProperties( layerName - null because we want all extensions , returns count , returns all parameters)
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s fillInstanceExtensionNames :: vkEnumerateExtensionProperties() getting count failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s fillInstanceExtensionNames :: vkEnumerateExtensionProperties() getting count succeeded \n", INFO_LOG);
	}


	VkExtensionProperties* vkExtensionProperties_array = NULL;
	vkExtensionProperties_array = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * instanceExtensionCount);
	if (vkExtensionProperties_array == NULL)
	{
		fprintf(gpFile, "%s fillInstanceExtensionNames :: vkExtensionProperties_array memory allocation failed \n", ERROR_LOG);
	}


	vkResult = vkEnumerateInstanceExtensionProperties(NULL, &instanceExtensionCount, vkExtensionProperties_array);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s fillInstanceExtensionNames :: vkEnumerateExtensionProperties() getting properties failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s fillInstanceExtensionNames :: vkEnumerateExtensionProperties() getting properties succeeded \n", INFO_LOG);
	}


	char** instanceExtensionNames_array = NULL;
	instanceExtensionNames_array = (char**)malloc(sizeof(char*) * instanceExtensionCount);
	if (vkExtensionProperties_array == NULL)
	{
		fprintf(gpFile, "%s fillInstanceExtensionNames :: vkExtensionProperties_array memory allocation failed \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}


	for (uint32_t i = 0; i < instanceExtensionCount; i++)
	{
		instanceExtensionNames_array[i] = (char*)malloc(sizeof(char) * (strlen(vkExtensionProperties_array[i].extensionName) + 1));
		if (instanceExtensionNames_array[i] == NULL)
		{
			fprintf(gpFile, "%s fillInstanceExtensionNames :: instanceExtensionNames_array[%d] memory allocation failed \n", ERROR_LOG, i);
			vkResult = VK_ERROR_INITIALIZATION_FAILED;
			return vkResult;
		}
		memcpy(instanceExtensionNames_array[i], vkExtensionProperties_array[i].extensionName, strlen(vkExtensionProperties_array[i].extensionName) + 1);
		fprintf(gpFile, "%s fillInstanceExtensionNames :: Vulkan Instance Extension = %s \n", INFO_LOG, instanceExtensionNames_array[i]);
	}


	if (vkExtensionProperties_array)
	{
		free(vkExtensionProperties_array);
		vkExtensionProperties_array = NULL;
	}


	VkBool32 vulkanSurfaceExtensionFound = VK_FALSE;
	VkBool32 win32SurfaceExtensionFound = VK_FALSE;
	VkBool32 debugReportExtensionFound = VK_FALSE;


	for (uint32_t i = 0; i < instanceExtensionCount; i++)
	{
		if (strcmp(instanceExtensionNames_array[i], VK_KHR_SURFACE_EXTENSION_NAME) == 0)
		{
			vulkanSurfaceExtensionFound = VK_TRUE;
			enabledInstanceExtensionNames_array[enableInstanceExtensionCount++] = VK_KHR_SURFACE_EXTENSION_NAME;
		}


		if (strcmp(instanceExtensionNames_array[i], VK_KHR_WIN32_SURFACE_EXTENSION_NAME) == 0)
		{
			win32SurfaceExtensionFound = VK_TRUE;
			enabledInstanceExtensionNames_array[enableInstanceExtensionCount++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
		}


		if (strcmp(instanceExtensionNames_array[i], VK_EXT_DEBUG_REPORT_EXTENSION_NAME) == 0)
		{
			debugReportExtensionFound = VK_TRUE;
			if (bValidation == TRUE)
			{
				enabledInstanceExtensionNames_array[enableInstanceExtensionCount++] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
			}
			else
			{
				//array will not have entry of VK_EXT_DEBUG_REPORT_EXTENSION_NAME
			}
		}


	}


	for (uint32_t i = 0; i < instanceExtensionCount; i++)
	{
		if (instanceExtensionNames_array[i])
		{
			free(instanceExtensionNames_array[i]);
			instanceExtensionNames_array[i] = NULL;
		}
	}
	if (instanceExtensionNames_array)
	{
		free(instanceExtensionNames_array);
		instanceExtensionNames_array = NULL;
	}


	if (vulkanSurfaceExtensionFound == VK_FALSE)
	{
		fprintf(gpFile, "%s fillInstanceExtensionNames :: VK_KHR_SURFACE_EXTENSION_NAME not found \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED; // return hardcoded failure
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s fillInstanceExtensionNames :: VK_KHR_SURFACE_EXTENSION_NAME found \n", INFO_LOG);
	}


	if (win32SurfaceExtensionFound == VK_FALSE)
	{
		fprintf(gpFile, "%s fillInstanceExtensionNames :: VK_KHR_WIN32_SURFACE_EXTENSION_NAME not found \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED; // return hardcoded failure
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s fillInstanceExtensionNames :: VK_KHR_WIN32_SURFACE_EXTENSION_NAME found \n", INFO_LOG);
	}


	if (debugReportExtensionFound == VK_FALSE)
	{
		if (bValidation == TRUE)
		{
			vkResult = VK_ERROR_INITIALIZATION_FAILED; // return hardcoded failure
			fprintf(gpFile, "%s fillInstanceExtensionNames :: VK_EXT_DEBUG_REPORT_EXTENSION_NAME not found and bValidation is ON \n", ERROR_LOG);
			return vkResult;
		}
		else
		{
			fprintf(gpFile, "%s fillInstanceExtensionNames :: VK_EXT_DEBUG_REPORT_EXTENSION_NAME not found and bValidation is OFF \n", INFO_LOG);
		}
	}
	else
	{
		if (bValidation == TRUE)
		{
			fprintf(gpFile, "%s fillInstanceExtensionNames :: VK_EXT_DEBUG_REPORT_EXTENSION_NAME  found and bValidation is ON \n", INFO_LOG);
		}
		else
		{
			fprintf(gpFile, "%s fillInstanceExtensionNames :: VK_EXT_DEBUG_REPORT_EXTENSION_NAME found and bValidation is OFF \n", ERROR_LOG);
		}
	}


	for (uint32_t i = 0; i < enableInstanceExtensionCount; i++)
	{
		fprintf(gpFile, "%s fillInstanceExtensionNames :: Enable vulkan Instance Extension Name = %s \n", INFO_LOG, enabledInstanceExtensionNames_array[i]);
	}


	return vkResult;


}


VkResult fillValidationLayerNames(void)
{
	// variables declaration
	VkResult vkResult = VK_SUCCESS;
	uint32_t validationLayerCount = 0;
	vkResult = vkEnumerateInstanceLayerProperties(&validationLayerCount, NULL);  // vkEnumerateExtensionProperties( layerName - null because we want all extensions , returns count , returns all parameters)
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s fillValidationLayerNames :: vkEnumerateInstanceLayerProperties() getting count failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s fillValidationLayerNames :: vkEnumerateInstanceLayerProperties() getting count succeeded \n", INFO_LOG);
	}


	VkLayerProperties* vkLayerProperties_array = NULL;
	vkLayerProperties_array = (VkLayerProperties*)malloc(sizeof(VkLayerProperties) * validationLayerCount);
	if (vkLayerProperties_array == NULL)
	{
		fprintf(gpFile, "%s fillValidationLayerNames :: vkLayerProperties_array memory allocation failed \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}


	vkResult = vkEnumerateInstanceLayerProperties(&validationLayerCount, vkLayerProperties_array);  // vkEnumerateExtensionProperties( layerName - null because we want all extensions , returns count , returns all parameters)
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s fillValidationLayerNames :: vkEnumerateInstanceLayerProperties() getting extensions failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s fillValidationLayerNames :: vkEnumerateInstanceLayerProperties() getting extensions succeeded \n", INFO_LOG);
	}


	char** validationLayerNames_array = NULL;
	validationLayerNames_array = (char**)malloc(sizeof(char*) * validationLayerCount);
	if (validationLayerNames_array == NULL)
	{
		fprintf(gpFile, "%s fillValidationLayerNames :: validationLayerNames_array memory allocation failed \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}


	for (uint32_t i = 0; i < validationLayerCount; i++)
	{
		validationLayerNames_array[i] = (char*)malloc(sizeof(char) * (strlen(vkLayerProperties_array[i].layerName) + 1));
		if (validationLayerNames_array[i] == NULL)
		{
			fprintf(gpFile, "%s fillValidationLayerNames :: validationLayerNames_array[%d] memory allocation failed \n", ERROR_LOG, i);
			vkResult = VK_ERROR_INITIALIZATION_FAILED;
			return vkResult;
		}
		memcpy(validationLayerNames_array[i], vkLayerProperties_array[i].layerName, strlen(vkLayerProperties_array[i].layerName) + 1);
		fprintf(gpFile, "%s fillValidationLayerNames :: Vulkan Layer Extension = %s \n", INFO_LOG, validationLayerNames_array[i]);


	}


	if (vkLayerProperties_array)
	{
		free(vkLayerProperties_array);
		vkLayerProperties_array = NULL;
	}


	VkBool32 validationLayerFound = VK_FALSE;


	for (uint32_t i = 0; i < validationLayerCount; i++)
	{
		if (strcmp(validationLayerNames_array[i], "VK_LAYER_KHRONOS_validation") == 0)
		{
			validationLayerFound = VK_TRUE;
			enabledValidationLayerNames_array[enabledValidationLayerCount++] = "VK_LAYER_KHRONOS_validation";
		}
	}


	for (uint32_t i = 0; i < validationLayerCount; i++)
	{
		if (validationLayerNames_array[i])
		{
			free(validationLayerNames_array[i]);
			validationLayerNames_array[i] = NULL;
		}
	}


	if (validationLayerNames_array)
	{
		free(validationLayerNames_array);
		validationLayerNames_array = NULL;
	}


	if (validationLayerFound == VK_FALSE)
	{
		fprintf(gpFile, "%s fillValidationLayerNames :: VK_LAYER_KHRONOS_validation not supported \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED; // return hardcoded failure
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s fillValidationLayerNames :: VK_LAYER_KHRONOS_validation supported \n", INFO_LOG);
	}




	for (uint32_t i = 0; i < enabledValidationLayerCount; i++)
	{
		fprintf(gpFile, "%s fillValidationLayerNames :: Enable vulkan Layered Extension Name = %s \n", INFO_LOG, enabledValidationLayerNames_array[i]);
	}


	return vkResult;
}


VkResult createValidationCallbackFunction(void)
{
	//function declaration
	VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallback(VkDebugReportFlagsEXT, //VKAPI_ATTR for gcc and clang and VKAPI_CALL for windows
		VkDebugReportObjectTypeEXT,
		uint64_t,
		size_t,
		int32_t,
		const char*,
		const char*,
		void*);
	// variables declaration
	VkResult vkResult = VK_SUCCESS;
	PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT_fnptr = NULL;


	//get required function pointer
	vkCreateDebugReportCallbackEXT_fnptr = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(vkInstance,
		"vkCreateDebugReportCallbackEXT"
	);


	if (vkCreateDebugReportCallbackEXT_fnptr == NULL)
	{
		fprintf(gpFile, "%s createValidationCallbackFunction :: vkGetInstanceProcAddr failed for vkCreateDebugReportCallbackEXT \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createValidationCallbackFunction :: vkGetInstanceProcAddr success for vkCreateDebugReportCallbackEXT \n", INFO_LOG);
	}






	vkDestroyDebugReportCallbackEXT_fnptr = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(vkInstance,
		"vkDestroyDebugReportCallbackEXT"
	);


	if (vkDestroyDebugReportCallbackEXT_fnptr == NULL)
	{
		fprintf(gpFile, "%s createValidationCallbackFunction :: vkGetInstanceProcAddr failed for vkDestroyDebugReportCallbackEXT \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createValidationCallbackFunction :: vkGetInstanceProcAddr success for vkDestroyDebugReportCallbackEXT \n", INFO_LOG);
	}


	// get the vulkan callback object
	VkDebugReportCallbackCreateInfoEXT vkdebugReportCallbackCreateInfo;
	memset((void*)&vkdebugReportCallbackCreateInfo, 0, sizeof(VkDebugReportCallbackCreateInfoEXT));
	vkdebugReportCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	vkdebugReportCallbackCreateInfo.pNext = NULL;
	vkdebugReportCallbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	vkdebugReportCallbackCreateInfo.pfnCallback = debugReportCallback;
	vkdebugReportCallbackCreateInfo.pUserData = NULL;


	vkResult = vkCreateDebugReportCallbackEXT_fnptr(vkInstance, &vkdebugReportCallbackCreateInfo, NULL, &vkDebugReportCallbackEXT);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createValidationCallbackFunction :: vkCreateDebugReportCallbackEXT_fnptr() getting count failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createValidationCallbackFunction :: vkCreateDebugReportCallbackEXT_fnptr() getting count succeeded \n", INFO_LOG);
	}


	return vkResult;
}


VkResult getSupportedSurface(void)
{
	VkResult vkResult = VK_SUCCESS;


	VkWin32SurfaceCreateInfoKHR vkwin32SurfaceCreateInfoKHR;
	memset((void*)&vkwin32SurfaceCreateInfoKHR, 0, sizeof(VkWin32SurfaceCreateInfoKHR));
	vkwin32SurfaceCreateInfoKHR.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	vkwin32SurfaceCreateInfoKHR.pNext = NULL;
	vkwin32SurfaceCreateInfoKHR.flags = 0;
	vkwin32SurfaceCreateInfoKHR.hinstance = (HINSTANCE)GetWindowLongPtr(ghwnd, GWLP_HINSTANCE);//(HINSTANCE)GetMoudleHandle(NULL)
	vkwin32SurfaceCreateInfoKHR.hwnd = ghwnd;


	vkResult = vkCreateWin32SurfaceKHR(vkInstance, &vkwin32SurfaceCreateInfoKHR, NULL, &vkSurfaceKHR);


	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s getSupportedSurface :: vkCreateWin32SurfaceKHR() failed %d \n", ERROR_LOG, vkResult);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s getSupportedSurface :: vkCreateWin32SurfaceKHR() succeeded \n", INFO_LOG);
	}


	return vkResult;
}


VkResult getPhysicalDevice(void)
{


	VkResult vkResult = vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, NULL);


	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s getPhysicalDevice :: vkEnumeratePhysicalDevices() failed first call %d \n", ERROR_LOG, vkResult);
		return vkResult;
	}
	else if (physicalDeviceCount == 0)
	{
		fprintf(gpFile, "%s getPhysicalDevice :: vkEnumeratePhysicalDevices() returns 0 devices \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s getPhysicalDevice :: vkEnumeratePhysicalDevices() succeeded first call \n", INFO_LOG);
	}




	vkPhysicalDevice_array = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * physicalDeviceCount);
	if (vkPhysicalDevice_array == NULL)
	{
		fprintf(gpFile, "%s getPhysicalDevice :: vkPhysicalDevice_array memory allocation failed \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}


	vkResult = vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, vkPhysicalDevice_array);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s getPhysicalDevice :: vkEnumeratePhysicalDevices() failed second call %d \n", ERROR_LOG, vkResult);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s getPhysicalDevice :: vkEnumeratePhysicalDevices() succeeded second call \n", INFO_LOG);
	}


	VkBool32 bFound = VK_FALSE;


	for (uint32_t i = 0; i < physicalDeviceCount; i++)
	{
		uint32_t queueCount = UINT32_MAX;
		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice_array[i], &queueCount, NULL); // physical device atleast has 1 queue array
		VkQueueFamilyProperties* vkQueueFamilyProperties_array = NULL;
		vkQueueFamilyProperties_array = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * queueCount);


		if (vkQueueFamilyProperties_array == NULL)
		{
			fprintf(gpFile, "%s getPhysicalDevice :: vkQueueFamilyProperties_array memory allocation failed \n", ERROR_LOG);
			vkResult = VK_ERROR_INITIALIZATION_FAILED;
			return vkResult;
		}


		vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice_array[i], &queueCount, vkQueueFamilyProperties_array);


		VkBool32* isQueueSurfaceSupported_array = NULL;
		isQueueSurfaceSupported_array = (VkBool32*)malloc(sizeof(VkBool32) * queueCount);




		if (isQueueSurfaceSupported_array == NULL)
		{
			fprintf(gpFile, "%s getPhysicalDevice :: isQueueSurfaceSupported_array memory allocation failed \n", ERROR_LOG);
			return vkResult;
		}




		for (uint32_t j = 0; j < queueCount; j++)
		{
			vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice_array[i], j, vkSurfaceKHR, &isQueueSurfaceSupported_array[j]);
		}


		for (uint32_t j = 0; j < queueCount; j++)
		{
			if (vkQueueFamilyProperties_array[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				if (isQueueSurfaceSupported_array[j] == VK_TRUE)
				{
					vkPhysicalDevice_selected = vkPhysicalDevice_array[i];
					graphicsQueueFamilyIndex_Selected = j;
					bFound = VK_TRUE;
					break;
				}
			}
		}


		if (isQueueSurfaceSupported_array)
		{
			free(isQueueSurfaceSupported_array);
			isQueueSurfaceSupported_array = NULL;
			fprintf(gpFile, "%s getPhysicalDevice :: isQueueSurfaceSupported_array freed successfully \n", INFO_LOG);
		}


		if (vkQueueFamilyProperties_array)
		{
			free(vkQueueFamilyProperties_array);
			vkQueueFamilyProperties_array = NULL;
			fprintf(gpFile, "%s getPhysicalDevice :: vkQueueFamilyProperties_array freed successfully \n", INFO_LOG);
		}


		if (bFound == VK_TRUE)
			break;
	}


	if (bFound == VK_TRUE)
	{
		fprintf(gpFile, "%s getPhysicalDevice :: Successful to select required physical device with graphics enabled \n", INFO_LOG);
	}
	else
	{
		if (vkPhysicalDevice_array)
		{
			free(vkPhysicalDevice_array);
			vkPhysicalDevice_array = NULL;
			fprintf(gpFile, "%s printVkInfo :: vkPhysicalDevice_array freed \n", INFO_LOG);


		}
		fprintf(gpFile, "%s getPhysicalDevice :: failed to select required physical device with graphics enabled  \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return (vkResult);
	}




	memset((void*)&vkPhysicalDeviceMemoryProperties, 0, sizeof(VkPhysicalDeviceMemoryProperties));


	vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevice_selected, &vkPhysicalDeviceMemoryProperties);


	VkPhysicalDeviceFeatures vkPhysicalDeviceFeatures;
	memset((void*)&vkPhysicalDeviceFeatures, 0, sizeof(VkPhysicalDeviceFeatures));


	vkGetPhysicalDeviceFeatures(vkPhysicalDevice_selected, &vkPhysicalDeviceFeatures);


	if (vkPhysicalDeviceFeatures.tessellationShader)
	{
		fprintf(gpFile, "%s getPhysicalDevice :: Selected physical device supports tessellation shader \n", INFO_LOG);
	}
	else
	{
		fprintf(gpFile, "%s getPhysicalDevice :: Selected physical device does not support tessellation shader \n", ERROR_LOG);
	}




	if (vkPhysicalDeviceFeatures.geometryShader)
	{
		fprintf(gpFile, "%s getPhysicalDevice :: Selected physical device supports geometry shader \n", INFO_LOG);
	}
	else
	{
		fprintf(gpFile, "%s getPhysicalDevice :: Selected physical device supports geometry shader \n", INFO_LOG);
	}




	return vkResult;
}


VkResult printVkInfo(void)
{
	VkResult vkResult = VK_SUCCESS;


	fprintf(gpFile, "\n\n\n********************* VULKAN INFO **************************** \n\n");


	for (uint32_t i = 0; i < physicalDeviceCount; i++)
	{
		VkPhysicalDeviceProperties vkPhysicalDeviceProperties;
		memset((void*)&vkPhysicalDeviceProperties, 0, sizeof(VkPhysicalDeviceProperties));
		vkGetPhysicalDeviceProperties(vkPhysicalDevice_array[i], &vkPhysicalDeviceProperties);


		//major - minor - patch
		uint32_t majorVersion = VK_API_VERSION_MAJOR(vkPhysicalDeviceProperties.apiVersion);
		uint32_t minorVersion = VK_API_VERSION_MINOR(vkPhysicalDeviceProperties.apiVersion);
		uint32_t patchVersion = VK_API_VERSION_PATCH(vkPhysicalDeviceProperties.apiVersion);


		fprintf(gpFile, "apiVersion  = %d.%d.%d \n", majorVersion, minorVersion, patchVersion);
		fprintf(gpFile, "deviceName  = %s \n", vkPhysicalDeviceProperties.deviceName);


		switch (vkPhysicalDeviceProperties.deviceType)
		{
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			fprintf(gpFile, "deviceType  = Integrated GPU (iGPU) \n");
			break;


		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			fprintf(gpFile, "deviceType  = Discrete GPU (dGPU) \n");
			break;


		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			fprintf(gpFile, "deviceType  = Virtual GPU \n");
			break;


		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			fprintf(gpFile, "deviceType  = CPU \n");
			break;


		case VK_PHYSICAL_DEVICE_TYPE_OTHER:
			fprintf(gpFile, "deviceType  = other \n");
			break;


		default:
			fprintf(gpFile, "deviceType  = UNKNOWN \n");
		}


		fprintf(gpFile, "vendorId  = 0x%04x \n", vkPhysicalDeviceProperties.vendorID);
		fprintf(gpFile, "deviceID  = 0x%04x \n", vkPhysicalDeviceProperties.deviceID);
	}


	fprintf(gpFile, "\n\n********************* VULKAN INFO **************************** \n\n\n\n");


	if (vkPhysicalDevice_array)
	{
		free(vkPhysicalDevice_array);
		vkPhysicalDevice_array = NULL;
		fprintf(gpFile, "%s printVkInfo :: vkPhysicalDevice_array freed \n", INFO_LOG);


	}






	return vkResult;
}


VkResult fillDeviceExtensionNames(void)
{
	VkResult vkResult = VK_SUCCESS;


	uint32_t deviceExtensionCount = 0;
	vkResult = vkEnumerateDeviceExtensionProperties(vkPhysicalDevice_selected, NULL, &deviceExtensionCount, NULL);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s fillDeviceExtensionNames :: vkEnumerateDeviceExtensionProperties() getting count failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s fillDeviceExtensionNames :: vkEnumerateDeviceExtensionProperties() getting count succeeded \n", INFO_LOG);
	}


	VkExtensionProperties* vkExtensionProperties_array = NULL;
	vkExtensionProperties_array = (VkExtensionProperties*)malloc(sizeof(VkExtensionProperties) * deviceExtensionCount);
	if (vkExtensionProperties_array == NULL)
	{
		fprintf(gpFile, "%s fillDeviceExtensionNames :: vkExtensionProperties_array memory allocation failed \n", ERROR_LOG);
		return vkResult;
	}


	vkResult = vkEnumerateDeviceExtensionProperties(vkPhysicalDevice_selected, NULL, &deviceExtensionCount, vkExtensionProperties_array);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s fillDeviceExtensionNames :: vkEnumerateDeviceExtensionProperties() getting properties failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s fillDeviceExtensionNames :: vkEnumerateDeviceExtensionProperties() getting properties succeeded \n", INFO_LOG);
	}


	fprintf(gpFile, "%s fillDeviceExtensionNames :: deviceExtensionCount - %d \n", INFO_LOG, deviceExtensionCount);


	char** deviceExtensionNames_array = NULL;


	deviceExtensionNames_array = (char**)malloc(sizeof(char*) * deviceExtensionCount);
	if (deviceExtensionNames_array == NULL)
	{
		fprintf(gpFile, "%s fillDeviceExtensionNames :: deviceExtensionNames_array memory allocation failed \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}


	for (uint32_t i = 0; i < deviceExtensionCount; i++)
	{
		deviceExtensionNames_array[i] = (char*)malloc(sizeof(char) * (strlen(vkExtensionProperties_array[i].extensionName) + 1));
		if (deviceExtensionNames_array[i] == NULL)
		{
			fprintf(gpFile, "%s fillDeviceExtensionNames :: deviceExtensionNames_array[%d] memory allocation failed \n", ERROR_LOG, i);
			vkResult = VK_ERROR_INITIALIZATION_FAILED;
			return vkResult;
		}


		memcpy(deviceExtensionNames_array[i], vkExtensionProperties_array[i].extensionName, strlen(vkExtensionProperties_array[i].extensionName) + 1);
		fprintf(gpFile, "%s fillDeviceExtensionNames :: Vulkan Device Extension = %s \n", INFO_LOG, deviceExtensionNames_array[i]);


	}


	if (vkExtensionProperties_array)
	{
		free(vkExtensionProperties_array);
		vkExtensionProperties_array = NULL;
	}


	VkBool32 vulkanSwapChainExtensionFound = VK_FALSE;


	for (uint32_t i = 0; i < deviceExtensionCount; i++)
	{
		if (strcmp(deviceExtensionNames_array[i], VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
		{
			vulkanSwapChainExtensionFound = VK_TRUE;
			enabledDeviceExtensionNames_array[enableDeviceExtensionCount++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
		}
	}


	for (uint32_t i = 0; i < deviceExtensionCount; i++)
	{
		if (deviceExtensionNames_array[i])
		{
			free(deviceExtensionNames_array[i]);
			deviceExtensionNames_array[i] = NULL;
		}
	}
	if (deviceExtensionNames_array)
	{
		free(deviceExtensionNames_array);
		deviceExtensionNames_array = NULL;
	}


	if (vulkanSwapChainExtensionFound == VK_FALSE)
	{
		fprintf(gpFile, "%s fillDeviceExtensionNames :: VK_KHR_SWAPCHAIN_EXTENSION_NAME not found \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED; // return hardcoded failure
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s fillDeviceExtensionNames :: VK_KHR_SWAPCHAIN_EXTENSION_NAME found \n", INFO_LOG);
	}


	for (uint32_t i = 0; i < enableDeviceExtensionCount; i++)
	{
		fprintf(gpFile, "%s fillDeviceExtensionNames :: Enable vulkan device Extension Name = %s \n", INFO_LOG, enabledDeviceExtensionNames_array[i]);
	}


	return vkResult;
}


VkResult createVulkanDevice(void)
{
	//function declarations
	VkResult fillDeviceExtensionNames(void);


	// variable declarations
	VkResult vkResult = VK_SUCCESS;


	vkResult = fillDeviceExtensionNames();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createVulkanDevice :: fillDeviceExtensionNames() is failed \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createVulkanDevice :: fillDeviceExtensionNames() succeeded \n", INFO_LOG);
	}


	float queuePriority[1];
	queuePriority[0] = 1.0;
	VkDeviceQueueCreateInfo queueCreateInfo;
	memset(&queueCreateInfo, 0, sizeof(VkDeviceQueueCreateInfo));
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.pNext = NULL;
	queueCreateInfo.flags = 0;
	queueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex_Selected; // Ensure this is set correctly
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = queuePriority;


	VkDeviceCreateInfo vkCreateDeviceInfo;
	memset((void*)&vkCreateDeviceInfo, 0, sizeof(VkDeviceCreateInfo));
	vkCreateDeviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	vkCreateDeviceInfo.pNext = NULL;
	vkCreateDeviceInfo.flags = 0;
	vkCreateDeviceInfo.enabledExtensionCount = enableDeviceExtensionCount;
	vkCreateDeviceInfo.ppEnabledExtensionNames = enabledDeviceExtensionNames_array;
	vkCreateDeviceInfo.enabledLayerCount = 0;
	vkCreateDeviceInfo.ppEnabledLayerNames = NULL;
	vkCreateDeviceInfo.pEnabledFeatures = NULL;
	vkCreateDeviceInfo.queueCreateInfoCount = 0;
	vkCreateDeviceInfo.pQueueCreateInfos = NULL;
	vkCreateDeviceInfo.queueCreateInfoCount = 1;
	vkCreateDeviceInfo.pQueueCreateInfos = &queueCreateInfo;


	vkResult = vkCreateDevice(vkPhysicalDevice_selected, &vkCreateDeviceInfo, NULL, &vkdevice);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createVulkanDevice :: vkCreateDevice() is failed \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createVulkanDevice :: vkCreateDevice() succeeded \n", INFO_LOG);
	}


	return vkResult;
}




void getDeviceQueue(void)
{
	vkGetDeviceQueue(vkdevice, graphicsQueueFamilyIndex_Selected, 0, &vkQueue);
	if (vkQueue == VK_NULL_HANDLE)
	{
		fprintf(gpFile, "%s getDeviceQueue :: vkGetDeviceQueue() returned NULL \n", ERROR_LOG);
	}
	else
	{
		fprintf(gpFile, "%s getDeviceQueue :: vkGetDeviceQueue() succeeded \n", INFO_LOG);
	}
}


VkResult getPhysicalDeviceSurfaceFormatAndColorSpace(void)
{
	// variable declarations
	VkResult vkResult = VK_SUCCESS;
	uint32_t formatCount = 0;


	// get the count of supported color formats
	vkResult = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice_selected, vkSurfaceKHR, &formatCount, NULL);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s getPhysicalDeviceSurfaceFormatAndColorSpace :: vkGetPhysicalDeviceSurfaceFormatsKHR() is failed first time \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else if (formatCount == 0)
	{
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s getPhysicalDeviceSurfaceFormatAndColorSpace :: vkGetPhysicalDeviceSurfaceFormatsKHR() succeeded first time \n", INFO_LOG);
	}


	VkSurfaceFormatKHR* vkSurfacrFormatKHR_array = NULL;
	vkSurfacrFormatKHR_array = (VkSurfaceFormatKHR*)malloc(sizeof(VkSurfaceFormatKHR) * formatCount);


	if (vkSurfacrFormatKHR_array == NULL)
	{
		fprintf(gpFile, "%s getPhysicalDeviceSurfaceFormatAndColorSpace :: vkSurfacrFormatKHR_array memory allocation failed \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}


	vkResult = vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice_selected, vkSurfaceKHR, &formatCount, vkSurfacrFormatKHR_array);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s getPhysicalDeviceSurfaceFormatAndColorSpace :: vkGetPhysicalDeviceSurfaceFormatsKHR() is failed second time \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s getPhysicalDeviceSurfaceFormatAndColorSpace :: vkGetPhysicalDeviceSurfaceFormatsKHR() succeeded second time \n", INFO_LOG);
	}




	// decide color surface format first
	if (formatCount == 1 && vkSurfacrFormatKHR_array[0].format == VK_FORMAT_UNDEFINED)
	{
		vkFormat_color = VK_FORMAT_B8G8R8A8_UNORM;
	}
	else
	{
		vkFormat_color = vkSurfacrFormatKHR_array[0].format;
	}


	vkColorSpaceKHR = vkSurfacrFormatKHR_array[0].colorSpace;


	if (vkSurfacrFormatKHR_array)
	{
		free(vkSurfacrFormatKHR_array);
		vkSurfacrFormatKHR_array = NULL;
		fprintf(gpFile, "%s getPhysicalDeviceSurfaceFormatAndColorSpace :: vkSurfacrFormatKHR_array freed \n", INFO_LOG);
	}
	return vkResult;
}


VkResult getPhysicalDevicePresentMode(void)
{
	// variable declarations
	VkResult vkResult = VK_SUCCESS;


	//code
	uint32_t presentModeCount = 0;
	vkResult = vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice_selected, vkSurfaceKHR, &presentModeCount, NULL);


	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s getPhysicalDevicePresentMode :: vkGetPhysicalDeviceSurfacePresentModeKHR() is failed first time \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else if (presentModeCount == 0)
	{
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s getPhysicalDevicePresentMode :: vkGetPhysicalDeviceSurfacePresentModeKHR() succeeded first time \n", INFO_LOG);
	}


	VkPresentModeKHR* vkPresentModeKHR_array = NULL;
	vkPresentModeKHR_array = (VkPresentModeKHR*)malloc(sizeof(VkPresentModeKHR) * presentModeCount);
	if (vkPresentModeKHR_array == NULL)
	{
		fprintf(gpFile, "%s getPhysicalDevicePresentMode :: vkPresentModeKHR memory allocation failed \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}


	vkResult = vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice_selected, vkSurfaceKHR, &presentModeCount, vkPresentModeKHR_array);


	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s getPhysicalDevicePresentMode :: vkGetPhysicalDeviceSurfacePresentModeKHR() is failed second time \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s getPhysicalDevicePresentMode :: vkGetPhysicalDeviceSurfacePresentModeKHR() succeeded second time \n", INFO_LOG);
	}


	// decide presentation mode
	for (uint32_t i = 0; i < presentModeCount; i++)
	{
		if (vkPresentModeKHR_array[i] == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			vkPresentModeKHR = VK_PRESENT_MODE_MAILBOX_KHR;
			fprintf(gpFile, "%s getPhysicalDevicePresentMode :: vkPresentModeKHR set to VK_PRESENT_MODE_MAILBOX_KHR \n", INFO_LOG);
			break;
		}
	}


	if (vkPresentModeKHR != VK_PRESENT_MODE_MAILBOX_KHR)
	{
		vkPresentModeKHR = VK_PRESENT_MODE_FIFO_KHR;
		fprintf(gpFile, "%s getPhysicalDevicePresentMode :: vkPresentModeKHR set to VK_PRESENT_MODE_FIFO_KHR \n", INFO_LOG);
	}


	if (vkPresentModeKHR_array)
	{
		free(vkPresentModeKHR_array);
		vkPresentModeKHR_array = NULL;
		fprintf(gpFile, "%s getPhysicalDevicePresentMode :: vkPresentModeKHR_array is deallocated \n", INFO_LOG);
	}


	return vkResult;
}


VkResult createSwapchain(VkBool32 vsync) // vertical sync
{


	// function declarations
	VkResult getPhysicalDeviceSurfaceFormatAndColorSpace(void);
	VkResult getPhysicalDevicePresentMode(void);


	// variables declaration
	VkResult vkResult = VK_SUCCESS;


	// code
	vkResult = getPhysicalDeviceSurfaceFormatAndColorSpace();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createSwapChain :: getPhysicalDeviceSurfaceFormatAndColorSpace() is failed \n", ERROR_LOG);
	}
	else
	{
		fprintf(gpFile, "%s createSwapChain :: getPhysicalDeviceSurfaceFormatAndColorSpace() succeeded \n", INFO_LOG);
	}


	VkSurfaceCapabilitiesKHR vkSurfaceCapabilitesKHR;
	memset((void*)&vkSurfaceCapabilitesKHR, 0, sizeof(VkSurfaceCapabilitiesKHR));


	vkResult = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice_selected, vkSurfaceKHR, &vkSurfaceCapabilitesKHR);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createSwapChain :: vkGetPhysicalDeviceSurfaceCapabilitiesKHR()  failed \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createSwapChain :: vkGetPhysicalDeviceSurfaceCapabilitiesKHR() succeeded \n", INFO_LOG);
	}


	uint32_t testingNumberOfSwapchainImages = vkSurfaceCapabilitesKHR.minImageCount + 1;
	uint32_t desiredNumberOfSwapchainImages = 0;


	if (vkSurfaceCapabilitesKHR.maxImageCount > 0 && vkSurfaceCapabilitesKHR.maxImageCount < testingNumberOfSwapchainImages)
	{
		desiredNumberOfSwapchainImages = vkSurfaceCapabilitesKHR.maxImageCount;
	}
	else
	{
		desiredNumberOfSwapchainImages = vkSurfaceCapabilitesKHR.minImageCount;
	}
	fprintf(gpFile, "%s createSwapChain :: desiredNumberOfSwapchainImages = %d\n", INFO_LOG, desiredNumberOfSwapchainImages);
	fprintf(gpFile, "%s createSwapChain :: vkSurfaceCapabilitesKHR.maxImageCount = %d\n", INFO_LOG, vkSurfaceCapabilitesKHR.maxImageCount);
	fprintf(gpFile, "%s createSwapChain :: vkSurfaceCapabilitesKHR.minImageCount = %d\n", INFO_LOG, vkSurfaceCapabilitesKHR.minImageCount);


	memset((void*)&vkExtent2D_swapchain, 0, sizeof(VkExtent2D));
	if (vkSurfaceCapabilitesKHR.currentExtent.width != UINT32_MAX)
	{
		vkExtent2D_swapchain.width = vkSurfaceCapabilitesKHR.currentExtent.width;
		vkExtent2D_swapchain.height = vkSurfaceCapabilitesKHR.currentExtent.height;
		fprintf(gpFile, "%s createSwapChain :: Swapchain image Width X Height = %d X  %d\n", INFO_LOG, vkExtent2D_swapchain.width,
			vkExtent2D_swapchain.height);
	}
	else
	{
		// if surfaceSize is already defined then swapchain image size must match with it
		VkExtent2D vkExtent2D;
		memset((void*)&vkExtent2D, 0, sizeof(VkExtent2D));
		vkExtent2D.width = (uint32_t)winWidth;
		vkExtent2D.height = (uint32_t)winHeight;
		vkExtent2D_swapchain.width = glm::max(vkSurfaceCapabilitesKHR.minImageExtent.width,
			glm::min(vkSurfaceCapabilitesKHR.maxImageExtent.width, vkExtent2D.width));
		vkExtent2D_swapchain.height = glm::max(vkSurfaceCapabilitesKHR.minImageExtent.height,
			glm::min(vkSurfaceCapabilitesKHR.maxImageExtent.height, vkExtent2D.height));
		fprintf(gpFile, "%s createSwapChain :: Swapchain image Width X Height = %d X  %d\n", INFO_LOG, vkExtent2D_swapchain.width,
			vkExtent2D_swapchain.height);
	}


	VkImageUsageFlags vkImageUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	// VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT - compulsory
	// VK_IMAGE_USAGE_TRANSFER_SRC_BIT - maybe useful for fbo , texture , compute


	VkSurfaceTransformFlagBitsKHR vkSurfaceTransformFlagBitsKHR;
	if (vkSurfaceCapabilitesKHR.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
	{
		vkSurfaceTransformFlagBitsKHR = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	}
	else
	{
		vkSurfaceTransformFlagBitsKHR = vkSurfaceCapabilitesKHR.currentTransform;
	}


	vkResult = getPhysicalDevicePresentMode();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createSwapChain :: getPhysicalDevicePresentMode() is failed \n", ERROR_LOG);
	}
	else
	{
		fprintf(gpFile, "%s createSwapChain :: getPhysicalDevicePresentMode() succeeded \n", INFO_LOG);
	}


	VkSwapchainCreateInfoKHR vkSwapchainCreateInfoKHR;
	memset((void*)&vkSwapchainCreateInfoKHR, 0, sizeof(VkSwapchainCreateInfoKHR));
	vkSwapchainCreateInfoKHR.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	vkSwapchainCreateInfoKHR.pNext = NULL;
	vkSwapchainCreateInfoKHR.flags = 0;
	vkSwapchainCreateInfoKHR.surface = vkSurfaceKHR;
	vkSwapchainCreateInfoKHR.minImageCount = desiredNumberOfSwapchainImages;
	vkSwapchainCreateInfoKHR.imageFormat = vkFormat_color;
	vkSwapchainCreateInfoKHR.imageColorSpace = vkColorSpaceKHR;
	vkSwapchainCreateInfoKHR.imageExtent.width = vkExtent2D_swapchain.width;
	vkSwapchainCreateInfoKHR.imageExtent.height = vkExtent2D_swapchain.height;
	vkSwapchainCreateInfoKHR.imageUsage = vkImageUsageFlags;
	vkSwapchainCreateInfoKHR.preTransform = vkSurfaceTransformFlagBitsKHR;
	vkSwapchainCreateInfoKHR.imageArrayLayers = 1;
	vkSwapchainCreateInfoKHR.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	vkSwapchainCreateInfoKHR.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	vkSwapchainCreateInfoKHR.presentMode = vkPresentModeKHR;
	vkSwapchainCreateInfoKHR.clipped = VK_TRUE;


	vkResult = vkCreateSwapchainKHR(vkdevice, &vkSwapchainCreateInfoKHR, NULL, &vkSwapchainKHR);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createSwapChain :: vkCreateSwapchainKHR() is failed \n", ERROR_LOG);
	}
	else
	{
		fprintf(gpFile, "%s createSwapChain :: vkCreateSwapchainKHR() succeeded \n", INFO_LOG);
	}


	return vkResult;
}


VkResult createImagesAndImageViews(void)
{
	// function declaration
	VkResult getSupportedDepthFormat(void);


	// variables declaration
	VkResult vkResult = VK_SUCCESS;


	vkResult = vkGetSwapchainImagesKHR(vkdevice, vkSwapchainKHR, &swapchainImageCount, NULL);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createImagesAndImageViews :: vkGetSwapchainImagesKHR() is failed first call \n", ERROR_LOG);
		return vkResult;
	}
	else if (swapchainImageCount == 0)
	{
		fprintf(gpFile, "%s createImagesAndImageViews :: vkGetSwapchainImagesKHR() failed swapchainImageCount = 0\n", INFO_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createImagesAndImageViews :: swapchainImageCount = %d \n", INFO_LOG, swapchainImageCount);
		fprintf(gpFile, "%s createImagesAndImageViews :: vkGetSwapchainImagesKHR() succeeded first call \n", INFO_LOG);
	}


	swapchainImage_array = (VkImage*)malloc(sizeof(VkImage) * swapchainImageCount);
	if (swapchainImage_array == NULL)
	{
		fprintf(gpFile, "%s createImagesAndImageViews :: swapchainImage_array memory allocation failed \n", ERROR_LOG);
	}


	vkResult = vkGetSwapchainImagesKHR(vkdevice, vkSwapchainKHR, &swapchainImageCount, swapchainImage_array);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createImagesAndImageViews :: vkGetSwapchainImagesKHR() is failed second call \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createImagesAndImageViews :: vkGetSwapchainImagesKHR() succeeded second call \n", INFO_LOG);
	}


	swapchainImageView_array = (VkImageView*)malloc(sizeof(VkImageView) * swapchainImageCount);
	if (swapchainImageView_array == NULL)
	{
		fprintf(gpFile, "%s createImagesAndImageViews :: swapchainImageView_array memory allocation failed \n", ERROR_LOG);
	}


	VkImageViewCreateInfo vkImageViewCreateInfo;
	memset((void*)&vkImageViewCreateInfo, 0, sizeof(VkImageViewCreateInfo));


	vkImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	vkImageViewCreateInfo.pNext = NULL;
	vkImageViewCreateInfo.flags = 0;
	vkImageViewCreateInfo.format = vkFormat_color;
	vkImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R; //VkComponentMapping struct , VK_COMPONENT_SWIZZLE enum
	vkImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
	vkImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
	vkImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
	vkImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; //VkImageSubresourceRange struct
	vkImageViewCreateInfo.subresourceRange.baseMipLevel = 0; //same as parent mipmap level
	vkImageViewCreateInfo.subresourceRange.levelCount = 1;
	vkImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	vkImageViewCreateInfo.subresourceRange.layerCount = 1;
	vkImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;


	for (uint32_t i = 0; i < swapchainImageCount; i++)
	{
		vkImageViewCreateInfo.image = swapchainImage_array[i];
		vkResult = vkCreateImageView(vkdevice, &vkImageViewCreateInfo, NULL, &swapchainImageView_array[i]);
		if (vkResult != VK_SUCCESS)
		{
			fprintf(gpFile, "%s createImagesAndImageViews :: vkCreateImageView() is failed for iteration - %d\n", ERROR_LOG, i);
			return vkResult;
		}
		else
		{
			fprintf(gpFile, "%s createImagesAndImageViews :: vkCreateImageView() succeeded \n", INFO_LOG);
		}
	}


	// for Depth Image
	vkResult = getSupportedDepthFormat();
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createImagesAndImageViews :: getSupportedDepthFormat()  failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createImagesAndImageViews :: getSupportedDepthFormat() succeeded \n", INFO_LOG);
	}


	// for depth image initialize VkImageCreateInfo
	VkImageCreateInfo vkImageCreateInfo;
	memset((void*)&vkImageCreateInfo, 0, sizeof(VkImageCreateInfo));


	vkImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	vkImageCreateInfo.pNext = NULL;
	vkImageCreateInfo.flags = 0;
	vkImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	vkImageCreateInfo.format = vkFormat_Depth;
	vkImageCreateInfo.extent.width = winWidth;
	vkImageCreateInfo.extent.height = winHeight;
	vkImageCreateInfo.extent.depth = 1;
	vkImageCreateInfo.mipLevels = 1;
	vkImageCreateInfo.arrayLayers = 1;
	vkImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	vkImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	vkImageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;


	vkResult = vkCreateImage(vkdevice, &vkImageCreateInfo, NULL, &vkImage_Depth);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createImagesAndImageViews :: vkCreateImage() failed\n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createImagesAndImageViews :: vkCreateImage() succeeded \n", INFO_LOG);
	}


	// memory requirements for depth image


	VkMemoryRequirements vkMemoryRequirements;
	memset((void*)&vkMemoryRequirements, 0, sizeof(VkMemoryRequirements));


	// gets memory requirements
	vkGetImageMemoryRequirements(vkdevice, vkImage_Depth, &vkMemoryRequirements);


	VkMemoryAllocateInfo vkMemoryAllocateInfo;
	memset((void*)&vkMemoryAllocateInfo, 0, sizeof(VkMemoryAllocateInfo));
	vkMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	vkMemoryAllocateInfo.pNext = NULL;
	vkMemoryAllocateInfo.allocationSize = vkMemoryRequirements.size; //region size , which is also aligned
	vkMemoryAllocateInfo.memoryTypeIndex = 0; //initial value before entering the loop


	//memoryTypeIndex - we want that index that supports VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
	//VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT because CPU will be writing in it
	for (uint32_t i = 0; i < vkPhysicalDeviceMemoryProperties.memoryTypeCount; i++)
	{
		if ((vkMemoryRequirements.memoryTypeBits & 1) == 1)
		{
			if (vkPhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
			{
				vkMemoryAllocateInfo.memoryTypeIndex = i;
				break;
			}
		}
		vkMemoryRequirements.memoryTypeBits >>= 1;
	}


	vkResult = vkAllocateMemory(vkdevice, &vkMemoryAllocateInfo, NULL, &vkDeviceMemory_Depth); // allocates memory from vulkan heap
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createImagesAndImageViews :: vkAllocateMemory()  failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createImagesAndImageViews :: vkAllocateMemory() succeeded  \n", INFO_LOG);
	}


	vkResult = vkBindImageMemory(vkdevice, vkImage_Depth, vkDeviceMemory_Depth, 0); //binds vulkan devicememory object handle with vulkan buffer object handle
	//0 - from where to bind (offset) - source
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createImagesAndImageViews :: vkBindBufferMemory()  failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createImagesAndImageViews :: vkBindBufferMemory() succeeded  \n", INFO_LOG);
	}




	// create ImageView for above depth image
	memset((void*)&vkImageViewCreateInfo, 0, sizeof(VkImageViewCreateInfo));


	vkImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	vkImageViewCreateInfo.pNext = NULL;
	vkImageViewCreateInfo.flags = 0;
	vkImageViewCreateInfo.format = vkFormat_Depth;
	vkImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT; //VkImageSubresourceRange struct
	vkImageViewCreateInfo.subresourceRange.baseMipLevel = 0; //same as parent mipmap level
	vkImageViewCreateInfo.subresourceRange.levelCount = 1;
	vkImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	vkImageViewCreateInfo.subresourceRange.layerCount = 1;
	vkImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	vkImageViewCreateInfo.image = vkImage_Depth;


	vkResult = vkCreateImageView(vkdevice, &vkImageViewCreateInfo, NULL, &VkImageView_Depth);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createImagesAndImageViews :: vkCreateImageView() is failed for depth\n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createImagesAndImageViews :: vkCreateImageView() succeeded for depth \n", INFO_LOG);
	}


	return vkResult;
}


VkResult getSupportedDepthFormat(void)
{
	// variables declaration
	VkResult vkResult = VK_SUCCESS;
	VkFormat vkFormat_depth_array[] = {
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM
	};


	for (uint32_t i = 0; i < sizeof(vkFormat_depth_array) / sizeof(vkFormat_depth_array[0]);
		i++)
	{
		VkFormatProperties vkFormatProperties;
		memset((void*)&vkFormatProperties, 0, sizeof(VkFormatProperties));


		vkGetPhysicalDeviceFormatProperties(vkPhysicalDevice_selected, vkFormat_depth_array[i], &vkFormatProperties);
		if (vkFormatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			vkFormat_Depth = vkFormat_depth_array[i];
			vkResult = VK_SUCCESS;
			break;
		}
	}




	return vkResult;
}




VkResult createCommandPool(void)
{
	// variables declaration
	VkResult vkResult = VK_SUCCESS;


	VkCommandPoolCreateInfo vkCommandPoolCreateInfo;
	memset((void*)&vkCommandPoolCreateInfo, 0, sizeof(VkCommandPoolCreateInfo));
	vkCommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	vkCommandPoolCreateInfo.pNext = NULL;
	vkCommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // it can be reset and re-started
	vkCommandPoolCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex_Selected;


	vkResult = vkCreateCommandPool(vkdevice, &vkCommandPoolCreateInfo, NULL, &vkCommandPool);


	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createCommandPool :: vkCreateCommandPool() is failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createCommandPool :: vkCreateCommandPool() succeeded  \n", INFO_LOG);
	}


	return vkResult;
}


VkResult createCommandBuffers(void)
{
	// variables declaration
	VkResult vkResult = VK_SUCCESS;


	VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo;
	memset((void*)&vkCommandBufferAllocateInfo, 0, sizeof(VkCommandBufferAllocateInfo));


	vkCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	vkCommandBufferAllocateInfo.pNext = NULL;
	vkCommandBufferAllocateInfo.commandPool = vkCommandPool;
	vkCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // will go to queue
	vkCommandBufferAllocateInfo.commandBufferCount = 1;


	vkCommandBuffer_array = (VkCommandBuffer*)malloc(sizeof(VkCommandBuffer) * swapchainImageCount);
	if (vkCommandBuffer_array == NULL)
	{
		fprintf(gpFile, "%s createCommandBuffers :: vkCommandBuffer_array memory allocation failed \n", ERROR_LOG);
	}


	for (uint32_t i = 0; i < swapchainImageCount; i++)
	{
		vkResult = vkAllocateCommandBuffers(vkdevice, &vkCommandBufferAllocateInfo, &vkCommandBuffer_array[i]);
		if (vkResult != VK_SUCCESS)
		{
			fprintf(gpFile, "%s createCommandBuffers :: vkAllocateCommandBuffers() is failed at %d index\n", ERROR_LOG, i);
			return vkResult;
		}
		else
		{
			fprintf(gpFile, "%s createCommandBuffers :: vkAllocateCommandBuffers() succeeded  \n", INFO_LOG);
		}
	}


	return vkResult;
}


VkResult createVertexBuffers(void)
{
	// variables declaration
	VkResult vkResult = VK_SUCCESS;

	// code
	
	// vertex position buffer
	memset((void*)&vertexData_position, 0, sizeof(vertexData));


	VkBufferCreateInfo vkBufferCreateInfo;
	memset((void*)&vkBufferCreateInfo, 0, sizeof(VkBufferCreateInfo));
	vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vkBufferCreateInfo.pNext = NULL;
	vkBufferCreateInfo.flags = 0; // valid flags are used in scattered buffer or sparse buffer
	vkBufferCreateInfo.size = sizeof(float) * 3 * numVerts;
	vkBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	//vkBufferCreateInfo has other member .sharingMode  used to share same buffer between different queues.(VK_SHARING_MODE_CONCURRENT)
	//by default set to VK_SHARING_MODE_EXCLUSIVE
	//if set to VK_SHARING_MODE_CONCURRENT , we must set .queueFamilyIndexCount  and .pQueueFamilyIndices . This tells Vulkan which queue families will access the buffer.


/*
	.) Vulkan does not encourage allocating memory per resource (like per buffer or image).
	.) Instead of allocating memory in individual bytes, Vulkan recommends allocating large memory blocks (regions).
	.) The minimum allocation size is often 4096 bytes (4 KB) or more, depending on the GPU's requirements.
	.) This is a deliberate design choice by Vulkan.
	.) The Vulkan specification suggests allocating a small number of large-sized memory blocks.
	.) These blocks should then be reused repeatedly for different resources by sub-allocating from them.
*/


	vkResult = vkCreateBuffer(vkdevice, &vkBufferCreateInfo, NULL, &vertexData_position.vkBuffer);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createVertexBuffers :: vkCreateBuffer()  failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createVertexBuffers :: vkCreateBuffer() succeeded  \n", INFO_LOG);
	}


	VkMemoryRequirements vkMemoryRequirements;
	memset((void*)&vkMemoryRequirements, 0, sizeof(VkMemoryRequirements));


	vkGetBufferMemoryRequirements(vkdevice, vertexData_position.vkBuffer, &vkMemoryRequirements);


	VkMemoryAllocateInfo vkMemoryAllocateInfo;
	memset((void*)&vkMemoryAllocateInfo, 0, sizeof(VkMemoryAllocateInfo));
	vkMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	vkMemoryAllocateInfo.pNext = NULL;
	vkMemoryAllocateInfo.allocationSize = vkMemoryRequirements.size; //region size , which is also aligned
	vkMemoryAllocateInfo.memoryTypeIndex = 0; //initial value before entering the loop


	for (uint32_t i = 0; i < vkPhysicalDeviceMemoryProperties.memoryTypeCount; i++)
	{
		if ((vkMemoryRequirements.memoryTypeBits & 1) == 1)
		{
			if (vkPhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
			{
				vkMemoryAllocateInfo.memoryTypeIndex = i;
				break;
			}
		}
		vkMemoryRequirements.memoryTypeBits >>= 1;
	}


	vkResult = vkAllocateMemory(vkdevice, &vkMemoryAllocateInfo, NULL, &vertexData_position.vkDeviceMemory); // allocates memory from vulkan heap
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createVertexBuffers :: vkAllocateMemory()  failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createVertexBuffers :: vkAllocateMemory() succeeded  \n", INFO_LOG);
	}


	vkResult = vkBindBufferMemory(vkdevice, vertexData_position.vkBuffer, vertexData_position.vkDeviceMemory, 0); //binds vulkan devicememory object handle with vulkan buffer object handle
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createVertexBuffers :: vkBindBufferMemory()  failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createVertexBuffers :: vkBindBufferMemory() succeeded  \n", INFO_LOG);
	}


	void* data = NULL;
	vkResult = vkMapMemory(vkdevice,
		vertexData_position.vkDeviceMemory,
		0,
		vkMemoryAllocateInfo.allocationSize,
		0, //reserved
		&data);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createVertexBuffers :: vkMapMemory()  failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createVertexBuffers :: vkMapMemory() succeeded  \n", INFO_LOG);
	}


	memcpy(data, pPositions, sizeof(float) * 3 * numVerts);


	vkUnmapMemory(vkdevice, vertexData_position.vkDeviceMemory);

	// vertex normal buffer
	memset((void*)&vertexData_normal, 0, sizeof(vertexData));

	memset((void*)&vkBufferCreateInfo, 0, sizeof(VkBufferCreateInfo));
	vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vkBufferCreateInfo.pNext = NULL;
	vkBufferCreateInfo.flags = 0; // valid flags are used in scattered buffer or sparse buffer
	vkBufferCreateInfo.size = sizeof(float) * 3 * numVerts;
	vkBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

	vkResult = vkCreateBuffer(vkdevice, &vkBufferCreateInfo, NULL, &vertexData_normal.vkBuffer);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createVertexBuffers :: vkCreateBuffer()  failed for normal\n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createVertexBuffers :: vkCreateBuffer() succeeded for normal \n", INFO_LOG);
	}

	memset((void*)&vkMemoryRequirements, 0, sizeof(VkMemoryRequirements));

	vkGetBufferMemoryRequirements(vkdevice, vertexData_normal.vkBuffer, &vkMemoryRequirements);

	memset((void*)&vkMemoryAllocateInfo, 0, sizeof(VkMemoryAllocateInfo));
	vkMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	vkMemoryAllocateInfo.pNext = NULL;
	vkMemoryAllocateInfo.allocationSize = vkMemoryRequirements.size; // region size , which is also aligned
	vkMemoryAllocateInfo.memoryTypeIndex = 0;						 // initial value before entering the loop

	for (uint32_t i = 0; i < vkPhysicalDeviceMemoryProperties.memoryTypeCount; i++)
	{
		if ((vkMemoryRequirements.memoryTypeBits & 1) == 1)
		{
			if (vkPhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
			{
				vkMemoryAllocateInfo.memoryTypeIndex = i;
				break;
			}
		}
		vkMemoryRequirements.memoryTypeBits >>= 1;
	}

	vkResult = vkAllocateMemory(vkdevice, &vkMemoryAllocateInfo, NULL, &vertexData_normal.vkDeviceMemory); // allocates memory from vulkan heap
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createVertexBuffers :: vkAllocateMemory()  failed for normal \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createVertexBuffers :: vkAllocateMemory() succeeded for normal\n", INFO_LOG);
	}

	vkResult = vkBindBufferMemory(vkdevice, vertexData_normal.vkBuffer, vertexData_normal.vkDeviceMemory, 0); // binds vulkan devicememory object handle with vulkan buffer object handle
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createVertexBuffers :: vkBindBufferMemory()  failed for normal\n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createVertexBuffers :: vkBindBufferMemory() succeeded for normal\n", INFO_LOG);
	}

	data = NULL;
	vkResult = vkMapMemory(vkdevice,
		vertexData_normal.vkDeviceMemory,
		0,
		vkMemoryAllocateInfo.allocationSize,
		0, // reserved
		&data);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createVertexBuffers :: vkMapMemory()  failed for normal\n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createVertexBuffers :: vkMapMemory() succeeded for normal\n", INFO_LOG);
	}

	memcpy(data, pNormals, sizeof(float) * 3 * numVerts);

	vkUnmapMemory(vkdevice, vertexData_normal.vkDeviceMemory);

	// vertex texcords buffer
	memset((void*)&vertexData_texcord, 0, sizeof(vertexData));

	memset((void*)&vkBufferCreateInfo, 0, sizeof(VkBufferCreateInfo));
	vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vkBufferCreateInfo.pNext = NULL;
	vkBufferCreateInfo.flags = 0; // valid flags are used in scattered buffer or sparse buffer
	vkBufferCreateInfo.size = sizeof(float) * 2 * numVerts;
	vkBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

	vkResult = vkCreateBuffer(vkdevice, &vkBufferCreateInfo, NULL, &vertexData_texcord.vkBuffer);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createVertexBuffers :: vkCreateBuffer()  failed for texcords\n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createVertexBuffers :: vkCreateBuffer() succeeded for texcords \n", INFO_LOG);
	}

	memset((void*)&vkMemoryRequirements, 0, sizeof(VkMemoryRequirements));

	vkGetBufferMemoryRequirements(vkdevice, vertexData_texcord.vkBuffer, &vkMemoryRequirements);

	memset((void*)&vkMemoryAllocateInfo, 0, sizeof(VkMemoryAllocateInfo));
	vkMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	vkMemoryAllocateInfo.pNext = NULL;
	vkMemoryAllocateInfo.allocationSize = vkMemoryRequirements.size; // region size , which is also aligned
	vkMemoryAllocateInfo.memoryTypeIndex = 0;						 // initial value before entering the loop

	for (uint32_t i = 0; i < vkPhysicalDeviceMemoryProperties.memoryTypeCount; i++)
	{
		if ((vkMemoryRequirements.memoryTypeBits & 1) == 1)
		{
			if (vkPhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
			{
				vkMemoryAllocateInfo.memoryTypeIndex = i;
				break;
			}
		}
		vkMemoryRequirements.memoryTypeBits >>= 1;
	}

	vkResult = vkAllocateMemory(vkdevice, &vkMemoryAllocateInfo, NULL, &vertexData_texcord.vkDeviceMemory); // allocates memory from vulkan heap
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createVertexBuffers :: vkAllocateMemory()  failed for texcords \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createVertexBuffers :: vkAllocateMemory() succeeded for texcords\n", INFO_LOG);
	}

	vkResult = vkBindBufferMemory(vkdevice, vertexData_texcord.vkBuffer, vertexData_texcord.vkDeviceMemory, 0); // binds vulkan devicememory object handle with vulkan buffer object handle
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createVertexBuffers :: vkBindBufferMemory()  failed for texcords\n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createVertexBuffers :: vkBindBufferMemory() succeeded for texcords\n", INFO_LOG);
	}

	data = NULL;
	vkResult = vkMapMemory(vkdevice,
		vertexData_texcord.vkDeviceMemory,
		0,
		vkMemoryAllocateInfo.allocationSize,
		0, // reserved
		&data);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createVertexBuffers :: vkMapMemory()  failed for texcords\n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createVertexBuffers :: vkMapMemory() succeeded for texcords\n", INFO_LOG);
	}

	memcpy(data, pTexCoords, sizeof(float) * 2 * numVerts);

	vkUnmapMemory(vkdevice, vertexData_texcord.vkDeviceMemory);

	return vkResult;
}

VkResult createIndexBuffer(void)
{
	// variables declaration
	VkResult vkResult = VK_SUCCESS;


	// position index buffer
	VkBufferCreateInfo vkBufferCreateInfo;
	memset((void*)&vertexData_index, 0, sizeof(vertexData));

	memset((void*)&vkBufferCreateInfo, 0, sizeof(VkBufferCreateInfo));
	vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vkBufferCreateInfo.pNext = NULL;
	vkBufferCreateInfo.flags = 0; // valid flags are used in scattered buffer or sparse buffer
	vkBufferCreateInfo.size = sizeof(unsigned int) * numElements;
	vkBufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

	vkResult = vkCreateBuffer(vkdevice, &vkBufferCreateInfo, NULL, &vertexData_index.vkBuffer);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createIndexBuffer :: vkCreateBuffer()  failed for index buffer \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createIndexBuffer :: vkCreateBuffer() succeeded for index buffer \n", INFO_LOG);
	}
	VkMemoryRequirements vkMemoryRequirements;
	memset((void*)&vkMemoryRequirements, 0, sizeof(VkMemoryRequirements));

	vkGetBufferMemoryRequirements(vkdevice, vertexData_index.vkBuffer, &vkMemoryRequirements);

	VkMemoryAllocateInfo vkMemoryAllocateInfo;
	memset((void*)&vkMemoryAllocateInfo, 0, sizeof(VkMemoryAllocateInfo));
	vkMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	vkMemoryAllocateInfo.pNext = NULL;
	vkMemoryAllocateInfo.allocationSize = vkMemoryRequirements.size; //region size , which is also aligned
	vkMemoryAllocateInfo.memoryTypeIndex = 0; //initial value before entering the loop

	for (uint32_t i = 0; i < vkPhysicalDeviceMemoryProperties.memoryTypeCount; i++)
	{
		if ((vkMemoryRequirements.memoryTypeBits & 1) == 1)
		{
			if (vkPhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
			{
				vkMemoryAllocateInfo.memoryTypeIndex = i;
				break;
			}
		}
		vkMemoryRequirements.memoryTypeBits >>= 1;
	}

	vkResult = vkAllocateMemory(vkdevice, &vkMemoryAllocateInfo, NULL, &vertexData_index.vkDeviceMemory); // allocates memory from vulkan heap
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createIndexBuffer :: vkAllocateMemory()  failed for index buffer \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createIndexBuffer :: vkAllocateMemory() succeeded for index buffer \n", INFO_LOG);
	}

	vkResult = vkBindBufferMemory(vkdevice, vertexData_index.vkBuffer, vertexData_index.vkDeviceMemory, 0); //binds vulkan devicememory object handle with vulkan buffer object handle
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createIndexBuffer :: vkBindBufferMemory()  failed for index\n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createIndexBuffer :: vkBindBufferMemory() succeeded for index \n", INFO_LOG);
	}

	void* data = NULL;
	vkResult = vkMapMemory(vkdevice,
		vertexData_index.vkDeviceMemory,
		0,
		vkMemoryAllocateInfo.allocationSize,
		0, //reserved
		&data);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createIndexBuffer :: vkMapMemory()  failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createIndexBuffer :: vkMapMemory() succeeded  \n", INFO_LOG);
	}

	memcpy(data, pElements, sizeof(unsigned int) * numElements);

	vkUnmapMemory(vkdevice, vertexData_index.vkDeviceMemory);

	return vkResult;
}

VkResult createTexture(const char* textureFileName)
{
	// variables declaration
	VkResult vkResult = VK_SUCCESS;

	// code
	//1. get image data
	FILE* fp = NULL;
	fp = fopen(textureFileName, "rb");
	if (fp == NULL)
	{
		fprintf(gpFile, "%s createTexture :: fopen() failed for %s \n", ERROR_LOG, textureFileName);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}

	uint8_t* imageData = NULL;
	int texture_width, texture_height, texture_channels;
	imageData = stbi_load_from_file(fp, &texture_width, &texture_height, &texture_channels, STBI_rgb_alpha);
	if (imageData == NULL || texture_width <= 0 || texture_height <= 0 || texture_channels <= 0)
	{
		fprintf(gpFile, "%s createTexture :: stbi_load_from_file() failed for %s \n", ERROR_LOG, textureFileName);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}

	VkDeviceSize image_size = texture_width * texture_height * 4; //RGBA

	//2. create staging buffer
	VkBuffer vkBuffer_stagingBuffer = VK_NULL_HANDLE;
	VkDeviceMemory vkDeviceMemory_stagingBuffer = VK_NULL_HANDLE;

	VkBufferCreateInfo vkBufferCreateInfo_stagingBuffer;
	memset((void*)&vkBufferCreateInfo_stagingBuffer, 0, sizeof(VkBufferCreateInfo));
	vkBufferCreateInfo_stagingBuffer.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vkBufferCreateInfo_stagingBuffer.pNext = NULL;
	vkBufferCreateInfo_stagingBuffer.flags = 0; // valid flags are used in scattered buffer or sparse buffer
	vkBufferCreateInfo_stagingBuffer.size = image_size;
	vkBufferCreateInfo_stagingBuffer.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT; // this buffer is used as source buffer for transfer operation
	vkBufferCreateInfo_stagingBuffer.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // this buffer is not shared between different queues

	vkResult = vkCreateBuffer(vkdevice, &vkBufferCreateInfo_stagingBuffer, NULL, &vkBuffer_stagingBuffer);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createTexture :: vkCreateBuffer() failed for staging buffer \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createTexture :: vkCreateBuffer() succeeded for staging buffer \n", INFO_LOG);
	}

	VkMemoryRequirements vkMemoryRequirements_stagingBuffer;
	memset((void*)&vkMemoryRequirements_stagingBuffer, 0, sizeof(VkMemoryRequirements));

	vkGetBufferMemoryRequirements(vkdevice, vkBuffer_stagingBuffer, &vkMemoryRequirements_stagingBuffer);

	VkMemoryAllocateInfo vkMemoryAllocateInfo_stagingBuffer;
	memset((void*)&vkMemoryAllocateInfo_stagingBuffer, 0, sizeof(VkMemoryAllocateInfo));
	vkMemoryAllocateInfo_stagingBuffer.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	vkMemoryAllocateInfo_stagingBuffer.pNext = NULL;
	vkMemoryAllocateInfo_stagingBuffer.allocationSize = vkMemoryRequirements_stagingBuffer.size; //region size , which is also aligned
	vkMemoryAllocateInfo_stagingBuffer.memoryTypeIndex = 0;

	for (uint32_t i = 0; i < vkPhysicalDeviceMemoryProperties.memoryTypeCount; i++)
	{
		if ((vkMemoryRequirements_stagingBuffer.memoryTypeBits & 1) == 1)
		{
			if (vkPhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags &
				(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
			{
				vkMemoryAllocateInfo_stagingBuffer.memoryTypeIndex = i;
				break;
			}
		}
		vkMemoryRequirements_stagingBuffer.memoryTypeBits >>= 1;
	}

	vkResult = vkAllocateMemory(vkdevice, &vkMemoryAllocateInfo_stagingBuffer, NULL, &vkDeviceMemory_stagingBuffer);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createTexture :: vkAllocateMemory() failed for staging buffer \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createTexture :: vkAllocateMemory() succeeded for staging buffer \n", INFO_LOG);
	}

	vkResult = vkBindBufferMemory(vkdevice, vkBuffer_stagingBuffer, vkDeviceMemory_stagingBuffer, 0);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createTexture :: vkBindBufferMemory() failed for staging buffer \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createTexture :: vkBindBufferMemory() succeeded for staging buffer \n", INFO_LOG);
	}

	void* data = NULL;
	vkResult = vkMapMemory(vkdevice,
		vkDeviceMemory_stagingBuffer,
		0,
		image_size,
		0, //reserved
		&data);

	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createTexture :: vkMapMemory() failed for staging buffer \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createTexture :: vkMapMemory() succeeded for staging buffer \n", INFO_LOG);
	}

	memcpy(data, imageData, image_size);

	vkUnmapMemory(vkdevice, vkDeviceMemory_stagingBuffer);

	// as copy is done , we can free the imageData given by stbi_load_from_file()
	stbi_image_free(imageData);
	imageData = NULL;
	fprintf(gpFile, "%s createTexture :: stbi_image_free() succeeded for %s \n", INFO_LOG, textureFileName);

	//2. create device only visible , empty but enough sized vkImage  = size of image (imageWidth * imageHeight)
	VkImageCreateInfo vkImageCreateInfo;
	memset((void*)&vkImageCreateInfo, 0, sizeof(VkImageCreateInfo));
	vkImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	vkImageCreateInfo.pNext = NULL;
	vkImageCreateInfo.flags = 0; // valid flags are used in scattered buffer or sparse buffer
	vkImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;

	vkImageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM; //sRGB used at certain places
	vkImageCreateInfo.extent.width = texture_width;
	vkImageCreateInfo.extent.height = texture_height;
	vkImageCreateInfo.extent.depth = 1;
	vkImageCreateInfo.mipLevels = 1;
	vkImageCreateInfo.arrayLayers = 1;

	vkImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	vkImageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	vkImageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT; // this is destination image for transfer operation
	vkImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	vkImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; //initial layout of image is undefined , it will be defined later

	vkResult = vkCreateImage(vkdevice, &vkImageCreateInfo, NULL, &vkImage_texture);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createTexture :: vkCreateImage() failed for texture \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createTexture :: vkCreateImage() succeeded for texture \n", INFO_LOG);
	}

	VkMemoryRequirements vkMemoryRequirements_image;
	memset((void*)&vkMemoryRequirements_image, 0, sizeof(VkMemoryRequirements));

	vkGetImageMemoryRequirements(vkdevice, vkImage_texture, &vkMemoryRequirements_image);

	VkMemoryAllocateInfo vkMemoryAllocateInfo_image;
	memset((void*)&vkMemoryAllocateInfo_image, 0, sizeof(VkMemoryAllocateInfo));
	vkMemoryAllocateInfo_image.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	vkMemoryAllocateInfo_image.pNext = NULL;
	vkMemoryAllocateInfo_image.allocationSize = vkMemoryRequirements_image.size; //region size , which is also aligned
	vkMemoryAllocateInfo_image.memoryTypeIndex = 0;

	for (uint32_t i = 0; i < vkPhysicalDeviceMemoryProperties.memoryTypeCount; i++)
	{
		if ((vkMemoryRequirements_image.memoryTypeBits & 1) == 1)
		{
			if (vkPhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
			{
				vkMemoryAllocateInfo_image.memoryTypeIndex = i;
				break;
			}
		}
		vkMemoryRequirements_image.memoryTypeBits >>= 1;
	}

	vkResult = vkAllocateMemory(vkdevice, &vkMemoryAllocateInfo_image, NULL, &vkDeviceMemory_texture);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createTexture :: vkAllocateMemory() failed for texture \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createTexture :: vkAllocateMemory() succeeded for texture \n", INFO_LOG);
	}

	vkResult = vkBindImageMemory(vkdevice, vkImage_texture, vkDeviceMemory_texture, 0);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createTexture :: vkBindImageMemory() failed for texture \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createTexture :: vkBindImageMemory() succeeded for texture \n", INFO_LOG);
	}

	//3. send image trasition layout to vulkan/GPU before the actual copy of image from staging buffer to empty vkImage using pipeline barrier
	VkCommandBuffer vkCommandBuffer_transition_image_layout = VK_NULL_HANDLE;
	VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo_transition_image_layout;
	memset((void*)&vkCommandBufferAllocateInfo_transition_image_layout, 0, sizeof(VkCommandBufferAllocateInfo));
	vkCommandBufferAllocateInfo_transition_image_layout.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	vkCommandBufferAllocateInfo_transition_image_layout.pNext = NULL;
	vkCommandBufferAllocateInfo_transition_image_layout.commandPool = vkCommandPool;
	vkCommandBufferAllocateInfo_transition_image_layout.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	vkCommandBufferAllocateInfo_transition_image_layout.commandBufferCount = 1;

	vkResult = vkAllocateCommandBuffers(vkdevice, &vkCommandBufferAllocateInfo_transition_image_layout, &vkCommandBuffer_transition_image_layout);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createTexture :: vkAllocateCommandBuffers() failed for transition image layout \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createTexture :: vkAllocateCommandBuffers() succeeded for transition image layout \n", INFO_LOG);
	}

	VkCommandBufferBeginInfo vkCommandBufferBeginInfo_transition_image_layout;
	memset((void*)&vkCommandBufferBeginInfo_transition_image_layout, 0, sizeof(VkCommandBufferBeginInfo));
	vkCommandBufferBeginInfo_transition_image_layout.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkCommandBufferBeginInfo_transition_image_layout.pNext = NULL;
	vkCommandBufferBeginInfo_transition_image_layout.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkResult = vkBeginCommandBuffer(vkCommandBuffer_transition_image_layout, &vkCommandBufferBeginInfo_transition_image_layout);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createTexture :: vkBeginCommandBuffer() failed for transition image layout \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createTexture :: vkBeginCommandBuffer() succeeded for transition image layout \n", INFO_LOG);
	}

	VkPipelineStageFlags vkPipelineStageFlags_src = 0;
	VkPipelineStageFlags vkPipelineStageFlags_dst = 0;
	VkImageMemoryBarrier vkImageMemoryBarrier;
	memset((void*)&vkImageMemoryBarrier, 0, sizeof(VkImageMemoryBarrier));

	vkImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	vkImageMemoryBarrier.pNext = NULL;
	vkImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	vkImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	vkImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	vkImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	vkImageMemoryBarrier.image = vkImage_texture;
	vkImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	vkImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	vkImageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	vkImageMemoryBarrier.subresourceRange.layerCount = 1;
	vkImageMemoryBarrier.subresourceRange.levelCount = 1;

	if (vkImageMemoryBarrier.oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		vkImageMemoryBarrier.newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		vkImageMemoryBarrier.srcAccessMask = 0;
		vkImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		vkPipelineStageFlags_src = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		vkPipelineStageFlags_dst = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (vkImageMemoryBarrier.oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
		vkImageMemoryBarrier.newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		vkImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		vkImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		vkPipelineStageFlags_src = VK_PIPELINE_STAGE_TRANSFER_BIT;
		vkPipelineStageFlags_dst = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		fprintf(gpFile, "%s createTexture :: unsupported layout transition \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}

	vkCmdPipelineBarrier(vkCommandBuffer_transition_image_layout,
		vkPipelineStageFlags_src,
		vkPipelineStageFlags_dst,
		0,
		0,
		NULL,
		0,
		NULL,
		1,
		&vkImageMemoryBarrier);

	vkResult = vkEndCommandBuffer(vkCommandBuffer_transition_image_layout);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createTexture :: vkEndCommandBuffer() failed for transition image layout \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createTexture :: vkEndCommandBuffer() succeeded for transition image layout \n", INFO_LOG);
	}

	VkSubmitInfo vkSubmitInfo_transition_image_layout;
	memset((void*)&vkSubmitInfo_transition_image_layout, 0, sizeof(VkSubmitInfo));
	vkSubmitInfo_transition_image_layout.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	vkSubmitInfo_transition_image_layout.pNext = NULL;
	vkSubmitInfo_transition_image_layout.commandBufferCount = 1;
	vkSubmitInfo_transition_image_layout.pCommandBuffers = &vkCommandBuffer_transition_image_layout;

	vkResult = vkQueueSubmit(vkQueue, 1, &vkSubmitInfo_transition_image_layout, VK_NULL_HANDLE);

	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createTexture :: vkQueueSubmit() failed for transition image layout \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createTexture :: vkQueueSubmit() succeeded for transition image layout \n", INFO_LOG);
	}

	vkResult = vkQueueWaitIdle(vkQueue);
	vkFreeCommandBuffers(vkdevice, vkCommandPool, 1, &vkCommandBuffer_transition_image_layout);

	//4. copy image from staging buffer to vkImage
	VkCommandBuffer vkCommandBuffer_buffer_to_image_copy = VK_NULL_HANDLE;
	VkCommandBufferAllocateInfo vkCommandBufferAllocateInfo_buffer_to_image_copy;
	memset((void*)&vkCommandBufferAllocateInfo_buffer_to_image_copy, 0, sizeof(VkCommandBufferAllocateInfo));
	vkCommandBufferAllocateInfo_buffer_to_image_copy.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	vkCommandBufferAllocateInfo_buffer_to_image_copy.pNext = NULL;
	vkCommandBufferAllocateInfo_buffer_to_image_copy.commandPool = vkCommandPool;
	vkCommandBufferAllocateInfo_buffer_to_image_copy.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	vkCommandBufferAllocateInfo_buffer_to_image_copy.commandBufferCount = 1;

	vkResult = vkAllocateCommandBuffers(vkdevice, &vkCommandBufferAllocateInfo_buffer_to_image_copy, &vkCommandBuffer_buffer_to_image_copy);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createTexture :: vkAllocateCommandBuffers() failed for buffer to image copy \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createTexture :: vkAllocateCommandBuffers() succeeded for buffer to image copy \n", INFO_LOG);
	}

	VkCommandBufferBeginInfo vkCommandBufferBeginInfo_buffer_to_image_copy;
	memset((void*)&vkCommandBufferBeginInfo_buffer_to_image_copy, 0, sizeof(VkCommandBufferBeginInfo));
	vkCommandBufferBeginInfo_buffer_to_image_copy.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkCommandBufferBeginInfo_buffer_to_image_copy.pNext = NULL;
	vkCommandBufferBeginInfo_buffer_to_image_copy.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkResult = vkBeginCommandBuffer(vkCommandBuffer_buffer_to_image_copy, &vkCommandBufferBeginInfo_buffer_to_image_copy);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createTexture :: vkBeginCommandBuffer() failed for buffer to image copy \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createTexture :: vkBeginCommandBuffer() succeeded for buffer to image copy \n", INFO_LOG);
	}

	VkBufferImageCopy vkBufferImageCopy;
	memset((void*)&vkBufferImageCopy, 0, sizeof(VkBufferImageCopy));
	vkBufferImageCopy.bufferOffset = 0; // where to start
	vkBufferImageCopy.bufferRowLength = 0; // 0 means no row length
	vkBufferImageCopy.bufferImageHeight = 0; // 0 means no image height
	vkBufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	vkBufferImageCopy.imageSubresource.mipLevel = 0;
	vkBufferImageCopy.imageSubresource.baseArrayLayer = 0;
	vkBufferImageCopy.imageSubresource.layerCount = 1;
	vkBufferImageCopy.imageOffset.x = 0;
	vkBufferImageCopy.imageOffset.y = 0;
	vkBufferImageCopy.imageOffset.z = 0;
	vkBufferImageCopy.imageExtent.width = texture_width;
	vkBufferImageCopy.imageExtent.height = texture_height;
	vkBufferImageCopy.imageExtent.depth = 1;

	vkCmdCopyBufferToImage(vkCommandBuffer_buffer_to_image_copy,
		vkBuffer_stagingBuffer,
		vkImage_texture,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&vkBufferImageCopy);

	vkResult = vkEndCommandBuffer(vkCommandBuffer_buffer_to_image_copy);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createTexture :: vkEndCommandBuffer() failed for buffer to image copy \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createTexture :: vkEndCommandBuffer() succeeded for buffer to image copy \n", INFO_LOG);
	}

	VkSubmitInfo vkSubmitInfo_buffer_to_image_copy;
	memset((void*)&vkSubmitInfo_buffer_to_image_copy, 0, sizeof(VkSubmitInfo));
	vkSubmitInfo_buffer_to_image_copy.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	vkSubmitInfo_buffer_to_image_copy.pNext = NULL;
	vkSubmitInfo_buffer_to_image_copy.commandBufferCount = 1;
	vkSubmitInfo_buffer_to_image_copy.pCommandBuffers = &vkCommandBuffer_buffer_to_image_copy;

	vkResult = vkQueueSubmit(vkQueue, 1, &vkSubmitInfo_buffer_to_image_copy, VK_NULL_HANDLE);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createTexture :: vkQueueSubmit() failed for buffer to image copy \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createTexture :: vkQueueSubmit() succeeded for buffer to image copy \n", INFO_LOG);
	}

	vkResult = vkQueueWaitIdle(vkQueue);
	vkFreeCommandBuffers(vkdevice, vkCommandPool, 1, &vkCommandBuffer_buffer_to_image_copy);

	//5. transition image layout to shader read only optimal
	vkCommandBuffer_transition_image_layout = VK_NULL_HANDLE;
	memset((void*)&vkCommandBufferAllocateInfo_transition_image_layout, 0, sizeof(VkCommandBufferAllocateInfo));
	vkCommandBufferAllocateInfo_transition_image_layout.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	vkCommandBufferAllocateInfo_transition_image_layout.pNext = NULL;
	vkCommandBufferAllocateInfo_transition_image_layout.commandPool = vkCommandPool;
	vkCommandBufferAllocateInfo_transition_image_layout.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	vkCommandBufferAllocateInfo_transition_image_layout.commandBufferCount = 1;

	vkResult = vkAllocateCommandBuffers(vkdevice, &vkCommandBufferAllocateInfo_transition_image_layout, &vkCommandBuffer_transition_image_layout);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createTexture :: vkAllocateCommandBuffers() failed for transition image layout \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createTexture :: vkAllocateCommandBuffers() succeeded for transition image layout \n", INFO_LOG);
	}

	memset((void*)&vkCommandBufferBeginInfo_transition_image_layout, 0, sizeof(VkCommandBufferBeginInfo));
	vkCommandBufferBeginInfo_transition_image_layout.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkCommandBufferBeginInfo_transition_image_layout.pNext = NULL;
	vkCommandBufferBeginInfo_transition_image_layout.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkResult = vkBeginCommandBuffer(vkCommandBuffer_transition_image_layout, &vkCommandBufferBeginInfo_transition_image_layout);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createTexture :: vkBeginCommandBuffer() failed for transition image layout \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createTexture :: vkBeginCommandBuffer() succeeded for transition image layout \n", INFO_LOG);
	}

	vkPipelineStageFlags_src = 0;
	vkPipelineStageFlags_dst = 0;
	memset((void*)&vkImageMemoryBarrier, 0, sizeof(VkImageMemoryBarrier));

	vkImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	vkImageMemoryBarrier.pNext = NULL;
	vkImageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	vkImageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	vkImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	vkImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	vkImageMemoryBarrier.image = vkImage_texture;
	vkImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	vkImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	vkImageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	vkImageMemoryBarrier.subresourceRange.layerCount = 1;
	vkImageMemoryBarrier.subresourceRange.levelCount = 1;

	if (vkImageMemoryBarrier.oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		vkImageMemoryBarrier.newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		vkImageMemoryBarrier.srcAccessMask = 0;
		vkImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		vkPipelineStageFlags_src = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		vkPipelineStageFlags_dst = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (vkImageMemoryBarrier.oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
		vkImageMemoryBarrier.newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		vkImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		vkImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		vkPipelineStageFlags_src = VK_PIPELINE_STAGE_TRANSFER_BIT;
		vkPipelineStageFlags_dst = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		fprintf(gpFile, "%s createTexture :: unsupported layout transition \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}

	vkCmdPipelineBarrier(vkCommandBuffer_transition_image_layout,
		vkPipelineStageFlags_src,
		vkPipelineStageFlags_dst,
		0,
		0,
		NULL,
		0,
		NULL,
		1,
		&vkImageMemoryBarrier);

	vkResult = vkEndCommandBuffer(vkCommandBuffer_transition_image_layout);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createTexture :: vkEndCommandBuffer() failed for transition image layout \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createTexture :: vkEndCommandBuffer() succeeded for transition image layout \n", INFO_LOG);
	}

	memset((void*)&vkSubmitInfo_transition_image_layout, 0, sizeof(VkSubmitInfo));
	vkSubmitInfo_transition_image_layout.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	vkSubmitInfo_transition_image_layout.pNext = NULL;
	vkSubmitInfo_transition_image_layout.commandBufferCount = 1;
	vkSubmitInfo_transition_image_layout.pCommandBuffers = &vkCommandBuffer_transition_image_layout;

	vkResult = vkQueueSubmit(vkQueue, 1, &vkSubmitInfo_transition_image_layout, VK_NULL_HANDLE);

	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createTexture :: vkQueueSubmit() failed for transition image layout \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createTexture :: vkQueueSubmit() succeeded for transition image layout \n", INFO_LOG);
	}

	vkResult = vkQueueWaitIdle(vkQueue);
	vkFreeCommandBuffers(vkdevice, vkCommandPool, 1, &vkCommandBuffer_transition_image_layout);


	//6. uninitialize staging buffer
	if (vkBuffer_stagingBuffer)
	{
		vkDestroyBuffer(vkdevice, vkBuffer_stagingBuffer, NULL);
		vkBuffer_stagingBuffer = VK_NULL_HANDLE;
	}

	if (vkDeviceMemory_stagingBuffer)
	{
		vkFreeMemory(vkdevice, vkDeviceMemory_stagingBuffer, NULL);
		vkDeviceMemory_stagingBuffer = VK_NULL_HANDLE;
	}

	//7. create image view for texture
	VkImageViewCreateInfo vkImageViewCreateInfo;
	memset((void*)&vkImageViewCreateInfo, 0, sizeof(VkImageViewCreateInfo));
	vkImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	vkImageViewCreateInfo.pNext = NULL;
	vkImageViewCreateInfo.image = vkImage_texture;
	vkImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	vkImageViewCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	vkImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	vkImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	vkImageViewCreateInfo.subresourceRange.levelCount = 1;
	vkImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	vkImageViewCreateInfo.subresourceRange.layerCount = 1;


	vkResult = vkCreateImageView(vkdevice, &vkImageViewCreateInfo, NULL, &vkImageView_texture);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createTexture :: vkCreateImageView() is failed for texture\n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createTexture :: vkCreateImageView() succeeded for texture \n", INFO_LOG);
	}

	//8. create sampler for texture
	VkSamplerCreateInfo vkSamplerCreateInfo;
	memset((void*)&vkSamplerCreateInfo, 0, sizeof(VkSamplerCreateInfo));
	vkSamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	vkSamplerCreateInfo.pNext = NULL;
	vkSamplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	vkSamplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	vkSamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	vkSamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	vkSamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	vkSamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	vkSamplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	vkSamplerCreateInfo.anisotropyEnable = VK_FALSE; // anisotropic filtering is not supported
	vkSamplerCreateInfo.maxAnisotropy = 16.0f;
	vkSamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	vkSamplerCreateInfo.compareEnable = VK_FALSE;
	vkSamplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;

	vkResult = vkCreateSampler(vkdevice, &vkSamplerCreateInfo, NULL, &vkSampler_texture);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createTexture :: vkCreateSampler() is failed for texture\n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createTexture :: vkCreateSampler() succeeded for texture \n", INFO_LOG);
	}

	return vkResult;
}


VkResult createUniformBuffer(void)
{
	// function declarations
	VkResult updateUniformBuffer(void);


	// variables declaration
	VkResult vkResult = VK_SUCCESS;


	// code
	VkBufferCreateInfo vkBufferCreateInfo;
	memset((void*)&vkBufferCreateInfo, 0, sizeof(VkBufferCreateInfo));
	vkBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vkBufferCreateInfo.pNext = NULL;
	vkBufferCreateInfo.flags = 0; // valid flags are used in scattered buffer or sparse buffer
	vkBufferCreateInfo.size = sizeof(MyUniformData);
	vkBufferCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;


	memset((void*)&uniformData, 0, sizeof(UniformData));


	vkResult = vkCreateBuffer(vkdevice, &vkBufferCreateInfo, NULL, &uniformData.vkBuffer);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createUniformBuffer :: vkCreateBuffer()  failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createUniformBuffer :: vkCreateBuffer() succeeded  \n", INFO_LOG);
	}


	VkMemoryRequirements vkMemoryRequirements;
	memset((void*)&vkMemoryRequirements, 0, sizeof(VkMemoryRequirements));


	// gets memory requirements
	vkGetBufferMemoryRequirements(vkdevice, uniformData.vkBuffer, &vkMemoryRequirements);


	VkMemoryAllocateInfo vkMemoryAllocateInfo;
	memset((void*)&vkMemoryAllocateInfo, 0, sizeof(VkMemoryAllocateInfo));
	vkMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	vkMemoryAllocateInfo.pNext = NULL;
	vkMemoryAllocateInfo.allocationSize = vkMemoryRequirements.size; //region size , which is also aligned
	vkMemoryAllocateInfo.memoryTypeIndex = 0; //initial value before entering the loop


	//memoryTypeIndex - we want that index that supports VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
	//VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT because CPU will be writing in it
	for (uint32_t i = 0; i < vkPhysicalDeviceMemoryProperties.memoryTypeCount; i++)
	{
		if ((vkMemoryRequirements.memoryTypeBits & 1) == 1)
		{
			if (vkPhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
			{
				vkMemoryAllocateInfo.memoryTypeIndex = i;
				break;
			}
		}
		vkMemoryRequirements.memoryTypeBits >>= 1;
	}


	vkResult = vkAllocateMemory(vkdevice, &vkMemoryAllocateInfo, NULL, &uniformData.vkDeviceMemory); // allocates memory from vulkan heap
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createUniformBuffer :: vkAllocateMemory()  failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createUniformBuffer :: vkAllocateMemory() succeeded  \n", INFO_LOG);
	}


	vkResult = vkBindBufferMemory(vkdevice, uniformData.vkBuffer, uniformData.vkDeviceMemory, 0); //binds vulkan devicememory object handle with vulkan buffer object handle
	//0 - from where to bind (offset) - source
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createUniformBuffer :: vkBindBufferMemory()  failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createUniformBuffer :: vkBindBufferMemory() succeeded  \n", INFO_LOG);
	}
	return vkResult;


}


VkResult updateUniformBuffer(void)
{
	// variables declaration
	VkResult vkResult = VK_SUCCESS;


	// code
	MyUniformData myUniformData;
	memset((void*)&myUniformData, 0, sizeof(MyUniformData));


	// update matrices
	myUniformData.modelMatrix = glm::mat4(1.0);
	glm::mat4 translationMatrix = glm::mat4(1.0);
	translationMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, -2.0f));

	glm::mat4 rotationMatrix = glm::mat4(1.0);
	rotationMatrix = glm::rotate(glm::mat4(1.0), glm::radians(angleTeapot), glm::vec3(0.0f,1.0f,0.0f));
	myUniformData.modelMatrix = translationMatrix * rotationMatrix;
	myUniformData.viewMatrix = glm::mat4(1.0);
	glm::mat4 perspectiveProjectionMatrix = glm::mat4(1.0);


	perspectiveProjectionMatrix = glm::perspective(glm::radians(45.0f),
		(float)winWidth / (float)winHeight,
		0.1f,
		100.0f);


	/*
	| 00 01 02 03 |
	| 10 11 12 13 |
	| 20 21 22 23 |
	| 30 31 32 33 |
	*/
	perspectiveProjectionMatrix[1][1] = perspectiveProjectionMatrix[1][1] * (-1.0f);


	myUniformData.projectionMatrix = perspectiveProjectionMatrix;


	// light related uniform
	myUniformData.lightDiffuse[0] = 1.0f;
	myUniformData.lightDiffuse[1] = 1.0f;
	myUniformData.lightDiffuse[2] = 1.0f;
	myUniformData.lightDiffuse[3] = 1.0f;


	myUniformData.lightPosition[0] = 0.0f;
	myUniformData.lightPosition[1] = 0.0f;
	myUniformData.lightPosition[2] = 2.0f;
	myUniformData.lightPosition[3] = 1.0f;


	myUniformData.materialDiffuse[0] = 0.9f;
	myUniformData.materialDiffuse[1] = 0.9f;
	myUniformData.materialDiffuse[2] = 0.9f;
	myUniformData.materialDiffuse[3] = 1.0f;


	// key pressed related uniform
	if (bLight == TRUE)
	{
		myUniformData.lKeyIsPressed = 1;
	}
	else
	{
		myUniformData.lKeyIsPressed = 0;
	}

	if (bTexture == TRUE)
	{
		myUniformData.tKeyIsPressed = 1;
	}
	else
	{
		myUniformData.tKeyIsPressed = 0;
	}


	void* data = NULL;
	vkResult = vkMapMemory(vkdevice,
		uniformData.vkDeviceMemory,
		0, // offset start at beginning
		sizeof(MyUniformData), // size to map
		0, // flags (reserved)
		&data); // cpu pointer to mapped gpu memory
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s updateUniformBuffer :: vkMapMemory()  failed \n", ERROR_LOG);
		return vkResult;
	}


	memcpy(data, &myUniformData, sizeof(MyUniformData)); //dest - src - size


	vkUnmapMemory(vkdevice, uniformData.vkDeviceMemory); //unmap - gpu can now safely read


	return vkResult;
}


VkResult createShaders(void)
{
	// variables declaration
	VkResult vkResult = VK_SUCCESS;


	//code
	// vertex-shader
	const char* szFileName = "shader.vert.spv";
	FILE* fp = NULL;
	size_t size;


	fp = fopen(szFileName, "rb"); // read binary
	if (fp == NULL)
	{
		fprintf(gpFile, "%s createShaders :: fp == NULL \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createShaders :: fp opened \n", INFO_LOG);
	}
	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	if (size == 0)
	{
		fprintf(gpFile, "%s createShaders :: file size 0  \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	fseek(fp, 0L, SEEK_SET);


	char* shaderData = (char*)malloc(sizeof(char) * size);
	size_t retVal = fread(shaderData, size, 1, fp);
	if (retVal != 1)
	{
		fprintf(gpFile, "%s createShaders :: retVal !=1 \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createShaders :: fread success \n", INFO_LOG);
	}
	fclose(fp);


	VkShaderModuleCreateInfo vkShaderModuleCreateInfo;
	memset((void*)&vkShaderModuleCreateInfo, 0, sizeof(VkShaderModuleCreateInfo));
	vkShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vkShaderModuleCreateInfo.pNext = NULL;
	vkShaderModuleCreateInfo.flags = 0; // reserved for future use
	vkShaderModuleCreateInfo.codeSize = size;
	vkShaderModuleCreateInfo.pCode = (uint32_t*)shaderData;


	vkResult = vkCreateShaderModule(vkdevice, &vkShaderModuleCreateInfo, NULL, &vkShaderModule_vertex_shader);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createShaders :: vkCreateShaderModule() is failed  call \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createShaders :: vkCreateShaderModule() succeeded \n", INFO_LOG);
	}


	if (shaderData)
	{
		free(shaderData);
		shaderData = NULL;
	}
	fprintf(gpFile, "%s createShaders :: Vertex Shader Module created()\n", INFO_LOG);


	// fragment-shader
	szFileName = "shader.frag.spv";
	fp = NULL;
	size = 0;


	fp = fopen(szFileName, "rb"); // read binary
	if (fp == NULL)
	{
		fprintf(gpFile, "%s createShaders :: fp == NULL fragment shader\n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createShaders :: fp opened fragment shader\n", INFO_LOG);
	}
	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	if (size == 0)
	{
		fprintf(gpFile, "%s createShaders :: file size 0  for fragment shader\n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	fseek(fp, 0L, SEEK_SET);


	shaderData = (char*)malloc(sizeof(char) * size);
	retVal = fread(shaderData, size, 1, fp);
	if (retVal != 1)
	{
		fprintf(gpFile, "%s createShaders :: retVal !=1 fragment shader\n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createShaders :: fread success \n", INFO_LOG);
	}
	fclose(fp);


	memset((void*)&vkShaderModuleCreateInfo, 0, sizeof(VkShaderModuleCreateInfo));
	vkShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vkShaderModuleCreateInfo.pNext = NULL;
	vkShaderModuleCreateInfo.flags = 0; // reserved for future use
	vkShaderModuleCreateInfo.codeSize = size;
	vkShaderModuleCreateInfo.pCode = (uint32_t*)shaderData;


	vkResult = vkCreateShaderModule(vkdevice, &vkShaderModuleCreateInfo, NULL, &vkShaderModule_fragment_shader);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createShaders :: vkCreateShaderModule() is failed fragment shader \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createShaders :: vkCreateShaderModule() succeeded fragment shader \n", INFO_LOG);
	}


	if (shaderData)
	{
		free(shaderData);
		shaderData = NULL;
	}
	fprintf(gpFile, "%s createShaders :: Fragment Shader Module created()\n", INFO_LOG);


	return vkResult;
}

VkResult createDescriptorSetLayout(void)
{
	// variables declaration
	VkResult vkResult = VK_SUCCESS;

	/*
	typedef struct VkDescriptorSetLayoutBinding {
		uint32_t              binding;               // Binding number in the shader
		VkDescriptorType      descriptorType;        // Type of descriptor (e.g., uniform buffer, sampler)
		uint32_t              descriptorCount;       // Number of descriptors in the binding (e.g., array size)
		VkShaderStageFlags    stageFlags;            // Shader stages this binding is visible to
		const VkSampler*      pImmutableSamplers;    // Optional: static samplers for this binding , across shaders
	} VkDescriptorSetLayoutBinding;
*/

// initialize descriptor set binding
	VkDescriptorSetLayoutBinding vkDescriptorSetLayoutBinding_array[2];
	memset((void*)vkDescriptorSetLayoutBinding_array, 0, sizeof(VkDescriptorSetLayoutBinding) * _ARRAYSIZE(vkDescriptorSetLayoutBinding_array));

	// for MVP uniform buffer
	vkDescriptorSetLayoutBinding_array[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	vkDescriptorSetLayoutBinding_array[0].binding = 0; //0th index in shader
	vkDescriptorSetLayoutBinding_array[0].descriptorCount = 1;
	vkDescriptorSetLayoutBinding_array[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	vkDescriptorSetLayoutBinding_array[0].pImmutableSamplers = NULL;

	// for texture image & sampler
	vkDescriptorSetLayoutBinding_array[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	vkDescriptorSetLayoutBinding_array[1].binding = 1; //1st index in shader
	vkDescriptorSetLayoutBinding_array[1].descriptorCount = 1;
	vkDescriptorSetLayoutBinding_array[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	vkDescriptorSetLayoutBinding_array[1].pImmutableSamplers = NULL;

	VkDescriptorSetLayoutCreateInfo vkDescriptorSetLayoutCreateInfo;
	memset((void*)&vkDescriptorSetLayoutCreateInfo, 0, sizeof(VkDescriptorSetLayoutCreateInfo));
	vkDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	vkDescriptorSetLayoutCreateInfo.pNext = NULL;
	vkDescriptorSetLayoutCreateInfo.flags = 0; //reserved
	vkDescriptorSetLayoutCreateInfo.bindingCount = _ARRAYSIZE(vkDescriptorSetLayoutBinding_array);
	vkDescriptorSetLayoutCreateInfo.pBindings = vkDescriptorSetLayoutBinding_array; // array of VkDescriptorSetLayoutBinding



	vkResult = vkCreateDescriptorSetLayout(vkdevice, &vkDescriptorSetLayoutCreateInfo, NULL, &vkDescriptorSetLayout);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createDescriptorSetLayout :: vkCreateDescriptorSetLayout()  failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createDescriptorSetLayout :: vkCreateDescriptorSetLayout() succeeded  \n", INFO_LOG);
	}
	return vkResult;
}




VkResult createPipelineLayout(void)
{
	// variables declaration
	VkResult vkResult = VK_SUCCESS;


	VkPipelineLayoutCreateInfo  vkPipelineLayoutCreateInfo;
	memset((void*)&vkPipelineLayoutCreateInfo, 0, sizeof(VkPipelineLayoutCreateInfo));
	vkPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	vkPipelineLayoutCreateInfo.pNext = NULL;
	vkPipelineLayoutCreateInfo.flags = 0; //reserved
	vkPipelineLayoutCreateInfo.setLayoutCount = 1;
	vkPipelineLayoutCreateInfo.pSetLayouts = &vkDescriptorSetLayout;
	vkPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	vkPipelineLayoutCreateInfo.pPushConstantRanges = NULL;


	vkResult = vkCreatePipelineLayout(vkdevice, &vkPipelineLayoutCreateInfo, NULL, &vkPipelineLayout);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createPipelineLayout :: vkCreatePipelineLayout()  failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createPipelineLayout :: vkCreatePipelineLayout() succeeded  \n", INFO_LOG);
	}


	return vkResult;
}

VkResult createDescriptorPool(void)
{
	// variables declaration
	VkResult vkResult = VK_SUCCESS;

	// code
	//1. before creating actual descriptor pool , Vulkan expects descriptor pool size
	VkDescriptorPoolSize vkDescriptorPoolSize_array[2]; // 1 for MVP uniform buffer , 1 for texture image & sampler
	memset((void*)vkDescriptorPoolSize_array, 0, sizeof(VkDescriptorPoolSize) * _ARRAYSIZE(vkDescriptorPoolSize_array));
	vkDescriptorPoolSize_array[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	vkDescriptorPoolSize_array[0].descriptorCount = 1;
	vkDescriptorPoolSize_array[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	vkDescriptorPoolSize_array[1].descriptorCount = 1;

	//2. create the pool
	VkDescriptorPoolCreateInfo vkDescriptorPoolCreateInfo;
	memset((void*)&vkDescriptorPoolCreateInfo, 0, sizeof(VkDescriptorPoolCreateInfo));
	vkDescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	vkDescriptorPoolCreateInfo.pNext = NULL;
	vkDescriptorPoolCreateInfo.flags = 0;
	vkDescriptorPoolCreateInfo.poolSizeCount = _ARRAYSIZE(vkDescriptorPoolSize_array); // size of how many pool structs
	vkDescriptorPoolCreateInfo.pPoolSizes = vkDescriptorPoolSize_array;
	vkDescriptorPoolCreateInfo.maxSets = 2;

	vkResult = vkCreateDescriptorPool(vkdevice, &vkDescriptorPoolCreateInfo, NULL, &vkDescriptorPool);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createDescriptorPool :: vkCreateDescriptorPool()  failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createDescriptorPool :: vkCreateDescriptorPool() succeeded  \n", INFO_LOG);
	}
	return vkResult;
}

VkResult createDescriptorSet(void)
{
	// variables declaration
	VkResult vkResult = VK_SUCCESS;

	// code
	// 1. initialize descriptorSet allocation info
	VkDescriptorSetAllocateInfo vkDescriptorSetAllocateInfo;
	memset((void*)&vkDescriptorSetAllocateInfo, 0, sizeof(VkDescriptorSetAllocateInfo));

	vkDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	vkDescriptorSetAllocateInfo.pNext = NULL;
	vkDescriptorSetAllocateInfo.descriptorPool = vkDescriptorPool;
	vkDescriptorSetAllocateInfo.descriptorSetCount = 1; // we have 1 set contatining 2 descriptors
	vkDescriptorSetAllocateInfo.pSetLayouts = &vkDescriptorSetLayout;

	vkResult = vkAllocateDescriptorSets(vkdevice, &vkDescriptorSetAllocateInfo, &vkDescriptorSet);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createDescriptorSet :: vkAllocateDescriptorSets()  failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createDescriptorSet :: vkAllocateDescriptorSets() succeeded  \n", INFO_LOG);
	}

	// 2. describe if we want buffer as uniform or image as uniform
	// for MVP uniform buffer
	VkDescriptorBufferInfo vkDescriptorBufferInfo;
	memset((void*)&vkDescriptorBufferInfo, 0, sizeof(VkDescriptorBufferInfo));
	vkDescriptorBufferInfo.buffer = uniformData.vkBuffer;
	vkDescriptorBufferInfo.offset = 0;
	vkDescriptorBufferInfo.range = sizeof(MyUniformData);

	// for texture image & sampler
	VkDescriptorImageInfo vkDescriptorImageInfo;
	memset((void*)&vkDescriptorImageInfo, 0, sizeof(VkDescriptorImageInfo));
	vkDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	vkDescriptorImageInfo.imageView = vkImageView_texture;
	vkDescriptorImageInfo.sampler = vkSampler_texture;


	// Two ways to update a.) Writing to shader directly or b.) coping from one shader to another
	// 3. update above descriptor set directly to the shader

	VkWriteDescriptorSet vkWriteDescriptorSet_array[2];
	memset((void*)vkWriteDescriptorSet_array, 0, sizeof(VkWriteDescriptorSet) * _ARRAYSIZE(vkWriteDescriptorSet_array));
	vkWriteDescriptorSet_array[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	vkWriteDescriptorSet_array[0].pNext = NULL;
	vkWriteDescriptorSet_array[0].dstSet = vkDescriptorSet;
	vkWriteDescriptorSet_array[0].dstBinding = 0; // must match shader layout(binding = 0)
	vkWriteDescriptorSet_array[0].dstArrayElement = 0;
	vkWriteDescriptorSet_array[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	vkWriteDescriptorSet_array[0].descriptorCount = 1;
	vkWriteDescriptorSet_array[0].pBufferInfo = &vkDescriptorBufferInfo;
	vkWriteDescriptorSet_array[0].pImageInfo = NULL;
	vkWriteDescriptorSet_array[0].pTexelBufferView = NULL;

	vkWriteDescriptorSet_array[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	vkWriteDescriptorSet_array[1].pNext = NULL;
	vkWriteDescriptorSet_array[1].dstSet = vkDescriptorSet;
	vkWriteDescriptorSet_array[1].dstBinding = 1; // must match shader layout(binding = 1)
	vkWriteDescriptorSet_array[1].dstArrayElement = 0;
	vkWriteDescriptorSet_array[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	vkWriteDescriptorSet_array[1].descriptorCount = 1;
	vkWriteDescriptorSet_array[1].pImageInfo = &vkDescriptorImageInfo;
	vkWriteDescriptorSet_array[1].pBufferInfo = NULL;
	vkWriteDescriptorSet_array[1].pTexelBufferView = NULL;

	vkUpdateDescriptorSets(vkdevice, _ARRAYSIZE(vkWriteDescriptorSet_array), vkWriteDescriptorSet_array, 0, NULL);
	//0, NULL for copying from one shader to another

	return vkResult;
}


VkResult createRenderPass(void)
{
	// variables declaration
	VkResult vkResult = VK_SUCCESS;


	// declare and initialize VkAttachmentDescription Structure
	VkAttachmentDescription vkAttachmentDescription_array[2];
	memset((void*)vkAttachmentDescription_array, 0, sizeof(VkAttachmentDescription) * _ARRAYSIZE(vkAttachmentDescription_array));


	vkAttachmentDescription_array[0].flags = 0; // can be set to VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT , it can share memory with another attachments
	vkAttachmentDescription_array[0].format = vkFormat_color;
	vkAttachmentDescription_array[0].samples = VK_SAMPLE_COUNT_1_BIT; // specifies an image with one sample per pixel.
	vkAttachmentDescription_array[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // specifies that the contents within the render area will be cleared to a uniform value
	vkAttachmentDescription_array[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE; // specifies the contents generated during the render pass and within the render area are written to memory
	vkAttachmentDescription_array[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // specifies that the previous contents within the area need not be preserved; the contents of the attachment will be undefined inside the render area.
	vkAttachmentDescription_array[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // specifies the contents within the render area are not needed after rendering, and may be discarded
	vkAttachmentDescription_array[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; //defines whay should be done with data arrangement .  specifies that the layout is unknown.
	vkAttachmentDescription_array[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // only be used for presenting a presentable image for display.


	vkAttachmentDescription_array[1].flags = 0;
	vkAttachmentDescription_array[1].format = vkFormat_Depth;
	vkAttachmentDescription_array[1].samples = VK_SAMPLE_COUNT_1_BIT;
	vkAttachmentDescription_array[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	vkAttachmentDescription_array[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	vkAttachmentDescription_array[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	vkAttachmentDescription_array[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	vkAttachmentDescription_array[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	vkAttachmentDescription_array[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


	// declare and initialize VKAttachmentReference for Color
	VkAttachmentReference vkAttachmentReference_color;
	memset((void*)&vkAttachmentReference_color, 0, sizeof(VkAttachmentReference));
	vkAttachmentReference_color.attachment = 0; // speicfies the index of vkAttachmentDescription_array 0th attachment
	vkAttachmentReference_color.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // only be used as a color or resolve attachment in a VkFramebuffer.


	// declare and initialize VKAttachmentReference for Depth
	VkAttachmentReference vkAttachmentReference_depth;
	memset((void*)&vkAttachmentReference_depth, 0, sizeof(VkAttachmentReference));
	vkAttachmentReference_depth.attachment = 1; // speicfies the index of vkAttachmentDescription_array 0th attachment
	vkAttachmentReference_depth.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; // only be used as a color or resolve attachment in a VkFramebuffer.




	// declare and initialize VkSubpassDescription
	VkSubpassDescription vkSubpassDescription;
	memset((void*)&vkSubpassDescription, 0, sizeof(VkSubpassDescription));
	vkSubpassDescription.flags = 0;
	vkSubpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // graphics pipeline {color , depth stencil , input , resolved , preserved attaachments}
	vkSubpassDescription.inputAttachmentCount = 0;
	vkSubpassDescription.pInputAttachments = NULL;
	vkSubpassDescription.colorAttachmentCount = 1;// should be the count of vkAttachmentReference for color
	vkSubpassDescription.pColorAttachments = &vkAttachmentReference_color;
	vkSubpassDescription.pResolveAttachments = NULL;
	vkSubpassDescription.pDepthStencilAttachment = &vkAttachmentReference_depth;
	vkSubpassDescription.preserveAttachmentCount = 0;
	vkSubpassDescription.pPreserveAttachments = NULL;


	// declare and initialize VkRenderPassCreateInfo
	VkRenderPassCreateInfo vkRenderPassCreateInfo;
	memset((void*)&vkRenderPassCreateInfo, 0, sizeof(VkRenderPassCreateInfo));
	vkRenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	vkRenderPassCreateInfo.pNext = NULL;
	vkRenderPassCreateInfo.flags = 0;
	vkRenderPassCreateInfo.attachmentCount = _ARRAYSIZE(vkAttachmentDescription_array);
	vkRenderPassCreateInfo.pAttachments = vkAttachmentDescription_array;
	vkRenderPassCreateInfo.subpassCount = 1;
	vkRenderPassCreateInfo.pSubpasses = &vkSubpassDescription;
	vkRenderPassCreateInfo.dependencyCount = 0;
	vkRenderPassCreateInfo.pDependencies = NULL;


	// create renderpass
	vkResult = vkCreateRenderPass(vkdevice, &vkRenderPassCreateInfo, NULL, &vkRenderPass);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createRenderPass :: vkCreateRenderPass()  failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createRenderPass :: vkCreateRenderPass() succeeded  \n", INFO_LOG);
	}
	return vkResult;
}




VkResult createPipeline(void)
{
	// variables declaration
	VkResult vkResult = VK_SUCCESS;


	//vertex input state
	VkVertexInputBindingDescription vkVertexInputBindingDescription_array[3];
	memset((void*)vkVertexInputBindingDescription_array, 0, sizeof(VkVertexInputBindingDescription) * _ARRAYSIZE(vkVertexInputBindingDescription_array));


	//position
	vkVertexInputBindingDescription_array[0].binding = 0; //equivalent to calling glGenBuffer , and setting the index of glGenBuffer
	//OpenGL would this for us , in Vulkan we are doing it manually
	vkVertexInputBindingDescription_array[0].stride = sizeof(float) * 3; // jump by 3 floating values
	vkVertexInputBindingDescription_array[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX; //equivalent of GL_ARRAY_BUFFER


	//normal
	vkVertexInputBindingDescription_array[1].binding = 1;
	vkVertexInputBindingDescription_array[1].stride = sizeof(float) * 3;
	vkVertexInputBindingDescription_array[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	//texcord
	vkVertexInputBindingDescription_array[2].binding = 2;
	vkVertexInputBindingDescription_array[2].stride = sizeof(float) * 2;
	vkVertexInputBindingDescription_array[2].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;


	// position
	VkVertexInputAttributeDescription vkVertexInputAttributeDescription_array[3];
	memset((void*)vkVertexInputAttributeDescription_array, 0, sizeof(VkVertexInputAttributeDescription) * _ARRAYSIZE(vkVertexInputAttributeDescription_array));
	vkVertexInputAttributeDescription_array[0].binding = 0; //Matches the binding index used in vkVertexInputBindingDescription_array
	//for interleaved , inputBindingdescription and inputAttributeDescription binding values will change
	vkVertexInputAttributeDescription_array[0].location = 0; //Matches the input location in the vertex shader:
	vkVertexInputAttributeDescription_array[0].format = VK_FORMAT_R32G32B32_SFLOAT; //SFLOAT(signed float vec3 can be negative) this format represents three 32-bit floating-point numbers (maps to vec3)
	vkVertexInputAttributeDescription_array[0].offset = 0; //"In every vertex, start reading this attribute after skipping offset bytes from the start of the vertex


	vkVertexInputAttributeDescription_array[1].binding = 1;
	vkVertexInputAttributeDescription_array[1].location = 1;
	vkVertexInputAttributeDescription_array[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	vkVertexInputAttributeDescription_array[1].offset = 0;

	vkVertexInputAttributeDescription_array[2].binding = 2;
	vkVertexInputAttributeDescription_array[2].location = 2;
	vkVertexInputAttributeDescription_array[2].format = VK_FORMAT_R32G32_SFLOAT;
	vkVertexInputAttributeDescription_array[2].offset = 0;


	VkPipelineVertexInputStateCreateInfo vkPipelineVertexInputStateCreateInfo;
	memset((void*)&vkPipelineVertexInputStateCreateInfo, 0, sizeof(VkPipelineVertexInputStateCreateInfo));
	vkPipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vkPipelineVertexInputStateCreateInfo.pNext = NULL;
	vkPipelineVertexInputStateCreateInfo.flags = 0;
	vkPipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = _ARRAYSIZE(vkVertexInputBindingDescription_array);
	vkPipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = vkVertexInputBindingDescription_array;
	vkPipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = _ARRAYSIZE(vkVertexInputAttributeDescription_array);
	vkPipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vkVertexInputAttributeDescription_array;


	//input assembly state
	VkPipelineInputAssemblyStateCreateInfo vkPipelineInputAssemblyStateCreateInfo;
	memset((void*)&vkPipelineInputAssemblyStateCreateInfo, 0, sizeof(VkPipelineInputAssemblyStateCreateInfo));
	vkPipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	vkPipelineInputAssemblyStateCreateInfo.pNext = NULL;
	vkPipelineInputAssemblyStateCreateInfo.flags = 0;
	vkPipelineInputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	vkPipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;   // disable primitive restart
	/*
	uint16_t indices[] = {
	0, 1, 2, 3,
	0xFFFF,         // Vulkan sees this, restarts here
	4, 5, 6, 7
	};
	*/


	// rasterizer state
	VkPipelineRasterizationStateCreateInfo vkPipelineRasterizationStateCreateInfo;
	memset((void*)&vkPipelineRasterizationStateCreateInfo, 0, sizeof(VkPipelineRasterizationStateCreateInfo));
	vkPipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	vkPipelineRasterizationStateCreateInfo.pNext = NULL;
	vkPipelineRasterizationStateCreateInfo.flags = 0;
	vkPipelineRasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	vkPipelineRasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE; // do VK_CULL_MODE_BACK_BIT | VK_CULL_MODE_FRONT_BIT (VK_CULL_MODE_FRONT_AND_BACK) when using points and lines to skip culling processing
	vkPipelineRasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	vkPipelineRasterizationStateCreateInfo.lineWidth = 1.0f;


	// color blend state
	VkPipelineColorBlendAttachmentState vkPipelineColorBlendAttachmentState_array[1];
	memset((void*)vkPipelineColorBlendAttachmentState_array, 0, sizeof(VkPipelineColorBlendAttachmentState) * _ARRAYSIZE(vkPipelineColorBlendAttachmentState_array));
	vkPipelineColorBlendAttachmentState_array[0].blendEnable = VK_FALSE;
	vkPipelineColorBlendAttachmentState_array[0].colorWriteMask = 0xf; // this tells the GPU which components (R, G, B, A) of a pixel's color can be written to the framebuffer.


	/*
	The value 0xf (binary 1111) means:
	VK_COLOR_COMPONENT_R_BIT = 1 (0001)
	VK_COLOR_COMPONENT_G_BIT = 2 (0010)
	VK_COLOR_COMPONENT_B_BIT = 4 (0100)
	VK_COLOR_COMPONENT_A_BIT = 8 (1000)
	*/
	//OpenGL equivalent : glDisable(GL_BLEND);   glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);


	VkPipelineColorBlendStateCreateInfo vkPipelineColorBlendStateCreateInfo;
	memset((void*)&vkPipelineColorBlendStateCreateInfo, 0, sizeof(VkPipelineColorBlendStateCreateInfo));
	vkPipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	vkPipelineColorBlendStateCreateInfo.pNext = NULL;
	vkPipelineColorBlendStateCreateInfo.flags = 0;
	vkPipelineColorBlendStateCreateInfo.attachmentCount = _ARRAYSIZE(vkPipelineColorBlendAttachmentState_array);
	vkPipelineColorBlendStateCreateInfo.pAttachments = vkPipelineColorBlendAttachmentState_array;
	/*
		we can have multiple blend attachments , for multiple attachments or outs in Fragment shader
	*/




	// viewport scissor state
	VkPipelineViewportStateCreateInfo vkPipelineViewportStateCreateInfo;
	memset((void*)&vkPipelineViewportStateCreateInfo, 0, sizeof(VkPipelineViewportStateCreateInfo));
	vkPipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	vkPipelineViewportStateCreateInfo.pNext = NULL;
	vkPipelineViewportStateCreateInfo.flags = 0;
	vkPipelineViewportStateCreateInfo.viewportCount = 1; // we can specificy multiple viewport


	//memset of global variable
	memset((void*)&vkViewport, 0, sizeof(VkViewport));
	vkViewport.x = 0;
	vkViewport.y = 0;
	vkViewport.width = (float)vkExtent2D_swapchain.width;
	vkViewport.height = (float)vkExtent2D_swapchain.height;
	vkViewport.minDepth = 0.0f;
	vkViewport.maxDepth = 1.0f;


	// in opengl glDepthRange(0.0, 1.0);
	/*
		in  directx
		d3d11ViewPort.MinDepth = 0.0f;
		d3d11ViewPort.MaxDepth = 1.0f;
	*/


	//Scissor Test is a feature that restricts rendering to a rectangular region of the framebuffer.
	vkPipelineViewportStateCreateInfo.pViewports = &vkViewport;
	vkPipelineViewportStateCreateInfo.scissorCount = 1;


	//memset of global variable
	memset((void*)&vkRect2D_scissor, 0, sizeof(VkRect2D));
	vkRect2D_scissor.offset.x = 0;
	vkRect2D_scissor.offset.y = 0;
	vkRect2D_scissor.extent.width = vkExtent2D_swapchain.width;
	vkRect2D_scissor.extent.height = vkExtent2D_swapchain.height;


	//glEnable(GL_SCISSOR_TEST);
	//glScissor(0, 0, width, height);


	vkPipelineViewportStateCreateInfo.pScissors = &vkRect2D_scissor;


	/*
		vkCreateGraphicsPipelines() : can create multiple pipelines .
		count of viewport and scissor count members of this structure must be same
	*/


	// depth stencil state
	// depth stencil state
	VkPipelineDepthStencilStateCreateInfo  vkPipelineDepthStencilStateCreateInfo;
	memset((void*)&vkPipelineDepthStencilStateCreateInfo, 0, sizeof(VkPipelineDepthStencilStateCreateInfo));


	vkPipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	vkPipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
	vkPipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
	vkPipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	vkPipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
	vkPipelineDepthStencilStateCreateInfo.back.failOp = VK_STENCIL_OP_KEEP;
	vkPipelineDepthStencilStateCreateInfo.back.passOp = VK_STENCIL_OP_KEEP;
	vkPipelineDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
	vkPipelineDepthStencilStateCreateInfo.front = vkPipelineDepthStencilStateCreateInfo.back;
	vkPipelineDepthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;




	// dynamic state - states that can be changed dynamically
	// dynamic state - viewport , scissor , depth bias , blend constants , stencil mask , line width etc (made in command buffer)
	//24 spheres can be done using dynamic viewport or multi viewport




	// dynamic state
	// we dont have any dynamic state


	// multisample state
	VkPipelineMultisampleStateCreateInfo vkPipelineMultisampleStateCreateInfo;
	memset((void*)&vkPipelineMultisampleStateCreateInfo, 0, sizeof(VkPipelineMultisampleStateCreateInfo));
	vkPipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	vkPipelineMultisampleStateCreateInfo.pNext = NULL;
	vkPipelineMultisampleStateCreateInfo.flags = 0;
	vkPipelineMultisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; // if not specified gives validation error




	// shader state 
	VkPipelineShaderStageCreateInfo vkPipelineShaderStageCreateInfo_array[2];
	memset((void*)vkPipelineShaderStageCreateInfo_array, 0, sizeof(VkPipelineShaderStageCreateInfo) * _ARRAYSIZE(vkPipelineShaderStageCreateInfo_array));
	//vertex shader
	vkPipelineShaderStageCreateInfo_array[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vkPipelineShaderStageCreateInfo_array[0].pNext = NULL; //compulsory since it has extensions
	vkPipelineShaderStageCreateInfo_array[0].flags = 0;
	vkPipelineShaderStageCreateInfo_array[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	vkPipelineShaderStageCreateInfo_array[0].module = vkShaderModule_vertex_shader;
	vkPipelineShaderStageCreateInfo_array[0].pName = "main";
	vkPipelineShaderStageCreateInfo_array[0].pSpecializationInfo = NULL; //pre-compiled values
	//fragment shader
	vkPipelineShaderStageCreateInfo_array[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vkPipelineShaderStageCreateInfo_array[1].pNext = NULL; //compulsory since it has extensions
	vkPipelineShaderStageCreateInfo_array[1].flags = 0;
	vkPipelineShaderStageCreateInfo_array[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	vkPipelineShaderStageCreateInfo_array[1].module = vkShaderModule_fragment_shader;
	vkPipelineShaderStageCreateInfo_array[1].pName = "main";
	vkPipelineShaderStageCreateInfo_array[1].pSpecializationInfo = NULL; //pre-compiled values


	// tessellation State
	// not required


	// pipline cache object
	VkPipelineCacheCreateInfo vkPipelineCacheCreateInfo;
	memset((void*)&vkPipelineCacheCreateInfo, 0, sizeof(VkPipelineCacheCreateInfo));
	vkPipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	vkPipelineCacheCreateInfo.pNext = NULL;
	vkPipelineCacheCreateInfo.flags = 0;


	VkPipelineCache vkPipelineCache = VK_NULL_HANDLE;
	vkResult = vkCreatePipelineCache(vkdevice, &vkPipelineCacheCreateInfo, NULL, &vkPipelineCache);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createPipeline :: vkCreatePipelineCache()  failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createPipeline :: vkCreatePipelineCache()  succeeded  \n", INFO_LOG);
	}


	// create actual graphics pipeline
	VkGraphicsPipelineCreateInfo  vkGraphicsPipelineCreateInfo;
	memset((void*)&vkGraphicsPipelineCreateInfo, 0, sizeof(VkGraphicsPipelineCreateInfo));
	vkGraphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	vkGraphicsPipelineCreateInfo.pNext = NULL;
	vkGraphicsPipelineCreateInfo.flags = 0;
	vkGraphicsPipelineCreateInfo.pVertexInputState = &vkPipelineVertexInputStateCreateInfo;
	vkGraphicsPipelineCreateInfo.pInputAssemblyState = &vkPipelineInputAssemblyStateCreateInfo;
	vkGraphicsPipelineCreateInfo.pRasterizationState = &vkPipelineRasterizationStateCreateInfo;
	vkGraphicsPipelineCreateInfo.pColorBlendState = &vkPipelineColorBlendStateCreateInfo;
	vkGraphicsPipelineCreateInfo.pViewportState = &vkPipelineViewportStateCreateInfo;
	vkGraphicsPipelineCreateInfo.pDepthStencilState = &vkPipelineDepthStencilStateCreateInfo;
	vkGraphicsPipelineCreateInfo.pDynamicState = NULL;
	vkGraphicsPipelineCreateInfo.pMultisampleState = &vkPipelineMultisampleStateCreateInfo;
	vkGraphicsPipelineCreateInfo.stageCount = _ARRAYSIZE(vkPipelineShaderStageCreateInfo_array);
	vkGraphicsPipelineCreateInfo.pStages = vkPipelineShaderStageCreateInfo_array;
	vkGraphicsPipelineCreateInfo.pTessellationState = NULL;
	vkGraphicsPipelineCreateInfo.layout = vkPipelineLayout;
	vkGraphicsPipelineCreateInfo.renderPass = vkRenderPass;
	vkGraphicsPipelineCreateInfo.subpass = 0;
	vkGraphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; // required if you want to create Pipeline from other template Pipeline
	vkGraphicsPipelineCreateInfo.basePipelineIndex = 0;


	vkResult = vkCreateGraphicsPipelines(vkdevice, vkPipelineCache, 1, &vkGraphicsPipelineCreateInfo, NULL, &vkPipeline);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createPipeline :: vkCreateGraphicsPipelines()  failed \n", ERROR_LOG);
		if (vkPipelineCache)
		{
			vkDestroyPipelineCache(vkdevice, vkPipelineCache, NULL);
			fprintf(gpFile, "%s createPipeline :: vkDestroyPipelineCache()  succeeded  \n", INFO_LOG);
			vkPipelineCache = VK_NULL_HANDLE;
		}
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createPipeline :: vkCreateGraphicsPipelines()  succeeded  \n", INFO_LOG);
	}


	// destroy pipeline cache
	if (vkPipelineCache)
	{
		vkDestroyPipelineCache(vkdevice, vkPipelineCache, NULL);
		fprintf(gpFile, "%s createPipeline :: vkDestroyPipelineCache()  succeeded  \n", INFO_LOG);
		vkPipelineCache = VK_NULL_HANDLE;
	}


	return vkResult;
}
VkResult createFramebuffers(void)
{
	// variables declaration
	VkResult vkResult = VK_SUCCESS;


	vkFramebuffers_array = (VkFramebuffer*)malloc(sizeof(VkFramebuffer) * swapchainImageCount);
	if (vkFramebuffers_array == NULL)
	{
		fprintf(gpFile, "%s createFramebuffers :: vkFramebuffers_array memory allocation failed \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}


	for (uint32_t i = 0; i < swapchainImageCount; i++)
	{
		VkImageView vkImageView_attachments_array[2];
		memset((void*)vkImageView_attachments_array, 0, sizeof(VkImageView) * _ARRAYSIZE(vkImageView_attachments_array));


		// framebuffers needs to have VkImageView


		VkFramebufferCreateInfo vkFramebufferCreateInfo;
		memset((void*)&vkFramebufferCreateInfo, 0, sizeof(VkFramebufferCreateInfo));


		vkFramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		vkFramebufferCreateInfo.pNext = NULL;
		vkFramebufferCreateInfo.flags = 0;
		vkFramebufferCreateInfo.renderPass = vkRenderPass;
		vkFramebufferCreateInfo.attachmentCount = _ARRAYSIZE(vkImageView_attachments_array);
		vkFramebufferCreateInfo.pAttachments = vkImageView_attachments_array;
		vkFramebufferCreateInfo.width = vkExtent2D_swapchain.width;
		vkFramebufferCreateInfo.height = vkExtent2D_swapchain.height;
		vkFramebufferCreateInfo.layers = 1;


		// set the attachments
		vkImageView_attachments_array[0] = swapchainImageView_array[i];
		vkImageView_attachments_array[1] = VkImageView_Depth;
		vkResult = vkCreateFramebuffer(vkdevice, &vkFramebufferCreateInfo, NULL, &vkFramebuffers_array[i]);
		if (vkResult != VK_SUCCESS)
		{
			fprintf(gpFile, "%s createFramebuffers :: vkCreateFramebuffer()  failed \n", ERROR_LOG);
			return vkResult;
		}
		else
		{
			fprintf(gpFile, "%s createFramebuffers :: vkCreateFramebuffer() succeeded  \n", INFO_LOG);
		}
	}


	return vkResult;
}


VkResult createSemaphores(void)
{
	// variables declaration
	VkResult vkResult = VK_SUCCESS;
	VkSemaphoreCreateInfo vkSemaphoreCreateInfo;
	memset((void*)&vkSemaphoreCreateInfo, 0, sizeof(VkSemaphoreCreateInfo));
	vkSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	vkSemaphoreCreateInfo.pNext = NULL;
	vkSemaphoreCreateInfo.flags = 0; // msut be 0 , it's reserved


	vkResult = vkCreateSemaphore(vkdevice, &vkSemaphoreCreateInfo, NULL, &vkSemaphore_backbuffer);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createSemaphores :: vkCreateSemaphore() for vkSemaphore_backbuffer failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createSemaphores :: vkCreateSemaphore() for vkSemaphore_backbuffer succeeded  \n", INFO_LOG);
	}


	vkResult = vkCreateSemaphore(vkdevice, &vkSemaphoreCreateInfo, NULL, &vkSemaphore_rendercomplete);
	if (vkResult != VK_SUCCESS)
	{
		fprintf(gpFile, "%s createSemaphores :: vkCreateSemaphore() for vkSemaphore_rendercomplete failed \n", ERROR_LOG);
		return vkResult;
	}
	else
	{
		fprintf(gpFile, "%s createSemaphores :: vkCreateSemaphore() for vkSemaphore_rendercomplete succeeded  \n", INFO_LOG);
	}


	return vkResult;
}


VkResult createFences(void)
{
	// variables declaration
	VkResult vkResult = VK_SUCCESS;


	VkFenceCreateInfo vkFenceCreateInfo;
	memset((void*)&vkFenceCreateInfo, 0, sizeof(VkFenceCreateInfo));


	vkFenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	vkFenceCreateInfo.pNext = NULL;
	vkFenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;


	vkFence_array = (VkFence*)malloc(sizeof(VkFence) * swapchainImageCount);
	if (vkFence_array == NULL)
	{
		fprintf(gpFile, "%s createFences :: vkFence_array memory allocation failed \n", ERROR_LOG);
		vkResult = VK_ERROR_INITIALIZATION_FAILED;
		return vkResult;
	}


	for (uint32_t i = 0; i < swapchainImageCount; i++)
	{
		vkResult = vkCreateFence(vkdevice, &vkFenceCreateInfo, NULL, &vkFence_array[i]);
		if (vkResult != VK_SUCCESS)
		{
			fprintf(gpFile, "%s createFences :: vkCreateFence()  failed at %d\n", ERROR_LOG, i);
			return vkResult;
		}
		else
		{
			fprintf(gpFile, "%s createFences :: vkCreateFence() succeeded at %d\n", INFO_LOG, i);
		}
	}




	return vkResult;
}


VkResult buildCommandBuffers(void)
{
	// variables declaration
	VkResult vkResult = VK_SUCCESS;


	// loop per swapchain image
	for (uint32_t i = 0; i < swapchainImageCount; i++)
	{
		//reset commandbuffers
		vkResult = vkResetCommandBuffer(vkCommandBuffer_array[i], 0); // 0 means - dont release resources of commandpool for this commandbuffer
		if (vkResult != VK_SUCCESS)
		{
			fprintf(gpFile, "%s buildCommandBuffers :: vkResetCommandBuffers()  failed at %d\n", ERROR_LOG, i);
			return vkResult;
		}
		else
		{
			fprintf(gpFile, "%s buildCommandBuffers :: vkResetCommandBuffers() succeeded at %d\n", INFO_LOG, i);
		}


		VkCommandBufferBeginInfo vkCommandBufferBeginInfo;
		memset((void*)&vkCommandBufferBeginInfo, 0, sizeof(VkCommandBufferBeginInfo));
		vkCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		vkCommandBufferBeginInfo.pNext = NULL;
		vkCommandBufferBeginInfo.flags = 0; // will use only PRIMARY command buffer and won't be use between multiple threads


		vkResult = vkBeginCommandBuffer(vkCommandBuffer_array[i], &vkCommandBufferBeginInfo);
		if (vkResult != VK_SUCCESS)
		{
			fprintf(gpFile, "%s buildCommandBuffers :: vkBeginCommandBuffer()  failed at %d\n", ERROR_LOG, i);
			return vkResult;
		}
		else
		{
			fprintf(gpFile, "%s buildCommandBuffers :: vkBeginCommandBuffer() succeeded at %d\n", INFO_LOG, i);
		}


		VkClearValue vkclearValue_array[2];
		memset((void*)vkclearValue_array, 0, sizeof(VkClearValue) * _ARRAYSIZE(vkclearValue_array));


		vkclearValue_array[0].color = vkClearColorValue;
		vkclearValue_array[1].depthStencil = vkClearDepthStencilValue;




		VkRenderPassBeginInfo vkRenderPassBeginInfo;
		memset((void*)&vkRenderPassBeginInfo, 0, sizeof(VkRenderPassBeginInfo));


		vkRenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		vkRenderPassBeginInfo.pNext = NULL;
		vkRenderPassBeginInfo.renderPass = vkRenderPass;
		vkRenderPassBeginInfo.renderArea.offset.x = 0;
		vkRenderPassBeginInfo.renderArea.offset.y = 0;
		vkRenderPassBeginInfo.renderArea.extent.width = vkExtent2D_swapchain.width;
		vkRenderPassBeginInfo.renderArea.extent.height = vkExtent2D_swapchain.height;
		vkRenderPassBeginInfo.clearValueCount = _ARRAYSIZE(vkclearValue_array);
		vkRenderPassBeginInfo.pClearValues = vkclearValue_array;
		vkRenderPassBeginInfo.framebuffer = vkFramebuffers_array[i];


		// begin the renderpass
		vkCmdBeginRenderPass(vkCommandBuffer_array[i], &vkRenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE); // this is part of subpass and primary command buffer


		//  bind with the pipeline
		vkCmdBindPipeline(vkCommandBuffer_array[i], VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);


		// bind with vertex position buffer
		VkDeviceSize vkdeviceSize_offset_position[1];
		memset((void*)vkdeviceSize_offset_position, 0, sizeof(VkDeviceSize) * _ARRAYSIZE(vkdeviceSize_offset_position));
		vkCmdBindVertexBuffers(vkCommandBuffer_array[i], 0, 1, &vertexData_position.vkBuffer, vkdeviceSize_offset_position);


		// bind with vertex normal buffer
		VkDeviceSize vkdeviceSize_offset_normal[1];
		memset((void*)vkdeviceSize_offset_normal, 0, sizeof(VkDeviceSize) * _ARRAYSIZE(vkdeviceSize_offset_normal));
		vkCmdBindVertexBuffers(vkCommandBuffer_array[i], 1, 1, &vertexData_normal.vkBuffer, vkdeviceSize_offset_normal);

		// bind with vertex texcord buffer
		VkDeviceSize vkdeviceSize_offset_texcord[1];
		memset((void*)vkdeviceSize_offset_texcord, 0, sizeof(VkDeviceSize) * _ARRAYSIZE(vkdeviceSize_offset_texcord));
		vkCmdBindVertexBuffers(vkCommandBuffer_array[i], 2, 1, &vertexData_texcord.vkBuffer, vkdeviceSize_offset_texcord);

		vkCmdBindIndexBuffer(vkCommandBuffer_array[i], vertexData_index.vkBuffer, 0, VK_INDEX_TYPE_UINT32); //UINT16 because its unsigned short


		vkCmdBindDescriptorSets(
			vkCommandBuffer_array[i],
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			vkPipelineLayout,
			0,                       // 0th index of descriptor set
			1,                       // descriptorSetCount
			&vkDescriptorSet,
			0,                       // for dynamic shader stage
			NULL                  // for dynamic shader stage
		);




		// 0 - VVIP - first binding in shader
		// 
		// here call drawing functions
		vkCmdDrawIndexed(vkCommandBuffer_array[i], numElements, 1, 0, 0, 1);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		// end render pass
		vkCmdEndRenderPass(vkCommandBuffer_array[i]);


		// End the command buffer
		vkResult = vkEndCommandBuffer(vkCommandBuffer_array[i]);
		if (vkResult != VK_SUCCESS)
		{
			fprintf(gpFile, "%s buildCommandBuffers :: vkEndCommandBuffer() failed at %d\n", ERROR_LOG, i);
			return vkResult;
		}
		else
		{
			fprintf(gpFile, "%s buildCommandBuffers :: vkEndCommandBuffer() succeeded at %d\n", INFO_LOG, i);
		}


	}
	return vkResult;
}


VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallback(
	VkDebugReportFlagsEXT vkDebugReportFlagsEXT,
	VkDebugReportObjectTypeEXT vkDebugReportObjectTypeEXT,
	uint64_t object,
	size_t location,
	int32_t messageCode,
	const char* pLayerPrefix,
	const char* pMessage,
	void* pUserData)
{
	//code
	fprintf(gpFile, "VMN_VALIDATION :: debugReportCallback %s (%d) = %s \n",
		pLayerPrefix, messageCode, pMessage);
	return VK_FALSE;
}


void addTriangle(float single_vertex[3][3], float single_normal[3][3], float single_texCoord[3][2])
{
	// function declarations
	BOOL closeEnough(const float, const float, const float);
	void normalizeVector(float[3]);

	// code
	unsigned int maxElements = numFaceIndices * 3;
	const float e = 0.00001f; // How small a difference to equate

	// First thing we do is make sure the normals are unit length!
	// It's almost always a good idea to work with pre-normalized normals
	normalizeVector(single_normal[0]);
	normalizeVector(single_normal[1]);
	normalizeVector(single_normal[2]);

	// Search for match - triangle consists of three verts
	for (unsigned int i = 0; i < 3; i++)
	{
		unsigned int j = 0;
		for (j = 0; j < numVerts; j++)
		{
			// If the vertex positions are the same
			if (closeEnough(pPositions[j * 3], single_vertex[i][0], e) &&
				closeEnough(pPositions[(j * 3) + 1], single_vertex[i][1], e) &&
				closeEnough(pPositions[(j * 3) + 2], single_vertex[i][2], e) &&

				// AND the Normal is the same...
				closeEnough(pNormals[j * 3], single_normal[i][0], e) &&
				closeEnough(pNormals[(j * 3) + 1], single_normal[i][1], e) &&
				closeEnough(pNormals[(j * 3) + 2], single_normal[i][2], e) &&

				// And Texture is the same...
				closeEnough(pTexCoords[j * 2], single_texCoord[i][0], e) &&
				closeEnough(pTexCoords[(j * 2) + 1], single_texCoord[i][1], e))
			{
				// Then add the index only
				pElements[numElements] = j;
				numElements++;
				break;
			}
		}

		// No match for this vertex, add to end of list
		if (j == numVerts && numVerts < maxElements && numElements < maxElements)
		{
			pPositions[numVerts * 3] = single_vertex[i][0];
			pPositions[(numVerts * 3) + 1] = single_vertex[i][1];
			pPositions[(numVerts * 3) + 2] = single_vertex[i][2];

			pNormals[numVerts * 3] = single_normal[i][0];
			pNormals[(numVerts * 3) + 1] = single_normal[i][1];
			pNormals[(numVerts * 3) + 2] = single_normal[i][2];

			pTexCoords[numVerts * 2] = single_texCoord[i][0];
			pTexCoords[(numVerts * 2) + 1] = single_texCoord[i][1];

			pElements[numElements] = numVerts;
			numElements++;
			numVerts++;
		}
	}
}

void normalizeVector(float u[3])
{
	// function declarations
	void scaleVector(float[3], const float);
	float getVectorLength(const float[3]);

	// code
	scaleVector(u, 1.0f / getVectorLength(u));
}

void scaleVector(float v[3], const float scale)
{
	// code
	v[0] *= scale;
	v[1] *= scale;
	v[2] *= scale;
}

float getVectorLength(const float u[3])
{
	// function declarations
	float getVectorLengthSquared(const float[3]);

	// code
	return(sqrtf(getVectorLengthSquared(u)));
}

float getVectorLengthSquared(const float u[3])
{
	// code
	return((u[0] * u[0]) + (u[1] * u[1]) + (u[2] * u[2]));
}

BOOL closeEnough(const float fCandidate, const float fCompare, const float fEpsilon)
{
	// code
	return((fabs(fCandidate - fCompare) < fEpsilon));
}
