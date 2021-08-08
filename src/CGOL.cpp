#include "SFML/Graphics.hpp"
#include <array>
#include <random>
#include <functional>
#include <iostream>
#include <chrono>

constexpr uint16_t grid_x = 80;
constexpr uint16_t grid_y = 80;
constexpr uint16_t screen_width = 800;
constexpr uint16_t screen_height = 800;
constexpr uint16_t cell_width = screen_width / grid_x;
constexpr uint16_t cell_height = screen_height / grid_y;

struct Cell
{
    Cell() : state(0){};
    bool state;
};

uint8_t countNeighbors(const std::array<std::array<Cell, grid_x>, grid_y> &grid, const uint16_t row, const uint16_t col)
{
    uint8_t cnt = 0;

    cnt += grid[row - 1][col - 1].state;
    cnt += grid[row - 1][col].state;
    cnt += grid[row - 1][col + 1].state;
    cnt += grid[row][col - 1].state;
    cnt += grid[row][col + 1].state;
    cnt += grid[row + 1][col - 1].state;
    cnt += grid[row + 1][col].state;
    cnt += grid[row + 1][col + 1].state;

    return cnt;
}

int main()
{
    /* --- Setup --- */
    // Random number generation
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> gen(0, 3);

    // Window setup
    sf::RenderWindow window(sf::VideoMode(screen_width, screen_height), "Conway's Game of Life - C++ & SFML");
    window.setFramerateLimit(10);

    // Rectangle to visualize cells
    sf::RectangleShape rect({cell_width, cell_height});
    rect.setFillColor(sf::Color::White);

    // Double buffer for the two cellular automata states
    std::array<std::array<Cell, grid_x>, grid_y> grid_buffer0;
    std::array<std::array<Cell, grid_x>, grid_y> grid_buffer1;

    std::array<std::array<Cell, grid_x>, grid_y> *grid_curr = &grid_buffer0;
    std::array<std::array<Cell, grid_x>, grid_y> *grid_next = &grid_buffer1;

    // Fill array with random values and border with dead cells
    for (size_t row = 0; row < grid_buffer0.size(); row++)
    {
        for (size_t col = 0; col < grid_buffer0[row].size(); col++)
        {
            // Check if current element is a border
            if (row == 0 || col == 0 || row == grid_y - 1 || col == grid_x - 1)
            {
                grid_buffer0[row][col].state = 0;
                grid_buffer1[row][col].state = 0;
            }
            else
            {
                // Random number is 0 <= rng <= 3, therefore  rng > 2 will give 25% chance
                grid_buffer0[row][col].state = gen(generator) > 2;
                grid_buffer1[row][col].state = grid_buffer0[row][col].state;
            }
        }
    }

    /* --- Main Loop --- */

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Calculate next state
        for (size_t row = 1; row < grid_curr->size() - 1; row++)
        {
            for (size_t col = 1; col < (*grid_curr)[row].size() - 1; col++)
            {
                auto neighbors = countNeighbors(*grid_curr, row, col);
                (*grid_next)[row][col].state = (*grid_curr)[row][col].state;
                if ((*grid_curr)[row][col].state == 0 && neighbors == 3)
                {
                    (*grid_next)[row][col].state = 1;
                }
                else if ((*grid_curr)[row][col].state == 1)
                {
                    if (neighbors < 2 || neighbors > 3)
                        (*grid_next)[row][col].state = 0;
                    else
                        (*grid_next)[row][col].state = 1;
                }
            }
        }
        // Swap
        auto tmp = grid_curr;
        grid_curr = grid_next;
        grid_next = tmp;

        // Render
        window.clear();

        for (size_t row = 0; row < grid_curr->size(); row++)
        {
            for (size_t col = 0; col < (*grid_curr)[row].size(); col++)
            {
                rect.setPosition(col * cell_width, row * cell_height);
                if ((*grid_curr)[row][col].state == 1)
                    rect.setFillColor(sf::Color::White);
                else
                    rect.setFillColor(sf::Color::Black);
                window.draw(rect);
            }
        }
        window.display();
    }

    return 0;
}