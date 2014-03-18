#include "bsp.h"

bsp::bsp (const char* fp)
{
	std::cout << "Opening file...";
	map.open (fp, std::ios::binary);
	std::cout << "done.\n";

	std::cout << "Reading header...";
	map.seekg(0);
	map.read ((char*)&header, sizeof (Header));
	std::cout << "done.\n";

	if (header.magic == VBSP_HEADER)
		std::cout << "File matches VBSP.\n";

	if (header.version == 0x17)
		std::cout << "File matches Dota 2 BSP version.\n";
	else
		std::cout << "BSP is not Dota 2 BSP.\n";

	std::cout << "Verifying lumps...\n";
	for (int x = 0; x < 64; x++)
	{
		if (header.lumps[x].length == 0)
			std::cout << "Lump " << x << " is not included in this file.\n";
	}

	std::cout << "Reading node lump...";
	readLump (map, nodes, LUMP_NODES);
	std::cout << "done.\n";

	std::cout << "Reading leaf lump...";
	readLump (map, leafs, LUMP_LEAFS);
	std::cout << "done.\n";

	std::cout << "Reading face lump...";
	readLump (map, faces, LUMP_FACES);
	std::cout << "done.\n";

	std::cout << "Reading edge lump...";
	readLump (map, edges, LUMP_EDGES);
	std::cout << "done.\n";

	std::cout << "Reading surfedge lump...";
	readLump (map, SurfEdges, LUMP_SURFEDGES);
	std::cout << "done.\n";

	std::cout << "Creating leafface lump...";\
	readLump (map, leafFaces, LUMP_LEAFFACES);
	std::cout << "done.\n";

	std::cout << "Reading vertex lump...";
	readLump (map, vertices, LUMP_VERTICES);
	std::cout << "done.\n";

	std::cout << "Reading plane lump...";
	readLump (map, planes, LUMP_PLANES);
	std::cout << "done.\n";

	map.close();

	mapNormals.resize (vertices.size(), Vertex(0,0,0));
}

void bsp::LoadMapDetails(glm::vec3 pos)
{
	int curLeaf = WalkBSPTree (pos, 0);
	std::cout << "Current leaf: " << curLeaf;

	ComputeBSP (0, curLeaf, pos);
}

void bsp::ComputeBSP (int node, int leaf, glm::vec3 pos) // my mind i dont even
{
	if (node < 0)
	{
		if (node == -1)
			return;

		RenderLeaf (~node);
		return;
	}

	float location;

	if (planes[nodes[node].planeIndex].type == 0) // plane x
		location = pos.x - planes[nodes[node].planeIndex].distance;
	else if (planes[nodes[node].planeIndex].type == 1) // plane y
		location = pos.y - planes[nodes[node].planeIndex].distance;
	else if (planes[nodes[node].planeIndex].type == 2) // plane z
		location = pos.z - planes[nodes[node].planeIndex].distance;
	else
		location = dotProduct (planes[nodes[node].planeIndex].normal, pos) - planes[nodes[node].planeIndex].distance;

	if (location > 0)
	{
		ComputeBSP (nodes[node].children[1], leaf, pos);
		ComputeBSP (nodes[node].children[0], leaf, pos);
	}

	else
	{
		ComputeBSP (nodes[node].children[0], leaf, pos);
		ComputeBSP (nodes[node].children[1], leaf, pos);
	}

}

float bsp::dotProduct (Vertex a, glm::vec3 b)
{
	return  (a.x * b.x +
			 a.y * b.y +
			 a.z * b.z);
}

void bsp::RenderLeaf (int leaf) // loadleaf()
{
	for (int x = 0; x < leafs[leaf].numLeafFaces; x++)
		RenderFace (leafFaces[leafs[leaf].firstLeafFace + x]);
}

void bsp::RenderFace (int face) // loadface()
{
	int hub, firstPoint, newPoint; // key indices for each face
	// 8 and 9, 20 and 21, 12, and 13, 24 and 25, 38 and 39, 
	for (int x = 0; x < faces[face].numEdges; x++) // loop through every single edge in a face, this will end up making a triangle fan
	{
		int EdgeIndex = SurfEdges[faces[face].firstEdge + x]; // Edge index
		EdgeIndex = std::abs (EdgeIndex); // don't want to be accessing using negatives

		if (x == 0)
			hub = edges[EdgeIndex].vert[0]; // if this is the first run through the first vertex is the "hub" index that all of the triangles in the plane will refer to

		firstPoint = edges[EdgeIndex].vert[0]; // the second point after the hub
		newPoint = edges[EdgeIndex].vert[1]; // last point to create a full triangle

		/*still trying to fix normals*/
		mapNormals[hub] = planes[faces[face].planeIndex].normal;
		mapNormals[firstPoint] = planes[faces[face].planeIndex].normal;
		mapNormals[newPoint] = planes[faces[face].planeIndex].normal;

		// push back every single index 
		indices.push_back (hub);
		indices.push_back (firstPoint);
		indices.push_back (newPoint);
	}

	indices.push_back (999988887777); // once we are done rendering our plane we put in our primitive restart index to start a new plane
}

template <typename T> void bsp::readLump (std::ifstream& map, std::vector<T>& elements, unsigned int index)
{
	elements.clear();
	
	if (header.lumps[index].length == 0) // lump doesn't exist
		return;

	map.seekg (header.lumps[index].offset);

	for (int x = 0; x < header.lumps[index].length / sizeof(T); x++)
	{
		T element;
		map.read ((char*)&element, sizeof(T));
		elements.push_back (element);
	}
}

//http://sourceforge.net/p/hlbsp/code/HEAD/tree/hlbsp.cpp#l897
// a lot of the code right here is from ^, make own code once I get everything solid
int bsp::WalkBSPTree (glm::vec3 pos, int node)
{
	for (int x = 0; x < 2; x++) 
	{
		if (nodes[node].children[x] >= 0) // node
		{
			if (PointIsInside (pos, nodes[nodes[node].children[x]].mins, nodes[nodes[node].children[x]].maxs))
				return WalkBSPTree (pos, nodes[node].children[x]);
		}

		else if (~nodes[node].children[x] != 0) // NOT x = -x - 1, so this an index for the leaves
		{
			if (PointIsInside (pos, leafs[~(nodes[node].children[x])].mins, leafs[~(nodes[node].children[x])].maxs))
				return ~nodes[node].children[x];
		}
	}

	return -1;
}

bool bsp::PointIsInside (glm::vec3 pos, short min[3], short max[3])
{
	if ((min[0] <= pos.x && pos.x <= max[0] &&
        min[1] <= pos.y && pos.y <= max[1] &&
        min[2] <= pos.z && pos.z <= max[2]) ||
       (min[0] >= pos.x && pos.x >= max[0] &&
        min[1] >= pos.y && pos.y >= max[1] &&
        min[2] >= pos.z && pos.z >= max[2]))
		return true;
	else
		return false;
}