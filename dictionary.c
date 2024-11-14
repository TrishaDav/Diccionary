#include "dictionary.h"

FILE* initializeDataDictionary(const char *dictionaryName) {
    long mainHeader =  EMPTY_POINTER;              //mainHeader es la cabesera.
    
    printf("Initializing Data Dictionary...\n");

    FILE *dictionary = fopen(dictionaryName,"w+");

    fwrite(&mainHeader, sizeof(mainHeader), 1, dictionary);
   
    return EXIT_SUCCESS;
}


int appendEntity(FILE* dataDictionary, ENTITY newEntity){

    fseek(dataDictionary, 0, SEEK_END);
    //this is the direction of the new Entity
    long entityDirection = ftell(dataDictionary);

    fwrite(newEntity.name, DATA_BLOCK_NAME, 1, dataDictionary);
    fwrite(&newEntity.dataPointer, sizeof(long), 1, dataDictionary);
    fwrite(&newEntity.atributesPointer, sizeof(long), 1, dataDictionary);
    fwrite(&newEntity.nextEntity, sizeof(long), 1, dataDictionary);


    return entityDirection;
}

void reorderEntities(FILE* dataDictionary, ENTITY newEntity, long newEntityDirection){
    long entityDataPointer= 0;
    long entityDirection;
    ENTITY currentEntity;

    fseek(dataDictionary, entityDataPointer,SEEK_SET);

    fread(&entityDirection,sizeof(long),1,dataDictionary);

    while(entityDirection != -1){
        fseek(dataDictionary, entityDirection,SEEK_SET);
        fread(&currentEntity.name,DATA_BLOCK_NAME,1,dataDictionary);
        fread(&currentEntity.dataPointer,sizeof(long),1,dataDictionary);
        fread(&currentEntity.atributesPointer,sizeof(long),1,dataDictionary);
        entityDataPointer= ftell(dataDictionary);
        fread(&currentEntity.nextEntity,sizeof(long),1,dataDictionary);
        entityDirection= currentEntity.nextEntity;
    }

    fseek(dataDictionary, entityDataPointer, SEEK_SET);
    fwrite(&newEntityDirection,sizeof(long), 1, dataDictionary);
}