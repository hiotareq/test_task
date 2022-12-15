#include <algorithm>
#include <fstream>
#include <map>

#include "../include/graph.h"

namespace graphNamespace
{

    void graph::dfs(size_t row, std::unordered_set<size_t> &mem)
    {
        for (size_t j = 0; j < N; ++j)
        {
            if (matrix[row][j] == 1)
            {
                if (mem.find(j) != mem.end())
                {
                    matrix[row][j] = 0;
                }
                else
                {
                    mem.insert(j);
                    dfs(j, mem);
                }
            }
        }
        // чтобы не удалять лишние элементы матрицы
        // ВАЖНО ДЛЯ ЛОГИКИ
        mem.erase(row);
    }

    void graph::prepareGraph()
    {
        for (size_t i = 0; i < N; ++i)
        {
            std::unordered_set<size_t> mem;
            mem.insert(i);
            dfs(i, mem);
        }

        // матрица обновилась -> ОБНОВИЛИСЬ некоторые СВЯЗИ: теперь нет циклов
        updateVertices();
    }

    std::unordered_set<size_t> graph::getSources() const
    {
        std::unordered_set<size_t> answer;
        size_t i = 0;
        for (size_t j = 0; j < N; j++)
        {
            for (i = 0; i < N; i++)
            {
                if (matrix[i][j] != 0)
                    break;
            }
            if (i == N)
            {
                answer.insert(j);
            }
        }

        return answer;
    }

    std::unordered_set<size_t> graph::getRunoffs() const
    {
        std::unordered_set<size_t> answer;
        size_t j = 0;
        for (size_t i = 0; i < N; i++)
        {
            for (j = 0; j < N; j++)
            {
                if (matrix[i][j] != 0)
                    break;
            }
            if (j == N)
                answer.insert(i);
        }

        return answer;
    }

    void graph::updateVertices()
    {
        vertices.clear();
        vertices.resize(N);
        for (size_t i = 0; i < N; ++i)
        {
            vertice tmp;
            tmp.data = i + 1;
            for (size_t j = 0; j < N; j++)
            {
                if (matrix[i][j] != 0)
                {
                    tmp.edges.insert(j);
                }
            }
            vertices[i] = tmp;
        }
    }

    bool vertice::operator==(const vertice &other) const
    {
        return other.data == data;
    }

    bool operator==(const vertice &v1, const vertice &v2)
    {
        return v1.data == v2.data;
    }

    std::vector<std::vector<size_t>> graph::getLevels()
    {
        std::vector<std::vector<size_t>> answer;

        // истоки
        std::unordered_set<size_t> sources = getSources();

        // стоки
        std::unordered_set<size_t> runoffs = getRunoffs();
        answer.push_back(std::vector<size_t>(0));
        for (auto &num : sources)
        {
            answer[0].push_back(num);
        }

        // хэш-таблицы текущего и следующего уровней (аналогия поиска в ширину)
        std::unordered_set<size_t> next_level, cur_level;

        // таблица для отслеживания уже использованных в построении уровней вершин
        std::unordered_set<size_t> used;
        for (auto &num : sources)
        {
            used.insert(num);
        }

        // таблица для отбора на удаление из следующего уровня
        std::unordered_set<size_t> toErase;

        // начало поиска в ширину
        for (auto &num : sources)
        {
            cur_level.insert(num);
        }
        while (cur_level.size() > 0 || next_level.size() > 0)
        {
            for (auto &num : cur_level)
            {
                for (auto &edge : vertices[num].edges)
                {
                    next_level.insert(edge);
                }
            }
            cur_level.clear();
            for (const size_t &num : next_level)
            {
                // усли вершина - сток
                if (runoffs.find(num) != runoffs.end() ||
                    used.find(num) != used.end())
                {
                    toErase.insert(num);
                }
            }

            // на одном уровне не должно быть вершин, которые ведут друг в друга
            for (const size_t &outV : next_level)
            {
                for (const size_t &innV : next_level)
                {
                    if (outV == innV)
                        continue;
                    if (vertices[outV].edges.find(innV) != vertices[outV].edges.end())
                    {
                        if (toErase.find(outV) == toErase.end())
                        {
                            toErase.insert(innV);
                        }
                    }
                }
            }

            // поиск потерянных связей
            for (const size_t &nextLV : next_level)
            {
                for (const size_t &usedV : used)
                {
                    // если есть потеря
                    if (vertices[nextLV].edges.find(usedV) != vertices[nextLV].edges.end())
                    {
                        // поворачиваем ребро в другую сторону
                        vertices[nextLV].edges.erase(usedV);
                        vertices[usedV].edges.insert(nextLV);
                    }
                }
            }

            // удаление отобранных вершин
            for (const size_t &num : toErase)
            {
                next_level.erase(std::find(next_level.begin(), next_level.end(), num));
            }
            toErase.clear();

            for (auto &num : next_level)
            {
                used.insert(num);
            }

            if (next_level.size() > 0)
            {
                answer.push_back(std::vector<size_t>(0));
                for (auto &num : next_level)
                {
                    answer[answer.size() - 1].push_back(num);
                }
            }
            std::swap(cur_level, next_level);
        }//конец поиска в ширину

        answer.push_back(std::vector<size_t>(0));
        for (auto &num : runoffs)
        {
            answer[answer.size() - 1].push_back(num);
        }
        return answer;
    }

