#include <view/sfml_application.h>
#include <iostream>
#include <vector>
#include <memory>
#include <chrono>



enum State
{
	LOCKED, UNLOCKED, VIOLATION,
};

class Turnstile {
public:
	void lock() { std::cout << "Locking...\n"; }
	void unlock() { std::cout << "Unlocking...\n"; }
	void alarm() { std::cout << "Alarm!\n"; }
	void thankYou() { std::cout << "Thank you!\n"; }
	State currentState = LOCKED;

};

enum Event {
	COIN, PASS, RESET, READY
};

void transition(Turnstile& obj, Event ev) {
	switch (obj.currentState)
	{
	case LOCKED:
		switch (ev) {
		case COIN:
			obj.currentState = UNLOCKED;
			obj.unlock();
			break;
		case PASS:
			obj.alarm();
			obj.currentState = VIOLATION;
			break;
		case RESET:
			break;
		case READY:
			break;
		default:
			break;
		}
		break;

	case UNLOCKED:
		switch (ev) {
		case COIN:
			obj.thankYou();
			break;
		case PASS:
			obj.lock();
			obj.currentState = LOCKED;
			break;
		case RESET:
			break;
		case READY:
			break;
		default:
			break;
		}
		break;

	case VIOLATION:
		switch (ev)
		{
		case COIN:
			obj.currentState = VIOLATION;
			break;
		case PASS:
			obj.currentState = VIOLATION;
			break;
		case RESET:
			obj.alarm();
			obj.currentState = LOCKED;
			break;
		case READY:
			obj.currentState = LOCKED;
			break;
		default:
			break;
		}
		break;

	}
}
bool PassInput = 0;
bool Passdow = false;
bool CoinInput = 0;
bool CoinDow = false;
bool ResetInput = 0;
bool readyInput = 0;

int main() {
	std::vector<std::shared_ptr<sf::Texture>> textures = sfml_application::loadTextures({
		"assets/Yellow.png",
		"assets/Green.png",
		"assets/Red.png",
		});

	// Verifica si las texturas se cargaron correctamente
	for (size_t i = 0; i < textures.size(); ++i) {
		if (textures[i]) {
			std::cout << "Textura " << i << " cargada correctamente.\n";
		}
		else {
			std::cerr << "Error cargando textura " << i << ".\n";
		}
	}

	Turnstile turnstileObject;

	sfml_application::runGame("Turnstile FSM", [&turnstileObject](float deltaTime)
	{
		// Update: Read input and send event to STM
		PassInput = sf::Keyboard::isKeyPressed(sf::Keyboard::Num2);
		CoinInput = sf::Keyboard::isKeyPressed(sf::Keyboard::Num1);
		ResetInput = sf::Keyboard::isKeyPressed(sf::Keyboard::Num3);
		readyInput = sf::Keyboard::isKeyPressed(sf::Keyboard::Num4);
		if (!PassInput) {
			Passdow = false;
		}
		if (!CoinInput) {
			CoinDow = false;
		}
		// esimerkki pass, eventin lhettmisest:
		if(PassInput && !Passdow){
			transition(turnstileObject, PASS);
			Passdow = true;
		} 
		else if(CoinInput && !CoinDow){
			transition(turnstileObject, COIN);
			CoinDow = true;
		}
		else if (ResetInput){
			transition(turnstileObject, RESET);
		}
		else if(readyInput){
			transition(turnstileObject, READY);
		}
		
		return 0;
	},
		[&textures, &turnstileObject](auto& window) {
			// Render sprite according to current state of turnstileObject:
			sfml_application::render(window, sf::Vector2f(11.5f, 8.2f), 0.0f, 
				*textures[static_cast<int>(turnstileObject.currentState)]);

			return window.isOpen();
		});

	return 0;
}


