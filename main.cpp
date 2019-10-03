#include <stdio.h>
#include <curl/curl.h>

//#define SKIP_PEER_VERIFICATION
//#define SKIP_HOSTNAME_VERIFICATION

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

int main(int argc, char *argv[])
{
    if(argc < 4) {
        printf("Syntax: curly [URL] [HEADER FILENAME] [BODY FILENAME]\n");
        return 0;
    }

    const char *url = argv[1];
    const char *headerFilename = argv[2];
    const char *bodyFilename = argv[3];
    FILE *bodyfile;
    FILE *headerfile;

    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);

        /* no progress meter please */ 
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);

        /* send all data to this function  */ 
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

        /* open the header file */ 
        headerfile = fopen(headerFilename, "wb");
        if(!headerfile) {
            curl_easy_cleanup(curl);
            return -1;
        }

        /* open the body file */ 
        bodyfile = fopen(bodyFilename, "wb");
        if(!bodyfile) {
            curl_easy_cleanup(curl);
            fclose(headerfile);
            return -1;
        }

        /* we want the headers be written to this file handle */ 
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, headerfile);

        /* we want the body be written to this file handle instead of stdout */ 
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, bodyfile);


#ifdef SKIP_PEER_VERIFICATION
        /*
         * If you want to connect to a site who isn't using a certificate that is
         * signed by one of the certs in the CA bundle you have, you can skip the
         * verification of the server's certificate. This makes the connection
         * A LOT LESS SECURE.
         *
         * If you have a CA cert for the server stored someplace else than in the
         * default bundle, then the CURLOPT_CAPATH option might come handy for
         * you.
         */ 
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif

#ifdef SKIP_HOSTNAME_VERIFICATION
        /*
         * If the site you're connecting to uses a different host name that what
         * they have mentioned in their server certificate's commonName (or
         * subjectAltName) fields, libcurl will refuse to connect. You can skip
         * this check, but this will make the connection less secure.
         */ 
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif

        /* Perform the request, res will get the return code */ 
        res = curl_easy_perform(curl);
        /* Check for errors */ 
        if(res == CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() worked!\n");
        } else {
            fprintf(stderr, "curl_easy_perform() failed [error %d]: %s\n", res, curl_easy_strerror(res));
        }

        /* close the header file */ 
        fclose(headerfile);

        /* close the body file */ 
        fclose(bodyfile);

        /* always cleanup */ 
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return 0;
}
