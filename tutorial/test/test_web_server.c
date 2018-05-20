#include <microhttpd.h>
#include "unity.h"
#include "web_server.h"

const size_t no_data = 0;
const size_t some_valid_data = 122;
const char* valid_data = "{ 
\"cardType\": [
\"Visa\",
\"MasterCard\",
\"EFTPOS\"
],
\"TransactionType\": [
\"Cheque\",
\"Savings\",
\"Credit\"
] 
}";


void setUp(void)
{
}

void tearDown(void)
{
}

void test_web_server_Get_on_terminals_expect_get_terminals(void) {
	struct MHD_Connection connectionMock;
	void* connectionInfo;

	handleGets_Expect(); // could probably add Expects with Parameters to these two
	get_terminals_Expect();

	answer_to_connection(NULL, &connectionMock,
                      "terminals/", "GET",
                      "1.1", "",
                      &no_data, &connectionInfo);
}


void test_web_server_Get_on_terminals_id_expect_get_terminals_with_id(void) {
	struct MHD_Connection connectionMock;
	void* connectionInfo;

	handleGets_Expect(); // could probably add Expects with Parameters to these two
	get_terminals_Expect();

	answer_to_connection(NULL, &connectionMock,
                      "terminals/23", "GET",
                      "1.1", "",
                      &no_data, &connectionInfo);
}

void test_web_server_Post_on_terminals_expect_post_terminals(void) {
	struct MHD_Connection connectionMock;
	void* connectionInfo;

	handlePosts_Expect(); // could probably add Expects with Parameters to these two
	post_terminals_Expect();

	answer_to_connection(NULL, &connectionMock,
                      "terminals/", "POST",
                      "1.1", valid_data,
                      &some_valid_data, &connectionInfo);
}
