#ifndef AP_FILTER_H
apr_status_t ap_pass_brigade(ap_filter_t *next, apr_bucket_brigade *bb){
    //...
    return AP_NOBODY_WROTE;
}
#endif
static apr_status_t upgrade_connection(request_rec *r)
{
    struct conn_rec *conn = r->connection;
    apr_bucket_brigade *bb;
    SSLConnRec *sslconn;
    apr_status_t rv;
    SSL *ssl;
	//...
    if (rv == APR_SUCCESS) {
        APR_BRIGADE_INSERT_TAIL(bb, apr_bucket_flush_create(conn->bucket_alloc));
        rv = ap_pass_brigade(conn->output_filters, bb);
    }
	//...
    return APR_SUCCESS;
}
