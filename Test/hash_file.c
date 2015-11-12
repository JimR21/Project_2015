#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BF.h"

typedef struct {
	int id;
	char name[15];
	char surname[20];
	char status[1];
	char dateOfBirth[10];
	int salary;
	char section[1];
	int daysOff;
	int prevYears;
} Record;

typedef struct {
	int fileDesc;
	char attrType;
	char* attrName;
	int attrLength;
	int depth;
} HT_info;

void printHT_info(HT_info);

int hashFunction(Record, int, int);

long int charToInt(char*);

void printRecord(Record);

void printSituation(HT_info*);

int insertRecord_overflow_2(HT_info*, Record, int, int);

int createOverflowBucket(HT_info*, Record, int);

unsigned fnv_hash ( void *key, int len );

int OVERFLOW_BUCKETS_NUM = 0;		//arithmos twn overflow block


int HT_CreateIndex(char *fileName, char attrType, char* attrName, int attrLength, int depth) {
	HT_info info;
	int temp, i;
	char fileType = '2';			//2-> arxeio hash
	void* ptr = &fileType;
	void* block;

	BF_Init();

	if (BF_CreateFile(fileName) < 0) return -1;

	info.fileDesc = BF_OpenFile(fileName);
	if(info.fileDesc < 0) return -2;
	info.attrType = attrType;
	info.attrName = malloc(attrLength+1);		//to attrLength DEN metraei ton termatiko xaraktera tou string
	strcpy(info.attrName, attrName);
	info.attrLength = attrLength;
	info.depth = depth;

	int bucketsNum = 1;
	for(i=0;i<depth;i++) bucketsNum = bucketsNum*2;		//bucketsNum = 2^depth
	//printf("\nbuckets = %d\n", bucketsNum);

	if(BF_AllocateBlock(info.fileDesc)) return -3;
	if(BF_ReadBlock(info.fileDesc, 0, &block) < 0) return -4;		//arxikopoihsh prwtou block

	memcpy(block, ptr, 1);					//adigrafi pliroforias arxeiou
	block++;
	memcpy(block, &info, sizeof(HT_info));

	//free(info.attrName);
	if(BF_WriteBlock(info.fileDesc, 0) < 0) return -5;

	if(BF_AllocateBlock(info.fileDesc)) return -6;				//2o block gia to evretirio
	if(BF_ReadBlock(info.fileDesc, 1, &block) < 0) return -7;
	int* index = malloc(bucketsNum*sizeof(int));
	for(i=0;i<bucketsNum;i++) index[i] = i+2;					//arxikopoihsh tou evretiriou
	memcpy(block, index, bucketsNum*sizeof(int));
	if(BF_WriteBlock(info.fileDesc, 1) < 0) return -8;

	//free(index);
	//BF_ReadBlock(info.fileDesc, 1, &block);

	for(i=0;i<bucketsNum;i++) {		//desmeusi kai arxikopoihsei twn buckets/blocks
		if(BF_AllocateBlock(info.fileDesc)) return -9;
		if(BF_ReadBlock(info.fileDesc, i+2, &block) < 0) return -10;
		//if(i != BF_GetBlockCounter(info.fileDesc)-1-2) {//printf("STOP\n");getchar();}

		temp = depth;		//topiko vathos = oliko vathos
		ptr = &temp;
		memcpy(block, ptr, sizeof(int));

		block += sizeof(int);

		temp = 0;		//arthmos eggrafwn, arxika einai 0
		ptr = &temp;
		memcpy(block, ptr, sizeof(int));

		if(BF_WriteBlock(info.fileDesc, i+2) < 0) return -11;
	}
	//getchar();

	if(BF_CloseFile(info.fileDesc) < 0) return -12;

	return 0;
}


HT_info* HT_OpenIndex(char* fileName) {
	char fileType;
	void* block;
	HT_info* info;

	int fileDesc = BF_OpenFile(fileName);
	if(fileDesc < 0) return NULL;

	if(BF_ReadBlock(fileDesc, 0, &block) < 0) return NULL;

	memcpy(&fileType, block, 1);
	if(fileType != '2') return NULL;

	block++;
	memcpy(info, block, sizeof(HT_info));

	return info;
}

int HT_CloseIndex(HT_info* header_info) {
	void* block;
	HT_info* info = malloc(sizeof(HT_info));
	int* index;

	if(BF_ReadBlock(header_info->fileDesc, 0, &block) < 0) return -13;
	block++;
	memcpy(info, block, sizeof(HT_info));

	free(info->attrName);
	free(info);

	int i, bucketsNum = 1;
	for(i=0;i<bucketsNum;i++) bucketsNum = bucketsNum*2;
	index = malloc(bucketsNum*sizeof(int));
	if(BF_ReadBlock(header_info->fileDesc, 0, &block) < 0) return -14;
	memcpy(index, block, bucketsNum*sizeof(int));
	free(index);

	if(BF_CloseFile(header_info->fileDesc) < 0) return -15;
	return 0;
}

int HT_InsertEntry(HT_info* header_info, Record record) {
	int i, j, bucketsNum = 1;
	int fieldNum;
	int* recordNum;
	int* temp;
	void* block;
	for(i=0;i<header_info->depth;i++) bucketsNum = bucketsNum*2;		//bucketsNum = 2^depth

	if(!strcmp(header_info->attrName, "id")) {
		fieldNum = 1;
	} else if(!strcmp(header_info->attrName, "name")) {
		fieldNum = 2;
	} else if(!strcmp(header_info->attrName, "surname")) {
		fieldNum = 3;
	} else if(!strcmp(header_info->attrName, "status")) {
		fieldNum = 4;
	} else if(!strcmp(header_info->attrName, "dateOfBirth")) {
		fieldNum = 5;
	} else if(!strcmp(header_info->attrName, "salary")) {
		fieldNum = 6;
	} else if(!strcmp(header_info->attrName, "section")) {
		fieldNum = 7;
	} else if(!strcmp(header_info->attrName, "daysOff")) {
		fieldNum = 8;
	} else if(!strcmp(header_info->attrName, "prevYears")) {
		fieldNum = 9;
	}
	int destBucket = hashFunction(record, header_info->depth, fieldNum);		//se poio bucket tha mpei i eggrafi
	//printf("i eggrafi tha eisaxthei sto bucket %d\n", destBucket);

	int insert = insertRecord(header_info, record, destBucket);
	if(insert == 0) return 0;		//i eisagwgi oloklirwthike
	else {							//uperxeilusi -> koitazw tis 2 periptwseis
		int* recordsNum;

		if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) return -21;
		int* index = block;

		if(BF_ReadBlock(header_info->fileDesc, index[destBucket], &block) < 0) return -1;	//diavazw to topiko vathos tou block pou tha eisagw
		temp = block;							//o temp deixnei sto topiko vathos tou destBucket
		if(*temp < header_info->depth) {		//topiko vathos < oliko -> spaw ta filarakia kai eisagw tin eggrafi
			//printSituation(header_info);
			//printf("YPERXEILUSI 1 -> SPAW FILARAKIA\n");
			//getchar();

			insert = insertRecord_overflow(header_info, record, destBucket, fieldNum);
			if(insert == 0) return 0;
			else if(insert != 0) {				// paw stin 2i periptwsi uperxeilusis
				//printf("insertRecord_overflow, insert = %d\n", insert);
				insert = insertRecord_overflow_2(header_info, record, destBucket, fieldNum);
				//printf("apo overflow_2 me pateda, insert = %d\n", insert);
				//getchar();
				return insert;
			}
		} else {	//to oliko vathos au3anetai -> diplasiasmos twn kadwn

			insert = insertRecord_overflow_2(header_info, record, destBucket, fieldNum);
			//printf("insertRecord_overflow_2, insert = %d\n", insert);
			//getchar();
			return insert;
		}
	}

	return 555;	//error!
}

