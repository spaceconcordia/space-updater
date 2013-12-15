#include <Updater.h>
int main(){
    const char* path_new      = "/home/apps/new";
    const char* path_current  = "/home/apps/current";
    const char* path_old      = "/home/apps/old";
    const char* path_rollback = "/home/apps/rollback";
    const char* log_folder    = "/home/git/space-updater/logs";

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
