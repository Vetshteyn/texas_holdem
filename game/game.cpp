#include <iostream>
#include <Windows.h>
#include <algorithm>
using namespace std;

//структура карты
typedef struct sCard
{
	char suit;						//масть
	int value;						//значение
	bool used;						//флаг, чтобы одна карта не использовалась дважды
	struct sCard* next, * prev;		//указатели на следующую и предыдущую кариты
} Card;

//структура игрка
typedef struct sPlayer
{
	string name;					//имя
	int number;						//номер
	int stack;						//стэк (количество фишек)
	int currentBet;					//текущая ставка
	int hand_value;					//финальная комбинация
	bool is_BB;						//является ли игрок большим блайндом
	bool is_all_in;					//флаг олл-ина
	Card first, second;				//рука (две карты игрока)
	struct sPlayer* next, * prev;	//указатели на следующего и предыдущего игроков
} Player;

//функция cоздания карты
Card* create_сard()
{
	Card* card = new Card();
	card->next = NULL;
	card->prev = NULL;
	return card;
}

//функция cоздания игрока
Player* create_player()
{
	Player* player = new Player();
	player->next = NULL;
	player->prev = NULL;
	return player;
}

//функция добавления карты в колоду
Card* adding_card_to_the_deck(Card* head, Card* card)
{
	if (head != NULL)	//если список не пуст, то элемент привяжется к крайнему элементу
	{
		head->prev = card;
		card->next = head;
	}
	head = card;
	return head;
}

//функция добавления игрока за стол
Player* adding_player_to_the_list(Player* head, Player* player)
{
	if (head != NULL)	//если список не пуст, то элемент привяжется к крайнему элементу
	{
		head->prev = player;
		player->next = head;
	}
	head = player;
	return head;
}

//функция удаления карты из колоды
Card* remove_card(Card* head, Card* card)
{
	if (card == head)	//если элемент является заголовком, то заголовок нужно переназначить
	{
		head = head->next;
		head->prev = NULL;
		card->next = NULL;
	}
	else if (card->next == NULL)	//если элемент последний, то его отвязать только с одной стороны, иначе с двух
	{
		card->prev->next = NULL;
		card->prev = NULL;
	}
	else
	{
		card->prev->next = card->next;
		card->next->prev = card->prev;
		card->prev = NULL;
		card->next = NULL;
	}
	return head;
}

//функция удаления игрока со стола
Player* remove_looser(Player* head, Player* player, int& player_count)
{
	if (player == head)	//если элемент является заголовком, то заголовок нужно переназначить
	{
		head = head->next;
		head->prev = NULL;
		player->next = NULL;
	}
	else if (player->next == NULL)	//если элемент последний, то его отвязать только с одной стороны, иначе с двух
	{
		player->prev->next = NULL;
		player->prev = NULL;
	}
	else
	{
		player->prev->next = player->next;
		player->next->prev = player->prev;
		player->prev = NULL;
		player->next = NULL;
	}
	player_count--;
	return head;
}

//функция генерации колоды
Card* deck_generating(Card* head)
{
	Card* card;
	for (int i = 0; i < 4; i++)			//масти
	{
		for (int j = 0; j < 13; j++)	//значения
		{
			card = create_сard();
			card->suit = '\x03' + i;
			card->value = j + 2;
			head = adding_card_to_the_deck(head, card);
		}
	}
	return head;
}

//функция регистрации игроков
Player* player_registration(Player* head, int& player_count)
{
	Player* player;	//новый игрок
	//ввод количества игроков
	do	//проверка: игроков может быть от 2 до 10
	{
		cout << "Сколько будет игроков? (от 2 до 10): ";
		cin >> player_count;
		if (player_count < 2 || player_count > 10)
			cout << "Игроков должно быть от 2 до 10, введите заново;" << endl;
	} while (player_count < 2 || player_count > 10);
	cout << endl;
	//добавление игроков и заполнение имен
	for (int i = 0; i < player_count; i++)
	{
		player = create_player();
		player->number = player_count - i;
		cout << "Введите имя " << player->number << "-го игрока:" << endl;
		cin >> player->name;
		do
		{
			cout << "Введите стэк " << player->number << "-го игрока:" << endl;
			cin >> player->stack;
			if (player->stack < 1)
				cout << "Стек не может быть меньше 1, введите заново;" << endl;
		} while (player->stack < 1);
		head = adding_player_to_the_list(head, player);
		cout << endl;
	}
	head->is_BB = true;
	return head;
}

