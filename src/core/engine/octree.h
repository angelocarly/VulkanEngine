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

class OctreeNode: public IRenderable
{
public:

	~OctreeNode()
	{
//		for (int i = 0; i < 8; i++) {
//			if (_children[i] != nullptr) {
//				delete _children[i];
//				_children[i] = nullptr;
//			}
//		}
	}

	OctreeNode(glm::vec3 position, glm::vec3 size)
		: _parent(nullptr), _parentIndex(ROOT), _position(position), _size(size)
	{
//        for(int i = 0; i<8; i++)
//        {
//            _children.push_back();
//        }
		for (int i = 0; i < 8; i++) {
			_children[i] = nullptr;
		}
	};

	static OctreeNode convert_array(float* data, int dimension, glm::vec3 pos, glm::vec3 size)
	{
		OctreeNode root(pos, size);

		OctreeNode *node = &root;
		int max_level = (int)ceil(log(dimension) / log(2)) - 1;

		int level = 0;
		int section = 0;
		glm::ivec3 rpos = glm::ivec3(0);
		std::vector<int> sectionstack;
		while (true) {
			bool nextnode = false;
			for (; section < 8; section++) {
				node->addChild(section);
				node->getChildren()[section]->setColor(glm::vec3(-1));

				if (level == max_level) {
					// move array in node
					int nodesize = pow(2, max_level - level);
					glm::ivec3 npos = rpos + glm::ivec3(section % 2, (int) floor(section % 4 / 2.0f), (int) floor(section / 4.0f)) * nodesize;
					node->getChildren()[section]
						->setColor(glm::vec3(data[npos.x + npos.y * dimension + npos.z * dimension * dimension]));
				}
				else {
					// go in child
					node = node->getChildren()[section];
					nextnode = true;
					int nodesize = pow(2, max_level - level);
					rpos += glm::ivec3(section % 2, (int) floor(section % 4 / 2), (int) floor(section / 4)) * nodesize;
					level++;
					sectionstack.push_back(section);
					section = 0;
					break;
				}
			}

			if (nextnode) continue;

			if (node->hasParent()) {
				// go to parent
				level--;
				node = node->getParent();
				section = sectionstack.back();
				sectionstack.pop_back();
				int nodesize = pow(2, max_level - level);
				rpos -= glm::ivec3(section % 2, (int) floor(section % 4 / 2), (int) floor(section / 4)) * nodesize;
				section++;
			}
			else {
				break;
			}
		}

		return root;
	}

	void cleanup_empty_nodes()
	{
		OctreeNode *o = this;
		std::vector<int> sectionstack;
		int section = 0;
		while (true) {
			bool nextnode = false;

			if (!o->hasChildren()) {
				if (o->getColor() == glm::vec3(-1)) {
					// delete node
					o = o->getParent();
					section = sectionstack.back();
					o->deleteChild(section);
					sectionstack.pop_back();
					section++;
					continue;
				}
				else {
					if (!o->hasParent()) break;

					o = o->getParent();
					section = sectionstack.back() + 1;
					sectionstack.pop_back();
					continue;
				}
			}

			for (; section < 8; section++) {
				if (o->getChildren()[section] != nullptr) {
					o = o->getChildren()[section];
					sectionstack.push_back(section);
					nextnode = true;
					section = 0;
					break;
				}
			}

			if (nextnode) continue;

			if (section == 7 && sectionstack.empty()) break;

			if (sectionstack.size() == 0) break;

			o = o->getParent();
			section = sectionstack.back() + 1;
			sectionstack.pop_back();
		}
	}

	OctreeNode(OctreeNode &parent, int parentIndex)
		: _parent(&parent), _parentIndex(parentIndex)
	{

		_size = glm::vec3(_parent->_size);
		_size /= 2;
		switch (parentIndex) {
		case LEFT_FRONT_TOP: _position = _parent->_position + glm::vec3(0, _size.y, 0);
			break;
		case RIGHT_FRONT_TOP: _position = _parent->_position + glm::vec3(_size.x, _size.y, 0);
			break;
		case LEFT_BACK_TOP: _position = _parent->_position + glm::vec3(0, _size.y, _size.x);
			break;
		case RIGHT_BACK_TOP: _position = _parent->_position + glm::vec3(_size.x, _size.y, _size.z);
			break;
		case LEFT_FRONT_BOTTOM: _position = _parent->_position + glm::vec3(0, 0, 0);
			break;
		case RIGHT_FRONT_BOTTOM: _position = _parent->_position + glm::vec3(_size.x, 0, 0);
			break;
		case LEFT_BACK_BOTTOM: _position = _parent->_position + glm::vec3(0, 0, _size.x);
			break;
		case RIGHT_BACK_BOTTOM: _position = _parent->_position + glm::vec3(_size.x, 0, _size.x);
			break;
		}

		for (int i = 0; i < 8; i++) {
			_children[i] = nullptr;
		}
	}

