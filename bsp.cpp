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

	std::cout << "Reading surfedge lump..."; // possible culprit of later errors
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

	/*for (int x = 0; x < nodes.size(); x++)
	{
		//std::cout << "Node " << x << " child 1: " << nodes[0].children[0] << "    child 2: " << nodes[0].children[1] << std::endl;

		if (nodes[x].children[0] < 0 || nodes[x].children[1] < 0)
			std::cout << "Node " << x << " child 1: " << nodes[x].children[0] << "    child 2: " << nodes[x].children[1] << std::endl;
	}*/

	Vertex vert1; vert1.x = 0.0f; vert1.y = 0.5f; vert1.z = 0.0f;
	testvert.push_back (vert1);
	Vertex vert2; vert2.x = 0.5f; vert2.y = -0.5f; vert2.z = 0.0f;
	testvert.push_back (vert2);
	Vertex vert3; vert3.x = -0.5f; vert3.y = -0.5f; vert3.z = 0.0f;
	testvert.push_back (vert3);
}

void bsp::LoadMapDetails(glm::vec3 pos)
{
	/*
	// lazy, probably doesn't work correctly either- remove later
	// loop through every node for a negative (leaf) number, render that leaf
	for (int x = 0; x < nodes.size(); x++)
	{
		if (nodes[x].children[0] < 0)
			RenderLeaf (std::abs(nodes[x].children[0]) - 1);
		if (nodes[x].children[1] < 0)
			RenderLeaf (std::abs(nodes[x].children[1]) - 1);
	}
	*/
	/*bool finished = false;
	int curNode = 0;
	while (!finished)
	{
		std::cout << curNode;
		if (nodes[curNode].children[0] < 0)
			RenderLeaf (std::abs(nodes[curNode].children[0]) - 1);
		if (nodes[curNode].children[1] < 0)
			RenderLeaf (std::abs(nodes[curNode].children[1]) - 1);
		

		if (nodes[curNode].children[0] > 0)
			curNode = nodes[curNode].children[0];
		else if (nodes[curNode].children[1] > 0)
			curNode = nodes[curNode].children[1];

		std::cout << curNode << "aaa";

		if (nodes[curNode].children[0] < 0 && nodes[curNode].children[1] < 0)
			finished = true;

		std::cout << nodes[curNode].children[0] << std::endl;
	}*/

	int curLeaf = WalkBSPTree (pos, 0);
	std::cout << "Current leaf: " << curLeaf;

	ComputeBSP (0, curLeaf, pos);
}

void bsp::ComputeBSP (int node, int leaf, glm::vec3 pos)
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

	// 8 and 9, 20 and 21, 12, and 13, 24 and 25, 38 and 39, 
	for (int x = 0; x < faces[face].numEdges; x++) // loop through every single edge in a face
	{
		int EdgeIndex = SurfEdges[faces[face].firstEdge + x]; // Edge index

		/*if (EdgeIndex > 0) // from first to second vertex
		{
			Vertex tempVert;
			tempVert.x = vertices[edges[EdgeIndex].vert[0]].x;
			tempVert.y = vertices[edges[EdgeIndex].vert[0]].y;
			tempVert.z = vertices[edges[EdgeIndex].vert[0]].z;

			mapVerts.push_back (tempVert);
			glVertex3f (vertices[edges[EdgeIndex].vert[0]].x, vertices[edges[EdgeIndex].vert[0]].y, vertices[edges[EdgeIndex].vert[0]].z); 

		}

		else if (EdgeIndex < 0) // from second to first vertex
		{
			EdgeIndex *= -1;

			Vertex tempVert;
			tempVert.x = vertices[edges[EdgeIndex].vert[1]].x;
			tempVert.y = vertices[edges[EdgeIndex].vert[1]].y;
			tempVert.z = vertices[edges[EdgeIndex].vert[1]].z;

			mapVerts.push_back (tempVert);
			glVertex3f (vertices[edges[EdgeIndex].vert[1]].x, vertices[edges[EdgeIndex].vert[1]].y, vertices[edges[EdgeIndex].vert[1]].z); 
		}*/
			EdgeIndex = std::abs (EdgeIndex);

		Vertex tempVert;
			tempVert.x = vertices[edges[EdgeIndex].vert[0]].x;
			tempVert.y = vertices[edges[EdgeIndex].vert[0]].y;
			tempVert.z = vertices[edges[EdgeIndex].vert[0]].z;

			mapVerts.push_back (tempVert);

			tempVert.x = vertices[edges[EdgeIndex].vert[1]].x;
			tempVert.y = vertices[edges[EdgeIndex].vert[1]].y;
			tempVert.z = vertices[edges[EdgeIndex].vert[1]].z;

			mapVerts.push_back (tempVert);

		Vertex tempNorm = planes[faces[face].planeIndex].normal; // create normals
		mapNormals.push_back (tempNorm);
		mapNormals.push_back (tempNorm);
	}
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
// a lot of the code right here is from ^, make own code later
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