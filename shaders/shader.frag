#version 460 core
#extension GL_ARB_separate_shader_objects : enable




layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec3 out_diffuse_color;
layout(location = 1) in vec2 out_texcord;
layout(binding = 1) uniform sampler2D utextureSampler;

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


/*
EXAMPLE for color blend state




   layout(location = 0) out vec4 outColor0;
   layout(location = 1) out vec4 outColor1;
*/








/*
EXAMPLE for pSpecializationInfo
layout(constant_id = 0) const float red   = 1.0;
layout(constant_id = 1) const float green = 1.0;
*/




void main(void)
{
   //code
   FragColor =  vec4(out_diffuse_color, 1.0);
   if(u_MyUniformData.tKeyIsPressed.x == 1)
   {
		FragColor = FragColor * texture(utextureSampler,out_texcord);
   }
}
