#include <Updater.h>
int main(){
    #undef __PATH__
    #define __PATH__ "/home/jamg85/space"
    const char* path_new      = __PATH__"/apps/new";
    const char* path_current  = __PATH__"/apps/current";
    const char* path_old      = __PATH__"/apps/old";
    const char* path_rollback = __PATH__"/apps/rollback";
    const char* log_folder    = __PATH__"/git/space-updater/logs";

    Updater* updater = new Updater(path_new, path_current, path_old, path_rollback, log_folder);
    
    bool isSuccess = updater->StartUpdate();
    
    if (updater != NULL){
        delete updater;
        updater = NULL;
    }

    if (isSuccess == true){
        return 0;
    }else{
        return 1;
    }
}