//функция торгов
void betting(Player* head, int& player_count, int& BB, int& bank)
{
	Player* player = head;	//игрок
	int max_bet = 0,		//максимальная ставка в этом кону
		word = 0;			//слово игрока
	bool F = false;			//вспомогательная переменная-флаг
	//поиск большого блайнда (игрока, который поставит первым
	/*while (player->is_BB == false)
		player = player->next;
	//фиксированная ставка большого блайнда
	cout << "Ставка игрока " << player->name << " равна большому блайнду: " << BB << endl;
	if (player->stack - BB >= 0)	//проверка: стэк не должен стать отрицательным
	{
		player->stack -= BB;		//обновление стэка игрока
		player->currentBet = BB;	//обновление текущей ставки игрока
		bank += BB;								//обновление банка
		if (player->stack == 0)		//проверка на олл-ин (если стэк обнулился)
		{
			player->is_all_in = true;
			cout << "Игрок " << player->name << " идет олл-ин!" << endl;
			cout << "Текущая ставка игрока " << player->name << " равна " << player->currentBet << "; стэк равен " << player->stack << endl << endl;
		}
		else
		{
			cout << "Ставка игрока " << player->name << " принята;" << endl;
			cout << "Текущая ставка игрока " << player->name << " равна " << player->currentBet << "; стэк равен " << player->stack << endl << endl;
		}
	}
	else											//если стэк стал отрицательным, то происходит фолд
	{
		player->currentBet = -1;
		cout << "У вас нет столько фишек;( Вы выходите из игры;" << endl << endl;
	}
	max_bet = BB;
	if (player->next == NULL)	//если список кончился, то вернуться в начало
		player = head;
	else
		player = player->next;
	//фиксированная ставка малого блайнда
	cout << "Ставка игрока " << player->name << " равна малому блайнду: " << BB / 2 << endl;
	if (player->stack - BB / 2 >= 0)	//проверка: стэк не должен стать отрицательным
	{
		player->stack -= BB / 2;		//обновление стэка игрока
		player->currentBet = BB / 2;	//обновление текущей ставки игрока
		bank += BB / 2;								//обновление банка
		if (player->stack == 0)	//проверка на олл-ин (если стэк обнулился)
		{
			player->is_all_in = true;
			cout << "Игрок " << player->name << " идет олл-ин!" << endl;
			cout << "Текущая ставка игрока " << player->name << " равна " << player->currentBet << "; стэк равен " << player->stack << endl << endl;
		}
		else
		{
			cout << "Ставка игрока " << player->name << " принята;" << endl;
			cout << "Текущая ставка игрока " << player->name << " равна " << player->currentBet << "; стэк равен " << player->stack << endl << endl;
		}
	}
	else											//если стэк стал отрицательным, то происходит фолд
	{
		player->currentBet = -1;
		cout << "У вас нет столько фишек;( Вы выходите из игры;" << endl << endl;
	}*/
	//торги
	do	//проверка: нужен ли новый круг торгов
	{
		F = false;
		for (int i = 0; i < player_count; i++)
		{
			if (player->stack <= 0)		//если стэк пустой, то игрок не участвует в торгах
				cout << "Игрок " << player->name << " не может дальше вести торги с пустым банком;" << endl << endl;
			else if (player->currentBet >= 0 && player->stack > 0)	//если currentBet = -1, то игрок не участвует в торгах
			{
				cout << "Слово игрока " << player->name << " ('-1' - фолд, '0' - колл, '1' - рейз): ";
				cin >> word;
				if (word > 0)	//рейз
				{
					do			//проверка: если ставка меньше максимальной или превращает стэк в отрицательный, при этом не обнуляет стэк (олл-ин), то вводится заново
					{
						do		//проверка: ставка не должна быть отрицательной
						{
							cout << "Ставка игрока " << player->name << ": ";
							cin >> word;
							if (word <= 0)
								cout << "ставка должна быть положительной, введите еще раз нормально: " << endl;
						} while (word <= 0);
						if (player->stack - word < 0)
							cout << "У вас нет столько фишек;( Введите приемлемую ставку: " << endl;
						if (player->stack - word != 0 && player->currentBet + word < max_bet)
							cout << "Слишком маленькая ставка, попробуйте заново: " << endl;
					} while (player->stack - word != 0 && (player->currentBet + word < max_bet || player->stack - word < 0));
					player->stack -= word;			//обновление стэка
					player->currentBet += word;		//обновление текущей ставки игрока
					bank += word;									//обновление банка
					max_bet = player->currentBet;	//обновление максимальной ставки
					if (player->stack == 0)	//проверка на олл-ин (если стэк обнулился)
					{
						player->is_all_in = true;
						cout << "Игрок " << player->name << " идет олл-ин!" << endl;
						cout << "Текущая ставка игрока " << player->name << " равна " << player->currentBet << "; стэк равен " << player->stack << endl << endl;
					}
					else
					{
						cout << "Ставка игрока " << player->name << " принята;" << endl;
						cout << "Текущая ставка игрока " << player->name << " равна " << player->currentBet << "; стэк равен " << player->stack << endl << endl;
					}
				}
				else if (word == 0)	//колл
				{
					word = max_bet - player->currentBet;
					if (player->stack - word >= 0)	//проверка: стэк не должен стать отрицательным
					{
						player->stack -= word;		//обновление стэка игрока
						player->currentBet += word;	//обновление текущей ставки игрока
						bank += word;								//обновление банка
						if (player->stack == 0)	//проверка на олл-ин (если стэк обнулился)
						{
							player->is_all_in = true;
							cout << "Игрок " << player->name << " идет олл-ин!" << endl;
							cout << "Текущая ставка игрока " << player->name << " равна " << player->currentBet << "; стэк равен " << player->stack << endl << endl;
						}
						else
						{
							cout << "Ставка игрока " << player->name << " принята;" << endl;
							cout << "Текущая ставка игрока " << player->name << " равна " << player->currentBet << "; стэк равен " << player->stack << endl << endl;
						}
					}
					else											//если стэк стал отрицательным, то происходит фолд
					{
						word = -1;
						cout << "У вас нет столько фишек;( Вы выходите из игры;" << endl;
					}
				}
				if (word < 0)	//фолд
				{
					player->currentBet = -1;	//текущей ставке присваивается отрицательное значение, чтобы она в дальнейшем не прошла проверку
					cout << "Игрок " << player->name << " выходит из раздачи;" << endl << endl;
				}
			}
			if (player->next == NULL)	//если список кончился, то вернуться в начало
				player = head;
			else
				player = player->next;
		}
		for (int i = 0; i < player_count; i++)
		{
			if (player->currentBet != max_bet && player->currentBet != -1 && player->is_all_in != true)
				F = true;
			if (player->next == NULL)	//если список кончился, то вернуться в начало
				player = head;
			else
				player = player->next;
		}
	} while (F);
	return;
}

