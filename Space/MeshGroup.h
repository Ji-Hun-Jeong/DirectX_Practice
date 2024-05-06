#pragma once
class Mesh;
class Normal;
class MeshGroup
{
public:
	explicit MeshGroup(const Vector3& translation, const Vector3& rotation1, const Vector3& rotation2, const Vector3& scale);
	void AddMesh(shared_ptr<Mesh> mesh);
	void Update(float dt,bool updateNormal);
	void Render(ID3D11DeviceContext* context,bool drawNormal);
	shared_ptr<Mesh>& GetMesh(const string& name);
private:
	vector<shared_ptr<Mesh>> m_vecMeshes;
	vector<shared_ptr<Normal>> m_vecNormals;
public:
	Vector3 m_translation;
	Vector3 m_rotation1;
	Vector3 m_rotation2;
	Vector3 m_scale;
};
