# FreeFlight

### TODO:

* Bindless textures ✓
* Texture information in biome & chunk points ✓
* Lightning [in progress...]
  * materials
  * shadows
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
* After creation of new biome center some old chunk informatiom may become outdated, causing visual bugs, defferrent vertices in one place on the edge of chunks. Simple solution is to regenerate every vertex, like in `ManualUpdate`, but this is very slow 

* Lighting is bugged on chunk edges ✓
  * fixed, now neighboors for edge vertices are calculated correctly in `updateBuffers`

