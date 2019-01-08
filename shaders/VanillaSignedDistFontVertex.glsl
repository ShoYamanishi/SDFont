#version 330 core

in vec3 vertexLCS;
in vec3 normalLCS;
in vec2 texCoordIn;

uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform vec3 lightWCS;

out vec3 vertexWCS;
out vec2 texCoordOut;
out vec3 normalECS;
out vec3 vertexToEyeECS;
out vec3 vertexToLightECS;


void main() {

    mat4 MV  = V * M;

    mat4 MVP = P * MV;   

    gl_Position      = ( MVP * vec4( vertexLCS, 1.0 ) );

    texCoordOut      = texCoordIn;

    vertexWCS        = ( M   * vec4( vertexLCS, 1.0 ) ).xyz;

    vec3 vertexECS   = ( MV  * vec4( vertexLCS, 1.0 ) ).xyz;

    normalECS        = ( MV  * vec4( normalLCS, 0.0 ) ).xyz;

    vec3 lightECS    = ( V   * vec4( lightWCS,  1.0 ) ).xyz;

    vertexToEyeECS   = vec3(0,0,0) - vertexECS;

    vertexToLightECS = lightECS - vertexECS;

}
