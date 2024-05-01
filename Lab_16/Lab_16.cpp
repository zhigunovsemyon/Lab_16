/* Пусть задано N прямоугольников, каждый из них задан
с помощью координат его левого верхнего угла, ширины и высоты.
Определить прямоугольник наименьшего периметра, вывести
координаты его вершин и периметр.
Добавить возможность чтения и записи бинарных файлов
*/
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <locale.h>
#define ARR_INIT 16
#define BUFF_INIT 100

//Структура "прямоугольник"
typedef struct
{
	int x;
	int y;
	int hsize;
	int vsize;
}
Rectangle;

int WayToFill(void);
int HowManyRectangles(int maxAmount, FILE* source);
int	CalcPerimeter(Rectangle* Rect);
int GetSmallestRect(Rectangle Rects[], int amountOfRects);
int ReadSingleRect(const char source[], Rectangle* Rect);
int FillArrayOfRects(FILE* source, Rectangle rects[], int arrSize);
int ReadFromBinary(int maxSize, Rectangle rects[]);
int ReadFromKeyboard(int maxSize, Rectangle rects[]);
int DoWrite(void);
int WriteToBinary(Rectangle* rects, int arrSize);
void AskUserForFilename(char filename[], int maxSize);
void PrintRectangle(Rectangle* Rect, int perimeter);

int
main(void)
{
	setlocale(LC_ALL, "ru_RU.UTF-8");
	Rectangle rects[ARR_INIT];
	int arrSize;

	//Опрос пользователя на способ заполнения массива 
	if (WayToFill()) //Заполнение из клавиатуры
	{
		wprintf(L"Выбрано чтение с клавиатуры\n");
		//Функция заполнения массива с клавиауты, опрашивающая пользователя в процессе
		arrSize = ReadFromKeyboard(ARR_INIT, rects);

		//Если при попытке прочитать прямоугольник была ошибка, выводится соответствующая ошибка
		if (arrSize == 0)
		{
			wprintf(L"Введено неправильное значение! Завершение работы\n");
			return 1;
		}
	}
	else //Заполнение из файла
	{
		wprintf(L"Выбрано чтение из файла\n");

		//Заполнение массива из файла, возврат размера массива, либо кода ошибки
		arrSize = ReadFromBinary(ARR_INIT, rects);

		//Обработка различных кодов ошибок на основе возвращённого значения
		switch (arrSize)
		{
		case 0:
			wprintf(L"Ошибка при чтении размера массива! Завершение работы\n");
			return 1;
		case -2:
			wprintf(L"Не удалось открыть файл! Завершение работы\n");
			return 1;
		case -3://Код -3 означает, что размер массива в файле был слишком большим. Значит было...
			//прочитано максимально возможное количество. Выводится соответствующее сообщение.
			//Размеру массива обратно присваивается максимальный размер
			arrSize = ARR_INIT;
			wprintf(L"ВНИМАНИЕ! В данном файле слишком большое число структур, было прочитано только %d\n", arrSize);
			//Дальше осуществляется работа в штатном режиме
		}
	}

	if (DoWrite())			//Запись в файл
		if (WriteToBinary(rects, arrSize))
		{	//Если функция записи вернула флаг ошибки, программа выводит сообщение об ошибке и аварийно завершается
			wprintf(L"Не удалось открыть файл! Завершение работы\n");
			return 1;
		}

	int smallest = GetSmallestRect(rects, arrSize);	//Получение индекса минимального прямоугольника
	//Вывод соответствующего прямоугольника на экран
	PrintRectangle(rects + smallest, CalcPerimeter(rects + smallest));

	return 0;
}

