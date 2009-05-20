#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "common.h"
#include "../qrinput.h"
#include "../split.h"
#include "../qrspec.h"
#include "../rscode.h"

#define MAX_LENGTH 7091
static char data[MAX_LENGTH];
static char check[MAX_LENGTH];

static const char *AN = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";

#define drand(__scale__) ((__scale__) * (double)rand() / ((double)RAND_MAX + 1.0))

int fill8bitData(void)
{
	int len, i;

	len = 1 + (int)drand((MAX_LENGTH - 2));
	for(i=0; i<len; i++) {
		data[i] = (unsigned char)drand(255) + 1;
	}
	data[len] = '\0';

	return len;
}

void test_split_an(int num)
{
	QRinput *input;
	QRinput_List *list;
	int len, i, ret;

	len = 1 + (int)drand((MAX_LENGTH - 2));
	for(i=0; i<len; i++) {
		data[i] = AN[(int)drand(45)];
	}
	data[len] = '\0';

	input = QRinput_new2(0, QR_ECLEVEL_L);
	if(input == NULL) {
		perror("test_split_an aborted at QRinput_new2():");
		return;
	}
	ret = Split_splitStringToQRinput(data, input, QR_MODE_8, 1);
	if(ret < 0) {
		perror("test_split_an aborted at Split_splitStringToQRinput():");
		QRinput_free(input);
		return;
	}
	list = input->head;
	i = 0;
	while(list != NULL) {
		memcpy(check + i, list->data, list->size);
		i += list->size;
		list = list->next;
	}
	if(i != len) {
		printf("#%d: length is not correct. (%d should be %d)\n", num, i, len);
	}

	check[i] = '\0';
	ret = memcmp(data, check, len);
	if(ret != 0) {
		printf("#%d: data mismatched.\n", num);
		list = input->head;
		i = 0;
		while(list != NULL) {
			ret = memcmp(data + i, list->data, list->size);
			printf("wrong chunk:\n");
			printf(" position: %d\n", i);
			printf(" mode    : %d\n", list->mode);
			printf(" size    : %d\n", list->size);
			printf(" data    : %.*s\n", list->size, list->data);
			i += list->size;
			list = list->next;
		}
		exit(1);
	}
	QRinput_free(input);
}

void monkey_split_an(int loop)
{
	int i;

	puts("Monkey test: Split_splitStringToQRinput() - AlphaNumeric string.");
	srand(0);
	for(i=0; i<loop; i++) {
		test_split_an(i);
	}
}

void test_split_8(int num)
{
	QRinput *input;
	QRinput_List *list;
	int len, i, ret;

	len = fill8bitData();

	input = QRinput_new2(0, QR_ECLEVEL_L);
	if(input == NULL) {
		perror("test_split_8 aborted at QRinput_new2():");
		return;
	}
	ret = Split_splitStringToQRinput(data, input, QR_MODE_8, 1);
	if(ret < 0) {
		perror("test_split_8 aborted at Split_splitStringToQRinput():");
		QRinput_free(input);
		return;
	}
	list = input->head;
	i = 0;
	while(list != NULL) {
		memcpy(check + i, list->data, list->size);
		i += list->size;
		list = list->next;
	}
	if(i != len) {
		printf("#%d: length is not correct. (%d should be %d)\n", num, i, len);
	}

	check[i] = '\0';
	ret = memcmp(data, check, len);
	if(ret != 0) {
		printf("#%d: data mismatched.\n", num);
		list = input->head;
		i = 0;
		while(list != NULL) {
			ret = memcmp(data + i, list->data, list->size);
			printf("wrong chunk:\n");
			printf(" position: %d\n", i);
			printf(" mode    : %d\n", list->mode);
			printf(" size    : %d\n", list->size);
			printf(" data    : %.*s\n", list->size, list->data);
			i += list->size;
			list = list->next;
		}
		exit(1);
	}
	QRinput_free(input);
}

void monkey_split_8(int loop)
{
	int i;

	puts("Monkey test: Split_splitStringToQRinput() - 8bit char string.");
	srand(0);
	for(i=0; i<loop; i++) {
		test_split_8(i);
	}
}

