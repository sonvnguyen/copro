//ssl_engine_init.c
#ifdef HAVE_TLSEXT
static apr_status_t ssl_init_ctx_tls_extensions(
	//...
){
    apr_status_t rv;//Dead code if !HAVE_OCSP_STAPLING
	//...
#ifdef HAVE_OCSP_STAPLING
    if ((mctx->pkp == FALSE) && (mctx->stapling_enabled == TRUE)) {
        if ((rv = modssl_init_stapling(s, p, ptemp, mctx)) != APR_SUCCESS) {
            return rv;
        }
    }
#endif

#ifdef HAVE_SRP
    //...
#endif
    return APR_SUCCESS;
}
#endif