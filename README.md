**Проект**

Проект симулирует консольный TCP сервер, который может обрабатывать несколько клиентов одновременно с помощью linux сокетов.

**Задание**

Разработать на С++ простые консольные TCP клиент и сервер с использованием linux socket.
Код писать в стиле ООП.

Клиент получает из командной строки 3 параметра:
1) Текстовое имя клиента
2) Номер порта сервера
3) Период подключения к серверу в секундах

Клиент с указанным периодом подключается к серверу и отправляет текстовую строку в формате
[yyyy-mm-dd hh:mm:ss.ms] "имя_клиента"

Сервер получает из командной строки 1 параметр:
1) Номер порта

После запуска сервер слушает указанный порт, получает сообщения от клиентов и записывает их в файл log.txt
Каждое подключение клиента должно быть обработано в отдельном потоке.
Каждое сообщение должно быть записано в отдельную строку.
Сервер должен позволять работать с несколькими клиентами одновременно и обеспечивать корректный доступ к файлу log.txt

Например, запускаем сервер
server 3000

Запускаем клиенты
client Name1 3000 1
client Name2 3000 2
client Name3 3000 3

Тогда в log.txt ожидаем увидеть приблизительно такую картину (будет зависеть от момента запуска клиентов)
[2018-09-19 13:50:01.000] Name1
[2018-09-19 13:50:02.000] Name1
[2018-09-19 13:50:02.010] Name2
[2018-09-19 13:50:03.000] Name1
[2018-09-19 13:50:03.010] Name3
[2018-09-19 13:50:04.000] Name1
[2018-09-19 13:50:04.010] Name2
[2018-09-19 13:50:05.000] Name1
[2018-09-19 13:50:06.000] Name1
[2018-09-19 13:50:06.010] Name2
[2018-09-19 13:50:06.020] Name3

и так далее.

**Сборка проекта**

Для сборки проекта рядом с директорией src надо создать директорию build.

mkdir build
cd build

В директории build необходимо вызвать cmake, а затем make

cmake ../
make

В директории build создастся поддирректория bin с исполняемыми файлами client и server.

Запуск происходит в соответствии с условием из задания:

./client name port period

или

./server port

В этой же директории bin создастся файл log.txt.


**Пояснение к коду**

Код писался на языке C++ по 11 стандарту (т.к. в компании требуется хорошее знание C++ 11 стандарта).

Код для сервера и клиента инкапсулирован, т.е. пользователю доступны лишь конструкторы и метод для запуска.

Код написан в стиле ООП с использованием классов Client и Server.

Управление TCP соединением поделено на множество методов, если придется менять какую-либо часть функционала / дополнительно логировать, иными словами, для расширяемости кода.

Как клиент, так и сервер выполнены так, чтобы программа завершалась в исключительных случаях: клиент не смог подключиться к серверу, сервер не начал слушать сокет. Остальные ошибки (клиент отправил не все данные, не смог завершить связь, сервер не получил данные по соединению) считаются нефатальными. Однако программа может быть легко модифицирована, чтобы завершаться в нефатальных случаях. В задании ничего про это не сказано и было оставлено на усмотрение программиста.

При ошибке подключения к серверу клиент имеет в запасе еще CONN_RETRY_ попыток подключения с интервалом CONN_RETRY_PERIOD_SECONDS_.После этого клиент завершается с ошибкой "Cannot connect to the server".

Для обеспечения многопоточности и корректного доступа к файлу log.txt были выбраны std::thread и std::mutex. При попытке записи в log.txt сначала будет происходить захват мьютекса с помощью std::lock_guard.

Для того, чтобы не хардкодить индексы аргументов командой строки, были созданы enum'ы как для клиента, так и для сервера. В каждом enum'е есть элемент COUNT, чтобы иметь возможность узнать количество элементов в enum. Фича спорная, но и задача не продуктовая.

Также и для клиента, и для сервера добавлены конструкторы в случае, если в командную строку также будет передаваться IPv4 адрес сервера. По умолчанию выбран 127.0.0.1 ("localhost"), однако поддерживается функция и для любого другого корректного адреса в сети.

Количество клиентов, которое может одновременно обрабатывать сервер, указано в переменной MAX_CONN_NUM_. Может быть выставлено любым корректным числом.
