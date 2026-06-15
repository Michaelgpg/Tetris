#include <SFML/Graphics.hpp>
#include <array>
#include <vector>
#include <random>
#include <chrono>

const int WIDTH = 10;
const int HEIGHT = 20;
const int CELL_SIZE = 30;

const sf::Color COLORS[] = {
    sf::Color::Black,
    sf::Color::Cyan,
    sf::Color::Yellow,
    sf::Color(128, 0, 128),
    sf::Color::Green,
    sf::Color::Red,
    sf::Color::Blue,
    sf::Color(255, 165, 0)
};

// формы фигур [тип][поворот][y][x]
const int SHAPES[7][4][4][4] = {{
    // I
    {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},
    {{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0}},
    {{0,0,0,0},{0,0,0,0},{1,1,1,1},{0,0,0,0}},
    {{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}}
},{
    // O
    {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}},
    {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}},
    {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}},
    {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}}
},{
    // T
    {{0,0,0,0},{0,1,0,0},{1,1,1,0},{0,0,0,0}},
    {{0,0,0,0},{0,1,0,0},{0,1,1,0},{0,1,0,0}},
    {{0,0,0,0},{0,0,0,0},{1,1,1,0},{0,1,0,0}},
    {{0,0,0,0},{0,1,0,0},{1,1,0,0},{0,1,0,0}}
},{
    // S
    {{0,0,0,0},{0,1,1,0},{1,1,0,0},{0,0,0,0}},
    {{0,0,0,0},{0,1,0,0},{0,1,1,0},{0,0,1,0}},
    {{0,0,0,0},{0,0,0,0},{0,1,1,0},{1,1,0,0}},
    {{1,0,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}}
},{
    // Z
    {{0,0,0,0},{1,1,0,0},{0,1,1,0},{0,0,0,0}},
    {{0,0,0,0},{0,0,1,0},{0,1,1,0},{0,1,0,0}},
    {{0,0,0,0},{0,0,0,0},{1,1,0,0},{0,1,1,0}},
    {{0,0,1,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}}
},{
    // J
    {{0,0,0,0},{1,0,0,0},{1,1,1,0},{0,0,0,0}},
    {{0,0,0,0},{0,1,1,0},{0,1,0,0},{0,1,0,0}},
    {{0,0,0,0},{0,0,0,0},{1,1,1,0},{0,0,1,0}},
    {{0,0,0,0},{0,1,0,0},{0,1,0,0},{1,1,0,0}}
},{
    // L
    {{0,0,0,0},{0,0,1,0},{1,1,1,0},{0,0,0,0}},
    {{0,0,0,0},{0,1,0,0},{0,1,0,0},{0,1,1,0}},
    {{0,0,0,0},{0,0,0,0},{1,1,1,0},{1,0,0,0}},
    {{0,0,0,0},{1,1,0,0},{0,1,0,0},{0,1,0,0}}
}};

struct Tetromino {
    int type, rotation, x, y;

    std::vector<std::vector<int>> getMatrix() const {
        std::vector<std::vector<int>> m(4, std::vector<int>(4));
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                m[i][j] = SHAPES[type][rotation][i][j];
        return m;
    }
};

class Tetris {
private:
    std::array<std::array<int, WIDTH>, HEIGHT> field{};
    Tetromino current, next;
    int score = 0;
    bool gameOver = false;
    std::mt19937 rng{static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count())};

    Tetromino randomTetromino() {
        std::uniform_int_distribution<int> dist(0, 6);
        return {dist(rng), 0, WIDTH/2 - 2, 0};
    }

    bool collision(const Tetromino& t) const {
        auto m = t.getMatrix();
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                if (m[i][j]) {
                    int x = t.x + j, y = t.y + i;
                    if (x < 0 || x >= WIDTH || y >= HEIGHT) return true;
                    if (y >= 0 && field[y][x]) return true;
                }
        return false;
    }

    void merge() {
        auto m = current.getMatrix();
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                if (m[i][j]) {
                    int x = current.x + j, y = current.y + i;
                    if (y >= 0) field[y][x] = current.type + 1;
                }
    }

    void clearLines() {
        int cleared = 0;
        for (int y = HEIGHT-1; y >= 0; ) {
            bool full = true;
            for (int x = 0; x < WIDTH; x++)
                if (!field[y][x]) { full = false; break; }
            if (full) {
                for (int row = y; row > 0; row--)
                    field[row] = field[row-1];
                field[0] = {};
                cleared++;
            } else y--;
        }
        if (cleared) score += (cleared==1 ? 100 : cleared==2 ? 300 : cleared==3 ? 500 : 800);
    }

    void spawnNew() {
        current = next;
        next = randomTetromino();
        if (collision(current)) gameOver = true;
    }

