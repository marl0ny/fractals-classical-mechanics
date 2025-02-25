/* RK4 formula from Runge-Kutta methods Wikipedia page.
The derivatives of the coordinates are not computed here but in
previous computations, where here they are taken as input.

Reference:
    Wikipedia - Runge–Kutta methods
    https://en.wikipedia.org/wiki/Runge%E2%80%93Kutta_methods
*/
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

uniform sampler2D qTex;
uniform sampler2D qDotTex1;
uniform sampler2D qDotTex2;
uniform sampler2D qDotTex3;
uniform sampler2D qDotTex4;
uniform float dt;

void main() {
    vec4 q0 = texture2D(qTex, UV);
    vec4 qDot1 = texture2D(qDotTex1, UV); 
    vec4 qDot2 = texture2D(qDotTex2, UV); 
    vec4 qDot3 = texture2D(qDotTex3, UV); 
    vec4 qDot4 = texture2D(qDotTex4, UV); 
    fragColor = q0 + dt*(qDot1 + 2.0*qDot2 + 2.0*qDot3 + qDot4)/6.0;
}