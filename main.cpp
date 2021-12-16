#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <time.h>
#include <math.h>

const int winSize_x = 400;
const int winSize_y = 300;
const float gravity = 100.0;

float timeScale = 1.0;
bool updateCheck = true;
;

void fall(bool world[winSize_y][winSize_x], float deltaTime);
typedef struct particle
{
	short id;
	float life_time;
	sf::Vector2f velocity;
	sf::Color color;
	bool hasUpdated;
	short density;
} particle;
void inicialize(int i, int j, short id);
void updateSand(int x, int y);
void updateSmoke(int i, int j);
void updateWater(int i, int j);
void update(int i, int j);
void draw(int i, int j);

particle world[winSize_y][winSize_x] = {};
sf::RenderWindow window(sf::VideoMode(winSize_x, winSize_y), "Another Physics Simulator");
bool randomBool()
{
	return rand() > (RAND_MAX / 2);
}
float deltaTime;
short c_id = 1;
int main()
{
	sf::Clock dt;
	bool dragging = false;

	float deltatime = 0.f;
	sf::Clock clock;

	while (window.isOpen())
	{
		double frameTime = dt.restart().asSeconds();
		deltaTime = (frameTime < 1 / 60.0) ? frameTime : 1 / 60.0;
		sf::Event event;

		sf::Vector2i point;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;

			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					dragging = true;
				}
				if (event.mouseButton.button == sf::Mouse::Right)
				{
					sf::Vector2u s = window.getSize();
					sf::Vector2i p = sf::Mouse::getPosition(window);
					p.x = round(p.x * (((float)winSize_x) / s.x));
					p.y = round(p.y * (((float)winSize_y) / s.y));
					std::cout << p.y << " " << p.x << " id: " << world[p.y][p.x].id;
				}
				break;

			case sf::Event::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					dragging = false;
				}
				break;
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::D)
					c_id = 0;
				else if (event.key.code == sf::Keyboard::A)
					c_id = 1;
				else if (event.key.code == sf::Keyboard::W)
					c_id = 2;
				else if (event.key.code == sf::Keyboard::S)
					c_id = 3;
			}
		}
		if (dragging)
		{
			sf::Vector2u s = window.getSize();
			sf::Vector2i point = sf::Mouse::getPosition(window);
			point.x = round(point.x * (((float)winSize_x) / s.x));
			point.y = round(point.y * (((float)winSize_y) / s.y));
			inicialize(point.y, point.x, c_id);
			inicialize(point.y + 1, point.x, c_id);
			inicialize(point.y - 1, point.x, c_id);
			inicialize(point.y, point.x + 1, c_id);
			inicialize(point.y, point.x - 1, c_id);
			inicialize(point.y - 1, point.x + 1, c_id);
			inicialize(point.y - 1, point.x - 1, c_id);
			inicialize(point.y + 1, point.x + 1, c_id);
			inicialize(point.y + 1, point.x - 1, c_id);
		}
		if (clock.getElapsedTime().asSeconds() >= 0)
		{

			for (int i = 0; i < winSize_y; i++)
			{
				for (int j = 0; j < winSize_x; j++)
				{
					if (world[i][j].id != 0 && world[i][j].hasUpdated != updateCheck)
					{
						update(i, j);
					}
					draw(i, j);
				}
			}
			window.display();
			window.clear();
			clock.restart();
			updateCheck = !updateCheck;
		}
	}

	return 0;
}

void inicialize(int i, int j, short id)
{
	particle *k = &world[i][j];
	switch (id)
	{
	case 1:
		k->id = 1;
		k->color = sf::Color::Yellow;
		k->density = 3;
		break;
	case 2:
		k->id = 2;
		k->color = sf::Color::Blue;
		k->density = 2;
		break;
	case 3:
		world[i][j].id = 3;
		k->color.a = 255;
		k->color.r = 180;
		k->color.g = 180;
		k->color.b = 180;
		k->life_time = 3*(randomBool()?2:1);
		k->density = 1;
		break;
	default:
		k->id = 0;
		k->density = 0;
		break;
	}
}

