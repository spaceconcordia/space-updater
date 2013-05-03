#include "Updater.h"
int main(){
    const char* path_new      = "/home/jamg85/apps/new";
    const char* path_current  = "/home/jamg85/apps/current";
    const char* path_old      = "/home/jamg85/apps/old";
    const char* path_rollback = "/home/jamg85/apps/rollback";
    Updater* updater = new Updater(path_new, path_current, path_old, path_rollback);
    updater->StartUpdate();

    delete updater;

    return 0;
}
