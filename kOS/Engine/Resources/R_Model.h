#pragma once
#ifndef RMESH_H
#define RMESH_H

#include "Config/pch.h"
#include "Graphics/GraphicsReferences.h"
#include "Resource.h"
#include "Graphics/Shader.h"
#include "Graphics/Material.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#define MAX_BONE_INFLUENCE 4

struct BoneInfo
{
	glm::mat4 offsetMatrix;      // From ASSIMP during loading
	glm::mat4 finalTransformation{ 1.f }; // Calculated each frame
};

class R_Model : public Resource {

	class Model;
	class Bone;
	class Animation;
	class Animator;

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		// tangent
		glm::vec3 Tangent;
		// bitangent
		glm::vec3 Bitangent;
		//bone indexes which will influence this vertex
		int m_BoneIDs[MAX_BONE_INFLUENCE];
		//weights from each bone
		float m_Weights[MAX_BONE_INFLUENCE];
	};



	struct NodeData
	{
		std::string name;
		glm::mat4 transformation;
		std::vector<NodeData> children;
	};

	struct ModelData
	{
		std::string modelFileName{};
		std::string skeletonFileName{};
		glm::mat4 transformation{ 1.f };
		glm::vec4 colour{ 1.f };
		float animationTimer{};
	};

	class Mesh
	{
	public:
		// mesh data
		std::vector<Vertex>       vertices;
		std::vector<unsigned int> indices;
		std::vector<Textures>      textures;

		Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Textures> textures);
		void Draw(Shader& shader);
		void PBRDraw(Shader& shader, PBRMaterial const& mat);
	private:
		//  render data
		unsigned int VAO, VBO, EBO;

		void SetupMesh();
	};

	class Animation
	{
	public:

		Animation(const aiAnimation* anim, const aiScene* scene, std::unordered_map<std::string, int>& boneMap);

		const Bone* FindBone(const std::string& name) const;
		float GetDuration() const;
		float GetTicksPerSecond() const;
		const NodeData& GetRootNode() const;

	private:
		float m_Duration;
		float m_TicksPerSecond;
		std::string m_Name;
		std::unordered_map<std::string, Bone> m_Bones;


		NodeData CopyNodeHierarchy(const aiNode* src);
		NodeData m_RootNode;

		glm::mat4 ConvertToGLMMat4(const aiMatrix4x4& original);
	};

	class Animator
	{
	public:
		Animator(const Animation* animation, const std::vector<BoneInfo>& boneInfo, const std::unordered_map<std::string, int>& boneMap, const glm::mat4& globalInverse);

		void Update(float dt, glm::mat4 parentTransform);
		const std::vector<glm::mat4>& GetFinalBoneMatrices() const;

	private:
		const Animation* m_CurrentAnimation;
		std::vector<glm::mat4> m_FinalBoneMatrices;
		std::vector<BoneInfo> m_BoneInfo;
		std::unordered_map<std::string, int> m_BoneMap;
		glm::mat4 m_GlobalInverse{ 1.f };
		float m_CurrentTime = 0.0f;

		void CalculateBoneTransform(const NodeData& node, const glm::mat4& parentTransform);
		glm::mat4 ConvertToGLMMat4(const aiMatrix4x4& original);
	};

	class Bone
	{
	public:
		Bone() = default;
		Bone(const std::string& name, int id, const aiNodeAnim* channel);

		glm::mat4 Interpolate(float time) const;

		const std::string& GetName() const;
		int GetID() const;

	private:
		std::string m_Name;
		int m_ID;

		std::vector<glm::vec3> m_Positions;
		std::vector<float> m_PosTimes;

		std::vector<glm::quat> m_Rotations;
		std::vector<float> m_RotTimes;

		std::vector<glm::vec3> m_Scales;
		std::vector<float> m_ScaleTimes;

		int FindIndex(const std::vector<float>& times, float animTime) const;
		float GetFactor(float start, float end, float time) const;

		glm::mat4 InterpolatePosition(float time) const;
		glm::mat4 InterpolateRotation(float time) const;
		glm::mat4 InterpolateScale(float time) const;
	};

public:

	using Resource::Resource;

	void Load() override;

	void Unload() override;
	//Do get resource R_Model

	~R_Model() override {
		Unload(); // safe here because we're still in the derived destructor
	}

	void Draw(Shader& shader);
	void PBRDraw(Shader& shader, std::shared_ptr < PBRMaterial> const& pbrMat);
	void DrawAnimation(Shader& shader, PBRMaterial const& pbrMat, const std::vector<glm::mat4>& boneMatrices);

	const std::vector<Animation>& GetAnimations() const { return animations; }
	const std::vector<BoneInfo>& GetBoneInfo() const { return bone_info; }
	const std::unordered_map<std::string, int>& GetBoneMap() const { return bones_loaded; }
	std::unordered_map<std::string, int>& GetBoneMap() { return bones_loaded; }
	glm::mat4 GetGlobalInverse() const { return globalInverseTransform; }
	void LoadMesh(std::string meshFile);

	/// <summary>
	/// TEMPORARY HERE
	/// </summary>
	std::vector<Animation> animations;

	REFLECTABLE(R_Model);

private:

	template <typename T> T DecodeBinary(std::string& bin, int& offset);
	std::vector<Textures> textures_loaded;
	std::unordered_map<std::string, int> bones_loaded;
	std::vector<BoneInfo> bone_info; // Only contains the matrices of the bones not the bone itself
	// model data
	std::vector<Mesh> meshes;

	std::string directory;

	//For animation purposes
	glm::mat4 globalInverseTransform{ 1.f };


	void LoadModel(std::string path);

	void ExtractBoneWeights(aiMesh* mesh, std::vector<Vertex>& vertices);

	void ProcessNode(aiNode* node, const aiScene* scene, const aiMatrix4x4& transform);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene, const aiMatrix4x4& transform);
	std::vector<Textures> LoadMaterialTextures(aiMaterial* mat, aiTextureType type,
		TextureType typeName);
	std::vector<Textures> LoadMaterialTextures(std::string path, TextureType typeName);

	//Utility
	glm::mat4 ConvertToGLMMat4(const aiMatrix4x4& original);

};

#endif // !RMESH_H
