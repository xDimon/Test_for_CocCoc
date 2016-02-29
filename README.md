# Test_for_CocCoc
Отчет по тестовому заданию для соискателя позиции C/C++ Developer в компании Coc Coc

## Окружение
Система: Ubuntu 14.4.4 LTS<br>
IDE: Eclipse CDT

## Сборка
Для сборки нужно вызвать make в папке build.

## Описание
Реализована сортировка строк в файле комбинацией алгоритмов сортировки слиянием и быстрой сортировки:

-- Если блок данных имеет достаточный размер для размещения в блоке памяти заданного размера.<br>
-- -- Копируем блок в память, одновременно сохраняя массив указателей на начало каждой строки.<br>
-- -- После этого производится быстрая сортировка массива указателей (сравнивая строки по указателям).<br> 
-- -- Последовательно извлекая указатели из отсортированного массива соответствующие им строки копируются из памяти обратно в исходный блок.<br>
-- -- Возвращаем полученный блок<br>

-- Пытаеммся разделить блок на два по границе строк.<br>
-- -- Если блок не делится (включает ровно одну строку), возвращаем его без изменений.<br>
-- -- Иначе рекурсивно вызываем описанные действия для каждого из двух полученных под блоков, затем сливаем их в один и возвращаем его.<br>
    
Блок, возвращенный от самого первого вызова, содержит отсортированные строки исходного.

## Замечания
Файлы временные файлы создаются в текущей директории!
Работа с файловой системой полностью отдана в рапоряжение системы: входной и выходной файл, а так же временные файлы отображаются в память.