int insertRecord(HT_info* header_info, Record record, int destBucket) {
	void* block;

	if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) return -405;
	int* index = block;

	if(BF_ReadBlock(header_info->fileDesc, index[destBucket], &block) < 0) return -37;
	void* block_ptr = block;

	block += sizeof(int);
	int* temp = block;
	//printf("\narithmos eggrafwn %dou block = %d\n", index[destBucket],*temp);
	if(*temp < BLOCK_SIZE/sizeof(Record)) {	//kane eisagwgi
		block += sizeof(int);				//prospernaw ton int me ton arithmo eggrafwn
		block += (*temp)*sizeof(Record);	//prospernaw tin uparxouses eggrafes gia na grapsw tin kainouria
		memcpy(block, &record, sizeof(Record));

		(*temp)++;					//o neos arithmos eggrafwn tou block
		block_ptr += sizeof(int);	//i arxi tou block pou tha deixnei ston 2o int
		BF_WriteBlock(header_info->fileDesc, index[destBucket]);
		return 0;
	} else return -38;		//to bucket einai gemato
}

int insertRecord_overflow(HT_info* header_info, Record record, int destBucket, int fieldNum) {
	int i;
	int* recordsNum;
	int* temp;
	void* block;
	int bucketsNum = 1;
	for(i=0;i<header_info->depth;i++) bucketsNum = bucketsNum*2;	//arithmos deiktwn

	if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) return -410;
	int* index = block;

	//printf("i uperxeilusi egine sto bucket: %d ,\t pou einai sto block: %d\n", destBucket, index[destBucket]);
	if(BF_ReadBlock(header_info->fileDesc, index[destBucket], &block) < 0) return -39;
	int* localDepth = block;		//pairnw to vathos, 3erw oti d'<d
	recordsNum = block + sizeof(int);

	Record* recordsList = malloc((*recordsNum+1)*sizeof(Record));	//xwros gia tin eggrafi pou tha eisagoume

	int count = 0;
	block += 2*sizeof(int);		//o block deixnei stin 1i eggrafi
	////printf("recordsNum = %d\n", *recordsNum);
	//getchar();
	for(i=0;i<(*recordsNum);i++) {			//apothikeuw tis eggrafes tou bucket gia na tis 3anaeisagw
		memcpy(recordsList+i, block, sizeof(Record));
		//printRecord(recordsList[i]);
		block += sizeof(Record);
		count++;
	}
	////printf("adigrafikan %d eggrafes\n", count);
	recordsList[*recordsNum] = record;	//i nea eggrafi pou theloume na eisagoume
	int totalRecords = (*recordsNum) + 1;

	if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) return -40;
	index = block;				//o index deixnei ston pinaka deiktwn
	int pointer = 0;
	for(i=0;i<bucketsNum;i++) {
		if(index[i] == index[destBucket]) {
			pointer = i;
			break;
		}
	}

	if(BF_ReadBlock(header_info->fileDesc, index[destBucket], &block) < 0) return -39;
	localDepth = block;		//pairnw to vathos, 3erw oti d'<d

	int pointersNum = 1;
	for(i=0;i<header_info->depth-(*localDepth);i++) pointersNum = pointersNum*2;	//arithmos deiktwn pou deixnoun ston kado uperxeilusis


	if(BF_ReadBlock(header_info->fileDesc, index[destBucket], &block) < 0) return -476;
	localDepth = block;
	int tmp = *localDepth;
	char adeiasma = 0;
	for(i=0;i<BLOCK_SIZE;i++) {		//midenizw to block tis palias thesis tou prwtou block uperxeilusis
		memcpy(block, &adeiasma, 1);
		block ++;
	}
	if(BF_WriteBlock(header_info->fileDesc, index[destBucket]) < 0) return -94;
	if(BF_ReadBlock(header_info->fileDesc, index[destBucket], &block) < 0) return -477;
	localDepth = block;
	*localDepth = tmp;
	if(BF_WriteBlock(header_info->fileDesc, index[destBucket]) < 0) return -942;
	//getchar();

	if(BF_AllocateBlock(header_info->fileDesc)) return -3;
	//printf("last block = %d\n", BF_GetBlockCounter(header_info->fileDesc)-1);

	//metakinw ta block uperxeilusis sto telos kai eleutherwnw to block gia eggrafes
	if(OVERFLOW_BUCKETS_NUM > 0) {
		//printf("HAZARD overflow buckets = %d\n", OVERFLOW_BUCKETS_NUM);
		//getchar();

		int lastBlock = BF_GetBlockCounter(header_info->fileDesc)-1;
		int t = lastBlock-OVERFLOW_BUCKETS_NUM;
		for(i=t;i<lastBlock;i++) {		//gia ola ta block uperxeilusis
			if(BF_ReadBlock(header_info->fileDesc, i, &block) < 0) return -23;
			recordsNum = block;			//o arithmos eggrafwn tou block uperxeilusis
			if(*recordsNum >= BLOCK_SIZE/sizeof(Record)) {		//ama exw >6 eggrafes prepei na metaferw kai ton 2o kado an exw
				temp = block + sizeof(int) + (*recordsNum)*sizeof(Record);	//o temp deixnei sto telos twn eggrafwn
				if(*temp != 0) {				//exw kai 2o kado uperxeilusis
					(*temp) ++;					//tha metaferthei kata 1 block meta to epomeno block tis alusidas
				}
			}
			if(BF_WriteBlock(header_info->fileDesc, i) < 0) return -99;
		}


		//allazw to block pou tha deixnei to prwto block tis alusidas uperxeilusis (an exw uperxeilusi)
		void* block1;
		if(BF_ReadBlock(header_info->fileDesc, 0, &block1) < 0) return -239;
		block1 += 100;
		int* overflowNum = block1;
		int tmp = *overflowNum;
		block1 += sizeof(int);
		temp = block1;			//deixnei stin arxi twn apothikeumenwn arithmwn block
		for(i=0;i<tmp;i++) {
			if(BF_ReadBlock(header_info->fileDesc, *temp, &block) < 0) BF_PrintError("gfdv");
			temp += sizeof(int);
			block += sizeof(int);
			overflowNum = block1 +i*sizeof(int);
			if(BF_ReadBlock(header_info->fileDesc, *overflowNum, &block) < 0) BF_PrintError("fdesc");
			recordsNum = block;
			int* nea_alusida = block + 2*sizeof(int) + (*recordsNum)*sizeof(Record);
			(*nea_alusida)++;
			BF_WriteBlock(header_info->fileDesc, *overflowNum);
		}

		//metakinw ola ta block uperxeilusis 1 block de3ia na pane sto telos tou arxeiou
		void* block_2;
		for(i=0;i<OVERFLOW_BUCKETS_NUM;i++) {
			//printf("adigrafi tou block %d -> %d\n", lastBlock-1-i,lastBlock-i);
			//getchar();
			if(BF_ReadBlock(header_info->fileDesc, lastBlock-i, &block_2) < 0) BF_PrintError("ox");
			if(BF_ReadBlock(header_info->fileDesc, lastBlock-1-i, &block) < 0) BF_PrintError("ax");
			memcpy(block_2, block, BLOCK_SIZE);

			if(BF_WriteBlock(header_info->fileDesc, lastBlock-i) < 0) BF_PrintError("ix");
		}

		//midenizw to block prin ta block uperxeilusis gia to spasimo sta filarakia, einai bucket
		if(BF_ReadBlock(header_info->fileDesc, t, &block) < 0) BF_PrintError("ex");
		////printf("midenismos: t = %d,\ttotal blocks=%d,\toverflowblocks=%d\n", t,BF_GetBlockCounter(header_info->fileDesc)-1,OVERFLOW_BUCKETS_NUM);

		char adeiasma = 0;
		for(i=0;i<BLOCK_SIZE;i++) {		//midenizw to block tis palias thesis tou prwtou block uperxeilusis
			memcpy(block, &adeiasma, 1);
			block ++;
			count++;
		}
		if(BF_WriteBlock(header_info->fileDesc, t) < 0) return -94;
	}

	//printf("pointer = %d\n", pointer);
	//tropopoiw to evretirio kai arxikopoiw to neo block

	if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) return -410;
	index = block;

	if(BF_ReadBlock(header_info->fileDesc, index[pointer], &block) < 0) return -421;
	localDepth = block;
	int initializeDepth = *localDepth;

	int newBlockNum = BF_GetBlockCounter(header_info->fileDesc)-1-OVERFLOW_BUCKETS_NUM;
	if(BF_ReadBlock(header_info->fileDesc, newBlockNum, &block) < 0) return -420;
	localDepth = block;
	(*localDepth) = initializeDepth+1;	//tha auxithei to local depth logw tou spasimatos tou kadou
	temp = block + sizeof(int);
	BF_WriteBlock(header_info->fileDesc, newBlockNum);
	//printf("to vathos tou neou bucket (block %d) einai: %d\teggrafes = %d\n", newBlockNum,initializeDepth+1,*temp);

	int mpika = 0;
	for(i=0;i<pointersNum;i++) {
		void* block2;
		if(BF_ReadBlock(header_info->fileDesc, 1, &block2) < 0) return -410;
		index = block2;
		if(i >= pointersNum/2) index[pointer+i] = newBlockNum;
		else if(mpika == 0) {
			mpika = 1;
			////printf("pointersNum = %d\t, spasimo deiktwn: pointer+i = %d\n", pointersNum,pointer+i);
			if(BF_ReadBlock(header_info->fileDesc, index[pointer+i], &block) < 0) return -41;
			localDepth = block;
			(*localDepth)++;		//spaw ta filarakia au3anetai to d'
			////printf("to local depth tou block %d einai: %d\n", index[pointer+i],*localDepth);
			recordsNum = block + sizeof(int);
			(*recordsNum) = 0;		//adeiazw ta block gia eisagwgi twn eggrafwn me d'+1

			BF_WriteBlock(header_info->fileDesc, index[pointer+i]);
		}
	}

	//printSituation(header_info);
	//getchar();

	int insert;
	//printf("eisagwgi eggrafwn sta buckets,\tarithmos eggrafwn = %d\n", totalRecords);
	//getchar();
	for(i=0;i<totalRecords;i++) {
		destBucket = hashFunction(recordsList[i], header_info->depth, fieldNum);		//se poio bucket tha mpei i eggrafi
		//printf("i eggrafi tha XANAeisaxthei sto bucket %d\n", destBucket);
		insert =  insertRecord(header_info, recordsList[i], destBucket);
		//printf("i epanaeisagwgi tis eggrafis #%d: insert = %d\n", i,insert);

		if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) return -418;
		index = block;
		if(insert != 0) {	//i teleutea eggrafi den xwrese sto bucket, 3anatsekarw tin periptwsi d'<d alliws 2siasmos twn deiktwn
			if(BF_ReadBlock(header_info->fileDesc, index[destBucket], &block) < 0) return -42;
			localDepth = block;
			if(*localDepth < header_info->depth) {
				//printf("KALW ANADROMIKA TIN OVERFLOW\n");
				insert = insertRecord_overflow(header_info, recordsList[i], destBucket, fieldNum);	//prepei na einai i  teleutaia eggrafi
				//printf("insertRecord_overflow,  insert = %d\n", insert);
				//getchar();
			} else { 	//2i periptwsi uperxeilusis
				free(recordsList);
				return -2;
			}

			if(insert != 0) { 	//i insertRecord_overflow apetuxe, paw stin 2i periptwsi uperxeilusis
				free(recordsList);
				return -2;
			}
		}

	}

	//printSituation(header_info);
	//getchar();

	free(recordsList);
	return 0;
}



