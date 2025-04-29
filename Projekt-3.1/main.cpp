#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <windows.h>

#define M_PI 3.141592

using namespace std;
using namespace sf;

void run(); //Uruchomienie i wyœwitelanie okna aplikacji
void processEvents(); //Przechwytywanie zdarzeñ w aplikacji
void update(Time deltaTime); //Aktualizacja ka¿dego elementu na scenie
void handleInput(Keyboard::Key Key, bool isPressed); //Obs³uga zdarzeñ klawiatury i myszy
void render(); //Wyœwietlanie ka¿dego elementu na scenie
void checkBallBallCollision(); //Sprawdzanie kolizji kuli z inn¹ kul¹
void removeBallCollision(); //Próba usuniêcia kolizji poprzez algorytm przesuwania kul
Color randColor(); //Losowanie randomowego koloru dla kul
void moveIt(CircleShape* shape, Vector2f move); //Dodatkowe warunki do porusza kul po scenie
void checkClick(bool isPressed); //Sprawdzanie elementu który zosta³ naciœniêty
void checkMouseBallCollision(); //Sprawdzanie czy zachodzi kolizja kursora i kuli
void dragElement(); //Przenoszenie elementu przy pomocy przeci¹gania myszy
void drawData(); //Wypisanie wszystkich atrybutów elementów na ekranie
void eraseData(); //Czyszczenie konsoli
void gravity(Time deltaTime); //Dzia³anie grawitacji

RenderWindow window; //G³ówny obiekt aplikacji
int screenWidth = 800, screenHeight = 500; //Szerokoœæ i wysokoœæ okna
Texture boardTexture; //Textura do planszy na ekranie
Sprite boardSprite; //Dusza textury, któr¹ mo¿na wyœwietlaæ na ekranie
vector<CircleShape*> objects; //Tablica przychowuj¹ca dane do wyœwietlania kul na ekranie
vector<bool> collisions; //Flagi kolizji dla ka¿dej kuli
vector<Text> texts; //Tablica przechowuj¹ca teksty wyœwietlane na ekranie
vector<Vector2f> velocity; //Tablica przechowuj¹ca prêdkoœæ kuli
vector<Vector2f> force; //Tablica przechowuj¹ca si³ê kuli
vector<Vector2f> position; //Tablica przechowuj¹ca tymczasow¹ pozycjê kuli
float multiplier = 2; //Mno¿nik do wyœwietlania elementów, aby by³a lepsza widocznoœæ
float radius = 10; //Promieñ ka¿dej z kul
float amount = 80; //Iloœæ kul na scenie
Vector2f mapPosition = Vector2f(349, 49); //Startowa pozycja elementów na planszy
float speed = 2; //Szybkoœæ poruszania siê obiektów podczas przesuwania
int checkComponent; //Id element wybrany do sprawdzania poprzez naciœniêcie myszy
float dragX = 0, dragY = 0; //Offset do przesuwania kul za pomoc¹ myszy
bool drag = false, isDragging = false; //Flaga do przesuwania elementów za pomoc¹ myszy
bool gravityOn = false; //Flaga do w³¹czania grawitacji

