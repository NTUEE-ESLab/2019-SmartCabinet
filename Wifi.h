#ifndef wifi_h
#define wifi_h


#include "mbed.h"
#include "http_request.h"
#include "network-helper.h"
#include "mbed_mem_trace.h"
#include <cstdlib>
#include <string>

void print_memory_info() {
    // allocate enough room for every thread's stack statistics
    int cnt = osThreadGetCount();
    mbed_stats_stack_t *stats = (mbed_stats_stack_t*) malloc(cnt * sizeof(mbed_stats_stack_t));
 
    cnt = mbed_stats_stack_get_each(stats, cnt);
    for (int i = 0; i < cnt; i++) {
        printf("Thread: 0x%lX, Stack size: %lu / %lu\r\n", stats[i].thread_id, stats[i].max_size, stats[i].reserved_size);
    }
    free(stats);
 
    // Grab the heap statistics
    mbed_stats_heap_t heap_stats;
    mbed_stats_heap_get(&heap_stats);
    printf("Heap size: %lu / %lu bytes\r\n", heap_stats.current_size, heap_stats.reserved_size);
}

int dump_response(HttpResponse* res) {
    //printf("Status: %d - %s\n", res->get_status_code(), res->get_status_message().c_str());

    //printf("Headers:\n");
    //for (size_t ix = 0; ix < res->get_headers_length(); ix++) {
    //     printf("\t%s: %s\n", res->get_headers_fields()[ix]->c_str(), res->get_headers_values()[ix]->c_str());
    //}
    //printf("\nBody (%d bytes):\n\n%s\n", res->get_body_length(), res->get_body_as_string().c_str());
    printf( " get : \n %s,\n",res->get_body_as_string().c_str() );
    if( strncmp (res->get_body_as_string().c_str(),"no such user",11) ==0 )
    	return 0;
    else
    	return (int(res->get_body_as_string().c_str()[0])-int('A')+1);
}

class wifihandler{
public:
	wifihandler(){};
	~wifihandler(){};

	int Get_request( const char* question ){

		WiFiInterface* network = connect_to_default_network_interface();

		std::string q_str(question);
		std::string addr = "http://140.112.18.177:7122/";

		//print_memory_info();
		if (!network) {
        	printf("Cannot connect to the network, see serial output\n");
        	return -2;
    	}
        printf( "req : %s\n",( addr+q_str ).c_str()  );
		HttpRequest* get_req = new HttpRequest(network, HTTP_GET,  ( addr+q_str ).c_str() );

		//printf("after get_req init\n");
		//print_memory_info();


		HttpResponse* get_res = get_req->send();
		if( !get_res ){
			printf("HttpRequest failed (error code %d)\n", get_req->get_error());
            return -1;
		}
		int result = dump_response( get_res );
		printf(" result =  %d \n", result );
		delete get_req;
		network ->disconnect();
		return result;
	};

private:
	WiFiInterface* network;

};

#endif