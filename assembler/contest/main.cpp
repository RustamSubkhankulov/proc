#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

int main(void) {


	int n;
	int neg = 0;
	int pos = 0;
	int zero = 0;

	scanf("%d", &n);

	int* array = (int*)calloc(n, sizeof(int));

	for (int i = 0; i < n; i++) {

		scanf("%d", &array[i]);

		if (array[i] == 0) zero++;
		if (array[i] > 0) pos++;
		if (array[i] < 0) neg++;
	}

	int* neg_arr = (int*)calloc(neg, sizeof(int));
	int* pos_arr = (int*)calloc(pos, sizeof(int));
	int* zero_arr = (int*)calloc(zero, sizeof(int));

	int* neg_start = neg_arr;
	int* pos_start = pos_arr;
	int* zero_start = zero_arr;


	int zero_ct = 0;
	int pos_ct = 0;
	int neg_ct = 0;

	for (int i = 0; i < n; i++) {

		if (array[i] == 0) {
			zero_arr[zero_ct] = array[i];
			zero_ct++;
		}
		if (array[i] > 0) {
			pos_arr[pos_ct] = array[i];
			pos_ct++;
		}
		if (array[i] < 0) {
			neg_arr[neg_ct] = array[i];
			neg_ct++;
		}
	}
	free(array);

	for (int i = 1; i < pos; i++)

		for (int j = i; j > 0 && pos_arr[j - 1] > pos_arr[j]; j--) {

			int temp = pos_arr[j - 1];
			pos_arr[j - 1] = pos_arr[j];
			pos_arr[j] = temp;
		}

	for (int i = 1; i < neg; i++)

		for (int j = i; j > 0 && neg_arr[j - 1] < neg_arr[j]; j--) {

			int temp = neg_arr[j - 1];
			neg_arr[j - 1] = neg_arr[j];
			neg_arr[j] = temp;
		}

	for (int i = 0; i < pos; i++) {
		printf("%d ", pos_arr[i]);

	}
	for (int i = 0; i < neg; i++) {
		printf("%d ", neg_arr[i]);
	}

	free(neg_start);
	free(pos_start);
	free(zero_start);
}