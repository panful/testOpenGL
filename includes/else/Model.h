#pragma once
#include <vector>

#include "Mesh.h"
#include "Buffer.h"

class Model
{
public:
	Model(std::vector<std::vector<float>>);
	~Model();

	void draw();

private:
	std::vector<Mesh> m_meshs;
};

Model::Model(std::vector<std::vector<float>> data)
{		
	for (const auto& elem : data)
	{			
		std::vector<Vertex> vecVertex;
		Vertex vertex;

		for (auto it = elem.begin(); it != elem.end();it += 6)
		{
			vertex.Position = glm::vec3(*it, *(it + 1), *(it + 2));
			vertex.Color = glm::vec3(*(it + 3), *(it + 4), *(it + 5));

			vecVertex.emplace_back(vertex);	
		}
	
		
		m_meshs.emplace_back(Mesh(vecVertex));
	}
	//m_meshs[0].m_bDisplay = false;
}

Model::~Model()
{

}

void Model::draw()
{
	for (auto& elem : m_meshs)
	{
		if (elem.m_bDisplay)  //判断当前网格是否被隐藏
		{
			elem.draw();
		}
	}
}