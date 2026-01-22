#version 460 core
#extension GL_ARB_separate_shader_objects : enable




// 1. Keeps shaders as separate objects. GLSL shaders are treated more like Vulkan shaders.
// 2. Avoids unnecessary copying of data across shader stages.
// 3. Allows for separate resource management and segregation between stages.
// 4. Better debugging




layout(location = 0) in vec4 vPosition; //layout location is compulsory
layout(location = 1) in vec4 vNormal; //layout location is compulsory
layout(location = 2) in vec2 vTexcord; //layout location is compulsory
layout(location = 0) out vec3 out_diffuse_color; //layout location is compulsory
layout(location = 1) out vec2 out_texcord; //layout location is compulsory



layout(binding = 0) uniform MyUniformData
{
   // matrices related uniform
   mat4 modelMatrix;
   mat4 viewMatrix;
   mat4 projectionMatrix;


   // light related uniform
   vec4 lightDiffuse;
   vec4 lightPosition;
   vec4 materialDiffuse;


   //key pressed related uniform
   uvec4 lKeyIsPressed;
   uvec4 tKeyIsPressed;


}u_MyUniformData;




//Standard Portable Intermediate Representation




void main(void)
{
   //code
   if(u_MyUniformData.lKeyIsPressed.x == 1)
   {
       vec4 eyeCoordinates = u_MyUniformData.viewMatrix * u_MyUniformData.modelMatrix * vPosition;
       mat3 normalMatrix = mat3(transpose(inverse(u_MyUniformData.viewMatrix * u_MyUniformData.modelMatrix)));
       vec3 transformedNormal = normalize(normalMatrix * vNormal.xyz);
       vec3 lightSource = normalize(vec3(u_MyUniformData.lightPosition - eyeCoordinates));
       out_diffuse_color = vec3(u_MyUniformData.lightDiffuse) * vec3(u_MyUniformData.materialDiffuse) *
        max(dot(lightSource, transformedNormal), 0.0);
   }
   else
   {
       out_diffuse_color = vec3(1.0, 1.0, 1.0);
   }
   gl_Position = u_MyUniformData.projectionMatrix * u_MyUniformData.viewMatrix * u_MyUniformData.modelMatrix * vPosition;
   	out_texcord = vTexcord;
}



















