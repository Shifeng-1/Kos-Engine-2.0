#include "Config/pch.h"
#include "R_Model.h"

void PrintMat4(const glm::mat4& mat) {
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            std::cout << mat[col][row] << ' ';
        }
        std::cout << '\n';
    }
}

void R_Model::Load()
{
    LoadMesh(this->m_filePath.string());
}

void R_Model::Unload()
{

}
/*------------------------------------------------------------------------------------------*/
/*----------------------------------------MESH----------------------------------------------*/
/*------------------------------------------------------------------------------------------*/




R_Model::Mesh::Mesh(std::vector<Vertex> newVert, std::vector<unsigned int> newIndices, std::vector<Textures> newTextures)
    :vertices{ newVert }
    , indices{ newIndices }
    , textures{ newTextures }
{
    //set up mesh based on data
    SetupMesh();
}

void R_Model::Mesh::SetupMesh()
{
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
     // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
    glBindVertexArray(0);
}

void R_Model::Mesh::PBRDraw(Shader& shader, PBRMaterial const& mat) {


    //Set material textures
    //std::cout << textures.size() << '\n';
    std::string number;
    std::string name{};
    //Bind albedo
    glActiveTexture(GL_TEXTURE0); // activate proper texture unit before binding
    shader.SetInt("texture_diffuse1", 0);
    unsigned int currentTexture = 0;
    currentTexture = (mat.albedo) ? mat.albedo->RetrieveTexture() : 0;
    glBindTexture(GL_TEXTURE_2D, currentTexture);
    //Bind sepcular
    glActiveTexture(GL_TEXTURE1); // activate proper texture unit before binding
    shader.SetInt("texture_specular1", 1);
    currentTexture = (mat.specular) ? mat.specular->RetrieveTexture() : 0;
    glBindTexture(GL_TEXTURE_2D, currentTexture);
    //Bind normal
    glActiveTexture(GL_TEXTURE2); // activate proper texture unit before binding
    shader.SetInt("texture_normal1", 2);
    currentTexture = (mat.normal) ? mat.normal->RetrieveTexture() : 0;
    glBindTexture(GL_TEXTURE_2D, currentTexture);
    //Bind Metallic map
    glActiveTexture(GL_TEXTURE4); // activate proper texture unit before binding
    shader.SetInt("texture_ao1", 4);
    currentTexture = (mat.ao) ? mat.ao->RetrieveTexture() : 0;
    glBindTexture(GL_TEXTURE_2D, currentTexture);
    //Bind roughness
    glActiveTexture(GL_TEXTURE5); // activate proper texture unit before binding
    shader.SetInt("texture_roughness1", 5);
    currentTexture = (mat.roughness) ? mat.roughness->RetrieveTexture() : 0;
    glBindTexture(GL_TEXTURE_2D, currentTexture);

    glActiveTexture(GL_TEXTURE0);

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

/*------------------------------------------------------------------------------------------*/
/*----------------------------------------MODEL---------------------------------------------*/
/*------------------------------------------------------------------------------------------*/

void R_Model::LoadModel(std::string path)
{
    Assimp::Importer import;
    const aiScene * scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        //std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
   //     LOGGING_ERROR("ERROR::ASSIMP::", import.GetErrorString());
        LOGGING_INFO("Attempting load for mesh ", path);
        LoadMesh(path);
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));
    ProcessNode(scene->mRootNode, scene, aiMatrix4x4{});
    LOGGING_INFO("Loaded model:  {} ", path);
}