int main()
{
	//pobranie grafiki planszy z folderu data oraz ustawienie jej pozycji na ekranie
	if (!boardTexture.loadFromFile("data/background-map.png")) {
		cout << "Nie uda³o sie znale¿æ pliku: data/background-map.png" << endl;
	}
	boardSprite.setTexture(boardTexture);
	boardSprite.setPosition(mapPosition.x - 4, mapPosition.y - 4);
	
	//generowanie po³o¿enia ka¿dej kuli
	srand(time(NULL));
	for (int i = 0; i < amount; i++) {

		float x = ((rand() % 201) * 2) - 1; // zakres od 0 do 201, w zamyœle (-100) do (100) razy 2
		float y = ((rand() % 201) * 2) - 1; // i odejmujemy korektê wynikaj¹ca z mno¿enia
		CircleShape* circle = new CircleShape;
		circle->setPosition(Vector2f(x + mapPosition.x, mapPosition.y + (201 * 2) - y));
		position.push_back(Vector2f(x + mapPosition.x, mapPosition.y + (201 * 2) - y));
		circle->setRadius(radius * multiplier);
		circle->setOrigin(Vector2f(radius * multiplier, radius * multiplier));
		circle->setFillColor(randColor());
		objects.push_back(circle);
	}
	for (int i = 0; i < amount; i++) {
		collisions.push_back(false);
		velocity.push_back(Vector2f(0,0)); //wyzerowanie prêdkoœci
		force.push_back(Vector2f(0,0)); //wyzerowanie si³y
	}

	drawData(); //Rysowanie danych
	
	//Pobranie czcionki
	Font font;
	if (!font.loadFromFile("data/Roboto-Medium.ttf")) {
		cout << "Nie mo¿na za³adowaæ czcionki: data/Roboto-Medium.ttf" << endl;
	}

	//Zamiana typu int na string
	ostringstream n1;
	n1 << amount;
	ostringstream n2;
	n2 << radius;
	ostringstream n3;
	n3 << speed;
	//Tablica napisów potrzebnych do wyœwietlania na ekranie
	vector<string> t = { "Esc - Zamknij","P - Przesuwanie", "G - Grawitacja", "W - Dane w konsoli", "E - Czyszczenie konsoli","",
		n1.str() + " - Liczba elementów", n2.str() + " - Promien", n3.str()+" - Szybkosc"};

	for (int i = 0; i < t.size(); i++) {
		Text txt;
		txt.setCharacterSize(20);
		txt.setFillColor(Color::White);
		txt.setFont(font);
		txt.setString(t.at(i));
		txt.setPosition(40, 100 + (i * 25));
		texts.push_back(txt);
	}

    run(); // Uruchomienie g³ównej pêtli

    return 0;
}

Color randColor() {
	int colorValue = rand() % 8;
	switch (colorValue) { //losowanie kolorów dla efektu wizualnego
	case 0:
		return Color::Cyan;
		break;
	case 1:
		return Color::White;
		break;
	case 2:
		return Color::Yellow;
		break;
	case 3:
		return Color::Green;
		break;
	case 4:
		return Color::Magenta;
		break;
	case 5:
		return Color::Blue;
		break;
	case 6:
		return Color::Color(255, 100, 0);
		break;
	case 7:
		return Color::Color(140, 0, 255);
		break;
	}
}

void run() {
	//Stworzenie okna aplikacji
    window.create(VideoMode(screenWidth, screenHeight), "Projekt 3.1", Style::Close);
    window.setFramerateLimit(60);
    window.setActive();

	//G³ówna pêtla aplikacji oraz dzia³anie czasu deltaTime
	Clock clock;
	Time timeSinceLastUpdate = Time::Zero;
	while (window.isOpen()) {
		processEvents();
		timeSinceLastUpdate += clock.restart();
		while (timeSinceLastUpdate > seconds(1.f / 60)) {
			timeSinceLastUpdate -= seconds(1.f / 60);
			processEvents();
			update(seconds(1.f / 60));
		}
		render();
	}
}

void processEvents() {
	Event event;
	if (window.pollEvent(event)) {
		switch (event.type) {
		case Event::KeyPressed:
			handleInput(event.key.code, true);
			break;
		case Event::KeyReleased:
			handleInput(event.key.code, false);
			break;
		case Event::MouseButtonPressed:
			handleInput(event.key.code, true);
			isDragging = true;
			break;
		case Event::MouseButtonReleased:
			handleInput(event.key.code, false);
			isDragging = false;
			break;
		case Event::Closed: //zamkniêcie programu "X"
			window.close();
			break;
		}
	}
	if (event.type == Event::MouseMoved) { //Do obs³ugi poruszania elementem przy pomocy myszy
		if (isDragging) {
			dragElement();
		}
	}
}

void update(Time deltaTime) {
	if(drag == false) //Sprawdzanie kolizji myszy z kul¹ tylko gdy nie przeci¹gmy ¿adnej
		checkMouseBallCollision();
	if (gravityOn) // Gdy grawitacja jest w³¹czona - uruchamiamy funkcjê odpowiedzialn¹ za ni¹
		gravity(deltaTime);
	else
		checkBallBallCollision();// Gdy grawitacja jest wy³¹czona - uruchamiamy funkcjê odpowiedzialn¹ za sprawdzanie kolizji kul
}

