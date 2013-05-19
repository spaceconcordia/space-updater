#include <Updater.h>
int main(){
//    const char* path_new      = "/home/apps/new";
  //  const char* path_current  = "/home/apps/current";
  //  const char* path_old      = "/home/apps/old";
  //  const char* path_rollback = "/home/apps/rollback";
  //  Updater* updater = new Updater(path_new, path_current, path_old, path_rollback);
    Updater* updater = new Updater();
    bool isSuccess = updater->StartUpdate();

    delete updater;

    if (isSuccess == true){
        return 0;
    }else{
        return 1;
    }
}
