# FreeFlight

### TODO:

* Bindless textures ✓
* Texture information in biome & chunk points ✓
* Lightning [in progress...]
  * shadows
  * materials
* Smooth heights and textures [in progress...]
* Structures
  * changing terrain heights & textures
  * adding new vertices
  * separate models
* Multithreading
  * Thread pool
  * move new chunk creation and possibly some other operations there
* code overhaul
* Physics


### BUGS:

* Textures are sometimes corrupt and missing when border of the chunk is intersecting border of the biome. Problem with missing textures might be because of height smoothing and corrupted textures are probably appearing after incorrect mix() in the shader