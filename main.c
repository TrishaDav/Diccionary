
#include <stdio.h>
#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define EMPTY_POINTER -1
#define DATA_BLOCK_NAME 50

typedef struct Entity
{
    char name[DATA_BLOCK_NAME];
    long dataPointer;
    long atributesPointer;
    long nextEntity;
}ENTITY;

typedef struct Atribute {
    char name[DATA_BLOCK_NAME];
    long size;
    long nextAtribute;
} ATRIBUTE;

FILE* initializeDataDictionary(const char *dictionaryName) {
    long mainHeader = EMPTY_POINTER;
    FILE *dictionary = fopen(dictionaryName, "w+");
    if (dictionary == NULL) return NULL;
    fwrite(&mainHeader, sizeof(mainHeader), 1, dictionary);
    return dictionary;
}

int appendEntity(FILE* dataDictionary, ENTITY newEntity) {
    fseek(dataDictionary, 0, SEEK_END);
    long entityDirection = ftell(dataDictionary);
    fwrite(newEntity.name, DATA_BLOCK_NAME, 1, dataDictionary);
    fwrite(&newEntity.dataPointer, sizeof(long), 1, dataDictionary);
    fwrite(&newEntity.atributesPointer, sizeof(long), 1, dataDictionary);
    fwrite(&newEntity.nextEntity, sizeof(long), 1, dataDictionary);
    return entityDirection;
}

void reorderEntities(FILE* dataDictionary, ENTITY newEntity, long newEntityDirection) {
    long entityDataPointer = 0;
    long entityDirection;
    ENTITY currentEntity;
    fseek(dataDictionary, entityDataPointer, SEEK_SET);
    fread(&entityDirection, sizeof(long), 1, dataDictionary);
    while (entityDirection != -1) {
        fseek(dataDictionary, entityDirection, SEEK_SET);
        fread(&currentEntity.name, DATA_BLOCK_NAME, 1, dataDictionary);
        fread(&currentEntity.dataPointer, sizeof(long), 1, dataDictionary);
        fread(&currentEntity.atributesPointer, sizeof(long), 1, dataDictionary);
        entityDataPointer = ftell(dataDictionary);
        fread(&currentEntity.nextEntity, sizeof(long), 1, dataDictionary);
        entityDirection = currentEntity.nextEntity;
    }
    fseek(dataDictionary, entityDataPointer, SEEK_SET);
    fwrite(&newEntityDirection, sizeof(long), 1, dataDictionary);
}

void changeEntityName(FILE* dataDictionary, char currentName[50]) {
    ENTITY entity;
    fseek(dataDictionary, 0, SEEK_SET);
    while (fread(&entity, sizeof(ENTITY), 1, dataDictionary)) {
        if (strcmp(entity.name, currentName) == 0) {
            printf("Ingrese el nuevo nombre de la entidad: ");
            char newName[50];
            fgets(newName, sizeof(newName), stdin);
            newName[strcspn(newName, "\n")] = '\0';
            fseek(dataDictionary, -sizeof(ENTITY), SEEK_CUR);
            strcpy(entity.name, newName);
            fwrite(&entity, sizeof(ENTITY), 1, dataDictionary);
            printf("Entity name changed\n");
            return;
        }
    }
    printf("Entity not found\n");
}

void addatributeToEntity(FILE* dataDictionary, char entityName[50]) {
    ENTITY entity;
    ATRIBUTE newatribute;
    fseek(dataDictionary, 0, SEEK_SET);
    while (fread(&entity, sizeof(ENTITY), 1, dataDictionary)) {
        if (strcmp(entity.name, entityName) == 0) {
            printf("Name atribute:");
            fgets(newatribute.name, sizeof(newatribute.name), stdin);
            newatribute.name[strcspn(newatribute.name, "\n")] = '\0';
            printf("Size of atribute: ");
            scanf("%ld", &newatribute.size);
            getchar();
            newatribute.nextAtribute = EMPTY_POINTER;
            fseek(dataDictionary, 0, SEEK_END);
            fwrite(&newatribute, sizeof(ATRIBUTE), 1, dataDictionary);
            printf("Action performed\n");
            return;
        }
    }
    printf("Entity not found\n");
}

void deleteatributeFromEntity(FILE* dataDictionary, char entityName[50]) {
    ENTITY entity;
    ATRIBUTE atribute;
    long previousPointer = EMPTY_POINTER;
    long currentPointer;
    fseek(dataDictionary, 0, SEEK_SET);
    while (fread(&entity, sizeof(ENTITY), 1, dataDictionary)) {
        if (strcmp(entity.name, entityName) == 0) {
            currentPointer = entity.atributesPointer;
            while (currentPointer != EMPTY_POINTER) {
                fseek(dataDictionary, currentPointer, SEEK_SET);
                fread(&atribute, sizeof(ATRIBUTE), 1, dataDictionary);
                if (strcmp(atribute.name, entityName) == 0) {
                    if (previousPointer == EMPTY_POINTER) {
                        entity.atributesPointer = atribute.nextAtribute;
                    } else {
                        fseek(dataDictionary, previousPointer, SEEK_SET);
                        fwrite(&atribute.nextAtribute, sizeof(long), 1, dataDictionary);
                    }
                    printf("Atributo eliminado con éxito.\n");
                    return;
                }
                previousPointer = currentPointer;
                currentPointer = atribute.nextAtribute;
            }
            printf("Atribute not found\n");
            return;
        }
    }
    printf("Entity not found\n");
}