int insertRecord_overflow_2(HT_info* header_info, Record record, int destBucket, int fieldNum) {
	void* block;
	int bucketsNum = 1;
	int i, j, insert;
	int* index;
	int* temp;
	int* recordsNum;

	//printSituation(header_info);
	//printf("YPERXEILUSI 2 -> 2PLASIASMOS PINAKA\n");
	//getchar();

	for(i=0;i<header_info->depth;i++) bucketsNum = bucketsNum*2;

	if(header_info->depth < 7) {

		if(BF_ReadBlock(header_info->fileDesc, 0, &block) < 0) return -16;
		HT_info* ht_infoPtr = block+1;
		(ht_infoPtr->depth)++;		//to oliko vathos au3anetai
		(header_info->depth)++;		//enimerwnw kai to orisma tis sinartisis, TSEKARISMA
		BF_WriteBlock(header_info->fileDesc, 0);


		//orizw to neo euretirio
		if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) return -21;
		index = block;		//prospernaw ton int me ton arithmo twn kadwn uperxeilusis
		int* newIndex = malloc(2*bucketsNum*sizeof(int));
		for(i=0;i<bucketsNum;i++) {		//arxikopoiw tous deiktes tou newIndex
			newIndex[2*i] = index[i];
			newIndex[2*i+1] = index[i];
		}
		//printf("2plasiazw to euretirio, bucketsNum = %d\n", bucketsNum);
		//getchar();
		memcpy(block, newIndex, 2*bucketsNum*sizeof(int));		//apothikeuw to neo euretirio
		BF_WriteBlock(header_info->fileDesc, 1);

		//printSituation(header_info);
		//getchar();


		//3amadokimazw tin insert_overflow me tous diplasious deiktes
		//printf("mpainw stin insert overflow\n");
		//getchar();
		destBucket = 2*destBucket;		//ta diplasiasa pio panw
		insert = insertRecord_overflow(header_info, record, destBucket, fieldNum);
		//printf("insertRecord_overflow,\tinsert = %d\n", insert);
		if(insert != 0) {
			//anadromika 3anadiplasiazw tou deiktes

			if(header_info->depth < 7) {
				insert = insertRecord_overflow_2(header_info, record, destBucket, fieldNum);
				//printf("insertRecord_overflow_2, insert = %d\n", insert);
				//getchar();
			} else {		//dimiorgia kadou uperxeilusis
				int tol = createOverflowBucket(header_info, record, destBucket);
				//printf("createOverflowBucket, egine = %d\n", tol);
				return tol;
			}
		}
		return 0;
	} else {
		int tol = createOverflowBucket(header_info, record, destBucket);
		//printf("createOverflowBucket, egine = %d\n", tol);
		//getchar();
		return tol;
	}
}