public:
    Tetris() {
        next = randomTetromino();
        spawnNew();
    }

    void moveLeft()  { Tetromino t = current; t.x--; if (!collision(t)) current = t; }
    void moveRight() { Tetromino t = current; t.x++; if (!collision(t)) current = t; }
    void rotate()    { Tetromino t = current; t.rotation = (t.rotation+1)%4; if (!collision(t)) current = t; }

    void hardDrop() {
        while (true) {
            Tetromino t = current;
            t.y++;
            if (collision(t)) break;
            current = t;
        }
        merge();
        clearLines();
        spawnNew();
    }

    bool update() {
        Tetromino t = current;
        t.y++;
        if (collision(t)) {
            merge();
            clearLines();
            spawnNew();
            return true;
        }
        current = t;
        return false;
    }

    void draw(sf::RenderWindow& window) {
        for (int y = 0; y < HEIGHT; y++)
            for (int x = 0; x < WIDTH; x++) {
                sf::RectangleShape cell(sf::Vector2f(CELL_SIZE-1, CELL_SIZE-1));
                cell.setPosition(sf::Vector2f(x*CELL_SIZE, y*CELL_SIZE));
                cell.setFillColor(COLORS[field[y][x]]);
                cell.setOutlineThickness(1);
                cell.setOutlineColor(sf::Color(50,50,50));
                window.draw(cell);
            }

        auto m = current.getMatrix();
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                if (m[i][j]) {
                    sf::RectangleShape cell(sf::Vector2f(CELL_SIZE-1, CELL_SIZE-1));
                    cell.setPosition(sf::Vector2f((current.x+j)*CELL_SIZE, (current.y+i)*CELL_SIZE));
                    cell.setFillColor(COLORS[current.type+1]);
                    cell.setOutlineThickness(1);
                    cell.setOutlineColor(sf::Color(50,50,50));
                    window.draw(cell);
                }

        sf::Font font;
        if (font.openFromFile("/usr/share/fonts/TTF/DejaVuSans.ttf")) {
            sf::Text text(font);
            text.setCharacterSize(24);
            text.setFillColor(sf::Color::White);
            text.setPosition(sf::Vector2f(WIDTH*CELL_SIZE+10, 10));
            text.setString("Score: " + std::to_string(score));
            window.draw(text);

            if (gameOver) {
                text.setString("GAME OVER\nPress R");
                text.setPosition(sf::Vector2f(WIDTH*CELL_SIZE+10, 60));
                window.draw(text);
            }

            text.setString("Next:");
            text.setPosition(sf::Vector2f(WIDTH*CELL_SIZE+10, 130));
            window.draw(text);

            auto nm = next.getMatrix();
            for (int i = 0; i < 4; i++)
                for (int j = 0; j < 4; j++)
                    if (nm[i][j]) {
                        sf::RectangleShape cell(sf::Vector2f(CELL_SIZE-1, CELL_SIZE-1));
                        cell.setPosition(sf::Vector2f(WIDTH*CELL_SIZE+10 + j*CELL_SIZE, 170 + i*CELL_SIZE));
                        cell.setFillColor(COLORS[next.type+1]);
                        cell.setOutlineThickness(1);
                        cell.setOutlineColor(sf::Color(50,50,50));
                        window.draw(cell);
                    }
        }
    }

    bool isGameOver() const { return gameOver; }
    void restart() {
        field = {};
        score = 0;
        gameOver = false;
        next = randomTetromino();
        spawnNew();
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode({WIDTH*CELL_SIZE+200, HEIGHT*CELL_SIZE}), "Tetris");
    window.setFramerateLimit(60);
    Tetris game;
    sf::Clock clock;
    float fallInterval = 0.5f;

    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (!game.isGameOver()) {
                    if (keyPressed->scancode == sf::Keyboard::Scan::Left) game.moveLeft();
                    else if (keyPressed->scancode == sf::Keyboard::Scan::Right) game.moveRight();
                    else if (keyPressed->scancode == sf::Keyboard::Scan::Down) game.update();
                    else if (keyPressed->scancode == sf::Keyboard::Scan::Up) game.rotate();
                    else if (keyPressed->scancode == sf::Keyboard::Scan::Space) game.hardDrop();
                }
                if (keyPressed->scancode == sf::Keyboard::Scan::R && game.isGameOver())
                    game.restart();
            }
        }

        if (!game.isGameOver() && clock.getElapsedTime().asSeconds() >= fallInterval) {
            game.update();
            clock.restart();
        }

        window.clear(sf::Color::Black);
        game.draw(window);
        window.display();
    }
    return 0;
}