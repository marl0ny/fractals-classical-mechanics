#if __VERSION__ <= 120
attribute vec4 position;
varying vec2 UV;
#else
in vec4 position;
out vec2 UV;
#endif

#if (__VERSION__ >= 330) || (defined(GL_ES) && __VERSION__ >= 300)
#define texture2D texture
#else
#define texture texture2D
#endif

#if (__VERSION__ > 120) || defined(GL_ES)
precision highp float;
#endif

uniform sampler2D coordTex;
uniform float length1;
uniform float length2;
uniform float viewScale;
uniform vec2 viewOffset;

void main() {
    UV = position.xy;
    vec4 coord = texture2D(coordTex, UV);
    float pi1 = coord[0], pi2 = coord[1];
    float phi1 = coord[2], phi2 = coord[3];
    vec2 r0 = length1*vec2(sin(phi1), -cos(phi1));
    vec2 r1 = length2*vec2(sin(phi2), -cos(phi2));
    vec2 r;
    if (position[3] < 1.0)
        r = viewOffset;
    else if (position[3] < 2.0)
        r = viewOffset + viewScale*r0;
    else if (position[3] < 3.0)
        r = viewOffset + viewScale*r0;
    else if (position[3] < 4.0)
        r = viewOffset + viewScale*(r0 + r1);
    gl_Position = vec4(r, 0.0, 1.0);

}