int createOverflowBucket(HT_info* header_info, Record record, int destBucket) {
	void* block;
	int i, j;
	int* recordsNum;
	int* index;
	int* temp;

	//printf("ftiaxnw kado uperxeilusis\n");

	if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) return -217;
	index = block;

	//printf("to block pou egine i uperxeilusi: %d,\tarithmos bucket: %d\n", index[destBucket],destBucket);

	if(BF_ReadBlock(header_info->fileDesc, index[destBucket], &block) < 0) return -29; 	//isws den xreiazetai, to ekana prin
	int t = index[destBucket];
	recordsNum = block + sizeof(int);		//o recordsNum deixnei ston arithmo eggrafwn
	int* overflowBucket = block + 2*sizeof(int) + (*recordsNum)*sizeof(Record);		//prepei *recordsNum = 7(ama den itan gematos tha ginotan swsta i insert)
	void* overflowBucket_ptr;

	//printf("records num = %d, prepei na einai 7\toveflow bucket = %d\n", *recordsNum,*overflowBucket);
	//getchar();

	int mpika = 0;
	while(*overflowBucket != 0) {
		mpika = 1;
		if(BF_ReadBlock(header_info->fileDesc, *overflowBucket, &overflowBucket_ptr) < 0) return -30;
		block = overflowBucket_ptr;		//o block deixnei sto block uperxeilusis
		recordsNum = block;				//deixnei ston arithmo tou plithous twn eggrafwn
		block += sizeof(int);			//deixnei stin prwti eggrafi
		if(*recordsNum < BLOCK_SIZE/sizeof(Record)) {		//an exei xwro to overflow block eisagw tin eggrafi
			block += (*recordsNum)*sizeof(Record);
			memcpy(block, &record, sizeof(Record));		//adigrafw tin eggrafi sto block
			(*recordsNum)++;	//exw 1 parapanw eggrafi sto bucket
			if(BF_WriteBlock(header_info->fileDesc, *overflowBucket) < 0) return -31;

			/*char* c = overflowBucket_ptr;
			//printf("\n-------------------------\n");
			for(i=0;i<BLOCK_SIZE;i++) {
				//printf("%d ", *c);
				c++;
			}
			//printf("ektupwsi block uperxeilusisOUA\n");
			getchar();*/

			return 0;
		} else {				//elegxw an exw allo kado uperxeilusis stin alusida
			t = *overflowBucket;
			//printf("t = %d\n");
			//getchar();
			overflowBucket = overflowBucket_ptr + sizeof(int) + (*recordsNum)*sizeof(Record);
		}
	}
	//printf("go go go\n");
	//getchar();
	//an ftasw edw tha prepei na einai exw gemato to teleutaio block tis alusidas kai oxi allo kado uperxeilusis, kai *overflowBucket = 0
	//alliws ftiaxnw neo block uperxeilusis(sto telos tou arxeiou) kai to sindew me to palio
	if(*overflowBucket == 0) {		//prepei na einai 0
		OVERFLOW_BUCKETS_NUM++;
		//printf("arithmos overflow buckets = %d\n", OVERFLOW_BUCKETS_NUM);
		//getchar();
		if(BF_ReadBlock(header_info->fileDesc, t, &block) < 0) return -297;
		recordsNum = block + sizeof(int);
		temp = block + 2*sizeof(int) + (*recordsNum)*sizeof(Record);
		*temp = BF_GetBlockCounter(header_info->fileDesc);		//to neo block uperxeilusis
		if(BF_WriteBlock(header_info->fileDesc, t) < 0) return -310;

		if(mpika == 0) {
			if(BF_ReadBlock(header_info->fileDesc, 0, &block) < 0) return -227;
			block += 100;			//to 100 einai authaireti timi!
			int* overflowBlocksNum = block;
			block += sizeof(int);
			temp = block + (*overflowBlocksNum)*sizeof(int);
			*temp = t;				//apothikeuw ta blocks pou exoun kado uperxeilusis
			(*overflowBlocksNum)++;
			BF_WriteBlock(header_info->fileDesc, 0);
		}

		if(BF_AllocateBlock(header_info->fileDesc)) return -32;
		int lastBlock = BF_GetBlockCounter(header_info->fileDesc)-1;
		if(BF_ReadBlock(header_info->fileDesc, lastBlock, &block) < 0) return -33;
		j = 1;			//to neo block tha exei 1 eggrafi mesa
		recordsNum = &j;
		memcpy(block, recordsNum, sizeof(int));
		block += sizeof(int);
		memcpy(block, &record, sizeof(Record));
		if(BF_WriteBlock(header_info->fileDesc, lastBlock) < 0) return -34;
		//printSituation(header_info);
		//getchar();

		/*if(BF_ReadBlock(header_info->fileDesc, t, &block) < 0) return -353;
		char* c = block;
		//printf("\n-------------------------\n");
		for(i=0;i<BLOCK_SIZE;i++) {
			//printf("%d ", *c);
			c++;
		}
		//printf("ektupwsi block sunedesis,\tblock uperxeilusis = %d\n", t);
		getchar();

		if(BF_ReadBlock(header_info->fileDesc, lastBlock, &block) < 0) return -353;
		c = block;
		//printf("\n-------------------------\n");
		for(i=0;i<BLOCK_SIZE;i++) {
			//printf("%d ", *c);
			c++;
		}
		getchar();*/
	}

	return 0;
}



