#!/bin/bash


# 需要使用 sudo 权限执行，否则无法修改 /etc/ld.so.conf.d 的权限

# 指定安装的目录
# --------------------------------------------------------------------------
package_dir="./"
ucd_name="ucd_v2.5.3"
# --------------------------------------------------------------------------

# 必须要有指定的参数
if [ $# == 0 ] ; then
    install_dir="/home/ldq/Apps_jokker"
    echo "- use default install_dir : ${install_dir}"
else
    install_dir=$1
fi

echo "- start install ucd"

# 检查安装文件夹是否存在
if [ ! -d $install_dir ]; then
    echo -e "\x1b[1;31merror : foler not exists : ${install_dir} \033[0m"
    exit 1
else
    echo "- install_dir OK"
fi

# 检查库文件夹是否存在
if [ ! -d "${package_dir}/so_dir" ]; then
    echo -e "\x1b[1;31merror : foler not exists : ${package_dir}/so_dir \033[0m"
    exit 1
else
    echo "- so_dir OK"
fi

# 检查启动文件是否存在
if [ ! -f "${package_dir}/${ucd_name}" ]; then
    echo -e "\x1b[1;31merror : foler not exists : ${package_dir}/${ucd_name} \033[0m"
    exit 1
else
    echo "- ucd OK"
fi

# 复制库文件夹和 ucd 文件
cp -r "${package_dir}/so_dir" "${install_dir}"
echo "- copy so_dir OK"

cp "${package_dir}/${ucd_name}" "${install_dir}"
echo "- copy ucd_version OK"

# 检查配置文件夹是否存在
if [ ! -d "/etc/ld.so.conf.d" ]; then
    echo -e "\x1b[1;31merror : foler not exists : /etc/ld.so.conf.d \033[0m"
    exit 1
else
    echo "- check /etc/ld.so.conf.d OK"
fi

# 检查配置文件是否存在
if [ ! -f "/etc/ld.so.conf.d/${ucd_name}.conf" ]; then
    echo "${install_dir}/so_dir" > "/etc/ld.so.conf.d/${ucd_name}.conf"
    echo "- create /etc/ld.so.conf.d/${ucd_name}.conf"
else
    rm "/etc/ld.so.conf.d/${ucd_name}.conf"
    echo "${install_dir}/so_dir" > "/etc/ld.so.conf.d/${ucd_name}.conf"
    echo "- /etc/ld.so.conf.d/${ucd_name}.conf update"
fi

if [ ! -f "/etc/ld.so.conf.d/${ucd_name}.conf" ]; then
    echo -e "- \x1b[1;31merror : create .conf failed \033[0m"
    exit -1
fi

echo "- install success !"

# 配置文件生效
echo "-------------------------------------------------"
ldconfig
echo "- ldconfig OK !"

# 提示文件
echo "-------------------------------------------------"
echo -e "\033[1;33muse alias by change file : ~/.bash_aliases\033[0m"
echo "-------------------------------------------------"


