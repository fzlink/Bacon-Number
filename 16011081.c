#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define true 1
#define false 0
#define R 31 // Horner metodu icin belirlenen asal sayi
#define MAX_DOC_NAME 100 // Maksimum dokuman uzunlugu
#define MAX_MOVIE_LINE 4000	// Dosyadaki filmin maksimum satir uzunlugu
#define MAX_NAME_LEN 120 // Aktor veya filmin isminin maksimum uzunlugu
#define QUEUE_SIZE 500000 // Kuyrugun maksimum uzunlugu
#define M 750000 // Hash yapilacak tablonun buyuklugu

typedef struct node {
	char *name; // Film veya aktor ismi
	int hash; // Hash numarasi
	struct node *next; // Linkli liste yapisi icin next pointeri
}NODE;

/////////QUEUE
typedef struct queue {
	struct node *queue[QUEUE_SIZE]; // kuyruk icin node dizisi
	int front; // Kuyrugun  basi
	int rear; // Kuyrugun sonu
}QUEUE;

void enqueue(QUEUE *q, NODE *node) {
	q->rear++;
	q->queue[q->rear] = node;
}

int isEmpty(QUEUE*q) {
	if (q->front > q->rear)
		return 1;
	return 0;
}

NODE *dequeue(QUEUE *q) {
	if (!isEmpty(q)) {
		q->front++;
		return q->queue[q->front - 1];
	}
	else
		printf("Kuyruk Bos,Eleman Cikartilamaz.");
}

//////////////////
void clearScreen() {
	system("cls");
}

void nullList(NODE **list) {
	int i; // Dongu degiskeni
	for (i = 0; i < M; i++) {
		list[i] = NULL;
	}
}

void zeroVisited(int visited[]) {
	int i; // Dongu degiskeni
	for (i = 0; i < M; i++)
		visited[i] = 0;
}

void printHashTable(NODE **list) {
	int i; // Dongu degiskeni
	for (i = 0; i < M; i++) {
		if (list[i] != NULL)
			printf("%s\n", list[i]->name);
	}
}

unsigned long long int hash(unsigned long long int hash_no, int i) {
	unsigned long long int h1 = hash_no % M; // Hash fonksiyonu
	return(h1 + i) % M;
}

unsigned long long int getHashNumber(char *str) {
	int i = 0; // Artirim icin degisken
	unsigned long long int key = 0; // Hash sayisi degiskeni
	int N = strlen(str); // Kelime uzunlugu
	while (i < N) {
		key += str[i++] * pow(R, N - i - 1);
	}
	return key;
}

unsigned long long int getKey(char *str, int i) {
	return hash(getHashNumber(str), i);
}

int searchFromHash(char *str, NODE **list) {
	int i = 0; // Hash yapilirken linear probing saglayan degisken
	unsigned long long int key = getKey(str, i++); // Hashlendikten sonra gelen key degeri
	if (list[key] == NULL) {
		return -1;
	}
	while (list[key] && strcmp(list[key]->name, str) != 0)
		key = getKey(str, i++);
	if(list[key])
		return key;
	return -1;
}

NODE *createNode(char *name) {
	NODE *newNode = malloc(sizeof(NODE)); // Yeni node
	newNode->name = malloc(MAX_NAME_LEN * sizeof(char));
	strcpy(newNode->name, name);
	newNode->next = NULL;
	return newNode;
}

void readFile(FILE *file, NODE **list) {
	unsigned long long int hash_no; // Kelimenin hash sayisi
	unsigned long long int key; // Hashlendikten sonra gelen key degeri
	unsigned long long int movie_key; // Bulunulan filmin key degeri
	char movie_line[MAX_MOVIE_LINE]; // Okunan film satiri
	char is_same = false; // Ayni aktor mu kontrolu
	char *token; // Film satiri / karakterine gore token edilirken kullanilan string
	struct node *current; // Linkli Liste icin
	int i; // Film mi kontrolu icin
	int	j;  // Hash yapilirken linear probing saglayan degisken
	int movie_count = 0; // Film sayisi
	int	actor_count = 0; // Aktor sayisi
	while (fgets(movie_line, MAX_MOVIE_LINE, file) != NULL) {
		//printf("%s", movie_line);
		movie_line[strlen(movie_line) - 1] = '\0';
		i = j = 0;
		token = strtok(movie_line, "/");
		while (token != NULL) {
			is_same = false;
			hash_no = getHashNumber(token);
			j = 0;
			key = hash(hash_no, j);
			while (list[key] != NULL && !is_same) {
				if (strcmp(token, list[key]->name) == 0) {

					is_same = true;
					//printf("%s\n",list[key]->name);
				}
				if (!is_same) {
					j++;
					key = hash(hash_no, j);
				}

			}

			if (i == 0) {
				list[key] = createNode(token);
				list[key]->hash = key;
				movie_key = key;
				movie_count++;
			}
			else {
				if (!is_same) {
					list[key] = createNode(token);
					list[key]->hash = key;
					actor_count++;
				}
				/////Aktor->film
				NODE *newNode = createNode(list[movie_key]->name);
				newNode->hash = movie_key;
				current = list[key];
				while (current->next != NULL) {
					current = current->next;
				}
				current->next = newNode;

				/////Film->aktor
				NODE *newNode2 = createNode(token);
				newNode2->hash = key;
				current = list[movie_key];
				while (current->next != NULL) {
					current = current->next;
				}
				current->next = newNode2;
			}

			i++;
			token = strtok(NULL, "/");

		}
	}
	printf("\nFilm Sayisi : %d\n", movie_count);
	printf("\nAktor Sayisi : %d\n", actor_count);
}


