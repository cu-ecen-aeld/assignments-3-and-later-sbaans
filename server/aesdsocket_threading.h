#include <stdbool.h>
#include <pthread.h>

/**

 */
struct connection_thread_data{
    /*

     */

    /* Mutex */
    pthread_mutex_t *pmutex;

     /* Socket Descriptor */
    int accept_socket_descriptor;

    /**
     * Set to true if the thread completed with success, false
     * if an error occurred.
     */
    bool thread_complete_success;
};


/**

*/
bool start_connection_thread(pthread_t *thread, pthread_mutex_t *mutex, int accept_socket,int numthread);