	virtual void draw(IRenderProvider &renderProvider)
	{
		if (!hasChildren()) {
			glm::mat4 transform = glm::translate(glm::mat4(1), _position);
			transform = glm::scale(transform, _size);
			renderProvider.bindModelTransform(transform);
			renderProvider.drawModel();
		}
		else {
			for (int i = 0; i < 8; i++) {
				if (_children[i] == nullptr) continue;
				_children[i]->draw(renderProvider);
			}
		}
	}

	glm::vec3 calculatePosition(glm::vec3 rootPos, float rootDimensions)
	{
		if (_parentIndex == ROOT) {
			return rootPos;
		}
		return glm::vec3(0, 0, 0);
	}

//    std::vector<std::reference_wrapper<OctreeNode>> getChildren()
//    {
//        return _children;
//    }
	OctreeNode **getChildren()
	{
		return _children;
	}

	OctreeNode *getParent()
	{
		return _parent;
	}

	glm::vec3 getSize()
	{
		return _size;
	}

	glm::vec3 getPosition()
	{
		return _position;
	}

	void addChild(int section)
	{
		if (_children[section] != nullptr) return;

		_children[section] = new OctreeNode(*this, section);
	}

	void deleteChild(int position)
	{
		if (_children[position] == nullptr) return;
//		delete _children[position];
		_children[position] = nullptr;
	}

	bool hasChildren()
	{
		for (auto i: _children) if (i != nullptr) return true;
		return false;
	}

	bool hasParent()
	{
		return _parent != nullptr;
	}

	bool getFirstEmptyChildIndex()
	{
		int index = -1;
		for (int i = 0; i < 7; i++) {
			if (_children[i] == nullptr) {
				index = i;
				break;
			}
		}
		return index;
	}

	bool getFirstChildIndex()
	{
		int index = -1;
		for (int i = 0; i < 7; i++) {
			if (_children[i] != nullptr) {
				index = i;
				break;
			}
		}
		return index;
	}

	glm::vec3 getColor()
	{
		return _color;
	}

	void setColor(glm::vec3 color)
	{
		_color = color;
	}

private:
//    std::vector<std::reference_wrapper<OctreeNode>> _children;
	OctreeNode *_children[8];
	glm::vec3 _position;
	glm::vec3 _size;

	OctreeNode *_parent;
	int _parentIndex;

	glm::vec3 _color;
};

class Octree: public IRenderable
{
public:

	Octree(vks::VksDevice device)
	{
		std::vector<vks::VksModel::Vertex> vertices{
			{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // Bottom
			{{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
			{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
			{{1.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
			{{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
			{{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}, // Top
			{{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
			{{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
			{{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
			{{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
			{{0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
			{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // Front
			{{1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
			{{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
			{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
			{{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
			{{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
			{{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // Back
			{{1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
			{{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
			{{0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
			{{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
			{{0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
			{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // Left
			{{0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
			{{0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
			{{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
			{{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
			{{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // Right
			{{1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
			{{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
			{{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
			{{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
			{{1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}
		};

		model = new vks::VksModel(device, vertices);

//		randomize();
	};

	virtual void draw(IRenderProvider &renderProvider)
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

		OctreeNode *currentNode;
		while (!unTraversedNodes.empty()) {
			currentNode = &unTraversedNodes.front().get();
			int currentLevel = levels.front();
			unTraversedNodes.pop_front();
			levels.pop_front();

			if (currentLevel >= maxlevel) continue;

			for (int i = 0; i < 8; i++) {
				currentNode->addChild(i);
				OctreeNode *child = currentNode->getChildren()[i];

				if (isCubeInside(child->getPosition(), child->getSize())) {
					unTraversedNodes.push_back(*child);
					levels.push_back(currentLevel + 1);
				}
				else {
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
	vks::VksModel *model;

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
