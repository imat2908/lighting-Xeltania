#version 430

in vec3 vertPos;
in vec3 N;
in vec3 lightPos;
/*TODO:: Complete your shader code for a full Phong shading*/ 

struct matParams
{
	vec3 Kd;            // Diffuse reflectivity
	vec3 Ka;			// Ambient Relectivity
	vec3 Ks;			// Specular Reflectivity
	vec3 Ld;            // Diffuse light intensity
	vec3 La;			// Ambient light intensity
	vec3 Ls;			// Specular intensity
};

struct attenuationParam // Struct containing the parameters used by the Attenuation.
{
	float cosA;
	float att;
	vec3 attRadiance;
};

struct spotlightParams // Struct containing the parameters used by the Spotlight.
{
    vec3 pos;
    vec3 direction;
    float cutoff;

    vec3 Kd, Ka, Ks;
    vec3 La, Ld, Ls;
};

struct camParams // Struct for the camera parameters.
{
	vec3 View;
};

// ADS Structs
uniform matParams ADS;
uniform attenuationParam Att;
uniform camParams Cam;

// Spotlight Structs
uniform spotlightParams Spotlight;

// complete to a full phong shading
layout( location = 0 ) out vec4 FragColour;

        // Methods to calculate ADS //
vec4 diffuse(vec3 light) 
{
    //calculate Diffuse Light Intensity making sure it is not negative and is clamped 0 to 1  
    vec4 Id = vec4(ADS.Kd, 1.0) * vec4(ADS.Ld, 1.0) * max(dot(N, light), 0.0);// padded because fragcolour is vec4
    Id = clamp(Id, 0.0, 1.0); // Avoid overshooting

    return Id;
}

vec4 ambient()
{
    // Calculating Ambient Light Intensity
    vec4 Ia = vec4(ADS.La, 1.0) * vec4(ADS.Ka, 1.0);
    Ia = clamp(Ia, 0.0, 1.0);

    return Ia;
}

vec4 specular(vec3 light)
{
    // Calculating Specularity Intensity
    vec3 normalizedDist = normalize(Cam.View - vertPos);
    vec3 reflectedNormal = (reflect(-light, N));
    float cosphi = dot(normalizedDist, reflectedNormal);

    vec4 Is = (vec4(ADS.Ls, 1.0) * vec4(ADS.Ks, 1.0)) * pow(cosphi, 5);
    Is = clamp(Is, 0.0, 1.0);

    return Is;
}

float attenuation(float k, float l, float q)  // Method to calculate attenuation taking in parameters for the constant, linear, and quadratic coefficients.
{
    // Calculating Attenuation
    float d = distance(lightPos, vertPos);
    float att = 1 / (k + (l * d) + (q * pow(d, 2)));

    return att;
}

float spotlight() 
{
    // Spotlight calculations
    vec3 normalizedPos = normalize(Spotlight.pos - vertPos);
    vec3 normalizedSpot = normalize(Spotlight.direction);
    float angleBetween = dot(normalizedPos, normalizedSpot);

    return angleBetween;
}
// Attempted Toon shader code
vec4 toonShader(vec3 light, vec4 amb, vec4 diffspec) 
{
	const int frequency = 10;
	const float sf = 1 / frequency;
	float cosine = max( dot(light, N), 0);
	vec4 diffuse = vec4(((ADS.Kd * floor(cosine * frequency)) * sf),1);
	
	vec4 toonshader = (amb + diffspec + diffuse);
	toonshader.w = 1;
	return toonshader;
}
void main()
{
    //Calculate the light vector
    vec3 L = normalize(lightPos - vertPos);  // Getting the normal from the lightsource to the vertices of the objects?

    // Access the ADS values from the methods : 
    vec4 A = ambient();
    vec4 D = diffuse(L);
    vec4 S = specular(L);
    // Access attenuation and spotlight : 
    float Att = attenuation(1.f, 0.0f, 0.0f);
    float spotlight = spotlight();

    //Multiply the Reflectivity by the Diffuse intensity (do full shading if in the spotlight, otherwise just ambient)
    //if (spotlight < Spotlight.cutoff) 
    //{
    //    FragColour = (A + Att * D + Att * S);
    //}
    //else 
    //{
    //FragColour = A;
    //}
	FragColour = toonShader(L, A, (S));

}
