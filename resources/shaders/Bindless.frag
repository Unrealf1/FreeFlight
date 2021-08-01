#version 430

#extension GL_ARB_bindless_texture : enable

in vec3 colour;
in vec2 TexCoord;

out vec4 fragColor;

flat in sampler2D ourTexture;
flat in sampler2D secondaryTexture;
in float secondary_texture_weight;

struct LightInfo
{
	vec3 dir; //положение источника света в системе координат ВИРТУАЛЬНОЙ КАМЕРЫ!
	vec3 La; //цвет и интенсивность окружающего света
	vec3 Ld; //цвет и интенсивность диффузного света
	vec3 Ls; //цвет и интенсивность бликового света
};
uniform LightInfo light;

const vec3 Ks = vec3(1.0, 1.0, 1.0); //Коэффициент бликового отражения
const float shininess = 1208.0;

in vec3 normalCamSpace; //нормаль в системе координат камеры (интерполирована между вершинами треугольника)
in vec4 posCamSpace; //координаты вершины в системе координат камеры (интерполированы между вершинами треугольника)
in vec2 texCoord; //текстурные координаты (интерполирована между вершинами треугольника)

in vec3 debug;

void main() {
    vec4 texColor = texture(ourTexture, TexCoord);
	vec4 others = texture(secondaryTexture, TexCoord);

	texColor = mix(texColor, others, secondary_texture_weight);

	vec3 diffuseColor = texColor.rgb;

    vec3 normal = normalize(normalCamSpace); //нормализуем нормаль после интерполяции

	vec3 viewDirection = normalize(-posCamSpace.xyz); //направление на виртуальную камеру (она находится в точке (0.0, 0.0, 0.0))
	
	vec3 lightDirCamSpace = normalize(light.dir);

	float NdotL = max(dot(normal, lightDirCamSpace.xyz), 0.0); //скалярное произведение (косинус)

    vec3 color = diffuseColor * (light.La + light.Ld * NdotL);
	//color = normalCamSpace;
    if (NdotL > 0.0) {			
		vec3 halfVector = normalize(lightDirCamSpace.xyz + viewDirection); //биссектриса между направлениями на камеру и на источник света

		float blinnTerm = max(dot(normal, halfVector), 0.0); //интенсивность бликового освещения по Блинну				
		blinnTerm = pow(blinnTerm, shininess); //регулируем размер блика
		color += (light.Ls * Ks * blinnTerm);
		//color = normalCamSpace;
	}
	fragColor = vec4(color, 1.0);
	//fragColor = vec4(vec3(debug / 20.0), 1.0);
}