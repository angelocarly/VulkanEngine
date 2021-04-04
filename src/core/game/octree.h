//
// Created by magnias on 25/03/2021.
//
#ifndef VULKANENGINE_OCTREE_H
#define VULKANENGINE_OCTREE_H

#include "../graphics/renderable.h"
#include "game_entity.h"
#include <list>
#include <vector>
#include <math.h>
//#include "entity.h"

#define ROOT -1
#define LEFT_FRONT_TOP 0
#define RIGHT_FRONT_TOP 1
#define LEFT_BACK_TOP 2
#define RIGHT_BACK_TOP 3
#define LEFT_FRONT_BOTTOM 4
#define RIGHT_FRONT_BOTTOM 5
#define LEFT_BACK_BOTTOM 6
#define RIGHT_BACK_BOTTOM 7

class OctreeNode : public IRenderable
{
 public:

	~OctreeNode()
	{
		for (int i = 0; i < 8; i++)
		{
			if (_children[i] != nullptr)
			{
				delete _children[i];
			}
		}
	}

	OctreeNode(glm::vec3 position, glm::vec3 size)
		: _parent(nullptr), _parentIndex(ROOT), _position(position), _size(size)
	{
//        for(int i = 0; i<8; i++)
//        {
//            _children.push_back();
//        }
		for (int i = 0; i < 8; i++)
		{
			_children[i] = nullptr;
		}
	};

	OctreeNode(OctreeNode& parent, int parentIndex)
		: _parent(&parent), _parentIndex(parentIndex)
	{

		_size = glm::vec3(_parent->_size);
		_size /= 2;
		switch (parentIndex)
		{
		case LEFT_FRONT_TOP:
			_position = _parent->_position + glm::vec3(0, _size.y, 0);
			break;
		case RIGHT_FRONT_TOP:
			_position = _parent->_position + glm::vec3(_size.x, _size.y, 0);
			break;
		case LEFT_BACK_TOP:
			_position = _parent->_position + glm::vec3(0, _size.y, _size.x);
			break;
		case RIGHT_BACK_TOP:
			_position = _parent->_position + glm::vec3(_size.x, _size.y, _size.z);
			break;
		case LEFT_FRONT_BOTTOM:
			_position = _parent->_position + glm::vec3(0, 0, 0);
			break;
		case RIGHT_FRONT_BOTTOM:
			_position = _parent->_position + glm::vec3(_size.x, 0, 0);
			break;
		case LEFT_BACK_BOTTOM:
			_position = _parent->_position + glm::vec3(0, 0, _size.x);
			break;
		case RIGHT_BACK_BOTTOM:
			_position = _parent->_position + glm::vec3(_size.x, 0, _size.x);
			break;
		}

		for (int i = 0; i < 8; i++)
		{
			_children[i] = nullptr;
		}
	}

	virtual void draw(IRenderProvider& renderProvider)
	{
		if (!hasChildren())
		{
			glm::mat4 transform = glm::translate(glm::mat4(1), _position);
			transform = glm::scale(transform, _size);
			renderProvider.bindModelTransform(transform);
			renderProvider.drawModel();
		}
		else
		{
			for (int i = 0; i < 8; i++)
			{
				if (_children[i] == nullptr) continue;
				_children[i]->draw(renderProvider);
			}
		}
	}

	glm::vec3 calculatePosition(glm::vec3 rootPos, float rootDimensions)
	{
		if (_parentIndex == ROOT)
		{
			return rootPos;
		}
		return glm::vec3(0, 0, 0);
	}

//    std::vector<std::reference_wrapper<OctreeNode>> getChildren()
//    {
//        return _children;
//    }
	OctreeNode** getChildren()
	{
		return _children;
	}

	glm::vec3 getSize()
	{
		return _size;
	}

	glm::vec3 getPosition()
	{
		return _position;
	}

	void addChild(int position)
	{
		if (_children[position]) return;

		_children[position] = new OctreeNode(*this, position);
	}

	void deleteChild(int position)
	{
		if (_children[position] == nullptr) return;
		delete _children[position];
		_children[position] = nullptr;
	}

