#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "file_system.h"
#include "terminal.h"
#include "system_call.h"
#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

/* divison error test */
int division_error() {
	TEST_HEADER;
	int a = 4;
	int b = 0;
	int c;
	c = a / b;
	int result = PASS;
	result = FAIL;
	return result;
}


int page_test() {
	TEST_HEADER;
	int result = PASS;
	int a;
	int* pointer = (int*) (0xB8000);
	a = *pointer;
	return result;
}

int page_fail_test() {
	TEST_HEADER;
	int result = PASS;
	int a;
	int* pointer = (int*) (0xB8000 - 1);
	a = *pointer;
	return result;
}


int page_null_error_test() {
	TEST_HEADER;
	int result = FAIL;
	int a;
	char* pointer = (char*) 0;
	a = *pointer;
	return result;
}

/* Checkpoint 2 tests */

int directory_read_test() {
	uint32_t fd;
	uint8_t buf[10000];
	uint32_t nbytes;
	const uint8_t* fname;
	clear();
	directory_open(fname);
	if (directory_read (0, buf, 0) == -1) {
		return FAIL;
	} else {
		return PASS;
	}
	directory_write (fd, buf, nbytes);
	directory_close (fd);
}

int file_read_test() {
	uint32_t fd;
	uint8_t buf[10000];
	uint32_t bytes_to_read = 10000;
	uint32_t bytes_read = 0;
	//const uint8_t* fname = "frame0.txt";
	//const uint8_t* fname = "verylargetextwithverylongname.txt";
	int i;
	clear();
	if (file_open ((const uint8_t*)"frame0.txt") == -1) {
		printf("failed to find file");
		printf("\n");
		return FAIL;
	} else {
		printf("success to find the file");
		printf("\n");
	}
	bytes_read = file_read(fd, buf, bytes_to_read);
	printf("bytes_read:%d", bytes_read);
    printf("\n");
    for (i = 0; i < bytes_read; i++) {
        if (buf[i] != NULL) {
            putc(buf[i]);
        }
    }
    printf("\n");	
	file_write (fd, buf, bytes_to_read);
	file_close (fd);
	return PASS;
}


int terminal_test() {
	char x[128];
    int val1;
	// int i = 0;
	val1 = read_terminal(0, x, 128);
	while(1) {
		// putc('\n');
		write_terminal(1, x, val1);
		// i++;
		// if (i = 100) {
		// 	break;
		// }
	}
	// write_terminal(1, x, 128);
	return PASS;
}


// int terminal_write_test(){
// 	int val1, val2;
// 	char* y1 = "sadhasjdhaskjhdjksahdjsakdhjsakdhjsakdhsjakdhsajkdhasjkdhsajdkashdjsakdhjaskdhjsakdhasjkdhasjkdhsajkghghjgjhghjghjghjghjghjghjgjhgjhghjgjhgjhgjhgjhghjghj";
// 	printf("Test input string length is 154:");
// 	val1 = write_terminal(1, y1, 128);
// 	printf("\n The number of input taken by the terminal is: %d \n", val1);
// 	char* y2 = "12364176371381232831721767631721726372312783";
// 	printf("Test input string length is 45:");
// 	val2 = write_terminal(1, y2, 128);
// 	printf("\n The number of input taken by the terminal is: %d \n", val2);
// 	return PASS;
// }

// int terminal_read_test() {
// 	char x[128];
//     int val1, val2;
//     char* y = "sadhasjdhaskjhdjksahdjsakdhjsakdhjsakdhsjakdhsajkdhasjkdhsajdkashdjsakdhjaskdhjsakdhasjkdhasjkdhsajkghghjgjhghjghjghjghjghjghjgjhgjhghjgjhgjhgjhgjhghjghjgjhjghgjhghjjhgjhdhsakjhkjh";
//     read_terminal(0 , x, 128);
//     printf("\n we successfully loadede the following: \n");
//     puts(x);
//     val1 = write_terminal(1, y, 128);
//     val2 = read_terminal(0 , x, 128);
//     printf("\n we successfully loadede the following: %d     %d \n",val1,val2);
//     puts(x);
// }

int RTC_write_test() {
	TEST_HEADER;
	int i = 4;
	RTC_write(1, &i, 4);
	for (i = 0; i < 40; i++) {
		RTC_read(NULL, NULL, NULL);
	}
	printf("\nread finish, change frequency to 64hz\n");
	i = 64;
	RTC_write(1, &i, 4);

	for (i = 0; i < (64*10); i++) {
		RTC_read(NULL, NULL, NULL);
	}
	printf("\nread finish, change frequency to 1024hz\n");
	i = 1024;
	RTC_write(1, &i, 4);
	return PASS;
}


/* Checkpoint 3 tests */

int test_execute() {
	if (execute((const uint8_t*)"hello") == -1) {
		printf("\nexecute failed\n");
		return FAIL;
	} else {
		printf("\nexecuted\n");
		return PASS;		
	}
}
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	/* Checkpoint 1 tests */
	//TEST_OUTPUT("idt_test", idt_test());
	// launch your tests here
	//TEST_OUTPUT("division_error_test", division_error());
	//TEST_OUTPUT("page_test", page_test());
	//TEST_OUTPUT("page_null_test", page_null_error_test());
	//TEST_OUTPUT("page_fail_test", page_fail_test());

	/* Checkpoint 2 tests */
	//TEST_OUTPUT("file_read_test", file_read_test());
	//TEST_OUTPUT("directory_read_test", directory_read_test());
	//TEST_OUTPUT("terminal_read_test", terminal_test());
	//TEST_OUTPUT("RTC_write_test", RTC_write_test());

	/* Checkpoint 3 tests */
	//TEST_OUTPUT("test_execute", test_execute());
}