void render() {
	window.clear(Color::Color(24,24,24));

	window.draw(boardSprite);// Rysowanie planszy

	for (CircleShape *circle : objects) { //Rysowanie obiektów
		window.draw(*circle);
	}
	for (Text& text : texts) { //Rysowanie tekstu
		window.draw(text);
	}

	window.display();
}

void handleInput(Keyboard::Key key, bool isPressed) {
	Vector2i mousePosition = Mouse::getPosition(window);

	if (key == Keyboard::Escape && isPressed == true)
		window.close();
	if (key == Keyboard::P && isPressed == true)
		removeBallCollision();
	if (key == Keyboard::G && isPressed == false) {
		gravityOn = !gravityOn; //Przestawienie grawitacji
		for (int i = 0; i < amount; i++) {
			velocity.at(i) = Vector2f(0, 0); //Zerowanie Prêdkoœci
			force.at(i) = Vector2f(0, 0); // Zerowanie si³y
		}
	}
	if (key == Keyboard::E && isPressed == true)
		eraseData();
	if (key == Keyboard::W && isPressed == true)
		drawData();
	if (key == Mouse::Left) {
		checkClick(isPressed);
	}
}

void checkBallBallCollision() {
	for (int i = 0; i < amount; i++) {
		CircleShape* circle1 = objects.at(i);
		for (int j = i + 1; j < amount; j++) {
			CircleShape* circle2 = objects.at(j);
			// Odleg³oœæ kul od siebie
			float length = sqrt(pow(circle1->getPosition().x - circle2->getPosition().x, 2) + 
				pow(circle1->getPosition().y - circle2->getPosition().y, 2));
			// Sprawdzanie kolizji miêdzy nimi i kolorowanie ich na czerwono
			if (length + 1 < (circle1->getRadius() + circle2->getRadius())) {
				collisions.at(i) = true;
				collisions.at(j) = true;
				circle1->setFillColor(Color::Red);
				circle2->setFillColor(Color::Red);
			}
		}
	}
	//Ponowne generowanie koloru dla kul które by³y w kolizji z innymi
	for (int i = 0; i < amount; i++) {
		CircleShape* circle = objects.at(i);
		if (collisions.at(i) == false && circle->getFillColor() == Color::Red) {
			circle->setFillColor(randColor());
		}
		collisions.at(i) = false;
	}
}

void removeBallCollision() {
	for (int i = 0; i < amount; i++) {
		CircleShape* circle1 = objects.at(i);
		for (int j = i + 1; j < amount; j++) {
			CircleShape* circle2 = objects.at(j);
			float length = sqrt(pow(circle1->getPosition().x - circle2->getPosition().x, 2) +
				pow(circle1->getPosition().y - circle2->getPosition().y, 2));
			if (length + 1 < (circle1->getRadius() + circle2->getRadius())) { // Jeœli zachodzi miêdzy nimi kolizja
				//K¹t miêdzy kul¹ a lini¹ ³¹czenia ( kolizyjn¹ )
				float alfa = sin(abs(circle1->getPosition().x - circle2->getPosition().x) / length) + (M_PI/2);

				// Odpowiednie ustawienie odrotnego k¹ta zale¿nie od po³o¿enia kul
				if (circle1->getPosition().x > circle2->getPosition().x) {
					if (circle1->getPosition().y > circle2->getPosition().y) 
						alfa = (M_PI / 2 + alfa);
				}
				else {
					if (circle1->getPosition().y < circle2->getPosition().y)
						alfa = -alfa;
					else
						alfa = M_PI + (M_PI / 2 - alfa);
				}

				float beta = M_PI + alfa; // k¹t odwrotny dla drugiej kuli

				//Nadanie rotacji dla kul
				circle1->setRotation(alfa * (180 / M_PI) - 90);
				circle2->setRotation(beta * (180 / M_PI) - 90);

				int mx1 = 1, my1 = 1, mx2 = 1, my2 = 1; //mno¿niki automatycznego poruszania siê kul na ekranie

				if (((circle1->getPosition().x - mapPosition.x + 1) / 2 - 100) >= 100) {
					my2 = -1; my1 = 0; mx1 = -1;
				}
				if (((circle1->getPosition().x - mapPosition.x + 1) / 2 - 100) <= -100) {
					my2 = -1; my1 = 0; mx1 = 1;
				}
				if (((circle2->getPosition().x - mapPosition.x + 1) / 2 - 100) >= 100) {
					my1 = -1; my2 = 0; mx2 = -1;
				}
				if (((circle2->getPosition().x - mapPosition.x + 1) / 2 - 100) <= -100) {
					my1 = -1; my2 = 0; mx2 = 1;
				}
				if ((100 - (circle1->getPosition().y - mapPosition.y - 1) / 2) >= 100) {
					mx2 = -1; mx1 = 0; my1 = 1;
				}
				if ((100 - (circle1->getPosition().y - mapPosition.y - 1) / 2) <= -100 ) {
					mx2 = -1; mx1 = 0; my1 = -1;
				} 
				if ((100 - (circle2->getPosition().y - mapPosition.y - 1) / 2) <= -100) {
					mx1 = -1; mx2 = 0; my2 = -1;
				}
				if ((100 - (circle2->getPosition().y - mapPosition.y - 1) / 2) >= 100) {
					mx1 = -1; mx2 = 0; my2 = 1;
				}
				// Zastosowanie poruszania siê kul wed³ug mno¿ników i odpowiedniej prêdkoœci
				moveIt(circle1,Vector2f(cos(alfa) * mx1, sin(alfa) * my1) * speed);
				moveIt(circle2, Vector2f(cos(beta) * mx2, sin(beta) * my2) * speed);
			}
		}
	}
}

