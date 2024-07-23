# -*- coding: utf-8  -*-
# -*- author: jokker -*-

# -*- coding: utf-8  -*-
# -*- author: jokker -*-

import os
import shutil
import sys


def move_require_so_to_assign_dir(require_txt_path, so_dir):
    """将动态库移动到指定路径"""
    with open(require_txt_path, 'r') as txt_path:
        i = 1
        for each_line in txt_path:
            if i <= 1:
                i += 1
                continue

            each = each_line.strip()
            #
            if "=>" in each:
                each = each.split("=>")[1]

            each = each.split(" (")[0].strip()
            save_path = os.path.join(so_dir, os.path.split(each)[1])
            shutil.copy(each, save_path)

            print(each.strip(), "==>", save_path)
            print("-"*50)


def get_require_txt(app_path, require_txt_path):
    os.system("ldd {0} > {1}".format(app_path, require_txt_path))



if __name__ == "__main__":


    # if len(sys.argv) != 3:
    #     print("move_dll save_dir ucd_name")
    # else:


    # saveDir     = sys.argv[1]
    # saveName    = sys.argv[2]       # ucd_v2.5.1
    # saveDir     = os.path.join(saveDir, saveName)
    # # ---------------------------
    # appPath = r"../bin/ucd"
    # # ---------------------------

    appPath = r"/home/ldq/beijingfeihua/011_语音识别/语音唤醒/bin/awaken_offline_sample"
    saveDir = r"/home/docker/docker_feihua/v0.0.1/x64"
    saveName = "demo"


    saveTxtPath = os.path.join(saveDir, "require.txt")
    soDir = os.path.join(saveDir, "so_dir")
    os.makedirs(saveDir,    exist_ok=True)
    os.makedirs(soDir,      exist_ok=True)

    get_require_txt(appPath, saveTxtPath)
    move_require_so_to_assign_dir(saveTxtPath, soDir)
    #
    shutil.copy(appPath, os.path.join(saveDir, saveName))
    shutil.copy("./install_ucd.sh", os.path.join(saveDir, "install_" + saveName + ".sh"))