    void graph::addFakeVertices(std::vector<std::vector<size_t>> &levels)
    {
        // счётчик фиктивных вершин
        size_t counter = 0;

        // вершины, связи с которыми из текущей вершины надо удалить
        std::map<size_t, std::vector<size_t>> toRemove;
        // то же самое, только не удалить, а создать
        std::map<size_t, std::vector<size_t>> toCreate;
        for (size_t i = 0; i < levels.size() - 1; ++i)
        {
            for (size_t j = 0; j < levels[i].size(); ++j)
            {
                for (auto &edge : vertices[levels[i][j]].edges)
                {
                    // нужного второго конца ребра нет в соседнем уровне
                    if (std::find(levels[i + 1].begin(), levels[i + 1].end(), edge) == levels[i + 1].end())
                    {
                        vertice tmp;
                        tmp.data = N + counter + 1;
                        tmp.edges.insert(edge);
                        vertices.push_back(tmp);

                        // добавление связи с фиктивной вершиной
                        toCreate[levels[i][j]].push_back(N + counter);

                        // удаление связи с реальной вершиной
                        toRemove[levels[i][j]].push_back(edge);

                        // добавление фиктивной вершины в соседний уровень
                        levels[i + 1].push_back(N + counter);

                        // не забыть увеличить счётчик фиктивных вершин
                        ++counter;
                    }
                }

                //создание новых связей с фиктивными вершинами
                for (auto &pair : toCreate)
                {
                    for (auto &num : pair.second)
                    {
                        vertices[pair.first].edges.insert(num);
                    }
                }
                toCreate.clear();

                //удаление старых связей с вершинами
                for (auto &pair : toRemove)
                {
                    for (auto &num : pair.second)
                    {
                        vertices[pair.first].edges.erase(num);
                    }
                }
                toRemove.clear();
            }
        }
    }

    void graph::sortLevels(std::vector<std::vector<size_t>> &levels)
    {
        addFakeVertices(levels);

        std::vector<std::vector<size_t>> sortedLevels;
        sortedLevels.resize(levels.size());
        for (size_t i = 0; i < sortedLevels.size(); ++i)
        {
            sortedLevels[i].resize(levels[i].size());
        }

        sortedLevels[0] = levels[0];

        size_t neighbours = 0;
        for (size_t i = 1; i < levels.size(); ++i)
        {
            for (size_t j = 0; j < levels[i].size(); ++j)
            {
                for (size_t k = 0; k < levels[i - 1].size(); ++k)
                {
                    // есть сосед слева
                    if (std::find(vertices[levels[i - 1][k]].edges.begin(), vertices[levels[i - 1][k]].edges.end(), levels[i][j]) != vertices[levels[i - 1][k]].edges.end())
                    {
                        neighbours += k;
                    }
                }

                neighbours /= 2;
                size_t tmp = neighbours;
                if (sortedLevels[i][tmp] != 0)
                {
                    while (sortedLevels[i][tmp] == 0 && tmp > 0)
                    {
                        --tmp;
                    }
                    if (sortedLevels[i][0] != 0)
                    {
                        tmp = neighbours + 1;
                        while (sortedLevels[i][tmp] != 0)
                        {
                            ++tmp;
                        }
                        sortedLevels[i][tmp] = levels[i][j];
                    }
                    else
                    {
                        sortedLevels[i][0] = levels[i][j];
                    }
                }
                else
                {
                    sortedLevels[i][tmp] = levels[i][j];
                }
                neighbours = 0;
            }
        }
        std::swap(levels, sortedLevels);
    }
} // namespace graph