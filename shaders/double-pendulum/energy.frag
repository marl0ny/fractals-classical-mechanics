/* Compute the energy of each of the double pendulums. */
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

uniform sampler2D coordTex;
uniform float mass1;
uniform float mass2;
uniform float length1;
uniform float length2;
uniform float gravity;

/* Time derivative of the angular positions phi1 and phi2. */
float dotPhi(int i, vec4 coord) {
    float pi1 = coord[0], pi2 = coord[1];
    float phi1 = coord[2], phi2 = coord[3];
    float m11 = (mass1 + mass2)*length1;
    float m12 = mass2*length1*length2*cos(phi1 - phi2);
    float m21 = mass2*length1*length2*cos(phi1 - phi2);
    float m22 = mass2*length2;
    if (i == 1)
        return -m22*pi1/(m12*m21 - m22*m11) + m12*pi2/(m12*m21 - m22*m11);
    else if (i == 2)
        return m21*pi1/(m12*m21 - m22*m11) - m11*pi2/(m12*m21 - m22*m11);
}

float dotPhi1(vec4 coord) {
    return dotPhi(1, coord);
}

float dotPhi2(vec4 coord) {
    return dotPhi(2, coord);
}

float lagrangian(vec4 coord) {
    float pi1 = coord[0], pi2 = coord[1];
    float phi1 = coord[2], phi2 = coord[3];
    return (
        0.5*(mass1 + mass2)*pow(length1*dotPhi1(coord), 2.0)
        + 0.5*mass2*pow(length2*dotPhi2(coord), 2.0)
        + mass2*length1*length2
            *dotPhi1(coord)*dotPhi2(coord)*cos(phi1 - phi2)
        + (mass1 + mass2)*gravity*length1*cos(phi1)
        + mass2*gravity*length2*cos(phi2));
}

float hamiltonian(vec4 coord) {
    float pi1 = coord[0], pi2 = coord[1];
    float phi1 = coord[2], phi2 = coord[3];
    return dotPhi1(coord)*pi1 + dotPhi2(coord)*pi2 - lagrangian(coord);
}

void main() {
    vec4 coord = texture2D(coordTex, UV);
    float energy = hamiltonian(coord);
    fragColor = vec4(energy/10.0, 0.0, -energy/10.0, 1.0);
}
