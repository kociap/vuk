#version 460
#pragma shader_stage(vertex)
#extension GL_AMD_shader_explicit_vertex_parameter : require

layout(location = 0) in vec3 ipos;
layout(location = 1) in vec2 iuv;


layout(binding = 0) uniform VP {
	mat4 view;
	mat4 projection;
};

layout(binding = 1) buffer readonly Model {
	mat4 model[];
};

out gl_PerVertex 
{
    vec4 gl_Position;
};

layout (location = 0) out flat uint oIDFlat;
layout (location = 1) out uint oID;
layout (location = 2) precise out vec2 oUV;
layout (location = 3) out flat uint oMeshID;

void main() {
	oIDFlat = oID = gl_VertexIndex;
	oUV = iuv;
	oMeshID = gl_BaseInstance;
    gl_Position = projection * view * model[gl_BaseInstance] * vec4(ipos, 1.0);
}