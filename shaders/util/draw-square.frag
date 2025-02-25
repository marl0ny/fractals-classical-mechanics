#if (__VERSION__ >= 330) || (defined(GL_ES) && __VERSION__ >= 300)
#define texture2D texture
#else
#define texture texture2D
#endif

#if (__VERSION__ > 120) || defined(GL_ES)
precision highp float;
#endif
    
#if __VERSION__ <= 120
varying vec2 UV;
#define fragColor gl_FragColor
#else
in vec2 UV;
out vec4 fragColor;
#endif

uniform vec2 bottomLeftPosition;
uniform vec2 dimensions;
uniform vec4 color;

void main() {
    fragColor = vec4(0.0);
    if (UV.x >= bottomLeftPosition.x
        && UV.y >= bottomLeftPosition.y
        && UV.x < (bottomLeftPosition.x + dimensions.x)
        && UV.y < (bottomLeftPosition.y + dimensions.y))
        fragColor = color;
}

