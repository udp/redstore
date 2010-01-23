#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "redstore.h"


http_response_t* handle_sparql_query(http_request_t *request, void* user_data)
{
    char *query_string = NULL;
    librdf_query* query = NULL;
    librdf_query_results* results = NULL;
    http_response_t* response = NULL;

    query_string = http_request_get_argument(request, "query");
    if (!query_string) {
        response = redstore_error_page(REDSTORE_DEBUG, HTTP_BAD_REQUEST, "query was missing query string");
        goto CLEANUP;
    }

    query = librdf_new_query(world, "sparql", NULL, (unsigned char *)query_string, NULL);
    if (!query) {
        response = redstore_error_page(REDSTORE_ERROR, HTTP_INTERNAL_SERVER_ERROR, "librdf_new_query failed");
        goto CLEANUP;
    }

    results = librdf_model_query_execute(model, query);
    if (!results) {
		response = redstore_error_page(REDSTORE_ERROR, HTTP_INTERNAL_SERVER_ERROR, "librdf_model_query_execute failed");
        goto CLEANUP;
    }
    
    query_count++;

    if (librdf_query_results_is_bindings(results)) {
        response = format_bindings_query_result(request, results);
    } else if (librdf_query_results_is_graph(results)) {
        librdf_stream *stream = librdf_query_results_as_stream(results);
        response = format_graph_stream(request, stream);
        librdf_free_stream(stream);
    } else if (librdf_query_results_is_boolean(results)) {
		response = redstore_error_page(REDSTORE_INFO, HTTP_NOT_IMPLEMENTED, "Boolean result format is not supported.");
    } else if (librdf_query_results_is_syntax(results)) {
		response = redstore_error_page(REDSTORE_INFO, HTTP_NOT_IMPLEMENTED, "Syntax results format is not supported.");
    } else {
		response = redstore_error_page(REDSTORE_ERROR, HTTP_INTERNAL_SERVER_ERROR, "Unknown results type.");
    }


CLEANUP:
    if (results) librdf_free_query_results(results);
    if (query) librdf_free_query(query);
    if (query_string) free(query_string);

    return response;
}
