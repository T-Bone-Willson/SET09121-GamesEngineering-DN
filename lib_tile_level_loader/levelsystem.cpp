#include "LevelSystem.h"
#include <fstream>
#include <iostream>

using namespace std;
using namespace sf;

std::unique_ptr < LevelSystem::TILE[]> LevelSystem::_tiles;
size_t LevelSystem::_width;
size_t LevelSystem::_height;
//Vector2f LevelSystem::_offset(0.0f, 30.0f);

float LevelSystem::_tileSize(100.f);
vector<std::unique_ptr<sf::RectangleShape>> LevelSystem::_sprites;
Vector2f LevelSystem::_offset(0.0f, 30.0f);

size_t LevelSystem::getHeight() {
	return _height;
}

size_t LevelSystem::getWidth() {
	return _width;
}

std::map<LevelSystem::TILE, sf::Color> LevelSystem::_colours{ // THIS MAY BE "_colours"!!!!!!!!!!!!!!!
	{WALL, Color::White}, {END, Color::Red}
};

sf::Color LevelSystem::getColor(LevelSystem::TILE t) {
	auto it = _colours.find(t);
	if (it == _colours.end()) {
		_colours[t] = Color::Transparent;
	}
	return _colours[t];
}

void LevelSystem::setColor(LevelSystem::TILE t, sf::Color c) {
	LevelSystem::_colours[t] = c;
}

void LevelSystem::LevelSystem::loadLevelFile(const std::string& path, float tileSize) {
	_tileSize = tileSize;
	size_t w = 0, h = 0;
	string buffer;

	// Load in file to buffer
	ifstream f(path);
	if (f.good()) {
		f.seekg(0, std::ios::end);
		buffer.resize(f.tellg());
		f.seekg(0);
		f.read(&buffer[0], buffer.size());
		f.close();
	}
	else {
		throw string("Couldn't open level file: ") + path;
	}

	std::vector<TILE> temp_tiles;
	for (int i = 0; i < buffer.size(); ++i) {
		const char c = buffer[i];
		switch (c) {
		case 'w':
			temp_tiles.push_back(WALL);
			break;
		case 's':
			temp_tiles.push_back(START);
			break;
		case 'e':
			temp_tiles.push_back(END);
			break;
		case ' ':
			temp_tiles.push_back(EMPTY);
			break;
		case '+':
			temp_tiles.push_back(WAYPOINT);
			break;
		case 'n':
			temp_tiles.push_back(ENEMY);
			break;
		case '\n':			// End of line
			if (w == 0) {	// If we havn't written width yet
				w = i;		// Set width
			}
			h++;			// Increment height
			break;
		default:
			cout << c << endl; // Don't know what this tile type is
		}
	}

	h++; // THIS MAY WORK!

	if (temp_tiles.size() != (w * h)) {
		throw string("Can't parse level file. SORRY!!!") + path; // Error message for user
	}
	// Makes a 1D Array for tile information
	_tiles = std::make_unique<TILE[]>(w * h);
	_width = w; // set static class vars
	_height = h;
	std::copy(temp_tiles.begin(), temp_tiles.end(), &_tiles[0]);
	cout << "Level " << path << "Loaded! " << w << "x" << h << std::endl;
	buildSprites();
}

void LevelSystem::buildSprites() {
	_sprites.clear();
	for (size_t y = 0; y < LevelSystem::getHeight(); ++y) {
		for (size_t x = 0; x < LevelSystem::getWidth(); ++x) {
			auto s = make_unique<sf::RectangleShape>();
			s->setPosition(getTilePosition({ x, y }));
			s->setSize(Vector2f(_tileSize, _tileSize));
			s->setFillColor(getColor(getTile({ x, y })));
			_sprites.push_back(move(s));
		}
	}
}

sf::Vector2f LevelSystem::getTilePosition(sf::Vector2ul p) {
	return (Vector2f(p.x, p.y) * _tileSize);
}

LevelSystem::TILE LevelSystem::getTile(sf::Vector2ul p) {
	if (p.x > _width || p.y > _height) {
		throw string("Wooahh! Tile out of range Brah: ") + to_string(p.x) + "," + to_string(p.y) + ")";
	}
	return _tiles[(p.y * _width) + p.x]; // read p56 and fully understand this process!
}

LevelSystem::TILE LevelSystem::getTileAt(Vector2f v) {
	auto a = v - _offset;
	if (a.x < 0 || a.y < 0) {
		throw string("Tile out of range ");
	}
	return getTile(Vector2ul((v - _offset) / (_tileSize)));
}
// DOING A TEST TO SEE IF I DON't NEED THE CODE BELOW!!!

/*std::vector<sf::Vector2ul> LevelSystem::findTiles(LevelSystem::TILE tile) {
	std::vector<sf::Vector2ul> ret;
	for (int i = 0; i < _width * _height; i++) {
		if (_tiles[i] == tile) {
			ret.push_back(Vector2ul(i % _width, i / _width));
		}
	}
	if (ret.size() == 0) {
		throw string("No teils found");
	}
	return ret;
}*/

// Test Code Below!!!

std::vector<sf::Vector2ul> LevelSystem::findTiles(LevelSystem::TILE tile) {
	std::vector<sf::Vector2ul> ret;
	for (int i = 0; i < _width * _height; i++) {
		if (_tiles[i] == tile) {
			ret.push_back(Vector2ul(i % _width, i / _width));
		}

	}
	if (ret.size() == 0) {
		throw string("YO DAWG, I HEARD YOU LIKE TILES? BUT WE GOT NONE!!! ");
	}
	return ret;
}

void LevelSystem::render(RenderWindow &window) {
	for (size_t i = 0; i < _width * _height; ++i) {
		window.draw(*_sprites[i]);
	}
}

