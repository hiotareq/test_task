#include <iostream>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <string>
#include <sstream>
#include <fstream>

#include "../include/graph.h"

using namespace graphNamespace;

int main()
{
    size_t N;

    // чтение данных и заполнение графа
    ///////////////////////////////////////////////////////////////////////////
    std::ifstream inputData;
    inputData.open("data/matrix_data.txt");
    if (!inputData.is_open())
    {
        std::cout << "Error while opening file" << std::endl;
        return -1;
    }
    inputData >> N;

    graph G(N);

    for (size_t i = 0; i < N; ++i)
    {
        for (size_t j = 0; j < N; ++j)
        {
            inputData >> G.matrix[i][j];
        }
    }

    inputData.close();
    ///////////////////////////////////////////////////////////////////////////

    G.prepareGraph();

    std::vector<std::vector<size_t>> levels = G.getLevels();

    G.sortLevels(levels);

    // Ниже только отрисовка
    ///////////////////////////////////////////////////////////////////////////

    sf::RenderWindow window(sf::VideoMode(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height), "Awesome graph");

    std::vector<sf::CircleShape> circles;
    circles.resize(N, sf::CircleShape(10));
    std::vector<sf::VertexArray> lines;
    std::vector<std::vector<sf::VertexArray>> linesToDraw;
    size_t circleCounter = 0;

    sf::Font font;
    if (!font.loadFromFile("fonts/arial_narrow_7.ttf"))
        std::cout << "ERROR!";

    std::vector<sf::Text> numbers;

    size_t step = 50;

    for (size_t i = 0; i < levels.size(); ++i)
    {
        for (size_t j = 0; j < levels[i].size(); ++j)
        {
            // вершина не фиктивная -> рисуем её
            if (levels[i][j] < N)
            {
                sf::Text text;

                std::stringstream ss;
                ss << circleCounter + 1;

                circles[circleCounter].setPosition(sf::Vector2f(50 + i * step, window.getSize().y / 2 + j * step));
                circleCounter++;

                text.setFont(font);
                text.setString(ss.str());
                text.setCharacterSize(15);

                // абсцисса на 5 больше, чем у круга, ордината на два меньше, чем у круга
                text.setPosition(sf::Vector2f(56 + i * step, window.getSize().y / 2 + j * step));
                text.setFillColor(sf::Color::Red);
                numbers.push_back(text);
            }

            // если не последний уровень, то рисуем линии
            if (i < levels.size() - 1)
            {
                // найти конечную точку для линии
                std::vector<size_t> edges;
                for (auto &edge : G.vertices[levels[i][j]].edges)
                {
                    edges.push_back(edge);
                }

                lines.resize(edges.size());
                for (size_t f = 0; f < edges.size(); ++f)
                {
                    lines[f] = sf::VertexArray(sf::Lines, 2);

                    // абсцисса и ордината больше на 10, чем у круга
                    lines[f][0].position = sf::Vector2f(60 + i * step, window.getSize().y / 2 + 10 + j * step);

                    // найти вертикальную координату для второго конца ребра
                    size_t height = 0;
                    for (size_t k = 0; k < levels[i + 1].size(); ++k)
                    {
                        if (edges[f] == levels[i + 1][k])
                        {
                            height = k;
                            break;
                        }
                    }

                    // абсцисса больше на 60, чем у круга, ордината больше на 10
                    lines[f][1].position = sf::Vector2f(110 + i * step, window.getSize().y / 2 + 10 + height * step);
                }
                linesToDraw.push_back(lines);
                lines.clear();
            }
        }
    }

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:

                window.close();
                break;
            }
        }

        window.clear();

        for (size_t i = 0; i < linesToDraw.size(); ++i)
        {
            for (size_t j = 0; j < linesToDraw[i].size(); ++j)
            {
                window.draw(linesToDraw[i][j]);
            }
        }
        // draw here
        for (size_t i = 0; i < circles.size(); ++i)
        {
            window.draw(circles[i]);
            window.draw(numbers[i]);
        }

        window.display();
    }

    return 0;
}