void findPath(int parent[],char *actor1,char *actor2,NODE **list){
	int i;
	int	j;
	char *str = "0";
	char *str_movie;
	char *str_actor;
	int bacon_no = 0;
	int *reverse = malloc(M * sizeof(int));
	unsigned long long int key = searchFromHash(actor2, list);
	if (strcmp(actor1, actor2) == 0) {
		printf("Ayni aktoru girdiniz.");
		return;
	}
	j = 1;
	reverse[0] = key;
	i = parent[key];
	reverse[j++] = i;
	str = list[i]->name;
	//printf("%s\n", str);
	while(strcmp(str,actor1) != 0){
		i = parent[i];
		reverse[j++] = i;
		str = list[i]->name;
		//printf("%s\n",str);
	}
	j--;
	while (j > 0) {
		bacon_no++;
		if (bacon_no > 6) {
			printf("\n...\n...\nBaglanti bulunamadi. (bacon sayisi > 6) \n");
		}
		else {
			str_actor = list[reverse[j--]]->name;
			printf("%s - ", str_actor);
			str_actor = list[reverse[--j]]->name;
			printf("%s", str_actor);
			j++;
			str_movie = list[reverse[j--]]->name;
			printf(" \" %s \" \n", str_movie);
		}
	}
	printf("Bacon Sayisi: %d\n",bacon_no);
}

void findConnection(char *actor1, char *actor2, NODE **list) {
	QUEUE *q; // Kuyruk
	NODE *val; // Kuyruktan cekilen eleman
	NODE *current; // Linkli liste uzerinde gezinti icin
	int *visited; // Visited dizisi
	int	*parent; // Yol bulmak icin parent dizisi
	int key; // Hash key degeri;
	char found = false; // Aktor bulundu mu
	unsigned long long int visitedKey; // Visited dizisi icin indis degiskeni
	unsigned long long int parentKey; // Parent dizisi icin indis degiskeni
	visited = malloc(M * sizeof(int));
	parent = malloc(M * sizeof(int));
	zeroVisited(visited);
	zeroVisited(parent);
	q = malloc(sizeof(QUEUE));
	q->front = 0;
	q->rear = -1;
	key = searchFromHash(actor1, list);
	enqueue(q, list[key]);

	if (key < 0) {
		printf("Boyle bir aktor bulunmamakta.");
		return;
	}
	visited[key] = 1;
	while (!isEmpty(q) && !found) {
		val = dequeue(q);
		//printf("%s\n", val->name);
		if (strcmp(val->name, actor2) == 0) {
			findPath(parent,actor1,actor2,list);
			found = true;
		}
		else {
			current = val;
			visitedKey = current->next->hash;
			parentKey = current->hash;
			while (current->next != NULL) {
				if (visited[visitedKey] == 0) {
					enqueue(q, list[visitedKey]);
					visited[visitedKey] = 1;
					parent[visitedKey] = parentKey;
				}
				current = current->next;
				if (current->next != NULL)
					visitedKey = current->next->hash;
			}
		}
	}
}


int main() {
	char option; // Menu secim
	char file_str[MAX_DOC_NAME]; // Dosya ismi
	char actor_buffer[MAX_NAME_LEN]; // Aktor 1 stringi
	char actor_buffer2[MAX_NAME_LEN]; // Aktor 2 stringi
	char graph_created = false;
	FILE *file; // File pointer
	NODE **list; // Graf
	strcpy(file_str, "input-mpaa.txt");
	list = malloc(M * sizeof(NODE*));
	nullList(list);
	while (true) {
		fflush(stdin);
		clearScreen();
		printf("----------------------------------------BACON SAYISI------------------------------------\n\n");
		printf("Lutfen istenen rakamlari kullanarak bir islem seciniz.\n");
		printf("1 - Graf Olustur (input-mpaa.txt)\n");
		printf("2 - Kevin Bacon Sayisi bul\n");
		printf("3 - Artistler arasi mesafe bul\n");
		printf("0 - Programi sonlandir\n");
		option = getch();
		switch (option) {
		case '1':
			clearScreen();
			if (graph_created == true) {
				printf("Graf zaten olusturuldu.");
			}
			else{
				printf("Graf olusturuluyor...\n");
				file = fopen(file_str, "r");
				readFile(file, list);
				fclose(file);
				//printHashTable(list); // Comment kaldirilarak tum tablodaki elemanlarin yazdirilmasi saglanir.
				graph_created = true;
			}

			printf("\nMenuye donmek icin '9' tusuna basiniz.");
			while (getch() != '9')
				printf("");
			break;
		case '2':
				clearScreen();
			if (graph_created == false) {
				printf("\nLutfen once grafi olusturunuz.\n");
			}
			else {
				printf("Lutfen Bacon sayisini aradiginiz aktoru giriniz. (soyadi, adi) seklinde\n");
				scanf(" %[^\n]s", actor_buffer);
				printf("\n");
				findConnection(actor_buffer, "Bacon, Kevin", list);
			}
			printf("\nMenuye donmek icin '9' tusuna basiniz.");
			while (getch() != '9')
				printf("");
			break;
		case '3':
			clearScreen();
			if (graph_created == false) {
				printf("\nLutfen once grafi olusturunuz.\n");
			}
			else {
				printf("Lutfen uzaklik aradiginiz 1.aktoru giriniz. (soyadi, adi) seklinde\n");
				scanf(" %[^\n]s", actor_buffer);
				printf("Lutfen uzaklik aradiginiz 2.aktoru giriniz. (soyadi, adi) seklinde\n");
				scanf(" %[^\n]s", actor_buffer2);
				printf("\n");
				findConnection(actor_buffer, actor_buffer2, list);
			}
			printf("\nMenuye donmek icin '9' tusuna basiniz.");
			while (getch() != '9')
				printf("");
			break;
		case '0':
			exit(0);
			break;
		default:
			break;
		}
	}
}
