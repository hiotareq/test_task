#include <iostream>
#include <vector>
#include <unordered_set>

namespace graphNamespace
{
    // Сструктура вершины графа с её номером и сетом вершин, с которыми она соединена. Операторы сравнения для поиска одинаковых вершин (std::find)
    struct vertice
    {
        size_t data;
        std::unordered_set<size_t> edges;

        bool operator==(const vertice &) const;

        friend bool operator==(const vertice &v1, const vertice &v2);
    };

    class graph
    {
    public:
        graph(const size_t p_N) : N(p_N)
        {
            matrix.resize(N);
            for (auto &vec : matrix)
                vec.resize(N);

            vertices.resize(N);

            updateVertices();
        };

        graph() : graph(0){};

        // преобразование от цикличного неориентированного графа к ациклическому ориентированному
        void prepareGraph();

        // матрица смежности
        std::vector<std::vector<double>> matrix;

        // набор вершин с их рёбрами
        std::vector<vertice> vertices;

        // обновление связей между вершинами при изменении направлений
        void updateVertices();

        // Получаем истоки(вершины, куда не входят рёбра-связи) - столбец, заполненный нулями
        std::unordered_set<size_t> getSources() const;

        // Получаем стоки (вершины, откуда не выходит рёбер-связей) - строка, заполненная нулями
        std::unordered_set<size_t> getRunoffs() const;

        // вертикальные уровни рисунка
        std::vector<std::vector<size_t>> getLevels();

        // упорядочивание на вертикальных уровнях с помощью метода медиан соседей с предыдущего уровня
        void sortLevels(std::vector<std::vector<size_t>> &levels);

    private:
        // размер матрицы
        size_t N;

        // поиск в глубину циклов в графе и удаление связей, способствующих циклу; row - вершина в которой сейчас находимся и откуда совершаем перемещения (ей соотвествует строка матрицы), mem - вершина, циклы которой нам надо удалить
        void dfs(size_t row, std::unordered_set<size_t> &mem);

        // добавление фиктивных вершин, чтобы не было наложения вершин друг на друга
        void addFakeVertices(std::vector<std::vector<size_t>> &levels);
    };
}