	bool hasChildren()
	{
		for (int i = 0; i < 8; i++) if (_children[i]) return true;
		return false;
	}

 private:
//    std::vector<std::reference_wrapper<OctreeNode>> _children;
	OctreeNode* _children[8];
	glm::vec3 _position;
	glm::vec3 _size;

	OctreeNode* _parent;
	int _parentIndex;
};

class Octree : public IRenderable
{
 public:

	Octree(vks::VksDevice device)
	{
		std::vector<vks::VksModel::Vertex> vertices{
			{{ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }}, // Bottom
			{{ 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }},
			{{ 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f }},
			{{ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }},
			{{ 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f }},
			{{ 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }},
			{{ 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }}, // Top
			{{ 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }},
			{{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f }},
			{{ 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }},
			{{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f }},
			{{ 0.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }},
			{{ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }}, // Front
			{{ 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }},
			{{ 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }},
			{{ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }},
			{{ 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }},
			{{ 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }},
			{{ 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }}, // Back
			{{ 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }},
			{{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f }},
			{{ 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }},
			{{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f }},
			{{ 0.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }},
			{{ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }}, // Left
			{{ 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }},
			{{ 0.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f }},
			{{ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }},
			{{ 0.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f }},
			{{ 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }},
			{{ 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }}, // Right
			{{ 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }},
			{{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f }},
			{{ 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }},
			{{ 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f }},
			{{ 1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }}
		};

		model = new vks::VksModel(device, vertices);

		randomize();
	};

	virtual void draw(IRenderProvider& renderProvider)
	{
		renderProvider.bindModel(*model);
		root.draw(renderProvider);
	}

	void randomize()
	{
		int maxlevel = 5;

		std::list<std::reference_wrapper<OctreeNode>> unTraversedNodes;
		std::list<int> levels;

		unTraversedNodes.emplace_back(root);
		levels.emplace_back(0);

		OctreeNode* currentNode;
		while (!unTraversedNodes.empty())
		{
			currentNode = &unTraversedNodes.front().get();
			int currentLevel = levels.front();
			unTraversedNodes.pop_front();
			levels.pop_front();

			if (currentLevel >= maxlevel) continue;

			for (int i = 0; i < 8; i++)
			{
				currentNode->addChild(i);
				OctreeNode* child = currentNode->getChildren()[i];

				if (isCubeInside(child->getPosition(), child->getSize()))
				{
					unTraversedNodes.push_back(*child);
					levels.push_back(currentLevel + 1);
				}
				else
				{
					currentNode->deleteChild(i);
				}
			}

		}
//        for (int i = 0; i < 10; i++)
//        {
//            currentNode->addChild(2);
//            currentNode->addChild(1);
//            currentNode->addChild(4);
//            currentNode->addChild(3);
//
//            currentNode = currentNode->getChildren()[3];
//        }
	}

 private:
	OctreeNode root = OctreeNode(glm::vec3(), glm::vec3(10));
	vks::VksModel* model;

	bool isCubeInside(glm::vec3 pos, glm::vec3 size)
	{
		if (isInside(glm::vec3(pos.x, pos.y, pos.z))) return true;
		if (isInside(glm::vec3(pos.x + size.x, pos.y, pos.z))) return true;
		if (isInside(glm::vec3(pos.x, pos.y + size.y, pos.z))) return true;
		if (isInside(glm::vec3(pos.x + size.x, pos.y + size.y, pos.z))) return true;
		if (isInside(glm::vec3(pos.x, pos.y, pos.z + size.z))) return true;
		if (isInside(glm::vec3(pos.x + size.x, pos.y, pos.z + size.z))) return true;
		if (isInside(glm::vec3(pos.x, pos.y + size.y, pos.z + size.z))) return true;
		if (isInside(glm::vec3(pos.x + size.x, pos.y + size.y, pos.z + size.z))) return true;
		if (isInside(glm::vec3(pos.x + size.x / 2, pos.y + size.y / 2, pos.z + size.z / 2))) return true;
		return false;
	}

	bool isInside(glm::vec3 pos)
	{
		float dist = glm::distance(pos, glm::vec3(5));

		return dist < 2.0f;
	}
};

#endif //VULKANENGINE_OCTREE_H
