#ifndef OSLINUX_UTIL_H
#define OSLINUX_UTIL_H

void safe_realloc(void** ptr,unsigned int size);
// Get size of file;
unsigned long get_fsize(char *file_path);

struct File{
    char *name;
    u_int64_t size;
};

void freeFile(void *File);

struct List{
    struct List *prev;
    void *data;
    struct List *next;
};

//возвращает 1 или 0 в зависимости от того, соответственно пустой или нет список
int isListEmpty(struct List *list);

//проверяет наличие объекта, равного по значению любому элементу списка, возвращает
//указатель на элемент или NULL, если такого нет
struct List * findElem(struct List *list, void *object, size_t size);

//вставляет object в list на позицию position(0 или 1 - начало, -1 - конец списка)
//возвращает указатель на вставленный элемент или NULL при ошибке
struct List * insertElem(struct List **list, int position, void *object, size_t size);

//удаляет элемент из списка list на позиции position(0 или 1 - начало, -1 - конец списка)
void removeElem(struct List **list, int position, void (*freeElem)(void *elem));

//очищает список
void removeList(struct List **list, void (*freeElem)(void *elem));

#endif //OSLINUX_UTIL_H