//функция выбора из колоды рандомной карты
Card card_pick(Card* head)
{
	srand(time(0)); // Инициализация генератора случайных чисел с использованием текущего времени
	Card* card;
	do	//проверка: карта не должна быть уже использованной
	{
		card = head;
		int random = rand() % 52;	//рандомный номер карты
		for (int i = 0; i < random; i++)
			card = card->next;
	} while (card->used);
	card->used = true;				//карта отмечается использованной
	return *card;
}

//вспомогательная функция-компаратор
bool cmp(Card& a, Card& b)
{
	if (a.value < b.value) return true;
	if (a.value > b.value) return false;
	return a.suit < b.suit;
}

//функция-проверка на стрит
bool Straight(Card Hand[])
{
	int F = 0;
	for (int i = 0; i < 6; i++)
	{
		if (Hand[i].value == Hand[i + 1].value - 1)
			F++;
		else
			F = 0;
	}
	return F > 3;
}

//функция-проверка на флэш
bool Flush(Card Hand[])
{
	int F1 = 0, F2 = 0, F3 = 0, F4 = 0;
	for (int i = 0; i < 6; i++)
	{
		if (Hand[i].suit == '\x03') F1++;
		else if (Hand[i].suit == '\x04') F2++;
		else if (Hand[i].suit == '\x05') F3++;
		else if (Hand[i].suit == '\x06') F4++;
	}
	return (F1 > 4 || F2 > 4 || F3 > 4 || F4 > 4);
}

//функция-проверка на повторяющиеся карты
int longrun(Card Hand[], int& second)
{
	second = 0;
	int first = 1, current = 1;
	int v = Hand[0].value;
	for (int i = 1; i < 7; i++)
	{
		if (v == Hand[i].value)
			current++;
		else
		{
			if (current > first)
			{
				second = first;
				first = current;
			}
			if ((current <= first) && (current > second))
				second = current;
			current = 1;
			v = Hand[i].value;
		}
	}
	if (current > first)
	{
		second = first;
		first = current;
	}
	if ((current <= first) && (current > second))
		second = current;
	return first;
}

