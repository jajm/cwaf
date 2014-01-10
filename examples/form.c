#include <libobject/json.h>
#include "cwaf_router.h"
#include "cwaf_env.h"
#include "cwaf_params.h"
#include "cwaf.h"

array_t * print_form()
{
	array_t *response;
	string_t *body;
	char *form_elements =
		"  <div id=\"input_checkbox\">"
		"    <label for=\"input_checkbox\">checkbox</label>"
		"    <input type=\"checkbox\" name=\"input_checkbox\" id=\"input_checkbox\" />"
		"  </div>"
		"  <div id=\"input_color\">"
		"    <label for=\"input_color\">color</label>"
		"    <input type=\"color\" name=\"input_color\" id=\"input_color\" />"
		"  </div>"
		"  <div id=\"input_date\">"
		"    <label for=\"input_date\">date</label>"
		"    <input type=\"date\" name=\"input_date\" id=\"input_date\" />"
		"  </div>"
		"  <div id=\"input_datetime\">"
		"    <label for=\"input_datetime\">datetime</label>"
		"    <input type=\"datetime\" name=\"input_datetime\" id=\"input_datetime\" />"
		"  </div>"
		"  <div id=\"input_datetime-local\">"
		"    <label for=\"input_datetime-local\">datetime-local</label>"
		"    <input type=\"datetime-local\" name=\"input_datetime-local\" id=\"input_datetime-local\" />"
		"  </div>"
		"  <div id=\"input_email\">"
		"    <label for=\"input_email\">email</label>"
		"    <input type=\"email\" name=\"input_email\" id=\"input_email\" />"
		"  </div>"
		"  <div id=\"input_file\">"
		"    <label for=\"input_file\">file</label>"
		"    <input type=\"file\" name=\"input_file\" id=\"input_file\" />"
		"  </div>"
		"  <div id=\"input_file_multiple\">"
		"    <label for=\"input_file_multiple\">file (multiple)</label>"
		"    <input type=\"file\" name=\"input_file_multiple\" id=\"input_file_multiple\" multiple=\"multiple\"/>"
		"  </div>"
		"  <div id=\"input_hidden\">"
		"    <label for=\"input_hidden\">hidden</label>"
		"    <input type=\"hidden\" name=\"input_hidden\" id=\"input_hidden\" value=\"hidden_value\" />"
		"  </div>"
		"  <div id=\"input_month\">"
		"    <label for=\"input_month\">month</label>"
		"    <input type=\"month\" name=\"input_month\" id=\"input_month\" />"
		"  </div>"
		"  <div id=\"input_number\">"
		"    <label for=\"input_number\">number</label>"
		"    <input type=\"number\" name=\"input_number\" id=\"input_number\" />"
		"  </div>"
		"  <div id=\"input_password\">"
		"    <label for=\"input_password\">password</label>"
		"    <input type=\"password\" name=\"input_password\" id=\"input_password\" />"
		"  </div>"
		"  <div id=\"input_radio\">"
		"    <label>radio</label>"
		"    <input type=\"radio\" name=\"input_radio\" id=\"input_radio_1\" value=\"1\" />"
		"    <label for=\"input_radio_1\">1</label>"
		"    <input type=\"radio\" name=\"input_radio\" id=\"input_radio_2\" value=\"2\" />"
		"    <label for=\"input_radio_2\">2</label>"
		"  </div>"
		"  <div id=\"input_range\">"
		"    <label for=\"input_range\">range</label>"
		"    <input type=\"range\" name=\"input_range\" id=\"input_range\" />"
		"  </div>"
		"  <div id=\"input_search\">"
		"    <label for=\"input_search\">search</label>"
		"    <input type=\"search\" name=\"input_search\" id=\"input_search\" />"
		"  </div>"
		"  <div id=\"input_tel\">"
		"    <label for=\"input_tel\">tel</label>"
		"    <input type=\"tel\" name=\"input_tel\" id=\"input_tel\" />"
		"  </div>"
		"  <div id=\"input_text\">"
		"    <label for=\"input_text\">text</label>"
		"    <input type=\"text\" name=\"input_text\" id=\"input_text\" />"
		"  </div>"
		"  <div id=\"input_time\">"
		"    <label for=\"input_time\">time</label>"
		"    <input type=\"time\" name=\"input_time\" id=\"input_time\" />"
		"  </div>"
		"  <div id=\"input_url\">"
		"    <label for=\"input_url\">url</label>"
		"    <input type=\"url\" name=\"input_url\" id=\"input_url\" />"
		"  </div>"
		"  <div id=\"input_week\">"
		"    <label for=\"input_week\">week</label>"
		"    <input type=\"week\" name=\"input_week\" id=\"input_week\" />"
		"  </div>"
		"  <div id=\"select\">"
		"    <label for=\"select\">select</label>"
		"    <select name=\"select\" id=\"select\">"
		"      <option value=\"one\">one</option>"
		"      <option value=\"two\">two</option>"
		"      <option value=\"three\">three</option>"
		"    </select>"
		"  </div>"
		"  <div id=\"select_multiple\">"
		"    <label for=\"select_multiple\">select (multiple)</label>"
		"    <select name=\"select_multiple\" id=\"select_multiple\" multiple=\"multiple\">"
		"      <option value=\"one\">one</option>"
		"      <option value=\"two\">two</option>"
		"      <option value=\"three\">three</option>"
		"    </select>"
		"  </div>"
		"  <input type=\"submit\">";

	body = string("<html><body><h1>Example form</h1>"

		"<h2>application/x-www-form-urlencoded</h2>"
		"<form action=\"\" method=\"post\" enctype=\"application/x-www-form-urlencoded\">"
		, form_elements,
		"</form>"

		"<h2>multipart/form-data</h2>"
		"<form action=\"\" method=\"post\" enctype=\"multipart/form-data\">"
		, form_elements,
		"</form>"

		"<h2>text/plain</h2>"
		"<form action=\"\" method=\"post\" enctype=\"text/plain\">"
		, form_elements,
		"</form>"

		"</body></html>");

	response = array(
		string("200 OK"),
		array(string("Content-type"), string("text/html; charset=utf-8")),
		array(body)
	);

	return response;
}

array_t * submit_form()
{
	array_t *response;
	const hash_t *params;

	params = cwaf_params_get_all();
	char *params_json = object_to_json(params, OBJECT_TO_JSON_VERY_PRETTY);

	hash_t *env = cwaf_env_get();
	char *env_json = object_to_json(env, OBJECT_TO_JSON_VERY_PRETTY);

	response = array(
		string("200 OK"),
		array(string("Content-type"), string("text/html; charset=utf-8")),
		array(
			string("<pre>", params_json, "</pre>"),
			string("<pre>", env_json, "</pre>"),
		)
	);
	free(params_json);
	free(env_json);
	cwaf_env_finalize();

	return response;
}

int main()
{
	array_t *router;

	router = cwaf_router_new(
		"GET", "", &print_form,
		"POST", "", &submit_form,
		NULL, NULL, NULL
	);
	cwaf_init(router);
	object_free(router);

	return 0;
}