void R_Model::LoadMesh(std::string meshFile) {
    std::ifstream inputFile(meshFile.c_str(), std::ios::binary);
    if (!inputFile) {
        LOGGING_ERROR("Failed to open mesh file");
    }
    std::string serialized((std::istreambuf_iterator<char>(inputFile)),
        std::istreambuf_iterator<char>());
    int offset = 0;
    // std::cout << "Mesh file path is: " << meshFile << '\n';
     //std::cout<<"Mesh file is: " << serialized << '\n';
    unsigned int meshCount = static_cast<unsigned int>(DecodeBinary<size_t>(serialized, offset));
    // std::cout << "Mesh  size is " << meshCount << '\n';

    for (unsigned int i{ 0 }; i < meshCount; i++) {
        //Load each vertex and indice in the thingy
        std::vector<Vertex> newVert;
        std::vector<unsigned int> newIndices;
        //Get vertex count
        unsigned int vCount = static_cast<unsigned int>(DecodeBinary<size_t>(serialized, offset));
        // std::cout << "Vertex size is " << vCount << '\n';
        for (unsigned int j{ 0 }; j < vCount; j++) {
            Vertex vert;
            vert.Position.x = DecodeBinary<float>(serialized, offset);
            vert.Position.y = DecodeBinary<float>(serialized, offset);
            vert.Position.z = DecodeBinary<float>(serialized, offset);

            vert.Normal.x = DecodeBinary<float>(serialized, offset);
            vert.Normal.y = DecodeBinary<float>(serialized, offset);
            vert.Normal.z = DecodeBinary<float>(serialized, offset);

            vert.TexCoords.x = DecodeBinary<float>(serialized, offset);
            vert.TexCoords.y = DecodeBinary<float>(serialized, offset);

            vert.Tangent.x = DecodeBinary<float>(serialized, offset);
            vert.Tangent.y = DecodeBinary<float>(serialized, offset);
            vert.Tangent.z = DecodeBinary<float>(serialized, offset);

            vert.Bitangent.x = DecodeBinary<float>(serialized, offset);
            vert.Bitangent.y = DecodeBinary<float>(serialized, offset);
            vert.Bitangent.z = DecodeBinary<float>(serialized, offset);

            ////Decode bone ids
            vert.m_BoneIDs[0] = DecodeBinary<int>(serialized, offset);
            vert.m_BoneIDs[1] = DecodeBinary<int>(serialized, offset);
            vert.m_BoneIDs[2] = DecodeBinary<int>(serialized, offset);
            vert.m_BoneIDs[3] = DecodeBinary<int>(serialized, offset);

            vert.m_Weights[0] = DecodeBinary<float>(serialized, offset);
            vert.m_Weights[1] = DecodeBinary<float>(serialized, offset);
            vert.m_Weights[2] = DecodeBinary<float>(serialized, offset);
            vert.m_Weights[3] = DecodeBinary<float>(serialized, offset);

            newVert.push_back(vert);
            //std::cout << "Pushing new vert";

        }
        unsigned int indicesCount = static_cast<unsigned int>(DecodeBinary<size_t>(serialized, offset));
        // std::cout << "Indices size is " << indicesCount << '\n';
        for (unsigned int i{ 0 }; i < indicesCount; i++) {
            newIndices.push_back(DecodeBinary<unsigned int>(serialized, offset));
        }

        // //This sets up and pushes a new mesh into the family
        this->meshes.push_back(Mesh{ newVert,newIndices,std::vector<Textures>{} });
    }
    unsigned int indicesCount = static_cast<unsigned int>(DecodeBinary<size_t>(serialized, offset));
    for (unsigned int i{ 0 }; i < indicesCount; i++) {
        unsigned int stringSize = static_cast<unsigned int>(DecodeBinary<size_t>(serialized, offset));
        //std::cout << "STRING SIZE IS " << stringSize << '\n';
        std::string key;
        for (unsigned int j{ 0 }; j < stringSize; j++) {
            key += DecodeBinary<char>(serialized, offset);
            //std::cout<<"KEY CHAR: " << key << '\n';
        }
        //std::cout << "KEY " << key << '\n';
        //return;;
        bones_loaded[key] = DecodeBinary<int>(serialized, offset);
    }

    indicesCount = static_cast<unsigned int>(DecodeBinary<size_t>(serialized, offset));

    for (unsigned int i{ 0 }; i < indicesCount; i++) {
        glm::mat4 offsetMatrix = DecodeBinary<glm::mat4>(serialized, offset);
        glm::mat4 transformationMatrix = DecodeBinary<glm::mat4>(serialized, offset);
        bone_info.push_back(BoneInfo{ offsetMatrix,transformationMatrix });
    }
}