void HT_GetAllEntries(HT_info* header_info, void* value) {
	int i, j, temp, count = 0;
	int searchBlock, bucketNum;
	int blocksNo;
	void* block;
	char* value_ptr = value;
	int* index;
	int* recordsNum;
	int* overflowBucket;
	Record record;
	Record* record_ptr;

	blocksNo = BF_GetBlockCounter(header_info->fileDesc);
	//if(blocksNo < 0) {//printf("\nerror_1\n"); return;}

	printf("\n\n\n--------------EKTIPWNW--------------------\n\n\n");

	if(value == NULL) {
		int blocksNum = BF_GetBlockCounter(header_info->fileDesc)-1;
		for(i=2;i<blocksNum-OVERFLOW_BUCKETS_NUM+1;i++) {
			//printf("eggrafes tou block: %d\n", i);
			if(BF_ReadBlock(header_info->fileDesc, i, &block) < 0) BF_PrintError("xoubla");
			block += sizeof(int);
			recordsNum = block;
			block += sizeof(int);
			for(j=0;j<(*recordsNum);j++) {
				record_ptr = block;
				block += sizeof(Record);
				printRecord(*record_ptr);
				count++;
			}

			if(*recordsNum !=  BLOCK_SIZE/sizeof(Record)) continue;

			overflowBucket = block;

			if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) BF_PrintError("\nerror_02");
			index = block;

			while(*overflowBucket != 0) {
				 if(BF_ReadBlock(header_info->fileDesc, *overflowBucket, &block) < 0) BF_PrintError("\nerror_04");
				recordsNum = block;
				block += sizeof(int);
				for(j=0;j<*recordsNum;j++) {
					record_ptr = block;
					printRecord(*record_ptr);
					count++;
					block += sizeof(Record);
				}
				overflowBucket = block;		//deixnei meta to telos twn eggrafwn, an != 0, periexei # apo block uperxeilusis
			}
		}
		printf("#eggrafwn pou ektipwthikan: %d\n", count);
		return;
	}

	char* fieldName = header_info->attrName;
	printf("fieldName = %s\n", fieldName);

	if(!strcmp(fieldName, "id")) {
		int* value_ptr = malloc(sizeof(header_info->attrLength));
		printf("attrLength = %d,\t(prepei = 4)\n", header_info->attrLength);
		memcpy(value_ptr, value, header_info->attrLength);

		printf("ta pedia id pou psaxnw: %d\n", *value_ptr);

		record.id = *value_ptr;
		bucketNum = hashFunction(record, header_info->depth, 1);
		if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) BF_PrintError("\nerror_1");
		index = block;
		searchBlock = index[bucketNum];

		if(BF_ReadBlock(header_info->fileDesc, searchBlock, &block) < 0) BF_PrintError("\nerror_11");
		recordsNum = block + sizeof(int);
		block += 2*sizeof(int);

		for(i=0;i<*recordsNum;i++) {		//gia kathe eggrafi tou arxeiou
			record_ptr = block;

			if(record_ptr->id == record.id) {
				printRecord(*record_ptr);
				count++;
			}

			block += sizeof(Record);
		}
		if(*recordsNum !=  BLOCK_SIZE/sizeof(Record)) {
			printf("#eggrafwn pou ektipwthikan: %d\n", count);
			return;
		}

		overflowBucket = block;

		if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) BF_PrintError("\nerror_12");
		index = block;

		if(BF_ReadBlock(header_info->fileDesc, searchBlock, &block) < 0) BF_PrintError("\nerror_13");
		while(*overflowBucket != 0) {
			if(BF_ReadBlock(header_info->fileDesc, *overflowBucket, &block) < 0) BF_PrintError("\nerror_14");
			recordsNum = block;
			block += sizeof(int);
			for(i=0;i<*recordsNum;i++) {
				record_ptr = block;
				if(record_ptr->id == record.id) {
					printRecord(*record_ptr);
					count++;
				}
				block += sizeof(Record);
			}
			overflowBucket = block;
		}

	} else if(!strcmp(fieldName, "name")) {
		char* value_ptr = malloc(sizeof(header_info->attrLength+1));
		printf("attrLength = %d\n", header_info->attrLength);
		memcpy(value_ptr, value, header_info->attrLength);
		value_ptr[header_info->attrLength] = 0;		//termatikos xaraktiras
		printf("ta pedia name pou psaxnw: %s\n", value_ptr);

		strcpy(record.name, value_ptr);
		bucketNum = hashFunction(record, header_info->depth, 2);
		if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) BF_PrintError("\nerror_2");
		index = block;
		searchBlock = index[bucketNum];

		if(BF_ReadBlock(header_info->fileDesc, searchBlock, &block) < 0) BF_PrintError("\nerror_21");
		recordsNum = block + sizeof(int);
		block += 2*sizeof(int);

		for(i=0;i<*recordsNum;i++) {		//gia kathe eggrafi tou arxeiou
			record_ptr = block;

			if(!strcmp(record_ptr->name, record.name)) {
				printRecord(*record_ptr);
				count++;
			}

			block += sizeof(Record);
		}
		if(*recordsNum !=  BLOCK_SIZE/sizeof(Record)) {
			printf("#eggrafwn pou ektipwthikan: %d\n", count);
			return;
		}

		overflowBucket = block;

		if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) BF_PrintError("\nerror_22");
		index = block;

		if(BF_ReadBlock(header_info->fileDesc, searchBlock, &block) < 0) BF_PrintError("\nerror_23");
		while(*overflowBucket != 0) {
			if(BF_ReadBlock(header_info->fileDesc, *overflowBucket, &block) < 0) BF_PrintError("\nerror_24");
			recordsNum = block;
			block += sizeof(int);
			for(i=0;i<*recordsNum;i++) {
				record_ptr = block;
				if(!strcmp(record_ptr->name, record.name)) {
					printRecord(*record_ptr);
					count++;
				}
				block += sizeof(Record);
			}
			overflowBucket = block;
		}
	} else if(!strcmp(fieldName, "surname")) {
		char* value_ptr = malloc(sizeof(header_info->attrLength+1));
		printf("attrLength = %d\n", header_info->attrLength);
		memcpy(value_ptr, value, header_info->attrLength);
		value_ptr[header_info->attrLength] = 0;		//termatikos xaraktiras
		printf("ta pedia surname pou psaxnw: %s\n", value_ptr);

		strcpy(record.surname, value_ptr);
		bucketNum = hashFunction(record, header_info->depth, 2);
		if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) BF_PrintError("\nerror_3");
		index = block;
		searchBlock = index[bucketNum];

		if(BF_ReadBlock(header_info->fileDesc, searchBlock, &block) < 0) BF_PrintError("\nerror_31");
		recordsNum = block + sizeof(int);
		block += 2*sizeof(int);

		for(i=0;i<*recordsNum;i++) {		//gia kathe eggrafi tou arxeiou
			record_ptr = block;

			if(!strcmp(record_ptr->surname, record.surname)) {
				printRecord(*record_ptr);
				count++;
			}

			block += sizeof(Record);
		}
		if(*recordsNum !=  BLOCK_SIZE/sizeof(Record)) {
			printf("#eggrafwn pou ektipwthikan: %d\n", count);
			return;
		}

		overflowBucket = block;

		if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) BF_PrintError("\nerror_32");
		index = block;

		if(BF_ReadBlock(header_info->fileDesc, searchBlock, &block) < 0) BF_PrintError("\nerror_33");
		while(*overflowBucket != 0) {
			if(BF_ReadBlock(header_info->fileDesc, *overflowBucket, &block) < 0) BF_PrintError("\nerror_34");
			recordsNum = block;
			block += sizeof(int);
			for(i=0;i<*recordsNum;i++) {
				record_ptr = block;
				if(!strcmp(record_ptr->surname, record.surname)) {
					printRecord(*record_ptr);
					count++;
				}
				block += sizeof(Record);
			}
			overflowBucket = block;
		}

	} else if(!strcmp(fieldName, "status")) {	//statis -> status
		char* value_ptr = malloc(sizeof(header_info->attrLength));
		printf("attrLength = %d,\t(prepei = 1)\n", header_info->attrLength);
		memcpy(value_ptr, value, header_info->attrLength);
		printf("ta pedia status pou psaxnw: %c\n", value_ptr[0]);

		record.status[0] = value_ptr[0];
		bucketNum = hashFunction(record, header_info->depth, 4);
		if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) BF_PrintError("\nerror_4");
		index = block;
		searchBlock = index[bucketNum];

		if(BF_ReadBlock(header_info->fileDesc, searchBlock, &block) < 0) BF_PrintError("\nerror_41");
		recordsNum = block + sizeof(int);
		block += 2*sizeof(int);

		for(i=0;i<*recordsNum;i++) {		//gia kathe eggrafi tou arxeiou
			record_ptr = block;

			if(record_ptr->status[0] == record.status[0]) {
				printRecord(*record_ptr);
				count++;
			}

			block += sizeof(Record);
		}
		if(*recordsNum !=  BLOCK_SIZE/sizeof(Record)) {
			printf("#eggrafwn pou ektipwthikan: %d\n", count);
			return;
		}

		overflowBucket = block;

		if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) BF_PrintError("\nerror_42");
		index = block;

		if(BF_ReadBlock(header_info->fileDesc, searchBlock, &block) < 0) BF_PrintError("\nerror_43");
		while(*overflowBucket != 0) {
			if(BF_ReadBlock(header_info->fileDesc, *overflowBucket, &block) < 0) BF_PrintError("\nerror_44");
			recordsNum = block;
			block += sizeof(int);
			for(i=0;i<*recordsNum;i++) {
				record_ptr = block;
				if(record_ptr->status[0] == record.status[0]) {
					printRecord(*record_ptr);
					count++;
				}
				block += sizeof(Record);
			}
			overflowBucket = block;
		}

	} else if(!strcmp(fieldName, "dateOfBirth")) {
		char* value_ptr = malloc(sizeof(header_info->attrLength+1));
		printf("attrLength = %d\n", header_info->attrLength);
		memcpy(value_ptr, value, header_info->attrLength);
		value_ptr[header_info->attrLength] = 0;		//termatikos xaraktiras
		printf("ta pedia dateOfBirth pou psaxnw: %s\n", value_ptr);

		strcpy(record.surname, value_ptr);
		bucketNum = hashFunction(record, header_info->depth, 5);
		if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) BF_PrintError("\nerror_5");
		index = block;
		searchBlock = index[bucketNum];

		if(BF_ReadBlock(header_info->fileDesc, searchBlock, &block) < 0) BF_PrintError("\nerror_51");
		recordsNum = block + sizeof(int);
		block += 2*sizeof(int);

		for(i=0;i<*recordsNum;i++) {		//gia kathe eggrafi tou arxeiou
			record_ptr = block;

			if(!strcmp(record_ptr->dateOfBirth, record.dateOfBirth)) {
				printRecord(*record_ptr);
				count++;
			}

			block += sizeof(Record);
		}
		if(*recordsNum !=  BLOCK_SIZE/sizeof(Record)) {
			printf("#eggrafwn pou ektipwthikan: %d\n", count);
			return;
		}

		overflowBucket = block;

		if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) BF_PrintError("\nerror_32");
		index = block;

		if(BF_ReadBlock(header_info->fileDesc, searchBlock, &block) < 0) BF_PrintError("\nerror_53");
		while(*overflowBucket != 0) {
			if(BF_ReadBlock(header_info->fileDesc, *overflowBucket, &block) < 0) BF_PrintError("\nerror_54");
			recordsNum = block;
			block += sizeof(int);
			for(i=0;i<*recordsNum;i++) {
				record_ptr = block;
				if(!strcmp(record_ptr->dateOfBirth, record.dateOfBirth)) {
					printRecord(*record_ptr);
					count++;
				}
				block += sizeof(Record);
			}
			overflowBucket = block;
		}

	} else if(!strcmp(fieldName, "salary")) {
		int* value_ptr = malloc(sizeof(header_info->attrLength));
		printf("attrLength = %d,\t(prepei = 4)\n", header_info->attrLength);
		memcpy(value_ptr, value, header_info->attrLength);
		printf("ta pedia salary pou psaxnw: %d\n", *value_ptr);

		record.salary = *value_ptr;
		bucketNum = hashFunction(record, header_info->depth, 6);
		if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) BF_PrintError("\nerror_6");
		index = block;
		searchBlock = index[bucketNum];

		if(BF_ReadBlock(header_info->fileDesc, searchBlock, &block) < 0) BF_PrintError("\nerror_61");
		recordsNum = block + sizeof(int);
		block += 2*sizeof(int);

		for(i=0;i<*recordsNum;i++) {		//gia kathe eggrafi tou arxeiou
			record_ptr = block;

			if(record_ptr->salary == record.salary) {
				printRecord(*record_ptr);
				count++;
			}

			block += sizeof(Record);
		}
		if(*recordsNum !=  BLOCK_SIZE/sizeof(Record)) {
			printf("#eggrafwn pou ektipwthikan: %d\n", count);
			return;
		}

		overflowBucket = block;

		if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) BF_PrintError("\nerror_62");
		index = block;

		if(BF_ReadBlock(header_info->fileDesc, searchBlock, &block) < 0) BF_PrintError("\nerror_63");
		while(*overflowBucket != 0) {
			if(BF_ReadBlock(header_info->fileDesc, *overflowBucket, &block) < 0) BF_PrintError("\nerror_64");
			recordsNum = block;
			block += sizeof(int);
			for(i=0;i<*recordsNum;i++) {
				record_ptr = block;
				if(record_ptr->salary == record.salary) {
					printRecord(*record_ptr);
					count++;
				}
				block += sizeof(Record);
			}
			overflowBucket = block;
		}

	} else if(!strcmp(fieldName, "section")) {
		char* value_ptr = malloc(sizeof(header_info->attrLength));
		printf("attrLength = %d,\t(prepei = 1)\n", header_info->attrLength);
		memcpy(value_ptr, value, header_info->attrLength);
		printf("ta pedia section pou psaxnw: %c\n", value_ptr[0]);

		record.status[0] = value_ptr[0];
		bucketNum = hashFunction(record, header_info->depth, 7);
		if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) BF_PrintError("\nerror_7");
		index = block;
		searchBlock = index[bucketNum];

		if(BF_ReadBlock(header_info->fileDesc, searchBlock, &block) < 0) BF_PrintError("\nerror_71");
		recordsNum = block + sizeof(int);
		block += 2*sizeof(int);

		for(i=0;i<*recordsNum;i++) {		//gia kathe eggrafi tou arxeiou
			record_ptr = block;

			if(record_ptr->section[0] == record.section[0]) {
				printRecord(*record_ptr);
				count++;
			}

			block += sizeof(Record);
		}
		if(*recordsNum !=  BLOCK_SIZE/sizeof(Record)) {
			printf("#eggrafwn pou ektipwthikan: %d\n", count);
			return;
		}

		overflowBucket = block;

		if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) BF_PrintError("\nerror_72");
		index = block;

		if(BF_ReadBlock(header_info->fileDesc, searchBlock, &block) < 0) BF_PrintError("\nerror_73");
		while(*overflowBucket != 0) {
			if(BF_ReadBlock(header_info->fileDesc, *overflowBucket, &block) < 0) BF_PrintError("\nerror_74");
			recordsNum = block;
			block += sizeof(int);
			for(i=0;i<*recordsNum;i++) {
				record_ptr = block;
				if(record_ptr->section[0] == record.section[0]) {
					printRecord(*record_ptr);
					count++;
				}
				block += sizeof(Record);
			}
			overflowBucket = block;
		}

	} else if(!strcmp(fieldName, "daysOff")) {
		int* value_ptr = malloc(sizeof(header_info->attrLength));
		printf("attrLength = %d,\t(prepei = 4)\n", header_info->attrLength);
		memcpy(value_ptr, value, header_info->attrLength);
		printf("ta pedia daysOff pou psaxnw: %d\n", *value_ptr);

		record.daysOff = *value_ptr;
		bucketNum = hashFunction(record, header_info->depth, 8);
		if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) BF_PrintError("\nerror_8");
		index = block;
		searchBlock = index[bucketNum];

		if(BF_ReadBlock(header_info->fileDesc, searchBlock, &block) < 0) BF_PrintError("\nerror_81");
		recordsNum = block + sizeof(int);
		block += 2*sizeof(int);

		for(i=0;i<*recordsNum;i++) {		//gia kathe eggrafi tou arxeiou
			record_ptr = block;

			if(record_ptr->daysOff == record.daysOff) {
				printRecord(*record_ptr);
				count++;
			}

			block += sizeof(Record);
		}
		if(*recordsNum !=  BLOCK_SIZE/sizeof(Record)) {
			printf("#eggrafwn pou ektipwthikan: %d\n", count);
			return;
		}

		overflowBucket = block;

		if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) BF_PrintError("\nerror_82");
		index = block;

		if(BF_ReadBlock(header_info->fileDesc, searchBlock, &block) < 0) BF_PrintError("\nerror_83");
		while(*overflowBucket != 0) {
			if(BF_ReadBlock(header_info->fileDesc, *overflowBucket, &block) < 0) BF_PrintError("\nerror_84");
			recordsNum = block;
			block += sizeof(int);
			for(i=0;i<*recordsNum;i++) {
				record_ptr = block;
				if(record_ptr->daysOff == record.daysOff) {
					printRecord(*record_ptr);
					count++;
				}
				block += sizeof(Record);
			}
			overflowBucket = block;
		}

	} else if(!strcmp(fieldName, "prevYears")) {
		int* value_ptr = malloc(sizeof(header_info->attrLength));
		printf("attrLength = %d,\t(prepei = 4)\n", header_info->attrLength);
		memcpy(value_ptr, value, header_info->attrLength);
		printf("ta pedia prevYears pou psaxnw: %d\n", *value_ptr);

		record.prevYears = *value_ptr;
		bucketNum = hashFunction(record, header_info->depth, 6);
		if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) BF_PrintError("\nerror_8");
		index = block;
		searchBlock = index[bucketNum];

		if(BF_ReadBlock(header_info->fileDesc, searchBlock, &block) < 0) BF_PrintError("\nerror_91");
		recordsNum = block + sizeof(int);
		block += 2*sizeof(int);

		for(i=0;i<*recordsNum;i++) {		//gia kathe eggrafi tou arxeiou
			record_ptr = block;

			if(record_ptr->prevYears == record.prevYears) {
				printRecord(*record_ptr);
				count++;
			}

			block += sizeof(Record);
		}
		if(*recordsNum !=  BLOCK_SIZE/sizeof(Record)) {
			printf("#eggrafwn pou ektipwthikan: %d\n", count);
			return;
		}

		overflowBucket = block;

		if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) BF_PrintError("\nerror_92");
		index = block;

		if(BF_ReadBlock(header_info->fileDesc, searchBlock, &block) < 0) BF_PrintError("\nerror_93");
		while(*overflowBucket != 0) {
			if(BF_ReadBlock(header_info->fileDesc, *overflowBucket, &block) < 0) BF_PrintError("\nerror_94");
			recordsNum = block;
			block += sizeof(int);
			for(i=0;i<*recordsNum;i++) {
				record_ptr = block;
				if(record_ptr->prevYears == record.prevYears) {
					printRecord(*record_ptr);
					count++;
				}
				block += sizeof(Record);
			}
			overflowBucket = block;
		}

	} else printf("\nINPUT ERROR!\n");

	printf("#eggrafwn pou ektipwthikan: %d\n", count);
	return;

}