//функция определения комбинации
int Combination(Card Hand[])
{
	bool royal = false,			//флаги комбинаций
		flush = false,
		straight = false,
		four = false,
		full_house = false,
		three = false,
		two_pairs = false,
		pair = false,
		high = false;
	int first = 1, second = 1;	//число одинаковых карт
	//сортировка в порядке возрастания значений
	sort(Hand, Hand + 7, cmp);
	//выяснение комбинации первой руки
	straight = Straight(Hand);
	flush = Flush(Hand);
	if (flush && straight && Hand[6].value == 14)
		return 10;
	else if (flush && straight)
		return 9;
	else
	{
		first = longrun(Hand, second);
		if (first == 4)
			return 8;
		else if (first == 3 && second > 1)
			return 7;
		else if (flush)
			return 6;
		else if (straight)
			return 5;
		else if (first == 3)
			return 4;
		else if (first == 2 && second == 2)
			return 3;
		else if (first == 2)
			return 2;
		else
			return 1;
	}
}

//функция вывода карты в консоль
void card_print(Card& card)
{
	cout << endl;
	if (card.value == 14) cout << "туз";
	else if (card.value == 13) cout << "король";
	else if (card.value == 12) cout << "дама";
	else if (card.value == 11) cout << "валет";
	else cout << card.value;
	if(card.suit == '\x03') cout << " черви";
	else if (card.suit == '\x04') cout << " буби";
	else if (card.suit == '\x05') cout << " крести";
	else cout << " пики";
}

//функция выявления победителя
Player* winner_finder(Player* head, int& player_count, int& bank, Card Table[])
{
	int winner_value = 0, player_count_0 = player_count, i = 0;	//вспомогательные переменные
	Player* player = head;
	for (i = 0; i < player_count_0; i++)	//определение комбинации каждого игрока
	{
		Table[5] = player->first;
		Table[6] = player->second;
		player->hand_value = Combination(Table);
		player = player->next;
	}
	player = head;
	for (i = 0; i < player_count_0; i++)	//определение сильнейшей комбинации
	{
		if (winner_value < player->hand_value)
			winner_value = player->hand_value;
		player = player->next;
	}
	player = head;
	for (i = 0; i < player_count_0; i++)	//выявление проигравших
	{
		if (player->next != NULL)
		{
			if (winner_value > player->hand_value)
			{
				cout << "Игрок " << player->name << " проигрывает, его карты:";
				card_print(player->first);
				cout << ", ";
				card_print(player->second);
				cout << ";" << endl << "его стэк : " << player->stack << endl;
				player = player->next;
				head = remove_looser(head, player->prev, player_count);
			}
			else
				player = player->next;
		}
		else
			if (winner_value > player->hand_value)
			{
				cout << "Игрок " << player->name << " проигрывает, его карты:";
				card_print(player->first);
				cout << ", ";
				card_print(player->second);
				cout << ";" << endl << "его стэк : " << player->stack << endl;
				head = remove_looser(head, player, player_count);
			}
	}
	player = head;
	for (i = 0; i < player_count; i++)	//объявление победителей
	{
		player->stack += bank / player_count;
		cout << "Игрок " << player->name << " побеждает, его карты: ";
		card_print(player->first);
		cout << ", ";
		card_print(player->second);
		cout << ";" << endl << " его выигрыш : " << bank / player_count << endl;
		cout << "; его стэк: " << player->stack << endl;
		player = player->next;
	}
	return head;
}

//функция удаления игроков
Player* remove_losers(Player* head, int& player_count)
{
	Player* player = head;
	int player_count_0 = player_count;
	for (int i = 0; i < player_count_0; i++)
	{
		if (player->next != NULL)
		{
			if (player->currentBet < 0)	//если ставка отрицательная, то игрок удаляется
			{
				player = player->next;
				cout << "Игрок " << player->prev->name << " выбывает, его стэк: " << player->prev->stack << endl;
				head = remove_looser(head, player->prev, player_count);
			}
			else
				player = player->next;
		}
		else 
			if (player->currentBet < 0)
			{
				cout << "Игрок " << player->name << " выбывает, его стэк: " << player->stack << endl;
				head = remove_looser(head, player, player_count);
			}
	}
	return head;
}

