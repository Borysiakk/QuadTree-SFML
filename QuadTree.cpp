﻿#include "QuadTree.hpp"
#include <iostream>

QuadTree::QuadTree(sf::IntRect Bounds, std::vector<QuadTree*> & ListNode):mBounds(Bounds),mListNode(ListNode)
{
	rectangle.setPosition(mBounds.left, mBounds.top);
	rectangle.setSize(sf::Vector2f(mBounds.width, mBounds.height));
	rectangle.setFillColor(sf::Color::Transparent);
	rectangle.setOutlineColor(sf::Color::Black);
	rectangle.setOutlineThickness(-0.5f);
	mListNode.push_back(this);
}

void QuadTree::insert(Object::Ptr Object)
{
	bool end = false;
	QuadTree * root = this;
	ObjectsQuadTree ObjectTemp;
	while (!end)
	{
		if (root->isNodeTree() == true)
		{
			IntersectsType type = root->intersects(Object->getBoundingBox());
			if (type != IntersectsType::None)
			{
				root = root->mChildren[static_cast<int>(type)].get();
			}
			else
			{
				root->mObjects.push_back(Object);
				end = true;
			}
		}
		else
		{
			if (root->mObjects.size() >= 2)
			{
				root->mObjects.push_back(Object);
				root->CreateArrayChildren();
				for (auto & obj : root->mObjects)
				{
					auto type = root->intersects(obj->getBoundingBox());
					if (type == IntersectsType::None)
					{
						ObjectTemp.push_back(obj);
					}
					else
					{
						root->insert(obj);
					}
				}
				root->mObjects = std::move(ObjectTemp);
				end = true;
			}
			else
			{
				root->mObjects.push_back(Object);
				end = true;
			}
		}
	}
}

bool QuadTree::isNodeTree()
{
	if (mChildren[0] != nullptr)return true;
	else return false;
}

void QuadTree::CreateArrayChildren()
{
	int Width = mBounds.width / 2;
	int Height = mBounds.height / 2;
	int MiddleX = mBounds.left + (mBounds.width / 2);
	int MiddleY = mBounds.top + (mBounds.height / 2);

	mChildren[0] = std::make_unique<QuadTree>(sf::IntRect(mBounds.left, mBounds.top, Width, Height), mListNode);
	mChildren[1] = std::make_unique<QuadTree>(sf::IntRect(MiddleX, mBounds.top, Width, Height), mListNode);
	mChildren[2] = std::make_unique<QuadTree>(sf::IntRect(mBounds.left, MiddleY, Width, Height), mListNode);
	mChildren[3] = std::make_unique<QuadTree>(sf::IntRect(MiddleX, MiddleY, Width, Height), mListNode);
}

IntersectsType QuadTree::intersects(sf::FloatRect rect)
{
	int MiddleX = mBounds.left + (mBounds.width / 2);
	int MiddleY = mBounds.top + (mBounds.height / 2);

	bool top = (rect.top + rect.height > MiddleY);
	bool bottom = (rect.top < MiddleY);

	bool right = (rect.left + rect.width > MiddleX);
	bool left = (rect.left < MiddleX);

	if (top == true && bottom == true)
	{
		return IntersectsType::None;
	}

	else if (top == true && bottom == false)
	{
		//Południe
		if (right == true && left == true)
		{
			return IntersectsType::None;
		}
		else if (right == true && left == false)
		{
			return IntersectsType::SouthEast;
		}
		else if (right == false && left == true)
		{
			return IntersectsType::SouthWest;
		}
	}
	else if (top == false && bottom == true)
	{
		if (right == true && left == true)
		{
			return IntersectsType::None;
		}
		else if (right == true && left == false)
		{
			return IntersectsType::NorthEast;
		}
		else if (right == false && left == true)
		{
			return IntersectsType::NorthWest;
		}
	}
	else
	{
		return IntersectsType::None;
	}
}

QuadTree::ObjectsQuadTree QuadTree::getElement(sf::FloatRect rect)
{
	return find(this, rect)->mObjects;
}

QuadTree * QuadTree::find(QuadTree * root, sf::FloatRect & rect)
{
	if (root->isNodeTree() == true)
	{
		auto type = root->intersects(rect);
		if (type == IntersectsType::None)
		{
			return root;
		}
		else
		{
			return find(root->mChildren[static_cast<int>(type)].get(), rect);
		}
	}
	else
	{
		return root;
	}
}

void QuadTree::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	for (auto & node : mListNode)
	{
		target.draw(node->rectangle);
		for (auto & obj : node->mObjects)
		{
			target.draw(*obj);
		}
	}
}