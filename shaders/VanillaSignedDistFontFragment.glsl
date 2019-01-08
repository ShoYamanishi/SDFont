#version 330 core

precision mediump float;

in vec2 texCoordOut;
in vec3 vertexWCS;
in vec3 normalECS;
in vec3 vertexToEyeECS;
in vec3 vertexToLightECS;

out vec4 color;

uniform sampler2D fontTexture;
uniform int       effect;
uniform bool      useLight;
uniform float     lowThreshold;
uniform float     highThreshold;
uniform float     smoothing;
uniform vec3      baseColor;
uniform vec3      borderColor;

void main (void) {

    if ( effect == 0 ) {

        // Raw output with interpolation.

        color.rgb = baseColor;
        color.a   = texture( fontTexture, texCoordOut ).a;
    }
    else if ( effect == 1 ) {

        // Softened edge.

        color.rgb = baseColor;
        color.a   = smoothstep( 0.5 - smoothing,
                                0.5 + smoothing, 
                                texture( fontTexture, texCoordOut ).a );
    }
    else if ( effect == 2 ) {

        // Sharp edge.

        float alpha = texture(fontTexture, texCoordOut).a;

        if ( alpha >= lowThreshold ) {

            color.rgb = baseColor;
            color.a   = 1.0;
        }
        else {

            color.rgb = baseColor;
            color.a   = 0.0;
        }
    }
    else if ( effect == 3 ) {

        // Sharp edge with outer glow.

        float alpha = texture(fontTexture, texCoordOut).a;

        if ( alpha >= lowThreshold ) {

            color.rgb = baseColor;
            color.a   = 1.0;
        }
        else {

            color.rgb = borderColor;
            color.a   = alpha;
        }
    }        
    else if ( effect == 4 ) {

        // With border.

        float alpha = texture(fontTexture, texCoordOut).a;

        if ( alpha >= lowThreshold && alpha <= highThreshold ) {

            color.rgb = borderColor;
            color.a   = 1.0;
        }
        else if ( alpha >= highThreshold ) {

            color.rgb = baseColor;
            color.a   = 1.0;
        }
        else {

            color.rgb = baseColor;
            color.a = 0.0;
        }
    }
    else if ( effect == 5 ) {
        // Softened edge.
        float alpha = texture(fontTexture, texCoordOut).a;
        color.rgb = baseColor;

        if ( alpha < 0.5) {
            color.a = smoothstep( 0.5 - smoothing,
                                  0.5 + smoothing, 
                                  alpha            ); 
        }                               
        else {
            color.a = 0.5 -  smoothstep( 0.5 - smoothing,
                                         0.5 + smoothing, 
                                         0.75 * alpha     ); 
        }
    }
    else {

        // Rect box for debugging

        color.rgb = baseColor;
        color.a   = 1.0;

    }

    float dist = distance( vertexToLightECS, vec3( 0.0, 0.0, 0.0 ) );

    if ( useLight ) {

        color.rgb = color.rgb / sqrt(dist);
    }
}

