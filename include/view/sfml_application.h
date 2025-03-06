#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>

namespace sfml_application {

///
/// \brief LoadTextures from filenames.
/// \param fileNames	= List of filenames to load.
///
inline auto loadTextures(const std::vector<std::string>& fileNames) {
	std::vector<std::shared_ptr<sf::Texture> > loadedTextures;
	for(const auto& fileName : fileNames ) {
		auto tex = std::make_shared<sf::Texture>();
		tex->loadFromFile(fileName);
		loadedTextures.push_back(tex);
	}
	return loadedTextures;
}


///
/// \brief Renders a sprite.
/// \param window
/// \param position
/// \param rotation
/// \param texture
///
void render(auto& window, auto position, auto rotationInRadians, auto texture) {
	static const auto OFFSET = sf::Vector2f(0.5f,0.5f);
	static const float SCALE = 32.0f;
	sf::Vector2f scale = sf::Vector2f(SCALE/float(texture.getSize().x), SCALE/float(texture.getSize().y));
	sf::Sprite sprite;
	sprite.setOrigin(0.5f*SCALE, 0.5f*SCALE);
	sprite.setPosition(SCALE*(position+OFFSET));
	sprite.setTexture(texture);
	sprite.setScale(scale);
	sprite.setRotation(rotationInRadians * 180.0f / 3.1415f); // Rad to deg.
	window.draw(sprite);

}


///
/// \brief Renders 1D map
/// \param window
/// \param map
/// \param textures
///
void render(sf::RenderWindow& window, const std::vector<int>& map, const auto& textures) {
	for(size_t x = 0; x<map.size(); ++x) {
		auto val = map[x];
		assert(val >= 0 && val < textures.size()); // Mapissa laiton numero...
		render(window, sf::Vector2f(x,0), 0.0f, *textures[val]);
	}
}


///
/// \brief Renders 2D map.
/// \param window
/// \param map
/// \param textures
///
void render(sf::RenderWindow& window, const std::vector<std::vector<int> >& map, const auto& textures) {
	for(size_t y = 0; y<map.size(); ++y) {
		for(size_t x = 0; x<map[y].size(); ++x) {
			auto val = map[y][x];
			assert(val >= 0 && val < textures.size()); // Mapissa laiton numero...
			render(window, sf::Vector2f(x,y), 0.0f, *textures[val]);
		}
	}
}


///
/// \brief runGame funktio Avaa peli ikkunan ja suorittaa pelin loppuun.
/// \param windowTitle	= Ikkunan otsikkoteksti (pelin nimi)
/// \param update		= Pelin päivitysfunktio
/// \param render		= Pelimaailman renderöintifunktio
/// \return
///
int runGame(std::string windowTitle, auto update, auto render) {
	sf::RenderWindow window({800,600},windowTitle.c_str());
	bool gameRunning = true;
	while (window.isOpen() && gameRunning) {
		// check all the window's events that were triggered since the last iteration of the loop
		sf::Event event;
		sf::Clock frameTimer;
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
			window.close();
		}

		float deltaTime = frameTimer.getElapsedTime().asSeconds();
		frameTimer.restart();
		const auto& gameState = update(deltaTime);
		// clear the window with black color
		window.clear(sf::Color::Cyan);
		gameRunning = render(window);
		window.display();
	}
	return 0;
}

}
