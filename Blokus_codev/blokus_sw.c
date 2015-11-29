#include "co.h"
#include <stdio.h>

extern co_architecture co_initialize(int param);

void producer_func(co_stream output_stream) {

  co_stream_open(output_stream, O_WRONLY, CHAR_TYPE);
//  char *p = "0253aau0396l4374r3343t2337j03b3m435ap332bo03adk2315f43d5i3391g23c7e03dah0318b034ed13e2c0366a0";

  char *p = "a0253aau036bl23c7q3347o132br13dbt1394k4324p33c3s3311h0376i03den239dm1317e034cg036ef332ed0389a0";
  
  while((*p)!=0){
  	co_stream_write(output_stream, p, sizeof(char));
  	p++;
  }
  while(1);
}

void consumer_func(co_stream input_stream) {
	
//  char *p = "1CL55u078r08bt0ccq1b7j094m24co6d8k161l622p26ef3edg1bee248i33aa028c01ab000000000";

  char *p = "1CL55u069l198o6b5t527k4d8j074r091m742p4e4c0d1g215a00000000000000000000000000000";
  
  char c; int valid = 1;  int count = 0;

  co_stream_open(input_stream, O_RDONLY, CHAR_TYPE);
  
  co_stream_read(input_stream, &c, sizeof(char));
  printf("%c", c); ++p;
  
  co_stream_read(input_stream, &c, sizeof(char));
  printf("%c", c); ++p;
  
  co_stream_read(input_stream, &c, sizeof(char));
  printf("%c", c); ++p;
  
  printf("\n");
  
  while (co_stream_read(input_stream, &c, sizeof(char)) == co_err_none ) {
  	
    printf("%c", c);   	

	++count;	
	if(c != *(p++)) valid = 0;
	
	if(count == 4) { printf("\n"); count = 0; }
	
	if((*p) == 0) {
		printf("\nDone ");
		if(valid == 1) printf("PASS");
		else printf("FAIL");
		exit(0);
	}	
  }
  co_stream_close(input_stream);
}

int main(int argc, char *argv[]) {
    int param = 0;
    co_architecture my_arch;

    printf("starting...\n");

    my_arch = co_initialize(param);
    co_execute(my_arch);

    printf("complete.\n");
    return(0);
}
