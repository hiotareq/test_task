Алгоритм построения графа без пересечения вершин и с наименьшим количеством пересечений рёбер-связей из имеющейся матрицы смежности.

Этапы работы алгоритма:
1. Создание ациклического ориентированного графа из имеющегося неориентированного.
2. Разбиение графа на вертикальные уровни, в которых нет вершин, соединённых друг с другом.
3. Создание фиктивных вершин для того, чтобы не было пересечения вершин И для соединения вершин, не лежащих на соседних уровнях.
4. Сортировка вершин на уровнях для уменьшения пересечений рёбер.
5.

Пункт 1 реализован поиском в глубину и удалением ячеек матрицы смежности, порождающих графы.
Пункт 2 реализован методом стоков/истоков. Такие вершины появляются после пункта 1 => у нас есть вершины, из которых не выходит ни одного ребра, и вершины, в которые ни одного ребра не входит.
Пункт 3 реализован методом проб и ошибок. Идея проста: постепенно соединять вершины по пути от истоков к стокам.
Пункт 4 реализован методом медиан - вставки в медианное значение координат с фиксированного предыдущего или следующего уровней.

В данном алгоритме подразумевается пункт 5: выбор геометрических положений непосредственно перед отрисовкой. Но мне полученный результат уже понравился, поэтому я оставил этот пункт себе для развлечения на будущее.

Основные идеи алгоритма были взяты из методички(?) Д.Е.Бабурина "Иерархический подход для автоматического размещения ациклических графов". 

USAGE:
1. chmod +x ./run.sh
2. ./run.sh
