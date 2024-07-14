#pragma once
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>
class Mesh;
class ModelLoader 
{
public:
    void Load(std::string basePath, std::string filename);
private:
    void ProcessNode(aiNode* node, const aiScene* scene,
        DirectX::SimpleMath::Matrix tr);

    MeshData ProcessMesh(aiMesh* mesh, const aiScene* scene, aiMaterial** aiM);

    std::string ReadFilename(aiMaterial* material, aiTextureType type);

    void UpdateTangents(MeshData& meshData);

public:
    std::string basePath;
    shared_ptr<Mesh> resultMesh;
    // std::vector<MeshData> meshes;
    SINGLE(ModelLoader)
};