//Выбор пользователем способа заполнения массива. 
int
WayToFill(void)
{
	char buff[BUFF_INIT];
	char answer;
	do
	{
		wprintf(L"Введите f для заполения массива из файла,\n%ls",
			L"либо k для ввода с клавиатуры: ");
		fgets(buff, BUFF_INIT - 1, stdin);
		sscanf(buff, "%c", &answer);

		if (answer == 'f')
			return 0;		//Возврат кода заполнения из файла
		if (answer == 'k')
			return 1;		//Возврат кода заполнения с клавиатуры

		//При неправильном вводе опрос проводится заново
	} while (1);
}

//Записывает в буфер filename название файла, полученное у пользователя, не более данного размера maxSize
void
AskUserForFilename(char filename[], int maxSize)
{
	wprintf(L"Введите название файла: ");
	fgets(filename, maxSize - 1, stdin);
	sscanf(filename, "%[^\n]s", filename); //Ведёт чтение из строки
}

//Определяет количество прямоугольников не более данного maxAmount из переданного источника source
int
HowManyRectangles(int maxAmount, FILE* source)
{
	char buff[BUFF_INIT];
	int amount;
	fgets(buff, BUFF_INIT - 1, source);
	if (sscanf(buff, "%d", &amount) == 0)
		return 0; //Код ошибки при отсутствии прочтённого числа
	if (amount <= 0)
		return 0; //Код ошибки при неправильном значении
	if (amount >= maxAmount)
		return -1; //Код ошибки при большом количестве файлов

	return amount;	//Возврат количества при корректном вводе
}

//Способ наполнения данного массива rects, не более максимального размера maxSize, взаимодействующий с пользователем
int
ReadFromKeyboard(int maxSize, Rectangle rects[])
{
	int arrSize;
	do
	{
		wprintf(L"Сколько прямоугольников вы хотите ввести? ");
		arrSize = HowManyRectangles(maxSize, stdin);	//Ввод размера массива
		if (arrSize > 0)							//Если был возвращён корректный размер массива...
			break;									//Цикл ввода прерывается
		wprintf(L"Неправильное значение!\n");		//В противном случае выводится сообщение об ошибке...
	} while (1);									//Цикл начинается заново

	wprintf(L"Введите через пробел координату x левого верхнего угла прямоугольника,\n%ls",
		L"координату y, размер по горизонтали и по вертикали\n");
	if (FillArrayOfRects(stdin, rects, arrSize))	//Наполнение массива через функцию
		return 0;	//Если в результате заполнения был возвращён флаг ошибки, то этот флаг возвращается 

	return arrSize; //Возврат размера массива при правильной отработке
}

//Способ наполнения данного массива rects из бинарного файла, не более максимального размера maxSize
int
ReadFromBinary(int maxSize, Rectangle rects[])
{
	FILE* source;
	char filename[BUFF_INIT];

	//Опрос у пользователя названия файла, открытие его
	AskUserForFilename(filename, BUFF_INIT);
	source = fopen(filename, "rb");

	if (source == NULL)	//Если не удалось открыть файл...
		return -2;		//функция возвращает соответствующий флаг и завершает работу

	fseek(source, 0, SEEK_END);							//"Перемотка" в конец файла
	int arrSize = ftell(source) / sizeof(Rectangle);	//Подсчёт количества элементов
	rewind(source);										//"Перемотка" обратно в начало

	if (arrSize == 0)			//Если количество элементов по каким-либо причинам равно 0
	{
		fclose(source);
		return 0;				//Выводится соответствующий флаг
	}

	if (arrSize > maxSize)		//Если файл содержит элементов больше возможного...		
	{							//ведётся работа с максимальным значением
		fread(rects, sizeof(Rectangle), maxSize, source);
		fclose(source);
		return -3;				//Возврат кода о превышении размеров
	}

	//Чтение из файла. 
	fread(rects, sizeof(Rectangle), arrSize, source);
	fclose(source);
	return arrSize;
}

