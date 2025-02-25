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

const float PI = 3.141592653589793;
uniform float minPhi1;
uniform float maxPhi1;
uniform float minPhi2;
uniform float maxPhi2;

void main() {
    // Initial conjugate momenta
    float pi1 = 0.0;
    float pi2 = 0.0;
    // Initial angular separations
    float phi1 = minPhi1 + UV.x*(maxPhi1 - minPhi1);
    float phi2 = minPhi2 + UV.y*(maxPhi2 - minPhi2);
    fragColor = vec4(pi1, pi2, phi1, phi2);
}