void moveIt(CircleShape* shape, Vector2f move) {
	if (((shape->getPosition().x - mapPosition.x + 1) / 2 - 100) >= 100) {
		if (move.x > 0) move.x = 0;
	}
	else if (((shape->getPosition().x - mapPosition.x + 1) / 2 - 100) <= -100) {
		if (move.x < 0) move.x = 0;
	}
	if ((100 - (shape->getPosition().y - mapPosition.y - 1) / 2) >= 100) {
		if (move.y < 0) move.y = 0;
	}
	if ((100 - (shape->getPosition().y - mapPosition.y - 1) / 2) <= -100) {
		if (move.y > 0) move.y = 0;
	}
	shape->move(move);
}

void checkClick(bool isPressed) {
	Vector2i mousePos = Mouse::getPosition(window);

	if (checkComponent >= 0) { //Jeœli puœciliœmy element wyzeruj flagi odpowienie do przeci¹gania
		if (!isPressed) {
			dragX = 0, dragY = 0;
			drag = false;
		}
	}
}

void checkMouseBallCollision() {
	Vector2i mousePos = Mouse::getPosition(window); //Pozycja myszy w aplikacji
	for (int i = 0; i < amount; i++) {
		CircleShape* shape = objects.at(i);
		float length = sqrt(pow(position.at(i).x - mousePos.x, 2) + pow(position.at(i).y - mousePos.y, 2));
		if (length <= shape->getRadius()) {
			checkComponent = i; //Przypisanie Id elementu aktualnie przeci¹ganego
		}
	}
}

void dragElement() {
	Vector2i mousePos = Mouse::getPosition(window);
	if (checkComponent >= 0) {
		if (drag == false) {
			//Usatwienie offsetu dla myszy podczas poruszania obiektem
			dragX = position.at(checkComponent).x - mousePos.x, dragY = position.at(checkComponent).y - mousePos.y;
			drag = true;
		}
		else if (drag == true) {
			//Obliczanie wartoœci potrzebnych do przesuniêcia elementu wzglêdem planszy
			float x = mousePos.x + dragX, y = mousePos.y + dragY;
			if (mousePos.x + dragX > (mapPosition.x + 401)) x = mapPosition.x + 401;
			else if (mousePos.x + dragX < mapPosition.x) x = mapPosition.x;
			
			if (mousePos.y + dragY > (mapPosition.y + 401)) y = mapPosition.y + 401;
			else if (mousePos.y + dragY < mapPosition.y) y = mapPosition.y;

			position.at(checkComponent) = Vector2f(x, y);
			objects.at(checkComponent)->setPosition(position.at(checkComponent));
		}
	}
}