void printAttributeNames(FILE* dataDictionary, long attributesPointer) {
    ATRIBUTE attribute;
    if (attributesPointer == EMPTY_POINTER) {
        printf("Empty\n");
        return;
    }
    printf("  Atributes:\n");
    while (attributesPointer != EMPTY_POINTER) {
        fseek(dataDictionary, attributesPointer, SEEK_SET);
        fread(&attribute, sizeof(ATRIBUTE), 1, dataDictionary);
        printf("    - %s\n", attribute.name);
        attributesPointer = attribute.nextAtribute;
    }
}

void printEntitiesWithAttributeNames(FILE* dataDictionary) {
    ENTITY entity;
    long address = EMPTY_POINTER;
    rewind(dataDictionary);
    fread(&address, sizeof(long), 1, dataDictionary);
    if (address == EMPTY_POINTER) {
        printf("Empty\n");
        return;
    }
    printf("\n--- Data dictionary ---\n");
    while (address != EMPTY_POINTER) {
        fseek(dataDictionary, address, SEEK_SET);
        fread(&entity, sizeof(ENTITY), 1, dataDictionary);
        printf("Entity: %s\n", entity.name);
        printAttributeNames(dataDictionary, entity.atributesPointer);
        printf("\n");
        address = entity.nextEntity;
    }
    printf("--- End  ---\n");
}

int main(int argc, char** argv) {
    int menuOption, subMenuOption, entityOption;
    char dictionaryName[50], entityName[50], atributeName[50];
    ENTITY newEntity;
    FILE* dataDictionary;

    do {
        printf("\n--- Data Dictionary Menu ---\n");
        printf("1. Create New Dictionary\n");
        printf("2. Open Dictionary\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &menuOption);
        getchar(); 

        //Menu principal
        switch (menuOption) {
        case 1:
            printf("\nEnter the name of the new dictionary: ");
            fgets(dictionaryName, sizeof(dictionaryName), stdin);
            dictionaryName[strcspn(dictionaryName, "\n")] = '\0';

            initializeDataDictionary(dictionaryName); 
            printf("Dictionary '%s' created successfully!\n", dictionaryName);
            break;

        case 2:
            printf("\nEnter the name of the dictionary to open: ");
            fgets(dictionaryName, sizeof(dictionaryName), stdin);
            dictionaryName[strcspn(dictionaryName, "\n")] = '\0';

            printf("\n--- Dictionary Options ---\n");
            printf("1. Add Entity\n");
            printf("2. Modify Entity\n");
            printf("3. Delete Entity\n");
            printf("4. Print Dictionary\n");
            printf("5. Navigate Entities\n");
            printf("Enter your choice: ");
            
            scanf("%d", &subMenuOption);
            getchar(); 

           //Sub menu principal
            switch (subMenuOption) {
            case 1:
                printf("Add new entity\n");
                printf("\nEnter the entity name: ");
                fgets(entityName, sizeof(entityName), stdin);
                entityName[strcspn(entityName, "\n")] = '\0';
                newEntity.dataPointer = EMPTY_POINTER;
                newEntity.atributesPointer = EMPTY_POINTER;
                newEntity.nextEntity = EMPTY_POINTER;

                FILE* dataDictionary = fopen(dictionaryName, "r+"); 
                if (dataDictionary == NULL) {
                    printf("Error opening dictionary file.\n");
                    return 1; 
                }

                long entityDirection = appendEntity(dataDictionary, newEntity);
                reorderEntities(dataDictionary, newEntity, entityDirection); 
            
            //Menu de entidades
            case 2:
                printf("Ingrese nombre de la entidad: ");
                fgets(entityName, sizeof(entityName), stdin);
                entityName[strcspn(entityName, "\n")] = '\0'; // Remover salto de línea

                printf("Modificar entidad\n");
                printf("1. Cambiar nombre\n");
                printf("2. Agregar atributo\n");
                printf("3. Eliminar atributo\n");
                printf("Ingrese su opción: ");
                scanf("%d", &entityOption);
                getchar(); // Limpiar buffer de entrada

                switch (entityOption) {
                    case 1:
                        changeEntityName(dataDictionary, entityName);
                        break;
                    case 2:
                        addatributeToEntity(dataDictionary, entityName);
                        break;
                    case 3:
                        deleteatributeFromEntity(dataDictionary, entityName);
                        break;
                    default:
                        printf("Opción no válida\n");
                }
                break;

            case 3:
                printf("Entity name: ");
                fgets(entityName, sizeof(entityName), stdin);
                entityName[strcspn(entityName, "\n")] = '\0'; 

                deleteEntity(dataDictionary, entityName);
                break;

            case 4:
                printf("Content of txt\n");
                printEntitiesWithAttributeNames(dataDictionary);
                break;

                break;
            case 5:
                printf("Navigating through entities...\n");
                // Llamar a la función correspondiente
                break;
            default:
                printf("Invalid option\n");
            }
        
        }
    } while (menuOption != 3);

    fclose(dataDictionary);  

    return 0;
}