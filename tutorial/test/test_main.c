
#include "main.h"
#include "unity.h"
#include "mock_web_server.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void testMainShould_start_daemon() {
	start_daemon_Expect();
	main();
}