void drawData() {
	//Wyœwietlanie tablicy elementów przed rozsuwaniem i grawitacj¹
	for (int j = 0; j < amount; j++) {
		cout << j << ": x=" << (objects.at(j)->getPosition().x - mapPosition.x + 1) / 2 - 100 <<
			" y=" << 100 - (objects.at(j)->getPosition().y - mapPosition.y - 1) / 2 <<
			" r=" << objects.at(j)->getRadius() / multiplier << endl;
	}
	cout << "\n=======================================================================\n" << endl;
}

void eraseData() {
	system("CLS"); //Czyszczenie konsoli
}

void gravity(Time deltaTime) { 
	//Przypisanie pozycji obiektów do tablicy
	for (int i = 0; i < amount; i++) {
		CircleShape* shape = objects.at(i);
		position.at(i) = shape->getPosition();
	}

	//G³ówna pêtla sprawdzania kolizji
	for (int i = 0; i < amount; i++) {
		CircleShape* shape1 = objects.at(i);

		//Ustawienie odpowiednio si³y, predkoœci wraz z grawitacj¹ 
		force.at(i) = Vector2f(-velocity.at(i).x * 0.8f, -velocity.at(i).y * 0.8f + 100.f);
		velocity.at(i) += Vector2f(force.at(i) * deltaTime.asSeconds());
		position.at(i) = (position.at(i) + (velocity.at(i) * deltaTime.asSeconds()));

		//Sprawdzenie czy obiekty nie wychodz¹ poza granicê planszy
		if (position.at(i).x - shape1->getRadius() < mapPosition.x) {
			position.at(i).x = mapPosition.x + shape1->getRadius();
			velocity.at(i).x = abs(velocity.at(i).x);
		}
		else if (position.at(i).x + shape1->getRadius() > mapPosition.x + 401) {
			position.at(i).x = mapPosition.x - shape1->getRadius() + 401;
			velocity.at(i).x = -abs(velocity.at(i).x);
		}

		if (position.at(i).y - shape1->getRadius() < mapPosition.y) {
			position.at(i).y = mapPosition.y + shape1->getRadius();
			velocity.at(i).y = abs(velocity.at(i).y);
		}
		else if (position.at(i).y + shape1->getRadius() > mapPosition.y + 401) {
			position.at(i).y = mapPosition.y - shape1->getRadius() + 401;
			velocity.at(i).y = -abs(velocity.at(i).y);
		}

		//Wyzerowanie prekoœci dla ma³ych wartoœci (dLa efektywnoœci)
		if (sqrtf(pow(velocity.at(i).x, 2) + pow(velocity.at(i).y, 2)) < 0.01f)
		{
			velocity.at(i) = Vector2f(0, 0);
		}

		//Sprawdzenie kolizji z innymi kulami (dok³adne) i ich odbicie wzglêdem wektora normalnego
		for (int j = 0; j < amount; j++) {
			CircleShape* shape2 = objects.at(j);
			if (shape1 != shape2) {
				//Liczenie offsetu i dystansu dla kul
				Vector2f offset = position.at(j) - position.at(i);
				float distance = sqrt(offset.x * offset.x + offset.y * offset.y);
				float minDistance = shape1->getRadius() + shape2->getRadius();

				//Jeœli zachodzi miêdzy nimi kolizja
				if (distance < minDistance) {
					Vector2f normal = offset / distance; //Wektor normalny
					float overlap = (minDistance - distance) / 2.0f; //Wektor przesuniêcia ka¿dego elementu

					position.at(i) -= overlap * normal; //dodanie wektorów
					position.at(j) += overlap * normal;

					Vector2f relativeVelocity = velocity.at(i) - velocity.at(j); //Wyznaczanie wzglêdnej prêdkoœci
					float impulse = 2.0f * (normal.x * relativeVelocity.x + normal.y * relativeVelocity.y) / (1.0f + 1.0f); //Obliczanie implusu
					velocity.at(i) -= impulse * normal; //Aktualizacja prêdkoœci kuli
					velocity.at(j) += impulse * normal;
				}
			}
		}
	}
	//Zastosowanie pozycji z tabeli do ka¿dej kuli
	for (int i = 0; i < amount; i++) {
		CircleShape* shape = objects.at(i);
		shape->setPosition(position.at(i));
	}
}