void test_split_kanji(int num)
{
	QRinput *input;
	QRinput_List *list;
	int len, i, ret;

	len = fill8bitData();

	input = QRinput_new2(0, QR_ECLEVEL_L);
	if(input == NULL) {
		perror("test_split_kanji aborted at QRinput_new2():");
		return;
	}
	ret = Split_splitStringToQRinput(data, input, QR_MODE_KANJI, 1);
	if(ret < 0) {
		perror("test_split_kanji aborted at Split_splitStringToQRinput():");
		QRinput_free(input);
		return;
	}
	list = input->head;
	i = 0;
	while(list != NULL) {
		memcpy(check + i, list->data, list->size);
		i += list->size;
		list = list->next;
	}
	if(i != len) {
		printf("#%d: length is not correct. (%d should be %d)\n", num, i, len);
	}

	check[i] = '\0';
	ret = memcmp(data, check, len);
	if(ret != 0) {
		printf("#%d: data mismatched.\n", num);
		list = input->head;
		i = 0;
		while(list != NULL) {
			ret = memcmp(data + i, list->data, list->size);
			printf("wrong chunk:\n");
			printf(" position: %d\n", i);
			printf(" mode    : %d\n", list->mode);
			printf(" size    : %d\n", list->size);
			printf(" data    : %.*s\n", list->size, list->data);
			i += list->size;
			list = list->next;
		}
		exit(1);
	}
	QRinput_free(input);
}

void monkey_split_kanji(int loop)
{
	int i;

	puts("Monkey test: Split_splitStringToQRinput() - kanji string.");
	srand(0);
	for(i=0; i<loop; i++) {
		test_split_kanji(i);
	}
}

void test_split_structure(int num)
{
	QRinput *input;
	QRinput_Struct *s;
	QRcode_List *codes, *list;
	QRinput_InputList *il;
	int version;
	QRecLevel level;
	int len, c, i, ret;

	version = (int)drand(40) + 1;
	level = (QRecLevel)drand(4);

	len = fill8bitData();

	input = QRinput_new2(version, level);
	if(input == NULL) {
		perror("test_split_structure aborted at QRinput_new2():");
		return;
	}
	ret = Split_splitStringToQRinput(data, input, QR_MODE_KANJI, 1);
	if(ret < 0) {
		perror("test_split_structure aborted at Split_splitStringToQRinput():");
		QRinput_free(input);
		return;
	}
	s = QRinput_splitQRinputToStruct(input);
	if(s == NULL) {
		if(errno != 0 && errno != ERANGE) {
			perror("test_split_structure aborted at QRinput_splitQRinputToStruct():");
		}
		QRinput_free(input);
		return;
	}
	il = s->head;
	i = 0;
	while(il != NULL) {
		if(il->input->version != version) {
			printf("Test: version %d, level %c\n", version, levelChar[level]);
			printf("wrong version number.\n");
			printQRinputInfo(il->input);
			exit(1);
		}
		i++;
		il = il->next;
	}
	codes = QRcode_encodeInputStructured(s);
	if(codes == NULL) {
		perror("test_split_structure aborted at QRcode_encodeInputStructured():");
		QRinput_free(input);
		QRinput_Struct_free(s);
		return;
	}
	list = codes;
	il = s->head;
	c = 0;
	while(list != NULL) {
		if(list->code->version != version) {
			printf("#%d: data mismatched.\n", num);
			printf("Test: version %d, level %c\n", version, levelChar[level]);
			printf("code #%d\n", c);
			printf("Version mismatch: %d should be %d\n", list->code->version, version);
			printf("max bits: %d\n", QRspec_getDataLength(version, level) * 8 - 20);
			printQRinputInfo(il->input);
			printQRinput(input);
			exit(1);
		}
		list = list->next;
		il = il->next;
		c++;
	}

	QRinput_free(input);
	QRinput_Struct_free(s);
	QRcode_List_free(codes);
}

void monkey_split_structure(int loop)
{
	int i;

	puts("Monkey test: QRinput_splitQRinputToStruct.");
	srand(0);
	for(i=0; i<loop; i++) {
		test_split_structure(i);
	}
}

int main(int argc, char **argv)
{
	int loop = 1000;
	if(argc == 2) {
		loop = atoi(argv[1]);
	}
	monkey_split_an(loop);
	monkey_split_8(loop);
	monkey_split_kanji(loop);
	monkey_split_structure(loop);

	QRspec_clearCache();
	free_rs_cache();

	report();

	return 0;
}