int main(void) {
	FILE *input = fopen("records","r");
	Record record;
	int temp = HT_CreateIndex("file1", 'c', "id", 4, 3);
	printf("create index: %d", temp);

	HT_info* header_info = malloc(sizeof(HT_info));
	header_info = HT_OpenIndex("file1");
	printf("\nopening file...");
	printHT_info(*header_info);

	int count = 0;
	while(1) {
		count++;
		fscanf(input, "%d %s %s %c %s %d %c %d %d", &(record.id),&(record.name),&(record.surname),&(record.status[0]),&(record.dateOfBirth),&(record.salary),&(record.section[0]),&(record.daysOff),&(record.prevYears));
		printRecord(record);
		temp = HT_InsertEntry(header_info, record);
		//printf("egine insert tou record -> %d\n", temp);
		//printSituation(header_info);
		//getchar();
		if(count == 300) break;
	}
	fclose(input);


	int aa = 274;
	char* name = "Marina";
	HT_GetAllEntries(header_info, NULL);

	printf("\n\n\n\n\n");
	HT_GetAllEntries(header_info, &aa);


	temp = HT_CloseIndex(header_info);
	printf("\nclosing file = %d\n", temp);
	//free(header_info);
}


int hashFunction(Record record, int depth, int field) {	//epistrefei to bucket eisagwgis tis eggrafis
	int i, bucketsNum = 1;
	int destBucket;
	void* key;
	int len;
	int t;
	for(i=0;i<depth;i++) bucketsNum = bucketsNum*2;		//bucketsNum = 2^depth

	if(field == 1) {
		key = &(record.id);
		len = sizeof(int);
	} else if(field == 2) {
		destBucket = charToInt(record.name) % bucketsNum;
		key = &(record.name);
		len = 15;
	} else if(field == 3) {
		destBucket = charToInt(record.surname) % bucketsNum;
		key = &(record.surname);
		len = 20;
	} else if(field == 4) {
		destBucket = record.status[0] % bucketsNum;
		key = &(record.status[0]);
		len = 1;
	} else if(field == 5) {
		destBucket = charToInt(record.dateOfBirth) % bucketsNum;
		key = &(record.dateOfBirth);
		len = 10;
	} else if(field == 6) {
		destBucket = record.salary % bucketsNum;
		key = &(record.salary);
		len = sizeof(int);
	} else if(field == 7) {
		destBucket = record.section[0] % bucketsNum;
		key = &(record.section[0]);
		len = 1;
	} else if(field == 8) {
		destBucket = record.daysOff % bucketsNum;
		key = &(record.daysOff);
		len = sizeof(int);
	} else if(field == 9) {
		destBucket = record.prevYears % bucketsNum;
		key = &(record.prevYears);
		len = sizeof(int);
	}
	t = (fnv_hash(key, len) % 127);
	//printf("timi katakermatismou = %d\n", t);	//einai 7 bit
	t = t >> (7-depth);
	return t;
}