void R_Model::ProcessNode(aiNode* node, const aiScene* scene, const aiMatrix4x4& transform)
{
    const aiMatrix4x4 accTransform = transform * node->mTransformation;
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(ProcessMesh(mesh, scene, accTransform));
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene, accTransform);
    }
    //Include the animations for the .fbx model if any
    if (scene->HasAnimations())
    {
        //for (unsigned int i = 0; i < scene->mNumAnimations; ++i)
       // {
           // animations.emplace_back(scene->mAnimations[i], scene, bones_loaded);
       // }
    }
}

//Process meshes
R_Model::Mesh R_Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, const aiMatrix4x4& transform) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Textures> textures;
    const aiMatrix3x3 transform3x3 = aiMatrix3x3(transform);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        mesh->mVertices[i] = transform * mesh->mVertices[i];
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // normals
        if (mesh->HasNormals())
        {
            mesh->mNormals[i] = transform3x3 * mesh->mNormals[i];
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }

        // texture coordinates
        if (mesh->mTextureCoords[0]) //Check if got texturre coords
        {
            // std::cout << "LOADING TEXTURE\n";
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    // 1. diffuse maps
    //std:: vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, DIFFUSE);
    //textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    //// 2. specular maps
    //std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, SPECULAR);
    //textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    //// 3. normal maps
    //std::vector<Texture> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, NORMAL);
    //textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    //// 3. AO maps
    //std::vector<Texture> heightMaps = LoadMaterialTextures("ao.jpg",HEIGHT);
    //textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    //std::vector<Texture> roughnessMaps = LoadMaterialTextures("roughness.jpg", ROUGHNESS);
    //textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());


    // Extract bones and weights here
    ExtractBoneWeights(mesh, vertices);

    // return a mesh object created from the extracted mesh data
    return Mesh(vertices, indices, textures);
}

void R_Model::ExtractBoneWeights(aiMesh* mesh, std::vector<Vertex>& vertices)
{
    // if (mesh->mNumBones == 0)
    //     std::cout << "No Bones Detected" << std::endl;

    for (auto& v : vertices)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
            v.m_BoneIDs[i] = -1;    // use -1 to indicate "no bone"
            v.m_Weights[i] = 0.0f;
        }
    }

    for (unsigned int i = 0; i < mesh->mNumBones; i++)
    {
        std::string boneName = mesh->mBones[i]->mName.C_Str();
        int boneID{};

        if (bones_loaded.find(boneName) == bones_loaded.end())
        {
            boneID = static_cast<int>(bones_loaded.size());
            bones_loaded[boneName] = boneID;

            BoneInfo boneInfo;
            boneInfo.offsetMatrix = ConvertToGLMMat4(mesh->mBones[i]->mOffsetMatrix);
            //bone_info[boneID] = boneInfo;
            bone_info.push_back(boneInfo);
        }
        else
        {
            boneID = bones_loaded[boneName];
        }

        aiBone* bone = mesh->mBones[i];

        // Assign weights to vertices
        for (unsigned int j = 0; j < bone->mNumWeights; ++j)
        {
            int vertexID = bone->mWeights[j].mVertexId;
            float weight = bone->mWeights[j].mWeight;

            for (unsigned int k = 0; k < MAX_BONE_INFLUENCE; k++)
            {
                if (vertices[vertexID].m_Weights[k] == 0.0f)
                {
                    vertices[vertexID].m_BoneIDs[k] = boneID;
                    vertices[vertexID].m_Weights[k] = weight;
                    break;
                }
            }
        }
    }

    for (auto& v : vertices)
    {
        float total = 0.0f;
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
            total += v.m_Weights[i];
        if (total > 0.0f)
        {
            for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
                v.m_Weights[i] /= total;
        }
    }
}

template<typename T>
inline T R_Model::DecodeBinary(std::string& bin, int& offset)
{


    // Check if we have enough bytes remaining
    if (offset + sizeof(T) > bin.size()) {
        LOGGING_DEBUG("File overflow");;
        return T{};;
        // return ;
    }
    T value;
    // Copy the raw bytes directly from the string
    std::memcpy(&value, bin.data() + offset, sizeof(T));
    offset += sizeof(T);

    return value;
}

