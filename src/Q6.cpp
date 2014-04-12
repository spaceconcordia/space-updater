#include "SpaceDecl.h"
#include "Updater.h"
int main(){
    const char* path_new      = CS1_APPS"/new";
    const char* path_current  = CS1_APPS"/current";
    const char* path_old      = CS1_APPS"/old";
    const char* path_rollback = CS1_APPS"/rollback";
    const char* path_logs     = CS1_LOGS
    
    Updater* updater = new Updater(path_new, path_current, path_old, path_rollback, path_logs);

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