long int charToInt(char* string) {
	int i;
	long int sum = 0;

	for(i=0;i<strlen(string);i++) {
		if(string[i]<100) sum = sum*100 + string[i];
		else sum = sum*1000 + string[i];
	}
	return sum;
}


unsigned fnv_hash ( void *key, int len ) {
	unsigned char *p = key;
	unsigned h = 2166136261;
	int i;

	for ( i = 0; i < len; i++ )
		h = ( h * 16777619 ) ^ p[i];

	return h;
}


void printSituation(HT_info* header_info) {
	int i, j, bucketsNum = 1, count = 1, mpika = 0;
	int blocksNum;
	void* block;
	int* index;
	int* recordsNum;
	int* localDepth;
	int* idNum;
	for(i=0;i<header_info->depth;i++) bucketsNum = bucketsNum*2;		//gia tous deiktes

	BF_ReadBlock(header_info->fileDesc, 1, &block);
	index = block;

	blocksNum = BF_GetBlockCounter(header_info->fileDesc)-2-OVERFLOW_BUCKETS_NUM;

	printf("buckets number: %d ,\tblocks number: %d\t", bucketsNum,blocksNum);
	printf("depth: %d ,\toverflow bucket = %d\n", header_info->depth,OVERFLOW_BUCKETS_NUM);

	printf("index (pinakas deiktwn\n-------------------------------\n");
	for(i=0;i<bucketsNum;i++) //printf("%d ", index[i]);
	printf("\n--------------------\n");

	printf("++++++++++++++++ START +++++++++++++++++\n");

	for(i=0;i<blocksNum;i++) {
		if(BF_ReadBlock(header_info->fileDesc, i+2, &block) < 0) BF_PrintError("o dias gamietai 1");

		localDepth = block;
		recordsNum = block + sizeof(int);

		if(BF_ReadBlock(header_info->fileDesc, 1, &block) < 0) BF_PrintError("o dias gamietai 12");
		index = block;

		for(j=0;j<bucketsNum;j++) {
			if(index[j] == i+2 ) printf(", %d", j);
		}

		printf("  ->");
		printf("\nblock %d:\t\t", i+2);


		if(BF_ReadBlock(header_info->fileDesc, i+2, &block) < 0) BF_PrintError("o dias gamietai 13");
		block += 2*sizeof(int);
		for(j=0;j<(*recordsNum);j++) {
			idNum = block;
			printf("%d, ", *idNum);
			block += sizeof(Record);
		}
		printf("\nlocal depth: %d", *localDepth);


		if(i == blocksNum-1) printf("\n++++++++++++++  END  +++++++++++++++++\n");
		else printf("\n-------------------------------------------------\n");
	}

	if(OVERFLOW_BUCKETS_NUM > 0) {
		printf("OVERFLOW BLOCKS (xwris seira sundesis):\n");
		for(i=0;i<OVERFLOW_BUCKETS_NUM;i++) {
			BF_ReadBlock(header_info->fileDesc, blocksNum+i+2, &block);
			recordsNum = block;

			printf("block uperxeilusis %d sto block %d:\n", i,blocksNum+i+2);

			block += sizeof(int);
			for(j=0;j<*recordsNum;j++) {
				idNum = block;
				printf("%d, ", *idNum);
				block += sizeof(Record);
			}
			if(i == OVERFLOW_BUCKETS_NUM-1) printf("\n++++++++++++++  END  +++++++++++++++++\n");
			else printf("\n-------------------------------------------------\n");
		}
	}
}




void printHT_info(HT_info info) {
	printf("\n------------------------------------------------------\n");
	printf("fileDesc = %d  //  ", info.fileDesc);
	printf("attrType = %c\n", info.attrType);
	printf("attrName = %s\n", info.attrName);
	printf("attrLength = %d  //  ", info.attrLength);
	printf("depth = %d\n", info.depth);
	printf("\n------------------------------------------------------\n");

}


void printRecord(Record record) {
	printf("\n------------------------------------------------------\n");
	printf("id = %d  //  ", record.id);
	printf("name = %s\n", record.name);
	printf("surname = %s\n", record.surname);
	printf("status : %c  //  ", record.status[0]);
	printf("date of birth : %s\n", record.dateOfBirth);
	printf("salary = %d  //  ", record.salary);
	printf("section : %c  //  ", record.section[0]);
	printf("days off = %d  //  ", record.daysOff);
	printf("prev years : %d", record.prevYears);
	printf("\n------------------------------------------------------\n");
}