//Чтение прямоугольника в данный адрес Rect из данного текста source. Возвращает 0 при успешном выполнении, 1 при ошибке
int
ReadSingleRect(const char source[], Rectangle* Rect)
{
	if (sscanf(source, "%d %d %d %d", &Rect->x, &Rect->y, &Rect->hsize, &Rect->vsize) != 4)
		return 1;	//Код ошибки, если количество элементов в строке было меньше 4
	if (Rect->hsize <= 0)	//Размер должен быть положительным числом
		return 1;			//Возврат кода ошибки в противном случае
	if (Rect->vsize <= 0)	//Размер должен быть положительным числом
		return 1;			//Возврат кода ошибки в противном случае

	return 0;	//Возврат флага успешного чтения
}

//Чтение arrSize прямоугольников в массив rects из текстового источника source
int
FillArrayOfRects(FILE* source, Rectangle rects[], int arrSize)
{
	char textbuff[BUFF_INIT];
	for (int i = 0; i < arrSize; ++i)
	{
		fgets(textbuff, BUFF_INIT - 1, source);		//Ввод текста из источника
		if (ReadSingleRect(textbuff, rects + i))	//Если при попытке прочитать прямоугольник была ошибка...
			return 1;	//возвращается флаг ошибки
	}
	return 0;	//Возврат 0, если не было ошибки
}

//Опрос пользователя, будет ли он записывать массив в файл
int
DoWrite(void)
{
	char buff[BUFF_INIT];
	char answer;
	do
	{
		wprintf(L"Хотите записать массив в файл? y | n \n");
		fgets(buff, BUFF_INIT - 1, stdin);
		sscanf(buff, "%c", &answer);

		if (answer == 'n')
			return 0;		//Возврат кода записи файл
		if (answer == 'y')
			return 1;		//Возврат кода пропуска записи

		//При неправильном вводе опрос проводится заново
	} while (1);
}

//Запись в бинарный файл данного массива rects данного размера arrSize
int
WriteToBinary(Rectangle* rects, int arrSize)
{
	FILE* ourfile;
	char filename[BUFF_INIT];
	AskUserForFilename(filename, BUFF_INIT);	//Опрос у пользователя названия файла
	ourfile = fopen(filename, "wb");			//Открытие файла
	if (ourfile == NULL) //Проверка, был ли открыт файл
		return -1;

	//Запись в файл
	fwrite(rects, sizeof(Rectangle), arrSize, ourfile);
	fclose(ourfile);
	return 0; //Код успешного выполнения
}

//Поиск индекса прямоугольника c наименьшим периметром из массива Rects среди количества amountOfRects
int
GetSmallestRect(Rectangle Rects[], int amountOfRects)
{
	int candidate = 0;	//Для начала утверждается, что наименьший прямоугольник - первый

	//Цикл перебирает все прямоугольники и считает их периметры.
	//Перед началом цикла считает и сохраняет периметр первого прямоугольника
	for (int i = 1, returnedPerim, smallestPerim = CalcPerimeter(Rects); i < amountOfRects; ++i)
	{
		returnedPerim = CalcPerimeter(Rects + i);
		if (smallestPerim < returnedPerim)	//Если периметр i-го прямоугольника больше минимального
			continue;						//Цикл переходит на следующий элемент массива

		//Если периметр прямогольника меньше минимального, значение перезаписывается
		smallestPerim = returnedPerim;
		candidate = i;	//Индекс этого прямоугольника сохраняется
	}
	return candidate;		//Возврат индекса наименьшего прямоугольника
}

//Вычисление периметра прямоугольника, данного его указателем Rect
int
CalcPerimeter(Rectangle* Rect)
{
	return 2 * (Rect->hsize + Rect->vsize);
}

//Вывод расположения прямоугольника Rect, данного его указателем, и периметра perimeter
void
PrintRectangle(Rectangle* Rect, int perimeter)
{
	wprintf(L"Левый верхний угол прямогольника - (%d;%d)\n", Rect->x, Rect->y);
	wprintf(L"Периметр прямоугольника - %d\n", perimeter);
}
