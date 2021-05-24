#version 330

struct LightInfo {
    vec3 dir;
    vec3 La;
    vec3 Ld;
    vec3 Ls;
    float a0;
    float a1;
    float a2;
};

struct MaterialInfo
{
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float s;
};


in vec3 normalCamSpace; //нормаль в системе координат камеры (интерполирована между вершинами треугольника)
in vec4 posCamSpace; //координаты вершины в системе координат камеры (интерполированы между вершинами треугольника)
in vec2 TexCoord; //текстурные координаты (интерполирована между вершинами треугольника)

out vec4 fragColor; //выходной цвет фрагмента

uniform sampler2D ourTexture;
uniform LightInfo light;
uniform MaterialInfo material;
uniform vec3 cameraSpaceLightDir;

void main()
{
	vec4 base_colour = texture(ourTexture, TexCoord);
	if (base_colour.a < 0.01) {
		discard;
	}
	float alpha = base_colour.a;
	vec3 diffuseColor = base_colour.rgb;
	vec3 lightDirCamSpace = normalize(cameraSpaceLightDir);  //направление на источник света, пришло из uniform
	vec3 normal = normalize(normalCamSpace.xyz); //нормализуем нормаль после интерполяции, пришло из вершинного шейдера
	vec3 viewDirection = normalize(-posCamSpace.xyz); //направление на виртуальную камеру (она находится в точке (0.0, 0.0, 0.0)), пришло из вершинного шейдера
	float NdotL = max(dot(normal, lightDirCamSpace), 0.0); //скалярное произведение
	vec3 color = diffuseColor * (light.La + light.Ld * NdotL);
	if (NdotL > 0.0) {	
		vec3 halfVector = normalize(lightDirCamSpace + viewDirection); //биссектриса между направлениями на камеру и на источник света
		float blinnTerm = max(dot(normal, halfVector), 0.0); //интенсивность бликового освещения по Блинну				
		blinnTerm = pow(blinnTerm, material.s); //регулируем размер блика
		//color += light.Ls * material.Ks * blinnTerm;
	}
	fragColor = vec4(color, alpha);
}

