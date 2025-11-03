#include <iostream>
#include "Model.h"
#include <fstream>
#include <filesystem>
#include "BinaryParser.h"

void SerializeNodeData(std::string& text,NodeData* node) {
	BinaryReader br;
	//Serialize string
	text += br.EncodeBinary(node->name.size());
	for (char ch : node->name) {
		text += br.EncodeBinary(ch);
	}
	//Serialize transformation
	text += br.EncodeBinary(node->transformation);
	//Serialize children
	text += br.EncodeBinary(node->children.size());
	//Serialize children elements
	for (NodeData& nd : node->children) {
		SerializeNodeData(text, &nd);
	}
}

void ParseAnimation(std::string& serializedVertex,Animation& ani) {
	//SErialize duration
	BinaryReader br;
	serializedVertex += br.EncodeBinary(ani.GetDuration());
	//Serialize tics per second
	serializedVertex += br.EncodeBinary(ani.GetTicksPerSecond());
	//serialzie name
	serializedVertex += br.EncodeBinary(ani.m_Name.size());
	std::cout << "Animation NAME SIZE " << ani.m_Name.size() << std::endl;
	for (char ch : ani.m_Name) {
		serializedVertex += br.EncodeBinary(ch);
	}

	//serialize bone map;
	serializedVertex += br.EncodeBinary(ani.m_Bones.size());
	//std::cout << "BONE SIZE" << ani.m_Bones.size() << '\n';
	for (auto& pair : ani.m_Bones) {

		//Serialize key
		serializedVertex += br.EncodeBinary(pair.first.size());
		for (char ch : pair.first) {
			serializedVertex += br.EncodeBinary(ch);
		}
		//Setialize bone name
		serializedVertex += br.EncodeBinary(pair.second.GetName().size());
		for (char ch : pair.second.GetName()) {
			serializedVertex += br.EncodeBinary(ch);
		}
		//Serialize ID
		serializedVertex += br.EncodeBinary(pair.second.GetID());

		//Get positions
		serializedVertex += br.EncodeBinary(pair.second.m_Positions.size());
		for (glm::vec3 position : pair.second.m_Positions) {
			//Serialize derived m_positions
			serializedVertex += br.EncodeBinary(position.x);
			serializedVertex += br.EncodeBinary(position.y);
			serializedVertex += br.EncodeBinary(position.z);
		}
		//Get position time
		serializedVertex += br.EncodeBinary(pair.second.m_PosTimes.size());
		for (float posTime : pair.second.m_PosTimes) {
			//Serialize derived m_positions
			serializedVertex += br.EncodeBinary(posTime);
		}

		//Get Rotations
		serializedVertex += br.EncodeBinary(pair.second.m_Rotations.size());
		for (glm::quat rot : pair.second.m_Rotations) {
			//Serialize derived m_positions
			serializedVertex += br.EncodeBinary(rot.x);
			serializedVertex += br.EncodeBinary(rot.y);
			serializedVertex += br.EncodeBinary(rot.z);
			serializedVertex += br.EncodeBinary(rot.w);
		}
		//Get rotation time
		serializedVertex += br.EncodeBinary(pair.second.m_RotTimes.size());
		for (float rotTime : pair.second.m_RotTimes) {
			//Serialize derived m_positions
			serializedVertex += br.EncodeBinary(rotTime);
		}

		//Get Scales
		serializedVertex += br.EncodeBinary(pair.second.m_Scales.size());
		for (glm::vec3 rot : pair.second.m_Scales) {
			//Serialize derived m_positions
			serializedVertex += br.EncodeBinary(rot.x);
			serializedVertex += br.EncodeBinary(rot.y);
			serializedVertex += br.EncodeBinary(rot.z);
		}
		//Get scale time
		serializedVertex += br.EncodeBinary(pair.second.m_ScaleTimes.size());
		for (float scaleTime : pair.second.m_ScaleTimes) {
			//Serialize derived m_positions
			serializedVertex += br.EncodeBinary(scaleTime);
		}

	}

	//Serialize Node data
	SerializeNodeData(serializedVertex, &ani.m_RootNode);

}
int main(int argc, char* argv[])
{	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//Create window to be displayed
	GLFWwindow* window = glfwCreateWindow(1.f, 1.f, "GAM300 Graphics test", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create window";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	//Load GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Could not load GLAD";
		return -1;
	}
	std::string modelPath{ argv[1] };
	std::string metaPath{ argv[2] };
	std::string outputPath{ argv[3] };
	BinaryReader br;


	//float test = 1.2f;
	//std::string result=br.EncodeBinary(test);
	//std::cout << result << '\n';
	//Load model
	Model ourModel(modelPath.c_str());
	//Read meta path to determine how to encode binary
	std::ifstream metaFile(metaPath);
	std::string metaLine;
	std::getline(metaFile, metaLine);
	std::string serializedVertex;
	std::filesystem::path outPath = outputPath;
	if (ourModel.animations.size()) {
		std::cout << "Attempt to read animation file";
		std::cout << metaPath << '\n';
		std::cout << metaLine << '\n';
		std::cout << outputPath << '\n';
		// Change extension, e.g. ".anim"
		outPath.replace_extension(".anim");
		std::cout << outPath.string() << '\n';
		ParseAnimation(serializedVertex, ourModel.animations[0]);

		std::filesystem::path dir = std::filesystem::path(outputPath).parent_path();

		if (!std::filesystem::exists(dir)) {
			if (!std::filesystem::create_directories(dir)) {
				std::cerr << "Failed to create directories: " << dir << "\n";
				return 1;
			}
		}

		std::ofstream file(outPath, std::ios::binary);
		if (!file) {
			std::cerr << "Failed to open file for writing\n";
			return 0;
		}

		file.write(serializedVertex.data(), serializedVertex.size());
		file.close();
	}
	////Attempt decoding the binary
	//float decodedResult = br.DecodeBinary<float>(result);
	//std::cout << "Decoded result is" << decodedResult << '\n';
	//Transform to mesh
	serializedVertex.clear();
	serializedVertex += br.EncodeBinary(ourModel.meshes.size());
	for (Mesh& mesh : ourModel.meshes) {
		//Add vertexes inside
		serializedVertex += br.EncodeBinary(mesh.vertices.size());
		for (Vertex& vert : mesh.vertices) {
			//Encode position
			serializedVertex += br.EncodeBinary(vert.Position.x);
			serializedVertex += br.EncodeBinary(vert.Position.y);
			serializedVertex += br.EncodeBinary(vert.Position.z);
			//Encode Normal
			serializedVertex += br.EncodeBinary(vert.Normal.x);
			serializedVertex += br.EncodeBinary(vert.Normal.y);
			serializedVertex += br.EncodeBinary(vert.Normal.z);
			//Encode Texcoords
			serializedVertex += br.EncodeBinary(vert.TexCoords.x);
			serializedVertex += br.EncodeBinary(vert.TexCoords.y);
			//Encode Tangent
			serializedVertex += br.EncodeBinary(vert.Tangent.x);
			serializedVertex += br.EncodeBinary(vert.Tangent.y);
			serializedVertex += br.EncodeBinary(vert.Tangent.z);
			//Encode Bitangent
			serializedVertex += br.EncodeBinary(vert.Bitangent.x);
			serializedVertex += br.EncodeBinary(vert.Bitangent.y);
			serializedVertex += br.EncodeBinary(vert.Bitangent.z);

			//Encode bone ID and weight ID
			serializedVertex += br.EncodeBinary(vert.m_BoneIDs[0]);
			serializedVertex += br.EncodeBinary(vert.m_BoneIDs[1]);
			serializedVertex += br.EncodeBinary(vert.m_BoneIDs[2]);
			serializedVertex += br.EncodeBinary(vert.m_BoneIDs[3]);

			serializedVertex += br.EncodeBinary(vert.m_Weights[0]);
			serializedVertex += br.EncodeBinary(vert.m_Weights[1]);
			serializedVertex += br.EncodeBinary(vert.m_Weights[2]);
			serializedVertex += br.EncodeBinary(vert.m_Weights[3]);

		}
		//Add indices inside
		serializedVertex += br.EncodeBinary(mesh.indices.size());
		for (unsigned int indice : mesh.indices) {
			//Encode position
			serializedVertex += br.EncodeBinary(indice);

		}

		//Add bone info and other data
		//Serialize anim data
	
	}
	auto& bm=ourModel.GetBoneMap();
	serializedVertex += br.EncodeBinary(bm.size());
	std::cout <<"BM size: " << bm.size() << '\n';
	for (auto& pair : bm) {
		serializedVertex += br.EncodeBinary(pair.first.size());
		std::cout << "Key info" << pair.first.size() << '\n';
		for (char ch: pair.first) {
			serializedVertex += br.EncodeBinary(ch);
			//std::cout << "CHAR " << ch;
		}
		std::cout << std::endl;
		serializedVertex += br.EncodeBinary(pair.second);
	}

	auto& bim = ourModel.GetBoneInfo();
	std::cout << "Bim size: " << bim.size() << '\n';

	serializedVertex += br.EncodeBinary(bim.size());
	for (BoneInfo bi : bim) {
		/*serializedVertex += br.EncodeBinary(bi.offsetMatrix[0][0]);
		serializedVertex += br.EncodeBinary(bi.offsetMatrix[0][1]);
		serializedVertex += br.EncodeBinary(bi.offsetMatrix[0][2]);
		serializedVertex += br.EncodeBinary(bi.offsetMatrix[0][3]);

		serializedVertex += br.EncodeBinary(bi.offsetMatrix[1][0]);
		serializedVertex += br.EncodeBinary(bi.offsetMatrix[1][1]);
		serializedVertex += br.EncodeBinary(bi.offsetMatrix[1][2]);
		serializedVertex += br.EncodeBinary(bi.offsetMatrix[1][3]);

		serializedVertex += br.EncodeBinary(bi.offsetMatrix[2][0]);
		serializedVertex += br.EncodeBinary(bi.offsetMatrix[2][1]);
		serializedVertex += br.EncodeBinary(bi.offsetMatrix[2][2]);
		serializedVertex += br.EncodeBinary(bi.offsetMatrix[2][3]);

		serializedVertex += br.EncodeBinary(bi.offsetMatrix[3][0]);
		serializedVertex += br.EncodeBinary(bi.offsetMatrix[3][1]);
		serializedVertex += br.EncodeBinary(bi.offsetMatrix[3][2]);
		serializedVertex += br.EncodeBinary(bi.offsetMatrix[3][3]);*/
		serializedVertex += br.EncodeBinary(bi.offsetMatrix);
		serializedVertex += br.EncodeBinary(bi.finalTransformation);


	}
	//Test encoding and decoding a whole vertex component
	//Vertex testVertex;
	//testVertex.Position = glm::vec3{ 1.f,2.f,3.15f };
	//testVertex.Normal = glm::vec3{ 1.f,2.f,3.15f };
	//testVertex.TexCoords = glm::vec2{ 1.f,4.2f};
	//testVertex.Tangent = glm::vec3{ 1.f,2.f,3.15f };
	//testVertex.Bitangent = glm::vec3{ 1.f,2.f,3.15f };
	outPath.replace_extension(".mesh");

	std::filesystem::path dir = std::filesystem::path(outputPath).parent_path();

	if (!std::filesystem::exists(dir)) {
		if (!std::filesystem::create_directories(dir)) {
			std::cerr << "Failed to create directories: " << dir << "\n";
			return 1;
		}
	}

	std::ofstream file(outPath.string(), std::ios::binary);
	if (!file) {
		std::cerr << "Failed to open file for writing\n";
		return 0;
	}

	file.write(serializedVertex.data(), serializedVertex.size());
	file.close();

	//std::cout<<"Written data is " << serializedVertex << '\n';
	////Decode what ever result there is
	//std::ifstream inputFile("test.mesh", std::ios::binary);
	//if (!file) {
	//	throw std::runtime_error("Failed to open mesh file");
	//}

	//std::string serialized((std::istreambuf_iterator<char>(inputFile)),
	//	std::istreambuf_iterator<char>());
	//unsigned int meshCount = br.DecodeBinary<size_t>(serialized);
	//std::cout << "Mesh count is " << meshCount<<' ' <<ourModel.meshes.size() << '\n';
}