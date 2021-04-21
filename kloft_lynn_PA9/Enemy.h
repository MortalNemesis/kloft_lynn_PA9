#pragma once

#include <iostream>
#include <queue>

#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Network.hpp>

#include "Level.h" // Gives access to constants

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

class Enemy : public sf::Sprite
{
public:
	Enemy(const sf::Texture& texture, const sf::Vector2f& pos = sf::Vector2f(0, 0)) : sf::Sprite(texture)
	{
		setPosition(pos);
		health = 0;
		damage = 0;
		speed = 0;
	}

	void setHealth(int health)
	{
		this->health = health;
	}
	void setDamage(int damage)
	{
		this->damage = damage;
	}
	void setSpeed(int speed)
	{
		this->speed = speed;
	}
	void setTexture(sf::Texture& texture)
	{
		m_texture = texture;
		((sf::Sprite*)this)->setTexture(m_texture);
	}

	// width should be 22
	// height should be 16
	// waypoints last element stores TILE_END or invalid tile / deadend
	bool setWayPoints(int* tiles)
	{
		waypoints.clear();

		// starting tile location in tiles array
		sf::Vector2i start(-1, -1);

		// find starting location in tiles array
		bool flag = false;
		for (int i = 0; i < MAX_LEVEL_HEIGHT; i++)
		{
			for (int j = 0; j < MAX_LEVEL_WIDTH; j++)
			{
				if (tiles[MAX_LEVEL_WIDTH * i + j] == TILE_START)
				{
					flag = true;
					start.x = j;
					start.y = i;
					break;
				}
			}
			if (flag) break;
		}

		// check if did not find a starting point
		if (!flag)
		{
			std::cout << "Did not find a starting point for enemy" << std::endl;
			return false;
		}
		else
		{
			// add starting location to waypoints
			waypoints.push_back(sf::Vector2f(start.x * 64, start.y * 64 - 32));
		}

		// now search for end //
		int i = start.y, j = start.x;
		bool success = true;
		// - find first tile
		int previous = -1;
		if (j + 1 < MAX_LEVEL_WIDTH)
		{
			int tileNumber = tiles[i * MAX_LEVEL_WIDTH + j];
			if (tileNumber == TILE_HORIZONTAL || tileNumber == TILE_BOTTOMR_CORNER
				|| tileNumber == TILE_TOPR_CORNER)
			{
				j++;
			}
			else
			{
				std::cout << "No connecting tile to green square" << std::endl;
				return false;
			}
		}
		else if (j - 1 >= 0)
		{
			int tileNumber = tiles[i * MAX_LEVEL_WIDTH + j];
			if (tileNumber == TILE_HORIZONTAL || tileNumber == TILE_BOTTOML_CORNER
				|| tileNumber == TILE_TOPL_CORNER)
			{
				j--;
			}
			else
			{
				std::cout << "No connecting tile to green square" << std::endl;
				return false;
			}
		}
		else if (i + 1 < MAX_LEVEL_HEIGHT)
		{
			int tileNumber = tiles[i * MAX_LEVEL_WIDTH + j];
			if (tileNumber == TILE_VERTICAL || tileNumber == TILE_BOTTOMR_CORNER
				|| tileNumber == TILE_BOTTOML_CORNER)
			{
				i++;
			}
			else
			{
				std::cout << "No connecting tile to green square" << std::endl;
				return false;
			}
		}
		else
		{
			int tileNumber = tiles[i * MAX_LEVEL_WIDTH + j];
			if (tileNumber == TILE_VERTICAL || tileNumber == TILE_TOPR_CORNER
				|| tileNumber == TILE_TOPL_CORNER)
			{
				i--;
			}
			else
			{
				std::cout << "No connecting tile to green square" << std::endl;
				return false;
			}
		}

		while (tiles[i * MAX_LEVEL_WIDTH + j] != TILE_END)
		{
			if (i < 0 || j < 0 || i > MAX_LEVEL_HEIGHT || j > MAX_LEVEL_WIDTH)
			{
				success = false;
				break;
			}
			else
			{
				waypoints.push_back(sf::Vector2f(j * 64, i * 64 - 32));
			}

			switch (tiles[i * MAX_LEVEL_WIDTH + j])
			{
			case TILE_HORIZONTAL:
				if (previous == LEFT)
				{
					j--;
					previous = LEFT;
				}
				else if(previous == RIGHT)
				{
					j++;
					previous = RIGHT;
				}
				else
				{
					// came from illegal direction
					success = false;
				}
				break;
			case TILE_VERTICAL:
				if (previous == UP)
				{
					i--;
					previous = UP;
				}
				else if (previous == DOWN)
				{
					i++;
					previous = DOWN;
				}
				else
				{
					success = false;
				}
				break;
			case TILE_BOTTOML_CORNER:
				if (previous == DOWN)
				{
					j++;
					previous = RIGHT;
				}
				else if (previous == LEFT)
				{
					i--;
					previous = UP;
				}
				else
				{
					success = false;
				}
				break;
			case TILE_BOTTOMR_CORNER:
				if (previous == DOWN)
				{
					j--;
					previous = LEFT;
				}
				else if (previous == RIGHT)
				{
					i--;
					previous = UP;
				}
				else
				{
					success = false;
				}
				break;
			case TILE_TOPL_CORNER:
				if (previous == UP)
				{
					j++;
					previous = RIGHT;
				}
				else if (previous == LEFT)
				{
					i++;
					previous = DOWN;
				}
				else
				{
					success = false;
				}
				break;
			case TILE_TOPR_CORNER:
				if (previous == UP)
				{
					j--;
					previous = LEFT;
				}
				else if (previous == RIGHT)
				{
					i++;
					previous = DOWN;
				}
				break;
			case TILE_END:
				break;
			default:
				success = false;
				break;
			}
			if (!success)
			{
				std::cout << "Failed pathfinding" << std::endl;
				return false;
			}
		}
	}

	int getHealth()
	{
		return health;
	}
	int getDamate()
	{
		return damage;
	}
	int getSpeed()
	{
		return speed;
	}

	void move()
	{
		setPosition(waypoints.front());
		waypoints.pop_front();
	}

private:
	int health;
	int damage;
	int speed;
	sf::Texture m_texture;
	std::deque<sf::Vector2f> waypoints; // final element is either TILE_END or invalid tile / deadend
};