bool stuck(int i, int j)
{
	particle x = world[i][j];
	if (i + 1 < winSize_y && world[i + 1][j].density < x.density)
	{
		return false;
	}
	else if (i + 1 < winSize_y && j + 1 < winSize_x && world[i + 1][j + 1].density < x.density)
	{
		return false;
	}
	else if (i + 1 < winSize_y && j - 1 >= 0 && world[i + 1][j - 1].density < x.density)
	{
		return false;
	}
	if (x.id == 2 || x.id == 3)
	{
		if (j + 1 < winSize_x && world[i][j + 1].density < x.density)
		{
			return false;
		}
		else if (j - 1 >= 0 && world[i][j - 1].density < x.density)
		{
			return false;
		}
	}
	if (x.id == 3 && i - 1 >= 0)
	{
		if (world[i - 1][j].density <= x.density)
		{
			return false;
		}
		else if (j + 1 < winSize_x && world[i - 1][j + 1].density <= x.density)
		{
			return false;
		}
		else if (j - 1 >= 0 && world[i - 1][j - 1].density <= x.density)
		{
			return false;
		}
	}
	return true;
}

void update(int i, int j)
{
	world[i][j].hasUpdated = updateCheck;

	world[i][j].life_time -= deltaTime;
	if (stuck(i, j))
	{
		world[i][j].velocity.x = 0;
		world[i][j].velocity.y = 0;
		return;
	}
	switch (world[i][j].id)
	{
	case 0:
		break;
	case 1:
		updateSand(i, j);
		break;
	case 2:
		updateWater(i, j);
		break;
	case 3:
		updateSmoke(i, j);
		break;
	}
}

void updateSand(int i, int j)
{



	int maxD = world[i][j].velocity.y * gravity;
	world[i][j].velocity.y += gravity * deltaTime;

	for (i; maxD - i > 0; i++)
	{
		if (i + 1 < winSize_y)
		{
			int k = randomBool() ? 1 : -1;
			particle down = world[i + 1][j];

			if (down.density < world[i][j].density)
			{
				world[i + 1][j] = world[i][j];
				world[i][j] = down;
			}
			else if (j + k < winSize_x && j + k >= 0 && world[i + 1][j + k].density < world[i][j].density)
			{
				particle t = world[i + 1][j + k];
				world[i + 1][j + k] = world[i][j];
				world[i][j] = t;
			}
			else
			{
				break;
			}
		}
	}
}

void updateSmoke(int i, int j)
{
	if (world[i][j].life_time <= 0)
	{
		world[i][j].id = 0;
		return;
	}

	int h = randomBool() ? 1 : -1;
	h += (randomBool() && h > -1) ? -1 : 0;
	int k = randomBool() ? 1 : -1;
	if (i + h < winSize_y && i + h >= 0)
	{
		if (world[i + h][j].id == 0)
		{
			if (j + k < winSize_x && j + k >= 0 && world[i + h][j + k].id == 0)
			{
				world[i + h][j + k] = world[i][j];
			}
			else
				world[i + h][j] = world[i][j];
			world[i][j].id = 0;
		}
	}

}

void updateWater(int i, int j)
{
	particle *x = &world[i][j];
	int h = 1;
	if (i + h >= winSize_y)
	{
		h = 0;
	}
	int k = randomBool() ? 1 : -1;
	if (world[i + h][j].id == 0)
	{
		world[i + h][j] = world[i][j];
		inicialize(i, j, 0);
	}
	else if (j + k < winSize_x && j + k >= 0)
	{
		if (world[i + h][j + k].id == 0)
		{
			world[i + h][j + k] = world[i][j];
			inicialize(i, j, 0);
		}
		else if (world[i][j + k].id == 0)
		{
			world[i][j + k] = world[i][j];
			inicialize(i, j, 0);
		}
	}
}

void draw(int i, int j)
{
	if (world[i][j].id != 0)
	{
		sf::RectangleShape s = sf::RectangleShape(sf::Vector2f(1, 1));
		s.setFillColor(world[i][j].color);
		s.setPosition(sf::Vector2f(j, i));
		window.draw(s);
	}
}
