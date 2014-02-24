#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "include/GL/glew.h"
#include "include/glm/glm.hpp"

#define HEADER_LUMPS 64
#define VBSP_HEADER (('P'<<24) + ('S' << 16) + ('B' << 8) + 'V')

class bsp
{
public:
	bsp (const char* fp);
	void LoadMapDetails(glm::vec3 pos);
	
	enum lumpTypes {LUMP_ENTITIES = 0, LUMP_PLANES, LUMP_TEXDATA, LUMP_VERTICES, LUMP_VISIBILITY, LUMP_NODES, LUMP_TEXINFO, LUMP_FACES, LUMP_LIGHTING, LUMP_OCCLUSION, LUMP_LEAFS, LUMP_FACEIDS, LUMP_EDGES, LUMP_SURFEDGES, LUMP_MODELS, LUMP_WORLDLIGHTS,
				LUMP_LEAFFACES, LUMP_LEAFBRUSHES, LUMP_BRUSHES, LUMP_BRUSHSIDES, LUMP_AREAS, LUMP_AREAPORTALS, LUMP_PORTALS, LUMP_UNUSED0, LUMP_PROPCOLLISION, LUMP_CLUSTERS, LUMP_UNUSED1, LUMP_PROPHULLS, LUMP_PORTALVERTS, LUMP_UNUSED2,
				LUMP_PROPTRIS, LUMP_DISPINFO, LUMP_ORIGINALFACES, LUMP_PHYSDISP, LUMP_PHYSCOLLIDE, LUMP_VERTNORMALS, LUMP_VERTNORMALINDICES, LUMP_DISP_LIGHTMAP_ALPHAS, LUMP_DISP_VERTS, LUMP_DISP_LIGHTMAP_SAMPLE_POSITIONS,
				LUMP_GAME_LUMP, LUMP_LEAFWATERDATA, LUMP_PRIMITIVES, LUMP_PRIMVERTS, LUMP_PRIMINDICES, LUMP_PAKFILE, LUMP_CLIPPORTALVERTS, LUMP_CUBEMAPS, LUMP_TEXDATA_STRING_DATA, LUMP_TEXDATA_STRING_TABLE, LUMP_OVERLAYS, LUMP_LEAFMINDISTTOWATER,
				LUMP_FACE_MACRO_TEXTURE_INFO, LUMP_DISP_TRIS, LUMP_PHYSCOLLIDESURFACE, LUMP_PROP_BLOB, LUMP_WATEROVERLAYS, LUMP_LIGHTMAPPAGES, LUMP_LEAF_AMBIENT_INDEX_HDR, LUMP_LIGHTMAPPAGEINFOS, LUMP_LEAF_AMBIENT_INDEX, LUMP_LIGHTING_HDR,
				LUMP_WORLDLIGHTS_HDR, LUMP_LEAF_AMBIENT_LIGHTING_HDR, LUMP_LEAF_AMBIENT_LIGHTING, LUMP_XZIPPAKFILE, LUMP_FACES_HDR, LUMP_MAP_FLAGS, LUMP_OVERLAY_FADES, LUMP_OVERLAY_SYSTEM_LEVELS, LUMP_PHYSLEVEL, LUMP_DISP_MULTIBLEND};


	// TODO: need lump_vertices, lump_edges, lump_surfedges, lump_planes, lump_leaffaces, lump_nodes, ~all lump disps
	// TODO: Major cleanup

	//
	//
	// ONLY HALF OF THE TRIANGLES ARE BEING RENDERED???
	//      -- Maybe has something to do with the way the BSP tree is being traversed?-

	//  go down bsp like regular
	//

	// Contains the info of every lump (offset from beginning of file, length of lump, version (usually 0), and identification (usually 0)
	struct lumpInfo
	{
		int offset;
		int length;
		int version;
		char ident[4];
	};

	// Main header struct
	struct Header
	{
		int magic;
		int version;
		lumpInfo lumps[HEADER_LUMPS];
		int revision;
	};

	// Vertex struct
	struct Vertex
	{
		float x, y, z;
	};
	std::vector <Vertex> vertices;
	std::vector <Vertex> testvert;

	struct Plane
	{
		Vertex normal;
		float distance;
		int type;
	};
	std::vector <Vertex> mapNormals;

	struct Node
	{
		int planeIndex;
		int children[2];
		short mins[3];
		short maxs[3];
		unsigned short faceIndex;
		unsigned short numFaces;
		short area;
		short padding;
	};
	std::vector <Node> nodes;

	struct Leaf
	{
		int contents;
		short cluster;
		short area:9; // use only 9 bits
		short flags:7;
		short mins[3];
		short maxs[3];
		unsigned short firstLeafFace;
		unsigned short numLeafFaces;
		unsigned short firstLeafBrush;
		unsigned short numLeafBrushes;
		short leafWaterDataID;

	};
	std::vector <Leaf> leafs;
	std::vector <unsigned short> leafFaces;

	struct Face
	{
		unsigned short planeIndex;
		unsigned __int8 side; // visual c++ stuffs, change it later
		unsigned __int8 onNode;
		int firstEdge;
		short numEdges;
		short texInfo;
		short dispInfo;
		short surfaceFogVolumeID;
		unsigned __int8 styles[4];
		int lightOfs;
		float area;
		int lightmapTextureMinsInLuxels[2];
		int lightmapTextureSizeInLuxels[2];
		int origFace;
		unsigned short numPrims;
		unsigned short firstPrimID;
		unsigned int smoothingGroups;
	};
	std::vector <Face> faces;
	

	struct Edge
	{
		unsigned short vert[2];
	};
	std::vector <Edge> edges;

	std::vector <int> SurfEdges;

	std::vector <Plane> planes;
		std::vector <Vertex> PlaneVerts;

	std::vector <Vertex> mapVerts; // Final map vertices, used for rendering

private:
	std::ifstream map;
	Header header;
	
	int WalkBSPTree (glm::vec3 pos, int node);
	bool PointIsInside (glm::vec3 pos, short min[3], short max[3]);
	void ComputeBSP (int node, int leaf, glm::vec3 pos);
	void RenderLeaf (int leaf);
	void RenderFace (int face);
	float dotProduct (Vertex normal, glm::vec3 b);

	bool loadHeader();
	template <typename T>
	void readLump (std::ifstream& fp, std::vector<T>& elements, unsigned int index);
	// ^^^ thank you layla for showing this ^^^
};