//функция раздачи карт
void card_distribution(Player* head_player, int& player_count, Card* head_card)
{
	Player* player = head_player;
	cin.get();
	for (int i = 0; i < player_count; i++)
	{
		player->first = card_pick(head_card);
		player->second = card_pick(head_card);
		cout << "Игрок " << player->name << ", нажмите Enter, чтобы посмотреть свои карты:";
		cin.get();
		card_print(player->first);
		card_print(player->second);
		cout << endl << endl << "Игрок " << player->name << ", нажмите Enter, чтобы убрать карты:" << endl;
		cin.get();
		system("cls");
		player = player->next;
	}
}

int main()
{
	setlocale(LC_ALL, "Rus");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	Card* head_card = NULL, * card = NULL;
	Player* head_player = NULL, * player = NULL;
	Card Table[7]{};
	head_card = deck_generating(head_card);
	int player_count = 0, card_count = 52, BB = 0, bank = 0;
	//объявление большого блайнда
	//do	//проверка: большой блайнд должен быть положительным
	//{
	//	cout << "Введите сумму большого блайнда: ";
	//	cin >> BB;
	//	if (BB < 0)
	//		cout << "Большой блайнд должен быть положительным, введите заново;" << endl;
	//} while (BB < 0);
	//cout << endl;
	head_player = player_registration(head_player, player_count);	//регистрация игроков
	card_distribution(head_player, player_count, head_card);
	//префлоп
	cout << endl << "----------------------------------------" << endl << "ПРЕФЛОП" << endl << "----------------------------------------" << endl;
	betting(head_player, player_count, BB, bank);							//торги
	head_player = remove_losers(head_player, player_count);					//удаление пасанувших
	if (head_player->next == NULL)											//если остался 1 игрок, то подведение итогов
	{
		head_player->stack += bank / player_count;
		cout << "Игрок " << head_player->name << " побеждает, его карты: ";
		card_print(head_player->first);
		cout << ", ";
		card_print(head_player->second);
		cout << "; его выигрыш: " << bank << endl;
		cout << "; его стэк: " << head_player->stack << endl;
		return 0;
	}
	//флоп
	cout << endl << "----------------------------------------" << endl << "ФЛОП" << endl << "----------------------------------------" << endl;
	for (int i = 0; i < 3; i++)
	{
		Table[i] = card_pick(head_card);
		card_print(Table[i]);
	}
	cout << endl << endl << endl;
	betting(head_player, player_count, BB, bank);							//торги
	head_player = remove_losers(head_player, player_count);					//удаление пасанувших
	if (head_player->next == NULL)											//если остался 1 игрок, то подведение итогов
	{
		head_player->stack += bank / player_count;
		cout << "Игрок " << head_player->name << " побеждает, его карты: ";
		card_print(head_player->first);
		cout << ", ";
		card_print(head_player->second);
		cout << "; его выигрыш: " << bank << endl;
		cout << "; его стэк: " << head_player->stack << endl;
		return 0;
	}
	//терн
	cout << endl << "----------------------------------------" << endl << "ТЕРН" << endl << "----------------------------------------" << endl;
	Table[3] = card_pick(head_card);
	card_print(Table[3]);
	cout << endl << endl << endl;
	betting(head_player, player_count, BB, bank);							//торги
	head_player = remove_losers(head_player, player_count);					//удаление пасанувших
	if (head_player->next == NULL)											//если остался 1 игрок, то подведение итогов
	{
		head_player->stack += bank / player_count;
		cout << "Игрок " << head_player->name << " побеждает, его карты: ";
		card_print(head_player->first);
		cout << ", ";
		card_print(head_player->second);
		cout << "; его выигрыш: " << bank << endl;
		cout << "; его стэк: " << head_player->stack << endl;
		return 0;
	}
	//ривер
	cout << endl << "----------------------------------------" << endl << "РИВЕР" << endl << "----------------------------------------" << endl;
	Table[4] = card_pick(head_card);
	card_print(Table[4]);
	cout << endl << endl << endl;
	betting(head_player, player_count, BB, bank);							//торги
	head_player = remove_losers(head_player, player_count);					//удаление пасанувших
	if (head_player->next == NULL)											//если остался 1 игрок, то подведение итогов
	{
		head_player->stack += bank / player_count;
		cout << "Игрок " << head_player->name << " побеждает, его карты: ";
		card_print(head_player->first);
		cout << ", ";
		card_print(head_player->second);
		cout << "; его выигрыш: " << bank << endl;
		cout << "; его стэк: " << head_player->stack << endl;
		return 0;
	}
	head_player = winner_finder(head_player, player_count, bank, Table);
	return 0;
}