void R_Model::PBRDraw(Shader& shader, PBRMaterial const& pbrMat) {
    shader.SetBool("isNotRigged", false);
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].PBRDraw(shader, pbrMat);
}

void R_Model::DrawAnimation(Shader& shader, PBRMaterial const& pbrMat, const std::vector<glm::mat4>& boneMatrices)
{
    shader.SetBool("isNotRigged", true);
    if (!boneMatrices.empty())
    {
        for (int i = 0; i < boneMatrices.size(); i++)
        {
            shader.SetMat4("bones[" + std::to_string(i) + "]", boneMatrices[i]);
        }
    }
    // shader.SetMat4Array("bones", boneMatrices[0], boneMatrices.size());
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].PBRDraw(shader, pbrMat);
}

glm::mat4 R_Model::ConvertToGLMMat4(const aiMatrix4x4& original)
{
    glm::mat4 transformed{};

    transformed[0][0] = original.a1; transformed[1][0] = original.a2; transformed[2][0] = original.a3; transformed[3][0] = original.a4;
    transformed[0][1] = original.b1; transformed[1][1] = original.b2; transformed[2][1] = original.b3; transformed[3][1] = original.b4;
    transformed[0][2] = original.c1; transformed[1][2] = original.c2; transformed[2][2] = original.c3; transformed[3][2] = original.c4;
    transformed[0][3] = original.d1; transformed[1][3] = original.d2; transformed[2][3] = original.d3; transformed[3][3] = original.d4;

    return transformed;
}

/*------------------------------------------------------------------------------------------*/
/*----------------------------------------BONES---------------------------------------------*/
/*------------------------------------------------------------------------------------------*/

R_Model::Bone::Bone(const std::string& name, int id, const aiNodeAnim* channel) : m_Name(name), m_ID(id)
{
    //If the animation channel exists
    if (channel)
    {
        // Load position keyframes
        for (unsigned int i = 0; i < channel->mNumPositionKeys; i++)
        {
            m_Positions.push_back(glm::vec3(
                channel->mPositionKeys[i].mValue.x,
                channel->mPositionKeys[i].mValue.y,
                channel->mPositionKeys[i].mValue.z));
            m_PosTimes.push_back((float)channel->mPositionKeys[i].mTime);
        }

        // Load rotation keyframes
        for (unsigned int i = 0; i < channel->mNumRotationKeys; i++)
        {
            aiQuaternion q = channel->mRotationKeys[i].mValue;
            m_Rotations.push_back(glm::quat(q.w, q.x, q.y, q.z));
            m_RotTimes.push_back((float)channel->mRotationKeys[i].mTime);
        }

        // Load scaling keyframes
        for (unsigned int i = 0; i < channel->mNumScalingKeys; i++)
        {
            m_Scales.push_back(glm::vec3(
                channel->mScalingKeys[i].mValue.x,
                channel->mScalingKeys[i].mValue.y,
                channel->mScalingKeys[i].mValue.z));
            m_ScaleTimes.push_back((float)channel->mScalingKeys[i].mTime);
        }
    }
}

glm::mat4 R_Model::Bone::Interpolate(float time) const
{
    glm::mat4 T = InterpolatePosition(time);
    glm::mat4 R = InterpolateRotation(time);
    glm::mat4 S = InterpolateScale(time);
    return T * R * S;
}

const std::string& R_Model::Bone::GetName() const
{
    return m_Name;
}

int R_Model::Bone::GetID() const
{
    return m_ID;
}

int R_Model::Bone::FindIndex(const std::vector<float>& times, float animTime) const
{
    for (size_t i = 0; i < times.size() - 1; i++)
    {
        if (animTime < times[i + 1])
        {
            return static_cast<int>(i);
        }
    }
    return static_cast<int>(times.size() - 2);
}

float R_Model::Bone::GetFactor(float start, float end, float time) const
{
    float diff = end - start;
    ///Might division by zero
    return (time - start) / diff;
}

glm::mat4 R_Model::Bone::InterpolatePosition(float time) const
{
    if (m_Positions.size() == 1)
    {
        return glm::translate(glm::mat4(1.0f), m_Positions[0]);
    }

    int index = FindIndex(m_PosTimes, time);
    float factor = GetFactor(m_PosTimes[index], m_PosTimes[index + 1], time);
    glm::vec3 finalPos = glm::mix(m_Positions[index], m_Positions[index + 1], factor);
    return glm::translate(glm::mat4(1.0f), finalPos);
}

glm::mat4 R_Model::Bone::InterpolateRotation(float time) const
{
    if (m_Rotations.size() == 1)
    {
        return glm::mat4_cast(m_Rotations[0]);
    }

    int index = FindIndex(m_RotTimes, time);
    float factor = GetFactor(m_RotTimes[index], m_RotTimes[index + 1], time);
    glm::quat finalRot = glm::slerp(m_Rotations[index], m_Rotations[index + 1], factor);
    return glm::mat4_cast(finalRot);
}

glm::mat4 R_Model::Bone::InterpolateScale(float time) const
{
    if (m_Scales.size() == 1)
    {
        return glm::scale(glm::mat4(1.0f), m_Scales[0]);
    }

    int index = FindIndex(m_ScaleTimes, time);
    float factor = GetFactor(m_ScaleTimes[index], m_ScaleTimes[index + 1], time);
    glm::vec3 finalScale = glm::mix(m_Scales[index], m_Scales[index + 1], factor);
    return glm::scale(glm::mat4(1.0f), finalScale);
}

/*----------------------------------------------------------------------------------------------*/
/*----------------------------------------ANIMATION---------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/

R_Model::Animation::Animation(const aiAnimation* anim, const aiScene* scene, std::unordered_map<std::string, int>& boneMap)
{
    m_Duration = static_cast<float>(anim->mDuration);
    ///Magic number over here for now
    m_TicksPerSecond = static_cast<float>(anim->mTicksPerSecond != 0 ? anim->mTicksPerSecond : 25.0f);
    m_Name = anim->mName.C_Str();

    for (unsigned int i = 0; i < anim->mNumChannels; i++)
    {
        aiNodeAnim* channel = anim->mChannels[i];
        std::string boneName = channel->mNodeName.C_Str();

        if (boneMap.find(boneName) == boneMap.end())
        {
            boneMap[boneName] = static_cast<int>(boneMap.size());
        }

        m_Bones[boneName] = Bone(boneName, boneMap[boneName], channel);
    }
    m_RootNode = CopyNodeHierarchy(scene->mRootNode);
}

R_Model::NodeData R_Model::Animation::CopyNodeHierarchy(const aiNode* src)
{
    NodeData node;
    node.name = src->mName.C_Str();
    node.transformation = ConvertToGLMMat4(src->mTransformation);

    node.children.reserve(src->mNumChildren);
    for (unsigned int i = 0; i < src->mNumChildren; i++)
    {
        node.children.push_back(CopyNodeHierarchy(src->mChildren[i]));
    }

    return node;
}

glm::mat4 R_Model::Animation::ConvertToGLMMat4(const aiMatrix4x4& original)
{
    glm::mat4 transformed{};

    transformed[0][0] = original.a1; transformed[1][0] = original.a2; transformed[2][0] = original.a3; transformed[3][0] = original.a4;
    transformed[0][1] = original.b1; transformed[1][1] = original.b2; transformed[2][1] = original.b3; transformed[3][1] = original.b4;
    transformed[0][2] = original.c1; transformed[1][2] = original.c2; transformed[2][2] = original.c3; transformed[3][2] = original.c4;
    transformed[0][3] = original.d1; transformed[1][3] = original.d2; transformed[2][3] = original.d3; transformed[3][3] = original.d4;

    return transformed;
}

const R_Model::Bone* R_Model::Animation::FindBone(const std::string& name) const
{
    std::unordered_map<std::string, Bone>::const_iterator it = m_Bones.find(name);
    return it != m_Bones.end() ? &it->second : nullptr;
}

float R_Model::Animation::GetDuration() const { return m_Duration; }

float R_Model::Animation::GetTicksPerSecond() const { return m_TicksPerSecond; }

const R_Model::NodeData& R_Model::Animation::GetRootNode() const { return m_RootNode; }


